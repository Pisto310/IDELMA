//Arduino.h is included in the header of the NeoPixel lib

//#include "BTmodule.h"
#include "User_Lib.h"
#include "SK6812.h"
#include "Serial_lib.h"
#include "Dump.h"
#include "EEPROM_lib.h"

//HC-06 module address 00-14-03-05-5A-D5





//************    TEST VAR DECLARATIOIN     **************

uint16_t ramStart = 0x0200;

bool sparkleTest = 1;

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


uint16_t heartBeat = 500;                  // in milliseconds

uint8_t brightnessLED = 255;


void setup() {

  // initializing the built-in LED for HeartBeat
  pinMode(LED_BUILTIN, OUTPUT);

  // setting up the serial port for debugging on serial monitor and updating obj. attri.
  Serial.begin(9600);
  usbSerial.serialPort = &Serial;
  //Serial.setTimeout(1);

  bootUp();

  //*************   BT MODULE SET-UP   **************//
  
  /*
  // finding the BT module's baud rate
  uint16_t tempBaud = baudRateFinder(Serial1);
  uint16_t *defaultBaud = &tempBaud;

  setupBT(Serial1, defaultBaud, BAUD4);
  Serial.println(tempBaud);
  */

  //*************   BT MODULE SET-UP   **************//

  // byte test[6] = {0, 5, 1, 2, 2, 4};
  // byte testt[6] = {0, 3, 1, 4, 2, 0};
  // byte testtt[6] = {0, 3, 1, 4, 2, 6};

  // configBrd(test, 6);
  // updatingPixelAttr(0, 2, 0xFFFFFFFF);
  // updatingPixelAttr(0, 4, 0xFFFFFFFF);
  // updatingPixelAttr(1, 1, 0xDDDDDDDD);
  // updatingPixelAttr(2, 1, 0xAAAAAAAA);
  // updatingPixelAttr(2, 3, 0xAAAAAAAA);

  // dumpRam(Serial, &ramStart, 2480);

  // configBrd(testt, 6);

  // configBrd(testtt, 6);

  // updatingPixelAttr(2, 0, 0x99999999);
  // updatingPixelAttr(2, 2, 0x99999999);
  // updatingPixelAttr(2, 5, 0x99999999);

  // dumpRam(Serial, &ramStart, 2480);

  // sctsConfigSave();

  //*************   STRIP AND LEDS SET-UP   *************//
  
  //neopxlObjSetUp(sctZero, neopxlObjArr, ptrSctCntTracker, brightnessLED);
  //neopxlObjSetUp(sctOne,  neopxlObjArr, ptrSctCntTracker, brightnessLED);
  //neopxlObjSetUp(sctTwo,  neopxlObjArr, ptrSctCntTracker, brightnessLED); //, 0x38d6aa00);  //0xc4247800);
  //neopxlObjSetUp(sctSix,  neopxlObjArr, ptrSctCntTracker, brightnessLED);

  //*************   STRIP AND LEDS SET-UP   *************//
}

void loop() {

  if(ptrPxlInfo && !sparkleTest) {
    dumpRam(Serial, &ramStart, 2480);     // was 8192 for the whole RAM
    sparkleTest = !sparkleTest;
  }
  
  mcuHeartBeat(heartBeat);

  // Serial.write(array_1, sizeof(array_1));
	// delay(1000);
  
  // Gonna have to check for all serial (USB Serial and BT Serial 1)
  
  serialRxCheck(&usbSerial);
  serialTxCheck(&usbSerial);

  // stripColorFill(0, 0xFD8A7F00);
  // stripColorFill(1, 0x28FFA300);
  // stripColorFill(2, 0xFEC3177F);
  // stripColorFill(2, 0xFD8A7F00);
  // stripColorFill(3, 0xFD7E0800);
  // stripColorFill(4, 0xFD8A7F00);
  // stripColorFill(5, 0xFD7E0800);
  // stripColorFill(6, 0x000000B4);
  // stripColorFill(7, 0xFFFF0000);
  // stripColorFill(8, 0xFD8A7F00);

  // pixelActionsHandler();
  
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