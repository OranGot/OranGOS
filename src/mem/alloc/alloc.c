#include "../../include/alloc.h"
#include "../../include/dbg.h"
#include "../../include/multiboot_header.h"
#include "../../include/num.h"
#include "../../include/printf.h"
#include "../../include/strutil.h"
#include "../../include/pagedir.h"
#include "../../include/mmap.h"
#include "../../include/panic.h"
#include "../../include/alloc.h"
#include <stdint.h>
extern uint32_t startkernel;
extern uint32_t endkernel;
#define CWPAGE_DEFAULT (void*)0xDEADBEEF
//uint8_t cwbitmap[BITMAP_BYTES];
void *cwpage = CWPAGE_DEFAULT;
//char mmap[1024 * 1024];
//uint32_t segjmptable[128];
// extern struct multiboot_info_t;
// extern struct multiboot_memory_map_t;
extern uint32_t kernel_page_directory[1024] __attribute__((aligned(4096)));
// void kernel_allocator_setup(multiboot_info_t *mbd) {
//     memset(mmap, MMAP_FREE, 1024*1024);
//     uint8_t available_offset = 0;
//   uint32_t i;
//   for (i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
//     multiboot_memory_map_t *mmmt =
//         (multiboot_memory_map_t *)(mbd->mmap_addr + i);

//     if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
//       uint64_t kbsize = mmmt->len / (1024 * 4);
//       segjmptable[available_offset] = mmmt->addr;
//       printf("AVAILABLE: %llu\n", kbsize);
//       available_offset++;
//     }
//   }
//   //reserving kernel's area
//   segjmptable[available_offset] = (uint32_t)&endkernel;
//   mmap[(uint32_t)&startkernel/ (PAGE_SIZE)] = available_offset;
// }
void prepare_page(void* phy){
    bitmap_header *h;
    h->signature = KALLOCATOR_SIGNATURE;
    memset(h->bitmap, 0, BITMAP_BYTES);
    memcpy(phy, h, sizeof(bitmap_header));
    return;
}
int get_bitmap(void* paddr, bitmap_header* h){
    printf("sizeof header, %i\n", sizeof(bitmap_header));
    memcpy(h, paddr, sizeof(bitmap_header));
    if(h->signature != KALLOCATOR_SIGNATURE){
        dbg_printf("INVALID ALLOCATOR SIGNATURE\n");
        return 1 ;
    }
    return 0;
}
void kappend_page(uint32_t *pagedir, uint16_t flags) {
    void *phyaddr = get_free_phy_page();
    if(phyaddr == NULL){
        dbg_printf("kappend page failed\n");
    }
    reserve_addr((uint32_t)phyaddr);
    printf("got free page at 0x%p", phyaddr);
    //panic("test\n");
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
// int load_page(void *baseaddr) {
//   bitmap_header head;
//   memcpy(&head, baseaddr, sizeof(bitmap_header));
//   if (head.signature != KALLOCATOR_SIGNATURE) {
//     dbg_printf("INVALID ALLOCATOR SIGNATURE!!!\n");
//     return 1;
//   }
//   memcpy(cwbitmap, head.bitmap, BITMAP_BYTES);

//   cwpage = baseaddr;
//   return 0;
// }

extern uint32_t endkernel;
void save_page(void *baseaddr) {}
void *kalloc(uint32_t size) {
  if (cwpage == CWPAGE_DEFAULT) {
    dbg_printf("No pages had been allocated yet\n");
    void* phy;
    if((phy = get_free_phy_page()) == NULL){
        printf("no more pages available");
        return NULL;
    }
    if(reserve_addr((uint32_t)phy) == 1){
        dbg_printf("reserving failed");
        return NULL;
    };
    if(append_page(kernel_page_directory, (uint32_t)phy, 3) != 0){
        dbg_printf("ran out of space\n");
        return NULL;
    }
    prepare_page(phy);
    cwpage = phy;
    dbg_printf("page allocated\n");
  }
  bitmap_header *header;
  get_bitmap(cwpage,header);
  if(header == NULL){
      dbg_printf("invalid header\n");
      return NULL;
  }
  printf("cwpage = 0x%p\n", cwpage);
  uint16_t bestfit = 0;
  void *bestfitaddr = NULL;
  uint16_t currfit = 0;
  uint32_t curraddr = sizeof(bitmap_header) + (uint32_t)cwpage;
  uint8_t isfirst_iteration = 0;
  for (uint16_t bmpe = sizeof(bitmap_header) / 8; bmpe < BITMAP_BYTES; bmpe++) {
      //dbg_printf("one bit loop done\n");
    for (uint8_t bit = 0; bit < 8; bit++) {
        //still in header skip to next iteration
        //if(!isfirst_iteration)bit = sizeof(bitmap_header) % 8;
      if (!get_bit_from_num(header->bitmap[bmpe], bit)) {
        currfit++;
        curraddr++;
      } else {
        if (currfit - size == 0) {
            dbg_printf("allocator returning\n");
          return (void *)(bmpe * 8 + bit + sizeof(bitmap_header) + cwpage -
                          currfit);
        } else if (currfit - size > 0 && bestfit < currfit - size) {
            dbg_printf("allocator bestfitaddr changed\n");
          bestfitaddr = (void *)(bmpe * 8 + bit + sizeof(bitmap_header) +
                                 cwpage - currfit);
          bestfit = currfit;
        }
      }
    }
  }
  printf("best fit: %u, current fit: %u, bestfit address 0x%p\n", bestfit,
         currfit, bestfitaddr);
  //printf("maximum: %u", PAGE_SIZE - sizeof(bitmap_header));
  if (bestfit == 0 && currfit >= PAGE_SIZE - sizeof(bitmap_header)) {
    dbg_printf("FREE PAGE\n");
    uint16_t bitctr = 0;
    for(uint32_t i = 0; i<size/8; i++){
        //header->bitmap[BITMAP_BYTES - currfit/8+i] = 1;
        for(uint8_t bit = 0; bit< 8 && bitctr<size; bit++){
            bitctr++;
            set_bit_of_num(header->bitmap[BITMAP_BYTES- currfit/8+i], bit, 1);
        }
    }
    return cwpage + sizeof(bitmap_header);
  } else if (currfit == 0 && bestfit == 0) {
    dbg_printf("FULL PAGE!!!\n");
    // kappend_page(kernel_page_directory, DEF);
    return NULL;
  } else {
      uint16_t bitctr = 0;
      for(uint32_t i = 0; i<size/8; i++){
          //header->bitmap[BITMAP_BYTES - currfit/8+i] = 1;
          for(uint8_t bit = 0; bit< 8 && bitctr<size; bit++){
              bitctr++;
              set_bit_of_num(header->bitmap[BITMAP_BYTES- currfit/8+i], bit, 1);
          }
      }
    return bestfitaddr;
  }
}
void kfree(void *val, uint32_t size) {
  //load_page((void *)((uint32_t)val & 0xFFFFF000));
  bitmap_header *header;
  if(get_bitmap(cwpage,header) == 1){
      dbg_printf("invalid header in kfree\n");
  }
  for (uint32_t i = ((uint32_t)val & 0xFFF) - sizeof(bitmap_header);
       i < ((uint32_t)val & 0xFFF) - sizeof(bitmap_header) + size; i++) {
    for (uint8_t bit = 0; bit < 8; bit++) {
      header->bitmap[i] = set_bit_of_num(header->bitmap[i], bit, 0);
    }
  }
  return;
}
void printmem(multiboot_info_t *mbd) {
  uint32_t i;
  for (i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
    multiboot_memory_map_t *mmmt =
        (multiboot_memory_map_t *)(mbd->mmap_addr + i);
    printf("Start Addr: 0x%llx | Length: 0x%llx | Size: 0x%x | Type: 0x%lx\n",
           mmmt->addr, mmmt->len, mmmt->size, mmmt->type);

    if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {

      uint64_t pagesize = mmmt->len / 1024 / 4;
      printf("AVAILABLE: %llu(in pages)\n", pagesize);
    }
  }
}
