#include "lisp.hpp"

char const *lang_string(char const *symbol)
{
  void *v=find_symbol(symbol);
  if (!v || !DEFINEDP(symbol_value(v))) return "Language symbol missing!";
  else return lstring_value(symbol_value(v));  
}

