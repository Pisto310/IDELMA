/*
Created on: october 18 2023
Author : J-C Rodrigue
Description : Custom EEPROM library created by the author to use in IDELMA project (header file)
*/

#ifndef EEPROM_LIB_H_
#define EEPROM_LIB_H_

#include "Board.h"
#include <EEPROM.h>

// Following macros are to be updated together, ALWAYS
#define EEPROM_PAGE_SIZE                    64U
#define PWR_OF_TWO_EXPONENT                 6


#define EEPROM_TABLE_OF_CONTENT_PAGE        0
#define EEPROM_SCTS_METADATA_FIRST_PAGE     1


#define EEPROM_PAGE_FIRST_IDX(_pageNumber)  (EEPROM_PAGE_SIZE * _pageNumber)
#define EEPROM_CHAP_PAGE_LEN(_bytesCount)   ((_bytesCount >> PWR_OF_TWO_EXPONENT) + 1)

// #define EEPROM_SCTS_INFO_PAGE_CNT           (((sizeof(section_info_t) * MAX_NO_SCTS) >> PWR_OF_TWO_EXPONENT) + 1)

//**********    GLOBAL TYPES DECLARATION   ************//

typedef struct EepromChapter {
  uint8_t*  tocStatusIndic;
  uint8_t   firstPgeNbr;
  uint16_t  bytesCount;
}eeprom_chapter_t;

//**********    GLOBAL TYPES DECLARATION   ************//



//**********    GLOBAL FUNCTIONS DECLARATION   ************//

uint8_t getChapStatusIndic(eeprom_chapter_t eepromChap);

void eepromWriteChap(eeprom_chapter_t eepromChap, byte* dataAddr);
void eepromReadChap(eeprom_chapter_t eepromChap, byte* destAddr);

byte eepromByteRead(uint8_t eepromIdx);

void eepromReset(eeprom_chapter_t eepromChap);

void debugToc();

//**********    GLOBAL FUNCTIONS DECLARATION   ************//


#endif  /*  EEPROM_LIB_H_ */