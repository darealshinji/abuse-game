#include "lisp.hpp"
#include "specs.hpp"
#include "bus_type.hpp"

long block_size(Cell *level)  // return size needed to recreate this block
{
  int ret;
  if (!level) ret=0;    // NULL pointers don't need to be stored
  else 
  {
    int type=item_type(level);
    if (type==L_CONS_CELL)
    {
	long t=0; 
	void *b=level;
	for (;b && item_type(b)==L_CONS_CELL;b=CDR(b)) 
	{
	  t+=sizeof(cons_cell);
	}
	if (b) t+=block_size(b);
	for (b=level;b && item_type(b)==L_CONS_CELL;b=CDR(b))
	  t+=block_size(CAR(b));
	ret=t;
    } else if (type== L_NUMBER)
    { ret=sizeof(lisp_number); } 
    else if (type==L_CHARACTER)
    { ret=sizeof(lisp_character); }
    else if (type==L_STRING)
    { 
      ret=sizeof(lisp_string)+strlen(lstring_value(level))+1; 
      if (ret<8)
        ret=8;
    }
    else if (type==L_POINTER)
    { ret=sizeof(lisp_pointer); }
    else ret=0;
  }
#ifdef WORD_ALLIGN
  return (ret+3)&(~3);
#else
  return ret;
#endif
}



void write_level(bFILE *fp, Cell *level)
{
  int type=item_type(level);
  fp->write_uint8(type);


  switch (type)
  {
    case L_NUMBER :
    { fp->write_uint32(lnumber_value(level)); } break;
    case L_CHARACTER :
    { fp->write_uint16(lcharacter_value(level)); } break;
    case L_STRING :
    { long l=strlen(lstring_value(level))+1;
      fp->write_uint32(l);
      fp->write(lstring_value(level),l); 
    } break;
    case L_SYMBOL :
    { fp->write_uint32((long)level); } break;
    case L_CONS_CELL :
    {
      if (!level) fp->write_uint32(0);
      else
      {
	long t=0;
	void *b=level;
	for (;b && item_type(b)==L_CONS_CELL;b=CDR(b)) t++;
	if (b) 
	{
	  fp->write_uint32(-t);      // negative number means dotted list
	  write_level(fp,b);       // save end of dotted list     
	}
	else fp->write_uint32(t);

	for (b=level;b && item_type(b)==L_CONS_CELL;b=CDR(b))    
	  write_level(fp,CAR(b));
      }
    } break;
  }
}

Cell *load_block(bFILE *fp)
{
  int type=fp->read_uint8();
  switch (type)
  {   
    case L_NUMBER :
    { return new_lisp_number(fp->read_uint32()); } break;
    case L_CHARACTER :
    { return new_lisp_character(fp->read_uint16()); } break;
    case L_STRING :
    { long l=fp->read_uint32();
      lisp_string *s=new_lisp_string(l);
      fp->read(lstring_value(s),l);
      return s;
    } break;
    case L_SYMBOL :
    { return (void *)fp->read_uint32(); } break;
    case L_CONS_CELL :
    {
      long t=fp->read_uint32();
      if (!t) return NULL;
      else
      {
	long x=abs(t);
	cons_cell *last=NULL,*first=NULL;
	while (x)
	{
	  cons_cell *c=new_cons_cell();
	  if (first)
	    last->cdr=c;
	  else first=c;
	  last=c;
	  x--;
	}
	if (t<0)	
	  last->cdr=load_block(fp);
	else last->cdr=NULL;
	
	for (last=first,x=0;x<abs(t);x++,last=(cons_cell *)last->cdr)       
	  last->car=load_block(fp);	
	return first;
      }
    }
  }
  return NULL;
}
