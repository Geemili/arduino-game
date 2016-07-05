
#ifndef __DIRECTION_H__
#define __DIRECTION_H__

#include "offset.h"

namespace direction {
  enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST
  };

  // Gets the offset by the direction
  Offset offset(Direction dir);
}

#endif
