/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#ifndef BOARD_H_
#define BOARD_H_

#include "Arduino.h"

#define SERIAL_NUMBER           1
#define FW_VERSION_MAJOR        0
#define FW_VERSION_MINOR        1

#define BOARD_INFO_STRUCT_LEN(_infoStruct)    (sizeof(_infoStruct))   

//**********    GLOBAL TYPES DECLARATION   ************//

typedef struct {
  byte sn;
  byte fwVersionMajor;
  byte fwVersionMinor;
}board_infos_t;

//**********    GLOBAL TYPES DECLARATION   ************//


//**********    GLOBAL FUNC DECLARATION   ************//

board_infos_t getBoardInfos();
size_t infosBufferFill(byte byteBuffer[64]);

//**********    GLOBAL FUNC DECLARATION   ************//

#endif  /* BOARD_H_ */