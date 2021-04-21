/*
The LED pixel strips are instanciated here and scenes are also expanded upon
*/

// My set-up, when using the object's setPixelColor method is set as a GRBW string

#include "SK6812.h"


//**********    GLOBAL VARIABLES DECLARATION   ************//

Adafruit_NeoPixel sctZero = Adafruit_NeoPixel(LED_COUNT_SCT_0, PIN_SCT_0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel sctOne  = Adafruit_NeoPixel(LED_COUNT_SCT_1, PIN_SCT_1, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel sctTwo  = Adafruit_NeoPixel(LED_COUNT_SCT_2, PIN_SCT_2, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel sctSix  = Adafruit_NeoPixel(LED_COUNT_SCT_6, PIN_SCT_6, NEO_GRBW + NEO_KHZ800);

pixelInfo stripsArrayOfPxl[SCT_COUNT][LED_COUNT_MAX];

Adafruit_NeoPixel neopxlObjArr[SCT_COUNT];

//**********    GLOBAL VARIABLES DECLARATION   ************//




//**********    LOCAL VARIABLES DECLARATION   ************//

uint32_t sunColor = rgbw2wrgb(0xFFFF1400);

//**********    LOCAL VARIABLES DECLARATION   ************//





void neopxlObjSetUp(Adafruit_NeoPixel &neopxlObj, Adafruit_NeoPixel neopxlArr[], uint8_t *ptrToSctTracker, uint8_t maxBrightness, uint32_t startColor) {
  // loop to fill each pixel info in the array of strips
  for(uint8_t index = 0; index < neopxlObj.numPixels(); index++) {
    stripsArrayOfPxl[*ptrToSctTracker][index].pxlSct   = *ptrToSctTracker;
    stripsArrayOfPxl[*ptrToSctTracker][index].pxlNbr   = index;
    stripsArrayOfPxl[*ptrToSctTracker][index].pxlState = IDLE;
  }
  // adding the initialized Neopixel object to the array
  neopxlArr[*ptrToSctTracker] = neopxlObj;

  // done here since it needs to be done for each instanciated neopxlObj
  neopxlObj.begin();
  neopxlObj.setBrightness(maxBrightness);
  if(startColor) {
    stripColourFill(*ptrToSctTracker, startColor);
  }
  else {
    stripOFF(*ptrToSctTracker);
  }
  
  // increment the section count after every function call
  ++*ptrToSctTracker;
}










//******   PIXEL ACTIONS SECTION   ******//



// Function that can be called to update either one of the color attribute of a pixel
// If the HSV color format is passed, the associated bool should be passed as True
// Func updates the actual color attr by default. For updating the target color attr, bool should be passed as True
void pxlColorUpdt(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat, bool targetUpdt) {
  if(hsvFormat) {

    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;

    if(targetUpdt) {
      stripsArrayOfPxl[section][pixel].rgbwTarget = wrgb2rgbw(neopxlObjArr[section].ColorHSV(hue, sat, val));
      stripsArrayOfPxl[section][pixel].hsvTarget = color;
    }
    else {
      stripsArrayOfPxl[section][pixel].rgbwColor = wrgb2rgbw(neopxlObjArr[section].ColorHSV(hue, sat, val));
      stripsArrayOfPxl[section][pixel].hsvColor = color;
    }
  }

  else {
    if(targetUpdt) {
      stripsArrayOfPxl[section][pixel].rgbwTarget = color;
      stripsArrayOfPxl[section][pixel].hsvTarget = rgbw2hsv(color);
    }
    else {
      stripsArrayOfPxl[section][pixel].rgbwColor = color;
      stripsArrayOfPxl[section][pixel].hsvColor = rgbw2hsv(color);
    }
  }
}

// simple function to output a chosen color to a pixel
// color can be of either HSV or RGB format
void pxlColorOut(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat) {
  if(hsvFormat) {
    
    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;

    pxlColorUpdt(section, pixel, color, hsvFormat);
    neopxlObjArr[section].setPixelColor(pixel, neopxlObjArr[section].ColorHSV(hue, sat, val));
    neopxlObjArr[section].show();
  }
  else {
    pxlColorUpdt(section, pixel, color);
    neopxlObjArr[section].setPixelColor(pixel, rgbw2wrgb(color));
    neopxlObjArr[section].show();
  }
}

// turn off a single pixel
void pxlOFF(uint8_t section, uint8_t pixel) {
  pxlColorUpdt(section, pixel, 0x00000000);
  neopxlObjArr[section].setPixelColor(pixel, 0x00000000);
  neopxlObjArr[section].show();
}

void pxlIterator(uint8_t sctCount) {
  for(uint8_t i = 0; i < sctCount; i++) {
    for(uint8_t j = 0; j < neopxlObjArr[i].numPixels(); j++) {
      switch (stripsArrayOfPxl[i][j].pxlState) {
      case HSV_FADE:
        if(stripsArrayOfPxl[i][j].hsvColor != stripsArrayOfPxl[i][j].hsvTarget) {
          hsvFade(i, j);
        }
        else {
          stripsArrayOfPxl[i][j].pxlState = IDLE;
          Serial.println("here");
        }
        break;

      case SPARKLE:
        //sparkleSct(stripsArrayOfPxl[i][j]);
        break;
      
      default:
        stripsArrayOfPxl[i][j].pxlState = IDLE;       // check if statement is useful
        break;
      }
    }
  }
}



//******   PIXEL ACTIONS SECTION   ******//










//******   STRIP ACTIONS SECTION   ******//


// lights a whole strip with the color passed as input
void stripColourFill(uint8_t section, uint32_t color, bool hsvFormat) {
  if(hsvFormat) {
    
    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;
    
    for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
      pxlColorUpdt(section, pixel, color, hsvFormat);
    }
    neopxlObjArr[section].fill(neopxlObjArr[section].ColorHSV(hue, sat, val));
    neopxlObjArr[section].show();
  }
  else {
    for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
      pxlColorUpdt(section, pixel, color);
    }
    neopxlObjArr[section].fill(rgbw2wrgb(color));
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


//******   STRIP ACTIONS SECTION   ******//











//******   COLOR FORMATS SECTION   ******//



uint32_t rgbw2wrgb(uint32_t rgbwColor) {
  return((rgbwColor >> 8) | (rgbwColor << 24));
}

uint32_t wrgb2rgbw(uint32_t wrgbColor) {
  return((wrgbColor << 8) | wrgbColor >> 24);
}

// converts from RGB to HSV color space
// takes a RRGGBBWW color as input, and return a 32 bit value containing the HSV info
// return value is decomposed as follow : bits 0-7 -> VAL | bits 8-15 -> SAT | bits 16-31 -> HUE
uint32_t rgbw2hsv(uint32_t rgbwColor) {
  
  // values to combine when func will use return statement
  uint16_t hue = 0;
  uint8_t  sat = 0;
  uint8_t  val = 0;

  // extracting each color from the packed 32-bits value and converting to 16-bit val
  uint16_t red = (uint16_t)((rgbwColor & 0xFF000000) >> 16) | (uint16_t)((rgbwColor & 0xFF000000) >> 24);
  uint16_t grn = (uint16_t)((rgbwColor & 0x00FF0000) >>  8) | (uint16_t)((rgbwColor & 0x00FF0000) >> 16);
  uint16_t blu = (uint16_t) (rgbwColor & 0x0000FF00)        | (uint16_t)((rgbwColor & 0x0000FF00) >>  8);

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



//******   COLOR FORMATS SECTION   ******//










//******   LED EFFECTS SECTION   ******//

/*
// func that blinks a pixel a certain color only once
void blink(uint8_t section, uint8_t pixel, uint32_t color, uint16_t blinkTime) {

}
*/

// Initialize a fade action using the HSV color space for a pixel in a specific section
// target color passed as argument is of the 0xRRGGBBWW format
void hsvFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, uint32_t fadeTime) {
  
  // transition from RGB to HSV color space for actual and target color
  uint32_t actualHSV = rgbw2hsv(stripsArrayOfPxl[section][pixel].rgbwColor);
  uint32_t targetHSV = rgbw2hsv(targetRGB);

  //Serial.println(actualHSV);

  // extract hue, sat & val from actual and target colors
  uint16_t actualHue = (uint16_t)((actualHSV & 0xFFFF0000) >> 16);
  uint8_t  actualSat = (uint8_t) ((actualHSV & 0x0000FF00) >>  8);
  uint8_t  actualVal = (uint8_t)  (actualHSV & 0x000000FF)       ;

  uint16_t targetHue = (uint16_t)((targetHSV & 0xFFFF0000) >> 16);
  uint8_t  targetSat = (uint8_t) ((targetHSV & 0x0000FF00) >>  8);
  uint8_t  targetVal = (uint8_t)  (targetHSV & 0x000000FF)       ;

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
  pxlColorUpdt(section, pixel, targetHSV, 1, 1);
}

// function called in the pixel iterator to update the hsv values
void hsvFade(uint8_t section, uint8_t pixel) {

  // extracting actual pixel color and assigning to the next HSV to output as starting point
  uint32_t actualHSV = (stripsArrayOfPxl[section][pixel].hsvColor);
  uint16_t nextHue   = (int16_t)((actualHSV & 0xFFFF0000) >> 16);
  uint8_t  nextSat   = (int8_t) ((actualHSV & 0x0000FF00) >>  8);
  uint8_t  nextVal   = (int8_t)  (actualHSV & 0x000000FF)       ;

  if(millis() - stripsArrayOfPxl[section][pixel].actionOneStart >= absVar(stripsArrayOfPxl[section][pixel].actionOneTime) && stripsArrayOfPxl[section][pixel].actionOneTime != 0) {
    uint16_t targetHue = (int16_t)((stripsArrayOfPxl[section][pixel].hsvTarget & 0xFFFF0000) >> 16);
    if(stripsArrayOfPxl[section][pixel].actionOneTime & 0x80000000) {
      nextHue -= 43;                                                              // the steps are negative, we need to decrement
      if(nextHue <= targetHue) {
        stripsArrayOfPxl[section][pixel].actionOneTime = 0;                       // target is reached, no need to come back in statement again
        nextHue = targetHue;
      }
      else {
        stripsArrayOfPxl[section][pixel].actionOneStart = millis();
      }
    }
    else {
      nextHue += 43;                                                              // steps are positive, we increment
      if(nextHue >= targetHue) {
        stripsArrayOfPxl[section][pixel].actionOneTime = 0;                       // target is reached, no need to come back in statement again
        nextHue = targetHue;
      }
      else {
        stripsArrayOfPxl[section][pixel].actionOneStart = millis();
      }
    }
  }

  if(millis() - stripsArrayOfPxl[section][pixel].actionTwoStart >= absVar(stripsArrayOfPxl[section][pixel].actionTwoTime) && stripsArrayOfPxl[section][pixel].actionTwoTime != 0) {
    uint8_t targetSat = (int8_t)((stripsArrayOfPxl[section][pixel].hsvTarget & 0x0000FF00) >> 8);
    if(stripsArrayOfPxl[section][pixel].actionTwoTime & 0x80000000) {
      nextSat -= 1;
      if(nextSat <= targetSat) {
        stripsArrayOfPxl[section][pixel].actionTwoTime = 0;
        nextSat = targetSat;
      }
      else {
        stripsArrayOfPxl[section][pixel].actionTwoStart = millis();
      }
    }
    else {
      nextSat += 1;
      if(nextSat >= targetSat) {
        stripsArrayOfPxl[section][pixel].actionTwoTime = 0;
        nextSat = targetSat;
      }
      else {
        stripsArrayOfPxl[section][pixel].actionTwoStart = millis();
      }
    }
  }

  if(millis() - stripsArrayOfPxl[section][pixel].actionThreeStart >= absVar(stripsArrayOfPxl[section][pixel].actionThreeTime) && stripsArrayOfPxl[section][pixel].actionThreeTime != 0) {
    uint8_t targetVal = (int8_t)(stripsArrayOfPxl[section][pixel].hsvTarget & 0x000000FF);
    if(stripsArrayOfPxl[section][pixel].actionThreeTime & 0x80000000) {
      nextVal -= 1;
      if(nextVal <= targetVal) {
        stripsArrayOfPxl[section][pixel].actionThreeTime = 0;
        nextVal = targetVal;
      }
      else {
        stripsArrayOfPxl[section][pixel].actionThreeStart = millis();
      }
    }
    else {
      nextVal += 1;
      if(nextVal >= targetVal) {
        stripsArrayOfPxl[section][pixel].actionThreeTime = 0;
        nextVal = targetVal;
      }
      else {
        stripsArrayOfPxl[section][pixel].actionThreeStart = millis();
      }
    }
  }
  // outputting color to strip and updatting actual color attr.
  
  pxlColorUpdt(section, pixel, (uint32_t)nextHue << 16 | (uint32_t)nextSat << 8 | (uint32_t)nextVal, 1);
  neopxlObjArr[section].setPixelColor(pixel, neopxlObjArr[section].ColorHSV(nextHue, nextSat, nextVal));
  neopxlObjArr[section].show();
}



//******   LED EFFECTS SECTION   ******//











//********    LED SCENES SECTION    ********//

/*
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
*/

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