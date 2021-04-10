/*
The LED pixel strips are instanciated here and scenes are also expanded upon
*/

// My set-up, when using the object's setPixelColor method is set as a GRBW string

#include "SK6812.h"


//**********    GLOBAL VARIABLES DECLARATION   ************//

Adafruit_NeoPixel sctZero = Adafruit_NeoPixel(LED_COUNT_SCT_1, PIN_SCT_1, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel sctTwo = Adafruit_NeoPixel(LED_COUNT_SCT_2, PIN_SCT_2, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel sctSix = Adafruit_NeoPixel(LED_COUNT_SCT_6, PIN_SCT_6, NEO_GRBW + NEO_KHZ800);

pixelInfo stripsArrayOfPxl[SCT_COUNT][LED_COUNT_MAX];

Adafruit_NeoPixel neopxlObjArr[SCT_COUNT];

//**********    GLOBAL VARIABLES DECLARATION   ************//




//**********    LOCAL VARIABLES DECLARATION   ************//

uint32_t sunColor = rgbw2wrgb(0xFFFF1400);

//**********    LOCAL VARIABLES DECLARATION   ************//


//******   BASIC FUNCS SECTION   ******//

void neopxlObjSetUp(Adafruit_NeoPixel &neopxlObj, Adafruit_NeoPixel neopxlArr[], uint8_t *ptrToSctTracker, uint8_t maxBrightness, uint32_t startColor) {
  // loop to fill each pixel info in the array of strips
  for(uint8_t index = 0; index < neopxlObj.numPixels(); index++) {
    stripsArrayOfPxl[*ptrToSctTracker][index].pxlSct    = *ptrToSctTracker;
    stripsArrayOfPxl[*ptrToSctTracker][index].pxlNbr    = index;
    stripsArrayOfPxl[*ptrToSctTracker][index].pxlState  = IDLE;
    stripsArrayOfPxl[*ptrToSctTracker][index].rgbwColor = startColor;
    stripsArrayOfPxl[*ptrToSctTracker][index].hsvColor  = rgbw2hsv(startColor);
  }
  // done here since it needs to be done for each instanciated neopxlObj
  neopxlObj.begin();
  neopxlObj.setBrightness(maxBrightness);
  if(startColor) {
    neopxlObj.fill(rgbw2wrgb(startColor));
    neopxlObj.show();
  }
  else {
    neopxlObj.show();
  }
  // adding the initialized Neopixel object to the array
  neopxlArr[*ptrToSctTracker] = neopxlObj;
  
  // increment the section count after every function call
  ++*ptrToSctTracker;
}

void pxlIterator(uint8_t sctCount) {
  for(uint8_t i = 0; i < sctCount; i++) {
    for(uint8_t j = 0; j < neopxlObjArr[i].numPixels(); j++) {
      switch (stripsArrayOfPxl[i][j].pxlState) {
      case HSV_FADE:
        if(stripsArrayOfPxl[i][j].hsvColor != stripsArrayOfPxl[i][j].hsvTarget) {
          hsvFade(stripsArrayOfPxl[i][j]);
        }
        else {
          stripsArrayOfPxl[i][j].pxlState = IDLE;
          Serial.println("here");
        }
        break;

      case SPARKLE:
        sparkleSct(stripsArrayOfPxl[i][j]);
        break;
      
      default:
        stripsArrayOfPxl[i][j].pxlState = IDLE;       // check if statement is useful
        break;
      }
    }
  }
}

uint32_t rgbw2wrgb(uint32_t rgbwColor) {
  return((rgbwColor >> 8) | (rgbwColor << 24));
}

uint32_t wrgb2rgbw(uint32_t wrgbColor) {
  return((wrgbColor << 8) | wrgbColor >> 24);
}

// converts from RGB to HSV color space
// takes a RRGGBBWW color as input, and return a 32 bit value containing the HSV info
// return value is decomposed as follow : bits 0-7 -> VAL | bits 8-15 -> SAT | bits 16-31 -> HUE
uint32_t rgbw2hsv(uint32_t color) {
  
  // values to combine when func will use return statement
  uint16_t hue = 0;
  uint8_t  sat = 0;
  uint8_t  val = 0;

  // extracting each color from the packed 32-bits value and converting to 16-bit val
  uint16_t red = (uint16_t)((color & 0xFF000000) >> 16) | (uint16_t)((color & 0xFF000000) >> 24);
  uint16_t grn = (uint16_t)((color & 0x00FF0000) >>  8) | (uint16_t)((color & 0x00FF0000) >> 16);
  uint16_t blu = (uint16_t) (color & 0x0000FF00)        | (uint16_t)((color & 0x0000FF00) >>  8);

  uint8_t maxColorVal = max((uint8_t)(red & 0x00FF), max((uint8_t)(grn & 0x00FF), (uint8_t)(blu & 0x00FF)));
  uint8_t minColorVal = min((uint8_t)(red & 0x00FF), min((uint8_t)(grn & 0x00FF), (uint8_t)(blu & 0x00FF)));

  uint8_t delta = maxColorVal - minColorVal;

  // section to calculate the hue
  // normally, the first term of the equation is 60°, 
  // but in our case, the whole 360° is 65 535, so 60° is a sixth of that (10 922)
  if(red >= grn && grn >= blu) {
    hue = (uint16_t)(10922 * (((float)grn - (float)blu) / ((float)red - (float)blu)));
  }
  else if(grn > red && red >= blu) {
    hue = (uint16_t)(10922 * (2 - ((float)red - (float)blu) / ((float)grn - (float)blu)));
  }
  else if(grn >= blu && blu > red) {
    hue = (uint16_t)(10922 * (2 + ((float)blu - (float)red) / ((float)grn - (float)red)));
  }
  else if(blu > grn && grn > red) {
    hue = (uint16_t)(10922 * (4 - ((float)grn - (float)red) / ((float)blu - (float)red)));
  }
  else if(blu > red && red >= grn) {
    hue = (uint16_t)(10922 * (4 + ((float)red - (float)grn) / ((float)blu - (float)grn)));
  }
  else if(red >= blu && blu > grn) {
    hue = (uint16_t)(10922 * (6 - ((float)blu - (float)grn) / ((float)red - (float)grn)));
  }

  // saturation calculation
  if(maxColorVal != 0) {
    sat = (uint8_t)((float)delta / (float)maxColorVal * 255);
  }

  // val calculation
  val = maxColorVal;

  return(((uint32_t)hue << 16) | ((uint32_t)sat << 8) | ((uint32_t)val));
}

//******   BASIC FUNCS SECTION   ******//





//******   LED EFFECTS SECTION   ******//





// Initialize a strip/section to start sparkle. Called once in main
void sparkleInit(uint8_t section) {
  // randomly chooses first pxl to turn ON/OFF
  uint8_t sparklePxl = random(neopxlObjArr[section].numPixels());

  // updating pxl attributes
  stripsArrayOfPxl[section][sparklePxl].actionOneStart = millis();
  stripsArrayOfPxl[section][sparklePxl].actionOneTime = 50;
  stripsArrayOfPxl[section][sparklePxl].pxlState = SPARKLE;
  stripsArrayOfPxl[section][sparklePxl].rgbwColor = sunColor;

  // update neopxlObj
  neopxlObjArr[section].setPixelColor(sparklePxl, sunColor);
  neopxlObjArr[section].show();
}

// Create a sparkling effect for a whole section (strip)
void sparkleSct(pixelInfo pixel) {

  if(millis() - pixel.actionOneStart >= absVar(pixel.actionOneTime)) {

    // turn OFF pixel and actualize pxl attributes
    neopxlObjArr[pixel.pxlSct].setPixelColor(pixel.pxlNbr, 0);
    stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].pxlState = IDLE;
    stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].rgbwColor = 0;
    
    uint8_t nextSparklePxl = random(neopxlObjArr[pixel.pxlSct].numPixels());
    
    // if the rand generator gives the same result, do it until the result is different
    while(nextSparklePxl == pixel.pxlNbr) {
      nextSparklePxl = random(neopxlObjArr[pixel.pxlSct].numPixels());
    }

    // turn ON next pixel and actualize attributes
    neopxlObjArr[pixel.pxlSct].setPixelColor(nextSparklePxl, sunColor);
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].pxlState = SPARKLE;
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].rgbwColor = sunColor;
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].actionOneStart = millis();
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].actionOneTime = 50;
    neopxlObjArr[pixel.pxlSct].show();
  }
}

// Initialize a fade action using the HSV color space for a pixel in a specific section
// target color passed as argument is of the 0xRRGGBBWW format
void hsvFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, uint32_t fadeTime) {
  
  // transition from RGB to HSV color space for actual and target color
  uint32_t actualHSV = rgbw2hsv(stripsArrayOfPxl[section][pixel].rgbwColor);
  uint32_t targetHSV = rgbw2hsv(targetRGB);

  //Serial.println(wrgb2rgbw(neopxlObjArr[section].getPixelColor(pixel)));
  //Serial.println(actualHSV);

  // extract hue, sat & val from actual and target colors
  uint16_t actualHue = (uint16_t)((actualHSV & 0xFFFF0000) >> 16);
  uint8_t  actualSat = (uint8_t) ((actualHSV & 0x0000FF00) >>  8);
  uint8_t  actualVal = (uint8_t)  (actualHSV & 0x000000FF)       ;

  /*
  Serial.println(actualHue);
  Serial.println(actualSat);
  Serial.println(actualVal);

  Serial.println(targetHSV);
  */

  uint16_t targetHue = (uint16_t)((targetHSV & 0xFFFF0000) >> 16);
  uint8_t  targetSat = (uint8_t) ((targetHSV & 0x0000FF00) >>  8);
  uint8_t  targetVal = (uint8_t)  (targetHSV & 0x000000FF)       ;

  /*
  Serial.println(targetHue);
  Serial.println(targetSat);
  Serial.println(targetVal);

  delay(2000);
  */

  // Since hue is representative of a circle's angle, we want to find the way of rotation
  // with the shorter length to minimize the variety of colors in the fade
  int16_t hueDelta = 0;

  if(targetHue - actualHue == 32768 || actualHue - targetHue == 32768) {
    // means the target is exactly half a circle away, direction has no importance
    hueDelta = targetHue - actualHue;
  }
  else if(targetHue > actualHue) {
    if((targetHue - actualHue) < 32768) {
      // clockwise rotation
      hueDelta = targetHue - actualHue;
    }
    else if((targetHue - actualHue) > 32768) {
      // counter-clockwise rotation
      hueDelta = (targetHue - actualHue) - 65535;
    }
  }
  else if(actualHue > targetHue) {
    if((actualHue - targetHue) > 32768) {
      // clockwise rotation
      hueDelta = actualHue - targetHue;
    }
    else if((actualHue - targetHue) < 32768) {
      // counter-clockwise rotation
      hueDelta = (actualHue + targetHue) - 65535;
    }
  }

  // calculations for saturation and value deltas
  int16_t satDelta = (int16_t)targetSat - (int16_t)actualSat;
  int16_t valDelta = (int16_t)targetVal - (int16_t)actualVal;

  // steps are calculated and expressed in ms/bit, except for hue, where the unit is ms/43bits
  // since deltas may be negative, the values are signed 32-bit
  int32_t hueStep;
  hueDelta ? hueStep = ((float)fadeTime / hueDelta) * 43 : hueStep = 0; 
  int32_t satStep;
  satDelta ? satStep = (float)fadeTime / satDelta : satStep = 0;
  int32_t valStep;
  valDelta ? valStep = (float)fadeTime / valDelta : valStep = 0;

  /*
  Serial.println(hueStep);
  Serial.println(fadeTime);
  Serial.println(hueDelta);
  */

  /*
  Serial.println(satStep);
  Serial.println(fadeTime);
  Serial.println(satDelta);
  */

  /*
  Serial.println(valStep);
  Serial.println(fadeTime);
  Serial.println(valDelta);
  */

  // assigning step time to pixel attributes
  stripsArrayOfPxl[section][pixel].actionOneTime   = hueStep;
  stripsArrayOfPxl[section][pixel].actionTwoTime   = satStep;
  stripsArrayOfPxl[section][pixel].actionThreeTime = valStep;

  // assigning start time of each to pixel attributes
  stripsArrayOfPxl[section][pixel].actionOneStart   = millis();
  stripsArrayOfPxl[section][pixel].actionTwoStart   = millis();
  stripsArrayOfPxl[section][pixel].actionThreeStart = millis();

  // changing state of pixel and updating targetColor attribute
  stripsArrayOfPxl[section][pixel].pxlState = HSV_FADE;
  stripsArrayOfPxl[section][pixel].hsvTarget = targetHSV;
}

// function called in the pixel iterator to update the hsv values
void hsvFade(pixelInfo pixel) {

  // extracting actual pixel color and assigning to the next HSV to output as starting point
  uint32_t actualHSV = (stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].hsvColor);
  uint16_t nextHue   = (int16_t)((actualHSV & 0xFFFF0000) >> 16);
  uint8_t  nextSat   = (int8_t) ((actualHSV & 0x0000FF00) >>  8);
  uint8_t  nextVal   = (int8_t)  (actualHSV & 0x000000FF)       ;
  
  //Serial.println(pixel.rgbwColor);
  /*
  Serial.println(nextHue);
  Serial.println(nextSat);
  Serial.println(nextVal);(int8_t)
  */

  if(millis() - pixel.actionOneStart >= absVar(pixel.actionOneTime) && pixel.actionOneTime != 0) {
    uint16_t targetHue = (int16_t)((pixel.hsvTarget & 0xFFFF0000) >> 16);
    if(pixel.actionOneTime & 0x80000000) {
      nextHue -= 43;                                                              // the steps are negative, we need to decrement
      if(nextHue <= targetHue) {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionOneTime = 0;           // target is reached, no need to come back in statement again
        nextHue = targetHue;
      }
      else {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionOneStart = millis();
      }
    }
    else {
      nextHue += 43;                                                              // steps are positive, we increment
      if(nextHue >= targetHue) {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionOneTime = 0;           // target is reached, no need to come back in statement again
        nextHue = targetHue;
      }
      else {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionOneStart = millis();
      }
    }
  }

  if(millis() - pixel.actionTwoStart >= absVar(pixel.actionTwoTime) && pixel.actionTwoTime != 0) {
    uint8_t targetSat = (int8_t)((pixel.hsvTarget & 0x0000FF00) >> 8);
    if(pixel.actionTwoTime & 0x80000000) {
      nextSat -= 1;
      if(nextSat <= targetSat) {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionTwoTime = 0;
        nextSat = targetSat;
      }
      else {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionTwoStart = millis();
      }
    }
    else {
      nextSat += 1;
      if(nextSat >= targetSat) {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionTwoTime = 0;
        nextSat = targetSat;
      }
      else {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionTwoStart = millis();
      }
    }
  }

  if(millis() - pixel.actionThreeStart >= absVar(pixel.actionThreeTime) && pixel.actionThreeTime != 0) {
    uint8_t targetVal = (int8_t)(pixel.hsvTarget & 0x000000FF);
    if(pixel.actionThreeTime & 0x80000000) {
      nextVal -= 1;
      if(nextVal <= targetVal) {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionThreeTime = 0;
        nextVal = targetVal;
      }
      else {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionThreeStart = millis();
      }
    }
    else {
      nextVal += 1;
      if(nextVal >= targetVal) {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionThreeTime = 0;
        nextVal = targetVal;
      }
      else {
        stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionThreeStart = millis();
      }
    }
  }
  // outputting color to strip

  /*
  Serial.print((actualHSV & 0xFFFF0000) >> 16);
  Serial.print("\t");
  Serial.print(nextHue);
  Serial.print("\t");
  Serial.print((nextHue * 1530L + 32768) / 65536);
  Serial.print("\t");
  Serial.print((actualHSV & 0x0000FF00) >>  8);
  Serial.print("\t");
  Serial.println(nextSat);
  */
  
  //Serial.println(nextVal);
  
  //Serial.println(stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].rgbwColor);
  //Serial.println(wrgb2rgbw(neopxlObjArr[pixel.pxlSct].ColorHSV(nextHue, nextSat, nextVal)));

  //delay(2000);

  /*
  Serial.print(pixel.targetColor >> 24);
  Serial.print("\t");
  Serial.print((pixel.targetColor & 0x00FF0000) >> 16);
  Serial.print("\t"); 
  Serial.println((pixel.targetColor & 0x0000FF00) >> 8);
  Serial.println(rgbw2hsv(pixel.rgbwColor));
  */ 
  
  stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].hsvColor = (uint32_t)nextHue << 16 | (uint32_t)nextSat << 8 | (uint32_t)nextVal;
  stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].rgbwColor = wrgb2rgbw(neopxlObjArr[pixel.pxlSct].ColorHSV(nextHue, nextSat, nextVal));
  neopxlObjArr[pixel.pxlSct].setPixelColor(pixel.pxlNbr, neopxlObjArr[pixel.pxlSct].ColorHSV(nextHue, nextSat, nextVal));
  neopxlObjArr[pixel.pxlSct].show();
}



//******   LED EFFECTS SECTION   ******//


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