/*
10 top bits index into page dir
10 bits into page table
12 remaining bits address into page
there is a page directory per process
*/

#include "../../util/memutil.h"
#include "pagedir.h"
#include <stdint.h>
extern void loadPageDirectory(uint32_t *);
extern void enablePaging();

uint32_t kernel_page_directory[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t first_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
void paging_setup(void) {
  for (uint16_t i = 0; i < PAGE_TABLE_SIZE; i++) {
    // This sets the following flags to the pages:
    //   Supervisor: Only kernel-mode can access them
    //   Write Enabled: It can be both read from and written to
    //   Not Present: The page table is not present
    kernel_page_directory[i] = 0x00000002;
  }

  for (uint32_t i = 0; i < PAGE_TABLE_SIZE; i++) {
    first_page_table[i] =
        (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
  }
  kernel_page_directory[0] = ((unsigned int)first_page_table) | 3;
  loadPageDirectory(kernel_page_directory);
  enablePaging();
}
