
#include "level.h"

Level::Level(uint8_t width, uint8_t height) {
  this->width = width;
  this->height = height;

  this->player_pos = Pos{0, 0};
  this->player_dir = direction::EAST;
  this->player_item_a = NULL;
  this->player_item_b = NULL;

  this->exit_pos = Pos{0, 0};

  this->walls = new bool[this->width * this->height];
  for (int i=0; i<this->width*this->height; i++) walls[i] = false;
}

Level::~Level() {
  delete [] this->walls;
}

void Level::update() {
  bool ids[MAX_ID];
  for (int i = 0; i < MAX_SLOTS; i++) {
    Crate *crate = crate_at(slots[i]->pos);
    if (crate != NULL) {
      ids[slots[i]->id] = true;
    }
  }
  for (int i = 0; i < MAX_ID; i++) {
    this->ids[i] = ids[i];
  }
}

bool Level::id_is_active(Id id) {
  return this->ids[id];
}

Crate *Level::crate_at(Pos pos) {
  for (int i = 0; i < MAX_CRATES; i++) {
    if (this->crates[i] == NULL) continue;
    if (this->crates[i]->pos.x == pos.x && this->crates[i]->pos.y == pos.y) {
      return crates[i];
    }
  }
  return NULL;
}

bool Level::player_won() {
  return this->player_pos.x == this->exit_pos.x && this->player_pos.y == this->exit_pos.y;
}

bool Level::get_wall(Pos pos) {
  if (pos.x < this->width && pos.y < this->height) {
    return this->walls[pos.y * this->width + pos.x];
  }
  return true;
}

void Level::set_wall(Pos pos, bool value) {
  if (pos.x < this->width && pos.y < this->height) {
    this->walls[pos.y * width + pos.x] = value;
  }
}

void Level::pick_up_crate(Pos pos, bool item_slot_a) {
  // TODO
  // Crate *crate = this->crate_at(pos);
  // if (crate != NULL) {
  //   this->player_item_a = crate;
  //   this->remove_crate(crate);
  // }
}
