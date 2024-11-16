#include "../../include/pci.h"
#include "../../include/pio.h"
#include <stdint.h>
#include "../../include/printf.h"
#include "../../include/dbg.h"
#include "../../include/alloc.h"
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset) {
  uint32_t address;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint16_t tmp = 0;

  // Create configuration address as per Figure 1
  address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                       (offset & 0xFC) | ((uint32_t)0x80000000));

  // Write out the address
  outl(PCI_CONFIG_ADDRESS, address);
  // Read in the data
  // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
  uint32_t data;
  tmp = (uint16_t)(((data = inl(PCI_CONFIG_DATA)) >> ((offset & 2) * 8)) & 0xFFFF);
  return tmp;
}
static void print_dev(uint16_t bus, uint8_t device){
    uint16_t vendor, class, subclass;
    if ((vendor = pciConfigReadWord((uint8_t)bus, device, 0, PCI_VENDOR_ID)) == 0xFFFF) {
      //dbg_printf("device doesn't exist\n");
      return;
    }
    class = ((pciConfigReadWord((uint8_t)bus, device, 0, PCI_SUBCLASS) & 0xFF00) >> 8);
    subclass = (uint16_t)(pciConfigReadWord((uint8_t)bus, device, 0, PCI_SUBCLASS) & 0xFF);
    if((pciConfigReadWord(bus, device, 0, 8) & 0x80) != 0){
        for (uint8_t function = 1; function < 8; function++) {
            if ((vendor = pciConfigReadWord((uint8_t)bus, device, function, 0)) == 0xFFFF) {
              //dbg_printf("device doesn't exist\n");
              break;
            }
            class = ((pciConfigReadWord((uint8_t)bus, device, 0, 5) & 0xFFFF0000) >> 16);
            subclass = (pciConfigReadWord((uint8_t)bus, device, 0, 5) & 0xFFFF);
            dbg_printf("multi function device\n");
            printf("f: %hhx, v: %x\n, c: %x, sc: %x\n", function, vendor, class, subclass);
        }
    }
    printf("b: %u, d: %hhu, v: 0x%x,c: 0x%x,sc: 0x%x\n",bus,device, vendor, class, subclass);
    return;
}
void lspci(){
uint16_t bus, dev;
for(bus = 0; bus < 256;  bus++){
  for(dev = 0; dev< 32; dev++){
      print_dev(bus,  dev);
  }
}
}
void get_pci_normal_device(HEADER_TYPE_0x0* out, uint8_t bus,uint8_t slot,  uint8_t func){
    out->vendor_id =  pciConfigReadWord(bus, slot, func, PCI_VENDOR_ID);
    out->dev_id = pciConfigReadWord(bus, slot, func, PCI_DEVICE_ID);
    out->command = pciConfigReadWord(bus, slot, func, PCI_COMMAND);
    out->status = pciConfigReadWord(bus, slot, func, PCI_STATUS);
    out->revision_id = pciConfigReadWord(bus, slot, func, PCI_REVISION_ID);
    out->prog_if = pciConfigReadWord(bus, slot, func, PCI_PROG_IF);
    out->subclass = pciConfigReadWord(bus, slot, func, PCI_SUBCLASS);
    out->class_code = pciConfigReadWord(bus, slot, func, PCI_CLASS);
    out->cache_line_size = pciConfigReadWord(bus, slot, func, PCI_CACHE_LINE_SIZE);
    out->latency_timer = pciConfigReadWord(bus, slot, func, PCI_LATENCY_TIMER);
    out->header_type = pciConfigReadWord(bus, slot, func, PCI_HEADER_TYPE);
    out->BIST = pciConfigReadWord(bus, slot, func, PCI_BIST);
    out->BAR0 = pciConfigReadWord(bus, slot, func, PCI_BAR0) | (pciConfigReadWord(bus, slot, func, PCI_BAR0+2) << 16);
    out-> BAR1 = pciConfigReadWord(bus, slot, func, PCI_BAR1) | (pciConfigReadWord(bus, slot, func, PCI_BAR1+2) << 16);
    out->BAR2 = pciConfigReadWord(bus, slot, func, PCI_BAR2) | (pciConfigReadWord(bus, slot, func, PCI_BAR2+2) << 16);
    out->BAR3 = pciConfigReadWord(bus, slot, func, PCI_BAR3) | (pciConfigReadWord(bus, slot, func, PCI_BAR3+2) << 16);
    out->BAR4 = pciConfigReadWord(bus, slot, func, PCI_BAR4) | (pciConfigReadWord(bus, slot, func, PCI_BAR4+2) << 16);
    out->BAR5 = pciConfigReadWord(bus, slot, func, PCI_BAR5) | (pciConfigReadWord(bus, slot, func, PCI_BAR5+2) << 16);
    out->CardbusCISPointer = pciConfigReadWord(bus, slot, func, PCI_CARDBUS_CIS_POINTER) | (pciConfigReadWord(bus, slot, func, PCI_CARDBUS_CIS_POINTER+2) << 16);
    out->subsys_vendor_id = pciConfigReadWord(bus, slot, func, PCI_SYSTEM_VENDOR_ID);
    out->subsys_id = pciConfigReadWord(bus, slot, func, PCI_SYSTEM_ID);
    out->expansion_rom_base = pciConfigReadWord(bus, slot, func, PCI_EXP_ROM_BASE_ADDR) |(pciConfigReadWord(bus, slot, func, PCI_EXP_ROM_BASE_ADDR+2) << 16);
    out->capabilities_ptr = pciConfigReadWord(bus, slot, func, PCI_CAPABILITIES_PTR);
    out->int_line = pciConfigReadWord(bus, slot, func, PCI_INTERRUPT_LINE);
    out->int_pin = pciConfigReadWord(bus, slot, func, PCI_INTERRUPT_PIN);
    out->min_grant = pciConfigReadWord(bus, slot, func, PCI_MIN_GRANT);
    out->max_latency = pciConfigReadWord(bus, slot, func, PCI_MAX_LATENCY);
    return;
}
void print_device_normal(HEADER_TYPE_0x0* device){
    printf("vendor id: 0x%x, dev id: 0x%x, command: %u\nstatus: %u, revision id: 0x%x\n"
        "prog if: %u, subclass: 0x%x, class code: 0x%x\ncache line size 0x%x, latency timer: %u\n"
        "header type: 0x%x, BIST: 0x%x, BAR0: 0x%x, BAR1: 0x%x\nBAR2: 0x%x, BAR3: 0x%x, BAR4: 0x%x\n"
        "BAR5: 0x%x,Cardbus CIS pointer: 0x%x\nsubsystem vendor id: 0x%x, subsys id: 0x%x\n"
        "expansion rom: 0x%x, capabilities ptr: 0x%x\nint line %u, int pin %u, min grant: %u, max latency: %u\n"
        , device->vendor_id, device->dev_id, device->command,
        device->status, device->revision_id, device->prog_if, device->subclass,
        device->class_code, device->cache_line_size, device->latency_timer, device->header_type,
        device->BIST, device->BAR0, device->BAR1, device->BAR2, device->BAR3, device->BAR4, device->BAR5,
        device->CardbusCISPointer, device->subsys_vendor_id, device->subsys_id,
        device->expansion_rom_base, device->capabilities_ptr, device->int_line, device->int_pin,
        device->min_grant, device->max_latency
    );
    return;
}
