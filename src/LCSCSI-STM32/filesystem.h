#include <stdio.h>

#define SECTOR_SIZE				512

// Read/Write sector buffer (must be 256 bytes minimum)
// Testing shows that this is optimal when it matches the sector size of
// the SD card (which is 512 bytes).

#define SECTOR_BUFFER_SIZE		SECTOR_SIZE


// Calculate the length of the sector buffer in 256 byte sectors
#define SECTOR_BUFFER_LENGTH	(SECTOR_BUFFER_SIZE / SECTOR_SIZE)

// External prototypes
void filesystemInitialise(void);
void filesystemReset(void);

bool filesystemMount(void);
bool filesystemDismount(void);

void filesystemSetLunDirectory(uint8_t lunDirectoryNumber);
uint8_t filesystemGetLunDirectory(void);

bool filesystemSetLunStatus(uint8_t lunNumber, bool lunStatus);
bool filesystemReadLunStatus(uint8_t lunNumber);
bool filesystemTestLunStatus(uint8_t lunNumber);
void filesystemReadLunUserCode(uint8_t lunNumber, uint8_t userCode[5]);

bool filesystemCheckLunDirectory(uint8_t lunDirectory);
bool filesystemCheckLunImage(uint8_t lunNumber);

uint32_t filesystemGetLunSizeFromDsc(uint8_t lunDirectory, uint8_t lunNumber);
bool filesystemCreateDscFromLunImage(uint8_t lunDirectory, uint8_t lunNumber, uint32_t lunFileSize);

void filesystemGetUserCodeFromUcd(uint8_t lunDirectoryNumber, uint8_t lunNumber);

bool filesystemCreateLunImage(uint8_t lunNumber);
bool filesystemCreateLunDescriptor(uint8_t lunNumber);
bool filesystemReadLunDescriptor(uint8_t lunNumber, uint8_t buffer[]);
bool filesystemWriteLunDescriptor(uint8_t lunNumber, uint8_t buffer[]);
bool filesystemFormatLun(uint8_t lunNumber, uint8_t dataPattern);

bool filesystemOpenLunForRead(uint8_t lunNumber, uint32_t startSector, uint32_t requiredNumberOfSectors);
bool filesystemReadNextSector(uint8_t buffer[]);
bool filesystemCloseLunForRead(void);
bool filesystemOpenLunForWrite(uint8_t lunNumber, uint32_t startSector, uint32_t requiredNumberOfSectors);
bool filesystemWriteNextSector(uint8_t buffer[]);
bool filesystemCloseLunForWrite(void);

bool filesystemSetFatDirectory(uint8_t *buffer);
bool filesystemGetFatFileInfo(uint32_t fileNumber, uint8_t *buffer);
bool filesystemOpenFatForRead(uint32_t fileNumber, uint32_t blockNumber);
bool filesystemReadNextFatBlock(uint8_t *buffer);
bool filesystemCloseFatForRead(void);
