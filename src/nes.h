
#ifndef __NES_H__
#define __NES_H__

#include <Arduino.h>

namespace nes {
  enum Button {
    A       = B01111111,
    B       = B10111111,
    SELECT  = B11011111,
    START   = B11101111,
    UP      = B11110111,
    DOWN    = B11111011,
    LEFT    = B11111101,
    RIGHT   = B11111110,
  };

  class Pad {
  private:
    int latch;
    int clk;
    int data_in;
    uint8_t prev_data;
    uint8_t data;

    bool is_pressed(Button button, uint8_t data);

  public:
    Pad(int latch, int clk, int data_in);
    void begin();
    void update();
    bool is_pressed(Button button);
    // Checks if the button was pressed just a step ago and if it is now released
    bool just_released(Button button);
    bool just_pressed(Button button);
  };
}

#endif
