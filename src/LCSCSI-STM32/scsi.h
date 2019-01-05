#include <stdio.h>

// SCSI emulation bus states
#define SCSI_BUSFREE	0
#define SCSI_COMMAND	1
#define SCSI_STATUS		2
#define SCSI_MESSAGE	3

// Extras
#define SCSI_BUSBUSY	4



// SCSI emulation command states (Group 0 commands)
#define SCSI_TESTUNITREADY	10
#define SCSI_REZEROUNIT		11
#define SCSI_REQUESTSENSE	12
#define SCSI_FORMAT			13
#define SCSI_READ6			14
#define SCSI_WRITE6			15
#define SCSI_SEEK			16
#define SCSI_TRANSLATE		17
#define SCSI_MODESELECT		18
#define SCSI_MODESENSE		19
#define SCSI_STARTSTOP		20
#define SCSI_VERIFY			21
#define SCSI_INQUIRY		22
#define SCSI_SELECT			23

// SCSI Information transfer phases
#define ITPHASE_DATAOUT		0
#define ITPHASE_DATAIN		1
#define ITPHASE_COMMAND		2
#define ITPHASE_STATUS		3
#define ITPHASE_MESSAGEOUT	4
#define ITPHASE_MESSAGEIN	5

void scsiInitialise(void);
void scsiReset(void);


// Emulation mode (fixed / LV-DOS)
#define FIXED_EMULATION		0
#define LVDOS_EMULATION		1


void scsiProcessEmulation(void);
void scsiInformationTransferPhase(uint8_t transferPhase);

uint8_t scsiEmulationBusFree(void);
uint8_t scsiEmulationCommand(void);
uint8_t scsiEmulationStatus(void);
uint8_t scsiEmulationMessage(void);

uint8_t scsiCommandTestUnitReady(void);
uint8_t scsiCommandRezeroUnit(void);
uint8_t scsiCommandRequestSense(void);
uint8_t scsiCommandFormat(void);
uint8_t scsiCommandRead6(void);
uint8_t scsiCommandWrite6(void);
uint8_t scsiCommandSeek(void);
uint8_t scsiCommandTranslate(void);
uint8_t scsiCommandModeSelect(void);
uint8_t scsiCommandModeSense(void);
uint8_t scsiCommandStartStop(void);
uint8_t scsiCommandVerify(void);

uint8_t scsiCommandInquiry(void);
uint8_t scsiCommandSelect(void);


uint8_t scsiWriteFCode(void);
uint8_t scsiReadFCode(void);

uint8_t scsiBeebScsiSense(void);
uint8_t scsiBeebScsiSelect(void);

uint8_t scsiBeebScsiFatPath(void);
uint8_t scsiBeebScsiFatInfo(void);
uint8_t scsiBeebScsiFatRead(void);