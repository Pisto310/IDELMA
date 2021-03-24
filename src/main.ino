//Arduino.h is included in the header of the NeoPixel lib

#include "BTmodule.h"
#include "User_Lib.h"
#include "SK6812.h"

//HC-06 module address 00-14-03-05-5A-D5

/*!
  @brief   Brief of func
  @param   var  explanation
  @return  What the function returns
  @note   Write if necessary
*/

char play[5] = "play";
char stop[5] = "stop";

char btBuffer[64];

bool btFlag = 1;

bool sparkleTrig = 0;

uint16_t heartBeat = 1000;                  // in milliseconds

uint8_t brightnessLED = 50;

uint8_t sctCntTracker = 0;                  // var that keeps track of the index in the strips and nexpxlObj arrays
uint8_t *ptrSctCntTracker = &sctCntTracker;

void setup() {

  // initializing the built-in LED for HeartBeat
  pinMode(LED_BUILTIN, OUTPUT);

  // setting up the serial port for debugging on serial monitor
  Serial.begin(9600);
  
  
  
  //*************   BT MODULE SET-UP   **************//
  
  /*
  // finding the BT module's baud rate
  uint16_t tempBaud = baudRateFinder(Serial1);
  uint16_t *defaultBaud = &tempBaud;

  setupBT(Serial1, defaultBaud, BAUD4);
  Serial.println(tempBaud);
  */

  //*************   BT MODULE SET-UP   **************//




  //*************   STRIP AND LEDS SET-UP   *************//
  
  neopxlObjSetUp(sctZero, neopxlObjArr, ptrSctCntTracker, brightnessLED);
  neopxlObjSetUp(sctSix, neopxlObjArr, ptrSctCntTracker, brightnessLED);

  // decrementing the section count to have the exact number
  --sctCntTracker;

  //*************   STRIP AND LEDS SET-UP   *************//
}

void loop() {
  
  if(btFlag) {
    mcuHeartBeat(heartBeat);
  }

  if(!sparkleTrig) {
    sparkleInit(0);
    sparkleInit(1);
    sparkleTrig = !sparkleTrig;
  }

  pxlIterator(2);

  //sparkle(section1, 0x00, 0x00, 0x00, 0xFF, 25);

  /*
  serialReadToArray(Serial1, btBuffer, 64);

  if(!strncmp(btBuffer, play, 4)) {
    btFlag = 1;
    Serial1.print("Heartbeat is good");
  }

  if(!strncmp(btBuffer, stop, 4)) {
    btFlag = 0;
    Serial1.println("Heartbeat has stopped");
  }

  Serial1.println(btBuffer);
  */
}