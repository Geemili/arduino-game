
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <i2c_master.h>

#define OLED_ADDRESS 0x78 // 0x3c << 1
#define BLINK_DELAY_MS 1000

void setup_i2c();
void clear_display();
void draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color);
void draw_ascii(uint8_t col, uint8_t page, uint8_t number);

int main (void)
{
  // Variable used to store return values
  unsigned char ret;

  // Set pin 5 of PORTB (aka pin 13) to output and turn LED off
  DDRB |= _BV(DDB5);
  PORTB &= ~_BV(PORTB5);

  i2c_init();

  ret = i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_stop();
  if (ret) {
    PORTB |= _BV(PORTB5); // Turn on LED to indicate error
  } else {
    setup_i2c();
  }

  clear_display();
  int16_t x = 0;
  int16_t y = 0;
  int16_t w = 1;
  int16_t h = 1;

  // Set up timer
  TCCR1B |= ((1 << CS10) | (1 << CS12));

  while (1) {
    // clear_display();
    // draw_rect(x, y, w, h, 0);
    // w++; h++;
    // if (w >= 128) w = 0;
    // if (h >= 64) h = 0;

    char str[15];
    uint32_t time_to_draw = TCNT1;
    time_to_draw *= 64;
    sprintf(str, "%lu", time_to_draw);
    TCNT1 = 0;
    for (uint8_t i=0; i<15; i++) {
      if (str[i] == 0) break;
      draw_ascii(i*6, 0, str[i]);
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

void draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color) {
  if (w==0 || x >= 128 || h==0 || y >= 64) return;
  w--;

  uint8_t start_col = x,
          end_col = 127,
          start_page = y>>3,
          end_page = 7;

  if (x+w < 128) {
    end_col = x+w;
  }

  if (y+h < 64) {
    end_page = (y+h)>>3;
  }

  i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_write(0x00); // Command mode

  i2c_write(0x21); // Column
  i2c_write(start_col);    // start
  i2c_write(end_col);

  i2c_write(0x22); // Page address / row adress
  i2c_write(start_page); // start
  i2c_write(end_page);

  i2c_stop();

  uint8_t twbr_prev = TWBR;
  TWBR = 12;

  unsigned char ret;
  uint8_t num_transmitted = 0;
  uint8_t open = 0;
  for (uint16_t page=start_page; page<=end_page; page++) {
    for (uint16_t col=start_col; col<=end_col; col++) {
      if (num_transmitted==0) {
        ret = i2c_start(OLED_ADDRESS + I2C_WRITE);
        open = 1;
        i2c_write(0x40); // Data mode
      }
      if (ret) {
        i2c_stop();
        open = 0;
        PORTB |= _BV(PORTB5);
      } else {
        uint8_t segment = 0x00;
        for (uint8_t i=0; i<8; i++) {
          if (((page*8 + i) >= y) && ((page*8 + i) < (y+h))) {
            segment |= (color << i);
          }
        }
        i2c_write(segment);
        num_transmitted++;
        if (num_transmitted==16) {
          i2c_stop();
          open = 0;
          num_transmitted = 0;
        }
      }
    }
  }

  if (open) i2c_stop();

  TWBR = twbr_prev;
}

void draw_ascii(uint8_t col, uint8_t page, uint8_t number) {
  if (col >= 128 || page >= 8) return;

  i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_write(0x00); // Command mode

  i2c_write(0x21); // Column
  i2c_write(col);    // start
  i2c_write(col+4);

  i2c_write(0x22); // Page address / row adress
  i2c_write(page); // start
  i2c_write(page);

  i2c_stop();

  uint8_t twbr_prev = TWBR;
  TWBR = 12;

  unsigned char ret = i2c_start(OLED_ADDRESS + I2C_WRITE);
  i2c_write(0x40); // Data mode
  if (ret) {
    i2c_stop();
    PORTB |= _BV(PORTB5);
  } else {
    switch (number) {
      case 0x30: // 0
        i2c_write(0b01111100);
        i2c_write(0b10100010);
        i2c_write(0b10010010);
        i2c_write(0b10001010);
        i2c_write(0b01111100);
        break;
      case 0x31: // 1
        i2c_write(0b00000000);
        i2c_write(0b10000100);
        i2c_write(0b11111110);
        i2c_write(0b10000000);
        i2c_write(0b00000000);
        break;
      case 0x32: // 2
        i2c_write(0b10000100);
        i2c_write(0b11000010);
        i2c_write(0b10100010);
        i2c_write(0b10010010);
        i2c_write(0b10001100);
        break;
      case 0x33: // 3
        i2c_write(0b01000010);
        i2c_write(0b10000010);
        i2c_write(0b10001010);
        i2c_write(0b10010110);
        i2c_write(0b01100010);
        break;
      case 0x34: // 4
        i2c_write(0b00110000);
        i2c_write(0b00101000);
        i2c_write(0b00100100);
        i2c_write(0b11111110);
        i2c_write(0b00100000);
        break;
      case 0x35: // 5
        i2c_write(0b01001110);
        i2c_write(0b10001010);
        i2c_write(0b10001010);
        i2c_write(0b10001010);
        i2c_write(0b01110010);
        break;
      case 0x36: // 6
        i2c_write(0b01111000);
        i2c_write(0b10010100);
        i2c_write(0b10010010);
        i2c_write(0b10010010);
        i2c_write(0b01100010);
        break;
      case 0x37: // 7
        i2c_write(0b00000010);
        i2c_write(0b11100010);
        i2c_write(0b00010010);
        i2c_write(0b00001010);
        i2c_write(0b00000110);
        break;
      case 0x38: // 8
        i2c_write(0b01101100);
        i2c_write(0b10010010);
        i2c_write(0b10010010);
        i2c_write(0b10010010);
        i2c_write(0b01101100);
        break;
      case 0x39: // 9
        i2c_write(0b00001100);
        i2c_write(0b10010010);
        i2c_write(0b10010010);
        i2c_write(0b01010010);
        i2c_write(0b00111100);
        break;
      default:
        i2c_write(0b11111111);
        i2c_write(0b10000001);
        i2c_write(0b10000001);
        i2c_write(0b10000001);
        i2c_write(0b11111111);
        break;
      // case 0x4:
      //   i2c_write(0b00000000);
      //   i2c_write(0b00000000);
      //   i2c_write(0b00000000);
      //   i2c_write(0b00000000);
      //   i2c_write(0b00000000);
      //   break;
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
