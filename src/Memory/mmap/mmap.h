#pragma once
#include <stdint.h>
void *get_free_page();
void* kget_free_page();
int map_page_at_addr(uint32_t phy_addr, uint32_t* pagedir, uint32_t vaddr, uint16_t flags);
void remap_kernel();
