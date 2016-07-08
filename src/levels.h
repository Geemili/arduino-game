
#ifndef __LEVELS_H__
#define __LEVELS_H__

#include <avr/pgmspace.h>

const PROGMEM char LEVEL_00[] =
"\x08\x03\
########\
#@....!#\
########";

const PROGMEM char LEVEL_01[] =
"\x08\x03\
########\
#@..k.!#\
########";

const PROGMEM char LEVEL_02[] =
"\x08\x03\
####a###\
#@..k0!#\
########";

const PROGMEM char LEVEL_03[] =
"\x08\x04\
##a#####\
#@k#k#!#\
#..0.1.#\
##b#####";

const PROGMEM char LEVEL_04[] =
"\x08\x05\
##b#####\
#@.l.1!#\
#.######\
#...0.k#\
##f#####";

const PROGMEM char LEVEL_05[] =
"\x08\x05\
###f####\
#@.l21!#\
#.#b####\
#.l.0.k#\
####h###";

const PROGMEM char LEVEL_FF[] =
"\x09\x05\
#a##b##h#\
#@......#\
#.......#\
#0kl1.m2#\
#########";

PGM_P get_level_address(uint8_t num) {
  switch (num) {
    case 0x00: return LEVEL_00;
    case 0x01: return LEVEL_01;
    case 0x02: return LEVEL_02;
    case 0x03: return LEVEL_03;
    case 0x04: return LEVEL_04;
    case 0x05: return LEVEL_05;
    case 0xFF: return LEVEL_FF;
  }
  return LEVEL_00;
}

#endif
