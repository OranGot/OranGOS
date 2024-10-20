#include "pio.h"
#include "printf.h"
#include <stdarg.h>
#define DEBUG_PORT 0xE9
void dbg_putchar(char character, void *arg) { outb(DEBUG_PORT, character); }
void dbg_printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  fctprintf(dbg_putchar, 0, format, args);
  va_end(args);
}