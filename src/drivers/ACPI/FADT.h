#include "rsdt.h"
#include <stdint.h>
struct GenericAddressStructure {
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
};
struct FADT {
  struct ACPISDTHeader h;
  uint32_t FirmwareCtrl;
  uint32_t Dsdt;

  // field used in ACPI 1.0; no longer in use, for compatibility only
  uint8_t Reserved;

  uint8_t PreferredPowerManagementProfile;
  uint16_t SCI_Interrupt;
  uint32_t SMI_CommandPort;
  uint8_t AcpiEnable;
  uint8_t AcpiDisable;
  uint8_t S4BIOS_REQ;
  uint8_t PSTATE_Control;
  uint32_t PM1aEventBlock;
  uint32_t PM1bEventBlock;
  uint32_t PM1aControlBlock;
  uint32_t PM1bControlBlock;
  uint32_t PM2ControlBlock;
  uint32_t PMTimerBlock;
  uint32_t GPE0Block;
  uint32_t GPE1Block;
  uint8_t PM1EventLength;
  uint8_t PM1ControlLength;
  uint8_t PM2ControlLength;
  uint8_t PMTimerLength;
  uint8_t GPE0Length;
  uint8_t GPE1Length;
  uint8_t GPE1Base;
  uint8_t CStateControl;
  uint16_t WorstC2Latency;
  uint16_t WorstC3Latency;
  uint16_t FlushSize;
  uint16_t FlushStride;
  uint8_t DutyOffset;
  uint8_t DutyWidth;
  uint8_t DayAlarm;
  uint8_t MonthAlarm;
  uint8_t Century;

  // reserved in ACPI 1.0; used since ACPI 2.0+
  uint16_t BootArchitectureFlags;

  uint8_t Reserved2;
  uint32_t Flags;

  // 12 byte structure; see below for details
  GenericAddressStructure ResetReg;

  uint8_t ResetValue;
  uint8_t Reserved3[3];

  // 64bit pointers - Available on ACPI 2.0+
  uint64_t X_FirmwareControl;
  uint64_t X_Dsdt;

  GenericAddressStructure X_PM1aEventBlock;
  GenericAddressStructure X_PM1bEventBlock;
  GenericAddressStructure X_PM1aControlBlock;
  GenericAddressStructure X_PM1bControlBlock;
  GenericAddressStructure X_PM2ControlBlock;
  GenericAddressStructure X_PMTimerBlock;
  GenericAddressStructure X_GPE0Block;
  GenericAddressStructure X_GPE1Block;
};
// this doesn't include all of them just the ones with a single value
enum PossibleAddressSpaceVal {
  ACPI_SYS_MEM = 0,
  ACPI_SYS_IO = 1,
  ACPI_PCI_CONFIG_SPACE = 2,
  ACPI_EMBEDDED_CONTROLLER = 3,
  ACPI_SYSTEM_MANAGEMENT_BUS = 4,
  ACPI_SYSTEM_CMOS = 5,
  ACPI_PCI_DEVICE_BAR_TARGET = 6,
  ACPI_IPMI = 7, // Intelligent Platform Management Infrastructure
  ACPI_GEN_PURPOSE_IO = 8,
  ACPI_GEN_SERIAL_BUS = 9,
  ACPI_PLATFORM_COMMUNICATION_CHANNEL = 0x0A,
};
enum AccessSize {
  ACPI_ACCESS_UD = 0,
  ACPI_ACCESS_BYTE_ACCESS = 1,
  ACPI_ACCESS_16_BIT_WORD = 2,
  ACPI_ACCESS_32_BIT_WORD = 3,
  ACPI_ACCESS_64_BIT_WORD = 4,

};
enum PreferredPowerManagementProfile {
  UNSPECIFIED = 0,
  DESKTOP = 1,
  MOBILE = 2,
  WORKSTATION = 3,
  ENTERPRISE_SERVER = 4,
  SOHO_SERVER = 5,
  APPLIANCE_PC = 6,
  PERF_SAVER = 7,
};