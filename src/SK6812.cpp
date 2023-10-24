/*
The LED pixel strips are instanciated here and scenes are also expanded upon
*/

// My set-up, when using the object's setPixelColor method is set as a GRBW string

#include "SK6812.h"

//**********    GLOBAL VARIABLES DECLARATION   ************//

// volatile pixel_info_t stripsArrayOfPxl[SCT_COUNT][LED_COUNT_MAX];

//**********    GLOBAL VARIABLES DECLARATION   ************//





//**********    LOCAL VARIABLES DECLARATION   ************//

pixel_info_t* ptrPxlInfo = (pixel_info_t*)calloc(PXLINFO_HEAP_SIZE, sizeof(pixel_info_t));
pixel_info_t* arrPtrPxlInfo[MAX_NO_SCTS];

// Filling the neopxl Objects array
Adafruit_NeoPixel neopxlObjArr[MAX_NO_SCTS] = {
  Adafruit_NeoPixel(0,  PIN_SCT_0, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_1, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_2, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_3, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_4, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_5, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_6, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_7, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_8, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_9, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0, PIN_SCT_10, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0, PIN_SCT_11, NEO_GRBW + NEO_KHZ800),
};

section_info_t sectionInfoArr[MAX_NO_SCTS];

static uint8_t sctIndexTracker = 0;

eeprom_chapter_t sctInfoChap = {
  .startIdx   = EEPROM_PAGE_IDX(EEPROM_SCTS_INFO_START_PAGE),
  .bytesCount = (sizeof(section_info_t) * MAX_NO_SCTS)
};

//**********    LOCAL VARIABLES DECLARATION   ************//






//**********    LOCAL FUNCTIONS DECLARATION   ************//

// void sctReset(pixel_info_t* heapStartAddr, uint8_t section, uint8_t newPxlCount, uint16_t pxlInfoToReset = 0);

void setPxlInfo(uint8_t pxlCount);
void setNeoPxl(uint8_t pxlCount, uint8_t brightness);

void editPxlCount(uint8_t sctID, uint8_t newPxlCount);

void setupFromSave();

void removingPxlsFromSct(uint8_t section, uint8_t newPxlCount);
void addingPxlsToSct(uint8_t section, uint8_t newPxlCount);

// Put in a separate lib?
uint32_t rgbw2wrgb(uint32_t rgbwColor);
uint32_t wrgb2rgbw(uint32_t wrgbColor);
uint32_t rgbw2rgb(uint32_t rgbwColor);
uint32_t rgbw2hsv(uint32_t rgbwColor);

//**********    LOCAL FUNCTIONS DECLARATION   ************//


/// @brief Called to setup a section
/// @param pxlCount Number of pixel contained in the instanciated section
/// @param brightness Brightness setting, 0=minimum (off), 255=brightest
/// @param sctAsPxl Indicates if all LEDs separate entities or one single 
///                 entity (affects number of pixel_info_t obj created)
void setupSection(uint8_t pxlCount, uint8_t brightness, bool sctAsPxl) {
  if(sctAsPxl) {
    if(remainingHeapSpace(1) && remainingSctsPins()) {
      setPxlInfo(1);
      pixelsMgmtAdd(1);
    }
  }
  else {
    if(remainingHeapSpace(pxlCount) && remainingSctsPins()) {
      setPxlInfo(pxlCount);
      pixelsMgmtAdd(pxlCount);
    }
  }
  setNeoPxl(pxlCount, brightness);
  sectionsMgmtAdd();

  // updating the section info matrix
  sectionInfoArr[sctIndexTracker].pxlCount   = pxlCount;
  sectionInfoArr[sctIndexTracker].brightness = brightness;

  //**debug**//
  stripColorFill(sctIndexTracker, 0x3B659C00);
  //**debug**//

  sctIndexTracker++;
}


/// @brief Clears a section in its entirety
/// @param sctID Affected section's index
void clearSection(uint8_t sctID) {
  if(sctID < sctIndexTracker && neopxlObjArr[sctID].numPixels()) {
    removingPxlsFromSct(sctID, 0);
    sectionsMgmtRemove();
    sctIndexTracker--;
  }
}


/// @brief Edit section attributes
/// @param sctID Affected section's index
/// @param rxedPxlCount Section's pixel count as received through serial ('0' case is handled in clearSection)
void editSection(uint8_t sctID, uint8_t rxedPxlCount) {
  if(rxedPxlCount != sectionInfoArr[sctID].pxlCount) {
    editPxlCount(sctID, rxedPxlCount);
  }
  // More condition to test as more section attributes get added (brightness, sctAsPxl, etc.)
}


/// @brief Sets the pixel_info_t parameters for each pixels of a section. Called at section's inception
/// @param pxlCount Number of pixel contained in the instanciated section
void setPxlInfo(uint8_t pxlCount) {
  arrPtrPxlInfo[sctIndexTracker] = ptrPxlInfo;
  ptrPxlInfo += pxlCount;

  for(uint8_t pxlNbr = 0; pxlNbr < pxlCount; pxlNbr++) {
    (arrPtrPxlInfo[sctIndexTracker] + pxlNbr)->pxlSctID = sctIndexTracker;
    (arrPtrPxlInfo[sctIndexTracker] + pxlNbr)->pxlID    = pxlNbr;
    (arrPtrPxlInfo[sctIndexTracker] + pxlNbr)->pxlState = IDLE;
  }
}


/// @brief Sets the NeoPixel object when creating a section. Called at section's inception
/// @param pxlCount Number of pixel contained in the instanciated section
/// @param brightness Brightness setting, 0=minimum (off), 255=brightest
void setNeoPxl(uint8_t pxlCount, uint8_t brightness) {
  neopxlObjArr[sctIndexTracker].updateLength((uint16_t)pxlCount);
  neopxlObjArr[sctIndexTracker].begin();           // needs to be done for each instanciated neopxlObj
  neopxlObjArr[sctIndexTracker].setBrightness(brightness);
  // stripColorFill(sectionIndex, 0xFF00FF00);
  // stripOFF(sectionIndex);
}


/// @brief Edit the pixel count attribute of a particular section
/// @param sctID Affected section's index
/// @param newPxlCount Pixel count to be updated in affected section
void editPxlCount(uint8_t sctID, uint8_t newPxlCount) {
  if(sctID < sctIndexTracker && remainingHeapSpace(newPxlCount)) {
    if(newPxlCount < sectionInfoArr[sctID].pxlCount) {
      removingPxlsFromSct(sctID, newPxlCount);
    }
    else if(newPxlCount > sectionInfoArr[sctID].pxlCount) {
      addingPxlsToSct(sctID, newPxlCount);
    }
  }
}


/// @brief Used to acces the value of the local sctIndexTracker variable
/// @return sctIndexTracker local variable
uint8_t getSctIndexTracker() {
  return sctIndexTracker;
}


/// @brief Function that returns the section_info_t obj at the position given by the index parameter
/// @param index position of the section_info_t obj to return
/// @return a (pointer) section_info_t obj
section_info_t getSctInfos(uint8_t index) {
  return sectionInfoArr[index];
}


// This is a temporary func to change a pxl hsvTarget attr.
void updatingPixelAttr(uint8_t section, uint8_t pixel, uint32_t whatev) {
  
  // Got to check if the pixel number asked for is part of the section
  if(pixel < neopxlObjArr[section].numPixels()) {
    (arrPtrPxlInfo[section] + pixel)->hsvTarget = whatev;
  }
  else {
    Serial.println("pixel passed is out of range");
  }
}


/// @brief Verify if a previous section config save is contained in
///        the EEPROM.
/// @return A boolean indicating if there is a saved config (1) or not (0)
bool checkSctsConfigSave() {
  if (eepromByteRead(sctInfoChap.startIdx)) {
    return 1;
  }
  else {
    return 0;
  }
}


/// @brief Save user configuration into EEPROM for future
///        boot-up use.
void sctsConfigSave() {
  eepromWriteChap(sctInfoChap, (byte*) sectionInfoArr);
}


/// @brief Reads and extract the saved sections configuration
///        in EEPROM if there is one. Called at boot-up.
void sctsConfigRead() {
  // eepromReset(sctInfoChap);
  if (checkSctsConfigSave()) {
    eepromReadChap(sctInfoChap, (byte*) sectionInfoArr);
    setupFromSave();
  }
}


/// @brief Simple reset eeprom sct infos chap (for debug purposes)
void sctsConfigRst() {
  if (checkSctsConfigSave()) {
    eepromReset(sctInfoChap);
  }
}


/// @brief Setup board from a peviously saved configuration
void setupFromSave() {
  for (uint8_t i = 0; i < MAX_NO_SCTS; i++) {
    uint8_t pixelCount = sectionInfoArr[i].pxlCount;
    uint8_t brightness = sectionInfoArr[i].brightness;
    if (pixelCount && brightness) {
      setupSection(pixelCount, brightness);
    }
  }
}


// Setting up the board from a saved setup config
// void setupFromEepromSave(void) {

//   uint16_t eepromAddr = EEPROM_PAGE_ADDR(EEPROM_SCTS_MGMT_PAGE);
//   byte sctsToSetup = EEPROM.read(eepromAddr);

//   eepromAddr += BYTE_SIZE;

//   while(EEPROM.read(eepromAddr) || eepromAddr < (sizeof(sectionIndex) + 1)) {
//     createSection(EEPROM.read(eepromAddr), EEPROM.read(eepromAddr + BYTE_SIZE));
//     eepromAddr += sizeof(section_info_t);
//   }
// }

// void eepromPxlInfoRead(void) {

//   uint16_t counter_1 = 0;
//   uint8_t counter_2 = 0;

//   // Start address of where to write to in RAM casted as a single byte pointer
//   uint8_t *ramAddr = (uint8_t*) ptrPxlInfo;

//   uint8_t  blockSize = sizeof(pixel_info_t);            // Might upgrade to uint16_t?
//   uint8_t  numBlocks = PXLINFO_HEAP_SIZE;
//   uint16_t numBytesToRead = blockSize * numBlocks;

//   byte eepromVal;
//   size_t unitSize = sizeof(byte);

//   for(uint16_t eepromAddr = EEPROM_PXLINFO_START_ADDR; eepromAddr < (EEPROM_PXLINFO_START_ADDR + numBytesToRead); eepromAddr += unitSize) {
    
//     eepromVal = EEPROM.read(eepromAddr);

//     if(!counter_1 && counter_2 == eepromVal) {
//       *ramAddr = eepromVal;
//       arrPtrPxlInfo[counter_2] = (pixel_info_t*) ramAddr;
//       counter_2++;
//     }
//     else {    // case for which we're not at pixel_info_t obj first byte
//       *ramAddr = eepromVal;
//     }

//     // section below to increment counters & ramAddr
//     ramAddr += unitSize;

//     if(counter_1 < (blockSize - 1)) {
//       counter_1++;
//     }
//     else {
//       counter_1 = 0;
//     }
//   }
// }

void eepromMemCheck(void) {
  
  uint16_t eepromStartAddr = 0;

  for(uint8_t i = 0; i < 64; i++) {
    Serial.println(EEPROM.read(eepromStartAddr), HEX);
    eepromStartAddr += BYTE_SIZE;
  }
}

// // func that takes care of handling the next color value for fade actions (HSV or RGB)
// void nextColorVal(uint8_t *nextColor, int32_t *actionTime, uint32_t *actionStart, uint8_t targetColor, uint8_t incrDecr) {
//   if(*actionTime & 0x80000000) {
//     *nextColor -= incrDecr;                             // the steps are negative, we need to decrement
//     if(*nextColor <= targetColor) {
//       *actionTime = 0;                                  // target is reached, no need to come back in statement again
//       *nextColor = targetColor;
//     }
//     else {
//       *actionStart = millis();
//     }
//   }
//   else {
//     *nextColor += incrDecr;                             // steps are positive, we increment
//     if(*nextColor >= targetColor) {
//       *actionTime = 0;                                  // target is reached, no need to come back in statement again
//       *nextColor = targetColor;
//     }
//     else {
//       *actionStart = millis();
//     }
//   }
// }



void pixelActionsHandler(void) {
  for(uint8_t sct = 0; sct < sctIndexTracker; sct++) {
    for(uint8_t pxl = 0; pxl < sectionInfoArr[sct].pxlCount; pxl++) {
      switch ((arrPtrPxlInfo[sct] + pxl)->pxlState) {
      
      default:
        // FSM comes here if the pixel state is IDLE
        break;
      }
    }
  }
}




// void pxlIterator(uint8_t sectionIndex) {
//   for(uint8_t section = 0; section < sectionIndex; section++) {
//     for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
//       switch (stripsArrayOfPxl[section][pixel].pxlState) {
//       case HSV_FADE:
//         if(stripsArrayOfPxl[section][pixel].hsvColor != stripsArrayOfPxl[section][pixel].hsvTarget) {
//           hsvFade(section, pixel);
//         }
//         else {
//           stripsArrayOfPxl[section][pixel].pxlState = IDLE;
//           Serial.println("here");
//         }
//         break;

//       case RGB_FADE:
//         if(stripsArrayOfPxl[section][pixel].rgbwColor != stripsArrayOfPxl[section][pixel].rgbwTarget) {
//           rgbFade(section, pixel);
//         }
//         else {
//           stripsArrayOfPxl[section][pixel].pxlState = IDLE;
//           Serial.println("there");
//         }
//         break;

//       case BLINK_ONCE:
//         if(millis() - stripsArrayOfPxl[section][pixel].actionOneStart >= absVar(stripsArrayOfPxl[section][pixel].actionOneTime)) {
//         pxlOFF(section, pixel);
//         stripsArrayOfPxl[section][pixel].pxlState = IDLE;
//         }

//       case SPARKLE:
//         sparkleSct(section, pixel);
//         break;
      
//       default:
//         stripsArrayOfPxl[section][pixel].pxlState = IDLE;       // check if statement is useful
//         break;
//       }
//     }
//   }
// }





//******   PIXEL ACTIONS SECTION   ******//

// All functions to modify properties of a pixel (pixel_info)


void pxlStateUpdt(uint8_t section, uint8_t pixel, pixel_state_t state) {
  // First, maybe check if I have to reset the ongoing action times
  if((arrPtrPxlInfo[section] + pixel)->pxlActionStart.actionOneStart) {
    
    // func to erase action thingies

  }
  
  // changing state
  (arrPtrPxlInfo[section] + pixel)->pxlState = state;
}

// Function that can be called to update either one of the color attribute of a pixel
// If the HSV color format is passed, the associated bool should be passed as True
// Func updates the actual color attr by default. For updating the target color attr, bool should be passed as True
void pxlColorUpdt(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat, bool targetUpdt) {
  
  if(hsvFormat) {
    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;

    if(targetUpdt) {
      (arrPtrPxlInfo[section] + pixel)->rgbwTarget = wrgb2rgbw(neopxlObjArr[section].ColorHSV(hue, sat, val));
      (arrPtrPxlInfo[section] + pixel)->hsvTarget = color;
    }
    else {
      (arrPtrPxlInfo[section] + pixel)->rgbwColor = wrgb2rgbw(neopxlObjArr[section].ColorHSV(hue, sat, val));
      (arrPtrPxlInfo[section] + pixel)->hsvColor = color;
    }
  }

  else {
    if(targetUpdt) {
      (arrPtrPxlInfo[section] + pixel)->rgbwTarget = color;
      (arrPtrPxlInfo[section] + pixel)->hsvTarget = rgbw2hsv(color);
    }
    else {
      (arrPtrPxlInfo[section] + pixel)->rgbwColor = color;
      (arrPtrPxlInfo[section] + pixel)->hsvColor = rgbw2hsv(color);
    }
  }
}

// simple function to output a chosen color to a pixel
// color can be of either HSV or RGB format
// gamma32 correction is applied on the color outputted to the strip
void pxlColorOut(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat) {
  
  if(hsvFormat) {
    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;

    pxlColorUpdt(section, pixel, color, hsvFormat);
    neopxlObjArr[section].setPixelColor(pixel, neopxlObjArr[section].gamma32(neopxlObjArr[section].ColorHSV(hue, sat, val)));
    neopxlObjArr[section].show();
  }
  else {
    pxlColorUpdt(section, pixel, color);
    neopxlObjArr[section].setPixelColor(pixel, neopxlObjArr[section].gamma32(rgbw2wrgb(color)));
    neopxlObjArr[section].show();
  }
}

// turn off a single pixel
void pxlOFF(uint8_t section, uint8_t pixel) {
  pxlColorUpdt(section, pixel, 0x00000000);
  neopxlObjArr[section].setPixelColor(pixel, 0x00000000);
  neopxlObjArr[section].show();
}



// //******   PIXEL ACTIONS SECTION   ******//










// //******   STRIP ACTIONS SECTION   ******//


// lights a whole strip with the color passed as input
// gamma32 correction is applied on the color outputted to the strip
void stripColorFill(uint8_t section, uint32_t color, bool hsvFormat) {
  
  if(hsvFormat) {  
    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;
    
    for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
      pxlColorUpdt(section, pixel, color, hsvFormat);
    }
    neopxlObjArr[section].fill(neopxlObjArr[section].gamma32(neopxlObjArr[section].ColorHSV(hue, sat, val)));
    neopxlObjArr[section].show();
  }
  else {
    for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
      pxlColorUpdt(section, pixel, color);
    }
    neopxlObjArr[section].fill(neopxlObjArr[section].gamma32(rgbw2wrgb(color)));
    neopxlObjArr[section].show();
  }
}

// turn OFF all LEDs in a given strip (section)
void stripOFF(uint8_t section) {
  for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
      pxlColorUpdt(section, pixel, 0x00000000);
  }
  neopxlObjArr[section].clear();
  neopxlObjArr[section].show();  
}


// //******   STRIP ACTIONS SECTION   ******//











// //******   COLOR FORMATS SECTION   ******//



uint32_t rgbw2wrgb(uint32_t rgbwColor) {
  return((rgbwColor >> 8) | (rgbwColor << 24));
}

uint32_t wrgb2rgbw(uint32_t wrgbColor) {
  return((wrgbColor << 8) | wrgbColor >> 24);
}

uint32_t rgbw2rgb(uint32_t rgbwColor) {
  return(rgbwColor & 0xFFFFFF00);
}

// converts from RGB to HSV color space
// takes a RRGGBBWW color as input, and return a 32 bit value containing the HSV info
// return value is decomposed as follow : bits 0-7 -> VAL | bits 8-15 -> SAT | bits 16-31 -> HUE
uint32_t rgbw2hsv(uint32_t rgbwColor) {
  
  // values to combine when func will use return statement
  uint32_t hue = 0;
  uint32_t sat = 0;
  uint32_t val = 0;

  // extracting each color from the packed 32-bits value
  uint8_t red = (uint8_t)((rgbwColor & 0xFF000000) >> 24);
  uint8_t grn = (uint8_t)((rgbwColor & 0x00FF0000) >> 16);
  uint8_t blu = (uint8_t)((rgbwColor & 0x0000FF00) >>  8);

  // casting as float for hue calculation further down
  float redQ = (float)red;
  float grnQ = (float)grn;
  float bluQ = (float)blu;

  // checking which color val is Max and which is Min
  float maxColorVal = (uint8_t)(max(red, max(grn, blu)));
  float minColorVal = (uint8_t)(min(red, min(grn, blu)));

  // val calculation
  // in the case that the Value is zero, the other parameters are also at zero since the LED is OFF
  if(!maxColorVal) {
    return((hue << 16) | (sat << 8) | val);
  }
  else {
    val = maxColorVal;
  }

  // saturation calculation
  sat = (uint8_t)(((maxColorVal - minColorVal) / maxColorVal) * 255);
  // in the case of saturation 0, hue is not inmportant cause color is set to WHT
  if(!sat) {
    return((hue << 16) | (sat << 8) | val);
  }

  // hue calculation
  // normally, the first term of the equation is 60°, 
  // but in our case, the whole 360° is 65 535, so 60° is a sixth of that (10 922)
  if(red >= grn && grn >= blu) {
    hue = (uint16_t)(        10922 * ((grnQ - bluQ) / (redQ - bluQ)));
  }
  else if(grn > red && red >= blu) {
    hue = (uint16_t)(21845 - 10922 * ((redQ - bluQ) / (grnQ - bluQ)));
  }
  else if(grn >= blu && blu > red) {
    hue = (uint16_t)(21845 + 10922 * ((bluQ - redQ) / (grnQ - redQ)));
  }
  else if(blu > grn && grn > red) {
    hue = (uint16_t)(43690 - 10922 * ((grnQ - redQ) / (bluQ - redQ)));
  }
  else if(blu > red && red >= grn) {
    hue = (uint16_t)(43690 + 10922 * ((redQ - grnQ) / (bluQ - grnQ)));
  }
  else if(red >= blu && blu > grn) {
    hue = (uint16_t)(65535 - 10922 * ((bluQ - grnQ) / (redQ - grnQ)));
  }

  return(((uint32_t)hue << 16) | ((uint32_t)sat << 8) | ((uint32_t)val));
}



// //******   COLOR FORMATS SECTION   ******//










// //******   LED EFFECTS SECTION   ******//

// // func that blinks a pixel a certain color only once
// void blinkOnce(uint8_t section, uint8_t pixel, uint32_t color, uint16_t blinkTime, bool blinkState) {
  
//   // updating pxl attributes
//   stripsArrayOfPxl[section][pixel].actionOneStart = millis();
//   stripsArrayOfPxl[section][pixel].actionOneTime = blinkTime;
//   if(blinkState) {
//     stripsArrayOfPxl[section][pixel].pxlState = BLINK_ONCE;
//   }

//   // update neopxlObj
//   pxlColorOut(section, pixel, color);
// }

// // Initialize a fade action using the HSV color space for a pixel in a specific section
// // target color passed as argument is of the 0xRRGGBBWW format
// void hsvFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, int32_t fadeTime) {
  
//   // transition from RGB to HSV color space for actual and target color
//   uint32_t actualHSV = rgbw2hsv(stripsArrayOfPxl[section][pixel].rgbwColor);
//   uint32_t targetHSV = rgbw2hsv(targetRGB);

//   // extract hue, sat & val from actual and target colors
//   uint16_t actualHue = (uint16_t)((actualHSV & 0xFFFF0000) >> 16);
//   uint8_t  actualSat = (uint8_t) ((actualHSV & 0x0000FF00) >>  8);
//   uint8_t  actualVal = (uint8_t)  (actualHSV & 0x000000FF)       ;

//   uint16_t targetHue = (uint16_t)((targetHSV & 0xFFFF0000) >> 16);
//   uint8_t  targetSat = (uint8_t) ((targetHSV & 0x0000FF00) >>  8);
//   uint8_t  targetVal = (uint8_t)  (targetHSV & 0x000000FF)       ;

//   // Since hue is representative of a circle's angle, we want to find the way of rotation
//   // with the shorter length to minimize the variety of colors in the fade
//   int16_t hueDelta = 0;

//   if(targetHue - actualHue == 32768 || actualHue - targetHue == 32768) {
//     // means the target is exactly half a circle away, direction has no importance
//     hueDelta = targetHue - actualHue;
//   }
//   else if(targetHue > actualHue) {
//     if((targetHue - actualHue) < 32768) {
//       // clockwise rotation, delta is positive
//       hueDelta = targetHue - actualHue;
//     }
//     else if((targetHue - actualHue) > 32768) {
//       // counter-clockwise rotation, delta is negative
//       hueDelta = targetHue - actualHue - 65535;
//     }
//   }
//   else if(actualHue > targetHue) {
//     if((actualHue - targetHue) > 32768) {
//       // clockwise rotation, delta is positive
//       hueDelta = 65535 + targetHue - actualHue;
//     }
//     else if((actualHue - targetHue) < 32768) {
//       // counter-clockwise rotation, delta is negative
//       hueDelta = targetHue - actualHue;
//     }
//   }

//   // calculations for saturation and value deltas
//   int16_t satDelta = (int16_t)(targetSat - actualSat);
//   int16_t valDelta = (int16_t)(targetVal - actualVal);

//   // steps are calculated and expressed in ms/bit, except for hue, where the unit is ms/43bits
//   // since deltas may be negative, the values are signed 32-bit
//   int32_t hueSteps;
//   int32_t satSteps;
//   int32_t valSteps;

//   hueDelta ? hueSteps = ((float)fadeTime / hueDelta) * 43 : hueSteps = 0;
//   satDelta ? satSteps =         fadeTime / satDelta       : satSteps = 0;
//   valDelta ? valSteps =         fadeTime / valDelta       : valSteps = 0;

//   // assigning step time to pixel attributes
//   stripsArrayOfPxl[section][pixel].actionOneTime   = hueSteps;
//   stripsArrayOfPxl[section][pixel].actionTwoTime   = satSteps;
//   stripsArrayOfPxl[section][pixel].actionThreeTime = valSteps;

//   // assigning start time of each to pixel attributes
//   stripsArrayOfPxl[section][pixel].actionOneStart   = millis();
//   stripsArrayOfPxl[section][pixel].actionTwoStart   = millis();
//   stripsArrayOfPxl[section][pixel].actionThreeStart = millis();

//   // changing state of pixel and updating targetColor attribute
//   stripsArrayOfPxl[section][pixel].pxlState = HSV_FADE;
//   pxlColorUpdt(section, pixel, targetHSV, 1, 1);
// }

// // function called in the pixel iterator to update the hsv values
// void hsvFade(uint8_t section, uint8_t pixel) {

//   // extracting actual pixel color and assigning to the next HSV to output as starting point
//   uint32_t actualHSV = (stripsArrayOfPxl[section][pixel].hsvColor);
//   uint16_t nextHue   = (int16_t)((actualHSV & 0xFFFF0000) >> 16);
//   uint8_t  nextSat   = (int8_t) ((actualHSV & 0x0000FF00) >>  8);
//   uint8_t  nextVal   = (int8_t)  (actualHSV & 0x000000FF)       ;

//   if(millis() - stripsArrayOfPxl[section][pixel].actionOneStart >= absVar(stripsArrayOfPxl[section][pixel].actionOneTime) && stripsArrayOfPxl[section][pixel].actionOneTime != 0) {
//     uint16_t targetHue = (uint16_t)((stripsArrayOfPxl[section][pixel].hsvTarget & 0xFFFF0000) >> 16);
//     if(stripsArrayOfPxl[section][pixel].actionOneTime & 0x80000000) {
//       nextHue -= 43;                                                              // the steps are negative, we need to decrement
//       if(absVar(nextHue - targetHue) <= 43) {
//         stripsArrayOfPxl[section][pixel].actionOneTime = 0;                       // target is reached, no need to come back in statement again
//         nextHue = targetHue;
//       }
//       else {
//         stripsArrayOfPxl[section][pixel].actionOneStart = millis();
//       }
//     }
//     else {
//       nextHue += 43;                                                              // steps are positive, we increment
//       if(absVar(nextHue - targetHue) <= 43) {
//         stripsArrayOfPxl[section][pixel].actionOneTime = 0;                       // target is reached, no need to come back in statement again
//         nextHue = targetHue;
//       }
//       else {
//         stripsArrayOfPxl[section][pixel].actionOneStart = millis();
//       }
//     }
//   }

//   if(millis() - stripsArrayOfPxl[section][pixel].actionTwoStart >= absVar(stripsArrayOfPxl[section][pixel].actionTwoTime) && stripsArrayOfPxl[section][pixel].actionTwoTime != 0) {
//     uint8_t targetSat = (uint8_t)((stripsArrayOfPxl[section][pixel].hsvTarget & 0x0000FF00) >> 8);
//     nextColorVal(&nextSat, &stripsArrayOfPxl[section][pixel].actionTwoTime, &stripsArrayOfPxl[section][pixel].actionTwoStart, targetSat);
//   }

//   if(millis() - stripsArrayOfPxl[section][pixel].actionThreeStart >= absVar(stripsArrayOfPxl[section][pixel].actionThreeTime) && stripsArrayOfPxl[section][pixel].actionThreeTime != 0) {
//     uint8_t targetVal = (uint8_t)(stripsArrayOfPxl[section][pixel].hsvTarget & 0x000000FF);
//     nextColorVal(&nextVal, &stripsArrayOfPxl[section][pixel].actionThreeTime, &stripsArrayOfPxl[section][pixel].actionThreeStart, targetVal);
//   }
//   // outputting color to strip
//   pxlColorOut(section, pixel, (uint32_t)nextHue << 16 | (uint32_t)nextSat << 8 | (uint32_t)nextVal, 1);
// }

// // fade from color to color in the RGB space using linear interpolation between actual and target value
// // it was decided this func would not treat the white bits (8-LSB) of the input target color since it isn't ncessary
// // for a white fade, use the whtFade func
// void rgbFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, int32_t fadeTime) {

//   // extracting individual R, G & B values from the actual color attri. of the pixel
//   uint8_t actualRed = stripsArrayOfPxl[section][pixel].rgbwColor >> 24;
//   uint8_t actualGrn = stripsArrayOfPxl[section][pixel].rgbwColor >> 16;
//   uint8_t actualBlu = stripsArrayOfPxl[section][pixel].rgbwColor >>  8;

//   // extracting individual R, G & B values from the target color
//   uint8_t targetRed = targetRGB >> 24;
//   uint8_t targetGrn = targetRGB >> 16;
//   uint8_t targetBlu = targetRGB >>  8;

//   // calculating the deltas
//   int16_t redDelta = targetRed - actualRed;
//   int16_t grnDelta = targetGrn - actualGrn;
//   int16_t bluDelta = targetBlu - actualBlu;

//   // with incrementing by a bit, each step is then expressed as a number of ms/bit
//   int32_t redSteps;
//   int32_t grnSteps;
//   int32_t bluSteps;
//   redDelta ? redSteps = fadeTime / redDelta : redSteps = 0;
//   grnDelta ? grnSteps = fadeTime / grnDelta : grnSteps = 0;
//   bluDelta ? bluSteps = fadeTime / bluDelta : bluSteps = 0;

//   // updating timing attributes of the pixel, its state and target color
//   stripsArrayOfPxl[section][pixel].actionOneTime   = redSteps;
//   stripsArrayOfPxl[section][pixel].actionTwoTime   = grnSteps;
//   stripsArrayOfPxl[section][pixel].actionThreeTime = bluSteps;

//   stripsArrayOfPxl[section][pixel].actionOneStart   = millis();
//   stripsArrayOfPxl[section][pixel].actionTwoStart   = millis();
//   stripsArrayOfPxl[section][pixel].actionThreeStart = millis();

//   pxlColorUpdt(section, pixel, rgbw2rgb(targetRGB), 0, 1);
//   stripsArrayOfPxl[section][pixel].pxlState = RGB_FADE;
// }

// void rgbFade(uint8_t section, uint8_t pixel) {
  
//   // using each actual R, G & B value as starting point for the next value to output
//   uint8_t nextRed = stripsArrayOfPxl[section][pixel].rgbwColor >> 24;
//   uint8_t nextGrn = stripsArrayOfPxl[section][pixel].rgbwColor >> 16;
//   uint8_t nextBlu = stripsArrayOfPxl[section][pixel].rgbwColor >>  8;

//   if(millis() - stripsArrayOfPxl[section][pixel].actionOneStart >= absVar(stripsArrayOfPxl[section][pixel].actionOneTime) && stripsArrayOfPxl[section][pixel].actionOneTime != 0) {
//     uint8_t targetRed = stripsArrayOfPxl[section][pixel].rgbwTarget >> 24;
//     nextColorVal(&nextRed, &stripsArrayOfPxl[section][pixel].actionOneTime, &stripsArrayOfPxl[section][pixel].actionOneStart, targetRed);
//   }

//   if(millis() - stripsArrayOfPxl[section][pixel].actionTwoStart >= absVar(stripsArrayOfPxl[section][pixel].actionTwoTime) && stripsArrayOfPxl[section][pixel].actionTwoTime != 0) {
//     uint8_t targetGrn = stripsArrayOfPxl[section][pixel].rgbwTarget >> 16;
//     nextColorVal(&nextGrn, &stripsArrayOfPxl[section][pixel].actionTwoTime, &stripsArrayOfPxl[section][pixel].actionTwoStart, targetGrn);
//   }

//   if(millis() - stripsArrayOfPxl[section][pixel].actionThreeStart >= absVar(stripsArrayOfPxl[section][pixel].actionThreeTime) && stripsArrayOfPxl[section][pixel].actionThreeTime != 0) {
//     uint8_t targetBlu = stripsArrayOfPxl[section][pixel].rgbwTarget >> 8;
//     nextColorVal(&nextBlu, &stripsArrayOfPxl[section][pixel].actionThreeTime, &stripsArrayOfPxl[section][pixel].actionThreeStart, targetBlu);
//   }
//   // outputting to strip
//   pxlColorOut(section, pixel, wrgb2rgbw(neopxlObjArr[section].Color(nextRed, nextGrn, nextBlu, 0)));
// }



// //********   LED EFFECTS SECTION   ********//











// //********    LED SCENES SECTION    ********//



// // Initialize a strip/section to start sparkle. Called once in main
// void sparkleInit(uint8_t section) {
//   // randomly chooses first pxl to turn ON/OFF
//   uint8_t sparklePxl = random(neopxlObjArr[section].numPixels());
//   blinkOnce(section, sparklePxl, sunColor, 50, 0);
//   stripsArrayOfPxl[section][sparklePxl].pxlState = SPARKLE;
// }

// // Create a sparkling effect for a whole section (strip)
// void sparkleSct(uint8_t section, uint8_t pixel) {

//   if(millis() - stripsArrayOfPxl[section][pixel].actionOneStart >= absVar(stripsArrayOfPxl[section][pixel].actionOneTime)) {
//     pxlOFF(section, pixel);
//     stripsArrayOfPxl[section][pixel].pxlState = IDLE;
    
//     uint8_t nextSparklePxl = random(neopxlObjArr[pixel].numPixels());
    
//     // if the rand generator gives the same result, do it until it is different
//     while(nextSparklePxl == pixel) {
//       nextSparklePxl = random(neopxlObjArr[pixel].numPixels());
//     }

//     // turn ON next pixel and actualize attributes
//     blinkOnce(section, nextSparklePxl, sunColor, stripsArrayOfPxl[section][pixel].actionOneTime, 0);
//     stripsArrayOfPxl[section][nextSparklePxl].pxlState = SPARKLE;
//   }
// }

//********    LED SCENES SECTION    ********//


//***********    LOCAL FUNCS DEFINITION    ***********//


/// @brief Removes pixels in any given section. It's done in a bare-metal kind of way,
///        which is by shifting affected bytes in the heap. The number of bytes to shift
///        is dependent upon block size (pixel_info_t byte size) and how many blocks
///        there are to shift. When reaching the new address of ptrPxlInfo, all bytes
///        are then overwritten with 0x00 as a way to reset them for future use.
///        Note that this 0x00 byte reset isn't executed when erasing the last sct
///        because it will be updated when the next one is created
/// @param sctID Section from which to remove pixels
/// @param newPxlCount Updated pixel count (must be lower than original)
void removingPxlsFromSct(uint8_t sctID, uint8_t newPxlCount) {  
  
  uint8_t  freedHeapBlocks = sectionInfoArr[sctID].pxlCount - newPxlCount;     // Expressed in terms of a number of pixel_info_t OBJ
  uint16_t freedHeapBytes  = freedHeapBlocks * sizeof(pixel_info_t) * BYTE_SIZE;

  byte *heapEraseAddr           = (byte*)ptrPxlInfo - freedHeapBytes;
  byte *heapOverWriteDestAddr   = (byte*)(arrPtrPxlInfo[sctID] + newPxlCount);
  byte *heapOverWriteSourceAddr = (byte*)(arrPtrPxlInfo[sctID] + newPxlCount) + freedHeapBytes;

  uint8_t pxlInfoToShift = 0;

  ptrPxlInfo = (pixel_info_t*)heapEraseAddr;

  // Calculating number of pixel_info_t obj to shift in heap and then to how many bytes that amounts
  // While iterating, also changing the ptr addr contained in the array of ptr to pixel info
  for(uint8_t i = sctID + 1; i < sctIndexTracker; i++) {
    pxlInfoToShift += sectionInfoArr[i].pxlCount;
    arrPtrPxlInfo[i] -= freedHeapBlocks;
  }
  uint16_t bytesToShift = pxlInfoToShift * sizeof(pixel_info_t) * BYTE_SIZE;

  // This where the overwriting is done
  while(bytesToShift) {
    *heapOverWriteDestAddr = *heapOverWriteSourceAddr;
    if(heapOverWriteSourceAddr == heapEraseAddr) {
      *heapEraseAddr = 0x00;
      heapEraseAddr += BYTE_SIZE;
    }
    heapOverWriteDestAddr += BYTE_SIZE;
    heapOverWriteSourceAddr += BYTE_SIZE;
    bytesToShift--;
  }

  // Updating length of neopixel Obj, section info matrix & board infos
  neopxlObjArr[sctID].updateLength((uint16_t)newPxlCount);
  pixelsMgmtRemove(sectionInfoArr[sctID].pxlCount - newPxlCount);
  sectionInfoArr[sctID].pxlCount = newPxlCount;
}


/// @brief Adds pixels in any given section. It's done in a bare-metal kind of way,
///        which is by shifting affected bytes in the heap. The number of bytes to shift
///        is dependent upon block size (pixel_info_t byte size) and how many blocks
///        there are to shift. Once the last address of the new pixel blocks is reached,
///        existing bytes are overwritten with 0x00 to prevent any strange behavior.
/// @param sctID Section from which to add pixels
/// @param newPxlCount Updated pixel count (must be higher than original)
void addingPxlsToSct(uint8_t sctID, uint8_t newPxlCount) {

  uint8_t  toBeUsedHeapBlocks = newPxlCount - sectionInfoArr[sctID].pxlCount;                 // Expressed in terms of a number of pixel_info_t OBJ
  uint16_t toBeUsedHeapBytes  = toBeUsedHeapBlocks * sizeof(pixel_info_t) * BYTE_SIZE;

  byte *heapOverWriteDestAddr   = (byte*)ptrPxlInfo + toBeUsedHeapBytes - BYTE_SIZE;          // This addr should be the last of the heap where there will be data
  byte *heapOverWriteSourceAddr = (byte*)ptrPxlInfo - BYTE_SIZE;                              // Last addr where there is actual info
  byte *heapMemClearAddr        = (byte*)(arrPtrPxlInfo[sctID] + newPxlCount) - BYTE_SIZE;    // First addr where to erase the bytes to make room

  uint8_t pxlInfoToShift = 0;

  ptrPxlInfo = (pixel_info_t*)heapOverWriteDestAddr + BYTE_SIZE;

  // Calculating number of pixel_info_t obj to shift in heap and then to how many bytes that amounts
  for(uint8_t i = sctID + 1; i < sctIndexTracker; i++) {
    pxlInfoToShift += sectionInfoArr[i].pxlCount;
    arrPtrPxlInfo[i] += toBeUsedHeapBlocks;
  }
  uint16_t bytesToShift = pxlInfoToShift * sizeof(pixel_info_t) * BYTE_SIZE;

  // This where the overwriting is done
  while(bytesToShift) {
    *heapOverWriteDestAddr = *heapOverWriteSourceAddr;
    if(heapOverWriteSourceAddr == heapMemClearAddr) {
      *heapMemClearAddr = 0x00;
      heapMemClearAddr -= BYTE_SIZE;
    }
    heapOverWriteDestAddr -= BYTE_SIZE;
    heapOverWriteSourceAddr -= BYTE_SIZE;
    bytesToShift--;
  }

  // Updating the pixel info array with the newly created LEDs
  for(uint8_t pxlNbr = sectionInfoArr[sctID].pxlCount; pxlNbr < newPxlCount; pxlNbr++) {
      (arrPtrPxlInfo[sctID] + pxlNbr)->pxlSctID = sctID;
      (arrPtrPxlInfo[sctID] + pxlNbr)->pxlID    = pxlNbr;
      (arrPtrPxlInfo[sctID] + pxlNbr)->pxlState = IDLE;
  }

  // Updating length of neopixel Obj, section info matrix & board infos
  neopxlObjArr[sctID].updateLength((uint16_t)newPxlCount);
  pixelsMgmtAdd(newPxlCount - sectionInfoArr[sctID].pxlCount);
  sectionInfoArr[sctID].pxlCount = newPxlCount;
}

//***********    LOCAL FUNCS DEFINITION    ***********//


/*
// Converts single 8-bit color values into a packed 32-bit, and then set the pixel color
void setPixel(Adafruit_NeoPixel &neopxlObj, uint16_t pixel, uint8_t red, uint8_t grn, uint8_t blu, uint8_t wht) {
  neopxlObj.setPixelColor(pixel, neopxlObj.Color(red, grn, blu, wht));
}

// decomposes a 32-bits 0xRRGGBBWW color scheme in separate 8-bits value and stores in an array
void colorDecomposer(uint8_t destArray[], uint32_t longColor, uint8_t startIndx) {
  destArray[startIndx]     = uint8_t((longColor & 0xFF000000) >> 24);   // Red LED
  destArray[startIndx + 1] = uint8_t((longColor & 0x00FF0000) >> 16);   // Grn LED
  destArray[startIndx + 2] = uint8_t((longColor & 0x0000FF00) >> 8);    // Blu LED
  destArray[startIndx + 3] = uint8_t (longColor & 0x000000FF);          // Wht LED
}
*/

// Setup section pixel light-up indicator
// //**debug**//
// for(uint8_t i = 0; i < pxlCount; i++) {
//   pxlColorOut(sctIndexTracker, i, 0x3B659C00);
// }
// //**debug**//


// Clear section pixel light-up indicator
// //**debug**//
// stripOFF(section);

// for(uint8_t i = 0; i < sectionInfoArr[section].pxlCount; i++) {
//   pxlColorOut(section, i, (arrPtrPxlInfo[section])->rgbwColor);
// }
// //**debug**//


// Edit section REMOVING pixel light-up indicator
// //**debug**//
// stripOFF(section);
// //**debug**//

// removingPxlsFromSct(section, newPxlCount);

// //**debug**//
// for(uint8_t i = 0; i < sectionInfoArr[section].pxlCount; i++) {
//   pxlColorOut(section, i, 0xFC7F0300);
// }
// //**debug**//


// Edit section ADDING pixel light-up indicator
// //**debug**//
// stripOFF(section);

// for(uint8_t i = 0; i < sectionInfoArr[section].pxlCount; i++) {
//   pxlColorOut(section, i, 0x0000005F);
// }
// //**debug**//