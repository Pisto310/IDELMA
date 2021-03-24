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
    stripsArrayOfPxl[*ptrToSctTracker][index].rgbwColor = startColor;
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

uint32_t rgbw2wrgb(uint32_t rgbwColor) {
  return((rgbwColor >> 8) | (rgbwColor << 24));
}

//******   BASIC FUNCS SECTION   ******//





//******   LED SCENES SECTION   ******//

// Initialize a strip/section to start sparkle. Called once in main
void sparkleInit(uint8_t section) {
  // randomly chooses first pxl to turn ON/OFF
  uint8_t sparklePxl = random(neopxlObjArr[section].numPixels());

  // updating pxl attributes
  stripsArrayOfPxl[section][sparklePxl].timeStart = millis();
  stripsArrayOfPxl[section][sparklePxl].actionDuration = 50;
  stripsArrayOfPxl[section][sparklePxl].pxlState = SPARKLE;
  stripsArrayOfPxl[section][sparklePxl].rgbwColor = sunColor;

  // update neopxlObj
  neopxlObjArr[section].setPixelColor(sparklePxl, sunColor);
  neopxlObjArr[section].show();
}

// Create a sparkling effect for a whole section (strip)
void sparkleSct(pixelInfo pixel) {

  if(millis() - pixel.timeStart >= pixel.actionDuration) {

    //Serial.println(millis() - pixel.timeStart);
    //Serial.println(pixel.actionDuration);
    //Serial.println(pixel.pxlNbr);

    // turn OFF pixel and actualize pxl attributes
    neopxlObjArr[pixel.pxlSct].setPixelColor(pixel.pxlNbr, 0x00000000);
    stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].rgbwColor = 0x00000000;
    stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].pxlState = OFF;
    
    uint8_t nextSparklePxl = random(neopxlObjArr[pixel.pxlSct].numPixels());
    
    // if the rand generator gives the same result, do it until the result is different
    while(nextSparklePxl == pixel.pxlNbr) {
      nextSparklePxl = random(neopxlObjArr[pixel.pxlSct].numPixels());
    }

    // turn ON next pixel and actualize attributes
    neopxlObjArr[pixel.pxlSct].setPixelColor(nextSparklePxl, sunColor);
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].pxlState = SPARKLE;
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].rgbwColor = sunColor;
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].timeStart = millis();
    stripsArrayOfPxl[pixel.pxlSct][nextSparklePxl].actionDuration = stripsArrayOfPxl[pixel.pxlSct][pixel.pxlNbr].actionDuration;
    neopxlObjArr[pixel.pxlSct].show();
  }
}





//******   LED SCENES SECTION   ******//







/*
// func that fades from one color to the next in a given amount of time (255ms as default)
// colors are passed as 4-bytes 0xRRGGBBWW and pixels are passed one after another (cannot skip one)
void fadeColorToColor(Adafruit_NeoPixel &neopxlObj, uint8_t pxlCount, uint8_t firstPxl, uint32_t startColor, uint32_t targetColor, stripToFade &stripInfo) {
  
  static uint32_t fadeTime = 10000;
  
  
  Checks if any inputted pixel already have a fade to execute.
  If not, the function goes along as it should. 
  Otherwise, the already fading pixels are skipped and nothing 
  is done with them.
  
  //Serial.println(stripInfo.pxlTracker)
  if(stripInfo.pxlTracker < firstPxl + pxlCount) {
    for(stripInfo.pxlTracker; stripInfo.pxlTracker < (firstPxl + pxlCount); stripInfo.pxlTracker++) {
      //Serial.println(index);
      colorDecomposer(stripInfo.startColorArr, startColor, stripInfo.pxlTracker * 4); 
      Serial.print(stripInfo.startColorArr[index * 4]);
      delay(1000);
      Serial.print(stripInfo.startColorArr[index * 4 + 1]);
      delay(1000);
      Serial.print(stripInfo.startColorArr[index * 4 + 2]);
      delay(1000);
      Serial.println(stripInfo.startColorArr[index * 4 + 3]);
      delay(1000);
      colorDecomposer(stripInfo.targetColorArr, targetColor, stripInfo.pxlTracker * 4);
      Serial.print(stripInfo.targetColorArr[index * 4]);
      delay(1000);
      Serial.print(stripInfo.targetColorArr[index * 4 + 1]);
      delay(1000);
      Serial.print(stripInfo.targetColorArr[index * 4 + 2]);
      delay(1000);
      Serial.println(stripInfo.targetColorArr[index * 4 + 3]);
      delay(1000); 
      for(uint8_t i = 0; i < 4; i++) {  
        stripInfo.bitsGradient[stripInfo.pxlTracker + i] = stripInfo.targetColorArr[stripInfo.pxlTracker + i] - stripInfo.startColorArr[stripInfo.pxlTracker + i];
      }
    }
  }
  else {
    return;
  }

  static int16_t bitsGradient[4] = {0, 0, 0, 0};
  static uint8_t timeBtwnBits[4] = {0, 0, 0, 0};

  // calculating the difference between start and target colors 
  if(bitsGradient[0] == 0 && bitsGradient[1] == 0 && bitsGradient[2] == 0 && bitsGradient[3] == 0) {
    for(uint8_t i = 0; i < 4; i++) {
      bitsGradient[i] = targetColorArray[i] - startColorArray[i];
      if(bitsGradient[i] != 0) {
        timeBtwnBits[i] = fadeTime / bitsGradient[i];   // if result is a float, it is rounded down, always  
        if(bitsGradient[i] > 0) {
          switch (i) {
          case 0:
            timer.setTimer(timeBtwnBits[0], incrRedByte, bitsGradient[0]);
            break;

          case 1:
            timer.setTimer(timeBtwnBits[1], incrGrnByte, bitsGradient[1]);
            break;

          case 2:
            timer.setTimer(timeBtwnBits[2], incrBluByte, bitsGradient[2]);
            break;

          case 3:
            timer.setTimer(timeBtwnBits[3], incrWhtByte, bitsGradient[3]);
            break;

          default:
            break;
          }
        }
        if(bitsGradient[i] < 0) {
          switch (i) {
          case 0:
            timer.setTimer(timeBtwnBits[0], decrRedByte, abs(bitsGradient[0]));
            break;

          case 1:
            timer.setTimer(timeBtwnBits[1], decrGrnByte, abs(bitsGradient[1]));
            break;

          case 2:
            timer.setTimer(timeBtwnBits[2], decrBluByte, abs(bitsGradient[2]));
            break;

          case 3:
            timer.setTimer(timeBtwnBits[3], decrWhtByte, abs(bitsGradient[3]));
            break;

          default:
            break;
          }
        }
      }
    }
  }
  
  // Outputs the color on the strip
  uint32_t color = neopxlObj.Color(startColorArray[0], startColorArray[1], startColorArray[2], startColorArray[3]);
  neopxlObj.fill(color);
  neopxlObj.show();
}
*/