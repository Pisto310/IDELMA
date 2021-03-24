/*
header file for any functions to be used throughout the code
*/

#include "Arduino.h"

void homeMadeDelay(uint16_t msDelay);
void serialReadToArray(HardwareSerial &serialToRead, char *recvMess, uint8_t messLen);
void mcuHeartBeat(uint16_t heartBeat);