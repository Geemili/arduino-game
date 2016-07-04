
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

byte controller_data = 0;

byte xpos = 0;
byte ypos = 0;

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

void loop() {
  read_controller();

  // Reset display
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  switch (controller_data) {
    case BTN_UP: display.print("UP"); break;
    case BTN_LEFT: display.print("LEFT"); break;
    case BTN_RIGHT: display.print("RIGHT"); break;
    case BTN_DOWN: display.print("DOWN"); break;
    case BTN_A: display.print("A"); break;
    case BTN_B: display.print("B"); break;
    case BTN_START: display.print("START"); break;
    case BTN_SELECT: display.print("SELECT"); break;
  }

  display.display();
}
