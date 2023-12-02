/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib header file
*/

#ifndef SERIAL_LIB_H_
#define SERIAL_LIB_H_

#include "Arduino.h"
#include "HardwareSerial.h"

#define USER_BUFFER_SIZE    128
// #if SERIAL_RX_BUFFER_SIZE >= SERIAL_TX_BUFFER_SIZE
// #define BUFFER_LEN  SERIAL_RX_BUFFER_SIZE - 1
// #else
// #define BUFFER_LEN  SERIAL_TX_BUFFER_SIZE - 1
// #endif

typedef enum serial_rx {
  SER_RX_IDLE,
  SER_RX_RQST,
  SER_RX_CMPLT,
  SER_RX_FRZ
} serial_rx_state;

typedef enum serial_tx {
  SER_TX_IDLE,
  SER_TX_RQST,
  SER_TX_CMPLT,
  SER_TX_FRZ
} serial_tx_state;

typedef enum rqst_action {
  RQST_NONE,
  RQST_SER_NUM,
  RQST_FW_VERS,
  RQST_SCTS_MGMT,
  RQST_PXLS_MGMT,
  RQST_SCTS_ARR,
  RQST_CONFIG_BRD = 10,
  RQST_SAVE_CONFIG = 20,
  RQST_ALL_OFF = 254,
  RQST_SAVE_RESET = 255
} rqst_action_t;

typedef struct serial_buffer{
  byte buffer[USER_BUFFER_SIZE];
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