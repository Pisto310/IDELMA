/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib source file
*/

#include "Serial_lib.h"
#include "User_Lib.h"
#include "SK6812.h"
#include "Board.h"

//**********    LOCAL VARIABLES DECLARATION   ************//

enum serial_rqst{
  SER_RQST_NONE,
  SER_RQST_SER_NUM,
  SER_RQST_FW_VERS,
  SER_RQST_SCTS_MGMT,
  SER_RQST_PXLS_MGMT
  // SER_RQST_BRD_INFOS,
  // SER_RQST_SCT_INFO_ARR,
  // SER_RQST_SETUP_SCT,
  // SER_RQST_SAVE_SCTS_CONFIG,
  // SER_RQST_LED_COLOR_CHANGE
};

//**********    LOCAL VARIABLES DECLARATION   ************//




//**********    LOCAL FUNCTIONS DECLARATION   ************//

void serialClrBuf(serial_obj_t *serialObj);
void serialRqstHandler(serial_obj_t *serialObj);

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
    serialRqstHandler(serialObj);
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
      serialObj->buffer[serialObj->bytesInBuf] = '\n';
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
    serialClrBuf(serialObj);            // important to clear buffer first
    serialObj->bytesInBuf = 0;
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
uint8_t serialFillBuf(serial_obj_t *serialObj, byte *infoStartAddr, uint8_t infoBlockSize) {
  
  uint8_t spaceChar = 0x20;
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

void serialClrBuf(serial_obj_t *serialObj) {
  for(size_t i = 0; i < serialObj->bytesInBuf; i++) {
    serialObj->buffer[i] = 0;
  }
  serialObj->bytesInBuf = 0;
}

void serialReplyToRqst(serial_obj_t *serialObj, byte* ptrToInfo, uint8_t infoSize) {
  // clearing serial obj buffer (first) & its number of bytes attribute
  serialClrBuf(serialObj);

  serialObj->bytesInBuf = serialFillBuf(serialObj, (byte*) ptrToInfo, infoSize);
      
  serialObj->txStatus = SER_TX_RQST;
  serialObj->rxStatus = SER_RX_FRZ;
}

void serialRqstHandler(serial_obj_t *serialObj) {

  // Serial.print(serialObj->buffer[0]);

  switch (convertAsciiToHex(serialObj->buffer[0])) {

  case SER_RQST_SER_NUM:
    {
      serialReplyToRqst(serialObj, (byte*) getBoardInfosPtrs().serialNumPtr, sizeof(*(getBoardInfosPtrs().serialNumPtr)));
      break;
    }
  
  case SER_RQST_FW_VERS:
    {
      serialReplyToRqst(serialObj, (byte*) getBoardInfosPtrs().fw_versionPtr, sizeof(*(getBoardInfosPtrs().fw_versionPtr)));
      break;
    }
  
  case SER_RQST_SCTS_MGMT:
    {
      serialReplyToRqst(serialObj, (byte*) getBoardInfosPtrs().sectionsInfoPtr, sizeof(*(getBoardInfosPtrs().sectionsInfoPtr)));
      break;
    }

  case SER_RQST_PXLS_MGMT:
    {
      serialReplyToRqst(serialObj, (byte*) getBoardInfosPtrs().pixelsInfoPtr, sizeof(*(getBoardInfosPtrs().pixelsInfoPtr)));
      break;
    }
  // case SER_RQST_BRD_INFOS:
  //   {
  //     // clearing serial obj buffer (first) & its number of bytes attribute
  //     serialClrBuf(serialObj);
  //     serialObj->bytesInBuf = 0;          // Redundant since right after we fill it?
        
  //     // Get board infos and put them into the buffer if need to TX included in Rqst
  //     serialObj->bytesInBuf = serialFillBuf(serialObj, (byte*) getPtrBoardInfos(), sizeof(board_infos_t), 1);

  //     // Be sure to change state of TX Status to something other than frozen at the end
  //     // Freeze RX status also tho!
  //     serialObj->txStatus = SER_TX_RQST;
  //     serialObj->rxStatus = SER_RX_FRZ;
  //     break;
  //   }

  // case SER_RQST_SCT_INFO_ARR:
  //   {
  //     // clearing serial obj buffer (first) & its number of bytes attribute
  //     serialClrBuf(serialObj);
  //     serialObj->bytesInBuf = 0;

  //     // Putting the content of sectionInfoArr into the serial buffer
  //     serialObj->bytesInBuf = sectionInfoArrSerial(serialObj->buffer);

  //     serialObj->txStatus = SER_TX_RQST;
  //     serialObj->rxStatus = SER_RX_FRZ;
  //     break;
  //   }

  // case SER_RQST_SETUP_SCT:
  //   {
  //     // Do everything we need to do with content of buffer before erasing it
  //     // First, we need to count how many LEDs are required in the section
  //     uint8_t ledCount = 0;
  //     if(serialObj->bytesInBuf > 2) {
  //       ledCount = tenTimesByteMultiplier(convertAsciiToHex(serialObj->buffer[1]));
  //       ledCount += convertAsciiToHex(serialObj->buffer[2]);
  //     }
  //     else {
  //       ledCount = convertAsciiToHex(serialObj->buffer[1]);
  //     }
  //     // creating new section with required number of LEDs then clearing buffer
  //     createSection(ledCount);
  //     serialClrBuf(serialObj);
  //     serialObj->bytesInBuf = 0;
        
  //     // Send the updated board infos to the PC
  //     serialObj->bytesInBuf = boardInfosBufferFill(serialObj->buffer, 3, 6);

  //     serialObj->txStatus = SER_TX_RQST;
  //     serialObj->rxStatus = SER_RX_FRZ;
  //     break;
  //   }

  // case SER_RQST_SAVE_SCTS_CONFIG:
  //   {
  //     // Set-up config save in EEPROM
  //     setupSaveToEeprom();

  //     // clearing serial obj buffer (first) & its number of bytes attribute
  //     serialClrBuf(serialObj);
  //     serialObj->bytesInBuf = 0;          // Redundant since right after we fill it?
        
  //     // Send a one (1) to indicate the completion of the write operation
  //     // Filling buffer and bytesInBuf attr.
  //     serialObj->buffer[0] = 1;
  //     serialObj->bytesInBuf = 1;

  //     // Be sure to change state of TX Status to something other than frozen at the end
  //     // Freeze RX status also tho!
  //     serialObj->txStatus = SER_TX_RQST;
  //     serialObj->rxStatus = SER_RX_FRZ;
  //     break;
  //   }
  
  // case SER_RQST_LED_COLOR_CHANGE:
  //   {
  //     // Message bytes are organized as follows:
  //     // B1:        section number
  //     // B2:        pixel number
  //     // B3 - B10:  LED color to cast
  //     if(serialObj->buffer[1] == 0x20 || serialObj->buffer[2] == 0x20) {
  //       // We do nothing since we are missing the pixel or section info
  //       break;
  //     }
  //     uint8_t sct = convertAsciiToHex(serialObj->buffer[1]);
  //     uint8_t pxl = convertAsciiToHex(serialObj->buffer[2]);

  //     uint8_t red_1 = convertAsciiToHex(serialObj->buffer[3]);
  //     uint8_t red_0 = convertAsciiToHex(serialObj->buffer[4]);

  //     uint8_t grn_1 = convertAsciiToHex(serialObj->buffer[5]);
  //     uint8_t grn_0 = convertAsciiToHex(serialObj->buffer[6]);

  //     uint8_t blu_1 = convertAsciiToHex(serialObj->buffer[7]);
  //     uint8_t blu_0 = convertAsciiToHex(serialObj->buffer[8]);

  //     uint8_t wht_1 = convertAsciiToHex(serialObj->buffer[9]);
  //     uint8_t wht_0 = convertAsciiToHex(serialObj->buffer[10]);

  //     uint8_t red = (red_1 << 4 | red_0);
  //     uint8_t grn = (grn_1 << 4 | grn_0);
  //     uint8_t blu = (blu_1 << 4 | blu_0);
  //     uint8_t wht = (wht_1 << 4 | wht_0);

  //     uint32_t castColor = ((uint32_t) red) << 24 | ((uint32_t) grn) << 16 | ((uint32_t) blu) << 8 | ((uint32_t) wht);

  //     pxlColorOut(sct, pxl, castColor);

  //     serialObj->rxStatus = SER_RX_IDLE;
  //     break;
  //   }
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