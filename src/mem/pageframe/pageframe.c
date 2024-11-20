#include "../../include/multiboot_header.h"
#include "../../include/alloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../include/alloc.h"
#include "../../include/dbg.h"
#include "../../include/printf.h"
#define PAGEFRAME_RESERVED 1
#define PAGEFRAME_FREE 0
#define PAGE_MMAP_SIZE 1024*1024
bool global_page_map[PAGE_MMAP_SIZE]; //not optimised but I don't care
extern uint32_t startkernel;
extern uint32_t endkernel;
// void append_pagetable(uint32_t* pagedir){

// }
//sets the page as used
int reserve_addr(uint32_t phy){
    if(global_page_map[phy/ PAGE_SIZE] == PAGEFRAME_FREE){
        global_page_map[phy/ PAGE_SIZE] = PAGEFRAME_RESERVED;
        return 0;
    }
    return 1;
}
int append_page(uint32_t* pagedir, uint16_t flags){
    for(uint16_t i = 0; i < PAGE_TABLE_SIZE; i++){
        if(pagedir[i] % 2 != 0){
            //pagetable is present
            uint32_t* pt = (uint32_t*)(pagedir[i] & 0xFFFFF000);
            for(uint16_t pte = 0; pte<PAGE_TABLE_SIZE; pte++){
                if(pt[pte] % 2 == 0){
                    //page table entry is not present
                    pt[pte] = ((i << 22) | (pte << 12) | flags);
                    return 0;
                }
            }
        }
    }
    return 1;
}
//gets the physical address of next free page
void* get_free_phy_page(){
    for(uint32_t i = 0; i<PAGE_MMAP_SIZE; i++){
        if(global_page_map[i] == PAGEFRAME_FREE){
            // uint32_t pde, pte;
            // pde = i/PAGE_TABLE_SIZE;
            // pte = i % PAGE_TABLE_SIZE;
            printf("i is %u\nreturning 0x%x", i , i * PAGE_SIZE);
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL;
}
void setup_pageframe_allocator(multiboot_info_t *mbd){
    //reserving bios area and kernel's
    printf("about to stop at%u\n", ((uint32_t)&endkernel / PAGE_SIZE)+1);
    for(uint16_t i = 0; i < ((uint32_t)&endkernel / PAGE_SIZE)+1; i++){
        global_page_map[i] = PAGEFRAME_RESERVED;
    }
    for (uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
      multiboot_memory_map_t *mmmt =
          (multiboot_memory_map_t *)(mbd->mmap_addr + i);

      if (mmmt->type != MULTIBOOT_MEMORY_AVAILABLE) {
          dbg_printf("memory not available\n");
          for(uint16_t o =  mmmt->addr / PAGE_SIZE ; o< (mmmt->len + (mmmt->addr / PAGE_SIZE) ) / PAGE_SIZE; o++){
              global_page_map[o] = PAGEFRAME_RESERVED;
          }
      }
    }
}
void* append_page_at_addr(uint32_t phyaddr, uint16_t flags, uint32_t* pagedir){
    for(uint16_t pde = 0; pde<PAGE_TABLE_SIZE; pde++){
        if(pagedir[pde] % 2 == 1){
            uint32_t* pagetable = (uint32_t*)(pagedir[pde] & 0xFFFFF000);
            for(uint16_t pte = 0; pte< PAGE_TABLE_SIZE; pte++){
                if(pagetable[pde]% 2 == 0){
                    //page not present
                    pagetable[pte] = phyaddr | flags;
                    return create_virtual_address(pde, pte,0 );
                }
            }
        }
    }
    return NULL;
}
