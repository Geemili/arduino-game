
#ifndef __LEVELS_H__
#define __LEVELS_H__

#include <avr/pgmspace.h>

const PROGMEM char LEVEL_00[] =
"\x08\x04\
........\
########\
#@....!#\
########";

const PROGMEM char LEVEL_01[] =
"\x08\x04\
........\
########\
#@..k.!#\
########";

PGM_P get_level_address(uint8_t num) {
  switch (num) {
    case 0x00: return LEVEL_00;
    case 0x01: return LEVEL_01;
  }
  return LEVEL_00;
}

#endif
