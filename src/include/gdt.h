#include <stdint.h>
typedef struct {
  uint16_t limit;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t flags;
  uint8_t base_high;
} __attribute__((packed)) gdt_entry;
typedef struct {
  uint16_t limit;
  gdt_entry *base;
} __attribute__((packed)) gdt_ptr;
void setGDTgate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
                uint8_t gran);
void setup_gdt();