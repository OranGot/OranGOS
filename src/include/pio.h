#ifndef PIO_H
#define PIO_H
#include <stdint.h>
static inline void outb(uint16_t port, uint8_t val) {
  __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}
static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}
static inline void outl(uint16_t port, uint32_t value) {
  __asm__ __volatile__("outl %0, %1" : : "a"(value), "dN"(port));
}
static inline uint32_t inl(uint16_t port) {
  uint32_t ret;
  __asm__ __volatile__("inl %1, %0" : "=a"(ret) : "dN"(port));
  return ret;
}

static inline void io_wait(void) { outb(0x80, 0); }
// not gonna use that because idt already enables the interrupts
static inline void enable_interrupt(void) { __asm__ volatile("sti"); }
static inline void disable_interrupt(void) { __asm__ volatile("cli"); }

#endif