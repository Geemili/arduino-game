
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "level.h"
#include "level_loader.h"
#include "direction.h"
#include "offset.h"
#include "shapes.h"

#ifndef OLED_RESET_PIN
#define OLED_RESET_PIN 4
#endif
Adafruit_SSD1306 display(OLED_RESET_PIN);

#define NES_LATCH 2
#define NES_CLOCK 3
#define NES_DATA_IN 4
#define BTN_UP    B11110111
#define BTN_DOWN  B11111011
#define BTN_LEFT  B11111101
#define BTN_RIGHT B11111110
#define BTN_SELECT B11011111
#define BTN_START B11101111
#define BTN_A     B01111111
#define BTN_B     B10111111

#define SCREEN_MENU 0
#define SCREEN_GAME 1
#define SCREEN_PAUSE 2
#define SCREEN_LOAD_LEVEL 3
#define SCREEN_SELECT_LEVEL 4

byte current_screen = SCREEN_MENU;

byte controller_data = 0;
bool input_latch = false;

// Tells SCREEN_LOAD_LEVEL which one to load and SCREEN_GAME which number to display
uint8_t level_num = 0;
Level *level;

void setup() {
  display.begin();
  display.clearDisplay();
  display.display();

  pinMode(NES_LATCH, OUTPUT);
  pinMode(NES_CLOCK, OUTPUT);
  pinMode(NES_DATA_IN, INPUT);

  digitalWrite(NES_LATCH, HIGH);
  digitalWrite(NES_CLOCK, HIGH);
}

void read_controller() {
  controller_data = 0;
  digitalWrite(NES_LATCH, LOW);
  digitalWrite(NES_CLOCK, LOW);

  digitalWrite(NES_LATCH, HIGH);
  delayMicroseconds(2);
  digitalWrite(NES_LATCH, LOW);

  controller_data = digitalRead(NES_DATA_IN);

  for (int i = 1; i <= 7; i++) {
    digitalWrite(NES_CLOCK, HIGH);
    delayMicroseconds(2);
    controller_data = controller_data << 1;
    controller_data = controller_data + digitalRead(NES_DATA_IN);
    delayMicroseconds(4);
    digitalWrite(NES_CLOCK, LOW);
  }
}

bool is_pressed(byte button) {
  return (controller_data | button) == button;
}

void draw_ui_crate(int offset, shapes::CrateShape shape) {
  switch (shape) {
    case shapes::CRATE_HORIZONTAL:
      display.fillRect(offset, 4, 16, 8, WHITE);
      break;
    case shapes::CRATE_VERTICAL:
      display.fillRect(offset + 4, 0, 8, 16, WHITE);
      break;
    case shapes::CRATE_SMALL:
      display.fillRect(offset + 4, 4, 8, 8, WHITE);
      break;
  }
}

byte screen_menu() {
  if (controller_data == BTN_START) {
    level_num = 0;
    return SCREEN_LOAD_LEVEL;
  }
  if (controller_data == BTN_SELECT) {
    return SCREEN_SELECT_LEVEL;
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("ROUGELIKE");

  display.setTextSize(1);
  display.setCursor(0, 32);
  display.print("Press START to begin");

  display.display();

  return SCREEN_MENU;
}

byte screen_game() {
  Offset player_offset = Offset{0,0};
  bool do_a = false;
  bool do_b = false;
  switch (controller_data) {
    case BTN_UP:
      if (!input_latch) {
        player_offset.y -= 1;
        level->player_dir = direction::NORTH;
      }
      input_latch = true;
      break;
    case BTN_LEFT:
      if (!input_latch) {
        player_offset.x -= 1;
        level->player_dir = direction::WEST;
      }
      input_latch = true;
      break;
    case BTN_RIGHT:
      if (!input_latch) {
        player_offset.x += 1;
        level->player_dir = direction::EAST;
      }
      input_latch = true;
      break;
    case BTN_DOWN:
      if (!input_latch) {
        player_offset.y += 1;
        level->player_dir = direction::SOUTH;
      }
      input_latch = true;
      break;
    case BTN_A:
      if (!input_latch) do_a = true;
      input_latch = true;
      break;
    case BTN_B:
      if (!input_latch) do_b = true;
      input_latch = true;
      break;
    case BTN_START:
      if (!input_latch) return SCREEN_PAUSE;
      input_latch = true;
      break;
    // case BTN_SELECT: display.print("SELECT"); break;
    default:
      input_latch = false;
      break;
  }

  Pos next_pos = get_pos_offset(level->player_pos, player_offset);
  if (level->is_open(next_pos)) {
    level->player_pos = next_pos;
  }

  if (do_a || do_b) {
    Pos pick_pos = get_pos_offset(level->player_pos, direction::offset(level->player_dir));
    if (do_a && level->player_item_a==NULL) {
      level->pick_up_crate(pick_pos, true);
    } else if (do_b && level->player_item_b==NULL) {
      level->pick_up_crate(pick_pos, false);
    } else if (do_a && level->player_item_a!=NULL) {
      level->place_crate(pick_pos, true);
    } else if (do_b && level->player_item_b!=NULL) {
      level->place_crate(pick_pos, false);
    }
  }

  level->update();

  int offsetx = 64 - level->player_pos.x * 8 - 4;
  int offsety = 40 - level->player_pos.y * 8 - 4;

  // Reset display
  display.clearDisplay();

  for (uint8_t i = 0; i < level->width; i++) {
    for (uint8_t j = 0; j < level->height; j++) {
      if(level->get_wall(Pos{i, j})) {
        display.fillRect(i * 8 + offsetx, j * 8 + offsety, 8, 8, WHITE);
      }
    }
  }

  for (uint8_t i = 0; i < MAX_SLOTS; i++) {
    Slot *slot = level->slots[i];
    if (slot != NULL) {
      display.fillRoundRect(slot->pos.x * 8 + offsetx, slot->pos.y * 8 + offsety, 8, 8, 1, WHITE);
      switch (slot->shape) {
        case shapes::SLOT_HORIZONTAL:
          display.fillRect(slot->pos.x * 8 + 1 + offsetx, slot->pos.y * 8 + 2 + offsety, 6, 4, BLACK);
          break;
        case shapes::SLOT_VERTICAL:
          display.fillRect(slot->pos.x * 8 + 2 + offsetx, slot->pos.y * 8 + 1 + offsety, 4, 6, BLACK);
          break;
      }
    }
  }

  for (uint8_t i = 0; i < MAX_CRATES; i++) {
    Crate *crate = level->crates[i];
    if (crate != NULL) {
      switch (crate->shape) {
        case shapes::CRATE_HORIZONTAL:
          display.fillRect(crate->pos.x * 8 + offsetx, crate->pos.y * 8 + 2 + offsety, 8, 4, WHITE);
          break;
        case shapes::CRATE_VERTICAL:
          display.fillRect(crate->pos.x * 8 + 2 + offsetx, crate->pos.y * 8 + offsety, 4, 8, WHITE);
          break;
        case shapes::CRATE_SMALL:
          display.fillRect(crate->pos.x * 8 + 2 + offsetx, crate->pos.y * 8 + 2 + offsety, 4, 4, WHITE);
          break;
      }
    }
  }

  for (uint8_t i = 0; i < MAX_DOORS; i++) {
    Door *door = level->doors[i];
    if (door != NULL) {
      if (level->id_is_active(door->key)) {
        display.drawRect(door->pos.x * 8 + offsetx, door->pos.y * 8 + offsety, 8, 8, WHITE);
      } else {
        display.fillRect(door->pos.x * 8 + offsetx, door->pos.y * 8 + offsety, 8, 8, WHITE);
        display.fillCircle(door->pos.x * 8 + 4 + offsetx, door->pos.y * 8 + 4 + offsety, 1, BLACK);
      }
    }
  }

  display.drawRoundRect(level->exit_pos.x * 8 + 1 + offsetx, level->exit_pos.y * 8 + 1 + offsety, 6, 6, 2, WHITE);

  int pixelx = level->player_pos.x * 8 + offsetx;
  int pixely = level->player_pos.y * 8 + offsety;
  switch (level->player_dir) {
    case direction::NORTH:
      display.fillTriangle(pixelx, pixely+8, pixelx+8, pixely+8, pixelx+4, pixely, WHITE);
      break;
    case direction::EAST:
      display.fillTriangle(pixelx, pixely, pixelx, pixely+8, pixelx+8, pixely+4, WHITE);
      break;
    case direction::SOUTH:
      display.fillTriangle(pixelx, pixely, pixelx+8, pixely, pixelx+4, pixely+8, WHITE);
      break;
    case direction::WEST:
      display.fillTriangle(pixelx+8, pixely, pixelx+8, pixely+8, pixelx, pixely+4, WHITE);
      break;
  }

  display.fillRect(0, 0, 128, 16, BLACK);
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print(level_num, HEX);

  int crate_offset = 16;

  int offset_a = 40;
  display.setCursor(offset_a, 0);
  display.print("A");
  if (level->player_item_a!=NULL) {
    draw_ui_crate(offset_a + crate_offset, level->player_item_a->shape);
  }

  int offset_b = 80;
  display.setCursor(80, 0);//0 16 32 48 64 80 96 112 128
  display.print("B");
  if (level->player_item_b!=NULL) {
    draw_ui_crate(offset_b + crate_offset, level->player_item_b->shape);
  }

  display.display();

  if (level->player_won()) {
    level_num++;
    return SCREEN_LOAD_LEVEL;
  }
  return SCREEN_GAME;
}

byte screen_pause() {
  int selected = 0;
  while (true) {
    read_controller();

    if (!input_latch && is_pressed(BTN_UP)) selected -= 1;
    if (!input_latch && is_pressed(BTN_DOWN)) selected += 1;
    input_latch = (is_pressed(BTN_DOWN) || is_pressed(BTN_UP));
    if (selected > 1) selected = 0;
    if (selected < 0) selected = 1;
    if (is_pressed(BTN_A)) {
        if (selected == 0) return SCREEN_GAME;
        if (selected == 1) return SCREEN_MENU;
    }

    display.clearDisplay();
    display.setTextColor(WHITE);

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("PAUSE");

    display.setTextSize(1);
    display.setCursor(16, 32);
    display.print("Continue");
    display.setCursor(16, 40);
    display.print("Quit");

    if (selected == 0) display.setCursor(0,32);
    if (selected == 1) display.setCursor(0,40);
    display.print(">");

    display.display();
  }
}

byte screen_load_level() {
  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("LOADING...");

  display.display();

  if (level != NULL) {
    delete level;
  }
  level = load_level(level_num);
  return SCREEN_GAME;
}

byte screen_select_level() {
  switch (controller_data) {
    case BTN_UP:
      if (!input_latch) {
        level_num++;
      }
      input_latch = true;
      break;
    case BTN_DOWN:
      if (!input_latch) {
        level_num--;
      }
      input_latch = true;
      break;
    case BTN_A:
      if (!input_latch) return SCREEN_LOAD_LEVEL;
      input_latch = true;
      break;
    case BTN_B: return SCREEN_MENU;
    default:
      input_latch = false;
      break;
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Pick Level");

  display.setTextSize(5);
  display.setCursor(16, 16);
  char buf[3];
  sprintf(buf, "%3d", level_num);
  display.print(buf);

  display.display();
  return SCREEN_SELECT_LEVEL;
}

void loop() {
  read_controller();
  switch (current_screen) {
    case SCREEN_MENU:
      current_screen = screen_menu();
      break;
    case SCREEN_GAME:
      current_screen = screen_game();
      break;
    case SCREEN_PAUSE:
      current_screen = screen_pause();
      break;
    case SCREEN_LOAD_LEVEL:
      current_screen = screen_load_level();
      break;
    case SCREEN_SELECT_LEVEL:
      current_screen = screen_select_level();
      break;
    default:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Unknown screen.");
      display.display();
      break;
  }
}
