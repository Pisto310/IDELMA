/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib source file
*/

#include "Serial_lib.h"
#include "User_Lib.h"
#include "Board.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//**********    LOCAL VARIABLES DECLARATION   ************//

const char acknowledge = 0x06;
const char lineFeed    = '\n';
const char spaceChar   = ' ';

//**********    LOCAL VARIABLES DECLARATION   ************//




//**********    LOCAL FUNCTIONS DECLARATION   ************//

void serialRqstHandler(serial_obj_t *serialObj);

uint8_t rxDataParsing(ser_buffer_t *ser, rqst_action_t *receivedRqst);
uint8_t txDataEncoding(byte buffer[], byte *infoStartAddr, uint8_t infoBlockSize);

void clrBuffData(ser_buffer_t *ser, uint8_t startIndex = 0, uint8_t stopIndex = BUFFER_LEN);

void serialWrite(serial_obj_t *serialObj, byte dataToWrite[], uint8_t nbrOfBytes);

//**********    LOCAL FUNCTIONS DECLARATION   ************//


/// @brief 
/// @param serialObj 
void serialRxCheck(serial_obj_t *serialObj) {

  switch (serialObj->rxStatus) {
  
  case SER_RX_RQST:
    serialObj->RX.mssgLen = (*serialObj->serialPort).readBytes(serialObj->RX.buffer, BUFFER_LEN);
    if(serialObj->RX.mssgLen) {
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

    serialObj->txStatus = SER_TX_IDLE;
    serialObj->RX.mssgLen = rxDataParsing(&serialObj->RX, &serialObj->pendingRqst);
    // clrBuffData(&serialObj->RX, serialObj->RX.bufferLen); 
    // serialWrite(serialObj, serialObj->RX.buffer, serialObj->RX.bufferLen);
    serialObj->rxStatus = SER_RX_IDLE;
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

/// @brief 
/// @param serialObj 
void serialTxCheck(serial_obj_t *serialObj) {

  switch (serialObj->txStatus) {

  case SER_TX_RQST:
    // Adding the 'line feed' (\n) character and incrementing nbr of bytes in buffer
    if(serialObj->TX.mssgLen) {
      serialObj->TX.buffer[serialObj->TX.mssgLen] = lineFeed;
      serialObj->TX.mssgLen += 1;
      serialObj->txStatus = SER_TX_RDY;
    }
    break;
  
  case SER_TX_RDY:
    // Send buffer content on serial port    
    (*serialObj->serialPort).write(serialObj->TX.buffer, serialObj->TX.mssgLen);
    serialObj->txStatus = SER_TX_CMPLT;
    break;

  case SER_TX_CMPLT:
    // Unfreeze RX
    serialObj->rxStatus = SER_RX_IDLE;
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

/// @brief Encodes the metadata info on the tx buffer and sends it over serial to the requester
/// @param ser serial buffer type variable
/// @param ptr2MetaData pointer to the metadata info to send
/// @param rxNewStatus pointer to the RX serial status of a serial object type struct
/// @param txNewStatus pointer to the TX serial status of a serial object type struct
void sendMetaData(ser_buffer_t *ser, byte *ptr2MetaData, uint8_t metaDataBlockSize, serial_rx_state *rxNewStatus, serial_tx_state *txNewStatus) {
  ser->mssgLen = txDataEncoding(ser->buffer, ptr2MetaData, metaDataBlockSize);
  *txNewStatus = SER_TX_RQST;
  *rxNewStatus = SER_RX_FRZ;
}

// Handler for all requests coming in from serial port
void serialRqstHandler(serial_obj_t *serialObj) {
  
  switch(serialObj->pendingRqst) {
  case RQST_SER_NUM:
    {
      sendMetaData(&serialObj->TX, (byte*) getBoardInfosPtrs().serialNumPtr, sizeof(*(getBoardInfosPtrs().serialNumPtr)), &serialObj->rxStatus, &serialObj->txStatus);
      break;
    }
  case RQST_FW_VERS:
    {
      sendMetaData(&serialObj->TX, (byte*) getBoardInfosPtrs().fwVersionPtr, sizeof(*(getBoardInfosPtrs().fwVersionPtr)), &serialObj->rxStatus, &serialObj->txStatus);
      break;
    }
  case RQST_SCTS_MGMT:
    {
      sendMetaData(&serialObj->TX, (byte*) getBoardInfosPtrs().sectionsInfoPtr, sizeof(*(getBoardInfosPtrs().sectionsInfoPtr)), &serialObj->rxStatus, &serialObj->txStatus);
      break;
    }
  case RQST_PXLS_MGMT:
    {
      sendMetaData(&serialObj->TX, (byte*) getBoardInfosPtrs().pixelsInfoPtr, sizeof(*(getBoardInfosPtrs().pixelsInfoPtr)), &serialObj->rxStatus, &serialObj->txStatus);
      break;
    }
  case RQST_CONFIG_BRD:
    {
      uint8_t coreDataLen = 2;
      for (uint8_t i = 0; i < serialObj->RX.mssgLen; i += coreDataLen) {
        uint8_t sectionIndex = serialObj->RX.buffer[i];
        uint8_t pixelCount = serialObj->RX.buffer[i + 1];
        configSct(sectionIndex, pixelCount);
      }
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


// Basically, keeping the same logic as the messageParsing function in the python code
uint8_t rxDataParsing(ser_buffer_t *ser, rqst_action_t *receivedRqst) {
   
  bool rqstCheck = false;
  uint8_t unitsTracker = 0;
  uint8_t extractedNbr = 0;
  uint8_t newLen = 0;
  
  for(uint8_t i = 0; i < ser->mssgLen; i++) {
    if(ser->buffer[i] != lineFeed && ser->buffer[i] != spaceChar) {
      extractedNbr += convertAsciiToHex(ser->buffer[i]) * pow(10, unitsTracker);
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
      if(ser->buffer[i] == lineFeed) {
        break;
      }
    }
  }
  return(newLen);
}
 

// Function used to fill the serial Rx buffer array. The idea is to separate each digits composing a number into individual bytes
// all organized following the "little endian" order (units come first, then tens, then hundreds)
uint8_t txDataEncoding(byte buffer[], byte *infoStartAddr, uint8_t infoBlockSize) {
  
  // The following array is a rough representation of units[1], tens[2] & hundreds[3] of the base ten for digit splitting
  uint8_t baseTenArray[3] = {0, 0, 0};
  uint8_t arrNonZeroLen = 1;
  uint8_t newLen = 0;

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
      buffer[newLen] = baseTenArray[i];
      newLen++;
    }
    // Adding the space char to the buffer, except when the info block has been done in entirety
    if(i < infoBlockSize - BYTE_SIZE) {
      buffer[newLen] = spaceChar;
      newLen++;
      arrNonZeroLen = 1;
    }
  }
  return(newLen);
}


// Clearing unwanted characters in a serial buffer
void clrBuffData(ser_buffer_t *ser, uint8_t startIndex, uint8_t stopIndex) {
  for(uint8_t i = startIndex; i < stopIndex; i++) {
    ser->buffer[i] = 0;
  }
}



//**********    DEBUG FUNCTION   ************//

// Homemade Serial write func that adds the lineFeed char at the end of data to send
void serialWrite(serial_obj_t *serialObj, byte dataToWrite[], uint8_t nbrOfBytes) {
  
  byte serialArr[nbrOfBytes + 1];
  serialArr[nbrOfBytes] = lineFeed;

  for(uint8_t i = 0; i < (nbrOfBytes); i++) {
    serialArr[i] = dataToWrite[i];
  }
  serialObj->serialPort->write(serialArr, nbrOfBytes + 1);
}

//**********    DEBUG FUNCTION   ************//


// void serialNum(ser_buffer_t *ser, serial_rx_state *rxNewStatus, serial_tx_state *txNewStatus) {
//   ser->mssgLen = txDataEncoding(ser->buffer, (byte*) getBoardInfosPtrs().serialNumPtr, sizeof(*(getBoardInfosPtrs().serialNumPtr)));
//   *txNewStatus = SER_TX_RQST;
//   *rxNewStatus = SER_RX_FRZ;
// }


// void fwVersion(ser_buffer_t *ser, serial_rx_state *rxNewStatus, serial_tx_state *txNewStatus) {
//   ser->mssgLen = txDataEncoding(ser->buffer, (byte*) getBoardInfosPtrs().fwVersionPtr, sizeof(*(getBoardInfosPtrs().fwVersionPtr)));
//   *txNewStatus = SER_TX_RQST;
//   *rxNewStatus = SER_RX_FRZ;
// }


// void sctsManagement(ser_buffer_t *ser, serial_rx_state *rxNewStatus, serial_tx_state *txNewStatus) {
//   ser->mssgLen = txDataEncoding(ser->buffer, (byte*) getBoardInfosPtrs().sectionsInfoPtr, sizeof(*(getBoardInfosPtrs().sectionsInfoPtr)));
//   *txNewStatus = SER_TX_RQST;
//   *rxNewStatus = SER_RX_FRZ;
// }


// void pxlsManagement(ser_buffer_t *ser, serial_rx_state *rxNewStatus, serial_tx_state *txNewStatus) {
//   ser->mssgLen = txDataEncoding(ser->buffer, (byte*) getBoardInfosPtrs().pixelsInfoPtr, sizeof(*(getBoardInfosPtrs().pixelsInfoPtr)));
//   *txNewStatus = SER_TX_RQST;
//   *rxNewStatus = SER_RX_FRZ;
// }