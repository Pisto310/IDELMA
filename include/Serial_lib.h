/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib header file
*/

#ifndef SERIAL_LIB_H_
#define SERIAL_LIB_H_

#include "Arduino.h"
#include "HardwareSerial.h"

#if SERIAL_RX_BUFFER_SIZE >= SERIAL_TX_BUFFER_SIZE
#define BUFFER_LEN  SERIAL_RX_BUFFER_SIZE
#else
#define BUFFER_LEN  SERIAL_TX_BUFFER_SIZE
#endif

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

typedef enum serial_rx {
  SER_RX_IDLE,
  SER_RX_RQST,
  SER_RX_CMPLT,
  SER_RX_FRZ,
  SER_RX_DEADEND
} serial_rx_state;

typedef enum serial_tx {
  SER_TX_IDLE,
  SER_TX_RQST,
  SER_TX_RDY,
  SER_TX_CMPLT,
  SER_TX_FRZ
} serial_tx_state;

typedef enum rqst_action {
  RQST_NONE,
  RQST_SER_NUM,
  RQST_FW_VERS,
  RQST_SCTS_MGMT,
  RQST_PXLS_MGMT,
  RQST_CONFIG_BRD = 10,
} rqst_action_t;

typedef struct serial_buffer{
  byte buffer[BUFFER_LEN];
  uint8_t mssgLen;
} ser_buffer_t;

typedef struct serial_obj{
  HardwareSerial *serialPort;
  ser_buffer_t RX;
  ser_buffer_t TX;
  serial_rx_state rxStatus = SER_RX_IDLE;
  serial_tx_state txStatus = SER_TX_IDLE;
  rqst_action_t pendingRqst = RQST_NONE;
}serial_obj_t;

void serialRxCheck(serial_obj_t *serialObj);
void serialTxCheck(serial_obj_t *serialObj);

void serialColorRx(serial_obj_t *serialObj);

#endif    /* SERIAL_LIB_H_ */  