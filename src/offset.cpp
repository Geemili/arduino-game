
#include "offset.h"

Pos get_pos_offset(Pos pos, Offset offset) {
  uint8_t x = pos.x;
  uint8_t y = pos.y;
  if (-offset.x > pos.x) x = 0; else x += offset.x;
  if (-offset.y > pos.y) y = 0; else y += offset.y;
  return Pos{x, y};
}
