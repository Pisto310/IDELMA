/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib source file
*/

#include "Serial_lib.h"
#include "User_Lib.h"
#include "SK6812.h"
#include "Board.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//**********    LOCAL VARIABLES DECLARATION   ************//

enum serial_rqst {
  SER_RQST_NONE,
  SER_RQST_SER_NUM,
  SER_RQST_FW_VERS,
  SER_RQST_SCTS_MGMT,
  SER_RQST_PXLS_MGMT,
  SER_RQST_SETUP_SCT
  // SER_RQST_BRD_INFOS,
  // SER_RQST_SCT_INFO_ARR,
  // SER_RQST_SAVE_SCTS_CONFIG,
  // SER_RQST_LED_COLOR_CHANGE
};

const char acknowledge = 0x06;
const char lineFeed    = '\n';
const char spaceChar   = ' ';

//**********    LOCAL VARIABLES DECLARATION   ************//




//**********    LOCAL FUNCTIONS DECLARATION   ************//

void messageParsing(serial_obj_t *serialObj);
void clearBuffer(serial_obj_t *serialObj);
void rqstHandler(serial_obj_t *serialObj);

//**********    LOCAL FUNCTIONS DECLARATION   ************//



void serialRxCheck(serial_obj_t *serialObj) {

  switch (serialObj->rxStatus) {
  
  case SER_RX_RQST:
    serialObj->bytesInBuf = (*serialObj->serialPort).readBytes(serialObj->buffer, 64);
    if(serialObj->bytesInBuf) {
      // Read was succesful
      serialObj->rxStatus = SER_RX_CMPLT;
    }
    else {
      // Code breaking case, not supposed to be here
      serialObj->rxStatus = SER_RX_DEADEND;
      // Serial.print("Ain't supposed to be here!");
    }
    break;

  case SER_RX_CMPLT:
    // messageParsing(serialObj);
    // //clearBuffer(serialObj);
    // serialObj->rxStatus = SER_RX_IDLE;
    // serialObj->txStatus = SER_TX_IDLE;
    rqstHandler(serialObj);
    break;

  case SER_RX_FRZ:
    // when TX is busy, we skip checking for rx buffer
    break;
  
  default:
    // default case considers that rxStatus is IDLE
    //delay(3000);
    if(Serial.available()) {
      serialObj->rxStatus = SER_RX_RQST;
      serialObj->txStatus = SER_TX_FRZ;
    }
    break;
  }
}

void serialTxCheck(serial_obj_t *serialObj) {

  switch (serialObj->txStatus) {

  case SER_TX_RQST:
    // Adding the 'line feed' (\n) character and incrementing nbr of bytes in buffer
    if(serialObj->bytesInBuf) {
      serialObj->buffer[serialObj->bytesInBuf] = lineFeed;
      serialObj->bytesInBuf += 1;
      serialObj->txStatus = SER_TX_RDY;
    }
    break;
  
  case SER_TX_RDY:
    // Send buffer content on serial port    
    (*serialObj->serialPort).write(serialObj->buffer, serialObj->bytesInBuf);
    serialObj->txStatus = SER_TX_CMPLT;
    break;

  case SER_TX_CMPLT:
    // Unfreeze RX
    serialObj->rxStatus = SER_RX_IDLE;
    // clearing all TX attributes
    clearBuffer(serialObj);            // important to clear buffer first
    serialObj->txStatus = SER_TX_IDLE;
    break;

  case SER_TX_FRZ:
    // If RX is busy, we don't check request for transmitting
    break;
  
  default:
    // default TBD
    break;
  }
}

// Function used to fill the serial Rx buffer array. The idea is to separate each digits composing a number into individual bytes
// all organized following the "little endian" order (units come first, then tens, then hundreds)
uint8_t fillBuffer(serial_obj_t *serialObj, byte *infoStartAddr, uint8_t infoBlockSize) {
  
  // The following array is a rough representation of units[1], tens[2] & hundreds[3] of the base ten for digit splitting
  uint8_t baseTenArray[3] = {0, 0, 0};
  uint8_t serialBufLen = 0;
  int8_t arrNonZeroLen = 1;

  for(uint8_t i = 0; i < infoBlockSize; i += BYTE_SIZE) {
    if(*(infoStartAddr + i) >= 10) {
      digitSeparator(*(infoStartAddr + i), &baseTenArray[1], &baseTenArray[0]);
      arrNonZeroLen++;
      if(baseTenArray[1] >= 10) {
        digitSeparator(baseTenArray[1], &baseTenArray[2], &baseTenArray[1]);
        arrNonZeroLen++;
      }
    }
    else {
      baseTenArray[0] = *(infoStartAddr + i);
    }
    for(uint8_t i = 0; i < arrNonZeroLen; i++) {
      serialObj->buffer[serialBufLen] = baseTenArray[i];
      serialBufLen++;
    }

    // Adding the space char to the buffer
    serialObj->buffer[serialBufLen] = spaceChar;
    serialBufLen++;
    arrNonZeroLen = 1;
  }
  return(serialBufLen);
}

void replyToRqst(serial_obj_t *serialObj, byte* ptrToInfo, uint8_t infoSize) {
  clearBuffer(serialObj);
  serialObj->bytesInBuf = fillBuffer(serialObj, (byte*) ptrToInfo, infoSize);
      
  serialObj->txStatus = SER_TX_RQST;
  serialObj->rxStatus = SER_RX_FRZ;
}

// Basically, keeping the same logic as the messageParsing function in the python code
void messageParsing(serial_obj_t *serialObj) {
   
  uint8_t unitsTracker = 0;
  uint8_t extractedNbr = 0;

  uint8_t length = 0;
  
  for(uint8_t i = 0; i < serialObj->bytesInBuf; i++) {
    if(serialObj->buffer[i] == spaceChar) {
      unitsTracker = 0;
      serialObj->parsedMssg[i] = extractedNbr;
      
      length++;
    }
    else if(serialObj->buffer[i] == lineFeed) {
      break;
    }
    else {
      extractedNbr += serialObj->buffer[i] * pow(10, unitsTracker);
      unitsTracker++;
    }
  }
}

void clearBuffer(serial_obj_t *serialObj) {
  for(size_t i = 0; i < serialObj->bytesInBuf; i++) {
    serialObj->buffer[i] = 0;
  }
  serialObj->bytesInBuf = 0;
}

void rqstHandler(serial_obj_t *serialObj) {

  //Serial.print(serialObj->buffer[0]);
  switch (convertAsciiToHex(serialObj->buffer[0])) {

  case SER_RQST_SER_NUM:
    {
      replyToRqst(serialObj, (byte*) getBoardInfosPtrs().serialNumPtr, sizeof(*(getBoardInfosPtrs().serialNumPtr)));
      break;
    }
  
  case SER_RQST_FW_VERS:
    {
      replyToRqst(serialObj, (byte*) getBoardInfosPtrs().fw_versionPtr, sizeof(*(getBoardInfosPtrs().fw_versionPtr)));
      break;
    }
  
  case SER_RQST_SCTS_MGMT:
    {
      replyToRqst(serialObj, (byte*) getBoardInfosPtrs().sectionsInfoPtr, sizeof(*(getBoardInfosPtrs().sectionsInfoPtr)));
      break;
    }

  case SER_RQST_PXLS_MGMT:
    {
      replyToRqst(serialObj, (byte*) getBoardInfosPtrs().pixelsInfoPtr, sizeof(*(getBoardInfosPtrs().pixelsInfoPtr)));
      break;
    }

  case SER_RQST_SETUP_SCT:
    {
      byte ledCount;
      for(uint8_t i = 1; i < serialObj->bytesInBuf - 1; i += 3) {
        ledCount = convertAsciiToHex(serialObj->buffer[i]) + 
          tenTimesByteMultiplier(convertAsciiToHex(serialObj->buffer[i + 1])) + 
          hundredTimesByteMultiplier(convertAsciiToHex(serialObj->buffer[i + 2]));
      }
      createSection(ledCount);

      replyToRqst(serialObj, (byte*) &acknowledge, sizeof(acknowledge));
      break;
    }
  }
}





// // This func reads the first bytes of a serial buffer and saves the action to undertake
// void serialRqstHandler(serial_obj_t *serialObj) {
//   Serial.println(convertAsciiToHex(serialObj->rxByteBuf[0]), BIN);
//   serialObj->status = (serial_status_t)convertAsciiToHex(serialObj->rxByteBuf[0]);
// }

// void serialColorRx(serial_obj_t *serialObj) {
//   // All things related to color could be in another function
//   // keeping only lines 22-25 would make sense in the context of the func's name
//   int8_t rgbwColor[4] = {0, 0, 0, 0};

//   for(uint8_t i = 0; i < serialObj->bytesInBuf; i += 2) {
//     rgbwColor[i >> 1] = ((uint8_t) (convertAsciiToHex(serialObj->buffer[i])) << 4) | 
//                         ((uint8_t)  convertAsciiToHex(serialObj->buffer[i + 1]));
//    }

//   // uint32_t colorToSend = (((uint32_t) rgbwColor[0]) << 24) |
//   //                        (((uint32_t) rgbwColor[1]) << 16) |
//   //                        (((uint32_t) rgbwColor[2]) << 8 ) |
//   //                         ((uint32_t) rgbwColor[3]       );
    
//   // Serial.println(colorToSend);

//   // Pixel and sections ar hardcoded for debugging
//   //pxlColorOut(0, 0, colorToSend);
//   //Serial.println(stripsArrayOfPxl[0][0].rgbwColor);
// }