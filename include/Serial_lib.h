/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib header file
*/

#ifndef SERIAL_LIB_H_
#define SERIAL_LIB_H_

#include "Arduino.h"

// typedef enum {
//   SER_IDLE,
//   SER_AVAILABLE,
//   SER_RCVED,
//   SER_DECODE,
//   SER_SEND_BOARD_INFOS,
//   SER_SAVE_SETTINGS,
//   SER_SET_COLOR,
//   RESERVED_6,
//   RESERVED_7
// }serial_status_t;

typedef enum {
  SER_RX_IDLE,
  SER_RX_RQST,
  SER_RX_CMPLT,
  SER_RX_FRZ,
  SER_RX_DEADEND
}serial_rx_t;

typedef enum {
  SER_TX_IDLE,
  SER_TX_RQST,
  SER_TX_RDY,
  SER_TX_CMPLT,
  SER_TX_FRZ
}serial_tx_t;

typedef struct {
  HardwareSerial *serialPort;
  bool txBoardInfos = 0;
  bool txSetupInfos = 0;
  byte buffer[64];
  uint8_t bytesInBuf;
  serial_rx_t rxStatus = SER_RX_IDLE;
  serial_tx_t txStatus = SER_TX_IDLE;
}serial_obj_t;

void serialRxCheck(serial_obj_t *serialObj);
void serialTxCheck(serial_obj_t *serialObj);

void serialColorRx(serial_obj_t *serialObj);

#endif    /* SERIAL_LIB_H_ */  