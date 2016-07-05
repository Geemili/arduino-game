
#ifndef __OFFSET_H__
#define __OFFSET_H__

#include <Arduino.h>
#include "pos.h"

struct Offset {
  int8_t x;
  int8_t y;
};

Pos get_pos_offset(Pos pos, Offset offset);

#endif
