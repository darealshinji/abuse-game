#include "lisp.hpp"
#include "specs.hpp"
#include "bus_type.hpp"
#include <crtdbg.h>

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
long block_size(Cell *level)  // return size needed to recreate this block
{
    int ret;
    if (!level) 
        ret = 0;    // NULL pointers don't need to be stored
    else {
        int type = item_type(level);
        if (type == L_CONS_CELL) {
	        long t = 0; 
	        void *b;
            for (b = level; b && item_type(b) == L_CONS_CELL; b = CDR(b)) {
	            t += sizeof(cons_cell);
            }
	        if (b)
                t += block_size(b);
	        for (b = level; b && item_type(b) == L_CONS_CELL; b = CDR(b))
	            t += block_size(CAR(b));
	        ret = t;
        }
        else if (type == L_NUMBER) {
            ret = sizeof(lisp_number); 
        } 
        else if (type == L_CHARACTER) {
            ret = sizeof(lisp_character);
        }
        else if (type == L_STRING) {
            ret = sizeof(lisp_string) + strlen(lstring_value(level)) + 1; 
            if (ret < 8)
                ret = 8;
        }
        else if (type==L_POINTER) {
            ret = sizeof(lisp_pointer);
        }
        else 
            ret=0;
    }
#ifdef WORD_ALLIGN
    return (ret + 3) & (~3);
#else
    return ret;
#endif
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void write_level(bFILE *fp, Cell *level)
{
    int type = item_type(level);
    fp->write_byte(type);

#ifdef __DEBUG_CACHE
    _RPT1(_CRT_WARN, "write_level(bFILE, Cell) File Position: %d  ", fp->tell());
#endif
    switch (type) {
    case L_NUMBER :
#ifdef __DEBUG_CACHE
        _RPT1(_CRT_WARN, "Cached L_NUMBER value: %d\n", lnumber_value(level));
#endif
        fp->write_long(lnumber_value(level)); 
        break;
    case L_CHARACTER :
#ifdef __DEBUG_CACHE
        _RPT1(_CRT_WARN, "Cached L_CHARACTER value: %c\n", lcharacter_value(level));
#endif
        fp->write_short(lcharacter_value(level));
        break;
    case L_STRING : {
#ifdef __DEBUG_CACHE
            _RPT1(_CRT_WARN, "Cached L_STRING value: \"%s\"\n", lstring_value(level));
#endif
            long l = strlen(lstring_value(level)) + 1;
            fp->write_long(l);
            fp->write(lstring_value(level), l); 
        }
        break;
    case L_SYMBOL :
#ifdef __DEBUG_CACHE
        _RPT2(_CRT_WARN, "Cached L_SYMBOL value: %d, name: \"%s\"\n", (long) level, (char*) ((lisp_symbol*) level)->name+4);
#endif
        fp->write_long((long) level); 
        break;
    case L_CONS_CELL : {
#ifdef __DEBUG_CACHE
            _RPT0(_CRT_WARN, "Caching L_CONS_CELL\n");
#endif
            if (!level)
                fp->write_long(0);
            else {
	            long t = 0;
	            void *b;
	            for (b = level; b && item_type(b) == L_CONS_CELL; b = CDR(b))
                    t++;
                if (b) {
	                fp->write_long(-t);      // negative number means dotted list
	                write_level(fp,b);       // save end of dotted list     
                }
	            else 
                    fp->write_long(t);

	            for (b = level; b && item_type(b) == L_CONS_CELL; b = CDR(b))    
	                write_level(fp, CAR(b));
            }
        } 
        break;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
Cell *load_block(bFILE *fp)
{
    int type = fp->read_byte();
    switch (type) {
    case L_NUMBER : {
            lisp_number *ret = new_lisp_number(fp->read_long());
#ifdef __DEBUG_CACHE
            _RPT1(_CRT_WARN, "Loaded L_NUMBER from cache: %d\n", ret->num);
#endif
            return ret;
            //return new_lisp_number(fp->read_long()); 
            break;
        }
    case L_CHARACTER : {
            lisp_character *ret = new_lisp_character(fp->read_short());
#ifdef __DEBUG_CACHE
            _RPT1(_CRT_WARN, "Loaded L_CHARACTER from cache: %c\n", ret->ch);
#endif
            return ret;
            //return new_lisp_character(fp->read_short());
            break;
        }
    case L_STRING : {
            long l = fp->read_long();
            lisp_string *s = new_lisp_string(l);
            fp->read(lstring_value(s), l);
#ifdef __DEBUG_CACHE
            _RPT1(_CRT_WARN, "Loaded L_STRING from cache: \"%s\"\n", lstring_value(s));
#endif
            return s;
        } 
        break;
    case L_SYMBOL : {
            lisp_symbol *ret = (lisp_symbol*) fp->read_long();
#ifdef __DEBUG_CACHE
            _RPT2(_CRT_WARN, "Loaded L_SYMBOL from cache: %d, name: \"%s\"\n", (long) ret, (char*) ret->name+4);
#endif
            return ret;
            //return (void *)fp->read_long(); 
            break;
        }
    case L_CONS_CELL : {
#ifdef __DEBUG_CACHE
           _RPT0(_CRT_WARN, "Loading L_CONS_CELL from cache\n");            
#endif
            long t = fp->read_long();
            if (!t)
                return NULL;
            else {
	            long x = abs(t);
	            cons_cell *last, *first = NULL;
                while (x) {
	                cons_cell *c = new_cons_cell();
	                if (first)
	                    last->cdr = c;
	                else 
                        first = c;
	                last = c;
	                x--;
                }
	            if (t < 0)	
	                last->cdr = (cons_cell*) load_block(fp);
	            else 
                    last->cdr = NULL;
	            for (last = first, x = 0; x < abs(t); x++, last = (cons_cell *)last->cdr)       
	                last->car = (cons_cell*) load_block(fp);	
	            return first;
            }
        }
    } // End Switch
    return NULL;
}
