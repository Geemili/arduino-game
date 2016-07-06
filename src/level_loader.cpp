
#include "level_loader.h"
#include "levels.h"

Level *load_level(uint8_t level_num) {
  PGM_P level_address = get_level_address(level_num);
  // TODO: Make use of this. Ignoring length check right now
  // int len = strlen_P(level_address);

  uint8_t w = pgm_read_byte_near(level_address + 0);
  uint8_t h = pgm_read_byte_near(level_address + 1);
  Level *level = new Level(w, h);
  for (uint8_t j=0; j<h; j++) {
    for (uint8_t i=0; i<w; i++) {
      int tile_index = (j * w) + i;
      char c = pgm_read_byte_near(level_address + 2 + tile_index);
      switch (c) {
        case '#':
          level->walls[tile_index] = true;
          break;
        case 'e':
          level->exit_pos = Pos{i, j};
          break;
        case '@':
          level->player_pos = Pos{i, j};
          break;
        default: break; // Ignore everything else
      }
    }
  }
  return level;
}
