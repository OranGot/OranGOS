#include "../../include/gdt.h"
extern void gdt_flush(void *addr_t);
gdt_entry gdt_entry_inst[5];
gdt_ptr gdt_ptr_inst;
void setup_gdt() {
  setGDTgate(0, 0, 0, 0, 0);                // null segment
  setGDTgate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel code
  setGDTgate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel data
  setGDTgate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user code
  setGDTgate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user data
  gdt_flush(&gdt_ptr_inst);
}
void setGDTgate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
                uint8_t gran) {
  gdt_ptr_inst.limit = (sizeof(gdt_entry) * 5) - 1;
  gdt_ptr_inst.base = gdt_entry_inst;
  gdt_entry_inst[num].base_low = base & 0xFFFF;
  gdt_entry_inst[num].base_middle = (base >> 16) & 0xFF;
  gdt_entry_inst[num].base_high = (base >> 24) & 0xFF;

  gdt_entry_inst[num].limit = (limit & 0xFFFF);
  gdt_entry_inst[num].flags = (limit >> 16) & 0x0F;
  gdt_entry_inst[num].flags |= gran & 0xF0;
  gdt_entry_inst[num].access = access;
}
