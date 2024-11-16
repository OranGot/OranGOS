#include "include/alloc.h"
#include "include/pagedir.h"
#include "include/mmap.h"
#include "include/rsdp.h"
#include "include/pit.h"
#include "include/ps2.h"
#include "include/pci.h"
#include "include/AHCI.h"
#include "include/gdt.h"
#include "include/int.h"
#include "include/pic.h"
#include "include/test.h"
#include "include/console.h"
#include "include/cursor.h"
#include "include/dbg.h"
#include "include/multiboot_header.h"
#include "include/printf.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
// typedef uint16_t grub_uint16_t;
#include "include/kerninfo.h"

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
  terminal_initialize();
  dbg_printf("terminal initialised\n");
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
  paging_setup();
  dbg_printf("paging setup\n");
  // remap_kernel();
  // dbg_printf("kernel remapped to higher half\n");
  kernel_allocator_setup(mbd);
  dbg_printf("kernel allocator setup\n");
  init_ps2();

  dbg_printf("initialised ps2\n");
  printmem(mbd);

  // if (test_allocator() == 1) {
  //   printf("ALLOCATOR tests\n");
  // }
  // else{
  //     printf("Allocator tests passed\n");
  //}
  RSDP rsdp = locate_rsdp();
  printf("ACPI revision: %i\n", rsdp.Revision);
  if(find_ahci() == 1){
      printf("no AHCI found\n");
  }
  else{
      setup_ahci();
      //print_dbg_ahci();
      printf("AHCI setup\n");
  }

  printf("Welcome to OranGOS version %s!\n", RELEASE);
  _putchar('>');
  flip_console_mode();

  for (;;)
    ;
}
