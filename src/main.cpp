
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "level.h"
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
#define SCREEN_INVENTORY 3

byte current_screen = SCREEN_MENU;

byte controller_data = 0;
bool input_latch = false;

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

byte screen_menu() {
  if (controller_data == BTN_START) {
    level = new Level(8, 6);
    level->set_wall(Pos{1, 0}, true);
    level->set_wall(Pos{3, 5}, true);
    level->crates[0] = new Crate{Pos{2,0}, shapes::CRATE_HORIZONTAL};
    level->crates[1] = new Crate{Pos{3,0}, shapes::CRATE_VERTICAL};
    level->crates[2] = new Crate{Pos{4,0}, shapes::CRATE_SMALL};
    return SCREEN_GAME;
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
  if (!level->get_wall(next_pos)) {
    level->player_pos = next_pos;
  }

  if (do_a || do_b) {
    //TODO
  }

  int offsetx = 32;
  int offsety = 16;

  // Reset display
  display.clearDisplay();

  for (uint8_t i = 0; i < level->width; i++) {
    for (uint8_t j = 0; j < level->height; j++) {
      if(level->get_wall(Pos{i, j})) {
        display.fillRect(i * 8 + offsetx, j * 8 + offsety, 8, 8, WHITE);
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

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("00 A:");
  display.print(level->player_item_a!=NULL ? '+' : ' ');
  display.print(" B:");
  display.print(level->player_item_b!=NULL ? '+' : ' ');

  display.display();

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

byte screen_inventory() {
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
    display.print("INVENTORY");

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
    default:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Unknown screen.");
      display.display();
      break;
  }
}
