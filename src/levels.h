
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

const PROGMEM char LEVEL_02[] =
"\x08\x04\
........\
####a###\
#@..k0!#\
########";

const PROGMEM char LEVEL_03[] =
"\x08\x04\
##a#####\
#@k#k#!#\
#..0.1.#\
##b#####";

PGM_P get_level_address(uint8_t num) {
  switch (num) {
    case 0x00: return LEVEL_00;
    case 0x01: return LEVEL_01;
    case 0x02: return LEVEL_02;
    case 0x03: return LEVEL_03;
  }
  return LEVEL_00;
}

#endif
