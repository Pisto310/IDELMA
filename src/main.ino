//Arduino.h is included in the header of the NeoPixel lib

#include "BTmodule.h"
#include "User_Lib.h"
#include "SK6812.h"

//HC-06 module address 00-14-03-05-5A-D5





//************    TEST VAR DECLARATIOIN     **************

char play[5] = "play";
char stop[5] = "stop";

char btBuffer[64];

bool btFlag = 1;

bool sparkleTest = 0;

bool hueTest = 0;
uint32_t hsvTest;
uint16_t hue;
uint8_t sat;
uint8_t val;

bool hsvFadeTest = 1;
uint32_t orangeTarget  = 0xFFA60000;

uint32_t yellowTarget  = 0xFFFF0000;
uint32_t brightYellow  = 0xFFFF8000;

uint32_t blueTarget    = 0x0000FF00;

uint32_t cyanTarget    = 0x00FFFF00;
uint32_t brightCyan    = 0xDDFFFF00;
uint32_t lowCyan       = 0x4B7D7D00;

uint32_t brightRed     = 0xFF808000;
uint32_t lowBrightRed  = 0x80252500;

uint32_t randomTarget  = 0x38d6aa00;

//************    TEST VAR DECLARATIOIN     **************






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
  neopxlObjSetUp(sctTwo, neopxlObjArr, ptrSctCntTracker, brightnessLED, 0xFF000000);
  neopxlObjSetUp(sctSix, neopxlObjArr, ptrSctCntTracker, brightnessLED);

  // decrementing the section count to have the exact number
  --sctCntTracker;

  //*************   STRIP AND LEDS SET-UP   *************//
}

void loop() {
  
  if(btFlag) {
    mcuHeartBeat(heartBeat);
  }

  if(sparkleTest) {
    sparkleInit(0);
    sparkleInit(1);
    sparkleTest = !sparkleTest;
  }

  if(hueTest) {
    
    hsvTest = rgbw2hsv(0xff010000);
    //Serial.println(hsvTest);
    
    hue = uint16_t((hsvTest & 0xFFFF0000) >> 16);
    sat = uint8_t ((hsvTest & 0x0000FF00) >> 8);
    val = uint8_t  (hsvTest & 0x000000FF);

    Serial.println(wrgb2rgbw(neopxlObjArr[0].ColorHSV(43, 255, 255)));
    Serial.println(rgbw2hsv(wrgb2rgbw(neopxlObjArr[0].ColorHSV(43, 255, 255))));
    hueTest = !hueTest;
  }

  if(hsvFadeTest) {
    
    hsvFadeInit(1, 0, randomTarget, 30000);
    hsvFadeTest = !hsvFadeTest;

    /*
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t *ptr_r = &r;
    uint8_t *ptr_g = &g;
    uint8_t *ptr_b = &b;

    fast_hsv2rgb_32bit(4, 254, 255, ptr_r, ptr_g, ptr_b);
    
    Serial.print(r);
    Serial.print("\t");
    Serial.print(g);
    Serial.print("\t");
    Serial.print(b);
    Serial.print("\t");
    
    Serial.print(rgbw2hsv(0xff040100));      
    */
    //Serial.println(rgbw2hsv(0xFF050100));
  }

  pxlIterator(2);

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