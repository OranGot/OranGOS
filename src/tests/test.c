#include "../Memory/KernelAllocator/alloc.h"
#include "../util/dbg.h"
#include "../util/printf.h"
#include "../util/strutil.h"
#include <stddef.h>
int test_allocator() {
  dbg_printf("Running kernel allocator tests\n");
  char *block1 = kalloc(10);
  if (block1 == NULL) {
    dbg_printf("allocator number 1  failed\n");
    return 1;
  }
  for (uint8_t i = 0; i < 9; i++) {
    block1[i] = 'w';
  }
  block1[10] = '\0';

  printf("%s\n", block1);
  char *block2 = kalloc(20);
  if(block2 == NULL){
      dbg_printf("Allocator2 failed");
      return 1;
  }
  dbg_printf("allocated block2\n");
  memcpy(block2, block1, 9);
  for (uint8_t i = 9; i < 19; i++) {
    block2[i] = 'a';
  }
  block2[20] = '\0';
  printf("%s", block2);
  kfree(block1, sizeof(uint32_t) * 10);
  return 0;
}
