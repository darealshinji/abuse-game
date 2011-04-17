/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "lisp.h"
#include "lisp_gc.h"
#ifdef NO_LIBS
#include "fakelib.h"
#else
#include "macs.h"
#endif

#include "stack.h"

/*  Lisp garbage collection: uses copy/free algorithm
    Places to check:
      symbol
        values
    functions
    names
      stack
*/

// Stack where user programs can push data and have it GCed
grow_stack<void> l_user_stack(150);
// Stack of user pointers
grow_stack<void *> PtrRef::stack(1500);

size_t reg_ptr_total = 0;
size_t reg_ptr_list_size = 0;
void ***reg_ptr_list = NULL;

static uint8_t *cstart, *cend, *collected_start, *collected_end;

static void dump_memory(void *mem, int before, int after)
{
  uint8_t *p = (uint8_t *)mem;

  fprintf(stderr, "dumping memory around %p:\n", p);
  for (int i = -before; i < after; i++)
  {
    if (!(i & 15))
      fprintf(stderr, "%p: ", p + i);
    fprintf(stderr, "%c%02x%c", i ? ' ' : '[', p[i], i ? ' ' : ']');
    if (!((i + 1) & 15))
      fprintf(stderr, "\n");
  }
}

void register_pointer(void **addr)
{
  if (reg_ptr_total >= reg_ptr_list_size)
  {
    reg_ptr_list_size += 0x100;
    reg_ptr_list = (void ***)realloc(reg_ptr_list, sizeof(void **) * reg_ptr_list_size);
  }
  reg_ptr_list[reg_ptr_total++] = addr;
}

void unregister_pointer(void **addr)
{
  void ***reg_on = reg_ptr_list;
  for (size_t i = 0; i < reg_ptr_total; i++, reg_on++)
  {
    if (*reg_on == addr)
    {
      reg_ptr_total--;
      for (size_t j = i; j < reg_ptr_total; j++, reg_on++)
        reg_on[0] = reg_on[1];
      return ;
    }
  }
  fprintf(stderr, "Unable to locate ptr to unregister");
}

static LObject *CollectObject(LObject *x);

static LArray *CollectArray(LArray *x)
{
    size_t s = x->len;
    LArray *a = LArray::Create(s, NULL);
    LObject **src = x->GetData();
    LObject **dst = a->GetData();
    for (size_t i = 0; i < s; i++)
        dst[i] = CollectObject(src[i]);

    return a;
}

inline LList *CollectList(LList *x)
{
    LList *last = NULL, *first = NULL;

    for (; x && item_type(x) == L_CONS_CELL; )
    {
        LList *p = LList::Create();
        LObject *old_car = x->car;
        LObject *old_cdr = x->cdr;
        LObject *old_x = x;
        x = (LList *)CDR(x);
        ((LRedirect *)old_x)->type = L_COLLECTED_OBJECT;
        ((LRedirect *)old_x)->ref = p;

        p->car = CollectObject(old_car);
        p->cdr = CollectObject(old_cdr);

        if (last)
            last->cdr = p;
        else
            first = p;
        last = p;
    }
    if (x)
        last->cdr = CollectObject(x);
    return first; // we already set the collection pointers
}

static LObject *CollectObject(LObject *x)
{
  LObject *ret = x;

  if (((uint8_t *)x) >= cstart && ((uint8_t *)x) < cend)
  {
    //dump_memory(x, 32, 48);
    switch (item_type(x))
    {
      case L_BAD_CELL:
        lbreak("error: GC corrupted cell\n");
        break;
      case L_NUMBER:
        ret = LNumber::Create(((LNumber *)x)->num);
        break;
      case L_SYS_FUNCTION:
        ret = new_lisp_sys_function(((LSysFunction *)x)->min_args,
                                    ((LSysFunction *)x)->max_args,
                                    ((LSysFunction *)x)->fun_number);
        break;
      case L_USER_FUNCTION:
#ifndef NO_LIBS
        ret = new_lisp_user_function(((LUserFunction *)x)->alist,
                                     ((LUserFunction *)x)->blist);

#else
        {
          LObject *arg = CollectObject(((LUserFunction *)x)->arg_list);
          LObject *block = CollectObject(((LUserFunction *)x)->block_list);
          ret = new_lisp_user_function(arg, block);
        }
#endif
        break;
      case L_STRING:
        ret = LString::Create(lstring_value(x));
        break;
      case L_CHARACTER:
        ret = LChar::Create(lcharacter_value(x));
        break;
      case L_C_FUNCTION:
        ret = new_lisp_c_function(((LSysFunction *)x)->min_args,
                                  ((LSysFunction *)x)->max_args,
                                  ((LSysFunction *)x)->fun_number);
        break;
      case L_C_BOOL:
        ret = new_lisp_c_bool(((LSysFunction *)x)->min_args,
                              ((LSysFunction *)x)->max_args,
                              ((LSysFunction *)x)->fun_number);
        break;
      case L_L_FUNCTION:
        ret = new_user_lisp_function(((LSysFunction *)x)->min_args,
                                     ((LSysFunction *)x)->max_args,
                                     ((LSysFunction *)x)->fun_number);
        break;
      case L_POINTER:
        ret = LPointer::Create(lpointer_value(x));
        break;
      case L_1D_ARRAY:
        ret = CollectArray((LArray *)x);
        break;
      case L_FIXED_POINT:
        ret = LFixedPoint::Create(lfixed_point_value(x));
        break;
      case L_CONS_CELL:
        ret = CollectList((LList *)x);
        break;
      case L_OBJECT_VAR:
        ret = LObjectVar::Create(((LObjectVar *)x)->index);
        break;
      case L_COLLECTED_OBJECT:
        ret = ((LRedirect *)x)->ref;
        break;
      default:
        dump_memory(x, 8, 196);
        //*(char *)NULL = 0;
        lbreak("shouldn't happen. collecting bad object 0x%x\n",
               item_type(x));
        break;
    }
    ((LRedirect *)x)->type = L_COLLECTED_OBJECT;
    ((LRedirect *)x)->ref = ret;
  }
  else if ((uint8_t *)x < collected_start || (uint8_t *)x >= collected_end)
  {
    if (item_type(x) == L_CONS_CELL) // still need to remap cons_cells outside of space
    {
      for (; x && item_type(x) == L_CONS_CELL; x = CDR(x))
        ((LList *)x)->car = CollectObject(((LList *)x)->car);
      if (x)
        ((LList *)x)->cdr = CollectObject(((LList *)x)->cdr);
    }
  }

  return ret;
}

static void collect_symbols(LSymbol *root)
{
    if (!root)
        return;

    root->value = CollectObject(root->value);
    root->function = CollectObject(root->function);
    root->name = (LString *)CollectObject(root->name);
    collect_symbols(root->left);
    collect_symbols(root->right);
}

static void collect_stacks()
{
  long t = l_user_stack.son;

  void **d = l_user_stack.sdata;
  for (int i = 0; i < t; i++, d++)
    *d = CollectObject((LObject *)*d);

  t = PtrRef::stack.son;
  void ***d2 = PtrRef::stack.sdata;
  for (int i = 0; i < t; i++, d2++)
  {
    void **ptr = *d2;
    *ptr = CollectObject((LObject *)*ptr);
  }

  d2 = reg_ptr_list;
  for (size_t i = 0; i < reg_ptr_total; i++, d2++)
  {
    void **ptr = *d2;
    *ptr = CollectObject((LObject *)*ptr);
  }
}

void collect_space(int which_space) // should be tmp or permanent
{
  return; /* XXX: temporary hack */

  int old_space = current_space;
  cstart = space[which_space];
  cend = free_space[which_space];

  space_size[GC_SPACE] = space_size[which_space];
  uint8_t *new_space = (uint8_t *)malloc(space_size[GC_SPACE]);
  current_space = GC_SPACE;
  free_space[GC_SPACE] = space[GC_SPACE] = new_space;

  collected_start = new_space;
  collected_end = new_space + space_size[GC_SPACE];

//dump_memory((char *)lsym_root->name, 128, 196);
//dump_memory((char *)0xb6782025, 32, 48);
  collect_symbols(LSymbol::root);
  collect_stacks();

  // for debuging clear it out
  memset(space[which_space], 0, space_size[which_space]);
  free(space[which_space]);

  space[which_space] = new_space;
  free_space[which_space] = new_space
                          + (free_space[GC_SPACE] - space[GC_SPACE]);
  current_space = old_space;
}

