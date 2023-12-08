/*
Created on: December 7 2023
Author : J-C Rodrigue
Description : Implementation of all scenes
*/

#include "Scenes.h"

//**********    GLOBAL FUNC DEFINITION   ************//


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


//**********    GLOBAL FUNC DEFINITION   ************//