/*
Created on: october 18 2023
Author : J-C Rodrigue
Description : Custom EEPROM library created by the author to use in IDELMA project (CPP file)
*/

#include "EEPROM_lib.h"


//**********    LOCAL VARIABLES   ************//

// static uint8_t eepromPageIdxTracker = EEPROM_START_PAGE_IDX;

//**********    LOCAL VARIABLES   ************//





//**********    LOCAL FUNCTIONS DECLARATION   ************//

bool overflowCheck(uint16_t eepromIndex);

//**********    LOCAL FUNCTIONS DECLARATION   ************//





//**********    GLOBAL FUNC DEFINITION   ************//


/// @brief Write data blocks into the EEPROM
///        Interesting to note that the update() func needs the EEPROM
///        index, which acts just like an address (EEPROM size is 4KB)
/// @param eepromChap eepromChapter type obj to write to
/// @param dataAddr Start address of data to write to EEPROM
void eepromWriteChap(eeprom_chapter_t eepromChap, byte* dataAddr) {
  for (uint16_t i = 0; i < (eepromChap.bytesCount); i++) {
    if (!overflowCheck(eepromChap.startIdx + i)) {
      EEPROM.update((eepromChap.startIdx + i), *(dataAddr + (i * BYTE_SIZE)));

      // Serial.print((unsigned long) (dataAddr + i), HEX);
      // Serial.print(' ');
      // Serial.print(*(dataAddr + i), HEX);
      // Serial.print(' ');
      // Serial.print(eepromChap.startIdx + i, HEX);
      // Serial.print(' ');
      // Serial.println(EEPROM.read(eepromChap.startIdx + i), HEX);
    }
  }
}


/// @brief Reads a given chapter in EEPROM memory and returns
///        its content to the given destination address
/// @param eepromChap eepromChapter type obj to read from
/// @param destAddr Address of the container where to put read bytes
void eepromReadChap(eeprom_chapter_t eepromChap, byte* destAddr) {
  for (uint16_t i = 0; i < (eepromChap.bytesCount); i++) {
    if (!overflowCheck(eepromChap.startIdx + i)) {
      *(destAddr + (i * BYTE_SIZE)) = EEPROM.read(eepromChap.startIdx + i);
    }
  }
}


/// @brief Func to read a single byte from the EEPROM mem
/// @param eepromIdx Index of the EEPROM from which to extract byte
/// @return Read byte
byte eepromByteRead(uint8_t eepromIdx) {
  return ((byte) EEPROM.read(eepromIdx));
}


/// @brief Debug func to reset a chapter in EEPROM
/// @param eepromChap eepromChapter type obj to read from to reset
void eepromReset(eeprom_chapter_t eepromChap) {
  for (uint16_t i = 0; i < (eepromChap.bytesCount); i++) {
    if (!overflowCheck(eepromChap.startIdx + i)) {
      EEPROM.update(eepromChap.startIdx + i, 0x00);

      // Serial.print(eepromChap.startIdx + i, HEX);
      // Serial.print(' ');
      // Serial.println(EEPROM.read(eepromChap.startIdx + i), HEX);
    }
  }
}


//**********    GLOBAL FUNC DEFINITION   ************//





//**********    LOCAL FUNC DEFINITION   ************//


/// @brief Check if overflow will occur. Use func in condition checking
/// @param eepromIndex Index to compare to actual EEPROM length
/// @return Boolean for condition checking. 
///         True if overflow happening; False otherwise
bool overflowCheck(uint16_t eepromIndex) {
  if(eepromIndex == EEPROM.length()) {
    return 1;
  }
  else {
    return 0;
  }
}


//**********    LOCAL FUNC DEFINITION   ************//