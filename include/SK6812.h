/*
header file for declaring the functions (scenes) associated with the SK6812 LEDs
*/

#include "Adafruit_NeoPixel.h"
#include "Arduino.h"
#include "User_Lib.h"

// max number of sections to be instanciated. Based on available PWM pins of HW, 8 in our case
#define MAX_NO_SCTS         8

// macros to map the zone to its digital output pin
#define PIN_SCT_0           2
#define PIN_SCT_1           3
#define PIN_SCT_2           4
#define PIN_SCT_3           5
#define PIN_SCT_4           6
#define PIN_SCT_5           7
#define PIN_SCT_6           8
#define PIN_SCT_7           9




//******** Will be removed later ********//

// sections count, to be adjusted for each project
#define SCT_COUNT           8

// number of LEDs in most crowded section, to be adjusted for each project
#define LED_COUNT_MAX       6

// macros for nb of LEDs in each zone, variable from prj to prj
#define LED_COUNT_SCT_0     6
#define LED_COUNT_SCT_1     6
#define LED_COUNT_SCT_2     6
#define LED_COUNT_SCT_3     6
#define LED_COUNT_SCT_4     6
#define LED_COUNT_SCT_5     6
#define LED_COUNT_SCT_6     6
#define LED_COUNT_SCT_7     6

//******** Will be removed later ********//




// struct to store the info of each pixel
typedef struct pixelInfo_s {
  uint8_t   pxlSct;               // draws a parallel to a neopxlObj array for easier matching when doing actions
  uint8_t   pxlNbr;               // pixel number in the strip/neopxlObj
  uint8_t   pxlState;             // pixel state
  uint8_t   paddingByte;          // reserved for future use
  int32_t   actionOneTime;        // time between each step for hue (hueFade) or red (rgbFade). Also, default param. for other timed actions
  int32_t   actionTwoTime;        // time between each step for sat (hueFade) or grn (rgbFade)
  int32_t   actionThreeTime;      // time between each step for val (hueFade) or blu (rgbFade)
  uint32_t  actionOneStart;       // start time for action one
  uint32_t  actionTwoStart;       // start time for action two
  uint32_t  actionThreeStart;     // start time for action three
  uint32_t  rgbwColor;            // actual rgbw color of the pixel. getPixelColor() isn't precise when brightness is under 255
  uint32_t  hsvColor;             // actual hsv color of the pixel. Conversion error from hsv to rgbw causes trouble for hsvFade
  uint32_t  rgbwTarget;           // registered target RGBW color for RGB_FADE or WHT_FADE actions
  uint32_t  hsvTarget;            // registered target HSV color for HSV_FADE action
}pixelInfo_t;

enum activeLedAction {
  IDLE,                           // LED is ready for action
  HSV_FADE,                       // LED is in the process of fading in HSV color space
  RGB_FADE,                       // LED is in the process of fading in RGB color space
  BLINK_ONCE,                     // LED is set to blink only once
  BLINK,                          // LED is blinking
  SPARKLE                         // LED is in the process of sparkling
};







//**********   SK6812 STRIPS DECLARATION   **********//

/*
One Adafruit_NeoPixel object is equivalent to 22 bytes in RAM, regardless of how many LEDs
there are per sections
*/

//extern volatile byte array_1[16];

/*
extern Adafruit_NeoPixel sctZero;
extern Adafruit_NeoPixel sctOne;
extern Adafruit_NeoPixel sctTwo;
extern Adafruit_NeoPixel sctThree;
extern Adafruit_NeoPixel sctFour;
extern Adafruit_NeoPixel sctFive;
extern Adafruit_NeoPixel sctSix;
extern Adafruit_NeoPixel sctSeven;
*/

extern volatile byte array_2[16];

/*
1. Declare all Adafruit_NeoPixel objects
2. Allocate memory to store an array containing these objects
3. Read user commands to set the number of LEDs in each obj with updateLength func
4. Free allocated memory space when user has finished filling in sections

The idea here is to replace this 2D array of potentialy "empty" elements (since not
all strips/sections will have the same length) with an array of pointer to 1D arrays
of pixelInfo_t elements, each with varying length
*/


// 2D array containing the section number (row) and each pixel of that section (column)
// volatile extern pixelInfo_t stripsArrayOfPxl[SCT_COUNT][LED_COUNT_MAX];

/*
// array to combine all instantiated neopxl objects
extern Adafruit_NeoPixel neopxlObjArr[SCT_COUNT];
*/

//**********   SK6812 STRIPS DECLARATION   **********//










// void neopxlObjSetUp(Adafruit_NeoPixel &neopxlObj, Adafruit_NeoPixel neopxlArr[], uint8_t *ptrToSctCount, uint8_t maxBrightness, uint32_t startColor = 0);
// void nextColorVal(uint8_t *nextColor, int32_t *actionTime, uint32_t *actionStart, uint8_t targetColor, uint8_t incrDecr = 1);

// void pxlColorUpdt(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat = 0, bool targetUpdt = 0);
// void pxlColorOut(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat = 0);
// void pxlOFF(uint8_t section, uint8_t pixel);
// void pxlIterator(uint8_t sctCount);

// void stripColorFill(uint8_t section, uint32_t color, bool hsvFormat = 0);
// void stripOFF(uint8_t section);

// uint32_t rgbw2wrgb(uint32_t rgbwColor);
// uint32_t wrgb2rgbw(uint32_t wrgbColor);
// uint32_t rgbw2rgb(uint32_t rgbwColor);
// uint32_t rgbw2hsv(uint32_t rgbwColor);

// void blinkOnce(uint8_t section, uint8_t pixel, uint32_t color, uint16_t blinkTime, bool blinkState = 1); 
// void hsvFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, int32_t fadeTime);
// void hsvFade(uint8_t section, uint8_t pixel);
// void rgbFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, int32_t fadeTime);
// void rgbFade(uint8_t section, uint8_t pixel);

// void sparkleInit(uint8_t section);
// void sparkleSct(uint8_t section, uint8_t pixel);

uint8_t getAssignedScts();