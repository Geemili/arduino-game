
#ifndef __SHAPES_H__
#define __SHAPES_H__

#include <Arduino.h>

namespace shapes {
  enum WallShape {
    WALL_VERTICAL,
    WALL_HORIZONTAL
  };

  enum CrateShape {
    CRATE_SMALL,
    CRATE_VERTICAL,
    CRATE_HORIZONTAL
  };

  bool shape_fits(CrateShape c, WallShape w);
}

#endif
