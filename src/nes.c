
#include "nes.h"
#include <util/delay.h>

#define pinmode_output(DIGITAL_PIN) DIGITAL_PIN ## _DDR |= DIGITAL_PIN ## _BIT
#define pinmode_input(DIGITAL_PIN) DIGITAL_PIN ## _DDR &= ~ DIGITAL_PIN ## _BIT
#define set_high(DIGITAL_PIN) DIGITAL_PIN ## _PORT |= DIGITAL_PIN ## _BIT
#define set_low(DIGITAL_PIN) DIGITAL_PIN ## _PORT &= ~ DIGITAL_PIN ## _BIT
#define read_pin(DIGITAL_PIN) ((DIGITAL_PIN ## _PIN & DIGITAL_PIN ## _BIT) == DIGITAL_PIN ## _BIT)
#define BIT(x) (0x01 << (x))

#ifndef NES_LATCH
#define NES_LATCH
// Default to pin D2
#define NES_LATCH_PORT PORTD
#define NES_LATCH_BIT BIT(2)
#define NES_LATCH_DDR DDRD
#endif

#ifndef NES_CLOCK
#define NES_CLOCK
// Default to pin D3
#define NES_CLOCK_PORT PORTD
#define NES_CLOCK_BIT BIT(3)
#define NES_CLOCK_DDR DDRD
#endif

#ifndef NES_DATA_IN
#define NES_DATA_IN
// Default to pin D4
#define NES_DATA_IN_PORT PORTD
#define NES_DATA_IN_BIT BIT(4)
#define NES_DATA_IN_DDR DDRD
#define NES_DATA_IN_PIN PIND
#endif

static uint8_t nes_pad_prev_data = 0;
static uint8_t nes_pad_data = 0;

void nes_pad_init() {
  pinmode_output(NES_LATCH);
  pinmode_output(NES_CLOCK);
  pinmode_input(NES_DATA_IN);

  set_high(NES_LATCH);
  set_high(NES_CLOCK);
}

void nes_pad_update() {
  nes_pad_prev_data = nes_pad_data;
  nes_pad_data = 0;

  set_low(NES_LATCH);
  set_low(NES_CLOCK);

  set_high(NES_LATCH);
  _delay_us(2);
  set_low(NES_LATCH);

  nes_pad_data = read_pin(NES_DATA_IN);

  for (uint8_t i = 1; i <= 7; i++) {
    set_high(NES_CLOCK);
    _delay_us(2);
    nes_pad_data <<= 1;
    nes_pad_data += read_pin(NES_DATA_IN);
    _delay_us(4);
    set_low(NES_CLOCK);
  }
}

uint8_t nes_pad_get_data() {
  return nes_pad_data;
}

uint8_t nes_pad_is_pressed(uint8_t button) {
  return (nes_pad_data | button) == button;
}

uint8_t nes_pad_just_released(uint8_t button) {
  return ((nes_pad_prev_data | button) == button) &&
          !((nes_pad_data | button) == button);
}

uint8_t nes_pad_just_pressed(uint8_t button) {
  return !((nes_pad_prev_data | button) == button) &&
          ((nes_pad_data | button) == button);
}
