#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
#include <stdint.h>
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset);
typedef struct{
    uint16_t words[256/16];
}CONFIG_REGISTER;
//this thing is just here to pretend like it's useful, I just look at it as a cheatsheet
typedef struct{
    uint16_t dev_id, vendor_id;
    uint16_t status, command;
    uint8_t class_code, subclass, prog_if, revision_id;
    uint8_t BIST, header_type, latency_timer, cache_line_size;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t CardbusCISPointer;
    uint16_t subsys_id, subsys_vendor_id;
    uint32_t expansion_rom_base;
    uint8_t r1, r2, r3, capabilities_ptr; //r for reserved
    uint32_t r4;
    uint8_t max_latency, min_grant, int_pin, int_line;
}__attribute__((packed)) HEADER_TYPE_0x0;
void get_pci_normal_device(HEADER_TYPE_0x0* out, uint8_t bus,uint8_t slot,  uint8_t func);
void print_device_normal(HEADER_TYPE_0x0* device);
void lspci();
#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION_ID 0x08
#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0a
#define PCI_CLASS 0x0b
#define PCI_CACHE_LINE_SIZE 0x0c
#define PCI_LATENCY_TIMER 0x0d
#define PCI_HEADER_TYPE 0x0e
#define PCI_BIST 0x0f
#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_SECONDARY_BUS 0x09
#define PCI_SYSTEM_VENDOR_ID 0x2C
#define PCI_SYSTEM_ID 0x2E
#define PCI_EXP_ROM_BASE_ADDR 0x30
#define PCI_CAPABILITIES_PTR 0x34
#define PCI_INTERRUPT_LINE 0x3C
#define PCI_INTERRUPT_PIN 0x3D
#define PCI_MIN_GRANT 0x3E
#define PCI_CARDBUS_CIS_POINTER 0x28
#define PCI_MAX_LATENCY 0x3F
