#include "../../util/dbg.h"
#include "../../util/pio.h"
#include "../../util/printf.h"
#include <stdbool.h>
#include <stdint.h>
bool is_caps = false, is_lshift = false, is_rshift = false, is_ctrl = false;
static bool is_uppercase(void) {
  if (is_caps && is_lshift && !is_rshift)
    return false;
  else if (is_caps && !is_lshift && !is_rshift)
    return true;
  else if (!is_caps && is_lshift && !is_rshift)
    return true;
  else if (!is_caps && !is_lshift && is_rshift)
    return true;
  else
    return false;
}
#define PS2_DATA_PORT 0x60
#define PS2_COMMAND_PORT 0x64
#define PS2_MAX_ACK_RETRIES 3
bool is_dual_port = false;
typedef enum {
  PS2_CCMD_READ_B0_FROM_INTERNAL_RAM = 0x20,
  PS2_CCMD_READ_N_FROM_INTERNAL_RAM = 0x21, // for n = 0
  PS2_CCMD_WRITE_B0_TO_INTERNAL_RAM = 0x60,
  PS2_CCMD_WRITE_N_TO_INTERNAL_RAM = 0x61, // for n = 0
  PS2_CCMD_DISABLE_SEC_PS2_PORT = 0xA7,
  PS2_CCMD_ENABLE_SEC_PS2_PORT = 0xA8,
  PS2_CCMD_TEST_SEC_PS2_PORT = 0xA9,
  PS2_CCMD_TEST_FIRST_PS2_PORT = 0xAB,
  PS2_CCMD_DIAGNOSTIC_DUMP = 0xAC,
  PS2_CCMD_DISABLE_FIRST_PORT = 0xAD,
  PS2_CCMD_ENABLE_FIRST_PORT = 0xAE,
  PS2_CCMD_READ_CONTROLLER_INPUT_PORT = 0xC0,
  PS2_CCMD_COPY_0t3_INPUT_PORT_T_STATUS_4t7 = 0xC1,
  PS2_CCMD_COPY_4t7_INPUT_PORT_T_STATUS_4t7 = 0xC2,
  PS2_CCMD_READ_OUT = 0xD0,
  PS2_CCMD_WRITE_NEXT_CONTROLLER_OUT = 0xD1,
  PS2_CCMD_WRITE_NEXT_1PS2 = 0xD2,
  PS2_CCMD_WRITE_NEXT_2PS2 = 0xD3,
  PS2_CCMD_WRITE_NEXT_t_2PS2_IN = 0xD4,
  PS2_CCMD_PULSE_OUTPUT_LINE_LOW = 0xF0,
} PS2_CONTROLLER_COMMAND;
// typedef enum {
//   PSC_SET_LEDS = 0xED,
//   PSC_ECHO = 0xEE,
//   PSC_GET_CUR_SCAN_CODE_SET = 0xF0,
//   PSC_ID_KEYBOARD = 0xF2,
//   PSC_SET_TYPEMATIC_RATE_AND_DELAY = 0xF3,
//   PSC_ENABLE_SCANNING = 0xF4,
//   PSC_DISABLE_SCANNING = 0xF5,
//   PSC_SET_DEFAULTS = 0xF4,
//   PSC_SET_KEYS_TO_AUTOREPEAT = 0xF7,
//   PSC_SET_ALL_MAKE_RELEASE = 0xF9,
//   PSC_SET_SPECIFIC_AUTOREPEAT = 0xFB,
//   PSC_SET_SPECIFIC_MAKE_RELEASE = 0xFC,
//   PSC_SET_SPECIFIC_MAKE = 0xFD,
//   PSC_RESEND = 0xFE,
//   PSC_RESET_SELF_TEST = 0xFF,
// } PS2_COMMAND;
typedef enum {
  PSR_ACK = 0xFA,
  PSR_RESEND = 0xFE,
  PSR_SELF_TEST_PASS = 0xAA,
  PSR_KEY_DETECTION_ERROR = 0x00,
  PSR_SELF_TEST_FAIL = 0xFC,
  PSR_SELF_TEST_FAIL2 = 0xFD,
  PSR_KEY_DETECTION_ERROR2 = 0xFF,

} PS2_RESPONSE;
typedef enum {
  PESC = 0x01,
  PONE = 0x02,
  PS2_S1_PTWO = 0x03,
  PS2_S1_PTHREE = 0x04,
  PS2_S1_PFOUR = 0x05,
  PS2_S1_PFIVE = 0x06,
  PS2_S1_PSIX = 0x07,
  PS2_S1_PSEVEN = 0x08,
  PS2_S1_PEIGHT = 0x09,
  PS2_S1_PNINE = 0x0A,
  PS2_S1_PZERO = 0x0B,
  PS2_S1_PMINUS = 0x0C,
  PS2_S1_PEQ = 0x0D,
  PS2_S1_PBACKSPACE = 0x0E,
  PS2_S1_PTAB = 0x0F,
  PS2_S1_PQ = 0x10,
  PS2_S1_PW = 0x11,
  PS2_S1_PE = 0x12,
  PS2_S1_PR = 0x13,
  PS2_S1_PT = 0x14,
  PS2_S1_PY = 0x15,
  PS2_S1_PU = 0x16,
  PS2_S1_PI = 0x17,
  PS2_S1_PO = 0x18,
  PS2_S1_PP = 0x19,
  PS2_S1_PSQ_BR_L = 0x1A,
  PS2_S1_PSQ_BR_R = 0x1B,
  PS2_S1_PENTER = 0x1C,
  PS2_S1_PLCTRL = 0x1D,
  PS2_S1_PA = 0x1E,
  PS2_S1_PS = 0x1F,
  PS2_S1_PD = 0x20,
  PS2_S1_PF = 0x21,
  PS2_S1_PG = 0x22,
  PS2_S1_PH = 0x23,
  PS2_S1_PJ = 0x24,
  PS2_S1_PK = 0x25,
  PS2_S1_PL = 0x26,
  PS2_S1_PSEMICOLON = 0x27,
  PS2_S1_PSING_QUOTE = 0x28,
  PS2_S1_PBACK_TICK = 0x29,
  PS2_S1_PLSHIFT = 0x2A,
  PS2_S1_PBACKSLASH = 0x2B,
  PS2_S1_PZ = 0x2C,
  PS2_S1_PX = 0x2D,
  PS2_S1_PC = 0x2E,
  PS2_S1_PV = 0x2F,
  PS2_S1_PB = 0x30,
  PS2_S1_PN = 0x31,
  PS2_S1_PM = 0x32,
  PS2_S1_PCOMMA = 0x33,
  PS2_S1_PDOT = 0x34,
  PS2_S1_PSLASH = 0x35,
  PS2_S1_PRSHIFT = 0x36,
  PS2_S1_PKEYP_STAR = 0x37,
  PS2_S1_PLALT = 0x38,
  PS2_S1_PSPACE = 0x39,
  PS2_S1_PCAPS_LOCK = 0x3A,
  PS2_S1_PF1 = 0x3B,
  PS2_S1_PF2 = 0x3C,
  PS2_S1_PF3 = 0x3D,
  PS2_S1_PF4 = 0x3E,
  PS2_S1_PF5 = 0x3F,
  PS2_S1_PF6 = 0x40,
  PS2_S1_PF7 = 0x41,
  PS2_S1_PF8 = 0x42,
  PS2_S1_PF9 = 0x43,
  PS2_S1_PF10 = 0x44,
  PS2_S1_PF11 = 0x57,
  PS2_S1_PF12 = 0x58,
  PS2_S1_RLCTRL = 0x9D,
  PS2_S1_RLSHIFT = 0xAA,
  PS2_S1_RRSHIFT = 0xB6,
} PS2_SCAN_CODES1;
typedef enum {
  PS2_ANCIENT_AT_KEYBOARD,
  PS2_STANDART_PS2_MOUSE,
  PS2_MOUSE_WITH_SCROLL,
  PS2_FIVE_BUTTON_MOUSE,
  PS2_MF2_KEYBOARD,
  PS2_SHORT_KEYBOARD,
  PS2_N_97_OR_122_KEY_HOST,
  PS2_112_KEY_KEYBOARD,
  PS2_JAPANESE_G,
  PS2_JAPANESE_P,
  PS2_JAPANESE_A,
  PS2_NCD_SUN_LAYOUT,
} PS2_DEVICE;
uint8_t __ensure_ack(uint8_t command, uint16_t data) {
  for (uint8_t i = 0; i < PS2_MAX_ACK_RETRIES; i++) {
    io_wait();
    uint8_t response = inb(PS2_COMMAND_PORT);
    if (response == PSR_RESEND) {
      return __ensure_ack(command, data);
    } else {
      return response;
    }
  }
  return 0;
}
uint8_t get_status() {
  outb(PS2_COMMAND_PORT, PS2_CCMD_READ_B0_FROM_INTERNAL_RAM);
  return inb(PS2_DATA_PORT);
}
char __ret_ps2_key(uint8_t key) {
  const bool is_upper = is_uppercase();
  switch (key) {
  case PS2_S1_PW:
    if (is_upper)
      return 'W';
    return 'w';
    break;
  case PS2_S1_PQ:
    if (is_upper)
      return 'Q';
    return 'q';
    break;
  case PS2_S1_PEIGHT:
    if (is_upper)
      return '*';
    return '8';
    break;
  case PS2_S1_PEQ:
    if (is_upper)
      return '+';
    return '=';
    break;
  case PS2_S1_PFIVE:
    if (is_upper)
      return '%';
    return '5';
    break;
  case PS2_S1_PFOUR:
    if (is_upper)
      return '4';
    return '4';
    break;
  case PS2_S1_PMINUS:
    if (is_upper)
      return '_';
    return '-';
    break;
  case PS2_S1_PNINE:
    if (is_upper)
      return '(';
    return '9';
    break;
  case PS2_S1_PSEVEN:
    if (is_upper)
      return '&';
    return '7';
    break;
  case PS2_S1_PSIX:
    if (is_upper)
      return '^';
    return '6';
    break;
  case PS2_S1_PTAB:
    return '\t';
    break;
  case PS2_S1_PTHREE:
    if (is_upper)
      return '#';
    return '3';
    break;
  case PS2_S1_PTWO:
    if (is_upper)
      return '@';
    return '2';
    break;
  case PS2_S1_PBACKSPACE:
    return '\b';
    break;
  case PS2_S1_PZERO:
    if (is_upper)
      return ')';
    return '0';
    break;
  case PS2_S1_PE:
    if (is_upper)
      return 'E';
    return 'e';
    break;
  case PS2_S1_PT:
    if (is_upper)
      return 'T';
    return 't';
    break;
  case PS2_S1_PY:
    if (is_upper)
      return 'Y';
    return 'y';
    break;
  case PS2_S1_PU:
    if (is_upper)
      return 'U';
    return 'u';
    break;
  case PS2_S1_PI:
    if (is_upper)
      return 'I';
    return 'i';
    break;
  case PS2_S1_PO:
    if (is_upper)
      return 'O';
    return 'o';
    break;
  case PS2_S1_PP:
    if (is_upper)
      return 'P';
    return 'p';
    break;
  case PS2_S1_PSQ_BR_L:
    if (is_upper)
      return '{';
    return '[';
    break;
  case PS2_S1_PSQ_BR_R:
    if (is_upper)
      return '}';
    return ']';
    break;
  case PS2_S1_PENTER:
    return '\n';
    break;
  case PS2_S1_PLCTRL:
    return 'w';
    break;
  case PS2_S1_PA:
    if (is_upper)
      return 'A';
    return 'a';
    break;
  case PS2_S1_PS:
    if (is_upper)
      return 'S';
    return 's';
    break;
  case PS2_S1_PD:
    if (is_upper)
      return 'D';
    return 'd';
  case PS2_S1_PF:
    if (is_upper)
      return 'F';
    return 'f';
    break;
  case PS2_S1_PG:
    if (is_upper)
      return 'G';
    return 'g';
    break;
  case PS2_S1_PH:
    if (is_upper)
      return 'H';
    return 'h';
    break;
  case PS2_S1_PJ:
    if (is_upper)
      return 'J';
    return 'j';
    break;
  case PS2_S1_PK:
    if (is_upper)
      return 'K';
    return 'k';
    break;
  case PS2_S1_PL:
    if (is_upper)
      return 'L';
    return 'l';
    break;
  case PS2_S1_PSEMICOLON:
    if (is_upper)
      return ':';
    return ';';
    break;
  case PS2_S1_PSING_QUOTE:
    if (is_upper)
      return '"';
    return '\'';
    break;
  case PS2_S1_PBACK_TICK:
    if (is_upper)
      return '~';
    return '`';
    break;
  case PS2_S1_PLSHIFT:
    is_lshift = true;
    return 0;
    break;
  case PS2_S1_PBACKSLASH:
    if (is_upper)
      return '|';
    return '\\';
    break;
  case PS2_S1_PZ:
    if (is_upper)
      return 'Z';
    return 'z';
    break;
  case PS2_S1_PX:
    if (is_upper)
      return 'X';
    return 'x';
    break;
  case PS2_S1_PC:
    if (is_upper)
      return 'C';
    return 'c';
    break;
  case PS2_S1_PV:
    if (is_upper)
      return 'V';
    return 'v';
    break;
  case PS2_S1_PB:
    if (is_upper)
      return 'B';
    return 'b';
    break;
  case PS2_S1_PN:
    if (is_upper)
      return 'N';
    return 'n';
    break;
  case PS2_S1_PM:
    if (is_upper)
      return 'M';
    return 'm';
    break;
  case PS2_S1_PCOMMA:
    if (is_upper)
      return '<';
    return ',';
    break;
  case PS2_S1_PDOT:
    if (is_upper)
      return '>';
    return '.';
    break;
  case PS2_S1_PSLASH:
    if (is_upper)
      return '?';
    return '/';
    break;
  case PS2_S1_PRSHIFT:
    is_rshift = true;
    return 0;
    break;
  case PS2_S1_PKEYP_STAR:
    return '*';
    break;
  case PS2_S1_PLALT:
    return 'w';
    break;
  case PS2_S1_PSPACE:
    return ' ';
    break;
  case PS2_S1_PCAPS_LOCK:
    if (is_caps)
      is_caps = false;
    else
      is_caps = true;
    return 0;
    break;
  case PS2_S1_PF1:
    return 'w';
    break;
  case PS2_S1_PF2:
    return 'w';
    break;
  case PS2_S1_PF3:
    return 'w';
    break;
  case PS2_S1_PF4:
    return 'w';
    break;
  case PS2_S1_PF5:
    return 'w';
    break;
  case PS2_S1_PF6:
    return 'w';
    break;
  case PS2_S1_PF7:
    return 'w';
    break;
  case PS2_S1_PF8:
    return 'w';
    break;
  case PS2_S1_PF9:
    return 'w';
    break;
  case PS2_S1_PF10:
    return 'w';
    break;
  case PS2_S1_PF11:
    return 'w';
    break;
  case PS2_S1_PF12:
    return 'w';
    break;
  case PS2_S1_PR:
    if (is_upper)
      return 'R';
    return 'r';
    break;
  case PS2_S1_RLSHIFT:
    is_lshift = false;
    return 0;
  case PS2_S1_RRSHIFT:
    is_rshift = false;
    return 0;
  default:
    return 0;
  }
}

int init_ps2(void) {
  // sep 3
  outb(PS2_COMMAND_PORT, PS2_CCMD_DISABLE_FIRST_PORT);
  outb(PS2_COMMAND_PORT, PS2_CCMD_DISABLE_SEC_PS2_PORT);
  // step 4
  const uint8_t discard = inb(PS2_DATA_PORT);
  // step 5
  outb(PS2_COMMAND_PORT, PS2_CCMD_READ_B0_FROM_INTERNAL_RAM);
  const uint8_t ccb = inb(PS2_DATA_PORT);
  // step 6
  outb(PS2_COMMAND_PORT, 0xAA);
  const uint8_t self_test_result = inb(PS2_DATA_PORT);
  if (self_test_result != 0x55) {
    dbg_printf("PS2 self test failed\n");
    return -1;
  }
  // step 7
  outb(PS2_COMMAND_PORT, PS2_CCMD_ENABLE_SEC_PS2_PORT);
  outb(PS2_COMMAND_PORT, PS2_CCMD_READ_B0_FROM_INTERNAL_RAM);
  uint8_t cb = inb(PS2_DATA_PORT);
  // printf("oldcb: %08b\n", cb);
  if (!((cb & 0b00100000) >> 5)) {
    is_dual_port = true;
    outb(PS2_COMMAND_PORT, PS2_CCMD_DISABLE_SEC_PS2_PORT);
    outb(PS2_COMMAND_PORT, PS2_CCMD_WRITE_B0_TO_INTERNAL_RAM);
    uint8_t newcb = cb ^ 0b100010;
    // printf("newcb: %08b\n", newcb);
    outb(PS2_DATA_PORT, newcb);
  }
  // step 8
  outb(PS2_COMMAND_PORT, PS2_CCMD_TEST_FIRST_PS2_PORT);
  uint8_t sec_test_res = inb(PS2_DATA_PORT);
  if (sec_test_res) {
    printf("PS2 self test fail 2 %02x\n", sec_test_res);
    dbg_printf("self test fail 2\n");
    return -1;
  };
  if (is_dual_port) {
    outb(PS2_COMMAND_PORT, PS2_CCMD_TEST_SEC_PS2_PORT);
    if (inb(PS2_DATA_PORT)) {
      dbg_printf("self test fail 3\n");
      return -1;
    }
  }
  // step 9
  outb(PS2_COMMAND_PORT, PS2_CCMD_ENABLE_FIRST_PORT);
  outb(PS2_COMMAND_PORT, PS2_CCMD_ENABLE_SEC_PS2_PORT);
  uint8_t mask;
  if (is_dual_port) {
    mask = 0b11;
  } else {
    mask = 0b1;
  }
  outb(PS2_COMMAND_PORT, PS2_CCMD_WRITE_B0_TO_INTERNAL_RAM);
  outb(PS2_DATA_PORT, get_status() | mask);
  return 0;
}
void handle_ps2(void) {
  uint8_t key = inb(PS2_DATA_PORT);
  uint8_t c = __ret_ps2_key(key);
  if (c != 0) {
    _putchar(c);
  }
}
