#define PIT_MODE_COMMAND 0x43
#define PIT_CH0_RW 0x40
#include "../../include/pio.h"
#include <stdint.h>
#define PIT_LATCH_COMMAND 0xCC000000
void PIT_handle_pit(void) {}
#define PIT_DEFAULT_CHANNEL 0b00
#define PIT_DEFAULT_ACCESS_MODE
#define PIT_DEFAULT_OPERATING_MODE
void PIT_set_state(char state) { outb(PIT_MODE_COMMAND, state); }
//well this doesn't work
void PIT_disable_pit(void) { outl(PIT_MODE_COMMAND, PIT_LATCH_COMMAND); }
