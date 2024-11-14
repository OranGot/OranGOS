#include "../util/dbg.h"
#include "int.h"
#include <stdbool.h>
#include <stdint.h>

#define IDT_MAX_DESCRIPTORS 256
extern void *isr_stub_table[IDT_MAX_DESCRIPTORS];
static bool vectors[IDT_MAX_DESCRIPTORS];
__attribute__((aligned(0x10))) idt_entry idt[256];
idt_ptr idtr;
void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
  idt_entry *descriptor = &idt[vector];

  descriptor->base_low = (uint32_t)isr & 0xFFFF;
  descriptor->sel = 0x08; // this value can be whatever offset your kernel
                          // code selector is in your GDT
  descriptor->flags = flags;
  descriptor->base_high = (uint32_t)isr >> 16;
  descriptor->zero = 0;
}
void idt_init() {
  idtr.base = (uintptr_t)&idt[0];
  idtr.limit = (uint16_t)sizeof(idt_entry) * IDT_MAX_DESCRIPTORS - 1;

  for (uint16_t vector = 0; vector < IDT_MAX_DESCRIPTORS; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    vectors[vector] = true;
  }
  __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT
  __asm__ volatile("sti");                   // set the interrupt flag
}
