
#include "tm_stm32_usart.h"
 
/* In stdio.h file is everything related to output stream */
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "defines.h"
#include "debug.h"


#define DEBUG_USART USART2

void u_printf(const char *fmt, ...);

// Define default debug output flags (all debug off)
// Note: these are modified by the configure interrupt, so they all
// need to be volatile variables
#ifdef DEBUG
	// Default debug settings for debug builds
	volatile bool debugFlag_filesystem = true;
	volatile bool debugFlag_scsiCommands = true;
	volatile bool debugFlag_scsiBlocks = false;
	volatile bool debugFlag_scsiFcodes = true;
	volatile bool debugFlag_scsiState = true;
	volatile bool debugFlag_fatfs = true;
#else
	// Default debug settings for release builds
	volatile bool debugFlag_filesystem = false;
	volatile bool debugFlag_scsiCommands = false;
	volatile bool debugFlag_scsiBlocks = false;
	volatile bool debugFlag_scsiFcodes = false;
	volatile bool debugFlag_scsiState = false;
	volatile bool debugFlag_fatfs = false;
#endif

// Logs messages out via the built in uart. 
void vprint(const char *fmt, va_list argp)
{
	char string[200];
	if (0 < vsprintf(string, fmt, argp)) // build string
		{
			//TM_USART_Puts(DEBUG_USART, (uint8_t*)string);  // send message via UART
			debugString(string);
		}
}


void u_printf(const char *fmt, ...) 
{
	va_list argp;
	va_start(argp, fmt);
	vprint(fmt, argp);
	va_end(argp);
}




// This function outputs a string stored in RAM space to the UART
void debugString(char *string)
{
	TM_USART_Puts(DEBUG_USART, string);
}


// This function outputs a string stored in program space to the UART
// It should be called with a statement such as:
// debugString_P(PSTR("This is an example\r\n"));
//
// This prevents debug strings using valuable RAM space
void debugString_P(char *string)
{
	debugString(string);
}

// This function outputs a string stored in program space to the UART
// It should be called with a statement such as:
// debugStringInt16_P(PSTR("My value = "), value, newLineFlag);
//
// This prevents debug strings using valuable RAM space
// The 8 bit unsigned integer is displayed as hex
void debugStringInt8Hex_P(const char *addr, uint8_t integerValue, bool newLine)
{

	
	// Output the debug message string
	debugString(addr);
	
	// Output the integer
	if(newLine) u_printf("0x%02x\r\n", integerValue);
	else u_printf("0x%02x", integerValue);
}


// This function outputs a string stored in program space to the UART
// It should be called with a statement such as:
// debugStringInt16_P(PSTR("My value = "), value, newLineFlag);
//
// This prevents debug strings using valuable RAM space
void debugStringInt16_P(const char *addr, uint16_t integerValue, bool newLine)
{
	
	// Output the debug message string
	debugString(addr);
	
	// Output the integer
	if(newLine) u_printf("%u\r\n", integerValue);
	else u_printf("%u", integerValue);
}

// This function outputs a string stored in program space to the UART
// It should be called with a statement such as:
// debugStringInt32_P(PSTR("My value = "), value, newLineFlag);
//
// This prevents debug strings using valuable RAM space
void debugStringInt32_P(const char *addr, uint32_t integerValue, bool newLine)
{
	char c;
	
	// Output the debug message string
	debugString(addr);
	
	// Output the integer
	if(newLine) u_printf("%lu\r\n", integerValue);
	else u_printf("%lu", integerValue);
}

// This function outputs a hex dump of the passed buffer
void debugSectorBufferHex(uint8_t *buffer, uint16_t numberOfBytes)
{
	uint16_t i = 0;
	uint16_t index = 16;
	uint16_t width = 16;  // Width of output in bytes

	for(uint16_t byteNumber = 0 ; byteNumber < numberOfBytes ; byteNumber += 16)
	{
		for (i = 0; i < index; i++)
		{
			u_printf("%02x ", buffer[i + byteNumber]);
		}
		for (uint16_t spacer = index; spacer < width; spacer++)
			u_printf("	");
		
		u_printf(": ");
		
		for (i = 0; i < index; i++)
		{
			if (buffer[i + byteNumber] < 32 || buffer[i + byteNumber] >126) u_printf(".");
			else u_printf("%c", buffer[i + byteNumber]);
		}
		
		u_printf("\r\n");
	}
	
	u_printf("\r\n");
}

// This function decodes the contents of the LUN descriptor and outputs it to debug
void debugLunDescriptor(uint8_t *buffer)
{
	debugString_P(PSTR("File system: LUN Descriptor contents:\r\n"));
	
	// The first 4 bytes are the Mode Select Parameter List (ACB-4000 manual figure 5-18)
	debugString_P(PSTR("File system: Mode Select Parameter List:\r\n"));
	debugStringInt16_P(PSTR("File system:   Reserved (0) = "), buffer[0], true);
	debugStringInt16_P(PSTR("File system:   Reserved (0) = "), buffer[1], true);
	debugStringInt16_P(PSTR("File system:   Reserved (0) = "), buffer[2], true);
	debugStringInt16_P(PSTR("File system:   Length of Extent Descriptor List (8) = "), buffer[3], true);
	
	// The next 8 bytes are the Extent Descriptor list (there can only be one of these
	// and it's always 8 bytes) (ACB-4000 manual figure 5-19)
	debugString_P(PSTR("File system: Extent Descriptor List:\r\n"));
	debugStringInt16_P(PSTR("File system:   Density code = "), buffer[4], true);
	debugStringInt16_P(PSTR("File system:   Reserved (0) = "), buffer[5], true);
	debugStringInt16_P(PSTR("File system:   Reserved (0) = "), buffer[6], true);
	debugStringInt16_P(PSTR("File system:   Reserved (0) = "), buffer[7], true);
	debugStringInt16_P(PSTR("File system:   Reserved (0) = "), buffer[8], true);
	debugStringInt32_P(PSTR("File system:   Block size = "),
		((uint32_t)buffer[9] << 16) +
	((uint32_t)buffer[10] << 8) + (uint32_t)buffer[11],
		true);
	
	// The next 12 bytes are the Drive Parameter List (ACB-4000 manual figure 5-20)
	debugString_P(PSTR("File system: Drive Parameter List:\r\n"));
	debugStringInt16_P(PSTR("File system:   List format code = "), buffer[12], true);
	debugStringInt16_P(PSTR("File system:   Cylinder count = "), (buffer[13] << 8) + buffer[14], true);
	debugStringInt16_P(PSTR("File system:   Data head count = "), buffer[15], true);
	debugStringInt16_P(PSTR("File system:   Reduced write current cylinder = "), (buffer[16] << 8) + buffer[17], true);
	debugStringInt16_P(PSTR("File system:   Write pre-compensation cylinder = "), (buffer[18] << 8) + buffer[19], true);
	debugStringInt16_P(PSTR("File system:   Landing zone position = "), buffer[20], true);
	debugStringInt16_P(PSTR("File system:   Step pulse output rate code = "), buffer[21], true);

	// Note:
	//
	// The drive size (actual data storage) is calculated by the following formula:
	//
	// tracks = heads * cylinders
	// sectors = tracks * 33
	// (the '33' is because SuperForm uses a 2:1 interleave format with 33 sectors per
	// track (F-2 in the ACB-4000 manual))
	// bytes = sectors * block size (block size is always 256 bytes)
}