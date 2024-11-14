#include "AHCI.h"
#include "../PCI/pci.h"
#include "../../util/dbg.h"
#include "../../util/printf.h"
#include <stdint.h>
static uint8_t is_ahci(uint16_t bus, uint8_t device){
      uint16_t vendor, class, subclass;
    if ((vendor = pciConfigReadWord((uint8_t)bus, device, 0, 0)) == 0xFFFF) {
      dbg_printf("device doesn't exist\n");
      return 1;
    }
    if((class = ((pciConfigReadWord((uint8_t)bus, device, 0, 6) & 0xFFFF0000) >> 16)) != 0x01){
      dbg_printf("not a storage device\n");
      return 2;
    }
    if((subclass = (uint16_t)(pciConfigReadWord((uint8_t)bus, device, 0, 6) & 0xFFFF)) != 0x06){
      dbg_printf("incorrect subclass\n");
      return 2;
    }
    dbg_printf("AHCI found\n");
    return 0;

}
uint16_t ahci_bus, ahci_device;
HBA_MEM* ahci_mem_base;
void find_ahci(void){
  uint16_t bus, dev;
  uint8_t is_valid = 0;
  for(bus = 0; bus < 256;  bus++){
    for(dev = 0; dev< 32; dev++){
      if(is_ahci(bus, dev) == 0) {
        is_valid = 1;
        break;
      }
    }
  }
  if(!is_valid){
    dbg_printf("No AHCI");
    return;
  }
  ahci_bus = bus;
  ahci_device = dev;
}
void setup_ahci(void){
  ahci_mem_base = (HBA_MEM*)((uint32_t)pciConfigReadWord(ahci_bus, ahci_device, 0, 17) | (uint32_t)pciConfigReadWord(ahci_bus, ahci_device, 0, 18)); //bar5
}
#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

static AHCI_DEV_T check_type(HBA_PORT *port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;

	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_EMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void probe_port(HBA_MEM *abar)
{
	// Search disk in implemented ports
	uint32_t pi = abar->pi;
	int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				printf("SATA drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				printf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_EMB)
			{
				printf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				printf("PM drive found at port %d\n", i);
			}
			else
			{
				printf("No drive found at port %d\n", i);
			}
		}

		pi >>= 1;
		i ++;
	}
}
#define	AHCI_BASE	0x400000	// 4M

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000
// Start command engine
void start_cmd(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR)
		;

	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

// Stop command engine
void stop_cmd(HBA_PORT *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;

	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;

	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}

}
