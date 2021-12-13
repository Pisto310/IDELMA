/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#include "Board.h"


board_infos_t boardInfo = {
  .sn = SERIAL_NUMBER,
  .fwVersionMajor = FW_VERSION_MAJOR,
  .fwVersionMinor = FW_VERSION_MINOR
};


//**********    GLOBAL FUNC DEFINITION   ************//

board_infos_t getBoardInfos() {
  return boardInfo;
}

// Fills buffer with boardInfo struct content and returns how many bytes it now contains
size_t infosBufferFill(byte byteBuffer[64]) {
  for(uint8_t index = 0; index < BOARD_INFO_STRUCT_LEN(boardInfo); ++index) {
    switch (index) {
    
    case 0:
      byteBuffer[index] = boardInfo.sn;
      break;

    case 1:
      byteBuffer[index] = boardInfo.fwVersionMajor;
      break;
    
    case 2:
      byteBuffer[index] = boardInfo.fwVersionMinor;
    }
  }
  return(BOARD_INFO_STRUCT_LEN(boardInfo));
}

//**********    GLOBAL FUNC DEFINITION   ************//