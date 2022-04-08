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
  SER_RQST_SEND_INFOS,
  SER_RQST_SETUP_SCT,
  SER_RQST_SAVE_SCTS_CONFIG
};

//**********    LOCAL VARIABLES DECLARATION   ************//




//**********    LOCAL FUNCTIONS DECLARATION   ************//

void serialClrBuf(serial_obj_t *serialObj);
void serialRqstHandler(serial_obj_t *serialObj);
void serialBuildMessage(serial_obj_t *serialObj, byte messageBytes[]);

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
    if(serialObj->bytesInBuf) {
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

void serialClrBuf(serial_obj_t *serialObj) {
  for(size_t i = 0; i < serialObj->bytesInBuf; i++) {
    serialObj->buffer[i] = 0;
  }
}

void serialRqstHandler(serial_obj_t *serialObj) {

  //Serial.print(serialObj->buffer[0]);

  switch (convertAsciiToHex(serialObj->buffer[0])) {
  
  case SER_RQST_SEND_INFOS:
    {
      // clearing serial obj buffer (first) & its number of bytes attribute
      serialClrBuf(serialObj);
      serialObj->bytesInBuf = 0;          // Redundant since right after we fill it?
      
      // Get board infos and put them into the buffer if need to TX included in Rqst
      serialObj->bytesInBuf = boardInfosBufferFill(serialObj->buffer);
      
      // Be sure to change state of TX Status to something other than frozen at the end
      // Freeze RX status also tho!
      serialObj->txStatus = SER_TX_RQST;
      serialObj->rxStatus = SER_RX_FRZ;
      break;
    }

  case SER_RQST_SETUP_SCT:
    {
      // Do everything we need to do with content of buffer before erasing it
      // First, we need to count how many LEDs are required in the section
      uint8_t ledCount = 0;
      if(serialObj->bytesInBuf > 2) {
        ledCount = tenTimesByteMultiplier(convertAsciiToHex(serialObj->buffer[1]));
        ledCount += convertAsciiToHex(serialObj->buffer[2]);
      }
      else {
        ledCount = convertAsciiToHex(serialObj->buffer[1]);
      }
      // creating new section with required number of LEDs then clearing buffer
      createSection(ledCount);
      serialClrBuf(serialObj);
      serialObj->bytesInBuf = 0;
      
      // Send the updated board infos to the PC
      serialObj->bytesInBuf = boardInfosBufferFill(serialObj->buffer, 3, 6);

      serialObj->txStatus = SER_TX_RQST;
      serialObj->rxStatus = SER_RX_FRZ;
      break;
    }

  case SER_RQST_SAVE_SCTS_CONFIG:
    {
      // Set-up config save in EEPROM
      saveSctsConfig();

      // clearing serial obj buffer (first) & its number of bytes attribute
      serialClrBuf(serialObj);
      serialObj->bytesInBuf = 0;          // Redundant since right after we fill it?
      
      // Send a one (1) to indicate the completion of the write operation
      // Filling buffer and bytesInBuf attr.
      serialObj->buffer[0] = 1;
      serialObj->bytesInBuf = 1;

      // Be sure to change state of TX Status to something other than frozen at the end
      // Freeze RX status also tho!
      serialObj->txStatus = SER_TX_RQST;
      serialObj->rxStatus = SER_RX_FRZ;
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