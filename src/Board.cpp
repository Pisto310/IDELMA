/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#include "Board.h"
#include <EEPROM.h>


//**********    LOCAL VARIABLES   ************//

board_infos_t boardInfo = {
  .sn = SERIAL_NUMBER,
  .fwVersionMajor = FW_VERSION_MAJOR,
  .fwVersionMinor = FW_VERSION_MINOR,
  .sectionsMgmt = {
    .maxAllowed     = MAX_NO_SCTS,
    .stillAvailable = MAX_NO_SCTS,
    .currentlyUsed  = 0
  },
  .pixelsMgmt = {
    .maxAllowed     = PXLINFO_HEAP_SIZE,
    .stillAvailable = PXLINFO_HEAP_SIZE,
    .currentlyUsed  = 0
  }
};

byte* ptrBoardInfo = &boardInfo.sn;

//**********    LOCAL VARIABLES   ************//





//**********    LOCAL FUNCTIONS DECLARATION   ************//



//**********    LOCAL FUNCTIONS DECLARATION   ************//






//**********    GLOBAL FUNC DEFINITION   ************//

board_infos_t getBoardInfos() {
  return boardInfo;
}

// Fills buffer with boardInfo struct content and returns how many bytes it now contains
// User can choose from where to start in the struct (for example, sending only pxlsInfo mgmt info) 
uint8_t boardInfosBufferFill(byte byteBuffer[64], uint8_t start, uint8_t length) {
  
  uint8_t maxAllowedLen = (uint8_t) BOARD_INFO_STRUCT_LEN(boardInfo);
  
  // if length == zero, user didn't enter a value, so set it to default using macro
  if(!length) {
    length = maxAllowedLen;
  }

  // check if all parameters passed are permissible between one another
  if(start > (maxAllowedLen - 1)) {
    start = maxAllowedLen - 1;
  }
  else if((length + start) > maxAllowedLen) {
    length = maxAllowedLen - start;
  }

  for(uint8_t index = start; index < (start + length); ++index) {
    byteBuffer[index-start] = *(ptrBoardInfo + index * sizeof(byte));
  }
  return(length);
}

// returns either True of False if enough heap space is available to create the number of pixels asked for
bool remainingHeapSpace(uint8_t spaceNeeded) {
  return(boardInfo.pixelsMgmt.stillAvailable >= spaceNeeded);
}

// returns True or False depending if pins are still available to create anymore sections
bool remainingSctsPins() {
  return(boardInfo.sectionsMgmt.currentlyUsed < boardInfo.sectionsMgmt.maxAllowed);
}

// Only called within createSection() func
void sectionsMgmtAdd() {
  if(boardInfo.sectionsMgmt.stillAvailable) {
    boardInfo.sectionsMgmt.currentlyUsed++;
    boardInfo.sectionsMgmt.stillAvailable--;
  }
}

// Only called within createSection() func
void pixelsMgmtAdd(uint8_t spaceFilled) {
  if(boardInfo.pixelsMgmt.stillAvailable) {
    boardInfo.pixelsMgmt.currentlyUsed += spaceFilled;
    boardInfo.pixelsMgmt.stillAvailable -= spaceFilled;
  }
}

// Only called within resetSection() and clearSection() funcs
void sectionsMgmtRemove() {
  if(boardInfo.sectionsMgmt.currentlyUsed) {
    boardInfo.sectionsMgmt.currentlyUsed--;
    boardInfo.sectionsMgmt.stillAvailable++;
  }
}

// Only called within resetSection() and clearSection() funcs
void pixelsMgmtRemove(uint8_t spaceFreed) {
  if(boardInfo.pixelsMgmt.currentlyUsed && (boardInfo.pixelsMgmt.currentlyUsed - spaceFreed >= 0)) {
    boardInfo.pixelsMgmt.currentlyUsed -= spaceFreed;
    boardInfo.pixelsMgmt.stillAvailable += spaceFreed;
  }
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

bool powerUpEepromCheck(void) {
  
  // Tells how many byte should be saved in eeprom 1st page according to macro values
  // The + 1 indicates the first byte of the eeprom which is the number of sections previously assigned
  uint8_t eepromSctsConfigLen = (sizeof(section_info_t) * MAX_NO_SCTS) + 1;
  bool configFromEeprom = 1;

  for(uint16_t eepromAddr = EEPROM_PAGE_ADDR(EEPROM_SCTS_MGMT_PAGE); eepromAddr < eepromSctsConfigLen; eepromAddr++) {
    if(EEPROM.read(eepromAddr) == 0xFF) {
      configFromEeprom = !configFromEeprom;
      break;
    }
  }
  return(configFromEeprom);
}

//**********    GLOBAL FUNC DEFINITION   ************//





//**********    LOCAL FUNC DEFINITION   ************//



//**********    LOCAL FUNC DEFINITION   ************//