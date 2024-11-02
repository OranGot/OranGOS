#include "AHCI.h"
#include "../PCI/pci.h"
void PCI_scan_for_ahci() {
  for (uint8_t bus = 0; bus < 256; bus++) {
    for (uint8_t device = 0; device < 32; device++) {
      for (uint8_t function = 0; function < 8; function++) {
        uint16_t vendor_id = pci_get_vendor_id(bus, device, function);
        if (vendor_id != 0xFFFF) { // Check if a device is present
          uint8_t class_code = pci_get_class_code(bus, device, function);
          uint8_t subclass_code = pci_get_subclass_code(bus, device, function);
          uint8_t prog_if = pci_get_prog_if(bus, device, function);

          // Check if it's an AHCI controller
          if (class_code == 0x01 && subclass_code == 0x06 && prog_if == 0x01) {
            printf("AHCI controller found on Bus %d, Device %d, Function %d\n",
                   bus, device, function);
            // Now you have found the AHCI controller, and you can start
            // interacting with it.
            return;
          }
        }
      }
    }
  }
  printf("No AHCI controller found\n");
}