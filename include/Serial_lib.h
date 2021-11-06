/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib header file
*/

#include "Arduino.h"

typedef struct {
  HardwareSerial *serialPort;
  byte rxByteBuf[64];
  uint8_t xtractedBytes;
}serial_obj_t;

void serialRxRead(serial_obj_t *serialObj);
void serialRxInterpreter(serial_obj_t *serialObj);

void serialColorRx(serial_obj_t *serialObj);