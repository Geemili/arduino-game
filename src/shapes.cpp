
#include "shapes.h"

bool shapes::shape_fits(shapes::CrateShape c, shapes::WallShape w) {
    switch (c) {
      case CRATE_SMALL: return true;
      case CRATE_VERTICAL: return w == WALL_VERTICAL;
      case CRATE_HORIZONTAL: return w == WALL_HORIZONTAL;
    }
}
