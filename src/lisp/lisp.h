/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __LISP_HPP_
#define __LISP_HPP_

#include <cstdlib>
#include <stdint.h>

#ifdef L_PROFILE
#include "timing.h"
#endif

#define Cell void
#define MAX_LISP_TOKEN_LEN 200
enum { PERM_SPACE,
       TMP_SPACE,
       USER_SPACE,
       GC_SPACE };

#define FIXED_TRIG_SIZE 360               // 360 degrees stored in table
extern int32_t sin_table[FIXED_TRIG_SIZE];   // this should be filled in by external module
#define TBS 1662                          // atan table granularity
extern uint16_t atan_table[TBS];
#define NILP(x) (x==NULL)
#define DEFINEDP(x) (x!=l_undefined)
class bFILE;
extern int current_space;
extern bFILE *current_print_file;


enum { L_BAD_CELL,   // error catching type
       L_CONS_CELL, L_NUMBER, L_SYMBOL,     L_SYS_FUNCTION, L_USER_FUNCTION,
       L_STRING, L_CHARACTER, L_C_FUNCTION, L_C_BOOL,       L_L_FUNCTION, L_POINTER,
       L_OBJECT_VAR, L_1D_ARRAY,
       L_FIXED_POINT, L_COLLECTED_OBJECT };

typedef uint32_t ltype;    // make sure structures aren't packed differently on various compiler
                       // and sure that word, etc are word aligned

struct LispObject
{
    ltype type;
};

struct LispObjectVar : LispObject
{
    long number;
};

struct LispList : LispObject
{
    static LispList *Create();

    size_t GetLength();

    LispObject *cdr, *car;
};

struct LispNumber : LispObject
{
    static LispNumber *Create(long num);

    long num;
};

struct LispRedirect : LispObject
{
    LispObject *new_reference;
};

struct LispString : LispObject
{
    static LispString *Create(char const *string);
    static LispString *Create(char const *string, int length);
    static LispString *Create(int length);

    char *GetString();

    char str[1];
};

struct LispSymbol : LispObject
{
    static LispSymbol *Find(char const *name);
    static LispSymbol *FindOrCreate(char const *name);

    LispString *GetName();
    LispObject *GetFunction();
    LispObject *GetValue();

    void SetFunction(LispObject *fun);
    void SetValue(LispObject *value);
    void SetNumber(long num);

#ifdef L_PROFILE
    float time_taken;
#endif
    LispObject *value;
    LispObject *function;
    LispString *name;
    LispSymbol *left, *right; // tree structure
};

struct LispSysFunction : LispObject
{
    short min_args, max_args;
    short fun_number;
};

struct LispUserFunction : LispObject
{
#ifndef NO_LIBS
    intptr_t alist, blist;      // id for cached blocks
#else
    void *arg_list, *block_list;
#endif
};

struct LispArray : LispObject
{
    static LispArray *Create(int size, void *rest);

    inline LispObject **GetData() { return data; }
    LispObject *Get(long x);

    unsigned short size;
    // size * sizeof (void *) follows1

private:
    LispObject *data[1];
};

struct LispChar : LispObject
{
    int16_t pad;
    uint16_t ch;
};

struct LispPointer : LispObject
{
    void *addr;
};


struct LispFixedPoint : LispObject
{
    int32_t x;
};

static inline LispObject *&CAR(void *x) { return ((LispList *)x)->car; }
static inline LispObject *&CDR(void *x) { return ((LispList *)x)->cdr; }
static inline ltype item_type(void *x) { if (x) return *(ltype *)x; return L_CONS_CELL; }

void perm_space();
void tmp_space();
void use_user_space(void *addr, long size);
void *lpointer_value(void *lpointer);
int32_t lnumber_value(void *lnumber);
unsigned short lcharacter_value(void *c);
long lfixed_point_value(void *c);
void *lisp_atom(void *i);
void *lcdr(void *c);
void *lcar(void *c);
void *lisp_eq(void *n1, void *n2);
void *lisp_equal(void *n1, void *n2);
void lprint(void *i);
void *eval(void *prog);
void *eval_block(void *list);
void *eval_function(LispSymbol *sym, void *arg_list);
void *eval_user_fun(LispSymbol *sym, void *arg_list);
void *compile(char const *&s);
void *assoc(void *item, void *list);
void resize_tmp(int new_size);
void resize_perm(int new_size);

void push_onto_list(void *object, void *&list);
LispSymbol *add_c_object(void *symbol, int16_t number);
LispSymbol *add_c_function(char const *name, short min_args, short max_args, short number);
LispSymbol *add_c_bool_fun(char const *name, short min_args, short max_args, short number);
LispSymbol *add_lisp_function(char const *name, short min_args, short max_args, short number);
int read_ltoken(char *&s, char *buffer);
void print_trace_stack(int max_levels);


LispPointer *new_lisp_pointer(void *addr);
LispChar *new_lisp_character(uint16_t ch);
LispFixedPoint *new_lisp_fixed_point(int32_t x);
LispObjectVar *new_lisp_object_var(int16_t number);
LispSysFunction *new_lisp_sys_function(int min_args, int max_args, int fun_number);
LispSysFunction *new_lisp_c_function(int min_args, int max_args, int fun_number);
LispSysFunction *new_lisp_c_bool(int min_args, int max_args, int fun_number);

#ifdef NO_LIBS
LispUserFunction *new_lisp_user_function(void *arg_list, void *block_list);
#else
LispUserFunction *new_lisp_user_function(intptr_t arg_list, intptr_t block_list);
#endif

LispSysFunction *new_user_lisp_function(int min_args, int max_args, int fun_number);

int end_of_program(char *s);
void clear_tmp();
void lisp_init(long perm_size, long tmp_size);
void lisp_uninit();
extern LispSymbol *lsym_root;

extern uint8_t *space[4], *free_space[4];
extern int space_size[4];
void *nth(int num, void *list);
int32_t lisp_atan2(int32_t dy, int32_t dx);
int32_t lisp_sin(int32_t x);
int32_t lisp_cos(int32_t x);
void restore_heap(void *val, int heap);
void *mark_heap(int heap);

extern "C" {
void lbreak(const char *format, ...);
} ;

extern void clisp_init();                      // external initalizer call by lisp_init()
extern long c_caller(long number, void *arg);  // exten c function switches on number
extern void *l_caller(long number, void *arg);  // exten lisp function switches on number

extern void *l_obj_get(long number);  // exten lisp function switches on number
extern void l_obj_set(long number, void *arg);  // exten lisp function switches on number
extern void l_obj_print(long number);  // exten lisp function switches on number

// FIXME: get rid of this later
static inline void *symbol_value(void *sym) { return ((LispSymbol *)sym)->GetValue(); }
static inline char *lstring_value(void *str) { return ((LispString *)str)->GetString(); }

#include "lisp_opt.h"

#endif
