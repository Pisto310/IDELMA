/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#include "Board.h"
#include "User_Lib.h"
#include <EEPROM.h>

//**********    LOCAL VARIABLES   ************//

// board_infos_t boardInfo = {
//   .sn = SERIAL_NUMBER,
//   .fwVersionMajor = FW_VERSION_MAJOR,
//   .fwVersionMinor = FW_VERSION_MINOR,
//   .sectionsMgmt = {
//     .maxAllowed     = MAX_NO_SCTS,
//     .stillAvailable = MAX_NO_SCTS,
//     .currentlyUsed  = 0
//   },
//   .pixelsMgmt = {
//     .maxAllowed     = PXLINFO_HEAP_SIZE,
//     .stillAvailable = PXLINFO_HEAP_SIZE,
//     .currentlyUsed  = 0
//   }
// };

// byte* ptrBoardInfo = &boardInfo.sn;

const uint32_t sn = SERIAL_NUMBER;

const firmware_t fwVersion = {
  .majorNum = FW_VERSION_MAJOR,
  .minorNum = FW_VERSION_MINOR,
  .patchNum = FW_VERSION_PATCH
};

mutable_brdInfo_t sectionsInfo = {
  .capacity = MAX_NO_SCTS,
  .remaining = MAX_NO_SCTS,
  .assigned = 0
};

mutable_brdInfo_t pixelsInfo = {
  .capacity = PXLINFO_HEAP_SIZE,
  .remaining = PXLINFO_HEAP_SIZE,
  .assigned = 0
};

board_infos_ptrs_t ptrBoardInfos = {
  .serialNumPtr = &sn,
  .fwVersionPtr = &fwVersion,
  .sectionsInfoPtr = &sectionsInfo,
  .pixelsInfoPtr = &pixelsInfo
};

//**********    LOCAL VARIABLES   ************//





//**********    LOCAL FUNCTIONS DECLARATION   ************//



//**********    LOCAL FUNCTIONS DECLARATION   ************//






//**********    GLOBAL FUNC DEFINITION   ************//

board_infos_ptrs_t getBoardInfosPtrs(void) {
  return ptrBoardInfos;
}

// returns either True of False if enough heap space is available to create the number of pixels asked for
bool remainingHeapSpace(uint8_t spaceNeeded) {
  return(pixelsInfo.remaining >= spaceNeeded);
}

// returns True or False depending if pins are still available to create anymore sections
bool remainingSctsPins() {
  return(sectionsInfo.assigned < sectionsInfo.capacity);
}

// Only called within createSection() func
void sectionsMgmtAdd() {
  if(sectionsInfo.remaining) {
    sectionsInfo.assigned++;
    sectionsInfo.remaining--;
  }
}

// Only called within createSection() func
void pixelsMgmtAdd(uint8_t spaceFilled) {
  if(pixelsInfo.remaining) {
    pixelsInfo.assigned += spaceFilled;
    pixelsInfo.remaining -= spaceFilled;
  }
}

// Only called within resetSection() and clearSection() funcs
void sectionsMgmtRemove() {
  if(sectionsInfo.assigned) {
    sectionsInfo.assigned--;
    sectionsInfo.remaining++;
  }
}

// Only called within resetSection() and clearSection() funcs
void pixelsMgmtRemove(uint8_t spaceFreed) {
  if(pixelsInfo.assigned && (pixelsInfo.assigned - spaceFreed >= 0)) {
    pixelsInfo.assigned -= spaceFreed;
    pixelsInfo.remaining += spaceFreed;
  }
}

bool eepromBootSaveCheck(void) {
  
  // Tells how many byte should be saved in eeprom 1st page according to macro values
  // The + 1 indicates the first byte of the eeprom which is the number of sections previously assigned
  uint8_t eepromSctsConfigLen = (sizeof(section_info_t) * MAX_NO_SCTS) + 1;
  bool configFromEeprom = 1;

  for(uint16_t eepromAddr = EEPROM_PAGE_ADDR(EEPROM_SCTS_MGMT_PAGE); eepromAddr < eepromSctsConfigLen; eepromAddr++) {
    if(EEPROM.read(eepromAddr) > MAX_NO_SCTS || EEPROM.read(eepromAddr) == 0) {
      configFromEeprom = !configFromEeprom;
      break;
    }
  }
  return(configFromEeprom);
}

// Func to reset the eeprom's content
// Note that this operation takes multiple seconds
void eepromReset(void) {
  for(uint16_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0xFF);
  }
}

uint16_t eepromSave(uint16_t eepromAddr, byte* ramAddr, size_t blockSize, uint8_t numBlocks) {

  uint16_t totalBytes = blockSize * numBlocks;

  for(uint16_t i = 0; i < totalBytes; i++) {

    EEPROM.update(eepromAddr, *(ramAddr + (i * BYTE_SIZE)));
    eepromAddr += BYTE_SIZE;

    // Serial.print((unsigned long) (ramStartAddr + i), HEX);
    // Serial.print(' ');
    // Serial.print(*(ramStartAddr + i), HEX);
    // Serial.print(' ');
    // Serial.print(eepromStartAddr, HEX);
    // Serial.print(' ');
    // Serial.println(EEPROM.read(eepromStartAddr), HEX);
  }
  return(eepromAddr);
}

//**********    GLOBAL FUNC DEFINITION   ************//





//**********    LOCAL FUNC DEFINITION   ************//



//**********    LOCAL FUNC DEFINITION   ************//