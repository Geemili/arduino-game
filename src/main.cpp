
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

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

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

byte current_screen = SCREEN_MENU;

byte controller_data = 0;
bool input_latch = false;

#define LEVEL_WIDTH 8
#define LEVEL_HEIGHT 8
byte level[LEVEL_WIDTH * LEVEL_HEIGHT];

int xpos = 0;
int ypos = 0;
byte facing = 0;
byte equip_a = 0;
byte equip_b = 0;

int to_index(int x, int y) {
  return y * LEVEL_WIDTH + x;
}

void setup() {
  display.begin();
  display.clearDisplay();
  display.display();

  pinMode(NES_LATCH, OUTPUT);
  pinMode(NES_CLOCK, OUTPUT);
  pinMode(NES_DATA_IN, INPUT);

  digitalWrite(NES_LATCH, HIGH);
  digitalWrite(NES_CLOCK, HIGH);

  level[to_index(1, 0)] = 1;
  level[to_index(3, 5)] = 1;
  level[to_index(7, 7)] = 1;
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
  if (controller_data == BTN_START) return SCREEN_GAME;

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
  switch (controller_data) {
    case BTN_UP:
      if (!input_latch) {
        ypos -= 1;
        facing = NORTH;
      }
      input_latch = true;
      break;
    case BTN_LEFT:
      if (!input_latch) {
        xpos -= 1;
        facing = WEST;
      }
      input_latch = true;
      break;
    case BTN_RIGHT:
      if (!input_latch) {
        xpos += 1;
        facing = EAST;
      }
      input_latch = true;
      break;
    case BTN_DOWN:
      if (!input_latch) {
        ypos += 1;
        facing = SOUTH;
      }
      input_latch = true;
      break;
    // case BTN_A: display.print("A"); break;
    // case BTN_B: display.print("B"); break;
    case BTN_START:
      if (!input_latch) return SCREEN_PAUSE;
      input_latch = true;
      break;
    // case BTN_SELECT: display.print("SELECT"); break;
    default:
      input_latch = false;
      break;
  }

  if (xpos < 0) xpos = 0;
  if (xpos >= LEVEL_WIDTH) xpos = LEVEL_WIDTH-1;
  if (ypos < 0) ypos = 0;
  if (ypos >= LEVEL_HEIGHT) ypos = LEVEL_HEIGHT-1;

  int offsetx = 32;
  int offsety = 16;

  // Reset display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  for (int i = 0; i < LEVEL_WIDTH; i++) {
    for (int j = 0; j < LEVEL_HEIGHT; j++) {
      display.setCursor(i * 8 + offsetx, j * 8 + offsety);
      display.print(level[to_index(i, j)]==1 ? '+' : '.');
    }
  }

  int pixelx = xpos*8+offsetx;
  int pixely = ypos*8+offsety;
  switch (facing) {
    case NORTH:
      display.fillTriangle(pixelx, pixely+8, pixelx+8, pixely+8, pixelx+4, pixely, WHITE);
      break;
    case EAST:
      display.fillTriangle(pixelx, pixely, pixelx, pixely+8, pixelx+8, pixely+4, WHITE);
      break;
    case SOUTH:
      display.fillTriangle(pixelx, pixely, pixelx+8, pixely, pixelx+4, pixely+8, WHITE);
      break;
    case WEST:
      display.fillTriangle(pixelx+8, pixely, pixelx+8, pixely+8, pixelx, pixely+4, WHITE);
      break;
  }

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("A:");
  display.print(equip_a==1 ? '+' : ' ');
  display.print(" B:");
  display.print(equip_b==1 ? '+' : ' ');

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
