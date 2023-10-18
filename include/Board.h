/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#ifndef BOARD_H_
#define BOARD_H_

#include "Arduino.h"
#include "SK6812.h"

#define SERIAL_NUMBER                    0x12345678U

#define FW_VERSION_MAJOR                 0
#define FW_VERSION_MINOR                 1
#define FW_VERSION_PATCH                 0

#define BOARD_INFO_STRUCT_LEN(_infoStruct)    (sizeof(_infoStruct))

#define SERIAL_BUFFER_SIZE              64

#define BYTE_SIZE                  (sizeof(byte))

/* --------- MEMORY HEAP MANAGEMENT --------- */

#define PXLINFO_HEAP_SIZE               100
#define PXLINFO_MAX_INDEX(_size)    (_size - 1)

/* --------- MEMORY HEAP MANAGEMENT --------- */


/* ------------- EEPROM SECTION ------------- */

#define EEPROM_SCTS_MGMT_PAGE            0

#define EEPROM_PAGE_SIZE               64U
#define EEPROM_PAGE_ADDR(_pageNumber)       (EEPROM_PAGE_SIZE * _pageNumber)

/* ------------- EEPROM SECTION ------------- */


/* ---------- PHYSICAL PINS OF MCU ---------- */

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

/* ---------- PHYSICAL PINS OF MCU ---------- */


//**********    GLOBAL TYPES DECLARATION   ************//

typedef struct FW_Version {
  byte majorNum;
  byte minorNum;
  byte patchNum;
}firmware_t;

typedef struct MutableBrdInfo {
  byte capacity;
  byte remaining;
  byte assigned;
}mutable_brdInfo_t;

typedef struct BoardInfosPtrs {
  const uint32_t*    serialNumPtr;
  const firmware_t*  fwVersionPtr;
  mutable_brdInfo_t* sectionsInfoPtr;
  mutable_brdInfo_t* pixelsInfoPtr;
}board_infos_ptrs_t;

//**********    GLOBAL TYPES DECLARATION   ************//


//**********    GLOBAL FUNC DECLARATION   ************//

board_infos_ptrs_t getBoardInfosPtrs();

bool remainingHeapSpace(uint8_t spaceNeeded);
bool remainingSctsPins();
void sectionsMgmtAdd();
void pixelsMgmtAdd(uint8_t spaceFilled);
void sectionsMgmtRemove();
void pixelsMgmtRemove(uint8_t spaceFreed);

void configBrd(byte serialBuffer[], uint8_t mssgLen);

bool eepromBootSaveCheck(void);

void eepromReset(void);
uint16_t eepromSave(uint16_t eepromAddr, byte* ramAddr, size_t blockSize, uint8_t numBlocks);

//**********    GLOBAL FUNC DECLARATION   ************//

#endif  /* BOARD_H_ */