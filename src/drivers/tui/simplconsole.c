// depreciated!!! not used in build process
#include "../../include/pio.h"
#include <stdint.h>
#define PS2_DATA 0x60
#define PS2_COMMAND 0x64
uint8_t lastch = 0;
uint8_t is_lshift = 0;
uint8_t is_caps = 0;
uint8_t is_rshift = 0;
uint8_t is_alt = 0;
char ret_ps2(void) {
  const uint8_t PS2_out = inb(PS2_DATA);
  if (PS2_out == lastch) {
    return 0;
  }
  lastch = PS2_out;
  switch (PS2_out) {
  case 0x01:
    return 0x1B;
  case 0x02:
    return '1';
  case 0x03:
    return '2';
  case 0x04:
    return '3';
  case 0x05:
    return '4';
  case 0x06:
    return '5';
  case 0x07:
    return '6';
  case 0x08:
    return '7';
  case 0x09:
    return '8';
  case 0x0A:
    return '9';
  case 0x0B:
    return '0';
  case 0x0C:
    return '-';
  case 0x0D:
    return '=';
  case 0x0E:
    return '\b';
  case 0x0F:
    return '\t';
  case 0x10:
    return 'q';
  case 0x11:
    return 'w';
  case 0x12:
    return 'e';
  case 0x13:
    return 'r';
  case 0x14:
    return 't';
  case 0x15:
    return 'y';
  case 0x16:
    return 'u';
  case 0x17:
    return 'i';
  case 0x18:
    return 'o';
  case 0x19:
    return 'p';
  case 0x1A:
    return '[';
  case 0x1B:
    return ']';
  case 0x1C:
    return '\n';
  case 0x1D:
    return 0;
  case 0x1E:
    return 'a';
  case 0x1F:
    return 's';
  case 0x20:
    return 'd';
  case 0x21:
    return 'f';
  case 0x22:
    return 'g';
  case 0x23:
    return 'h';
  case 0x24:
    return 'j';
  case 0x25:
    return 'k';
  case 0x26:
    return 'l';
  case 0x27:
    return ';';
  case 0x28:
    return '\'';
  case 0x29:
    return '`';
  case 0x2A:
    is_lshift = 1;
    return 0;
  case 0x2B:
    return '\\';
  case 0x2C:
    return 'z';
  case 0x2D:
    return 'x';
  case 0x2E:
    return 'c';
  case 0x2F:
    return 'v';
  case 0x30:
    return 'b';
  case 0x31:
    return 'n';
  case 0x32:
    return 'm';
  case 0x33:
    return ',';
  case 0x34:
    return '.';
  case 0x35:
    return '/';
  case 0x36:
    is_rshift = 1;
    return 0;
  case 0x37:
    return '*';
  case 0x38:
    is_alt = 1;
    return 0;
  case 0x39:
    return ' ';
  case 0x3A:
    is_caps = 1;
    return 0;
  case 0x3B: // F1
    return 0;
  case 0x3C: // F2
    return 0;
  case 0x3D: // F3
    return 0;
  case 0x3E: // F4
    return 0;
  case 0x3F: // F5
    return 0;
  case 0x40: // F6
    return 0;
  // case 0x:
  //     return 0x1B;
  default:
    return 0;
  };
}
