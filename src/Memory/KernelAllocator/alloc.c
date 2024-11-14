#include "alloc.h"
#include "../../util/dbg.h"
#include "../../util/multiboot_header.h"
#include "../../util/num.h"
#include "../../util/printf.h"
#include "../../util/strutil.h"
#include "../PageTable/pagedir.h"
#include "../mmap/mmap.h"
#include <stdint.h>

#define CWPAGE_DEFAULT (void*)0xDEADBEEF
uint8_t cwbitmap[BITMAP_BYTES];
void *cwpage = CWPAGE_DEFAULT;
uint8_t mmap[1024 * 1024];
uint32_t segjmptable[128];
// extern struct multiboot_info_t;
// extern struct multiboot_memory_map_t;
extern uint32_t kernel_page_directory[1024] __attribute__((aligned(4096)));
void kernel_allocator_setup(multiboot_info_t *mbd) {

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
void kappend_page(uint32_t *pagedir, uint16_t flags) {
    void *phyaddr = kget_free_page();
  bitmap_header default_header;
  default_header.signature = KALLOCATOR_SIGNATURE;
 uint16_t pdi, pti;
  for (pdi = 0; pdi < PAGE_TABLE_SIZE; pdi++) {
    uint32_t *pagetable = kget_page_table(pagedir, pdi);
    if (pagetable[PAGE_TABLE_SIZE - 1] << 31 != 0) { // page table present
      for (pti = PAGE_TABLE_SIZE; pti > 0; pti--) {
        if (pagetable[pti] << 31 != 0) {
          pagetable[pti - 1] = create_page_table_entry((uint32_t)phyaddr, flags);
          memcpy((void*)phyaddr, &default_header, sizeof(bitmap_header));
          break;
        }
      }
    }
  }
  cwpage =create_virtual_address(pdi, pti, 0);

}
uint32_t *kget_page_table(uint32_t *pagedir, uint16_t id) {
  return (uint32_t *)(pagedir[id] & 0xFFFFF000);
}

uint32_t create_page_table_entry(uint32_t address, uint16_t flags) {
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
  if (head.signature != KALLOCATOR_SIGNATURE) {
    dbg_printf("INVALID ALLOCATOR SIGNATURE!!!\n");
    return 1;
  }
  memcpy(cwbitmap, head.bitmap, BITMAP_BYTES);

  cwpage = baseaddr;
  return 0;
}
void save_page(void *baseaddr) {}
void *kalloc(uint32_t size) {
  if (cwpage == CWPAGE_DEFAULT) {
    dbg_printf("No pages had been allocated yet\n");
    kappend_page(kernel_page_directory, DEFAULT_KERNEL_FLAGS);
  }
  uint16_t bestfit = 0;
  void *bestfitaddr = NULL;
  uint16_t currfit = 0;
  uint32_t curraddr = sizeof(bitmap_header) + (uint32_t)cwpage;
  uint8_t isfirst_iteration = 0;
  for (uint16_t bmpe = sizeof(bitmap_header) / 8; bmpe < BITMAP_BYTES; bmpe++) {
    for (uint8_t bit = 0; bit < 8; bit++) {
        //still in header skip to next iteration
        if(!isfirst_iteration)bit = sizeof(bitmap_header) % 8;
      if (!get_bit_from_num(cwbitmap[bmpe], bit)) {
        currfit++;
        curraddr++;
      } else {
        if (currfit - size == 0) {
            dbg_printf("allocator returning\n");
          return (void *)(bmpe * 8 + bit + sizeof(bitmap_header) + cwpage -
                          currfit);
        } else if (currfit - size > 0 && bestfit < currfit - size) {
            dbg_printf("allocator bestfitaddr changed");
          bestfitaddr = (void *)(bmpe * 8 + bit + sizeof(bitmap_header) +
                                 cwpage - currfit);
          bestfit = currfit;
        }
      }
    }
  }
  printf("best fit: %u, current fit: %u, bestfit address %p\n", bestfit,
         currfit, bestfitaddr);
  if (bestfit == 0 && currfit == PAGE_SIZE - sizeof(bitmap_header)) {
    dbg_printf("FREE PAGE\n");
    return cwpage + sizeof(bitmap_header);
  } else if (currfit == 0 && bestfit == 0) {
    dbg_printf("FULL PAGE!!!\n");
    // kappend_page(kernel_page_directory, DEF);
    return NULL;
  } else {
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
  return;
}
void printmem(multiboot_info_t *mbd) {
  uint32_t i;
  for (i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
    multiboot_memory_map_t *mmmt =
        (multiboot_memory_map_t *)(mbd->mmap_addr + i);
    printf("Start Addr: %llu | Length: %llu | Size: %lu | Type: %lu\n",
           mmmt->addr, mmmt->len, mmmt->size, mmmt->type);

    if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {

      uint64_t kbsize = mmmt->len / 1024;
      printf("AVAILABLE: %llu\n", kbsize);
      // plot_pages(mmmt->addr, mmmt->len);
      /*
       * Do something with this memory block!
       * BE WARNED that some of memory shown as availiable is actually
       * actively being used by the kernel! You'll need to take that
       * into account before writing to memory!
       */
    }
  }
}
