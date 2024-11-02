#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
#include <stdint.h>
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset);
uint8_t pciReadRegisterOffset(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pciReadFunctionNumber(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pciReadDeviceNumber(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pciReadDeviceBusNumber(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pciReadEnableBit(uint8_t bus, uint8_t slot, uint8_t func);