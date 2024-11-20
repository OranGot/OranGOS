#pragma once
#include "multiboot_header.h"
#include "pagedir.h"
#include <stdint.h>
#define BITMAP_BYTES (PAGE_SIZE / 8)
#define KALLOCATOR_SIGNATURE 0xFA

typedef struct {
  // i thought it would be funny to add a signature
  uint8_t signature;
  uint8_t bitmap[BITMAP_BYTES];
} __attribute__((packed)) bitmap_header;
void* append_page_at_addr(uint32_t phyaddr, uint16_t flags, uint32_t* pagedir);
void *create_virtual_address(uint16_t page_dir_entry, uint16_t page_table_entry,
                             uint16_t address_into_page);
uint32_t create_page_table_entry(uint32_t address, uint16_t flags);
uint32_t *kget_page_table(uint32_t *pagedir, uint16_t id);
void kfree(void *val, uint32_t size);
void *kalloc(uint32_t size);
void kernel_allocator_setup(multiboot_info_t *mbd);
// I don't know what write through means so I'll just set it
#define DEFAULT_USER_FLAGS 0b000001111
#define DEFAULT_KERNEL_FLAGS 0b100001111
void printmem(multiboot_info_t *mbd);
