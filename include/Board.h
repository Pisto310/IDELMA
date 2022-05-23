/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#ifndef BOARD_H_
#define BOARD_H_

#include "Arduino.h"
#include "SK6812.h"

#define SERIAL_NUMBER                    1
#define FW_VERSION_MAJOR                 0
#define FW_VERSION_MINOR                 1

#define BOARD_INFO_STRUCT_LEN(_infoStruct)    (sizeof(_infoStruct))

#define SERIAL_BUFFER_SIZE              64

#define BYTE_SIZE                  (sizeof(byte))

/* --------- MEMORY HEAP MANAGEMENT --------- */

#define PXLINFO_HEAP_SIZE               50
#define PXLINFO_MAX_INDEX(_size)    (_size - 1)

/* --------- MEMORY HEAP MANAGEMENT --------- */


/* ------------- EEPROM SECTION ------------- */

#define EEPROM_SCTS_MGMT_PAGE            0

#define EEPROM_PAGE_SIZE               64U
#define EEPROM_PAGE_ADDR(_pageNumber)       (EEPROM_PAGE_SIZE * _pageNumber)

/* ------------- EEPROM SECTION ------------- */



/* --------- PHYSICAL PINS OF MCU --------- */

// maximum number of sections that are possible to instanciate w/ the Arduino Mega
#define MAX_NO_SCTS         12

// all possible PWM pins on the Mega that can be used to control the SK6812 LEDs (12 total)
#define PIN_SCT_0            2
#define PIN_SCT_1            3
#define PIN_SCT_2            4
#define PIN_SCT_3            5
#define PIN_SCT_4            6
#define PIN_SCT_5            7
#define PIN_SCT_6            8
#define PIN_SCT_7            9
#define PIN_SCT_8           10
#define PIN_SCT_9           11
#define PIN_SCT_10          12
#define PIN_SCT_11          13

/* --------- PHYSICAL PINS OF MCU --------- */



//**********    GLOBAL TYPES DECLARATION   ************//

typedef struct {
  byte maxAllowed;             // maximum numbers of sections allowed, defined by a macro & limited by usable pins on MCU
  byte stillAvailable;         // shows how many sections (pins) are still available
  byte currentlyUsed;          // keeps track of the number of sections initialized
}sections_usage_t;

typedef struct {  
  byte maxAllowed;             // maximum numbers of pixels allowed since heap space is set at start of code
  byte stillAvailable;         // shows how many blocks of pixelInfo_t size are still available in the heap
  byte currentlyUsed;          // keeps track of the blocks of heap used to store pixelInfo_t objects
}pixels_usage_t;

typedef struct {
  byte sn;
  byte fwVersionMajor;
  byte fwVersionMinor;
  sections_usage_t sectionsMgmt;
  pixels_usage_t pixelsMgmt;
}board_infos_t;

//**********    GLOBAL TYPES DECLARATION   ************//


//**********    GLOBAL FUNC DECLARATION   ************//

board_infos_t getBoardInfos();
uint8_t boardInfosBufferFill(byte byteBuffer[64], uint8_t start=0, uint8_t length=0);

bool remainingHeapSpace(uint8_t spaceNeeded);
bool remainingSctsPins();
void sectionsMgmtAdd();
void pixelsMgmtAdd(uint8_t spaceFilled);
void sectionsMgmtRemove();
void pixelsMgmtRemove(uint8_t spaceFreed);

bool eepromBootSaveCheck(void);

void eepromReset(void);
uint16_t eepromSave(uint16_t eepromAddr, byte* ramAddr, size_t blockSize, uint8_t numBlocks);

//**********    GLOBAL FUNC DECLARATION   ************//

#endif  /* BOARD_H_ */