/*
10 top bits index into page dir
10 bits into page table
12 remaining bits address into page
there is a page directory per process
*/
#include "../../include/printf.h"
#include "../../include/memutil.h"
#include "../../include/alloc.h"

#include "../../include/kerninfo.h"
#include "../../include/pagedir.h"
#include <stdint.h>
#include "../../include/dbg.h"
extern void loadPageDirectory(uint32_t *);
extern void enablePaging();
#define KERNEL_OFFSET_IN_PAGES (KERNEL_BASE_OFFSET / PAGE_SIZE)
#define KERNEL_OFFSET_IN_PAGE_TABLES 0
uint32_t kernel_page_directory[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t first_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
//I'll preallocate 15 pages of the kernel so it will be easier to remap
uint32_t kernel_page_table1[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table2[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table3[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table4[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table5[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table6[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table7[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table8[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table9[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table10[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table11[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table12[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table13[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table14[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
uint32_t kernel_page_table15[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
#include "../../include/panic.h"
void paging_setup(void) {
  for (uint16_t i = 0; i < PAGE_TABLE_SIZE; i++) {
    // This sets the following flags to the pages:
    //   Supervisor: Only kernel-mode can access them
    //   Write Enabled: It can be both read from and written to
    //   Not Present: The page table is not present
    kernel_page_directory[i] = 0x00000002;
  }
  dbg_printf("page directory setup!\n");
  for (uint32_t i = 0; i < PAGE_TABLE_SIZE; i++) {
    // attributes: supervisor level, read/write, present.
    first_page_table[i] = (i * PAGE_SIZE) | 3;
    //kernel_page_table1[i] |= (3 | (PAGE_TABLE_DISTANCE * 0 + i * PAGE_SIZE));
    kernel_page_table2[i] = (3 | (PAGE_TABLE_DISTANCE * 1 + i * PAGE_SIZE));
    kernel_page_table3[i] = (3 | (PAGE_TABLE_DISTANCE * 2 + i * PAGE_SIZE));
    kernel_page_table4[i] = (3 | (PAGE_TABLE_DISTANCE * 3 + i * PAGE_SIZE));
    // kernel_page_table5[i] = (3 | (PAGE_TABLE_DISTANCE * 4 + i * PAGE_SIZE));
    // kernel_page_table6[i] = (3 | (PAGE_TABLE_DISTANCE * 5 + i * PAGE_SIZE));
    // kernel_page_table7[i] = (3 | (PAGE_TABLE_DISTANCE * 6 + i * PAGE_SIZE));
    // kernel_page_table8[i] = (3 | (PAGE_TABLE_DISTANCE * 7 + i * PAGE_SIZE));
    // kernel_page_table9[i] = (3 | (PAGE_TABLE_DISTANCE * 8 + i * PAGE_SIZE));
    // kernel_page_table10[i] |= DEFAULT_KERNEL_FLAGS;
    // kernel_page_table11[i] |= DEFAULT_KERNEL_FLAGS;
    // kernel_page_table12[i] |= DEFAULT_KERNEL_FLAGS;
    // kernel_page_table13[i] |= DEFAULT_KERNEL_FLAGS;
    // kernel_page_table14[i] |= DEFAULT_KERNEL_FLAGS;
    // kernel_page_table15[i] |= DEFAULT_KERNEL_FLAGS;
  }
  dbg_printf("page tables' setup finished\n");
  printf("Kernel offset in page tables = %u\n", (uint32_t)KERNEL_OFFSET_IN_PAGE_TABLES);
  kernel_page_directory[0] = ((uint32_t)first_page_table) | 3;
  //kernel_page_directory[KERNEL_OFFSET_IN_PAGE_TABLES] = ((uint32_t)kernel_page_table1) | 3;
  kernel_page_directory[1] = ((uint32_t)kernel_page_table1) | 3;
  kernel_page_directory[2] = ((uint32_t)kernel_page_table2) | 3;
  kernel_page_directory[3] = ((uint32_t)kernel_page_table3) | 3;
  kernel_page_directory[4] = ((uint32_t)kernel_page_table4) | 3;
  // kernel_page_directory[5] = ((uint32_t)kernel_page_table5) | 3;
  // kernel_page_directory[6] = ((uint32_t)kernel_page_table6) | 3;
  // kernel_page_directory[7] = ((uint32_t)kernel_page_table7) | 3;
  // kernel_page_directory[8] = ((uint32_t)kernel_page_table8) | 3;
  // kernel_page_directory[9] = ((uint32_t)kernel_page_table9) | 3;
  // kernel_page_directory[KERNEL_OFFSET_IN_PAGE_TABLES+10] = ((uint32_t)kernel_page_table10) | 3;
  // kernel_page_directory[KERNEL_OFFSET_IN_PAGE_TABLES+11] = ((uint32_t)kernel_page_table11) | 3;
  // kernel_page_directory[KERNEL_OFFSET_IN_PAGE_TABLES+12] = ((uint32_t)kernel_page_table12) | 3;
  // kernel_page_directory[KERNEL_OFFSET_IN_PAGE_TABLES+13] = ((uint32_t)kernel_page_table13) | 3;
  // kernel_page_directory[KERNEL_OFFSET_IN_PAGE_TABLES+14] = ((uint32_t)kernel_page_table14) | 3;
  dbg_printf("kernel page directory fully prepared\n");

  loadPageDirectory(kernel_page_directory);
  dbg_printf("page directory loaded\n");
  enablePaging();
  dbg_printf("paging enabled\n");
}
