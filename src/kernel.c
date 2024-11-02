#include "Memory/KernelAllocator/alloc.h"
#include "Memory/PageTable/pagedir.h"
#include "drivers/ACPI/rsdp.h"
#include "drivers/PIT/pit.h"
#include "drivers/PS2f/ps2.h"
#include "gdt/gdt.h"
#include "interrupts/int.h"
#include "interrupts/pic/pic.h"
#include "tests/test.h"
#include "tui/console.h"
#include "tui/cursor.h"
#include "util/dbg.h"
#include "util/multiboot_header.h"
#include "util/printf.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef uint16_t grub_uint16_t;
#define RELEASE "0.0.1"
#define __OranGOS 1
#define PIC_OFFSET 0x20

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
void kernel_main(multiboot_info_t *mbd, uint32_t magic) {
  if (MULTIBOOT_BOOTLOADER_MAGIC != magic) {
    dbg_printf("GRUB MAGIC FAIL\n");
    return;
  }
  if ((mbd->flags & 1) != 1) {
    dbg_printf("GRUB invalid flags!!\n");
    return;
  }

  dbg_printf("starting kernel init!\n");
  /* Initialize terminal interface */
  terminal_initialize();
  enable_cursor(0, 13);
  flip_console_mode();
  printf("  ___                   ____  ___  ____\n");
  printf(" / _ \\ _ __ __ _ _ __  / ___|/ _ \\/ ___| \n");
  printf("| | | | '__/ _` | '_ \\| |  _| | | \\___ \\\n");
  printf("| |_| | | | (_| | | | | |_| | |_| |___) |\n");
  printf(" \\___/|_|  \\__,_|_| |_|\\____|\\___/|____/ \n");
  setup_gdt();
  dbg_printf("GDT set up!\n");
  idt_init();
  dbg_printf("IDT set up!\n");
  PIC_remap(PIC_OFFSET, PIC_OFFSET + 8);
  dbg_printf("PIC remapped\n");
  init_ps2();
  dbg_printf("initialised ps2\n");
  printmem(mbd);
  paging_setup();
  dbg_printf("paging setup\n");
  kernel_allocator_setup(mbd);
  dbg_printf("kernel allocator setup\n");
  if (test_allocator() == 1) {
    dbg_printf("ALLOCATOR FAILED\n");
  }
  RSDP rsdp = locate_rsdp();
  printf("ACPI revision: %i\n", rsdp.Revision);
  printf("OranGOS kernel initialisation finished successfully\n");
  // testing kernel allocator

  _putchar('>');
  flip_console_mode();
  for (;;)
    ;
}
