#ifndef __PROPERTY_HPP_
#define __PROPERTY_HPP_

class property;
class property_manager
{
  property *first;
  property *find(char const *name);
  public :
  property_manager() { first=0; } 
  void load(char const *filename);
  void save(char const *filename);
  
  int getd(char const *name, int def) { return (int)get(name,def); }
  int get(char const *name, int def);
  char const *get(char const *name, char const *def);

  void setd(char const *name, int def) { set(name,def); }
  void set(char const *name, double def);
  void set(char const *name, char const *def);
  ~property_manager();
} ;


#endif
