/* Fetches a page from memory
 * This DOES NOT use PAE and is made for x86!!!
 *
 *
 *
 */
#include "../KernelAllocator/alloc.h"
#include "../PageTable/pagedir.h"
#include <stdint.h>
// I don't know what write through means so I'll just set it
#define DEFAULT_USER_FLAGS 0b000001111
#define DEFAULT_KERNEL_FLAGS 0b100001111
extern uint8_t mmap[1024 * 1024];
extern uint32_t segjmptable[128];
// everything above 1 is a breakpoint id
#define MMAP_FREE 0x0
#define MMAP_OCCUPIED 0x1

void *get_free_page() {
  uint32_t curr_offset = segjmptable[0];
  uint32_t i = 0;
  while (mmap[i] != MMAP_FREE && i < 1024 * 1024) {
    if (mmap[i] > 0x1) {
      curr_offset = segjmptable[mmap[i]];
    }
    i++;
  }
  mmap[i] = MMAP_OCCUPIED;
  return (void *)(curr_offset + (i * PAGE_SIZE));
}
void *mem_map(uint32_t *pagedir) {}