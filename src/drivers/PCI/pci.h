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