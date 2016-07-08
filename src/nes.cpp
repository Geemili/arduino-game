
#include "nes.h"

namespace nes {
  Pad::Pad(int latch, int clk, int data_in) {
    this->latch = latch;
    this->clk = clk;
    this->data_in = data_in;
  }

  void Pad::begin() {
    pinMode(this->latch, OUTPUT);
    pinMode(this->clk, OUTPUT);
    pinMode(this->data_in, INPUT);

    digitalWrite(this->latch, HIGH);
    digitalWrite(this->clk, HIGH);
  }

  bool Pad::is_pressed(Button button, uint8_t data) {
    return (data | button) == button;
  }

  bool Pad::is_pressed(Button button) {
    return this->is_pressed(button, this->data);
  }

  bool Pad::just_released(Button button) {
    return is_pressed(button, prev_data) && !is_pressed(button, data);
  }

  bool Pad::just_pressed(Button button) {
    return !is_pressed(button, prev_data) && is_pressed(button, data);
  }

  void Pad::update() {
    this->prev_data = this->data;
    this->data = 0;
    digitalWrite(this->latch, LOW);
    digitalWrite(this->clk, LOW);

    digitalWrite(this->latch, HIGH);
    delayMicroseconds(2);
    digitalWrite(this->latch, LOW);

    this->data = digitalRead(this->data_in);

    for (int i = 1; i <= 7; i++) {
      digitalWrite(this->clk, HIGH);
      delayMicroseconds(2);
      this->data = this->data << 1;
      this->data = this->data + digitalRead(this->data_in);
      delayMicroseconds(4);
      digitalWrite(this->clk, LOW);
    }
  }
}
