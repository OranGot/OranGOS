#include "rsdt.h"
#include "../util/dbg.h"
#include <stdbool.h>
#include <stdint.h>
bool doChecksum(struct ACPISDTHeader *tableHeader) {
  unsigned char sum = 0;

  for (int i = 0; i < tableHeader->Length; i++) {
    sum += ((char *)tableHeader)[i];
  }

  return sum == 0;
}
void *findFACP(void *RootSDT) {
  struct RSDT *rsdt = (struct RSDT *)RootSDT;
  int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

  for (int i = 0; i < entries; i++) {
    struct ACPISDTHeader *h =
        (struct ACPISDTHeader *)rsdt->PointerToOtherSDT[i];
    if (!strncmp(h->Signature, "FACP", 4)) {
      dbg_printf("FADT found");
      return (void *)h;
    }
  }

  // No FACP found
  return 0;
}