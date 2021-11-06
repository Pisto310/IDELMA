/*
Created on: october 25 2021
Author : J-C Rodrigue
Description : All things serial lib source file
*/

#include "Serial_lib.h"
#include "User_Lib.h"
#include "SK6812.h"

void serialRxRead(serial_obj_t *serialObj) {
  while((*serialObj->serialPort).available()) {
    serialObj->xtractedBytes = (*serialObj->serialPort).readBytes(serialObj->rxByteBuf, 64);
  }
}

void serialColorRx(serial_obj_t *serialObj) {
  // All things related to color could be in another function
  // keeping only lines 22-25 would make sense in the context of the func's name
  int8_t rgbwColor[4] = {0, 0, 0, 0};

  for(uint8_t i = 0; i < serialObj->xtractedBytes; i += 2) {
    rgbwColor[i >> 1] = ((uint8_t) (convertAsciiToHex(serialObj->rxByteBuf[i])) << 4) | 
                        ((uint8_t)  convertAsciiToHex(serialObj->rxByteBuf[i + 1]));
   }

  uint32_t colorToSend = (((uint32_t) rgbwColor[0]) << 24) |
                         (((uint32_t) rgbwColor[1]) << 16) |
                         (((uint32_t) rgbwColor[2]) << 8 ) |
                          ((uint32_t) rgbwColor[3]       );
    
  // Serial.println(colorToSend);

  // Pixel and sections ar hardcoded for debugging
  pxlColorOut(0, 0, colorToSend);
  Serial.println(stripsArrayOfPxl[0][0].rgbwColor);
}