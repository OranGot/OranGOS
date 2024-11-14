#include <stdint.h>

void tlb_flush_one(uint32_t virtual_address) {
  asm volatile("TLBFLSH   %0\n\t" ::"r"(virtual_address));
}
