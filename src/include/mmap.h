#pragma once
#include <stdint.h>
#include "multiboot_header.h"
// void *get_free_page();
// void* kget_free_page();
int map_page_at_addr(uint32_t phy_addr, uint32_t* pagedir, uint32_t vaddr, uint16_t flags);
void* append_page(uint32_t* pagedir, uint32_t phyaddr, uint16_t flags);
int reserve_addr(uint32_t phy);
void* get_free_phy_page();
void remap_kernel();
void setup_pageframe_allocator(multiboot_info_t *mbd);
// everything above 1 is a breakpoint id
#define MMAP_FREE -2
#define MMAP_OCCUPIED -1
