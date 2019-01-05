#include <stdbool.h>
#include <stdio.h>

//#include "stm32fxxx_hal.h"
#include "tm_stm32_exti.h"
// Local includes
#include "debug.h"
#include "hostadapter.h"
#include "tm_stm32_gpio.h"



//#include "tm_stm32_exti.h"
//#include "stm32f4xx_exti.h"
//#include "stm32f4xx_syscfg.h"
//#include "misc.h"


// Globals for the interrupt service routines
volatile bool nrstFlag = false;

// Timeout counter (used when interrupts are not available to ensure
// DMA read and writes do not hang the AVR waiting for host response
// Note: This is an unsigned 32 bit integer and should therefore be
// smaller than 4,294,967,295
#define TOC_MAX 100000

// Initialise the host adapter hardware (called on a cold-start of the AVR)
void hostadapterInitialise(void)
{
	// Initialise the host adapter input/output pins

	// Set the host adapter databus to input
	hostadapterDatabusInput();
	
		
	// Configure the status byte output pins to output
	//STATUS_NMSG_DDR |= STATUS_NMSG;  // Output
	//STATUS_NBSY_DDR |= STATUS_NBSY;  // Output
	
//	SCSI_RST_GPIO_Port->MODER |= STATUS_NMSG; 
//	SCSI_BSY_GPIO_Port->MODER |= STATUS_NBSY; 
	
	//TM_GPIO_Init(SCSI_RST_GPIO_Port, SCSI_RST_Pin, TM_GPIO_Mode_OUT, TM_GPIO_OType_OD, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	//TM_GPIO_Init(SCSI_BSY_GPIO_Port, SCSI_BSY_Pin, TM_GPIO_Mode_OUT, TM_GPIO_OType_OD, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);

	//STATUS_NREQ_DDR |= STATUS_NREQ;  // Output
	//STATUS_INO_DDR  |= STATUS_INO;   // Output
	//STATUS_CND_DDR  |= STATUS_CND;   // Output
	
//	SCSI_REQ_GPIO_Port->MODER |= STATUS_NREQ;
//	SCSI_I_O_GPIO_Port->MODER |= STATUS_INO;
//	SCSI_C_D_GPIO_Port->MODER |= STATUS_CND;
	
	TM_GPIO_Init(GPIOC, SCSI_REQ_Pin | SCSI_BSY_Pin | SCSI_I_O_Pin | SCSI_C_D_Pin, TM_GPIO_Mode_OUT, TM_GPIO_OType_OD, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	
	
	//	STATUS_NMSG_PORT |= STATUS_NMSG;  // Pin = 1 (inactive)
	//	STATUS_NBSY_PORT |= STATUS_NBSY;  // Pin = 1 (inactive)
	//	STATUS_NREQ_PORT |= STATUS_NREQ;  // Pin = 1 (inactive)
	//	STATUS_INO_PORT  |= STATUS_INO;   // Pin = 1 (inactive)
	//	STATUS_CND_PORT  |= STATUS_CND;   // Pin = 1 (inactive)
	
	TM_GPIO_SetPinHigh(GPIOC, SCSI_MSG_Pin | SCSI_BSY_Pin | SCSI_REQ_Pin | SCSI_C_D_Pin);
	
	
	// Configure the SCSI signal input pins to input
	//	NRST_DDR &= ~NRST;  // Input
	//	NCONF_DDR &= ~NCONF;  // Input
	//	NSEL_DDR &= ~NSEL;  // Input
	//	NACK_DDR &= ~NACK;  // Input
	//	INTNEXT_DDR &= ~INTNEXT;  // Input
	//	
	//	NRST_PORT &= ~NRST;  // Turn off weak pull-up
	//	NCONF_PORT &= ~NCONF;  // Turn off weak pull-up
	//	NSEL_PORT &= ~NSEL;  // Turn off weak pull-up
	//	NACK_PORT &= ~NACK;  // Turn off weak pull-up
	//	INTNEXT_PORT &= ~INTNEXT;  // Turn off weak pull-up
	

	TM_GPIO_Init(GPIOA, SCSI_RST_Pin | SCSI_CONF_Pin, TM_GPIO_Mode_IN, TM_GPIO_OType_OD, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	TM_GPIO_Init(GPIOC, SCSI_SEL_Pin | SCSI_ACK_Pin | SCSI_ATN_Pin, TM_GPIO_Mode_IN, TM_GPIO_OType_OD, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	

	// Set up the interrupt service routines for reset and configure signals
//	EICRA = 0;	
//	EICRB = 0;
//	EIMSK = 0;
	
//	NRST_EICR |= (1 << NRST_ISC1); 		// Failing edge of NRST
//	NCONF_EICR |= (1 << NCONF_ISC1); 	// Failing edge of NCONF
//	EIMSK |= (1 << NRST_INT); 			// Enable NRST interrupt
//	EIMSK |= (1 << NCONF_INT); 			// Enable NCONF interrupt

//	/*Configure GPIO pins : SCSI_RST_Pin SCSI_CONF_Pin */
//	GPIO_InitTypeDef GPIO_InitStruct;
//	
//	GPIO_InitStruct.Pin = SCSI_RST_Pin | SCSI_CONF_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// RST 
	if (TM_EXTI_Attach(GPIOA, GPIO_Pin_0, TM_EXTI_Trigger_Falling) != TM_EXTI_Result_Ok) {
		
	}
//	// CONF
//	if (TM_EXTI_Attach(GPIOA, GPIO_Pin_1, TM_EXTI_Trigger_Falling) != TM_EXTI_Result_Ok) {
//		
//	}
	TM_GPIO_Init(SCSI_RST_GPIO_Port, SCSI_RST_Pin, TM_GPIO_Mode_OUT, TM_GPIO_OType_OD, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	
	TM_GPIO_SetPinHigh(SCSI_RST_GPIO_Port, SCSI_RST_Pin);
	
}

void TM_EXTI_Handler(uint16_t GPIO_Pin) {
	// Rst
	if (GPIO_Pin == GPIO_Pin_0) {
		nrstFlag = true;
	}
    
	// Conf
	
	if (GPIO_Pin == GPIO_Pin_1) {
		uint8_t databusValue;
	
		// Read the databus value (containing the configuration command)
		// Don't invert the databus if we are connected to the internal bus
		if(hostadapterConnectedToExternalBus()) databusValue = ~DATABUS_PORT->IDR;
	
		// All debug off (Command 0)
		if(databusValue == 0)
		{
			debugFlag_filesystem = false;
			debugFlag_scsiCommands = false;
			debugFlag_scsiBlocks = false;
			debugFlag_scsiFcodes = false;
			debugFlag_scsiState = false;
			debugFlag_fatfs = false;
		}
	
		// All debug on (Command 1)
		if(databusValue == 1)
		{
			debugFlag_filesystem = true;
			debugFlag_scsiCommands = true;
			debugFlag_scsiBlocks = false;
			debugFlag_scsiFcodes = true;
			debugFlag_scsiState = true;
			debugFlag_fatfs = true;
		}
	
		// File system debug on/off (Command 10/11)
		if(databusValue == 10) debugFlag_filesystem = true;
		if (databusValue == 11) debugFlag_filesystem = false;
	
		// SCSI commands debug on/off (Command 12/13)
		if(databusValue == 12) debugFlag_scsiCommands = true;
		if (databusValue == 13) debugFlag_scsiCommands = false;
	
		// SCSI blocks debug on/off (Command 14/15)
		if(databusValue == 14) debugFlag_scsiBlocks = true;
		if (databusValue == 15) debugFlag_scsiBlocks = false;
	
		// SCSI F-codes debug on/off (Command 16/17)
		if(databusValue == 16) debugFlag_scsiFcodes = true;
		if (databusValue == 17) debugFlag_scsiFcodes = false;
	
		// SCSI state debug on/off (Command 18/19)
		if(databusValue == 18) debugFlag_scsiState = true;
		if (databusValue == 19) debugFlag_scsiState = false;
	
		// FAT FS debug on/off (Command 20/21)
		if(databusValue == 20) debugFlag_fatfs = true;
		if (databusValue == 21) debugFlag_fatfs = false;
        
	}
}



// Reset the host adapter (called when the host signals reset)
void hostadapterReset(void)
{
	// Set the host adapter databus to input
	hostadapterDatabusInput();
	
	// Turn off all host adapter signals
//	STATUS_NMSG_PORT |= STATUS_NMSG;
//	STATUS_NBSY_PORT |= STATUS_NBSY;
//	STATUS_NREQ_PORT |= STATUS_NREQ;
//	STATUS_INO_PORT  |= STATUS_INO;
//	STATUS_CND_PORT  |= STATUS_CND;

	TM_GPIO_SetPinHigh(GPIOC, SCSI_MSG_Pin | SCSI_BSY_Pin | SCSI_REQ_Pin | SCSI_I_O_Pin | SCSI_C_D_Pin);
	
}


// Set the databus direction to input
inline void hostadapterDatabusInput(void)
{
	// Set the host adapter databus to input
	//DATABUS_PORT->MODER &= 0xFFFF0000; // clear the bits first - reset value 
		
	// Turn off weak pull-ups
	//DATABUS_PORT->PUPDR &= 0xFFFF0000;
	
	TM_GPIO_Init(DATABUS_PORT, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, TM_GPIO_Mode_IN, TM_GPIO_OType_OD, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	
	
}

// Set the databus direction to output
inline void hostadapterDatabusOutput(void)
{
	
	//DATABUS_PORT->MODER &= 0xFFFF0000;  // clear the bits first - reset value 
	// Set the databus direction to output
	//DATABUS_PORT->MODER |= 0x00005555;
	
	TM_GPIO_Init(DATABUS_PORT, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, TM_GPIO_Mode_OUT, TM_GPIO_OType_OD, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
}

// Read a byte from the databus (directly)
inline uint8_t hostadapterReadDatabus(void)
{
	//return ~DATABUS_PORT->IDR;
	//return ~(*((uint8_t *)&DATABUS_PORT->IDR));
	return ~(DATABUS_PORT->IDR & 0xff); 
}

// Write a byte to the databus (directly)
inline void hostadapterWritedatabus(uint8_t databusValue)
{
	DATABUS_PORT->ODR = ~databusValue;
	//TM_GPIO_SetPortValue(DATABUS_PORT, ~databusValue);
}

// Function to read a byte from the host (using REQ/ACK)
inline uint8_t hostadapterReadByte(void)
{
	uint8_t databusValue = 0;

	// Set the REQuest signal
	//SCSI_REQ_GPIO_Port->ODR &= ~STATUS_NREQ;  // REQ = 0 (active)
	TM_GPIO_SetPinLow(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);
	
	// Wait for ACKnowledge
	//while(((SCSI_ACK_GPIO_Port->IDR & NACK) != 0) && nrstFlag == false);
	while((TM_GPIO_GetInputPinValue(SCSI_ACK_GPIO_Port, SCSI_ACK_Pin) != 0) && nrstFlag == false);
	
	// Clear the REQuest signal
	//SCSI_REQ_GPIO_Port->ODR |= STATUS_NREQ;  // REQ = 1 (inactive)
	TM_GPIO_SetPinHigh(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);
	
	// Read the databus value
	databusValue = ~(DATABUS_PORT->IDR & 0xff); 
	
	return databusValue;
}

// Function to write a byte to the host (using REQ/ACK)
inline void hostadapterWriteByte(uint8_t databusValue)
{
	// Write the byte of data to the databus
	DATABUS_PORT->ODR = ~databusValue;
	
	// Set the REQuest signal
	//STATUS_NREQ_PORT &= ~STATUS_NREQ;  // REQ = 0 (active)
	TM_GPIO_SetPinLow(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);
	
	// Wait for ACKnowledge
	//while(((NACK_PIN & NACK) != 0) && nrstFlag == false);
	//while(((SCSI_ACK_GPIO_Port->IDR & NACK) != 0) && nrstFlag == false);
	while((TM_GPIO_GetInputPinValue(SCSI_ACK_GPIO_Port, SCSI_ACK_Pin) != 0) && nrstFlag == false);
	
	// Clear the REQuest signal
	//SCSI_REQ_GPIO_Port->ODR |= STATUS_NREQ;   // REQ = 1 (inactive)
	TM_GPIO_SetPinHigh(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);
}

// Function to write the host reset flag
void hostadapterWriteResetFlag(bool flagState)
{
	nrstFlag = flagState;
}

// Function to return the state of the host reset flag
bool hostadapterReadResetFlag(void)
{
	return nrstFlag;
}

// Function to write the data phase flags and control databus direction
// Note: all SCSI signals are inverted logic
void hostadapterWriteDataPhaseFlags(bool message, bool commandNotData, bool inputNotOutput)
{
	if (message) TM_GPIO_SetPinLow(SCSI_MSG_GPIO_Port, SCSI_MSG_Pin); // SCSI_MSG_GPIO_Port->IDR &= ~STATUS_NMSG;  // MSG = active
	else TM_GPIO_SetPinHigh(SCSI_MSG_GPIO_Port, SCSI_MSG_Pin);   // MSG = inactive
	
	if(commandNotData) TM_GPIO_SetPinLow(SCSI_C_D_GPIO_Port, SCSI_C_D_Pin);   // SCSI_C_D_GPIO_Port->IDR &= ~STATUS_CND;  //  CD = active
	else TM_GPIO_SetPinHigh(SCSI_C_D_GPIO_Port, SCSI_C_D_Pin);   //  CD = inactive
	
	if(inputNotOutput)
	{
		//SCSI_I_O_GPIO_Port->ODR &= ~STATUS_INO;  //  IO = active
		TM_GPIO_SetPinLow(SCSI_I_O_GPIO_Port, SCSI_I_O_Pin); 
		hostadapterDatabusOutput();
	}
	else
	{
		//SCSI_I_O_GPIO_Port->ODR |= STATUS_INO;   //  IO = inactive
		TM_GPIO_SetPinHigh(SCSI_I_O_GPIO_Port, SCSI_I_O_Pin); 
		hostadapterDatabusInput();
	}

}


// Function to write the host busy flag
// Note: all SCSI signals are inverted logic
void hostadapterWriteBusyFlag(bool flagState)
{
//	if (flagState) TM_GPIO_SetPinHigh(SCSI_BSY_GPIO_Port, SCSI_BSY_Pin); // SCSI_BSY_GPIO_Port->IDR &= ~STATUS_NBSY;  // BSY = inactive
//	else TM_GPIO_SetPinLow(SCSI_BSY_GPIO_Port, SCSI_BSY_Pin); //SCSI_BSY_GPIO_Port->ODR |= STATUS_NBSY;   // BSY = active
	if(flagState) TM_GPIO_SetPinLow(SCSI_BSY_GPIO_Port, SCSI_BSY_Pin);  // SCSI_BSY_GPIO_Port->IDR &= ~STATUS_NBSY;  // BSY = inactive
   else TM_GPIO_SetPinHigh(SCSI_BSY_GPIO_Port, SCSI_BSY_Pin); //SCSI_BSY_GPIO_Port->ODR |= STATUS_NBSY;   // BSY = acti
}

bool hostadapterReadBusyFlag(void)
{
	if (TM_GPIO_GetInputPinValue(SCSI_BSY_GPIO_Port, SCSI_BSY_Pin) != 0) return false;
	
	return true;
	
}


// Function to write the host request flag
// Note: all SCSI signals are inverted logic
void hostadapterWriteRequestFlag(bool flagState)
{
	//if (flagState) TM_GPIO_SetPinHigh(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);  // SCSI_REQ_GPIO_Port->IDR &= ~STATUS_NREQ;  // REQ = inactive
	//else TM_GPIO_SetPinLow(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);  //SCSI_REQ_GPIO_Port->ODR |= STATUS_NREQ;   // REQ = active
	if(flagState) TM_GPIO_SetPinLow(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);   // SCSI_REQ_GPIO_Port->IDR &= ~STATUS_NREQ;  // REQ = inactive
	else TM_GPIO_SetPinHigh(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);   //SCSI_REQ_GPIO_Port->ODR |= STATUS_NREQ;   // REQ = active
}

// Function to read the state of the host select flag
// Note: all SCSI signals are inverted logic
bool hostadapterReadSelectFlag(void)
{
	//if ((SCSI_SEL_GPIO_Port->IDR & NSEL) != 0) return false;
	
	if(TM_GPIO_GetInputPinValue(SCSI_SEL_GPIO_Port, SCSI_SEL_Pin) != 0) return false;
	
	return true;
}

// Function to determine if the host adapter is connected to the external or internal
// host bus
bool hostadapterConnectedToExternalBus(void)
{
	return true;  // External bus
}

// Host DMA transfer functions ----------------------------------------------------------

// Host reads data from SCSI device using DMA transfer (reads a 256 byte block)
// Returns number of bytes transferred (for debug in case of DMA failure)
uint16_t hostadapterPerformReadDMA(uint8_t *dataBuffer)
{
	uint16_t currentByte = 0;
	uint32_t timeoutCounter = 0;

	// Loop to write bytes (unless a reset condition is detected)
	while(currentByte < 256 && timeoutCounter != TOC_MAX)
	{
		// Write the current byte to the databus and point to the next byte
		DATABUS_PORT->ODR = ~dataBuffer[currentByte++];

		// Set the REQuest signal
		//STATUS_NREQ_PORT &= ~STATUS_NREQ;  // REQ = 0 (active)
		//SCSI_REQ_GPIO_Port->ODR &= ~STATUS_NREQ;   // REQ = 0 (active)
		TM_GPIO_SetPinLow(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);
		
		// Wait for ACKnowledge
		timeoutCounter = 0;  // Reset timeout counter
		
		//while((SCSI_ACK_GPIO_Port->IDR & NACK) != 0)
		while(TM_GPIO_GetInputPinValue(SCSI_ACK_GPIO_Port, SCSI_ACK_Pin) != 0)
		{
			if (++timeoutCounter == TOC_MAX)
			{
				// Set the host reset flag and quit
				nrstFlag = true;
				return currentByte - 1;
			}
		}
		
		// Clear the REQuest signal
		//STATUS_NREQ_PORT |= STATUS_NREQ;  // REQ = 1 (inactive)
		//SCSI_REQ_GPIO_Port->ODR |= STATUS_NREQ;   // REQ = 1 (inactive)
		TM_GPIO_SetPinHigh(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);
	}
	
	return currentByte - 1;
}

// Host writes data to SCSI device using DMA transfer (writes a 256 byte block)
// Returns number of bytes transferred (for debug in case of DMA failure)
uint16_t hostadapterPerformWriteDMA(uint8_t *dataBuffer)
{
	uint16_t currentByte = 0;
	uint32_t timeoutCounter = 0;

	// Loop to read bytes (unless a reset condition is detected)
	while(currentByte < 256 && timeoutCounter != TOC_MAX)
	{
		// Set the REQuest signal
		//SCSI_REQ_GPIO_Port->ODR &= ~STATUS_NREQ;   // REQ = 0 (active)
		TM_GPIO_SetPinLow(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);
		
		// Wait for ACKnowledge
		timeoutCounter = 0;  // Reset timeout counter
		
		//while((SCSI_ACK_GPIO_Port->IDR & NACK) != 0)
		while(TM_GPIO_GetInputPinValue(SCSI_ACK_GPIO_Port, SCSI_ACK_Pin) != 0)
		{
			if (++timeoutCounter == TOC_MAX)
			{
				// Set the host reset flag and quit
				nrstFlag = true;
				return currentByte;
			}
		}
		
		// Read the current byte from the databus and point to the next byte
		dataBuffer[currentByte++] = ~DATABUS_PORT->IDR;
		
		// Clear the REQuest signal
		//SCSI_REQ_GPIO_Port->ODR |= STATUS_NREQ;   // REQ = 1 (inactive)
		TM_GPIO_SetPinHigh(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin);
	}
	
	return currentByte - 1;
}
