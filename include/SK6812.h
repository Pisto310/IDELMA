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
#include "LED_Actions.h"


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
// volatile extern pxl_metadata_t stripsArrayOfPxl[SCT_COUNT][LED_COUNT_MAX];

//**********   SK6812 STRIPS DECLARATION   **********//


extern pxl_metadata_t* pxlMetaDataPtr;
//extern pxl_metadata_t* pxlMetaDataPtrArr[];
//extern Adafruit_NeoPixel neopxlObjArr[];


//**********    GLOBAL FUNCTIONS DECLARATION   ************//


void createSection(byte sctIdx, sct_metadata_t sctMetaDataPckt);
void editSection(byte sctIdx, sct_metadata_t sctMetaDataPckt);
void deleteSection(byte sctIdx, sct_metadata_t sctMetaDataPckt);
                
                // TEMPORARY //
void updatingPixelAttr(uint8_t section, uint8_t pixel, uint32_t whatev);
                // TEMPORARY //

sct_metadata_t getSctMetaDatas(uint8_t index);
sct_metadata_t* getSctMetaDatasPtr();

void sctsConfigSave();
void sctsConfigRead();
void sctsConfigRst();

void stripOFF(uint8_t section);


//**********    GLOBAL FUNCTIONS DECLARATION   ************//

#endif  /*  SK6812_H_ */