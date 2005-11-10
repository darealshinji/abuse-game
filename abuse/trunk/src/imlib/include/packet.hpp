#ifndef __PACKET_HPP_
#define __PACKET_HPP_
#include "macs.hpp"
class packet
{
  public :
  uint8_t *buf;
  int32_t buf_size,ro,wo,rend;
  int pre_size;
  void make_bigger(int max);

  int get_read_position() { return ro; }
  void set_read_position(int x) { ro=x; }
  int read(uint8_t *buffer, int size);
  int write(uint8_t *buffer, int size);
  int eop() { return ro>=rend; }
  void reset();
  packet(int prefix_size=2);
  void get_string(char *st, int len);
  int advance(int32_t offset);

  void write_uint32(uint32_t x);      // writes can't fail...
  void write_uint16(uint16_t x);
  void write_uint8(uint8_t x);
  void insert_into(packet &pk);
  int size() { return rend-pre_size; }
  ~packet();
} ;



#endif
