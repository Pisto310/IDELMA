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

//**********    LOCAL VARIABLES   ************//





//**********    LOCAL FUNCTIONS DECLARATION   ************//

byte* boardInfosToArray();

//**********    LOCAL FUNCTIONS DECLARATION   ************//






//**********    GLOBAL FUNC DEFINITION   ************//

board_infos_t getBoardInfos() {
  return boardInfo;
}

// Fills buffer with boardInfo struct content and returns how many bytes it now contains
size_t infosBufferFill(byte byteBuffer[64]) {
  
  byte* iterArray = boardInfosToArray();
  
  for(uint8_t index = 0; index < BOARD_INFO_STRUCT_LEN(boardInfo); ++index) {
    byteBuffer[index] = *(iterArray + index);
  }
  return(BOARD_INFO_STRUCT_LEN(boardInfo));
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

byte* boardInfosToArray() {
  
  static byte boardInfosArray[sizeof(boardInfo)] = {
    boardInfo.sn,
    boardInfo.fwVersionMajor,
    boardInfo.fwVersionMinor,
    boardInfo.sectionsMgmt.maxAllowed,
    boardInfo.sectionsMgmt.stillAvailable,
    boardInfo.sectionsMgmt.currentlyUsed,
    boardInfo.pixelsMgmt.maxAllowed,
    boardInfo.pixelsMgmt.stillAvailable,
    boardInfo.pixelsMgmt.currentlyUsed
  };

  return(boardInfosArray);
}

//**********    LOCAL FUNC DEFINITION   ************//