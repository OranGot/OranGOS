#include "alloc.h"
#include "../../util/dbg.h"
#include "../../util/multiboot_header.h"
#include "../../util/num.h"
#include "../../util/printf.h"
#include "../../util/strutil.h"
#include "../PageTable/pagedir.h"
#include "../mmap/mmap.h"
#include <stdint.h>
// I don't know what write through means so I'll just set it
#define DEFAULT_USER_FLAGS 0b000001111
#define DEFAULT_KERNEL_FLAGS 0b100001111

uint8_t cwbitmap[BITMAP_BYTES];
void *cwpage = NULL;
uint8_t mmap[1024 * 1024];
uint32_t segjmptable[128];
// extern struct multiboot_info_t;
// extern struct multiboot_memory_map_t;
extern uint32_t kernel_page_directory[1024] __attribute__((aligned(4096)));
void kernel_allocator_setup(multiboot_info_t *mbd) {
  uint8_t ctr = 0;

  uint32_t i;
  for (i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
    multiboot_memory_map_t *mmmt =
        (multiboot_memory_map_t *)(mbd->mmap_addr + i);
    uint8_t available_offset = 0;
    if (mmmt->type == 1) {
      uint64_t kbsize = mmmt->len / (1024 * 4);
      segjmptable[available_offset] = mmmt->addr;
      printf("AVAILABLE: %llu\n", kbsize);
      available_offset++;
    }
  }
}
void kappend_page(uint32_t *pagedir, uint8_t flags) {
  const void *phyaddr = get_free_page();
  bitmap_header default_header;
  default_header.signature = KALLOCATOR_SIGNATURE;

  for (uint16_t i = 0; i < PAGE_TABLE_SIZE; i++) {
    uint32_t *pagetable = kget_page_table(pagedir, i);
    if (pagetable[PAGE_TABLE_SIZE - 1] << 31 == 0) { // page not present
      for (uint16_t o = PAGE_TABLE_SIZE; o > 0; o--) {
        if (pagetable[o] << 31 != 0) {
          pagetable[o - 1] = create_page_table_entry((uint32_t)phyaddr, flags);
          memcpy(phyaddr, &default_header, sizeof(bitmap_header));
        }
      }
      break;
    }
  }
}
uint32_t *kget_page_table(uint32_t *pagedir, uint16_t id) {
  return (uint32_t *)(pagedir[id] & 0x000);
}

uint32_t create_page_table_entry(uint32_t address, uint8_t flags) {
  return address << 11 | flags;
}
/*
10 top bits index into page dir
10 bits into page table
12 remaining bits address into page
there is a page directory per process
*/
void *create_virtual_address(uint16_t page_dir_entry, uint16_t page_table_entry,
                             uint16_t address_into_page) {
  return (void *)((page_dir_entry << 22) | ((page_table_entry & 0x03FF) << 12) |
                  (address_into_page & 0x0FFF));
}
int load_page(void *baseaddr) {
  bitmap_header head;
  memcpy(&head, baseaddr, sizeof(bitmap_header));
  // if (head.signature != KALLOCATOR_SIGNATURE) {
  //   dbg_printf("INVALID ALLOCATOR SIGNATURE!!!\n");
  //   return 1;
  // }
  memcpy(cwbitmap, head.bitmap, BITMAP_BYTES);

  cwpage = baseaddr;
  return 0;
}
void save_page(void *baseaddr) {}
void *kalloc(uint32_t size) {
  if (cwpage == NULL) {
    dbg_printf("No pages had been allocated yet\n");
    kappend_page(kernel_page_directory, DEFAULT_KERNEL_FLAGS);
  }
  uint16_t bestfit = 0;
  void *bestfitaddr = NULL;
  uint16_t currfit = 0;
  for (uint16_t bmpe = 0; bmpe < BITMAP_BYTES; bmpe++) {
    for (uint8_t bit = 0; bit < 8; bit++) {
      if (!get_bit_from_num(cwbitmap[bmpe], bit)) {
        currfit++;
      } else {
        if (currfit - size == 0) {
          return (void *)(bmpe * 8 + bit + sizeof(bitmap_header) + cwpage -
                          currfit);
        } else if (currfit - size > 0 && bestfit < currfit - size) {
          bestfitaddr = (void *)(bmpe * 8 + bit + sizeof(bitmap_header) +
                                 cwpage - currfit);
          bestfit = currfit;
        }
      }
    }
    if (bestfit == 0 && currfit != 0) {
      dbg_printf("Empty page\n");
      return cwpage + sizeof(bitmap_header);
    } else if (bestfit < size && currfit < size) {
      dbg_printf("Page is full");
      return NULL;
    } else if (bestfit > size) {
      return bestfitaddr;
    } else {
      dbg_printf("something weird");
      return NULL;
    }
  }
  printf("best fit: %u, current fit: %u, bestfit address %p\n", bestfit,
         currfit, bestfitaddr);
  if (bestfit == 0 && currfit != 0) {
    dbg_printf("FREE PAGE\n");
    for (uint32_t i = 0; i < size / 8; i++) {
    }
    return cwpage;
  } else if (currfit == 0 && bestfit == 0) {
    dbg_printf("FULL PAGE!!!\n");
    // kappend_page(kernel_page_directory, DEF);
    return NULL;
  } else {
    printf("ENTRY AT: %p\n", bestfitaddr);
    return bestfitaddr;
  }
}
void kfree(void *val, uint32_t size) {
  load_page((void *)((uint32_t)val & 0x000));
  for (uint32_t i = ((uint32_t)val & 0xFFF) - sizeof(bitmap_header);
       i < ((uint32_t)val & 0xFFF) - sizeof(bitmap_header) + size; i++) {
    for (uint8_t bit = 0; bit < 8; bit++) {
      cwbitmap[i] = set_bit_of_num(cwbitmap[i], bit, 0);
    }
  }
}