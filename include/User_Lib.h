/*
header file for any functions to be used throughout the code
*/

#ifndef USER_LIB_H_
#define USER_LIB_H_

#include "Arduino.h"

static const PROGMEM uint8_t _divByTenLookUp[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
  19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  25, 25, 25, 25, 25
};

void homeMadeDelay(uint16_t msDelay);
void serialReadToArray(HardwareSerial &serialToRead, char *recvMess, uint8_t messLen);
void mcuHeartBeat(uint16_t heartBeat);

byte convertAsciiToHex(char ascii);
uint32_t absVar(int32_t val);
uint8_t tenTimesByteMultiplier(byte initialVal);
uint8_t hundredTimesByteMultiplier(byte initialVal);
void digitSeparator(uint8_t number, uint8_t *divContainer, uint8_t *modContainer);

#endif