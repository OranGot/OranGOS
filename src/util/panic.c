#include "printf.h"
__attribute__((noreturn)) void panic(const char *msg) {
  printf("KERNEL PANIC %s", msg);
  __asm__ volatile("cli; hlt");
}