/*
Created on: october 18 2023
Author : J-C Rodrigue
Description : Custom EEPROM library created by the author to use in IDELMA project (CPP file)
*/

#include "EEPROM_lib.h"


//************    LOCAL VARIABLES   ************//

eeprom_chapter_t tableOfContent = {
  .tocStatusIndic = 0,
  .firstPgeNbr = EEPROM_TABLE_OF_CONTENT_PAGE,
  .bytesCount  = EEPROM_PAGE_SIZE
};

//************    LOCAL VARIABLES   ************//





//************    LOCAL FUNCTIONS DECLARATION   ************//

bool overflowCheck(uint16_t eepromIndex);
void setTocChapStatus(uint8_t chapPage, uint8_t chapLen, uint8_t statusIndic);

//************    LOCAL FUNCTIONS DECLARATION   ************//





//************    GLOBAL FUNC DEFINITION   ************//


/// @brief Access the ToC to check the status indicator of a given chapter
/// @param eepromChap Chapter obj of which to check the status
/// @return Status Indicator. If non-zero, it means the chapter is written.
///         Depending on the chapter type, this indicator's value may have
///         a certain meaning
uint8_t getChapStatusIndic(eeprom_chapter_t eepromChap) {
  return EEPROM.read(eepromChap.firstPgeNbr);
}


/// @brief Write data blocks into the EEPROM
///        Interesting to note that the update() func needs the EEPROM
///        index, which acts just like an address (EEPROM size is 4KB).
///        After the Chapter is written, the chapter index state of the
///        content table is set (btwn 1 to 255) for each chap pages
/// @param eepromChap eepromChapter type obj to write to
/// @param dataAddr   Start address of data to write to EEPROM
void eepromWriteChap(eeprom_chapter_t eepromChap, byte* dataAddr) {
  for (uint16_t i = 0; i < (eepromChap.bytesCount); i++) {
    if (!overflowCheck(EEPROM_PAGE_FIRST_IDX(eepromChap.firstPgeNbr) + i)) {
      EEPROM.update((EEPROM_PAGE_FIRST_IDX(eepromChap.firstPgeNbr) + i), *(dataAddr + (i * BYTE_SIZE)));
    }
    // Serial.print(EEPROM_PAGE_FIRST_IDX(eepromChap.firstPgeNbr) + i, HEX);
    // Serial.print(' ');
    // Serial.println(EEPROM.read(EEPROM_PAGE_FIRST_IDX(eepromChap.firstPgeNbr) + i), HEX);
  }
  // Serial.println(' ');
  setTocChapStatus(eepromChap.firstPgeNbr, EEPROM_CHAP_PAGE_LEN(eepromChap.bytesCount), *(eepromChap.tocStatusIndic));
}


/// @brief Reads a given chapter in EEPROM memory and returns
///        its content to the given destination address
/// @param eepromChap eepromChapter type obj to read from
/// @param destAddr   Address of the container where to put read bytes
void eepromReadChap(eeprom_chapter_t eepromChap, byte* destAddr) {  
  for (uint16_t i = 0; i < (eepromChap.bytesCount); i++) {
    if (!overflowCheck(EEPROM_PAGE_FIRST_IDX(eepromChap.firstPgeNbr) + i)) {
      *(destAddr + (i * BYTE_SIZE)) = EEPROM.read(EEPROM_PAGE_FIRST_IDX(eepromChap.firstPgeNbr) + i);
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
/// @param eepromChap eepromChapter type obj to reset
void eepromReset(eeprom_chapter_t eepromChap) {
  for (uint16_t i = 0; i < (eepromChap.bytesCount); i++) {
    if (!overflowCheck(EEPROM_PAGE_FIRST_IDX(eepromChap.firstPgeNbr) + i)) {
      EEPROM.update(EEPROM_PAGE_FIRST_IDX(eepromChap.firstPgeNbr) + i, 0x00);
    }
  }
  setTocChapStatus(eepromChap.firstPgeNbr, EEPROM_CHAP_PAGE_LEN(eepromChap.bytesCount), 0);
}


void debugToc() {
  eepromReset(tableOfContent);
}


//************    GLOBAL FUNC DEFINITION   ************//





//************    LOCAL FUNC DEFINITION   ************//


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


/// @brief Set or reset the status indicator of an eeprom chapter and
///        all its pages in the 'Table of Content' page (EEPROM idx 0 to
///        63). For example, a chapter that starts on page 5 and is made
///        of 3 pages total has its status reflected in the ToC as either
///        written (non-zero) or not (0). If written, the content of the 
///        EEPROM @ idxs 5, 6 & 7 will be non-zero and indicate a certain
///        characteristic about the content of the chapter. For the scts
///        metadata, that status indicator represents the total nbr of scts
///        saved. (Refer to google sheet for a visual representation).
/// @param chapPage First page number (1 to 63) of the affected chapter
/// @param chapLen  Number of pages in the chapter
/// @param structObjsCount Indicate if the state is to be set or reset
void setTocChapStatus(uint8_t chapPage, uint8_t chapLen, uint8_t statusIndic) {
  for (uint8_t i = chapPage; i < (chapPage + chapLen); i++) {
    EEPROM.update(i, statusIndic);

    // Serial.print(i, HEX);
    // Serial.print(' ');
    // Serial.println(EEPROM.read(i), HEX);
  }
}


//************    LOCAL FUNC DEFINITION   ************//


// Serial.print((unsigned long) (dataAddr + i), HEX);
// Serial.print(' ');
// Serial.print(*(dataAddr + i), HEX);
// Serial.print(' ');
// Serial.print(eepromChap.firstPgeIdx + i, HEX);
// Serial.print(' ');
// Serial.println(EEPROM.read(eepromChap.firstPgeIdx + i), HEX);