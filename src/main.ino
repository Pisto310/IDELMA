//Arduino.h is included in the header of the NeoPixel lib

//#include "BTmodule.h"
#include "User_Lib.h"
#include "SK6812.h"
#include "Serial_lib.h"
#include "Dump.h"

//HC-06 module address 00-14-03-05-5A-D5





//************    TEST VAR DECLARATIOIN     **************

// uint16_t ramStart = 0x0200;
// int *ptrLeFun = (int*)malloc(2);

// bool sparkleTest = 0;

serial_obj_t usbSerial;

/*
char play[5] = "play";
char stop[5] = "stop";

char btBuffer[64];

bool hueTest = 0;
uint32_t hsvTest;

uint16_t hue;
uint8_t sat;
uint8_t val;

bool hsvFadeTest = 0;
bool rgbFadeTest = 0;
uint32_t orangeTarget  = 0xFFA60000;

uint32_t yellowTarget  = 0xFFFF0000;
uint32_t brightYellow  = 0xFFFF8000;

uint32_t blueTarget    = 0x0000FF00;

uint32_t cyanTarget    = 0x00FFFF00;
uint32_t brightCyan    = 0xDDFFFF00;
uint32_t lowCyan       = 0x4B7D7D00;

uint32_t brightRed     = 0xFF808000;
uint32_t lowBrightRed  = 0x80252500;

uint32_t randomTarget  = 0xc4247800;//0xc4234900;

bool pxlColorOutTest = 0;

uint8_t serialTestSct = 0;
uint8_t serialTestPxl = 0;
*/

//************    TEST VAR DECLARATIOIN     **************




byte array_1[3] = {3, 1, 14};

uint16_t heartBeat = 1000;                  // in milliseconds

uint8_t brightnessLED = 50;

uint8_t sctCntTracker = 0;                  // var that keeps track of the index in the strips and neopxlObj arrays
uint8_t *ptrSctCntTracker = &sctCntTracker;


void setup() {

  // initializing the built-in LED for HeartBeat
  pinMode(LED_BUILTIN, OUTPUT);

  // setting up the serial port for debugging on serial monitor and updating obj. attri.
  Serial.begin(9600);
  usbSerial.serialPort = &Serial;
  //Serial.setTimeout(1);

  //*************   BT MODULE SET-UP   **************//
  
  /*
  // finding the BT module's baud rate
  uint16_t tempBaud = baudRateFinder(Serial1);
  uint16_t *defaultBaud = &tempBaud;

  setupBT(Serial1, defaultBaud, BAUD4);
  Serial.println(tempBaud);
  */

  //*************   BT MODULE SET-UP   **************//

  // array_1[0] = 0xAA;
  // array_1[15] = 0xBB;
  // array_2[0] = 0xCC;
  // array_2[15] = 0xDD;

  // *ptrLeFun = 0x5555;

  //*************   STRIP AND LEDS SET-UP   *************//
  
  //neopxlObjSetUp(sctZero, neopxlObjArr, ptrSctCntTracker, brightnessLED);
  //neopxlObjSetUp(sctOne,  neopxlObjArr, ptrSctCntTracker, brightnessLED);
  //neopxlObjSetUp(sctTwo,  neopxlObjArr, ptrSctCntTracker, brightnessLED); //, 0x38d6aa00);  //0xc4247800);
  //neopxlObjSetUp(sctSix,  neopxlObjArr, ptrSctCntTracker, brightnessLED);

  // decrementing the section count to have the exact number
  --sctCntTracker;

  //*************   STRIP AND LEDS SET-UP   *************//
}

void loop() {

  // if(!sparkleTest) {
  //   dumpRam(Serial, &ramStart, 8192);
  //   sparkleTest = !sparkleTest;
  // }
  
  mcuHeartBeat(heartBeat);

  // Serial.write(array_1, sizeof(array_1));
	// delay(1000);
  
  // Gonna have to check for all serial (USB Serial and BT Serial 1)
  
  serialRxCheck(&usbSerial);
  serialTxCheck(&usbSerial);
  
  // if(Serial.available()) {
  //   serialRxRead(&usbSerial);
  //   serialColorRx(&usbSerial);
  //   for(uint8_t i = 0; i < usbSerial.nbXtractedBytes; i++) {
  //     Serial.println(usbSerial.rxByteBuf[i]);
  //   }
  //   serialInterpreter(&usbSerial);
  // }



  // pxlIterator(4);

/*   if(sparkleTest) {
    sparkleInit(0);
    sparkleInit(1);
    sparkleTest = !sparkleTest;
  }

  if(hueTest) {

    uint32_t testColor = 0x9f50d300;
    
    hsvTest = rgbw2hsv(testColor);
    
    hue = uint16_t((hsvTest & 0xFFFF0000) >> 16);
    sat = uint8_t ((hsvTest & 0x0000FF00) >> 8);
    val = uint8_t  (hsvTest & 0x000000FF);

    Serial.print(hue);
    Serial.print('\t');
    Serial.print(sat);
    Serial.print('\t');
    Serial.println(val);

    Serial.println(wrgb2rgbw(neopxlObjArr[0].ColorHSV(hue, sat, val)));

    hueTest = !hueTest;
  }

  if(hsvFadeTest) {

    hsvFadeInit(2, 0, randomTarget, 30000);
    hsvFadeInit(0, 4, 0x8d943c00, 25000);
    hsvFadeInit(2, 2, 0xd3f42f00, 35000);
    hsvFadeInit(1, 0, 0x549a7b00, 40000);
    hsvFadeTest = !hsvFadeTest;
  }

  if(pxlColorOutTest) {

    pxlColorOut(0, 3, 0xAAA8FFFF, 1);
    pxlColorOutTest = !pxlColorOutTest;
    Serial.println(stripsArrayOfPxl[0][3].rgbwColor);
    Serial.println(stripsArrayOfPxl[0][3].hsvColor);

    delay(3000);

    pxlOFF(0, 3);
    Serial.println(stripsArrayOfPxl[0][3].rgbwColor);
    Serial.println(stripsArrayOfPxl[0][3].hsvColor);
  }

  if(rgbFadeTest) {

    rgbFadeInit(1, 5, randomTarget, 30000);
    rgbFadeInit(0, 4, 0x8d943c00, 25000);
    rgbFadeInit(2, 2, 0xd3f42f00, 35000);
    rgbFadeInit(1, 0, 0x549a7b00, 40000);
    rgbFadeTest = !rgbFadeTest;
  } */

/*   if(Serial.available() > 0) {
    // read the incoming bytes:
    nbrOfBytes = Serial.readBytes(serReadBuf, 10);

    if(serReadBuf[0] == 49) { 
      //if received bytes ASCII is '1', write the ASCII '49' to its human readable
      //equivalent of '1' on serial monitor
      Serial.write(serReadBuf, nbrOfBytes);
    }
  } */
  // Arduino read the bytes in the ASCII format. Sending '1' and printing it on the
  // serial monitor gives '49', its ASCII equivalent
  
 

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