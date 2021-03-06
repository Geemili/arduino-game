
#ifndef __LEVEL_H__
#define __LEVEL_H__

#ifndef MAX_DOORS
#define MAX_DOORS 4
#endif

#ifndef MAX_ID
#define MAX_ID 4
#endif

#ifndef MAX_CRATES
#define MAX_CRATES 4
#endif

#ifndef MAX_SLOTS
#define MAX_SLOTS 4
#endif

#include <Arduino.h>
#include "shapes.h"
#include "direction.h"
#include "pos.h"

typedef uint8_t Id;

struct Crate {
  public:
    Pos pos;
    shapes::CrateShape shape;
};

struct Slot {
  public:
    Id id;
    Pos pos;
    shapes::SlotShape shape;
};

struct Door {
  public:
    Pos pos;
    Id key;
};

class Level {
  public:
    Pos player_pos;
    direction::Direction player_dir;
    Crate *player_item_a;
    Crate *player_item_b;

    Pos exit_pos;

    uint8_t width, height;
    Crate *crates[MAX_CRATES];
    Slot *slots[MAX_SLOTS];
    Door *doors[MAX_DOORS];
    bool ids[MAX_ID];
    bool *walls; // width * height

    // Methods
    Level(uint8_t width, uint8_t height);
    ~Level();
    void update();
    bool player_won();
    void set_wall(Pos pos, bool value);
    bool get_wall(Pos pos);
    uint8_t add_crate(Crate *crate);
    Crate *crate_at(Pos pos);
    uint8_t index_of_crate_at(Pos pos);
    void place_crate(Pos pos, bool item_slot_a);
    void pick_up_crate(Pos pos, bool item_slot_a);
    uint8_t add_slot(Slot *slot);
    Slot *slot_at(Pos pos);
    bool is_open(Pos pos);
    uint8_t add_door(Door *door);
    Door *door_at(Pos pos);
    bool id_is_active(Id id);
};

#endif
