
#include "level_loader.h"
#include "levels.h"

#define CHAR_WALL '#'
#define CHAR_EXIT '!'
#define CHAR_PLAYER '@'

#define CHAR_CRATE_V 'k'
#define CHAR_CRATE_H 'l'
#define CHAR_CRATE_S 'm'

#define CHAR_SLOT_V0 'a'
#define CHAR_SLOT_V1 'b'
#define CHAR_SLOT_V2 'c'
#define CHAR_SLOT_V3 'd'

#define CHAR_SLOT_H0 'f'
#define CHAR_SLOT_H1 'g'
#define CHAR_SLOT_H2 'h'
#define CHAR_SLOT_H3 'i'

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
        case CHAR_WALL:
          level->walls[tile_index] = true;
          break;
        case CHAR_EXIT:
          level->exit_pos = Pos{i, j};
          break;
        case CHAR_PLAYER:
          level->player_pos = Pos{i, j};
          break;
        case CHAR_CRATE_V:
          level->add_crate(new Crate{Pos{i, j}, shapes::CRATE_VERTICAL});
          break;
        default: break; // Ignore everything else
      }
    }
  }
  return level;
}
