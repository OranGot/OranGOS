#include <stdint.h>
typedef struct {
  uint16_t base_low;
  uint16_t sel;
  uint8_t zero;
  uint8_t flags;
  uint16_t base_high;
} __attribute__((packed)) idt_entry;
typedef struct {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed)) idt_ptr;
void idt_init(void);
void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags);