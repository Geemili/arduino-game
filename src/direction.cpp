
#include "direction.h"

Offset direction::offset(Direction dir) {
  switch (dir) {
    case NORTH: return Offset{0, -1};
    case EAST: return Offset{1, 0};
    case SOUTH: return Offset{0, 1};
    case WEST: return Offset{-1, 0};
  }
}
