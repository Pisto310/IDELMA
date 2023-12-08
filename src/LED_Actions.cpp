/*
Created on: december 7 2023
Author : J-C Rodrigue
Description : Lighting actions related to LEDs (No scenes, only single actions)
*/

#include "LED_Actions.h"


//**********    GLOBAL FUNC DEFINITION   ************//


// void pixelActionsHandler(void) {
//   for(uint8_t sct = 0; sct < sctIndexTracker; sct++) {
//     for(uint8_t pxl = 0; pxl < sctsMetaDatasArr[sct].pxlCount; pxl++) {
//       switch ((pxlMetaDataPtrArr[sct] + pxl)->pxlState) {
      
//       default:
//         // FSM comes here if the pixel state is IDLE
//         break;
//       }
//     }
//   }
// }


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


//**********    GLOBAL FUNC DEFINITION   ************//
