
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <i2c_master.h>
#include "font.h"
#include "nes.h"

#define OLED_ADDRESS 0x78 // 0x3c << 1
#define BLINK_DELAY_MS 1000

void setup_i2c();
void clear_display();
void draw_ascii(uint8_t col, uint8_t page, char *text, uint8_t len);

int main (void)
{
  // Variable used to store return values
  unsigned char ret;

  // Set pin 5 of PORTB (aka pin 13) to output and turn LED off
  DDRB |= _BV(DDB5);
  PORTB &= ~_BV(PORTB5);

  i2c_init();
  nes_pad_init();

  ret = i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_stop();
  if (ret) {
    PORTB |= _BV(PORTB5); // Turn on LED to indicate error
    return 1;
  } else {
    setup_i2c();
  }

  clear_display();
  uint8_t x = 1;
  uint8_t y = 2;

  // Set up timer
  TCCR1B |= ((1 << CS10) | (1 << CS12));

  static char str[21];
  static uint16_t last_render_time = 0;
  uint16_t last_update_time = 0;
  while (1) {
    if (TCNT1-last_update_time >= 1920) {
      nes_pad_update();
      if (nes_pad_just_pressed(NES_LEFT)) x--;
      if (nes_pad_just_pressed(NES_RIGHT)) x++;
      if (nes_pad_just_pressed(NES_UP)) y--;
      if (nes_pad_just_pressed(NES_DOWN)) y++;
      last_update_time = TCNT1;
    }

    if (TCNT1-last_render_time >= 1920) { // Render every 30 ms
      sprintf(str, "Delta: %d", TCNT1-last_render_time);
      draw_ascii(0, 0, str, 21);
      for (uint8_t j=1; j<8; j++) {
        for (uint8_t i=0; i<21; i++) {
          if (y==j && x==i) {str[i] = '@';}
          else {str[i] = '.';}
        }
        draw_ascii(0, j, str, 21);
      }
      last_render_time = TCNT1;
    }
  }
}

void clear_display() {
  i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_write(0x00); // Command mode

  i2c_write(0x21); // Column
  i2c_write(0);    // start
  i2c_write(127);    // end

  i2c_write(0x22); // Page address / row adress
  i2c_write(0x0); // start
  i2c_write(0x7);

  i2c_stop();

  // TURBO MODE
  uint8_t twbr_prev = TWBR;
  TWBR = 12;

  unsigned char ret;
  uint8_t num_transmitted = 0;
  for (uint16_t page=0; page<8; page++) {
    for (uint16_t col=0; col<128; col++) {
      if (num_transmitted==0) {
        ret = i2c_start(OLED_ADDRESS + I2C_WRITE);
        i2c_write(0x40); // Data Mode
      }
      if (ret) {
        i2c_stop();
        PORTB |= _BV(PORTB5);
      } else {
        i2c_write(0x00);
        num_transmitted++;
        if (num_transmitted==16) {
          i2c_stop();
          num_transmitted = 0;
        }
      }
    }
  }

  if (num_transmitted != 0) i2c_stop();

  TWBR = twbr_prev;
}

void draw_ascii(uint8_t col, uint8_t page, char *text, uint8_t len) {
  if (col >= 128 || page >= 8 || len==0) return;

  uint8_t start_col = col,
          end_col = 127;

  if (col+len*6 < 128) {
    end_col = col + len*6;
  }

  i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_write(0x00); // Command mode

  i2c_write(0x21); // Column
  i2c_write(start_col);    // start
  i2c_write(end_col);

  i2c_write(0x22); // Page address / row adress
  i2c_write(page); // start
  i2c_write(page);

  i2c_stop();

  uint8_t twbr_prev = TWBR;
  TWBR = 12;

  unsigned char ret;
  ret = i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_write(0x40); // Data mode
  for (uint16_t i=0; i<len; i++) {
    if (ret) {
      i2c_stop();
      PORTB |= _BV(PORTB5);
      return;
    } else {
      if (text[i]==0) break; // Break if we reached null byte
      for (int j=0; j<5; j++) i2c_write(pgm_read_byte(&ASCII[text[i] - 0x20][j]));
      i2c_write(0b00000000);
    }
  }

  i2c_stop();

  TWBR = twbr_prev;
}

void setup_i2c() {
  i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_write(0x00); // Command mode

  // Setup the OLED screen
  i2c_write(0xAE); // Turn the display off

  i2c_write(0x00 | 0x0); // Lo col = 0
  i2c_write(0x10 | 0x0); // hi col = 0
  i2c_write(0x40 | 0x0); // Line #0

  i2c_write(0x81); // Contrast 0x81
  i2c_write(0xCF); // Flips display

  i2c_write(0xA1); // Segremap
  i2c_write(0xC8); // COMSCAN DEC
  i2c_write(0xA6); // Normal Display (Invert A7)

  i2c_write(0xA4); // Display everything on resume
  i2c_write(0xA8); // Set multiplex
  i2c_write(63); // 1/64 duty cycle

  i2c_write(0xD3); // Set display offset to...
  i2c_write(0x0);  // 1. If it is left at 0 it is offset by 1

  i2c_write(0xD5); // Set display clk
  i2c_write(0x80);

  i2c_write(0xD9); // Set precharge
  i2c_write(0xF1);

  i2c_write(0xDA); // Set COM pins
  i2c_write(0x12);

  i2c_write(0xDB);
  i2c_write(0x40);

  i2c_write(0x20); // Set memory adressing mode to...
  i2c_write(0x00); // horizontal

  i2c_write(0x40 | 0x0);

  i2c_write(0x8D);
  i2c_write(0x14);

  i2c_write(0xA4);
  i2c_write(0xAF);

  i2c_stop();
}
