
#ifndef __NES_H__
#define __NES_H__

#include <stdint.h>
#include <avr/io.h>

#define NES_A       0b01111111
#define NES_B       0b10111111
#define NES_SELECT  0b11011111
#define NES_START   0b11101111
#define NES_UP      0b11110111
#define NES_DOWN    0b11111011
#define NES_LEFT    0b11111101
#define NES_RIGHT   0b11111110

void nes_pad_init();
void nes_pad_update();
uint8_t nes_pad_get_data();
uint8_t nes_pad_is_pressed(uint8_t button);
uint8_t nes_pad_just_released(uint8_t button);
uint8_t nes_pad_just_pressed(uint8_t button);

#endif
