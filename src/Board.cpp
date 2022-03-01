/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#include "Board.h"



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
void sectionsMgmtUpdt() {
  if(boardInfo.sectionsMgmt.stillAvailable) {
    boardInfo.sectionsMgmt.currentlyUsed++;
    boardInfo.sectionsMgmt.stillAvailable--;
  }
}

// Only called within createSection() func
void pixelsMgmtUpdt(uint8_t spaceFilled) {
  if(boardInfo.pixelsMgmt.stillAvailable) {
    boardInfo.pixelsMgmt.currentlyUsed += spaceFilled;
    boardInfo.pixelsMgmt.stillAvailable -= spaceFilled;
  }
}

//**********    GLOBAL FUNC DEFINITION   ************//





//**********    LOCAL FUNC DEFINITION   ************//



//**********    LOCAL FUNC DEFINITION   ************//