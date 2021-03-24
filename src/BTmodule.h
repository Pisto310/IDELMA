/*
header file for declaring the function used in setting up the HC-06 BT module
*/
#include "Arduino.h"

// Not useful to define higher baud rates since 
// 115200 is highest a PC can support
#define BAUD1   1200
#define BAUD2   2400
#define BAUD3   4800
#define BAUD4   9600        // HC-06 default baud rate
#define BAUD5   19200
#define BAUD6   38400
#define BAUD7   57600
#define BAUD8   115200      

// pre-processing all received strings from module
#define AT_REPLY            "OK"
#define AT_NAME_REPLY       "OKsetname"
#define AT_PIN_REPLY        "OKsetPIN"
#define AT_BAUD1_REPLY      "OK1200"
#define AT_BAUD2_REPLY      "OK2400"
#define AT_BAUD3_REPLY      "OK4800"
#define AT_BAUD4_REPLY      "OK9600"
#define AT_BAUD5_REPLY      "OK19200"
#define AT_BAUD6_REPLY      "OK38400"
#define AT_BAUD7_REPLY      "OK57600"
#define AT_BAUD8_REPLY      "OK115200"

// pre-processing all send strings to module
#define AT_SEND             "AT"
#define AT_NAME_SET         "AT+NAME"
#define AT_PIN_SET          "AT+PIN"
#define AT_BAUD1_SET        "AT+BAUD1"
#define AT_BAUD2_SET        "AT+BAUD2"
#define AT_BAUD3_SET        "AT+BAUD3"
#define AT_BAUD4_SET        "AT+BAUD4"
#define AT_BAUD5_SET        "AT+BAUD5"
#define AT_BAUD6_SET        "AT+BAUD6"
#define AT_BAUD7_SET        "AT+BAUD7"
#define AT_BAUD8_SET        "AT+BAUD8"

uint16_t baudRateFinder(HardwareSerial &serialPort);
void setupBT(HardwareSerial &serialPort, uint16_t *defaultBaud, uint16_t setBaud = BAUD4, char moduleName[7] = "IDELMA", char modulePin[5] = "3003");

/*!
  @brief   function that periodically test the BT module
           communication with MCU
  @param   serialPort  an address to a HardwareSerial object
  @return  None
  @note    To be called every time the main loop run
*/
void commCheckBT(HardwareSerial &serialPort);