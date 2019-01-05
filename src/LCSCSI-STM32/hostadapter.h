#include <stdio.h>

#pragma once
// Host adapter hardware definitions



// SCSI control signals (inputs)
//#define NRST_PORT		GPIOD
//#define NRST_PIN		GPIO_PIN_0
//#define NRST_DDR		DDRD
#define NRST			(1 << 0)
//
//// TBA - work out how to do interrupts on the STM32F4
//#define NRST_INT		INT0
//#define NRST_INT_VECT	INT0_vect
//#define NRST_ISC0		ISC00
//#define NRST_ISC1		ISC01
//#define NRST_INTF		INTF0
//#define NRST_EICR		EICRA
//
//#define NCONF_PORT		GPIOA
//#define NCONF_PIN		GPIO_PIN_1
//#define NCONF_DDR		DDRD
#define NCONF			(1 << 1)
//
//// Again work out the interrupts
//#define NCONF_INT		INT1
//#define NCONF_INT_VECT	INT1_vect
//#define NCONF_ISC0		ISC10
//#define NCONF_ISC1		ISC11
//#define NCONF_INTF		INTF1
//#define NCONF_EICR		EICRA
//
//
//#define NACK_PORT		GPIOC
//#define NACK_PIN		GPIO_PIN_7
//#define NACK_DDR		DDRC
#define NACK			(1 << 7)
//
//#define NSEL_PORT		GPIOC
//#define NSEL_PIN		GPIO_PIN_6
//#define NSEL_DDR		DDRC
#define NSEL			(1 << 6)
//
//#define INTNEXT_PORT	GPIOC
//#define INTNEXT_PIN		GPIO_PIN_5
//#define INTNEXT_DDR		DDRC
#define INTNEXT			(1 << 5)
//
//// SCSI status signals (outputs)
//#define STATUS_NMSG_PORT	GPIOC
//#define STATUS_NMSG_PIN		GPIO_PIN_0
//#define STATUS_NMSG_DDR		DDRC
#define STATUS_NMSG			(1 << 0)
//
//#define STATUS_NBSY_PORT	GPIOC
//#define STATUS_NBSY_PIN		GPIO_PIN_1
//#define STATUS_NBSY_DDR		DDRC
#define STATUS_NBSY			(1 << 1)
//
//#define STATUS_NREQ_PORT	GPIOC
//#define STATUS_NREQ_PIN		GPIO_PIN_2
//#define STATUS_NREQ_DDR		DDRC
#define STATUS_NREQ			(1 << 2)
//
//#define STATUS_INO_PORT		GPIOC
//#define STATUS_INO_PIN		GPIO_PIN_3
//#define STATUS_INO_DDR		DDRC
#define STATUS_INO			(1 << 3)
//
//#define STATUS_CND_PORT		GPIOC
//#define STATUS_CND_PIN		GPIO_PIN_4
//#define STATUS_CND_DDR		DDRC
#define STATUS_CND			(1 << 4)
//
//// Host adapter data bus (First 8 bits of port B)
//#define DATABUS_PORT		GPIOB
//#define DATABUS_PIN		PINA
//#define DATABUS_DDR		DDRA

#define SCSI_MSG_Pin			GPIO_PIN_0
#define SCSI_MSG_GPIO_Port		GPIOC
#define SCSI_BSY_Pin			GPIO_PIN_1
#define SCSI_BSY_GPIO_Port		GPIOC
#define SCSI_REQ_Pin			GPIO_PIN_2
#define SCSI_REQ_GPIO_Port		GPIOC
#define SCSI_I_O_Pin			GPIO_PIN_3
#define SCSI_I_O_GPIO_Port		GPIOC
#define SCSI_C_D_Pin			GPIO_PIN_4
#define SCSI_C_D_GPIO_Port		GPIOC
#define SCSI_ATN_Pin			GPIO_PIN_5
#define SCSI_ATN_Port	GPIOC

#define SCSI_RST_Pin			GPIO_PIN_0
#define SCSI_RST_GPIO_Port		GPIOA
#define SCSI_CONF_Pin			GPIO_PIN_1
#define SCSI_CONF_GPIO_Port		GPIOA

#define SCSI_SEL_Pin			GPIO_PIN_6
#define SCSI_SEL_GPIO_Port		GPIOC
#define SCSI_ACK_Pin			GPIO_PIN_7
#define SCSI_ACK_GPIO_Port		GPIOC

#define SCSI_DB0_Pin			GPIO_PIN_0
#define SCSI_DB0_GPIO_Port		GPIOB
#define SCSI_DB1_Pin			GPIO_PIN_1
#define SCSI_DB1_GPIO_Port		GPIOB
#define SCSI_DB2_Pin			GPIO_PIN_2
#define SCSI_DB2_GPIO_Port		GPIOB

#define SCSI_DB3_Pin			GPIO_PIN_3
#define SCSI_DB3_GPIO_Port		GPIOB
#define SCSI_DB4_Pin			GPIO_PIN_4
#define SCSI_DB4_GPIO_Port		GPIOB
#define SCSI_DB5_Pin			GPIO_PIN_5
#define SCSI_DB5_GPIO_Port		GPIOB
#define SCSI_DB6_Pin			GPIO_PIN_6
#define SCSI_DB6_GPIO_Port		GPIOB
#define SCSI_DB7_Pin			GPIO_PIN_7
#define SCSI_DB7_GPIO_Port		GPIOB

// Parity wasn't implemented on the original that I could see, might be done by the CLPD
#define SCSI_PARITY_Pin			GPIO_PIN_8
#define SCSI_PARITY_GPIO_Port	GPIOB

#define DATABUS_PORT			GPIOB


// Function prototypes
void hostadapterInitialise(void);
void hostadapterReset(void);

uint8_t hostadapterReadDatabus(void);
void hostadapterWritedatabus(uint8_t databusValue);

void hostadapterDatabusInput(void);
void hostadapterDatabusOutput(void);

uint8_t hostadapterReadByte(void);
void hostadapterWriteByte(uint8_t databusValue);

uint16_t hostadapterPerformReadDMA(uint8_t *dataBuffer);
uint16_t hostadapterPerformWriteDMA(uint8_t *dataBuffer);

bool hostadapterConnectedToExternalBus(void);

void hostadapterWriteResetFlag(bool flagState);
bool hostadapterReadResetFlag(void);
void hostadapterWriteDataPhaseFlags(bool message, bool commandNotData, bool inputNotOutput);

void hostadapterWriteBusyFlag(bool flagState);
void hostadapterWriteRequestFlag(bool flagState);
bool hostadapterReadSelectFlag(void);
bool hostadapterReadBusyFlag(void);

