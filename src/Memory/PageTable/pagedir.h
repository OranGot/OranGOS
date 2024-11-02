#include <stdint.h>

void paging_setup(void);
#define MEM_LIMIT 0xFFFFF000
#define PAGE_TABLE_SIZE 1024
#define PAGE_SIZE 1024 * 4