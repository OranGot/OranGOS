#include "rsdp.h"
#include "../../util/dbg.h"
#include "../../util/strutil.h"
#include <stdint.h>
#define SEARCH_MAX 0x000FFFFF
#define SEARCH_LOW 0x000E0000
RSDP locate_rsdp(void) {
  dbg_printf("Starting locating rsdp\n");
  RSDP rsdp;
  char signature[8];
  for (uint32_t i = SEARCH_LOW; i < SEARCH_MAX; i++) {
    memcpy(signature, (void *)i, 8);
    if (!strcmp(signature, RSDP_SIGNATURE, 8)) {
      dbg_printf("RSDP found\n");
      memcpy(&rsdp, (void *)i, sizeof(RSDP));
      return rsdp;
    }
  }
  dbg_printf("RSDP not found(\n");
  return rsdp;
}
// int validate_rsdp(RSDP rsdp){
//     rsdp.
// }