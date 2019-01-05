
#define PSTR(str) (str)

// External globals
extern volatile bool debugFlag_filesystem;
extern volatile bool debugFlag_scsiCommands;
extern volatile bool debugFlag_scsiBlocks;
extern volatile bool debugFlag_scsiFcodes;
extern volatile bool debugFlag_scsiState;
extern volatile bool debugFlag_fatfs;

// Function prototypes
void debugString(char *string);
void debugString_P(char *string);

void debugStringInt8Hex_P(const char *addr, uint8_t integerValue, bool newLine);
void debugStringInt16_P(const char *addr, uint16_t integerValue, bool newLine);
void debugStringInt32_P(const char *addr, uint32_t integerValue, bool newLine);

void debugSectorBufferHex(uint8_t *buffer, uint16_t numberOfBytes);
void debugLunDescriptor(uint8_t *buffer);

void u_printf(const char *fmt, ...);