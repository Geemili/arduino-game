
#include <avr/io.h>
#include <util/delay.h>
#include <i2c_master.h>

#define OLED_ADDRESS 0x78 // 0x3c << 1
#define BLINK_DELAY_MS 1000

void setup_i2c();
void clear_display(int16_t x, int16_t y, int16_t w, int16_t h);
void draw_buffer();

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

  int16_t x = 0;
  int16_t y = 0;
  while (1) {
    clear_display(x, y, 5, 6);
    y++;
    if (y > 64) {
      x++;
      y = 0;
    }
    if (x > 128) {
      x = 0;
    }
  }
}

void setup_i2c() {
  i2c_start(OLED_ADDRESS + I2C_WRITE);

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
  i2c_write(0x0);  // nothing

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

void draw_buffer() {
  i2c_start(OLED_ADDRESS + I2C_WRITE);

  i2c_write(0x21); // Column
  i2c_write(0);    // start
  i2c_write(127);    // end

  i2c_write(0x22); // Page address / row adress
  i2c_write(0); // start
  i2c_write(7);

  i2c_stop();


  for (uint16_t i=0; i < 127; i++) {
    unsigned char ret = i2c_start(OLED_ADDRESS + I2C_WRITE);
    if (ret) {
      i2c_stop();
      PORTB |= _BV(PORTB5);
    } else {
      i2c_write(0x40);
      i2c_write(i & 0xFF);
      i2c_stop();
    }
  }
}

void clear_display(int16_t x, int16_t y, int16_t w, int16_t h) {
  i2c_start(OLED_ADDRESS + I2C_WRITE);

  i2c_write(0x21); // Column
  i2c_write(0);    // start
  i2c_write(127);    // end

  i2c_write(0x22); // Page address / row adress
  i2c_write(0x0); // start
  i2c_write(0x7);

  i2c_stop();

  unsigned char ret;
  uint8_t num_transmitted = 0;
  for (uint16_t page=0; page<7; page++) {
    for (uint16_t col=0; col<128; col++) {
      if (num_transmitted==0) {
        ret = i2c_start(OLED_ADDRESS + I2C_WRITE);
        i2c_write(0x40);
      }
      if (ret) {
        i2c_stop();
        PORTB |= _BV(PORTB5);
      } else {
        uint8_t segment = 0x00;
        if (col >= x && col < x+w) {
          if (y>>3 == page || (y+h)>>3 == page) {
            for (uint8_t i=0; i<8; i++) {
              if (page*8 + i >= y && page*8 + i < y+h) {
                segment |= (1 << i);
              }
            }
          }
        }
        i2c_write(segment);
        num_transmitted++;
        if (num_transmitted==16) {
          i2c_stop();
          num_transmitted = 0;
        }
      }
    }
  }
}
