#include <stdint.h>
typedef struct {
  char Signature[8];
  uint8_t Checksum;
  char OEMID[6];
  uint8_t Revision;
  uint32_t RsdtAddress;
}__attribute__((packed)) RSDP ;
#define RSDP_SIGNATURE "RSD PTR "
RSDP locate_rsdp(void);
typedef struct {
  char Signature[8];
  uint8_t Checksum;
  char OEMID[6];
  uint8_t Revision;
  uint32_t RsdtAddress;
}__attribute__((packed)) RSDPv1 ;
union RSDP_t {
  RSDPv1 v1;
  RSDP v2;
};