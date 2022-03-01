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

// uint32_t sunColor = rgbw2wrgb(0xFFFF1400);

//**********    LOCAL VARIABLES DECLARATION   ************//






//**********    LOCAL FUNCTIONS DECLARATION   ************//

void sectionsUsageUpdt();
void pixelsUsageUpdt(uint8_t pxlsUsed);

// Put in a separate lib?
uint32_t rgbw2wrgb(uint32_t rgbwColor);
uint32_t wrgb2rgbw(uint32_t wrgbColor);
uint32_t rgbw2rgb(uint32_t rgbwColor);
uint32_t rgbw2hsv(uint32_t rgbwColor);

//**********    LOCAL FUNCTIONS DECLARATION   ************//





void createSection(uint8_t nbrOfLEDs, uint8_t maxBrightness = 50) {

  if(remainingHeapSpace(nbrOfLEDs) && remainingSctsPins()) {
    
    static uint8_t sectionIndex = 0;

    // Everything related to pxlInfo struct is done here
    arrPtrPxlInfo[sectionIndex] = ptrPxlInfo;
    ptrPxlInfo += nbrOfLEDs;

    for(uint8_t pxlNbr = 0; pxlNbr < nbrOfLEDs; pxlNbr++) {
      (arrPtrPxlInfo[sectionIndex] + pxlNbr)->pxlSct   = sectionIndex;
      (arrPtrPxlInfo[sectionIndex] + pxlNbr)->pxlNbr   = pxlNbr;
      (arrPtrPxlInfo[sectionIndex] + pxlNbr)->pxlState = IDLE;
    }

    // Everything related to the neopxl object itself is done below
    neopxlObjArr[sectionIndex].updateLength((uint16_t)nbrOfLEDs);
    // needs to be done for each instanciated neopxlObj
    neopxlObjArr[sectionIndex].begin();
    neopxlObjArr[sectionIndex].setBrightness(maxBrightness);

    // updating sections and pixels usage
    sectionsMgmtUpdt();
    pixelsMgmtUpdt(nbrOfLEDs);
    sectionIndex++;
  }

  // else {
  //   Serial.println("Can't create anymore sections");
  // }
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

// void neopxlObjSetUp(Adafruit_NeoPixel &neopxlObj, Adafruit_NeoPixel neopxlArr[], uint8_t *ptrToSctTracker, uint8_t maxBrightness, uint32_t startColor) {
//   // loop to fill each pixel info in the array of strips
//   for(uint8_t index = 0; index < neopxlObj.numPixels(); index++) {
//     stripsArrayOfPxl[*ptrToSctTracker][index].pxlSct   = *ptrToSctTracker;
//     stripsArrayOfPxl[*ptrToSctTracker][index].pxlNbr   = index;
//     stripsArrayOfPxl[*ptrToSctTracker][index].pxlState = IDLE;
//   }
//   // adding the initialized Neopixel object to the array
//   neopxlArr[*ptrToSctTracker] = neopxlObj;

//   // done here since it needs to be done for each instanciated neopxlObj
//   neopxlObj.begin();
//   neopxlObj.setBrightness(maxBrightness);
//   if(startColor) {
//     stripColorFill(*ptrToSctTracker, startColor);
//   }
//   else {
//     stripOFF(*ptrToSctTracker);
//   }
  
//   // increment the section count after every function call
//   ++*ptrToSctTracker;
// }

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








// // //******   PIXEL ACTIONS SECTION   ******//



// // Function that can be called to update either one of the color attribute of a pixel
// // If the HSV color format is passed, the associated bool should be passed as True
// // Func updates the actual color attr by default. For updating the target color attr, bool should be passed as True
// void pxlColorUpdt(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat, bool targetUpdt) {
//   if(hsvFormat) {

//     uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
//     uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
//     uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;

//     if(targetUpdt) {
//       stripsArrayOfPxl[section][pixel].rgbwTarget = wrgb2rgbw(neopxlObjArr[section].ColorHSV(hue, sat, val));
//       stripsArrayOfPxl[section][pixel].hsvTarget = color;
//     }
//     else {
//       stripsArrayOfPxl[section][pixel].rgbwColor = wrgb2rgbw(neopxlObjArr[section].ColorHSV(hue, sat, val));
//       stripsArrayOfPxl[section][pixel].hsvColor = color;
//     }
//   }

//   else {
//     if(targetUpdt) {
//       stripsArrayOfPxl[section][pixel].rgbwTarget = color;
//       stripsArrayOfPxl[section][pixel].hsvTarget = rgbw2hsv(color);
//     }
//     else {
//       stripsArrayOfPxl[section][pixel].rgbwColor = color;
//       stripsArrayOfPxl[section][pixel].hsvColor = rgbw2hsv(color);
//     }
//   }
// }

// // simple function to output a chosen color to a pixel
// // color can be of either HSV or RGB format
// // gamma32 correction is applied on the color outputted to the strip
// void pxlColorOut(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat) {
//   if(hsvFormat) {
    
//     uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
//     uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
//     uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;

//     pxlColorUpdt(section, pixel, color, hsvFormat);
//     neopxlObjArr[section].setPixelColor(pixel, neopxlObjArr[section].gamma32(neopxlObjArr[section].ColorHSV(hue, sat, val)));
//     neopxlObjArr[section].show();
//   }
//   else {
//     pxlColorUpdt(section, pixel, color);
//     neopxlObjArr[section].setPixelColor(pixel, neopxlObjArr[section].gamma32(rgbw2wrgb(color)));
//     neopxlObjArr[section].show();
//   }
// }

// // turn off a single pixel
// void pxlOFF(uint8_t section, uint8_t pixel) {
//   pxlColorUpdt(section, pixel, 0x00000000);
//   neopxlObjArr[section].setPixelColor(pixel, 0x00000000);
//   neopxlObjArr[section].show();
// }

// void pxlIterator(uint8_t sctCount) {
//   for(uint8_t section = 0; section < sctCount; section++) {
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



// //******   PIXEL ACTIONS SECTION   ******//










// //******   STRIP ACTIONS SECTION   ******//


// // lights a whole strip with the color passed as input
// // gamma32 correction is applied on the color outputted to the strip
// void stripColorFill(uint8_t section, uint32_t color, bool hsvFormat) {
//   if(hsvFormat) {
    
//     uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
//     uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
//     uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;
    
//     for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
//       pxlColorUpdt(section, pixel, color, hsvFormat);
//     }
//     neopxlObjArr[section].fill(neopxlObjArr[section].gamma32(neopxlObjArr[section].ColorHSV(hue, sat, val)));
//     neopxlObjArr[section].show();
//   }
//   else {
//     for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
//       pxlColorUpdt(section, pixel, color);
//     }
//     neopxlObjArr[section].fill(neopxlObjArr[section].gamma32(rgbw2wrgb(color)));
//     neopxlObjArr[section].show();
//   }
// }

// // turn OFF all LEDs in a given strip (section)
// void stripOFF(uint8_t section) {
//   for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
//       pxlColorUpdt(section, pixel, 0x00000000);
//   }
//   neopxlObjArr[section].clear();
//   neopxlObjArr[section].show();  
// }


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