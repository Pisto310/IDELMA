/*
The LED pixel strips are instanciated here and scenes are also expanded upon
*/

// My set-up, when using the object's setPixelColor method is set as a GRBW string

#include "SK6812.h"


//**********    GLOBAL VARIABLES DECLARATION   ************//

Adafruit_NeoPixel sctZero = Adafruit_NeoPixel(LED_COUNT_SCT_1, PIN_SCT_1, NEO_GRBW + NEO_KHZ800);
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
    stripsArrayOfPxl[*ptrToSctTracker][index].pxlSct = *ptrToSctTracker;
    stripsArrayOfPxl[*ptrToSctTracker][index].pxlNbr = index;
    if(!startColor) {
      stripsArrayOfPxl[*ptrToSctTracker][index].pxlState = OFF;
    }
    else { 
      stripsArrayOfPxl[*ptrToSctTracker][index].pxlState = ON;
    }
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
      case ON:
        break;
      
      case OFF:
        break;

      case FADE:
        break;

      case SPARKLE:
        sparkleSct(stripsArrayOfPxl[i][j]);
        break;
      
      default:
        stripsArrayOfPxl[i][j].pxlState = IDLE;
        break;
      }
    }
  }
}

uint32_t rgbw2wrgb(uint32_t rgbwColor) {
  return((rgbwColor >> 8) | (rgbwColor << 24));
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
  uint16_t red = uint16_t((color & 0xFF000000) >> 16) | uint16_t((color & 0xFF000000) >> 24);
  uint16_t grn = uint16_t((color & 0x00FF0000) >>  8) | uint16_t((color & 0x00FF0000) >> 16);
  uint16_t blu = uint16_t (color & 0x0000FF00)        | uint16_t((color & 0x0000FF00) >>  8);

  uint8_t maxColorVal = max(uint8_t(red & 0x00FF), max(uint8_t(grn & 0x00FF), uint8_t(blu & 0x00FF)));
  uint8_t minColorVal = min(uint8_t(red & 0x00FF), min(uint8_t(grn & 0x00FF), uint8_t(blu & 0x00FF)));

  uint8_t delta = maxColorVal - minColorVal;

  // section to calculate the hue
  // normally, the first term of the equation is 60°, 
  // but in our case, the whole 360° is 65 535, so 60° is a sixth of that (10 922)
  if(red >= grn && grn >= blu) {
    hue = uint16_t((0xFFFF / 6) * ((float(grn) - float(blu)) / (float(red) - float(blu))));
  }
  else if(grn > red && red >= blu) {
    hue = uint16_t((0xFFFF / 6) * (2 - (float(red) - float(blu)) / (float(grn) - float(blu))));
  }
  else if(grn >= blu && blu > red) {
    hue = uint16_t((0xFFFF / 6) * (2 + (float(blu) - float(red)) / (float(grn) - float(red))));
  }
  else if(blu > grn && grn > red) {
    hue = uint16_t((0xFFFF / 6) * (4 - (float(grn) - float(red)) / (float(blu) - float(red))));
  }
  else if(blu > red && red >= grn) {
    hue = uint16_t((0xFFFF / 6) * (4 + (float(red) - float(grn)) / (float(blu) - float(grn))));
  }
  else if(red >= blu && blu > grn) {
    hue = uint16_t((0xFFFF / 6) * (6 - (float(blu) - float(grn)) / (float(red) - float(grn))));
  }

  // saturation calculation
  if(maxColorVal != 0) {
    sat = uint8_t((float(delta) / float(maxColorVal)) * 255);
  }

  // val calculation
  val = maxColorVal;

  return((uint32_t(hue) << 16) | (uint32_t(sat) << 8) | (uint32_t(val)));
}

//******   BASIC FUNCS SECTION   ******//





//******   LED SCENES SECTION   ******//





// Initialize a strip/section to start sparkle. Called once in main
void sparkleInit(uint8_t section) {
  // randomly chooses first pxl to turn ON/OFF
  uint8_t sparklePxl = random(neopxlObjArr[section].numPixels());

  // updating pxl attributes
  stripsArrayOfPxl[section][sparklePxl].actionOneStart = millis();
  stripsArrayOfPxl[section][sparklePxl].actionOneTime = 50;
  stripsArrayOfPxl[section][sparklePxl].pxlState = SPARKLE;

  // update neopxlObj
  neopxlObjArr[section].setPixelColor(sparklePxl, sunColor);
  neopxlObjArr[section].show();
}

// Create a sparkling effect for a whole section (strip)
void sparkleSct(pixelInfo pixel) {

  if(millis() - pixel.actionOneStart >= pixel.actionOneTime) {

    // turn OFF pixel and actualize pxl attributes
    neopxlObjArr[pixel.pxlSct].setPixelColor(pixel.pxlNbr, 0x00000000);
    stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].pxlState = OFF;
    
    uint8_t nextSparklePxl = random(neopxlObjArr[pixel.pxlSct].numPixels());
    
    // if the rand generator gives the same result, do it until the result is different
    while(nextSparklePxl == pixel.pxlNbr) {
      nextSparklePxl = random(neopxlObjArr[pixel.pxlSct].numPixels());
    }

    // turn ON next pixel and actualize attributes
    neopxlObjArr[pixel.pxlSct].setPixelColor(nextSparklePxl, sunColor);
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].pxlState = SPARKLE;
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].actionOneStart = millis();
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].actionOneTime = 50;
    neopxlObjArr[pixel.pxlSct].show();
  }
}

// Initialize a fade action using the HSV color space for a pixel in a specific section
// target color passed as argument is of the 0xRRGGBBWW format
void hueFadeInit(uint8_t pixel, uint8_t section, uint32_t targetRGB, uint32_t fadeTime) {
  
  // transition from RGB to HSV color space for actual and target color
  uint32_t actualHSV = rgbw2hsv(neopxlObjArr[section].getPixelColor(pixel));
  uint32_t targetHSV = rgbw2hsv(targetRGB);

  // extract hue, sat & val from actual and target colors
  uint16_t actualHue = uint16_t((actualHSV & 0xFFFF0000) >> 16);
  uint8_t  actualSat = uint8_t ((actualHSV & 0x0000FF00) >>  8);
  uint8_t  actualVal = uint8_t  (actualHSV & 0x000000FF)       ;

  uint16_t targetHue = uint16_t((targetHSV & 0xFFFF0000) >> 16);
  uint8_t  targetSat = uint8_t ((targetHSV & 0x0000FF00) >>  8);
  uint8_t  targetVal = uint8_t  (targetHSV & 0x000000FF)       ;

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
  int16_t satDelta = targetSat - actualSat;
  int16_t valDelta = targetVal - actualVal;

  // steps are calculated and are expressed in ms/bit
  uint32_t hueStep = fadeTime / hueStep;
  uint32_t satStep = fadeTime / satDelta;
  uint32_t valStep = fadeTime / valDelta;

}

void hueFade(pixelInfo pixel) {

}



//******   LED SCENES SECTION   ******//


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