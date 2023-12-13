/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib source file
*/

#include "Serial_lib.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//**********    LOCAL VARIABLES DECLARATION   ************//

const char ack         = 0x06;
const char nak         = 0x15;
const char lineFeed    = '\n';
const char spaceChar   = ' ';

//**********    LOCAL VARIABLES DECLARATION   ************//




//**********    LOCAL FUNCTIONS DECLARATION   ************//

void serialRqstHandler(serial_obj_t *serialObj);

// uint8_t rxDataParsing(serial_obj_t *serialObj, ser_buffer_t *ser, rqst_action_t *receivedRqst);
// uint8_t txDataEncoding(byte buffer[], byte *infoStartAddr, uint8_t infoBlockSize);
uint8_t rxDataParsing(ser_buffer_t *ser, rqst_action_t *receivedRqst);
void txDataEncoding(ser_buffer_t *ser, byte *infoStartAddr, uint8_t infoBlockSize);

// void clrBuffData(ser_buffer_t *ser, uint8_t startIndex = 0, uint8_t stopIndex = BUFFER_LEN);

void serialWrite(serial_obj_t *serialObj, byte dataToWrite[], uint8_t nbrOfBytes);

//**********    LOCAL FUNCTIONS DECLARATION   ************//


/// @brief 
/// @param serialObj 
void serialRxCheck(serial_obj_t *serialObj) {

  switch (serialObj->rxStatus) {
  
  case SER_RX_RQST:
    while ((*serialObj->serialPort).available()) {
      serialObj->RX.buffer[serialObj->RX.mssgLen] = (*serialObj->serialPort).read();
      if (serialObj->RX.buffer[serialObj->RX.mssgLen] != lineFeed) {
        serialObj->RX.mssgLen++;
      }
      else {
        serialObj->rxStatus = SER_RX_CMPLT;
      }
    }
    break;

  case SER_RX_CMPLT:
    serialObj->txStatus = SER_TX_IDLE;
    serialObj->RX.mssgLen = rxDataParsing(&serialObj->RX, &serialObj->pendingRqst);
    serialRqstHandler(serialObj);
    serialObj->rxStatus = SER_RX_IDLE;
    break;

  case SER_RX_FRZ:
    // when TX is busy, we skip checking for rx buffer
    break;
  
  default:
    // default case considers that rxStatus is IDLE
    if ((*serialObj->serialPort).available()) {
      serialObj->RX.mssgLen = 0;
      serialObj->rxStatus = SER_RX_RQST;
      serialObj->txStatus = SER_TX_FRZ;
    }
    break;
  }
}

/// @brief 
/// @param serialObj 
void serialTxCheck(serial_obj_t *serialObj) {

  switch (serialObj->txStatus) {

  case SER_TX_RQST:
    // Adding the 'line feed' (\n) character and incrementing nbr of bytes in buffer
    serialObj->TX.buffer[serialObj->TX.mssgLen] = lineFeed;
    serialObj->TX.mssgLen++;
    (*serialObj->serialPort).write(serialObj->TX.buffer, serialObj->TX.mssgLen);
    serialObj->txStatus = SER_TX_CMPLT;
    break;

  case SER_TX_CMPLT:
    // Unfreeze RX
    serialObj->TX.mssgLen = 0;
    serialObj->rxStatus = SER_RX_IDLE;
    serialObj->txStatus = SER_TX_IDLE;
    break;

  case SER_TX_FRZ:
    // If RX is busy, we don't check request for transmitting
    break;
  
  default:
    if (serialObj->TX.mssgLen) {
      serialObj->txStatus = SER_TX_RQST;
      serialObj->rxStatus = SER_RX_FRZ;
    }
    break;
  }
}


/// @brief Encodes the metadata info on the tx buffer and sends it over serial to the requester
/// @param serialObj 
/// @param ptr2MetaData 
/// @param metaDataBlockSize 
void sendMetaData(serial_obj_t *serialObj, byte *ptr2MetaData, uint8_t metaDataBlockSize) {
  // serialObj->TX.mssgLen = txDataEncoding(serialObj->TX.buffer, ptr2MetaData, metaDataBlockSize);
  txDataEncoding(&serialObj->TX, ptr2MetaData, metaDataBlockSize);
}


/// @brief Sends an "ACK" char over serial to let the PC know the request has been processed
/// @param serialObj Serial Object to send the ACK signal to
void sendAck(serial_obj_t *serialObj) {
  txDataEncoding(&serialObj->TX, (byte*) &ack, sizeof(ack));
}


/// @brief Sends a "NAK" char over serial to let the PC know the request cannot be processed
/// @param serialObj Serial Object to send the NAK signal to
void sendNak(serial_obj_t *serialObj) {
  txDataEncoding(&serialObj->TX, (byte*) &nak, sizeof(nak));
}


/// @brief Handles the necessary action to do after having received a command thru serial
/// @param serialObj 
void serialRqstHandler(serial_obj_t *serialObj) {
  
  switch(serialObj->pendingRqst) {
  case RQST_SER_NUM:
    {
      sendMetaData(serialObj, (byte*) getBrdMgmtMetaDatasPtr().serialNumPtr, sizeof(*(getBrdMgmtMetaDatasPtr().serialNumPtr)));
      break;
    }
  case RQST_FW_VERS:
    {
      sendMetaData(serialObj, (byte*) getBrdMgmtMetaDatasPtr().fwVersionPtr, sizeof(*(getBrdMgmtMetaDatasPtr().fwVersionPtr)));
      break;
    }
  case RQST_SCTS_MGMT:
    {
      sendMetaData(serialObj, (byte*) getBrdMgmtMetaDatasPtr().sctsMgmtMetaDataPtr, sizeof(*(getBrdMgmtMetaDatasPtr().sctsMgmtMetaDataPtr)));
      break;
    }
  case RQST_PXLS_MGMT:
    {
      sendMetaData(serialObj, (byte*) getBrdMgmtMetaDatasPtr().pxlsMgmtMetaDataPtr, sizeof(*(getBrdMgmtMetaDatasPtr().pxlsMgmtMetaDataPtr)));
      break;
    }
  case RQST_SCTS_ARR:
    {
      if (getBrdMgmtMetaDatasPtr().sctsMgmtMetaDataPtr->assigned) {
        sendMetaData(serialObj, (byte*) getSctMetaDatasPtr(), (sizeof(sct_metadata_t) * getBrdMgmtMetaDatasPtr().sctsMgmtMetaDataPtr->assigned));
      }
      else {
        sendNak(serialObj);
      }
      break;
    }
  case RQST_CONFIG_BRD:
    {
      configBrd(serialObj->RX.buffer, serialObj->RX.mssgLen);
      sendAck(serialObj);
      break;
    }
  case RQST_SAVE_CONFIG:
    {
      sctsConfigSave();
      // sendMetaData(serialObj, &(getBrdMgmtMetaDatasPtr().sctsMgmtMetaDataPtr)->assigned, 1);
      sendAck(serialObj);
      break;
    }
  case RQST_ALL_OFF:
    {
      allOff();
      sendAck(serialObj);
      break;
    }
  case RQST_SAVE_RESET:
    {
      eepromReset();
      sendAck(serialObj);
      break;
    }
  case RQST_NONE:
    {
      // Add something later?
      break;
    }
  default:
    {
      break;
    }
  }
  serialObj->pendingRqst = RQST_NONE;
}



// uint8_t rxDataParsing(serial_obj_t *serialObj, ser_buffer_t *ser, rqst_action_t *receivedRqst) 

/// @brief Parses a message received through the serial media.
///        The command is separated from the rest of the data
///        to facilitate further treatment 
/// @param ser Serial buffer type var containing the serial mssg
///            and the mssg length
/// @param receivedRqst Rqst/cmd container to be update from the
///                     mssg to be parsed
/// @return Updated length (in bytes) of the parsed rxed mssg
uint8_t rxDataParsing(ser_buffer_t *ser, rqst_action_t *receivedRqst) {
  bool rqstCheck = false;
  uint8_t unitsTracker = 0;
  uint8_t extractedNbr = 0;
  uint8_t newLen = 0;

  for(uint8_t i = 0; i <= ser->mssgLen; i++) {
    if(ser->buffer[i] != spaceChar && i < ser->mssgLen) {
      extractedNbr += convertAsciiToHex(ser->buffer[i]) * powOfTen(unitsTracker);
      unitsTracker++;
    }
    else {
      if(unitsTracker) {
        if(!rqstCheck) {
          *receivedRqst = (rqst_action_t) extractedNbr;
          rqstCheck = true;
        }
        else {
          ser->buffer[newLen] = extractedNbr;
          newLen++;
        }
        unitsTracker = 0;
        extractedNbr = 0;
      }
    }
  }
  return newLen;
}


/// @brief Encodes the info for a serial transmission to a rxer
///        and fills a buffer with the result. The encoding logic
///        is described in the Serial spreadsheets of the Drive
/// @param ser Serial buffer type var containing the serial mssg
///            and the mssg length
/// @param infoStartAddr First address of the block of info to send
/// @param infoBlockSize Size in bytes of the info block
/// @return Amount of bytes to send
void txDataEncoding(ser_buffer_t *ser, byte *infoStartAddr, uint8_t infoBlockSize) {
  // The following array is a rough representation of units[1], tens[2] & hundreds[3] of the base ten for digit splitting
  uint8_t baseTenArray[3] = {0, 0, 0};
  uint8_t arrNonZeroLen = 1;

  if (ser->mssgLen) {
    ser->buffer[ser->mssgLen] = spaceChar;
    ser->mssgLen++;
  }

  for (uint8_t i = 0; i < infoBlockSize; i += BYTE_SIZE) {
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
    for (uint8_t i = 0; i < arrNonZeroLen; i++) {
      ser->buffer[ser->mssgLen] = baseTenArray[i];
      ser->mssgLen++;
    }
    // Adding the space char to the buffer, except when the info block has been done in entirety
    if (i < infoBlockSize - BYTE_SIZE) {
      ser->buffer[ser->mssgLen] = spaceChar;
      ser->mssgLen++;
      arrNonZeroLen = 1;
    }
  }
}


// Clearing unwanted characters in a serial buffer
// void clrBuffData(ser_buffer_t *ser, uint8_t startIndex, uint8_t stopIndex) {
//   for(uint8_t i = startIndex; i < stopIndex; i++) {
//     ser->buffer[i] = 0;
//   }
// }



//**********    DEBUG FUNCTION   ************//

// Homemade Serial write func that adds the lineFeed char at the end of data to send
void serialWrite(serial_obj_t *serialObj, byte dataToWrite[], uint8_t nbrOfBytes) {
  // byte serialArr[nbrOfBytes + 1];
  // serialArr[nbrOfBytes] = lineFeed;

  // for(uint8_t i = 0; i < (nbrOfBytes); i++) {
  //   serialArr[i] = dataToWrite[i];
  // }
  // serialObj->serialPort->write(serialArr, nbrOfBytes + 1);
  serialObj->serialPort->write(dataToWrite, nbrOfBytes);
}

//**********    DEBUG FUNCTION   ************//







/// @brief Encodes the info for a serial transmission to a rxer
///        and fills a buffer with the result. The encoding logic
///        is described in the Serial spreadsheets of the Drive
/// @param buffer Where the encoded message is stored
/// @param infoStartAddr First address of the block of info to send
/// @param infoBlockSize Size in bytes of the info block
/// @return Amount of bytes to send
// uint8_t txDataEncoding(byte buffer[], byte *infoStartAddr, uint8_t infoBlockSize) {
  
//   // The following array is a rough representation of units[1], tens[2] & hundreds[3] of the base ten for digit splitting
//   uint8_t baseTenArray[3] = {0, 0, 0};
//   uint8_t arrNonZeroLen = 1;
//   uint8_t newLen = 0;

//   for(uint8_t i = 0; i < infoBlockSize; i += BYTE_SIZE) {
//     if(*(infoStartAddr + i) >= 10) {
//       digitSeparator(*(infoStartAddr + i), &baseTenArray[1], &baseTenArray[0]);
//       arrNonZeroLen++;
//       if(baseTenArray[1] >= 10) {
//         digitSeparator(baseTenArray[1], &baseTenArray[2], &baseTenArray[1]);
//         arrNonZeroLen++;
//       }
//     }
//     else {
//       baseTenArray[0] = *(infoStartAddr + i);
//     }
//     for(uint8_t i = 0; i < arrNonZeroLen; i++) {
//       buffer[newLen] = baseTenArray[i];
//       newLen++;
//     }
//     // Adding the space char to the buffer, except when the info block has been done in entirety
//     if(i < infoBlockSize - BYTE_SIZE) {
//       buffer[newLen] = spaceChar;
//       newLen++;
//       arrNonZeroLen = 1;
//     }
//   }
//   return(newLen);
// }