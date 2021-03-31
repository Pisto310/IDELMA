/*
header file for declaring the functions (scenes) associated with the SK6812 LEDs
*/

#include "Adafruit_NeoPixel.h"
#include "Arduino.h"
#include "User_Lib.h"

// sections count, to be adjusted for each project
#define SCT_COUNT           8

// number of LEDs in most crowded section, to be adjusted for each project
#define LED_COUNT_MAX       6

// macros to map the zone to its digital output pin
#define PIN_SCT_1           2
#define PIN_SCT_2           3
#define PIN_SCT_3           4
#define PIN_SCT_4           5
#define PIN_SCT_5           6
#define PIN_SCT_6           7
#define PIN_SCT_7           8
#define PIN_SCT_8           9

// macros for nb of LEDs in each zone, variable from prj to prj
#define LED_COUNT_SCT_1     6
#define LED_COUNT_SCT_2     6
#define LED_COUNT_SCT_3     6
#define LED_COUNT_SCT_4     6
#define LED_COUNT_SCT_5     6
#define LED_COUNT_SCT_6     6
#define LED_COUNT_SCT_7     6
#define LED_COUNT_SCT_8     6

// LED RGB colors lookup table
static const PROGMEM uint32_t colorsLookup[24][6] = {
  {0xFF000000, 0xFF646400, 0xFF969600, 0xFFC8C800, 0xFFE6E600, 0xFFFFFF00},
  {0xFF640000, 0xFF6E1400, 0xFF786400, 0xFFA59B00, 0xFFE6D200, 0xFFFFFF00},
  {0xFF960000, 0xFF8C1400, 0xFFB42800, 0xFFDC7800, 0xFFF0BE00, 0xFFFFFF00},
  {0xFFC80000, 0xFFD21400, 0xFFE66400, 0xFFF09600, 0xFFFFC800, 0xFFFFFF00},
  {0xFFFF0000, 0xFFFF1400, 0xFFFF5000, 0xFFFF9600, 0xFFFFD200, 0xFFFFFF00},
  {0xC8FF0000, 0xC8FF1400, 0xDCFF5000, 0xE6FF9600, 0xEBFFB400, 0xFFFFFF00},
  {0x96FF0000, 0x96FF1400, 0xC8FF2800, 0xDCFF9600, 0xE6FFAA00, 0xFFFFFF00},
  {0x64FF0000, 0x6EFF1400, 0x96FF2800, 0xC8FF9600, 0xE6FFA000, 0xFFFFFF00},
  {0x00FF0000, 0x64FF6400, 0x96FF9600, 0xC8FFC800, 0xE1FFE100, 0xFFFFFF00},
  {0x00FF6400, 0x14FF9600, 0x64FFB400, 0xB4FFD200, 0xEBFFDC00, 0xFFFFFF00},
  {0x00FF9600, 0x14FFA000, 0x64FFC800, 0xA0FFD200, 0xC8FFDC00, 0xFFFFFF00},
  {0x00FFC800, 0x14FFC800, 0x28FFDC00, 0x8CFFE600, 0xC8FFFF00, 0xFFFFFF00},
  {0x00FFFF00, 0x14FFFF00, 0x28FFFF00, 0x96FFFF00, 0xB4FFFF00, 0xFFFFFF00},
  {0x00C8FF00, 0x14D2FF00, 0x28E6FF00, 0x3CF0FF00, 0xAAFFFF00, 0xFFFFFF00},
  {0x0096FF00, 0x14B4FF00, 0x28C8FF00, 0x3CE6FF00, 0x96FFFF00, 0xFFFFFF00},
  {0x0064FF00, 0x1496FF00, 0x28B4FF00, 0x3CC8FF00, 0x8CFFFF00, 0xFFFFFF00},
  {0x0000FF00, 0x1464FF00, 0x2896FF00, 0x50B4FF00, 0xA0DCFF00, 0xFFFFFF00},
  {0x6400FF00, 0x7814FF00, 0x8C64FF00, 0x9696FF00, 0xC8C8FF00, 0xFFFFFF00},
  {0x9600FF00, 0x9632FF00, 0xB464FF00, 0xDC8CFF00, 0xE6B4FF00, 0xFFFFFF00},
  {0xC800FF00, 0xC814FF00, 0xDC3CFF00, 0xF05AFF00, 0xFFA0FF00, 0xFFFFFF00},
  {0xFF00FF00, 0xFF3CFF00, 0xFF64F000, 0xFF96FA00, 0xFFB4FF00, 0xFFFFFF00},
  {0xFF00C800, 0xFF14DC00, 0xFF28E600, 0xFF8CF000, 0xFFAAFF00, 0xFFFFFF00},
  {0xFF009600, 0xFF14C800, 0xFF28C800, 0xFF64C800, 0xFFA0F000, 0xFFFFFF00},
  {0xFF006400, 0xFF147800, 0xFF508C00, 0xFF64A000, 0xFF8CB400, 0xFFFFFF00}
};

// struct to store the info of each pixel
typedef struct pixelInfo_s {
  uint8_t   pxlSct;               // draws a parallel to a neopxlObj array for easier matching when doing actions
  uint8_t   pxlNbr;               // pixel number in the strip/neopxlObj
  uint8_t   pxlState;             // pixel state
  uint32_t  actionOneTime;        // time between each step for hue (hueFade) or red (lerpFade), or onTime for sparkle effect
  uint32_t  actionTwoTime;        // time between each step for sat (hueFade) or grn (lerpFade)
  uint32_t  actionThreeTime;      // time between each step for val (hueFade) or blu (lerpFade)
  uint32_t  actionOneStart;       // start time for action one
  uint32_t  actionTwoStart;       // start time for action two
  uint32_t  actionThreeStart;     // start time for action three
}pixelInfo;

enum activeLedAction {
  IDLE,                         // LED is ready for action
  ON,                           // LED is full on
  OFF,                          // LED is full off
  FADE,                         // LED is in the process of fading
  SPARKLE                       // LED is in the process of sparkling
};

//**********   SK6812 STRIPS DECLARATION   **********//

extern Adafruit_NeoPixel sctZero;
extern Adafruit_NeoPixel sctSix;

// 2D array containing the section number (row) and each pixel of that section (column)
extern pixelInfo stripsArrayOfPxl[SCT_COUNT][LED_COUNT_MAX];

// array to combine all instantiated neopxl objects
extern Adafruit_NeoPixel neopxlObjArr[SCT_COUNT];

//**********   SK6812 STRIPS DECLARATION   **********//

void neopxlObjSetUp(Adafruit_NeoPixel &neopxlObj, Adafruit_NeoPixel neopxlArr[], uint8_t *ptrToSctCount, uint8_t maxBrightness, uint32_t startColor = 0);
void pxlIterator(uint8_t sctCount);
uint32_t rgbw2wrgb(uint32_t rgbwColor);
uint32_t rgbw2hsv(uint32_t color);

void sparkleInit(uint8_t section);
void sparkleSct(pixelInfo_s pixel);
