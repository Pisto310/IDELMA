/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Every metadatas, pins configuration and size/limitation set for MCU
*/

#ifndef BOARD_H_
#define BOARD_H_

#include "Arduino.h"
#include "Serial_lib.h"
#include "User_Lib.h"
#include "EEPROM_lib.h"

#define HEARTBEAT_RHYTHM                 500U

#define SERIAL_NUMBER                    0x12345678U

#define FW_VERSION_MAJOR                 0
#define FW_VERSION_MINOR                 1
#define FW_VERSION_PATCH                 0

#define BOARD_INFO_STRUCT_LEN(_infoStruct)    (sizeof(_infoStruct))

#define SERIAL_BUFFER_SIZE              64U

#define BYTE_SIZE                  (sizeof(byte))

/* --------- MEMORY HEAP MANAGEMENT --------- */

#define PXLINFO_HEAP_SIZE              100U
#define PXLINFO_MAX_INDEX(_size)    (_size - 1)

/* --------- MEMORY HEAP MANAGEMENT --------- */


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

typedef struct BrdMgmtMetaDataAttr {
  byte capacity;
  byte remaining;
  byte assigned;
}brdMgmt_attr_t;

typedef struct BrdMetaDataPtrs {
  const uint32_t*   serialNumPtr;
  const firmware_t* fwVersionPtr;
  brdMgmt_attr_t*   sctsMgmtMetaDataPtr;
  brdMgmt_attr_t*   pxlsMgmtMetaDataPtr;
}brd_metadata_ptrs_t;

// typedef struct SerialSctMetaData {
//   byte sctIdx;
//   byte pxlCount;
//   byte brightness;
//   byte singlePxlCtrl;
// }ser_sct_metadata_t;

// typedef struct ConfigBrdDataPckt {
//   byte           subCmd;
//   byte           sctIdx;
//   sct_metadata_t configData;
// }config_brd_data_t;

//**********    GLOBAL TYPES DECLARATION   ************//


//**********    GLOBAL FUNC DECLARATION   *************//

void bootUp();

// void configBrd(serial_obj_t* nada, byte serialBuffer[], uint8_t mssgLen);
void configBrd(byte serialBuffer[], uint8_t mssgLen);

brd_metadata_ptrs_t getBrdMgmtMetaDatasPtr();

bool remainingHeapSpace(uint8_t spaceNeeded);
bool remainingSctsPins();
void sectionsMgmtAdd();
void pixelsMgmtAdd(uint8_t spaceFilled);
void sectionsMgmtRemove();
void pixelsMgmtRemove(uint8_t spaceFreed);

//******************************************           DEBUG           ************************************************//
void eepromReset();
void allOff();
//******************************************           DEBUG           ************************************************//

//**********    GLOBAL FUNC DECLARATION   ************//

#endif  /* BOARD_H_ */