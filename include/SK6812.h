/*
header file for declaring the functions (scenes) associated with the SK6812 LEDs
*/

#ifndef SK6812_H_
#define SK6812_H_

#include "Adafruit_NeoPixel.h"
#include "Arduino.h"
#include "User_Lib.h"
#include "Board.h"


typedef enum {
  IDLE,                           // LED is ready for action
  HSV_FADE,                       // LED is in the process of fading in HSV color space
  RGB_FADE,                       // LED is in the process of fading in RGB color space
  BLINK_ONCE,                     // LED is set to blink only once
  BLINK,                          // LED is blinking
  SPARKLE                         // LED is in the process of sparkling
}pixel_state_t;

typedef struct {
  int32_t actionOneTime;          // time between each step for hue (hueFade) or red (rgbFade). Also, default param. for other timed actions
  int32_t actionTwoTime;          // time between each step for sat (hueFade) or grn (rgbFade)
  int32_t actionThreeTime;        // time between each step for val (hueFade) or blu (rgbFade)
}pixel_actionTime_t;

typedef struct {
  uint32_t actionOneStart;        // start time for action one
  uint32_t actionTwoStart;        // start time for action two
  uint32_t actionThreeStart;      // start time for action three
}pixel_actionStart_t;

// struct to store the info of each pixel
// each struct occupies a space of 44 bytes
typedef struct {
  uint8_t             pxlSct;               // draws a parallel to a neopxlObj array for easier matching when doing actions
  uint8_t             pxlNbr;               // pixel number in the strip/neopxlObj
  pixel_state_t       pxlState;             // pixel state
  pixel_actionTime_t  pxlActionTimes;
  pixel_actionStart_t pxlActionStart;
  uint32_t            rgbwColor;            // actual rgbw color of the pixel. getPixelColor() isn't precise when brightness is under 255
  uint32_t            hsvColor;             // actual hsv color of the pixel. Conversion error from hsv to rgbw causes trouble for hsvFade
  uint32_t            rgbwTarget;           // registered target RGBW color for RGB_FADE or WHT_FADE actions
  uint32_t            hsvTarget;            // registered target HSV color for HSV_FADE action
}pixel_info_t;

// struct to store the info of a section of pixel
typedef struct {
  uint8_t nbrOfPxls;
  uint8_t setBrightness;
}section_info_t;


//**********   SK6812 STRIPS DECLARATION   **********//

// One Adafruit_NeoPixel object is equivalent to 22 bytes in RAM, regardless of how many LED there are per sections
extern Adafruit_NeoPixel  sct_0;
extern Adafruit_NeoPixel  sct_1;
extern Adafruit_NeoPixel  sct_2;
extern Adafruit_NeoPixel  sct_3;
extern Adafruit_NeoPixel  sct_4;
extern Adafruit_NeoPixel  sct_5;
extern Adafruit_NeoPixel  sct_6;
extern Adafruit_NeoPixel  sct_7;
extern Adafruit_NeoPixel  sct_8;
extern Adafruit_NeoPixel  sct_9;
extern Adafruit_NeoPixel sct_10;
extern Adafruit_NeoPixel sct_11;


// 2D array containing the section number (row) and each pixel of that section (column)
// volatile extern pixel_info_t stripsArrayOfPxl[SCT_COUNT][LED_COUNT_MAX];

//**********   SK6812 STRIPS DECLARATION   **********//


extern pixel_info_t* ptrPxlInfo;
//extern pixel_info_t* arrPtrPxlInfo[];
//extern Adafruit_NeoPixel neopxlObjArr[];


//**********    GLOBAL FUNCTIONS DECLARATION   ************//

void setupSection(uint8_t pxlCount, uint8_t maxBrightness = 50, bool sctsAsPxl = false);
void resetSection(uint8_t section, uint8_t newNbrOfLEDs, uint8_t maxBrightness = 50);
                
                // TEMPORARY //
void updatingPixelAttr(uint8_t section, uint8_t pixel, uint32_t whatev);
                // TEMPORARY //

uint8_t sectionInfoArrSerial(byte byteBuffer[64]);

void setupSaveToEeprom(void);
void setupFromEepromSave(void);

void eepromMemCheck(void);

void pixelActionsHandler(void);

// void neopxlObjSetUp(Adafruit_NeoPixel &neopxlObj, Adafruit_NeoPixel neopxlArr[], uint8_t *ptrToSctCount, uint8_t maxBrightness, uint32_t startColor = 0);
// void nextColorVal(uint8_t *nextColor, int32_t *actionTime, uint32_t *actionStart, uint8_t targetColor, uint8_t incrDecr = 1);

// void pxlIterator(uint8_t sctCount);

void pxlStateUpdt(uint8_t section, uint8_t pixel, pixel_state_t state);
void pxlColorUpdt(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat = 0, bool targetUpdt = 0);
void pxlColorOut(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat = 0);
void pxlOFF(uint8_t section, uint8_t pixel);

void stripColorFill(uint8_t section, uint32_t color, bool hsvFormat = 0);
void stripOFF(uint8_t section);

// void blinkOnce(uint8_t section, uint8_t pixel, uint32_t color, uint16_t blinkTime, bool blinkState = 1); 
// void hsvFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, int32_t fadeTime);
// void hsvFade(uint8_t section, uint8_t pixel);
// void rgbFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, int32_t fadeTime);
// void rgbFade(uint8_t section, uint8_t pixel);

// void sparkleInit(uint8_t section);
// void sparkleSct(uint8_t section, uint8_t pixel);

//**********    GLOBAL FUNCTIONS DECLARATION   ************//

#endif