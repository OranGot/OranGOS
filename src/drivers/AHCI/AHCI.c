#include "../../include/AHCI.h"
#include "../../include/pci.h"
#include "../../include/dbg.h"
#include "../../include/printf.h"
#include "../../include/alloc.h"
#include "../../include/mmap.h"
#include <stdint.h>
uint16_t ahci_bus, ahci_device;
static uint8_t is_ahci(uint16_t bus, uint8_t device){
      uint16_t vendor, class, subclass;
    if ((vendor = pciConfigReadWord((uint8_t)bus, device, 0, 0)) == 0xFFFF) {
      //dbg_printf("device doesn't exist\n");
      return 1;
    }
    if((class = (pciConfigReadWord((uint8_t)bus, device, 0, 0xa) & 0xFF00) >> 8) != 0x01){
      dbg_printf("not a storage device\n");
      return 2;
    }
    if((subclass = (pciConfigReadWord((uint8_t)bus, device, 0, 0xa) & 0xFF)) != 0x06){
      dbg_printf("incorrect subclass\n");
      return 2;
    }
    ahci_bus = bus;
    ahci_device = device;
    dbg_printf("AHCI found\n");
    return 0;

}
HEADER_TYPE_0x0* AHCI_PCI_HEADER;

HBA_MEM* ahci_mem_base;
int find_ahci(void){
  uint16_t bus, dev;
  uint8_t is_valid = 0;
  for(bus = 0; bus < 256;  bus++){
    for(dev = 0; dev< 32; dev++){
      if(is_ahci(bus, dev) == 0) {
        is_valid = 1;
        ahci_bus = bus;
        ahci_device = dev;
        break;
      }
    }
  }
  if(!is_valid){
    return 1 ;
  }

  return 0;
}
extern uint32_t kernel_page_directory[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));

void setup_ahci(void){
    dbg_printf("setting up AHCI\n");
   HEADER_TYPE_0x0* ahci_config = kalloc(sizeof(AHCI_PCI_HEADER));
   if(ahci_config == NULL){
       dbg_printf("kalloc failed\n");
   }
   printf("AHCI BUS: 0x%x, AHCI DEVICE: 0x%x\n", ahci_bus, ahci_device);
   dbg_printf("allocation done\n");
  get_pci_normal_device(ahci_config, ahci_bus, ahci_device, 0);
  dbg_printf("got device info\n");
  printf("AHCI PCI HEADER: %p\n", ahci_config);
  print_device_normal(ahci_config);
  dbg_printf("printed device\n");
  //ahci_mem_base = (volatile HBA_MEM*)ahci_config->BAR5;
  ahci_mem_base = append_page_at_addr(ahci_config->BAR5, 0b1011, kernel_page_directory);
  probe_port();
  dbg_printf("AHCI initialised\n");
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

void probe_port()
{
	// Search disk in implemented ports
	uint32_t pi = ahci_mem_base->pi;
	int i = 0;
	printf("pi: %x\n", pi);
	while (i<32)
	{
		if (pi & 1)
		{
		    // printf(" %i ", i);
			int dt = check_type(&ahci_mem_base->ports[i]);
			//printf("dt: %i ", dt);
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
				//printf("No drive found at port %d\n", i);
				dbg_printf("no drive\n");
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
void print_dbg_ahci(void){
    printf("AHCI MEM BASE AT: 0x%p\n", ahci_mem_base);
}
