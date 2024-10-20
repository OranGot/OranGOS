#include <stdint.h>
typedef struct {
  uint32_t data_seg;
  uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
  uint32_t interrupt_num, error_code;
  uint32_t eip, code_seg, eflags, esp, stack_seg;
} IDT_push_reg;
__attribute__((cdecl)) void exception_handler(IDT_push_reg *err);