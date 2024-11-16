#include "../../include/exep.h"
#include "../../include/console.h"
#include "../../include/dbg.h"
#include "../../include/printf.h"
#include <stdint.h>
__attribute__((cdecl)) void exception_handler(IDT_push_reg *err) {
  clear_screen();
  printf("EXCEPTION!!!\nError code: %08x, Code seg: %u,Data seg: %u\nEAX: "
         "%u, EBP: %u, EBX: %u, ECX: %u\nEDI: %u, EDX: %u, eflags: %u, EIP: "
         "%u\nESI: %u\nESP: %u\ninterrupt num: %u\nstack segment: %u\n",
         err->error_code, err->code_seg, err->data_seg, err->eax, err->ebp,
         err->ebx, err->ecx, err->edi, err->edx, err->eflags, err->eip,
         err->esi, err->esp, err->interrupt_num, err->stack_seg);
  switch (err->interrupt_num) {
  case 0:
    dbg_printf("Division error occured\n");
    break;
  case 1:
    dbg_printf("Debug fault\n");
    break;
  case 2:
    dbg_printf("Non maskable interrupt occured\n");
    break;
  case 3:
    dbg_printf("Breakpoint occured\n");
    break;
  case 4:
    dbg_printf("Overflow error occured\n");
    break;
  case 5:
    dbg_printf("Bound range exeeded\n");
    break;
  case 6:
    dbg_printf("Invalid opcode occured\n");
    break;
  case 7:
    dbg_printf("Device not available\n");
    break;
  case 8:
    dbg_printf("Double fault, good luck\n");
    break;
  case 9:
    dbg_printf("some FPU fault can't happen\n");
    break;
  case 10:
    dbg_printf("Invalid tss occured\n");
    break;
  case 11:
    dbg_printf("Segment not present occured\n");
    break;
  case 12:
    dbg_printf("Stack-segment fault\n");
    break;
  case 13:
    dbg_printf("General protection fault\n");
    break;
  case 14:
    dbg_printf("Page fault\n");
    break;
  case 15:
    dbg_printf("HOW!!!!!!!!!\n");
    break;
  case 16:
    dbg_printf("x87 floating point exception\n");
    break;
  case 17:
    dbg_printf("Alignement check\n");
    break;
  case 18:
    dbg_printf("machine check\n");
    break;
  case 19:
    dbg_printf("SIMD floating point exception\n");
    break;
  case 20:
    dbg_printf("Virtualization exception\n");
    break;
  case 21:
    dbg_printf("Control protection exception\n");
    break;
  case 28:
    dbg_printf("Hypervisor Injection exception\n");
    break;
  case 29:
    dbg_printf("VMM communication exception\n");
    break;
  case 30:
    dbg_printf("Security exception\n");
    break;
  default:
    dbg_printf("Faulty exeption handler, unknown exception\n");
  }

  __asm__ volatile("cli; hlt");
}
