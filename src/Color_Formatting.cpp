/*
Created on: december 7 2023
Author : J-C Rodrigue
Description : All manipulations related to reformatting colors (8-bit to 32-bit, RGB to HSV, WRGB to RGBW, etc.)
*/

#include "Color_Formatting.h"



//**************************************************************    GLOBAL FUNC DEFINITION   **************************************************************//


/// @brief Re-formats a 32-bit RGBW color scheme to a 32-bit WRGB one
/// @param rgbwColor 32-bit RGBW color to reformat
/// @return Resulting 32-bit WRGB reformatted color
uint32_t rgbw2wrgb(uint32_t rgbwColor) {
  return((rgbwColor >> 8) | (rgbwColor << 24));
}


/// @brief Re-formats a 32-bit WRGB color scheme to a 32-bit RGBW one
/// @param wrgbColor 32-bit WRGB color to reformat
/// @return Resulting 32-bit RGBW reformatted color
uint32_t wrgb2rgbw(uint32_t wrgbColor) {
  return((wrgbColor << 8) | wrgbColor >> 24);
}


/// @brief Re-formats a 32-bit RGBW color scheme to a 32-bit RGB one
/// @param wrgbColor 32-bit RGBW color to reformat
/// @return Resulting 32-bit RGB reformatted color
uint32_t rgbw2rgb(uint32_t rgbwColor) {
  return(rgbwColor & 0xFFFFFF00);
}


/// @brief Convert a 32-bit RGB or RGBW formatted color from the RGB
///        space to the HSV space.
/// @param rgbwColor 32-bit RGB or RGBW color to reformat
/// @return 32-bit HSV formatted color with each bits representing the following:
///          31                              15              7             0         
///          0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
///         | - - - - -  H U E  - - - - - - | - - S A T - - | - - V A L - - |
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

//**************************************************************    GLOBAL FUNC DEFINITION   **************************************************************//


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
