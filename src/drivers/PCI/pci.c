#include "pci.h"
#include "../../util/pio.h"
#include <stdint.h>
/*
 * offset:
 * 0 => 8 lower - register offset, 3 bits - function number, higher 5 - device
 * 1 => lower 8 bits - bus number, 7 bits reserved, upper 1 bit - enable bit
 */
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
  tmp = (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
  return tmp;
}
uint8_t pciReadRegisterOffset(uint8_t bus, uint8_t slot, uint8_t func) {
  return (uint8_t)pciConfigReadWord(bus, slot, func, 0);
}
uint8_t pciReadFunctionNumber(uint8_t bus, uint8_t slot, uint8_t func) {
  return (uint8_t)(pciConfigReadWord(bus, slot, func, 0) >> 8) & 0b00000111;
}
uint8_t pciReadDeviceNumber(uint8_t bus, uint8_t slot, uint8_t func) {
  return (uint8_t)(pciConfigReadWord(bus, slot, func, 0) >> 11);
}
uint8_t pciReadDeviceBusNumber(uint8_t bus, uint8_t slot, uint8_t func) {
  return (uint8_t)pciConfigReadWord(bus, slot, func, 1);
}
uint8_t pciReadEnableBit(uint8_t bus, uint8_t slot, uint8_t func) {
  return (uint8_t)(pciConfigReadWord(bus, slot, func, 1) >> 15);
}