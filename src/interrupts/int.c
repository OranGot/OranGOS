#include "../drivers/PS2f/ps2.h"
#include "../util/dbg.h"
#include "../util/printf.h"
#include "exep.h"
#include "pic/pic.h"
#include <stdint.h>
#define SYSCALL 0x80 - 32
void interrupt_handler(IDT_push_reg *interrupt) {
  // dbg_printf("interrupt handler triggered number %u\n",
  // interrupt->interrupt_num);
  if (interrupt->interrupt_num < 32) {
    exception_handler(interrupt);
  }
  switch (interrupt->interrupt_num - 32) {
  case 1:
    dbg_printf("PS2 handler activated\n");
    handle_ps2();
    break;
  case 0:
    // ignored for now
    // dbg_printf("Interrupt 32 triggered\n");
    break;
  case SYSCALL:
    dbg_printf("syscall triggered\n");
    /*
    not done
    */
    break;
  default:
    printf("unhandled interrupt code %u \n", interrupt->interrupt_num);
    break;
  }
  PIC_sendEOI(interrupt->interrupt_num);
  // dbg_printf("EOI sent\n");
}
