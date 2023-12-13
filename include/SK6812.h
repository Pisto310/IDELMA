/*
header file for declaring the functions (scenes) associated with the SK6812 LEDs
*/

#ifndef SK6812_H_
#define SK6812_H_

#include "Adafruit_NeoPixel.h"
#include "Board.h"
#include "EEPROM_lib.h"
#include "User_Lib.h"
#include "Color_Formatting.h"


//************************************************************    GLOBAL TYPES DECLARATION   ************************************************************//

typedef enum PixelStates{
  IDLE,                                     // LED is ready for action
  HSV_FADE,                                 // LED is in the process of fading in HSV color space
  RGB_FADE,                                 // LED is in the process of fading in RGB color space
  BLINK_ONCE,                               // LED is set to blink only once
  BLINK,                                    // LED is blinking
  SPARKLE                                   // LED is in the process of sparkling
}pixel_state_t;


typedef struct PixelActionTime {
  int32_t actionOneTime;                    // time between each step for hue (hueFade) or red (rgbFade). Also, default param. for other timed actions
  int32_t actionTwoTime;                    // time between each step for sat (hueFade) or grn (rgbFade)
  int32_t actionThreeTime;                  // time between each step for val (hueFade) or blu (rgbFade)
}pixel_actionTime_t;


typedef struct PixelActionStartTime {
  uint32_t actionOneStart;                  // start time for action one
  uint32_t actionTwoStart;                  // start time for action two
  uint32_t actionThreeStart;                // start time for action three
}pixel_actionStart_t;


// struct to store the info of each pixel
// each struct occupies a space of 44 bytes
typedef struct PixelMetaDatas {
  uint8_t             pxlSctID;             // draws a parallel to a neopxlObj array for easier matching when doing actions
  uint8_t             pxlID;                // pixel number in the strip/neopxlObj
  pixel_state_t       pxlState;             // pixel state
  pixel_actionTime_t  pxlActionTimes;
  pixel_actionStart_t pxlActionStart;
  uint32_t            rgbwColor;            // actual rgbw color of the pixel. getPixelColor() isn't precise when brightness is under 255
  uint32_t            hsvColor;             // actual hsv color of the pixel. Conversion error from hsv to rgbw causes trouble for hsvFade
  uint32_t            rgbwTarget;           // registered target RGBW color for RGB_FADE or WHT_FADE actions
  uint32_t            hsvTarget;            // registered target HSV color for HSV_FADE action
}pxl_metadata_t;


// struct to store the metadatas of a section of pixel
typedef struct SctMetaDatas {
  byte pxlCount;
  byte brightness;
  byte singlePxlCtrl;
}sct_metadata_t;

//*************************************************************     GLOBAL TYPES DECLARATION   ************************************************************//









//**************************************************************   GLOBAL FUNC DECLARATION   **************************************************************//

void createSection(byte sctIdx, sct_metadata_t sctMetaDataPckt);
void editSection(byte sctIdx, sct_metadata_t sctMetaDataPckt);
void deleteSection(byte sctIdx, sct_metadata_t sctMetaDataPckt);

void sctsConfigSave();
void sctsConfigRead();

sct_metadata_t* getSctMetaDatasPtr();
byte sctMemBlocksUsage(byte pxlCount, byte singlePxlCtrl);

//**************************************************************   GLOBAL FUNC DECLARATION   **************************************************************//








//**************************************************************   DEBUG   **************************************************************//

void sctsConfigRst();
void allOff();

//**************************************************************   DEBUG   **************************************************************//


#endif  /*  SK6812_H_ */