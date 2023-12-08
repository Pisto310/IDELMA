/*
Created on: december 7 2023
Author : J-C Rodrigue
Description : All manipulations related to reformatting colors (8-bit to 32-bit, RGB to HSV, WRGB to RGBW, etc.)
*/

#ifndef COLOR_FORMATTING_H_
#define COLOR_FORMATTING_H_

#include "Board.h"


//**********    GLOBAL FUNC DECLARATION   *************//

uint32_t rgbw2wrgb(uint32_t rgbwColor);
uint32_t wrgb2rgbw(uint32_t wrgbColor);
uint32_t rgbw2rgb(uint32_t rgbwColor);
uint32_t rgbw2hsv(uint32_t rgbwColor);

//**********    GLOBAL FUNC DECLARATION   *************//


#endif /*COLOR_FORMATTING_H_*/