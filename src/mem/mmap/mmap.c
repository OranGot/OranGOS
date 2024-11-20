/* Fetches a page from memory
 * This DOES NOT use PAE and is made for x86!!!
 *
 *
 *
 */
#include "../../include/alloc.h"
#include "../../include/printf.h"
#include "../../include/mmap.h"
#include "../../include/pagedir.h"
#include <stdint.h>
#include "../../include/dbg.h"
#include "../../include/kerninfo.h"
// I don't know what write through means so I'll just set it
#define DEFAULT_USER_FLAGS 0b000001111
#define DEFAULT_KERNEL_FLAGS 0b100001111
extern uint32_t kernel_page_directory[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
//extern char mmap[1024 * 1024];
//extern uint32_t segjmptable[128];

#define KERNEL_BASE_OFFSET 0xC0000000

//finds a free page at kernels
// void* kget_free_page(){
//     uint32_t curr_offset = 0;
//     uint32_t i = 0;
//     while (mmap[i] != MMAP_FREE && i < 1024 * 1024 && curr_offset + (i* PAGE_SIZE) < KERNEL_BASE_OFFSET) {
//       if (mmap[i] > -1) {
//         curr_offset = segjmptable[mmap[i]];
//       }
//       else{
//           curr_offset++;
//       }
//       i++;
//     }
//     printf("i: %u\n", i);
//     mmap[i] = MMAP_OCCUPIED;
//     return(void*)curr_offset;
// }
// void *get_free_page() {
//   uint32_t curr_offset = segjmptable[0];
//   uint32_t i = 0;
//   while (mmap[i] != MMAP_FREE && i < 1024 * 1024) {
//     if (mmap[i] > 0x1) {
//       curr_offset = segjmptable[mmap[i]];
//     }
//     i++;
//   }
//   mmap[i] = MMAP_OCCUPIED;
//   return (void *)(curr_offset + (i * PAGE_SIZE));
// }
// //void *mem_map(uint32_t *pagedir) {}

int map_page_at_addr(uint32_t phy_addr, uint32_t* pagedir, uint32_t vaddr, uint16_t flags){
    if(pagedir[(vaddr & 0xFFC00000) >> 22] << 31 == 0){ //page not present, need to allocate
        dbg_printf("oh oh no page table available at that address\n");
        return 1;
    }
    uint32_t* table = kget_page_table(pagedir, (vaddr & 0xFFC00000) >> 22);
    uint32_t entry;
    if(!(entry = table[(vaddr & 0x3FF000) >> 12] << 31)){
        dbg_printf("oh oh no page available at that address smh\n");
        return 1;
    }
    entry = (phy_addr << 12) | flags;
    return 0;
}
void remap_kernel(){
    dbg_printf("remapping the kernel");
    for(uint32_t offset = 0; offset < KERNEL_SIZE; offset+=PAGE_SIZE){
        if(map_page_at_addr(KERNEL_DEFAULT_PHYS + offset, kernel_page_directory, KERNEL_BASE_OFFSET+ offset, DEFAULT_KERNEL_FLAGS) == 1){
            dbg_printf("remapping failed\n");
            return;
        }
    }
    return;
}
