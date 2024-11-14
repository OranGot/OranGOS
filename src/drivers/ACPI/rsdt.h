#include <stdint.h>
struct ACPISDTHeader {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
};
struct RSDT {
  struct ACPISDTHeader h;
  uint32_t PointerToOtherSDT[(h.Length - sizeof(h)) / 4];
};