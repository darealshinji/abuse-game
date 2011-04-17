/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define TYPE_CHECKING 1
#include "bus_type.h"

#include "lisp.h"
#include "lisp_gc.h"
#include "symbols.h"

#ifdef NO_LIBS
#   include "fakelib.h"
#else
#   include "status.h"
#   include "macs.h"
#   include "specs.h"
#   include "dprint.h"
#   include "cache.h"
#   include "dev.h"
#endif

/* To bypass the whole garbage collection issue of lisp I am going to have
 * separate spaces where lisp objects can reside.  Compiled code and gloabal
 * variables will reside in permanant space.  Eveything else will reside in
 * tmp space which gets thrown away after completion of eval.  system
 * functions reside in permant space. */

bFILE *current_print_file=NULL;
lisp_symbol *lsym_root=NULL;
long ltotal_syms=0;



uint8_t *space[4], *free_space[4];
int space_size[4], print_level=0, trace_level=0, trace_print_level=1000;
int total_user_functions;

int current_space;  // normally set to TMP_SPACE, unless compiling or other needs

int break_level=0;

void l1print(void *block)
{
    if(!block || item_type(block) != L_CONS_CELL)
    {
        lprint(block);
        return;
    }

    dprintf("(");
    for( ; block && item_type(block) == L_CONS_CELL; block = CDR(block))
    {
        void *a = CAR(block);
        if(item_type(a) == L_CONS_CELL)
            dprintf("[...]");
        else
            lprint(a);
    }
    if (block)
    {
        dprintf(" . ");
        lprint(block);
    }
    dprintf(")");
}

void where_print(int max_lev = -1)
{
    dprintf("Main program\n");
    if (max_lev==-1) max_lev=l_ptr_stack.son;
    else if (max_lev>=l_ptr_stack.son) max_lev=l_ptr_stack.son-1;

    for (int i=0;i<max_lev;i++)
    {
        dprintf("%d> ", i);
        lprint(*l_ptr_stack.sdata[i]);
    }
}

void print_trace_stack(int max_levels)
{
    where_print(max_levels);
}

void lbreak(char const *format, ...)
{
  break_level++;
  bFILE *old_file=current_print_file;
  current_print_file=NULL;
  char st[300];
  va_list ap;
  va_start(ap, format);
  vsprintf(st, format, ap);
  va_end(ap);
  dprintf("%s\n", st);
  int cont=0;
  do
  {
    dprintf("type q to quit\n");
    dprintf("%d. Break> ", break_level);
    dgets(st, 300);
    if (!strcmp(st, "c") || !strcmp(st, "cont") || !strcmp(st, "continue"))
      cont=1;
    else if (!strcmp(st, "w") || !strcmp(st, "where"))
      where_print();
    else if (!strcmp(st, "q") || !strcmp(st, "quit"))
      exit(1);
    else if (!strcmp(st, "e") || !strcmp(st, "env") || !strcmp(st, "environment"))
    {
      dprintf("Enviorment : \nnot supported right now\n");

    }
    else if (!strcmp(st, "h") || !strcmp(st, "help") || !strcmp(st, "?"))
    {
      dprintf("CLIVE Debugger\n");
      dprintf(" w, where : show calling parents\n"
          " e, env   : show enviroment\n"
          " c, cont  : continue if possible\n"
          " q, quit  : quits the program\n"
          " h, help  : this\n");
    }
    else
    {
      char const *s=st;
      do
      {
                void *prog=compile(s);
                p_ref r1(prog);
                while (*s==' ' || *s=='\t' || *s=='\r' || *s=='\n') s++;
                lprint(eval(prog));
      } while (*s);
    }

  } while (!cont);
  current_print_file=old_file;
  break_level--;
}

void need_perm_space(char const *why)
{
  if (current_space!=PERM_SPACE && current_space!=GC_SPACE)
  {
    lbreak("%s : action requires permanant space\n", why);
    exit(0);
  }
}

void *mark_heap(int heap)
{
  return free_space[heap];
}

void restore_heap(void *val, int heap)
{
  free_space[heap] = (uint8_t *)val;
}

static int get_free_size(int which_space)
{
    return space_size[which_space]
            - (free_space[which_space] - space[which_space]);
}

void *lmalloc(int size, int which_space)
{
  return malloc(size);

#ifdef WORD_ALIGN
  size=(size+3)&(~3);
#endif

  if (size > get_free_size(which_space))
  {
    int fart = 1;
    fprintf(stderr, "%i > %i !!!\n", size, get_free_size(which_space));

    if (which_space == PERM_SPACE || which_space == TMP_SPACE)
    {
      collect_space(which_space);
      if (size <= get_free_size(which_space))
        fart = 0;
    }

    if (fart)
    {
      lbreak("lisp: cannot malloc %d bytes in space #%d\n", size, which_space);
      exit(0);
    }
    fprintf(stderr, "%i <= %i\n", size, get_free_size(which_space));
  }
  void *ret = (void *)free_space[which_space];
  free_space[which_space] += size;
  return ret;
}

void *eval_block(void *list)
{
  p_ref r1(list);
  void *ret=NULL;
  while (list)
  {
    ret=eval(CAR(list));
    list=CDR(list);
  }
  return ret;
}

lisp_1d_array *new_lisp_1d_array(int size, void *rest)
{
  p_ref r11(rest);
  size_t s=sizeof(lisp_1d_array)+size*sizeof(void *);
  if (s<8 + sizeof(intptr_t)) s=8 + sizeof(intptr_t);
  void *p=(lisp_1d_array *)lmalloc(s, current_space);
  ((lisp_1d_array *)p)->type=L_1D_ARRAY;
  ((lisp_1d_array *)p)->size=size;
  void **data=(void **)(((lisp_1d_array *)p)+1);
  memset(data, 0, size*sizeof(void *));
  p_ref r1(p);

  if (rest)
  {
    void *x=eval(CAR(rest));
    if (x==colon_initial_contents)
    {
      x=eval(CAR(CDR(rest)));
      data=(void **)(((lisp_1d_array *)p)+1);
      for (int i=0;i<size;i++, x=CDR(x))
      {
    if (!x)
    {
      lprint(rest);
      lbreak("(make-array) incorrect list length\n");
      exit(0);
    }
    data[i]=CAR(x);
      }
      if (x) { lprint(rest); lbreak("(make-array) incorrect list length\n"); exit(0); }
    }
    else if (x==colon_initial_element)
    {
      x=eval(CAR(CDR(rest)));
      data=(void **)(((lisp_1d_array *)p)+1);
      for (int i=0;i<size;i++)
        data[i]=x;
    }
    else
    {
      lprint(x);
      lbreak("Bad option argument to make-array\n");
      exit(0);
    }
  }

  return ((lisp_1d_array *)p);
}

lisp_fixed_point *new_lisp_fixed_point(int32_t x)
{
  lisp_fixed_point *p=(lisp_fixed_point *)lmalloc(sizeof(lisp_fixed_point), current_space);
  p->type=L_FIXED_POINT;
  p->x=x;
  return p;
}


lisp_object_var *new_lisp_object_var(int16_t number)
{
  lisp_object_var *p=(lisp_object_var *)lmalloc(sizeof(lisp_object_var), current_space);
  p->type=L_OBJECT_VAR;
  p->number=number;
  return p;
}


struct lisp_pointer *new_lisp_pointer(void *addr)
{
  if (addr==NULL) return NULL;
  lisp_pointer *p=(lisp_pointer *)lmalloc(sizeof(lisp_pointer), current_space);
  p->type=L_POINTER;
  p->addr=addr;
  return p;
}

struct lisp_character *new_lisp_character(uint16_t ch)
{
  lisp_character *c=(lisp_character *)lmalloc(sizeof(lisp_character), current_space);
  c->type=L_CHARACTER;
  c->ch=ch;
  return c;
}

struct lisp_string *new_lisp_string(char const *string)
{
  size_t size=sizeof(lisp_string)+strlen(string)+1;
  if (size<8 + sizeof(intptr_t)) size=8 + sizeof(intptr_t);

  lisp_string *s=(lisp_string *)lmalloc(size, current_space);
  s->type=L_STRING;
  char *sloc=((char *)s)+sizeof(lisp_string);
  strcpy(sloc, string);
  return s;
}

struct lisp_string *new_lisp_string(char const *string, int length)
{
  size_t size=sizeof(lisp_string)+length+1;
  if (size<8 + sizeof(intptr_t)) size=8 + sizeof(intptr_t);
  lisp_string *s=(lisp_string *)lmalloc(size, current_space);
  s->type=L_STRING;
  char *sloc=((char *)s)+sizeof(lisp_string);
  memcpy(sloc, string, length);
  sloc[length]=0;
  return s;
}

struct lisp_string *new_lisp_string(int length)
{
  size_t size=sizeof(lisp_string)+length;
  if (size<8 + sizeof(intptr_t)) size=8 + sizeof(intptr_t);
  lisp_string *s=(lisp_string *)lmalloc(size, current_space);
  s->type=L_STRING;
  char *sloc=((char *)s)+sizeof(lisp_string);
  strcpy(sloc, "");
  return s;
}

#ifdef NO_LIBS
lisp_user_function *new_lisp_user_function(void *arg_list, void *block_list)
{
  p_ref r1(arg_list), r2(block_list);
  lisp_user_function *lu=(lisp_user_function *)lmalloc(sizeof(lisp_user_function), current_space);
  lu->type=L_USER_FUNCTION;
  lu->arg_list=arg_list;
  lu->block_list=block_list;
  return lu;
}
#else
lisp_user_function *new_lisp_user_function(intptr_t arg_list, intptr_t block_list)
{
  int sp=current_space;
  if (current_space!=GC_SPACE)
    current_space=PERM_SPACE;       // make sure all functions get defined in permanant space

  lisp_user_function *lu=(lisp_user_function *)lmalloc(sizeof(lisp_user_function), current_space);
  lu->type=L_USER_FUNCTION;
  lu->alist=arg_list;
  lu->blist=block_list;

  current_space=sp;

  return lu;
}
#endif


lisp_sys_function *new_lisp_sys_function(int min_args, int max_args, int fun_number)
{
  // sys functions should reside in permanant space
  lisp_sys_function *ls=(lisp_sys_function *)lmalloc(sizeof(lisp_sys_function),
                             current_space==GC_SPACE ? GC_SPACE : PERM_SPACE);
  ls->type=L_SYS_FUNCTION;
  ls->min_args=min_args;
  ls->max_args=max_args;
  ls->fun_number=fun_number;
  return ls;
}

lisp_sys_function *new_lisp_c_function(int min_args, int max_args, int fun_number)
{
  // sys functions should reside in permanant space
  lisp_sys_function *ls=(lisp_sys_function *)lmalloc(sizeof(lisp_sys_function),
                             current_space==GC_SPACE ? GC_SPACE : PERM_SPACE);
  ls->type=L_C_FUNCTION;
  ls->min_args=min_args;
  ls->max_args=max_args;
  ls->fun_number=fun_number;
  return ls;
}

lisp_sys_function *new_lisp_c_bool(int min_args, int max_args, int fun_number)
{
  // sys functions should reside in permanant space
  lisp_sys_function *ls=(lisp_sys_function *)lmalloc(sizeof(lisp_sys_function),
                             current_space==GC_SPACE ? GC_SPACE : PERM_SPACE);
  ls->type=L_C_BOOL;
  ls->min_args=min_args;
  ls->max_args=max_args;
  ls->fun_number=fun_number;
  return ls;
}

lisp_sys_function *new_user_lisp_function(int min_args, int max_args, int fun_number)
{
  // sys functions should reside in permanant space
  lisp_sys_function *ls=(lisp_sys_function *)lmalloc(sizeof(lisp_sys_function),
                             current_space==GC_SPACE ? GC_SPACE : PERM_SPACE);
  ls->type=L_L_FUNCTION;
  ls->min_args=min_args;
  ls->max_args=max_args;
  ls->fun_number=fun_number;
  return ls;
}

lisp_number *new_lisp_node(long num)
{
  lisp_number *n=(lisp_number *)lmalloc(sizeof(lisp_number), current_space);
  n->type=L_NUMBER;
  n->num=num;
  return n;
}

lisp_symbol *new_lisp_symbol(char *name)
{
  lisp_symbol *s=(lisp_symbol *)lmalloc(sizeof(lisp_symbol), current_space);
  s->type=L_SYMBOL;
  s->name=new_lisp_string(name);
  s->value=l_undefined;
  s->function=l_undefined;
#ifdef L_PROFILE
  s->time_taken=0;
#endif
  return s;
}

lisp_number *new_lisp_number(long num)
{
  lisp_number *s=(lisp_number *)lmalloc(sizeof(lisp_number), current_space);
  s->type=L_NUMBER;
  s->num=num;
  return s;
}


cons_cell *new_cons_cell()
{
  cons_cell *c=(cons_cell *)lmalloc(sizeof(cons_cell), current_space);
  c->type=L_CONS_CELL;
  c->car=NULL;
  c->cdr=NULL;
  return c;
}


char *lerror(char const *loc, char const *cause)
{
  int lines;
  if (loc)
  {
    for (lines=0;*loc && lines<10;loc++)
    {
      if (*loc=='\n') lines++;
      dprintf("%c", *loc);
    }
    dprintf("\nPROGRAM LOCATION : \n");
  }
  if (cause)
    dprintf("ERROR MESSAGE : %s\n", cause);
  lbreak("");
  exit(0);
  return NULL;
}

void *nth(int num, void *list)
{
  if (num<0)
  {
    lbreak("NTH: %d is not a nonnegative fixnum and therefore not a valid index\n", num);
    exit(1);
  }

  while (list && num)
  {
    list=CDR(list);
    num--;
  }
  if (!list) return NULL;
  else return CAR(list);
}

void *lpointer_value(void *lpointer)
{
  if (!lpointer) return NULL;
#ifdef TYPE_CHECKING
  else if (item_type(lpointer)!=L_POINTER)
  {
    lprint(lpointer);
    lbreak(" is not a pointer\n");
    exit(0);
  }
#endif
  return ((lisp_pointer *)lpointer)->addr;
}

int32_t lnumber_value(void *lnumber)
{
  switch (item_type(lnumber))
  {
    case L_NUMBER :
      return ((lisp_number *)lnumber)->num;
    case L_FIXED_POINT :
      return (((lisp_fixed_point *)lnumber)->x)>>16;
    case L_STRING :
      return (uint8_t)*lstring_value(lnumber);
    case L_CHARACTER :
      return lcharacter_value(lnumber);
    default :
    {
      lprint(lnumber);
      lbreak(" is not a number\n");
      exit(0);
    }
  }
  return 0;
}

char *lstring_value(void *lstring)
{
#ifdef TYPE_CHECKING
  if (item_type(lstring)!=(ltype)L_STRING)
  {
    lprint(lstring);
    lbreak(" is not a string\n");
    exit(0);
  }
#endif
  return ((char *)lstring)+sizeof(lisp_string);
}



void *lisp_atom(void *i)
{
  if (item_type(i)==(ltype)L_CONS_CELL)
    return NULL;
  else return true_symbol;
}

void *lcdr(void *c)
{
  if (!c) return NULL;
  else if (item_type(c)==(ltype)L_CONS_CELL)
    return ((cons_cell *)c)->cdr;
  else
    return NULL;
}

void *lcar(void *c)
{
  if (!c) return NULL;
  else if (item_type(c)==(ltype)L_CONS_CELL)
    return ((cons_cell *)c)->car;
  else return NULL;
}

uint16_t lcharacter_value(void *c)
{
#ifdef TYPE_CHECKING
  if (item_type(c)!=L_CHARACTER)
  {
    lprint(c);
    lbreak("is not a character\n");
    exit(0);
  }
#endif
  return ((lisp_character *)c)->ch;
}

long lfixed_point_value(void *c)
{
  switch (item_type(c))
  {
    case L_NUMBER :
      return ((lisp_number *)c)->num<<16; break;
    case L_FIXED_POINT :
      return (((lisp_fixed_point *)c)->x); break;
    default :
    {
      lprint(c);
      lbreak(" is not a number\n");
      exit(0);
    }
  }
  return 0;
}

void *lisp_eq(void *n1, void *n2)
{
  if (!n1 && !n2) return true_symbol;    
  else if ((n1 && !n2) || (n2 && !n1)) return NULL;
  {
    int t1=*((ltype *)n1), t2=*((ltype *)n2);
    if (t1!=t2) return NULL;
    else if (t1==L_NUMBER)
    { if (((lisp_number *)n1)->num==((lisp_number *)n2)->num)
        return true_symbol;
      else return NULL;
    } else if (t1==L_CHARACTER)
    {
      if (((lisp_character *)n1)->ch==((lisp_character *)n2)->ch)
        return true_symbol;
      else return NULL;
    }
    else if (n1==n2)
      return true_symbol;
    else if (t1==L_POINTER)
      if (n1==n2) return true_symbol;
  }
  return NULL;
}

void *lget_array_element(void *a, long x)
{
#ifdef TYPE_CHECKING
  if (item_type(a)!=L_1D_ARRAY)
  {
    lprint(a);
    lbreak("is not an array\n");
    exit(0);
  }
#endif
  if (x>=((lisp_1d_array *)a)->size || x<0)
  {
    lbreak("array refrence out of bounds (%d)\n", x);
    exit(0);
  }
  return ((void **)(((lisp_1d_array *)a)+1))[x];
}

void *lisp_equal(void *n1, void *n2)
{
    if(!n1 && !n2) // if both nil, then equal
        return true_symbol;

    if(!n1 || !n2) // one nil, nope
        return NULL;

    int t1 = item_type(n1), t2 = item_type(n2);
    if(t1 != t2)
        return NULL;

    switch (t1)
    {
    case L_STRING :
        if (!strcmp(lstring_value(n1), lstring_value(n2)))
            return true_symbol;
        return NULL;
    case L_CONS_CELL :
        while (n1 && n2) // loop through the list and compare each element
        {
          if (!lisp_equal(CAR(n1), CAR(n2)))
            return NULL;
          n1=CDR(n1);
          n2=CDR(n2);
          if (n1 && *((ltype *)n1)!=L_CONS_CELL)
            return lisp_equal(n1, n2);
        }
        if (n1 || n2)
            return NULL;   // if one is longer than the other
        return true_symbol;
    default :
        return lisp_eq(n1, n2);
    }
}

int32_t lisp_cos(int32_t x)
{
  x=(x+FIXED_TRIG_SIZE/4)%FIXED_TRIG_SIZE;
  if (x<0) return sin_table[FIXED_TRIG_SIZE+x];
  else return sin_table[x];
}

int32_t lisp_sin(int32_t x)
{
  x=x%FIXED_TRIG_SIZE;
  if (x<0) return sin_table[FIXED_TRIG_SIZE+x];
  else return sin_table[x];
}

int32_t lisp_atan2(int32_t dy, int32_t dx)
{
  if (dy==0)
  {
    if (dx>0) return 0;
    else return 180;
  } else if (dx==0)
  {
    if (dy>0) return 90;
    else return 270;
  } else
  {
    if (dx>0)
    {
      if (dy>0)
      {
    if (abs(dx)>abs(dy))
    {
      int32_t a=dx*29/dy;
      if (a>=TBS) return 0;
      else return 45-atan_table[a];
    }
    else
    {
      int32_t a=dy*29/dx;
      if (a>=TBS) return 90;
      else return 45+atan_table[a];
    }
      } else
      {
    if (abs(dx)>abs(dy))
    {
      int32_t a=dx*29/abs(dy);
      if (a>=TBS)
        return 0;
      else
        return 315+atan_table[a];
    }
    else
    {
      int32_t a=abs(dy)*29/dx;
      if (a>=TBS)
        return 260;
      else
        return 315-atan_table[a];
    }
      }
    } else
    {
      if (dy>0)
      {
    if (abs(dx)>abs(dy))
    {
      int32_t a=-dx*29/dy;
      if (a>=TBS)
        return 135+45;
      else
        return 135+atan_table[a];
    }
    else
    {
      int32_t a=dy*29/-dx;
      if (a>=TBS)
        return 135-45;
      else
        return 135-atan_table[a];
    }
      } else
      {
    if (abs(dx)>abs(dy))
    {
      int32_t a=-dx*29/abs(dy);
      if (a>=TBS)
        return 225-45;
      else return 225-atan_table[a];
    }
    else
    {
      int32_t a=abs(dy)*29/abs(dx);
      if (a>=TBS)
        return 225+45;    
      else return 225+atan_table[a];
    }
      }
    }
  }
}


/*
lisp_symbol *find_symbol(char const *name)
{
  cons_cell *cs;
  for (cs=(cons_cell *)symbol_list;cs;cs=(cons_cell *)CDR(cs))
  {
    if (!strcmp( ((char *)((lisp_symbol *)cs->car)->name)+sizeof(lisp_string), name))
      return (lisp_symbol *)(cs->car);
  }
  return NULL;
}


lisp_symbol *make_find_symbol(char const *name)    // find a symbol, if it doesn't exsist it is created
{
  lisp_symbol *s=find_symbol(name);
  if (s) return s;
  else
  {
    int sp=current_space;
    if (current_space!=GC_SPACE)
      current_space=PERM_SPACE;       // make sure all symbols get defined in permanant space
    cons_cell *cs;
    cs=new_cons_cell();
    s=new_lisp_symbol(name);
    cs->car=s;
    cs->cdr=symbol_list;
    symbol_list=cs;
    current_space=sp;
  }
  return s;
}

*/

lisp_symbol *find_symbol(char const *name)
{
  lisp_symbol *p=lsym_root;
  while (p)
  {
    int cmp=strcmp(name, ((char *)p->name)+sizeof(lisp_string));
    if (cmp==0) return p;
    else if (cmp<0) p=p->left;
    else p=p->right;
  }
  return NULL;
}



lisp_symbol *make_find_symbol(char const *name)
{
  lisp_symbol *p=lsym_root;
  lisp_symbol **parent=&lsym_root;
  while (p)
  {
    int cmp=strcmp(name, ((char *)p->name)+sizeof(lisp_string));
    if (cmp==0) return p;
    else if (cmp<0)
    {
      parent=&p->left;
      p=p->left;
    }
    else
    {
      parent=&p->right;
      p=p->right;
    }
  }
  int sp=current_space;
  if (current_space!=GC_SPACE)
     current_space=PERM_SPACE;       // make sure all symbols get defined in permanant space

  p=(lisp_symbol *)malloc(sizeof(lisp_symbol));
  p->type=L_SYMBOL;
  p->name=new_lisp_string(name);

  if (name[0]==':')     // constant, set the value to ourself
    p->value=p;
  else
    p->value=l_undefined;
  p->function=l_undefined;
#ifdef L_PROFILE
  p->time_taken=0;
#endif
  p->left=p->right=NULL;
  *parent=p;
  ltotal_syms++;

  current_space=sp;
  return p;
}


void ldelete_syms(lisp_symbol *root)
{
  if (root)
  {
    ldelete_syms(root->left);
    ldelete_syms(root->right);
    free(root);
  }
}

void *assoc(void *item, void *list)
{
  if (item_type(list)!=(ltype)L_CONS_CELL)
    return NULL;
  else
  {
    while (list)
    {
      if (lisp_eq(CAR(CAR(list)), item))
        return lcar(list);    
      list=(cons_cell *)(CDR(list));
    }
  }
  return NULL;
}

long list_length(void *i)
{
  long x;

#ifdef TYPE_CHECKING
  if (i && item_type(i)!=(ltype)L_CONS_CELL)
  {
    lprint(i);
    lbreak(" is not a sequence\n");
    exit(0);
  }
#endif

  for(x = 0; i; i = CDR(i))
    x++;
  return x;
}

    

void *pairlis(void *list1, void *list2, void *list3)
{    
  if (item_type(list1)!=(ltype)L_CONS_CELL || item_type(list1)!=item_type(list2))
    return NULL;

  void *ret=NULL;
  long l1=list_length(list1), l2=list_length(list2);
  if (l1!=l2)
  {    
    lprint(list1);
    lprint(list2);
    lbreak("... are not the same length (pairlis)\n");
    exit(0);
  }
  if (l1!=0)
  {
    void *first=NULL, *last=NULL, *cur=NULL, *tmp;
    p_ref r1(first), r2(last), r3(cur);
    while (list1)
    {
      cur=new_cons_cell();
      if (!first) first=cur;
      if (last)
        ((cons_cell *)last)->cdr=cur;
      last=cur;
    
      cons_cell *cell=new_cons_cell();    
      tmp=lcar(list1);
      ((cons_cell *)cell)->car=tmp;
      tmp=lcar(list2);
      ((cons_cell *)cell)->cdr=tmp;
      ((cons_cell *)cur)->car=cell;

      list1=((cons_cell *)list1)->cdr;
      list2=((cons_cell *)list2)->cdr;
    }
    ((cons_cell *)cur)->cdr=list3;
    ret=first;
  } else ret=NULL;
  return ret;
}

void *lookup_symbol_function(void *symbol)
{
  return ((lisp_symbol *)symbol)->function;
}

void set_symbol_function(void *symbol, void *function)
{
  ((lisp_symbol *)symbol)->function=function;
}

void *lookup_symbol_value(void *symbol)
{
#ifdef TYPE_CHECKING
  if (((lisp_symbol *)symbol)->value!=l_undefined)
#endif
    return ((lisp_symbol *)symbol)->value;
#ifdef TYPE_CHECKING
  else
  {
    lprint(symbol);
    lbreak(" has no value\n");
    exit(0);
  }
#endif
  return NULL;
}

void set_variable_value(void *symbol, void *value)
{
  ((lisp_symbol *) symbol)->value=value;
}

lisp_symbol *add_sys_function(char const *name, short min_args, short max_args, short number)
{
  need_perm_space("add_sys_function");
  lisp_symbol *s=make_find_symbol(name);
  if (s->function!=l_undefined)
  {
    lbreak("add_sys_fucntion -> symbol %s already has a function\n", name);
    exit(0);
  }
  else s->function=new_lisp_sys_function(min_args, max_args, number);
  return s;
}

lisp_symbol *add_c_object(void *symbol, int16_t number)
{
  need_perm_space("add_c_object");
  lisp_symbol *s=(lisp_symbol *)symbol;
  if (s->value!=l_undefined)
  {
    lbreak("add_c_object -> symbol %s already has a value\n", lstring_value(symbol_name(s)));
    exit(0);
  }
  else s->value=new_lisp_object_var(number);
  return NULL;
}

lisp_symbol *add_c_function(char const *name, short min_args, short max_args, short number)
{
  total_user_functions++;
  need_perm_space("add_c_function");
  lisp_symbol *s=make_find_symbol(name);
  if (s->function!=l_undefined)
  {
    lbreak("add_sys_fucntion -> symbol %s already has a function\n", name);
    exit(0);
  }
  else s->function=new_lisp_c_function(min_args, max_args, number);
  return s;
}

lisp_symbol *add_c_bool_fun(char const *name, short min_args, short max_args, short number)
{
  total_user_functions++;
  need_perm_space("add_c_bool_fun");
  lisp_symbol *s=make_find_symbol(name);
  if (s->function!=l_undefined)
  {
    lbreak("add_sys_fucntion -> symbol %s already has a function\n", name);
    exit(0);
  }
  else s->function=new_lisp_c_bool(min_args, max_args, number);
  return s;
}


lisp_symbol *add_lisp_function(char const *name, short min_args, short max_args, short number)
{
  total_user_functions++;
  need_perm_space("add_c_bool_fun");
  lisp_symbol *s=make_find_symbol(name);
  if (s->function!=l_undefined)
  {
    lbreak("add_sys_fucntion -> symbol %s already has a function\n", name);
    exit(0);
  }
  else s->function=new_user_lisp_function(min_args, max_args, number);
  return s;
}

void skip_c_comment(char const *&s)
{
  s+=2;
  while (*s && (*s!='*' || *(s+1)!='/'))
  {
    if (*s=='/' && *(s+1)=='*')
      skip_c_comment(s);
    else s++;
  }
  if (*s) s+=2;
}

long str_token_len(char const *st)
{
  long x=1;
  while (*st && (*st!='"' || st[1]=='"'))
  {
    if (*st=='\\' || *st=='"') st++;
    st++; x++;
  }
  return x;
}

int read_ltoken(char const *&s, char *buffer)
{
  // skip space
  while (*s==' ' || *s=='\t' || *s=='\n' || *s=='\r' || *s==26) s++;
  if (*s==';')  // comment
  {
    while (*s && *s!='\n' && *s!='\r' && *s!=26) s++;
    return read_ltoken(s, buffer);
  } else if  (*s=='/' && *(s+1)=='*')   // c style comment
  {
    skip_c_comment(s);
    return read_ltoken(s, buffer);
  }
  else if (*s==0)
    return 0;
  else if (*s==')' || *s=='(' || *s=='\'' || *s=='`' || *s==',' || *s==26)
  {
    *(buffer++)=*(s++);
    *buffer=0;
  } else if (*s=='"')    // string
  {
    *(buffer++)=*(s++);          // don't read off the string because it
                                 // may be to long to fit in the token buffer
                                 // so just read the '"' so the compiler knows to scan the rest.
    *buffer=0;
  } else if (*s=='#')
  {
    *(buffer++)=*(s++);
    if (*s!='\'')
      *(buffer++)=*(s++);
    *buffer=0;
  } else
  {
    while (*s && *s!=')' && *s!='(' && *s!=' ' && *s!='\n' && *s!='\r' && *s!='\t' && *s!=';' && *s!=26)
      *(buffer++)=*(s++);
    *buffer=0;
  }
  return 1;
}


char n[MAX_LISP_TOKEN_LEN];  // assume all tokens will be < 200 characters

int end_of_program(char const *s)
{
  return !read_ltoken(s, n);
}


void push_onto_list(void *object, void *&list)
{
  p_ref r1(object), r2(list);
  cons_cell *c=new_cons_cell();
  c->car=object;
  c->cdr=list;
  list=c;
}

void *comp_optimize(void *list);

void *compile(char const *&s)
{
  void *ret=NULL;
  if (!read_ltoken(s, n))
    lerror(NULL, "unexpected end of program");
  if (!strcmp(n, "nil"))
    return NULL;
  else if (toupper(n[0])=='T' && !n[1])
    return true_symbol;
  else if (n[0]=='\'')                    // short hand for quote function
  {
    void *cs=new_cons_cell(), *c2=NULL, *tmp;
    p_ref r1(cs), r2(c2);

    ((cons_cell *)cs)->car=quote_symbol;
    c2=new_cons_cell();
    tmp=compile(s);
    ((cons_cell *)c2)->car=tmp;
    ((cons_cell *)c2)->cdr=NULL;
    ((cons_cell *)cs)->cdr=c2;
    ret=cs;
  }
  else if (n[0]=='`')                    // short hand for backquote function
  {
    void *cs=new_cons_cell(), *c2=NULL, *tmp;
    p_ref r1(cs), r2(c2);

    ((cons_cell *)cs)->car=backquote_symbol;
    c2=new_cons_cell();
    tmp=compile(s);
    ((cons_cell *)c2)->car=tmp;
    ((cons_cell *)c2)->cdr=NULL;
    ((cons_cell *)cs)->cdr=c2;
    ret=cs;
  }  else if (n[0]==',')              // short hand for comma function
  {
    void *cs=new_cons_cell(), *c2=NULL, *tmp;
    p_ref r1(cs), r2(c2);

    ((cons_cell *)cs)->car=comma_symbol;
    c2=new_cons_cell();
    tmp=compile(s);
    ((cons_cell *)c2)->car=tmp;
    ((cons_cell *)c2)->cdr=NULL;
    ((cons_cell *)cs)->cdr=c2;
    ret=cs;
  }
  else if (n[0]=='(')                     // make a list of everything in ()
  {
    void *first=NULL, *cur=NULL, *last=NULL;
    p_ref r1(first), r2(cur), r3(last);
    int done=0;
    do
    {
      char const *tmp=s;
      if (!read_ltoken(tmp, n))           // check for the end of the list
        lerror(NULL, "unexpected end of program");
      if (n[0]==')')
      {
                done=1;
                read_ltoken(s, n);                // read off the ')'
      }
      else
      {
                if (n[0]=='.' && !n[1])
                {
                  if (!first)
                    lerror(s, "token '.' not allowed here\n");    
                  else
                  {
                    void *tmp;
                    read_ltoken(s, n);              // skip the '.'
                    tmp=compile(s);
                    ((cons_cell *)last)->cdr=tmp;          // link the last cdr to
                    last=NULL;
                  }
                } else if (!last && first)
                  lerror(s, "illegal end of dotted list\n");
                else
                {        
                  void *tmp;
                  cur=new_cons_cell();
                  p_ref r1(cur);
                  if (!first) first=cur;
                  tmp=compile(s);    
                  ((cons_cell *)cur)->car=tmp;
                  if (last)
                    ((cons_cell *)last)->cdr=cur;
                  last=cur;
                }
      }
    } while (!done);
    ret=comp_optimize(first);

  } else if (n[0]==')')
    lerror(s, "mismatched )");
  else if (isdigit(n[0]) || (n[0]=='-' && isdigit(n[1])))
  {
    lisp_number *num=new_lisp_number(0);
    sscanf(n, "%ld", &num->num);
    ret=num;
  } else if (n[0]=='"')
  {
    ret=new_lisp_string(str_token_len(s));
    char *start=lstring_value(ret);
    for (;*s && (*s!='"' || s[1]=='"');s++, start++)
    {
      if (*s=='\\')
      {
                s++;
                if (*s=='n') *start='\n';
                if (*s=='r') *start='\r';
                if (*s=='t') *start='\t';
                if (*s=='\\') *start='\\';
      } else *start=*s;
      if (*s=='"') s++;
    }
    *start=0;
    s++;
  } else if (n[0]=='#')
  {
    if (n[1]=='\\')
    {
      read_ltoken(s, n);                   // read character name
      if (!strcmp(n, "newline"))
        ret=new_lisp_character('\n');
      else if (!strcmp(n, "space"))
        ret=new_lisp_character(' ');
      else
        ret=new_lisp_character(n[0]);
    }
    else if (n[1]==0)                           // short hand for function
    {
      void *cs=new_cons_cell(), *c2=NULL, *tmp;
      p_ref r4(cs), r5(c2);
      tmp=make_find_symbol("function");
      ((cons_cell *)cs)->car=tmp;
      c2=new_cons_cell();
      tmp=compile(s);
      ((cons_cell *)c2)->car=tmp;
      ((cons_cell *)cs)->cdr=c2;
      ret=cs;
    }
    else
    {
      lbreak("Unknown #\\ notation : %s\n", n);
      exit(0);
    }
  } else {
    ret = make_find_symbol(n);
  }
  return ret;
}


static void lprint_string(char const *st)
{
  if (current_print_file)
  {
    for (char const *s=st;*s;s++)
    {
/*      if (*s=='\\')
      {
    s++;
    if (*s=='n')
      current_print_file->write_uint8('\n');
    else if (*s=='r')
      current_print_file->write_uint8('\r');
    else if (*s=='t')
      current_print_file->write_uint8('\t');
    else if (*s=='\\')
      current_print_file->write_uint8('\\');
      }
      else*/
        current_print_file->write_uint8(*s);
    }
  }
  else
    dprintf(st);
}

void lprint(void *i)
{
  print_level++;
  if (!i)
    lprint_string("nil");
  else
  {
    switch ((short)item_type(i))
    {
      case L_CONS_CELL :
      {
                cons_cell *cs=(cons_cell *)i;
        lprint_string("(");
        for (;cs;cs=(cons_cell *)lcdr(cs))    
                {
                  if (item_type(cs)==(ltype)L_CONS_CELL)
                  {
                        lprint(cs->car);
                    if (cs->cdr)
                      lprint_string(" ");
                  }
                  else
                  {
                    lprint_string(". ");
                    lprint(cs);
                    cs=NULL;
                  }
                }
        lprint_string(")");
      }
      break;
      case L_NUMBER :
      {
                char num[10];
                sprintf(num, "%ld", ((lisp_number *)i)->num);
        lprint_string(num);
      }
      break;
      case L_SYMBOL :
        lprint_string((char *)(((lisp_symbol *)i)->name)+sizeof(lisp_string));
      break;
      case L_USER_FUNCTION :
      case L_SYS_FUNCTION :
        lprint_string("err... function?");
      break;
      case L_C_FUNCTION :
        lprint_string("C function, returns number\n");
      break;
      case L_C_BOOL :
        lprint_string("C boolean function\n");
      break;
      case L_L_FUNCTION :
        lprint_string("External lisp function\n");
            break;
      case L_STRING :
      {
                if (current_print_file)
                     lprint_string(lstring_value(i));
                else
             dprintf("\"%s\"", lstring_value(i));
      }
      break;

      case L_POINTER :
      {
                char ptr[10];
                    sprintf(ptr, "%p", lpointer_value(i));
                lprint_string(ptr);
      }
      break;
      case L_FIXED_POINT :
      {
                char num[20];
                sprintf(num, "%g", (lfixed_point_value(i)>>16)+
                          ((lfixed_point_value(i)&0xffff))/(double)0x10000);
                lprint_string(num);
      } break;
      case L_CHARACTER :
      {
                if (current_print_file)
                {
                  uint8_t ch=((lisp_character *)i)->ch;
                  current_print_file->write(&ch, 1);
                } else
                {
                  uint16_t ch=((lisp_character *)i)->ch;
                  dprintf("#\\");
                  switch (ch)
                  {
                    case '\n' :
                    { dprintf("newline"); break; }
                    case ' ' :
                    { dprintf("space"); break; }
                    default :
                      dprintf("%c", ch);
                  }
                }
      } break;
      case L_OBJECT_VAR :
      {
                l_obj_print(((lisp_object_var *)i)->number);
      } break;
      case L_1D_ARRAY :
      {
                lisp_1d_array *a=(lisp_1d_array *)i;
                void **data=(void **)(a+1);
                dprintf("#(");
                for (int j=0;j<a->size;j++)
                {
                  lprint(data[j]);
                  if (j!=a->size-1)
                    dprintf(" ");
                }
                dprintf(")");
      } break;
      case L_COLLECTED_OBJECT :
      {
                lprint_string("GC_refrence->");
                lprint(((lisp_collected_object *)i)->new_reference);
      } break;
      default :
        dprintf("Shouldn't happen\n");
    }
  }
  print_level--;
  if (!print_level && !current_print_file)
    dprintf("\n");
}

void *eval_sys_function(lisp_sys_function *fun, void *arg_list);

void *eval_function(lisp_symbol *sym, void *arg_list)
{
#ifdef TYPE_CHECKING
  int args, req_min, req_max;
  if (item_type(sym)!=L_SYMBOL)
  {
    lprint(sym);
    lbreak("EVAL : is not a function name (not symbol either)");
    exit(0);
  }
#endif

  void *fun=(lisp_sys_function *)(((lisp_symbol *)sym)->function);
  p_ref ref2( fun  );

  // make sure the arguments given to the function are the correct number
  ltype t=item_type(fun);

#ifdef TYPE_CHECKING
  switch (t)
  {
    case L_SYS_FUNCTION :
    case L_C_FUNCTION :
    case L_C_BOOL :
    case L_L_FUNCTION :
    {
      req_min=((lisp_sys_function *)fun)->min_args;
      req_max=((lisp_sys_function *)fun)->max_args;
    } break;
    case L_USER_FUNCTION :
    {
      return eval_user_fun(sym, arg_list);
    } break;
    default :
    {
      lprint(sym);
      lbreak(" is not a function name");
      exit(0);    
    } break;
  }

  if (req_min!=-1)
  {
    void *a=arg_list;
    for (args=0;a;a=CDR(a)) args++;    // count number of paramaters

    if (args<req_min)
    {
      lprint(arg_list);
      lprint(sym->name);
      lbreak("\nToo few parameters to function\n");
      exit(0);
    } else if (req_max!=-1 && args>req_max)
    {
      lprint(arg_list);
      lprint(sym->name);
      lbreak("\nToo many parameters to function\n");
      exit(0);
    }
  }
#endif

#ifdef L_PROFILE
  time_marker start;
#endif


  p_ref ref1(arg_list);
  void *ret=NULL;

  switch (t)
  {
    case L_SYS_FUNCTION :
    { ret=eval_sys_function( ((lisp_sys_function *)fun), arg_list); } break;
    case L_L_FUNCTION :
    { ret=l_caller( ((lisp_sys_function *)fun)->fun_number, arg_list); } break;
    case L_USER_FUNCTION :
    {
      return eval_user_fun(sym, arg_list);
    } break;
    case L_C_FUNCTION :
    case L_C_BOOL :
    {
      void *first=NULL, *cur=NULL, *tmp;
      p_ref r1(first), r2(cur);
      while (arg_list)
      {
        if (first) {
          tmp=new_cons_cell();
          ((cons_cell *)cur)->cdr=tmp;
          cur=tmp;
        } else
          cur=first=new_cons_cell();
    
        void *val=eval(CAR(arg_list));
        ((cons_cell *)cur)->car=val;
        arg_list=lcdr(arg_list);
      }
      if(t == L_C_FUNCTION)
        ret=new_lisp_number(c_caller( ((lisp_sys_function *)fun)->fun_number, first));
      else if (c_caller( ((lisp_sys_function *)fun)->fun_number, first))
        ret=true_symbol;
      else ret=NULL;
    } break;
    default :
      fprintf(stderr, "not a fun, shouldn't happen\n");
  }

#ifdef L_PROFILE
  time_marker end;
  ((lisp_symbol *)sym)->time_taken+=end.diff_time(&start);
#endif

  return ret;
}    

#ifdef L_PROFILE
void pro_print(bFILE *out, lisp_symbol *p)
{
  if (p)
  {
    pro_print(out, p->right);
    {
      char st[100];
      sprintf(st, "%20s %f\n", lstring_value(symbol_name(p)), ((lisp_symbol *)p)->time_taken);
      out->write(st, strlen(st));
    }
    pro_print(out, p->left);
  }
}

void preport(char *fn)
{
  bFILE *fp=open_file("preport.out", "wb");
  pro_print(fp, lsym_root);
  delete fp;
}
#endif

void *mapcar(void *arg_list)
{
  p_ref ref1(arg_list);
  void *sym=eval(CAR(arg_list));
  switch ((short)item_type(sym))
  {
    case L_SYS_FUNCTION :
    case L_USER_FUNCTION :
    case L_SYMBOL :
    break;
    default :
    {
      lprint(sym);
      lbreak(" is not a function\n");
      exit(0);
    }
  }
  int num_args=list_length(CDR(arg_list)), i, stop=0;
  if (!num_args) return 0;

  void **arg_on=(void **)malloc(sizeof(void *)*num_args);
  cons_cell *list_on=(cons_cell *)CDR(arg_list);
  long old_ptr_son=l_ptr_stack.son;

  for (i=0;i<num_args;i++)
  {
    arg_on[i]=(cons_cell *)eval(CAR(list_on));
    l_ptr_stack.push(&arg_on[i]);

    list_on=(cons_cell *)CDR(list_on);
    if (!arg_on[i]) stop=1;
  }

  if (stop)
  {
    free(arg_on);
    return NULL;
  }

  cons_cell *na_list=NULL, *return_list=NULL, *last_return=NULL;

  do
  {
    na_list=NULL;          // create a cons list with all of the parameters for the function

    cons_cell *first=NULL;                       // save the start of the list
    for (i=0;!stop &&i<num_args;i++)
    {
      if (!na_list)
        first=na_list=new_cons_cell();
      else
      {
        na_list->cdr=new_cons_cell();
                na_list=(cons_cell *)CDR(na_list);
      }


      if (arg_on[i])
      {
                na_list->car=CAR(arg_on[i]);
                arg_on[i]=(cons_cell *)CDR(arg_on[i]);
      }
      else stop=1;
    }
    if (!stop)
    {
      cons_cell *c=new_cons_cell();
      c->car=eval_function((lisp_symbol *)sym, first);
      if (return_list)
        last_return->cdr=c;
      else
        return_list=c;
      last_return=c;
    }
  }
  while (!stop);
  l_ptr_stack.son=old_ptr_son;

  free(arg_on);
  return return_list;
}

void *concatenate(void *prog_list)
{
  void *el_list=CDR(prog_list);
  p_ref ref1(prog_list), ref2(el_list);
  void *ret=NULL;
  void *rtype=eval(CAR(prog_list));

  long len=0;                                // determin the length of the resulting string
  if (rtype==string_symbol)
  {
    int elements=list_length(el_list);       // see how many things we need to concat
    if (!elements) ret=new_lisp_string("");
    else
    {
      void **str_eval=(void **)malloc(elements*sizeof(void *));
      int i, old_ptr_stack_start=l_ptr_stack.son;

      // evalaute all the strings and count their lengths
      for (i=0;i<elements;i++, el_list=CDR(el_list))
      {
        str_eval[i]=eval(CAR(el_list));
    l_ptr_stack.push(&str_eval[i]);

    switch ((short)item_type(str_eval[i]))
    {
      case L_CONS_CELL :
      {
        cons_cell *char_list=(cons_cell *)str_eval[i];
        while (char_list)
        {
          if (item_type(CAR(char_list))==(ltype)L_CHARACTER)
            len++;
          else
          {
        lprint(str_eval[i]);
        lbreak(" is not a character\n");        
        exit(0);
          }
          char_list=(cons_cell *)CDR(char_list);
        }
      } break;
      case L_STRING : len+=strlen(lstring_value(str_eval[i])); break;
      default :
        lprint(prog_list);
        lbreak("type not supported\n");
        exit(0);
      break;

    }
      }
      lisp_string *st=new_lisp_string(len+1);
      char *s=lstring_value(st);

      // now add the string up into the new string
      for (i=0;i<elements;i++)
      {
    switch ((short)item_type(str_eval[i]))
    {
      case L_CONS_CELL :
      {
        cons_cell *char_list=(cons_cell *)str_eval[i];
        while (char_list)
        {
          if (item_type(CAR(char_list))==L_CHARACTER)
            *(s++)=((lisp_character *)CAR(char_list))->ch;
          char_list=(cons_cell *)CDR(char_list);
        }
      } break;
      case L_STRING :
      {
        memcpy(s, lstring_value(str_eval[i]), strlen(lstring_value(str_eval[i])));
        s+=strlen(lstring_value(str_eval[i]));
      } break;
      default : ;     // already checked for, but make compiler happy
    }
      }
      free(str_eval);
      l_ptr_stack.son=old_ptr_stack_start;   // restore pointer GC stack
      *s=0;
      ret=st;
    }
  }
  else
  {
    lprint(prog_list);
    lbreak("concat operation not supported, try 'string\n");
    exit(0);
  }
  return ret;
}


void *backquote_eval(void *args)
{
  if (item_type(args)!=L_CONS_CELL)
    return args;
  else if (args==NULL)
    return NULL;
  else if ((lisp_symbol *) (((cons_cell *)args)->car)==comma_symbol)
    return eval(CAR(CDR(args)));
  else
  {
    void *first=NULL, *last=NULL, *cur=NULL, *tmp;
    p_ref ref1(first), ref2(last), ref3(cur), ref4(args);
    while (args)
    {
      if (item_type(args)==L_CONS_CELL)
      {
    if (CAR(args)==comma_symbol)               // dot list with a comma?
    {
      tmp=eval(CAR(CDR(args)));
      ((cons_cell *)last)->cdr=tmp;
      args=NULL;
    }
    else
    {
      cur=new_cons_cell();
      if (first)
        ((cons_cell *)last)->cdr=cur;
      else
            first=cur;
      last=cur;
          tmp=backquote_eval(CAR(args));
          ((cons_cell *)cur)->car=tmp;
       args=CDR(args);
    }
      } else
      {
    tmp=backquote_eval(args);
    ((cons_cell *)last)->cdr=tmp;
    args=NULL;
      }

    }
    return (void *)first;
  }
  return NULL;       // for stupid compiler messages
}


void *eval_sys_function(lisp_sys_function *fun, void *arg_list)
{
  p_ref ref1(arg_list);
  void *ret=NULL;
  switch (fun->fun_number)
  {
    case SYS_FUNC_PRINT:
    {
      ret=NULL;
      while (arg_list)
      {
        ret=eval(CAR(arg_list));  arg_list=CDR(arg_list);
    lprint(ret);
      }
      return ret;
    } break;
    case SYS_FUNC_CAR:
    { ret=lcar(eval(CAR(arg_list))); } break;
    case SYS_FUNC_CDR:
    { ret=lcdr(eval(CAR(arg_list))); } break;
    case SYS_FUNC_LENGTH:
    {
      void *v=eval(CAR(arg_list));
      switch (item_type(v))
      {
        case L_STRING : ret=new_lisp_number(strlen(lstring_value(v))); break;
        case L_CONS_CELL : ret=new_lisp_number(list_length(v)); break;
        default :
        { lprint(v);
          lbreak("length : type not supported\n");
        }
      }
    } break;                        
    case SYS_FUNC_LIST:
    {
      void *cur=NULL, *last=NULL, *first=NULL;
      p_ref r1(cur), r2(first), r3(last);
      while (arg_list)
      {
    cur=new_cons_cell();
    void *val=eval(CAR(arg_list));
    ((cons_cell *) cur)->car=val;
    if (last)
      ((cons_cell *)last)->cdr=cur;
    else first=cur;
    last=cur;
    arg_list=(cons_cell *)CDR(arg_list);
      }    
      ret=first;
    } break;
    case SYS_FUNC_CONS:
    { void *c=new_cons_cell();
      p_ref r1(c);
      void *val=eval(CAR(arg_list));
      ((cons_cell *)c)->car=val;
      val=eval(CAR(CDR(arg_list)));
      ((cons_cell *)c)->cdr=val;
      ret=c;
    } break;
    case SYS_FUNC_QUOTE:
    ret=CAR(arg_list);
    break;
    case SYS_FUNC_EQ:
    {
      l_user_stack.push(eval(CAR(arg_list)));
      l_user_stack.push(eval(CAR(CDR(arg_list))));
      ret=lisp_eq(l_user_stack.pop(1), l_user_stack.pop(1));
    } break;
    case SYS_FUNC_EQUAL:
    {
      l_user_stack.push(eval(CAR(arg_list)));
      l_user_stack.push(eval(CAR(CDR(arg_list))));
      ret=lisp_equal(l_user_stack.pop(1), l_user_stack.pop(1));
    } break;
    case SYS_FUNC_PLUS:
    {
      long sum=0;
      while (arg_list)
      {
    sum+=lnumber_value(eval(CAR(arg_list)));
    arg_list=CDR(arg_list);
      }
      ret=new_lisp_number(sum);
    }
    break;
    case SYS_FUNC_TIMES:
    {
      long sum;
      void *first=eval(CAR(arg_list));
      p_ref r1(first);
      if (arg_list && item_type(first)==L_FIXED_POINT)
      {
    sum=1<<16;
    do
    {
      sum=(sum>>8)*(lfixed_point_value(first)>>8);
      arg_list=CDR(arg_list);
      if (arg_list) first=eval(CAR(arg_list));
    } while (arg_list);

    ret=new_lisp_fixed_point(sum);
      } else
      { sum=1;
    do
    {
      sum*=lnumber_value(eval(CAR(arg_list)));
      arg_list=CDR(arg_list);
      if (arg_list) first=eval(CAR(arg_list));
    } while (arg_list);
    ret=new_lisp_number(sum);
      }
    }
    break;
    case SYS_FUNC_SLASH:
    {
      long sum=0, first=1;
      while (arg_list)
      {
    void *i=eval(CAR(arg_list));
    p_ref r1(i);
    if (item_type(i)!=L_NUMBER)
    {
      lprint(i);
      lbreak("/ only defined for numbers, cannot divide ");
      exit(0);
    } else if (first)
    {
      sum=((lisp_number *)i)->num;
      first=0;
    }
    else sum/=((lisp_number *)i)->num;
    arg_list=CDR(arg_list);
      }
      ret=new_lisp_number(sum);
    }
    break;
    case SYS_FUNC_MINUS:
    {
      long x=lnumber_value(eval(CAR(arg_list)));         arg_list=CDR(arg_list);
      while (arg_list)
      {
    x-=lnumber_value(eval(CAR(arg_list)));
    arg_list=CDR(arg_list);
      }
      ret=new_lisp_number(x);
    }
    break;
    case SYS_FUNC_IF:
    {
      if (eval(CAR(arg_list)))
      ret=eval(CAR(CDR(arg_list)));
      else
      { arg_list=CDR(CDR(arg_list));                 // check for a else part
    if (arg_list)    
      ret=eval(CAR(arg_list));
    else ret=NULL;
      }
    } break;
    case SYS_FUNC_SETQ:
    case SYS_FUNC_SETF:
    {
      void *set_to=eval(CAR(CDR(arg_list))), *i=NULL;
      p_ref r1(set_to), r2(i);
      i=CAR(arg_list);

      ltype x=item_type(set_to);
      switch (item_type(i))
      {
        case L_SYMBOL :
        {
          switch (item_type (((lisp_symbol *)i)->value))
          {
            case L_NUMBER :
            {
              if (x==L_NUMBER && ((lisp_symbol *)i)->value!=l_undefined)
              ((lisp_number *)(((lisp_symbol *)i)->value))->num=lnumber_value(set_to);
              else
              ((lisp_symbol *)i)->value=set_to;
            } break;
            case L_OBJECT_VAR :
            {
              l_obj_set(((lisp_object_var *)(((lisp_symbol *)i)->value))->number, set_to);
            } break;
            default :
            ((lisp_symbol *)i)->value=set_to;
          }
          ret=((lisp_symbol *)i)->value;
        } break;
        case L_CONS_CELL :   // this better be an 'aref'
        {
#ifdef TYPE_CHECKING
          void *car=((cons_cell *)i)->car;
          if (car==car_symbol)
          {
            car=eval(CAR(CDR(i)));
            if (!car || item_type(car)!=L_CONS_CELL)
            { lprint(car); lbreak("setq car : evaled object is not a cons cell\n"); exit(0); }
            ((cons_cell *)car)->car=set_to;
          } else if (car==cdr_symbol)
          {
            car=eval(CAR(CDR(i)));
            if (!car || item_type(car)!=L_CONS_CELL)
            { lprint(car); lbreak("setq cdr : evaled object is not a cons cell\n"); exit(0); }
            ((cons_cell *)car)->cdr=set_to;
          } else if (car==aref_symbol)
          {
#endif
            void *a=(lisp_1d_array *)eval(CAR(CDR(i)));
            p_ref r1(a);
#ifdef TYPE_CHECKING
            if (item_type(a)!=L_1D_ARRAY)
            {
              lprint(a);
              lbreak("is not an array (aref)\n");
              exit(0);
            }
#endif
            long num=lnumber_value(eval(CAR(CDR(CDR(i)))));
#ifdef TYPE_CHECKING
            if (num>=((lisp_1d_array *)a)->size || num<0)
            {
              lbreak("aref : value of bounds (%d)\n", num);
              exit(0);
            }
#endif
            void **data=(void **)(((lisp_1d_array *)a)+1);
            data[num]=set_to;
#ifdef TYPE_CHECKING
          } else
          {
            lbreak("expected (aref, car, cdr, or symbol) in setq\n");
            exit(0);
          }
#endif
          ret=set_to;
        } break;

        default :
        {
          lprint(i);
          lbreak("setq/setf only defined for symbols and arrays now..\n");
          exit(0);
        }
      }
    } break;
    case SYS_FUNC_SYMBOL_LIST:
      ret=NULL;
    break;
    case SYS_FUNC_ASSOC:
    {
      void *item=eval(CAR(arg_list));
      p_ref r1(item);
      void *list=(cons_cell *)eval(CAR(CDR(arg_list)));
      p_ref r2(list);
      ret=assoc(item, (cons_cell *)list);
    } break;
    case SYS_FUNC_NOT:
    case SYS_FUNC_NULL:
    if (eval(CAR(arg_list))==NULL) ret=true_symbol; else ret=NULL;
    break;
    case SYS_FUNC_ACONS:
    {
      void *i1=eval(CAR(arg_list)), *i2=eval(CAR(CDR(arg_list)));
      p_ref r1(i1);
      cons_cell *cs=new_cons_cell();
      cs->car=i1;
      cs->cdr=i2;
      ret=cs;
    } break;

    case SYS_FUNC_PAIRLIS:
    {    
      l_user_stack.push(eval(CAR(arg_list))); arg_list=CDR(arg_list);
      l_user_stack.push(eval(CAR(arg_list))); arg_list=CDR(arg_list);
      void *n3=eval(CAR(arg_list));
      void *n2=l_user_stack.pop(1);
      void *n1=l_user_stack.pop(1);
      ret=pairlis(n1, n2, n3);
    } break;
    case SYS_FUNC_LET:
    {
      // make an a-list of new variable names and new values
      void *var_list=CAR(arg_list),
           *block_list=CDR(arg_list);
      p_ref r1(block_list), r2(var_list);
      long stack_start=l_user_stack.son;

      while (var_list)
      {
    void *var_name=CAR(CAR(var_list)), *tmp;
#ifdef TYPE_CHECKING
    if (item_type(var_name)!=L_SYMBOL)
    {
      lprint(var_name);
      lbreak("should be a symbol (let)\n");
      exit(0);
    }
#endif

    l_user_stack.push(((lisp_symbol *)var_name)->value);
    tmp=eval(CAR(CDR(CAR(var_list))));    
    ((lisp_symbol *)var_name)->value=tmp;
    var_list=CDR(var_list);
      }

      // now evaluate each of the blocks with the new enviroment and return value
      // from the last block
      while (block_list)
      {    
    ret=eval(CAR(block_list));
    block_list=CDR(block_list);    
      }

      long cur_stack=stack_start;
      var_list=CAR(arg_list);      // now restore the old symbol values
      while (var_list)
      {
    void *var_name=CAR(CAR(var_list));
    ((lisp_symbol *)var_name)->value=l_user_stack.sdata[cur_stack++];
    var_list=CDR(var_list);
      }
      l_user_stack.son=stack_start;     // restore the stack
    }
    break;
    case SYS_FUNC_DEFUN:
    {
      void *symbol=CAR(arg_list);
#ifdef TYPE_CHECKING
      if (item_type(symbol)!=L_SYMBOL)
      {
    lprint(symbol);
    lbreak(" is not a symbol! (DEFUN)\n");
    exit(0);
      }

      if (item_type(arg_list)!=L_CONS_CELL)
      {
    lprint(arg_list);
    lbreak("is not a lambda list (DEFUN)\n");
    exit(0);
      }
#endif
      void *block_list=CDR(CDR(arg_list));

#ifndef NO_LIBS
      intptr_t a=cache.reg_lisp_block(lcar(lcdr(arg_list)));
      intptr_t b=cache.reg_lisp_block(block_list);
      lisp_user_function *ufun=new_lisp_user_function(a, b);
#else
      lisp_user_function *ufun=new_lisp_user_function(lcar(lcdr(arg_list)), block_list);
#endif
      set_symbol_function(symbol, ufun);
      ret=symbol;
    } break;
    case SYS_FUNC_ATOM:
    { ret=lisp_atom(eval(CAR(arg_list))); }
    case SYS_FUNC_AND:
    {
      void *l=arg_list;
      p_ref r1(l);
      ret=true_symbol;
      while (l)
      {
    if (!eval(CAR(l)))
    {
      ret=NULL;
      l=NULL;             // short-circuit
    } else l=CDR(l);
      }
    } break;
    case SYS_FUNC_OR:
    {
      void *l=arg_list;
      p_ref r1(l);
      ret=NULL;
      while (l)
      {
    if (eval(CAR(l)))
    {
      ret=true_symbol;
      l=NULL;            // short circuit
    } else l=CDR(l);
      }
    } break;
    case SYS_FUNC_PROGN:
    { ret=eval_block(arg_list); } break;
    case SYS_FUNC_CONCATENATE:
      ret=concatenate(arg_list);
    break;
    case SYS_FUNC_CHAR_CODE:
    {
      void *i=eval(CAR(arg_list));
      p_ref r1(i);
      ret=NULL;
      switch (item_type(i))
      {
        case L_CHARACTER :
        { ret=new_lisp_number(((lisp_character *)i)->ch); } break;
        case L_STRING :
        {  ret=new_lisp_number(*lstring_value(i)); } break;
        default :
        {
          lprint(i);
          lbreak(" is not character type\n");
          exit(0);
        }
      }        
    } break;
    case SYS_FUNC_CODE_CHAR:
    {
      void *i=eval(CAR(arg_list));
      p_ref r1(i);
      if (item_type(i)!=L_NUMBER)
      {
    lprint(i);
    lbreak(" is not number type\n");
    exit(0);
      }
      ret=new_lisp_character(((lisp_number *)i)->num);
    } break;
    case SYS_FUNC_COND:
    {
      void *block_list=CAR(arg_list);
      p_ref r1(block_list);
      if (!block_list) ret=NULL;
      else
      {
    ret=NULL;
        while (block_list)
    {
      if (eval(lcar(CAR(block_list))))
        ret=eval(CAR(CDR(CAR(block_list))));
      block_list=CDR(block_list);
    }
      }
    } break;
    case SYS_FUNC_SELECT:
    {
      void *selector=eval(CAR(arg_list));
      void *sel=CDR(arg_list);
      p_ref r1(selector), r2(sel);
      while (sel)
      {
    if (lisp_equal(selector, eval(CAR(CAR(sel)))))
    {
      sel=CDR(CAR(sel));
      while (sel)
      {
        ret=eval(CAR(sel));
        sel=CDR(sel);
      }
      sel=NULL;
    } else sel=CDR(sel);
      }
    } break;
    case SYS_FUNC_FUNCTION:
      ret=lookup_symbol_function(eval(CAR(arg_list)));
    break;
    case SYS_FUNC_MAPCAR:
      ret=mapcar(arg_list);
    case SYS_FUNC_FUNCALL:
    {
      void *n1=eval(CAR(arg_list));
      ret=eval_function((lisp_symbol *)n1, CDR(arg_list));
    } break;
    case SYS_FUNC_GT:
    {
      long n1=lnumber_value(eval(CAR(arg_list)));
      long n2=lnumber_value(eval(CAR(CDR(arg_list))));
      if (n1>n2) ret=true_symbol; else ret=NULL;
    }
    break;
    case SYS_FUNC_LT:
    {
      long n1=lnumber_value(eval(CAR(arg_list)));
      long n2=lnumber_value(eval(CAR(CDR(arg_list))));
      if (n1<n2) ret=true_symbol; else ret=NULL;
    }
    break;
    case SYS_FUNC_GE:
    {
      long n1=lnumber_value(eval(CAR(arg_list)));
      long n2=lnumber_value(eval(CAR(CDR(arg_list))));
      if (n1>=n2) ret=true_symbol; else ret=NULL;
    }
    break;
    case SYS_FUNC_LE:
    {
      long n1=lnumber_value(eval(CAR(arg_list)));
      long n2=lnumber_value(eval(CAR(CDR(arg_list))));
      if (n1<=n2) ret=true_symbol; else ret=NULL;
    }
    break;

    case SYS_FUNC_TMP_SPACE:
      tmp_space();
      ret=true_symbol;
    break;
    case SYS_FUNC_PERM_SPACE:
      perm_space();
      ret=true_symbol;
    break;
    case SYS_FUNC_SYMBOL_NAME:
      void *symb;
      symb=eval(CAR(arg_list));
#ifdef TYPE_CHECKING
      if (item_type(symb)!=L_SYMBOL)
      {
    lprint(symb);
    lbreak(" is not a symbol (symbol-name)\n");
    exit(0);
      }
#endif
      ret=((lisp_symbol *)symb)->name;
    break;
    case SYS_FUNC_TRACE:
      trace_level++;
      if (arg_list)
        trace_print_level=lnumber_value(eval(CAR(arg_list)));
      ret=true_symbol;
    break;
    case SYS_FUNC_UNTRACE:
      if (trace_level>0)
      {
                trace_level--;
                ret=true_symbol;
      } else ret=NULL;
    break;
    case SYS_FUNC_DIGSTR:
    {
      char tmp[50], *tp;
      long num=lnumber_value(eval(CAR(arg_list)));
      long dig=lnumber_value(eval(CAR(CDR(arg_list))));
      tp=tmp+49;
      *(tp--)=0;
      for (;num;)
      {
                int d;
                d=num%10;
                *(tp--)=d+'0';
                num/=10;
                dig--;
      }
      while (dig--)
        *(tp--)='0';
      ret=new_lisp_string(tp+1);
    } break;
    case SYS_FUNC_LOCAL_LOAD:
    case SYS_FUNC_LOAD:
    case SYS_FUNC_COMPILE_FILE:
    {
            void *fn = eval( CAR( arg_list ) );
            char *st = lstring_value( fn );
            p_ref r1( fn );
            bFILE *fp;
            if( fun->fun_number == SYS_FUNC_LOCAL_LOAD )
            {
                // A special test for gamma.lsp
                if( strcmp( st, "gamma.lsp" ) == 0 )
                {
                    char *gammapath;
                    gammapath = (char *)malloc( strlen( get_save_filename_prefix() ) + 9 + 1 );
                    sprintf( gammapath, "%sgamma.lsp", get_save_filename_prefix() );
                    fp = new jFILE( gammapath, "rb" );
                    free( gammapath );
                }
                else
                {
                    fp = new jFILE( st, "rb" );
                }
            }
            else
            {
                fp = open_file(st, "rb");
            }

            if( fp->open_failure() )
            {
                delete fp;
                if( DEFINEDP(symbol_value(load_warning)) && symbol_value(load_warning) )
                    dprintf("Warning : file %s does not exist\n", st);
                ret = NULL;
            }
            else
            {
                long l=fp->file_size();
                char *s=(char *)malloc(l + 1);
                if (!s)
                {
                  printf("Malloc error in load_script\n");
                  exit(0);
                }
            
                fp->read(s, l);
                s[l]=0;
                delete fp;
                char const *cs=s;
            #ifndef NO_LIBS
                char msg[100];
                sprintf(msg, "(load \"%s\")", st);
                if (stat_man) stat_man->push(msg, NULL);
                crc_manager.get_filenumber(st);               // make sure this file gets crc'ed
            #endif
                void *compiled_form=NULL;
                p_ref r11(compiled_form);
                while (!end_of_program(cs))  // see if there is anything left to compile and run
                {
            #ifndef NO_LIBS
                  if (stat_man) stat_man->update((cs-s)*100/l);
            #endif
                  void *m=mark_heap(TMP_SPACE);
                  compiled_form=compile(cs);
                  eval(compiled_form);
                  compiled_form=NULL;
                  restore_heap(m, TMP_SPACE);
                }    
            #ifndef NO_LIBS
                                if (stat_man) stat_man->update(100);
                if (stat_man) stat_man->pop();
            #endif
                free(s);
                ret=fn;
      }
    } break;
    case SYS_FUNC_ABS:
      ret=new_lisp_number(abs(lnumber_value(eval(CAR(arg_list))))); break;
    case SYS_FUNC_MIN:
    {
      int x=lnumber_value(eval(CAR(arg_list))), y=lnumber_value(eval(CAR(CDR(arg_list))));
      if (x<y) ret=new_lisp_number(x); else ret=new_lisp_number(y);
    } break;
    case SYS_FUNC_MAX:
    {
      int x=lnumber_value(eval(CAR(arg_list))), y=lnumber_value(eval(CAR(CDR(arg_list))));
      if (x>y) ret=new_lisp_number(x); else ret=new_lisp_number(y);
    } break;
    case SYS_FUNC_BACKQUOTE:
    {
      ret=backquote_eval(CAR(arg_list));
    } break;
    case SYS_FUNC_COMMA:
    {
      lprint(arg_list);
      lbreak("comma is illegal outside of backquote\n");
      exit(0);
      ret=NULL;
    } break;
    case SYS_FUNC_NTH:
    {
      long x=lnumber_value(eval(CAR(arg_list)));
      ret=nth(x, eval(CAR(CDR(arg_list))));
    } break;
    case SYS_FUNC_RESIZE_TMP:
        resize_tmp(lnumber_value(eval(CAR(arg_list)))); break;
    case SYS_FUNC_RESIZE_PERM:
        resize_perm(lnumber_value(eval(CAR(arg_list)))); break;
    case SYS_FUNC_COS:
        ret=new_lisp_fixed_point(lisp_cos(lnumber_value(eval(CAR(arg_list))))); break;
    case SYS_FUNC_SIN:
        ret=new_lisp_fixed_point(lisp_sin(lnumber_value(eval(CAR(arg_list))))); break;
    case SYS_FUNC_ATAN2:
    {
      long y=(lnumber_value(eval(CAR(arg_list))));   arg_list=CDR(arg_list);
      long x=(lnumber_value(eval(CAR(arg_list))));
      ret=new_lisp_number(lisp_atan2(y, x));
    } break;
    case SYS_FUNC_ENUM:
    {
      int sp=current_space;
      current_space=PERM_SPACE;
      long x=0;
      while (arg_list)
      {
    void *sym=eval(CAR(arg_list));
    p_ref r1(sym);
    switch (item_type(sym))
    {
      case L_SYMBOL :
      { ((lisp_symbol *)sym)->value=new_lisp_number(x); } break;
      case L_CONS_CELL :
      {
        void *s=eval(CAR(sym));
        p_ref r1(s);
#ifdef TYPE_CHECKING
        if (item_type(s)!=L_SYMBOL)
        { lprint(arg_list);
          lbreak("expecting (sybmol value) for enum\n");
          exit(0);
        }
#endif
        x=lnumber_value(eval(CAR(CDR(sym))));
        ((lisp_symbol *)sym)->value=new_lisp_number(x);
      } break;
      default :
      {
        lprint(arg_list);
        lbreak("expecting symbol or (symbol value) in enum\n");
        exit(0);
      }
    }
    arg_list=CDR(arg_list);
    x++;
      }
      current_space=sp;
    } break;
    case SYS_FUNC_QUIT:
    {
      exit(0);
    } break;
    case SYS_FUNC_EVAL:
    {
      ret=eval(eval(CAR(arg_list)));
    } break;
    case SYS_FUNC_BREAK: lbreak("User break"); break;
    case SYS_FUNC_MOD:
    {
      long x=lnumber_value(eval(CAR(arg_list))); arg_list=CDR(arg_list);
      long y=lnumber_value(eval(CAR(arg_list)));
      if (y==0) { lbreak("mod : division by zero\n"); y=1; }
      ret=new_lisp_number(x%y);
    } break;
/*    case SYS_FUNC_WRITE_PROFILE:
    {
      char *fn=lstring_value(eval(CAR(arg_list)));
      FILE *fp=fopen(fn, "wb");
      if (!fp)
        lbreak("could not open %s for writing", fn);
      else
      {    
    for (void *s=symbol_list;s;s=CDR(s))        
      fprintf(fp, "%8d  %s\n", ((lisp_symbol *)(CAR(s)))->call_counter,
          lstring_value(((lisp_symbol *)(CAR(s)))->name));
    fclose(fp);
      }
    } break;*/
    case SYS_FUNC_FOR:
    {
      void *bind_var=CAR(arg_list); arg_list=CDR(arg_list);
      p_ref r1(bind_var);
      if (item_type(bind_var)!=L_SYMBOL)
      { lbreak("expecting for iterator to be a symbol\n"); exit(1); }

      if (CAR(arg_list)!=in_symbol)
      { lbreak("expecting in after 'for iterator'\n"); exit(1); }
      arg_list=CDR(arg_list);

      void *ilist=eval(CAR(arg_list)); arg_list=CDR(arg_list);
      p_ref r2(ilist);

      if (CAR(arg_list)!=do_symbol)
      { lbreak("expecting do after 'for iterator in list'\n"); exit(1); }
      arg_list=CDR(arg_list);

      void *block=NULL, *ret=NULL;
      p_ref r3(block);
      l_user_stack.push(symbol_value(bind_var));  // save old symbol value
      while (ilist)
      {
                set_symbol_value(bind_var, CAR(ilist));
                for (block=arg_list;block;block=CDR(block))
                  ret=eval(CAR(block));
                ilist=CDR(ilist);
      }
      set_symbol_value(bind_var, l_user_stack.pop(1));
      ret=ret;
    } break;
    case SYS_FUNC_OPEN_FILE:
    {
      bFILE *old_file=current_print_file;
      void *str1=eval(CAR(arg_list));
      p_ref r1(str1);
      void *str2=eval(CAR(CDR(arg_list)));


      current_print_file=open_file(lstring_value(str1),
                   lstring_value(str2));

      if (!current_print_file->open_failure())
      {
                while (arg_list)
                {
                  ret=eval(CAR(arg_list));    
                  arg_list=CDR(arg_list);
                }
      }
      delete current_print_file;
      current_print_file=old_file;

    } break;
    case SYS_FUNC_BIT_AND:
    {
      long first=lnumber_value(eval(CAR(arg_list))); arg_list=CDR(arg_list);
      while (arg_list)
      {
        first&=lnumber_value(eval(CAR(arg_list)));
                arg_list=CDR(arg_list);
      }
      ret=new_lisp_number(first);
    } break;
    case SYS_FUNC_BIT_OR:
    {
      long first=lnumber_value(eval(CAR(arg_list))); arg_list=CDR(arg_list);
      while (arg_list)
      {
        first|=lnumber_value(eval(CAR(arg_list)));
                arg_list=CDR(arg_list);
      }
      ret=new_lisp_number(first);
    } break;
    case SYS_FUNC_BIT_XOR:
    {
      long first=lnumber_value(eval(CAR(arg_list))); arg_list=CDR(arg_list);
      while (arg_list)
      {
        first^=lnumber_value(eval(CAR(arg_list)));
                arg_list=CDR(arg_list);
      }
      ret=new_lisp_number(first);
    } break;
    case SYS_FUNC_MAKE_ARRAY:
    {
      long l=lnumber_value(eval(CAR(arg_list)));
      if (l>=2<<16 || l<=0)
      {
                lbreak("bad array size %d\n", l);
                exit(0);
      }
      ret=new_lisp_1d_array(l, CDR(arg_list));
    } break;
    case SYS_FUNC_AREF:
    {
      long x=lnumber_value(eval(CAR(CDR(arg_list))));
      ret=lget_array_element(eval(CAR(arg_list)), x);
    } break;
    case SYS_FUNC_IF_1PROGN:
    {
      if (eval(CAR(arg_list)))
        ret=eval_block(CAR(CDR(arg_list)));
      else ret=eval(CAR(CDR(CDR(arg_list))));

    } break;
    case SYS_FUNC_IF_2PROGN:
    {
      if (eval(CAR(arg_list)))
        ret=eval(CAR(CDR(arg_list)));
      else ret=eval_block(CAR(CDR(CDR(arg_list))));

    } break;
    case SYS_FUNC_IF_12PROGN:
    {
      if (eval(CAR(arg_list)))
        ret=eval_block(CAR(CDR(arg_list)));
      else ret=eval_block(CAR(CDR(CDR(arg_list))));

    } break;
    case SYS_FUNC_EQ0:
    {
      void *v=eval(CAR(arg_list));
      if (item_type(v)!=L_NUMBER || (((lisp_number *)v)->num!=0))
        ret=NULL;
      else ret=true_symbol;
    } break;
    case SYS_FUNC_PREPORT:
    {
#ifdef L_PROFILE
      char *s=lstring_value(eval(CAR(arg_list)));
      preport(s);
#endif
    } break;
    case SYS_FUNC_SEARCH:
    {
      void *arg1=eval(CAR(arg_list)); arg_list=CDR(arg_list);
      p_ref r1(arg1);       // protect this refrence
      char *haystack=lstring_value(eval(CAR(arg_list)));
      char *needle=lstring_value(arg1);

      char *find=strstr(haystack, needle);
      if (find)
        ret=new_lisp_number(find-haystack);
      else ret=NULL;
    } break;
    case SYS_FUNC_ELT:
    {
      void *arg1=eval(CAR(arg_list)); arg_list=CDR(arg_list);
      p_ref r1(arg1);       // protect this refrence
      long x=lnumber_value(eval(CAR(arg_list)));
      char *st=lstring_value(arg1);
      if (x < 0 || (unsigned)x >= strlen(st))
      { lbreak("elt : out of range of string\n"); ret=NULL; }
      else
        ret=new_lisp_character(st[x]);
    } break;
    case SYS_FUNC_LISTP:
    {
      return item_type(eval(CAR(arg_list)))==L_CONS_CELL ? true_symbol : NULL;
    } break;
    case SYS_FUNC_NUMBERP:
    {
      int t=item_type(eval(CAR(arg_list)));
      if (t==L_NUMBER || t==L_FIXED_POINT) return true_symbol; else return NULL;
    } break;
    case SYS_FUNC_DO:
    {
      void *init_var=CAR(arg_list);
      p_ref r1(init_var);
      int i, ustack_start=l_user_stack.son;      // restore stack at end
      void *sym=NULL;
      p_ref r2(sym);

      // check to make sure iter vars are symbol and push old values
      for (init_var=CAR(arg_list);init_var;init_var=CDR(init_var))
      {
                sym=CAR(CAR(init_var));
                if (item_type(sym)!=L_SYMBOL)
                { lbreak("expecting symbol name for iteration var\n"); exit(0); }
                l_user_stack.push(symbol_value(sym));
      }

      void **do_evaled=l_user_stack.sdata+l_user_stack.son;
      // push all of the init forms, so we can set the symbol
      for (init_var=CAR(arg_list);init_var;init_var=CDR(init_var))
                l_user_stack.push(eval(CAR(CDR(CAR((init_var))))));

      // now set all the symbols
      for (init_var=CAR(arg_list);init_var;init_var=CDR(init_var), do_evaled++)
      {
                sym=CAR(CAR(init_var));
                set_symbol_value(sym, *do_evaled);
      }

      i=0;       // set i to 1 when terminate conditions are meet
      do
      {
                i=(eval(CAR(CAR(CDR(arg_list))))!=NULL);
                if (!i)
                {
                  eval_block(CDR(CDR(arg_list)));
                  for (init_var=CAR(arg_list);init_var;init_var=CDR(init_var))
                    eval(CAR(CDR(CDR(CAR(init_var)))));
                }
      } while (!i);

      ret=eval(CAR(CDR(CAR(CDR(arg_list)))));

      // restore old values for symbols
      do_evaled=l_user_stack.sdata+ustack_start;
      for (init_var=CAR(arg_list);init_var;init_var=CDR(init_var), do_evaled++)
      {
                sym=CAR(CAR(init_var));
                set_symbol_value(sym, *do_evaled);
      }

      l_user_stack.son=ustack_start;

    } break;
    case SYS_FUNC_GC:
    {
      collect_space(current_space);
    } break;
    case SYS_FUNC_SCHAR:
    {
      char *s=lstring_value(eval(CAR(arg_list)));      arg_list=CDR(arg_list);
      long x=lnumber_value(eval(CAR(arg_list)));

      if ((unsigned)x >= strlen(s))
      { lbreak("SCHAR: index %d should be less than the length of the string\n", x); exit(0); }
      else if (x<0)
      { lbreak("SCHAR: index should not be negative\n"); exit(0); }
      return new_lisp_character(s[x]);
    } break;
    case SYS_FUNC_SYMBOLP:
    { if (item_type(eval(CAR(arg_list)))==L_SYMBOL) return true_symbol;
      else return NULL; } break;
    case SYS_FUNC_NUM2STR:
    {
      char str[20];
      sprintf(str, "%ld", (long int)lnumber_value(eval(CAR(arg_list))));
      ret=new_lisp_string(str);
    } break;
    case SYS_FUNC_NCONC:
    {
      void *l1=eval(CAR(arg_list)); arg_list=CDR(arg_list);
      p_ref r1(l1);
      void *first=l1, *next;
      p_ref r2(first);

      if (!l1)
      {
                l1=first=eval(CAR(arg_list));
                arg_list=CDR(arg_list);
      }

      if (item_type(l1)!=L_CONS_CELL)
      { lprint(l1); lbreak("first arg should be a list\n"); }
      do
      {
                next=l1;
                while (next) { l1=next; next=lcdr(next); }
                ((cons_cell *)l1)->cdr=eval(CAR(arg_list));    
                arg_list=CDR(arg_list);
      } while (arg_list);
      ret=first;
    } break;
    case SYS_FUNC_FIRST:
    { ret=CAR(eval(CAR(arg_list))); } break;
    case SYS_FUNC_SECOND:
    { ret=CAR(CDR(eval(CAR(arg_list)))); } break;
    case SYS_FUNC_THIRD:
    { ret=CAR(CDR(CDR(eval(CAR(arg_list))))); } break;
    case SYS_FUNC_FOURTH:
    { ret=CAR(CDR(CDR(CDR(eval(CAR(arg_list)))))); } break;
    case SYS_FUNC_FIFTH:
    { ret=CAR(CDR(CDR(CDR(CDR(eval(CAR(arg_list))))))); } break;
    case SYS_FUNC_SIXTH:
    { ret=CAR(CDR(CDR(CDR(CDR(CDR(eval(CAR(arg_list)))))))); } break;
    case SYS_FUNC_SEVENTH:
    { ret=CAR(CDR(CDR(CDR(CDR(CDR(CDR(eval(CAR(arg_list))))))))); } break;
    case SYS_FUNC_EIGHTH:
    { ret=CAR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(eval(CAR(arg_list)))))))))); } break;
    case SYS_FUNC_NINTH:
    { ret=CAR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(eval(CAR(arg_list))))))))))); } break;
    case SYS_FUNC_TENTH:
    { ret=CAR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(eval(CAR(arg_list)))))))))))); } break;
    case SYS_FUNC_SUBSTR:
    {
      long x1=lnumber_value(eval(CAR(arg_list))); arg_list=CDR(arg_list);
      long x2=lnumber_value(eval(CAR(arg_list))); arg_list=CDR(arg_list);
      void *st=eval(CAR(arg_list));
      p_ref r1(st);

      if (x1 < 0 || x1 > x2 || (unsigned)x2 >= strlen(lstring_value(st)))
        lbreak("substr : bad x1 or x2 value");

      lisp_string *s=new_lisp_string(x2-x1+2);
      if (x2-x1)
        memcpy(lstring_value(s), lstring_value(st)+x1, x2-x1+1);

      *(lstring_value(s)+(x2-x1+1))=0;
      ret=s;
    } break;
    case 99 :
    {
      void *r=NULL, *rstart=NULL;
      p_ref r1(r), r2(rstart);
      while (arg_list)
      {
        void *q=eval(CAR(arg_list));
        if (!rstart) rstart=q;
        while (r && CDR(r)) r=CDR(r);
        CDR(r)=q;    
        arg_list=CDR(arg_list);
      }
      return rstart;
    } break;

    default :
    { dprintf("Undefined system function number %d\n", ((lisp_sys_function *)fun)->fun_number); }
  }
  return ret;
}

void tmp_space()
{
  current_space=TMP_SPACE;
}

void perm_space()
{
  current_space=PERM_SPACE;
}

void use_user_space(void *addr, long size)
{
  current_space = 2;
  free_space[USER_SPACE] = space[USER_SPACE] = (uint8_t *)addr;
  space_size[USER_SPACE] = size;
}


void *eval_user_fun(lisp_symbol *sym, void *arg_list)
{
  void *ret=NULL;
  p_ref ref1(ret);

#ifdef TYPE_CHECKING
  if (item_type(sym)!=L_SYMBOL)
  {
    lprint(sym);
    lbreak("EVAL : is not a function name (not symbol either)");
    exit(0);
  }
#endif
#ifdef L_PROFILE
  time_marker start;
#endif


  lisp_user_function *fun=(lisp_user_function *)(((lisp_symbol *)sym)->function);

#ifdef TYPE_CHECKING
  if (item_type(fun)!=L_USER_FUNCTION)
  {
    lprint(sym);
    lbreak("is not a user defined function\n");
  }
#endif

#ifndef NO_LIBS
  void *fun_arg_list=cache.lblock(fun->alist);
  void *block_list=cache.lblock(fun->blist);
  p_ref r9(block_list), r10(fun_arg_list);
#else
  void *fun_arg_list=fun->arg_list;
  void *block_list=fun->block_list;
  p_ref r9(block_list), r10(fun_arg_list);
#endif



  // mark the start start, so we can restore when done
  long stack_start=l_user_stack.son;

  // first push all of the old symbol values
  void *f_arg=fun_arg_list;
  p_ref r18(f_arg);
  p_ref r19(arg_list);
  for (;f_arg;f_arg=CDR(f_arg))
  {
    lisp_symbol *s = (lisp_symbol *)CAR(f_arg);
    l_user_stack.push(s->value);
  }

  // open block so that local vars aren't saved on the stack
  {
    int new_start=l_user_stack.son;
    int i=new_start;
    // now push all the values we wish to gather
    for (f_arg=fun_arg_list;f_arg;)
    {
      if (!arg_list)
      { lprint(sym);  lbreak("too few parameter to function\n"); exit(0); }
      l_user_stack.push(eval(CAR(arg_list)));
      f_arg=CDR(f_arg);
      arg_list=CDR(arg_list);
    }


    // now store all the values and put them into the symbols
    for (f_arg=fun_arg_list;f_arg;f_arg=CDR(f_arg))
      ((lisp_symbol *)CAR(f_arg))->value=l_user_stack.sdata[i++];

    l_user_stack.son=new_start;
  }



  if (f_arg)
  { lprint(sym);  lbreak("too many parameter to function\n"); exit(0); }


  // now evaluate the function block
  while (block_list)
  {
    ret=eval(CAR(block_list));
    block_list=CDR(block_list);
  }

  long cur_stack=stack_start;
  for (f_arg=fun_arg_list;f_arg;f_arg=CDR(f_arg))
    ((lisp_symbol *)CAR(f_arg))->value=l_user_stack.sdata[cur_stack++];

  l_user_stack.son=stack_start;

#ifdef L_PROFILE
  time_marker end;
  ((lisp_symbol *)sym)->time_taken+=end.diff_time(&start);
#endif


  return ret;
}





void *eval(void *prog)
{


  void *ret=NULL;
  p_ref ref1(prog);


  int tstart=trace_level;

  if (trace_level)
  {
    if (trace_level<=trace_print_level)
    {
      dprintf("%d (%d, %d, %d) TRACE : ", trace_level,
          space_size[PERM_SPACE]-((char *)free_space[PERM_SPACE]-(char *)space[PERM_SPACE]),
          space_size[TMP_SPACE]-((char *)free_space[TMP_SPACE]-(char *)space[TMP_SPACE]),
          l_ptr_stack.son);
      lprint(prog);

      dprintf("\n");
    }
    trace_level++;
  }
  if (prog)
  {
    switch (item_type(prog))
    {
      case L_BAD_CELL :
      { lbreak("error : eval on a bad cell\n"); exit(0); } break;
      case L_CHARACTER :
      case L_STRING :
      case L_NUMBER :
      case L_POINTER :
      case L_FIXED_POINT :
      { ret=prog; } break;
      case L_SYMBOL :
      { if (prog==true_symbol)
                  ret=prog;
        else
                {
                  ret=lookup_symbol_value(prog);
                  if (item_type(ret)==L_OBJECT_VAR)
                    ret=l_obj_get(((lisp_object_var *)ret)->number);
                }
      } break;
      case L_CONS_CELL :
      {
        ret=eval_function((lisp_symbol *)CAR(prog), CDR(prog));
      }
      break;
      default :
        fprintf(stderr, "shouldn't happen\n");
    }
  }
  if (tstart)
  {
    trace_level--;
    if (trace_level<=trace_print_level)
      dprintf("%d (%d, %d, %d) TRACE ==> ", trace_level,
          space_size[PERM_SPACE]-((char *)free_space[PERM_SPACE]-(char *)space[PERM_SPACE]),
          space_size[TMP_SPACE]-((char *)free_space[TMP_SPACE]-(char *)space[TMP_SPACE]),
          l_ptr_stack.son);
    lprint(ret);
    dprintf("\n");
  }

/*  l_user_stack.push(ret);
  collect_space(PERM_SPACE);
  ret=l_user_stack.pop(1);  */


  return ret;
}

int total_symbols()
{
  return ltotal_syms;
}

void resize_perm(int new_size)
{
  if (new_size<((char *)free_space[PERM_SPACE]-(char *)space[PERM_SPACE]))
  {
    lbreak("resize perm : %d is to small to hold current heap\n", new_size);
    exit(0);
  } else if (new_size>space_size[PERM_SPACE])
  {
    lbreak("Only smaller resizes allowed for now.\n");
    exit(0);
  } else
    dprintf("doesn't work yet!\n");
}

void resize_tmp(int new_size)
{
  if (new_size<((char *)free_space[TMP_SPACE]-(char *)space[TMP_SPACE]))
  {
    lbreak("resize perm : %d is to small to hold current heap\n", new_size);
    exit(0);
  } else if (new_size>space_size[TMP_SPACE])
  {
    printf("Only smaller resizes allowed for now.\n");
    exit(0);
  } else if (free_space[TMP_SPACE]==space[TMP_SPACE])
  {
    free_space[TMP_SPACE] = space[TMP_SPACE] = (uint8_t *)realloc(space[TMP_SPACE], new_size);
    space_size[TMP_SPACE] = new_size;
    dprintf("Lisp : tmp space resized to %d\n", new_size);
  } else dprintf("Lisp :tmp not empty, cannot resize\n");
}

void l_comp_init();
void lisp_init(long perm_size, long tmp_size)
{
  unsigned int i;
  lsym_root = NULL;
  total_user_functions = 0;

  free_space[0] = space[0] = (uint8_t *)malloc(perm_size);
  space_size[0] = perm_size;

  free_space[1] = space[1] = (uint8_t *)malloc(tmp_size);
  space_size[1] = tmp_size;


  current_space=PERM_SPACE;


  l_comp_init();
  for(i = 0; i < sizeof(sys_funcs) / sizeof(*sys_funcs); i++)
    add_sys_function(sys_funcs[i].name,
                     sys_funcs[i].min_args, sys_funcs[i].max_args, i);
  clisp_init();
  current_space=TMP_SPACE;
  dprintf("Lisp : %d symbols defined, %d system functions, %d pre-compiled functions\n",
      total_symbols(), sizeof(sys_funcs) / sizeof(*sys_funcs), total_user_functions);
}

void lisp_uninit()
{
  free(space[0]);
  free(space[1]);
  ldelete_syms(lsym_root);
  lsym_root=NULL;
  ltotal_syms=0;
}

void clear_tmp()
{
  free_space[TMP_SPACE]=space[TMP_SPACE];
}

void *symbol_name(void *symbol)
{
  return ((lisp_symbol *)symbol)->name;
}


void *set_symbol_number(void *symbol, long num)
{
#ifdef TYPE_CHECKING
  if (item_type(symbol)!=L_SYMBOL)
  {
    lprint(symbol);
    lbreak("is not a symbol\n");
    exit(0);
  }
#endif
  if (((lisp_symbol *)symbol)->value!=l_undefined &&
      item_type(((lisp_symbol *)symbol)->value)==L_NUMBER)
    ((lisp_number *)((lisp_symbol *)symbol)->value)->num=num;
  else
    ((lisp_symbol *)(symbol))->value=new_lisp_number(num);

  return ((lisp_symbol *)(symbol))->value;
}

void *set_symbol_value(void *symbol, void *value)
{
#ifdef TYPE_CHECKING
  if (item_type(symbol)!=L_SYMBOL)
  {
    lprint(symbol);
    lbreak("is not a symbol\n");
    exit(0);
  }
#endif
  ((lisp_symbol *)(symbol))->value=value;
  return value;
}

void *symbol_function(void *symbol)
{
#ifdef TYPE_CHECKING
  if (item_type(symbol)!=L_SYMBOL)
  {
    lprint(symbol);
    lbreak("is not a symbol\n");
    exit(0);
  }
#endif
  return ((lisp_symbol *)symbol)->function;
}

void *symbol_value(void *symbol)
{
#ifdef TYPE_CHECKING
  if (item_type(symbol)!=L_SYMBOL)
  {
    lprint(symbol);
    lbreak("is not a symbol\n");
    exit(0);
  }
#endif
  return ((lisp_symbol *)symbol)->value;
}






