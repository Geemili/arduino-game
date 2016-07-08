
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
  for (int i=0; i<this->width*this->height; i++) this->walls[i] = false;
  for (int i=0; i<MAX_CRATES; i++) this->crates[i] = NULL;
  for (int i=0; i<MAX_SLOTS; i++) this->slots[i] = NULL;
  for (int i=0; i<MAX_DOORS; i++) this->doors[i] = NULL;
}

Level::~Level() {
  delete [] this->walls;
  delete [] this->player_item_a;
  delete [] this->player_item_b;
  for (int i=0; i<MAX_CRATES; i++) delete this->crates[i];
  for (int i=0; i<MAX_SLOTS; i++) delete this->slots[i];
  for (int i=0; i<MAX_DOORS; i++) delete this->doors[i];
}

void Level::update() {
  for (int i = 0; i < MAX_SLOTS; i++) {
    this->ids[i] = false;
  }
  for (int i = 0; i < MAX_SLOTS; i++) {
    Crate *crate = crate_at(slots[i]->pos);
    if (crate != NULL) {
      ids[slots[i]->id] = true;
    }
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

uint8_t Level::add_crate(Crate *crate) {
  for (int i = 0; i < MAX_CRATES; i++) {
    if (this->crates[i] == NULL) {
      this->crates[i] = crate;
      return i;
    }
  }
  return MAX_CRATES;
}

uint8_t Level::add_door(Door *door) {
  for (int i = 0; i < MAX_DOORS; i++) {
    if (this->doors[i] == NULL) {
      this->doors[i] = door;
      return i;
    }
  }
  return MAX_DOORS;
}

uint8_t Level::add_slot(Slot *slot) {
  for (int i = 0; i < MAX_SLOTS; i++) {
    if (this->slots[i] == NULL) {
      this->slots[i] = slot;
      return i;
    }
  }
  return MAX_SLOTS;
}

uint8_t Level::index_of_crate_at(Pos pos) {
  for (int i = 0; i < MAX_CRATES; i++) {
    if (this->crates[i] == NULL) continue;
    if (this->crates[i]->pos.x == pos.x && this->crates[i]->pos.y == pos.y) {
      return i;
    }
  }
  return MAX_CRATES;
}

Slot *Level::slot_at(Pos pos) {
  for (int i = 0; i < MAX_SLOTS; i++) {
    if (this->slots[i] == NULL) continue;
    if (this->slots[i]->pos.x == pos.x && this->slots[i]->pos.y == pos.y) {
      return slots[i];
    }
  }
  return NULL;
}

Door *Level::door_at(Pos pos) {
  for (int i = 0; i < MAX_DOORS; i++) {
    if (this->doors[i] == NULL) continue;
    if (this->doors[i]->pos.x == pos.x && this->doors[i]->pos.y == pos.y) {
      return doors[i];
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
  if (item_slot_a && this->player_item_a != NULL) return;
  else if (!item_slot_a && this->player_item_b != NULL) return;
  uint8_t crate = this->index_of_crate_at(pos);
  if (crate != MAX_CRATES) {
    if (item_slot_a) this->player_item_a = this->crates[crate];
    else this->player_item_b = this->crates[crate];
    this->crates[crate] = NULL;
  }
}

void Level::place_crate(Pos pos, bool item_slot_a) {
  // Check that we actually have a crate to place
  if (item_slot_a && this->player_item_a == NULL) return;
  else if (!item_slot_a && this->player_item_b == NULL) return;

  // Make sure that there is no wall there
  if(this->get_wall(pos)) return;
  
  // Don't let players place inside of doors
  Door *door = this->door_at(pos);
  if (door != NULL) return;

  // Don't place on top of other crates
  Crate *other_crate = this->crate_at(pos);
  if (other_crate != NULL) return;

  // Pull out the crate that we are using
  Crate *crate;
  if (item_slot_a) {
    crate = this->player_item_a;
  } else {
    crate = this->player_item_b;
  }

  Slot *slot = this->slot_at(pos);
  if (slot == NULL || shapes::shape_fits(crate->shape, slot->shape)) {
    crate->pos = pos;
    // Place the crate
    this->add_crate(crate);
    if (item_slot_a) {
      this->player_item_a = NULL;
    } else {
      this->player_item_b = NULL;
    }
  }
}

bool Level::is_open(Pos pos) {
  if (this->get_wall(pos)) return false;
  if (this->crate_at(pos)!=NULL) return false;
  if (this->slot_at(pos)!=NULL) return false;
  Door *door = this->door_at(pos);
  if (door!=NULL && !this->id_is_active(door->key)) return false;
  return true;
}
