
#include "shapes.h"

bool shapes::shape_fits(shapes::CrateShape c, shapes::SlotShape w) {
    switch (c) {
      case CRATE_SMALL: return true;
      case CRATE_VERTICAL: return w == SLOT_VERTICAL;
      case CRATE_HORIZONTAL: return w == SLOT_HORIZONTAL;
    }
}
