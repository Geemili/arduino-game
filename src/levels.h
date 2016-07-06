
#ifndef __LEVELS_H__
#define __LEVELS_H__

#include <avr/pgmspace.h>

const PROGMEM char LEVEL_00[] =
"\x08\x04\
........\
########\
#@....!#\
########";

PGM_P get_level_address(uint8_t num) {
  switch (num) {
    case 0x00: return LEVEL_00;
  }
  return LEVEL_00;
}

#endif
