#include "../util/printf.h"
#include "include/multiboot.h"
void printmem(multiboot_info_t *mbd) {
  int i;
  for (i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
    multiboot_memory_map_t *mmmt =
        (multiboot_memory_map_t *)(mbd->mmap_addr + i);

    printf("Start Addr: %x | Length: %x | Size: %x | Type: %d\n", mmmt->addr,
           mmmt->len, mmmt->size, mmmt->type);

    if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
      /*
       * Do something with this memory block!
       * BE WARNED that some of memory shown as availiable is actually
       * actively being used by the kernel! You'll need to take that
       * into account before writing to memory!
       */
    }
  }
}