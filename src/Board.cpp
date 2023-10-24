/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#include "Board.h"
#include "SK6812.h"

//**********    LOCAL VARIABLES   ************//

// board_infos_t boardInfo = {
//   .sn = SERIAL_NUMBER,
//   .fwVersionMajor = FW_VERSION_MAJOR,
//   .fwVersionMinor = FW_VERSION_MINOR,
//   .sectionsMgmt = {
//     .maxAllowed     = MAX_NO_SCTS,
//     .stillAvailable = MAX_NO_SCTS,
//     .currentlyUsed  = 0
//   },
//   .pixelsMgmt = {
//     .maxAllowed     = PXLINFO_HEAP_SIZE,
//     .stillAvailable = PXLINFO_HEAP_SIZE,
//     .currentlyUsed  = 0
//   }
// };

// byte* ptrBoardInfo = &boardInfo.sn;

const uint32_t sn = SERIAL_NUMBER;

const firmware_t fwVersion = {
  .majorNum = FW_VERSION_MAJOR,
  .minorNum = FW_VERSION_MINOR,
  .patchNum = FW_VERSION_PATCH
};

mutable_brdInfo_t sectionsInfo = {
  .capacity = MAX_NO_SCTS,
  .remaining = MAX_NO_SCTS,
  .assigned = 0
};

mutable_brdInfo_t pixelsInfo = {
  .capacity = PXLINFO_HEAP_SIZE,
  .remaining = PXLINFO_HEAP_SIZE,
  .assigned = 0
};

board_infos_ptrs_t ptrBoardInfos = {
  .serialNumPtr = &sn,
  .fwVersionPtr = &fwVersion,
  .sectionsInfoPtr = &sectionsInfo,
  .pixelsInfoPtr = &pixelsInfo
};

//**********    LOCAL VARIABLES   ************//





//**********    LOCAL FUNCTIONS DECLARATION   ************//

void configSct(uint8_t sctIndex, uint8_t pxlCount);

//**********    LOCAL FUNCTIONS DECLARATION   ************//






//**********    GLOBAL FUNC DEFINITION   ************//


void bootUp() {
  sctsConfigRead();
}


/// @brief Called when a board configuration command is received through serial
///        (in SerialRequestHandler, Serial_lib.cpp).
///        coreDataLen variable must reflect the amount of info contained in the 
///        sctInfoTuple attr in Python code (index, pxlCount), so 2
/// @param serialBuffer parsed and decoded message contained in serial buffer [byte array]
/// @param mssgLen length of RX message (without command byte)
void configBrd(byte serialBuffer[], uint8_t mssgLen) {
  uint8_t coreDataLen = 2;
    for (uint8_t i = 0; i < mssgLen; i += coreDataLen) {
      uint8_t sctID = serialBuffer[i];
      uint8_t pixelCount = serialBuffer[i + 1];
      configSct(sctID, pixelCount);
    }
}


/// @brief Fetch the struct containing all pointers to the board metadata
/// @return The local ptrBoardInfos struct
board_infos_ptrs_t getBoardInfosPtrs() {
  return ptrBoardInfos;
}


/// @brief Indicates if there is enough heap space to create required pixels
/// @param spaceNeeded Number of pixels to be created
/// @return True if enough space; False otherwise
bool remainingHeapSpace(uint8_t spaceNeeded) {
  return(pixelsInfo.remaining >= spaceNeeded);
}


/// @brief Indicates if there are still pins available to create a new sct
/// @return True if enough space; False otherwise
bool remainingSctsPins() {
  return(sectionsInfo.assigned < sectionsInfo.capacity);
}


/// @brief Updates the sectionInfo attributes when a section is added
void sectionsMgmtAdd() {
  if(sectionsInfo.remaining) {
    sectionsInfo.assigned++;
    sectionsInfo.remaining--;
  }
}


/// @brief Updates the pixelInfo attributes with how many pixels
///        were created along with a new section. Also called if
///        pixels were added to an existing section
/// @param spaceFilled Pixels added to new section or existing one 
void pixelsMgmtAdd(uint8_t spaceFilled) {
  if(pixelsInfo.remaining) {
    pixelsInfo.assigned += spaceFilled;
    pixelsInfo.remaining -= spaceFilled;
  }
}


/// @brief Updates the sectionInfo attributes when a section is removed
void sectionsMgmtRemove() {
  if(sectionsInfo.assigned) {
    sectionsInfo.assigned--;
    sectionsInfo.remaining++;
  }
}


// Only called within editSection() and clearSection() funcs

/// @brief Updates the pixelInfo attributes with how many pixels
///        were freed from removing pixels from a section. Since 
///        clearing a section removes all its pixels, it also
///        is invoked in a section deletion
/// @param spaceFreed 
void pixelsMgmtRemove(uint8_t spaceFreed) {
  if(pixelsInfo.assigned && (pixelsInfo.assigned - spaceFreed >= 0)) {
    pixelsInfo.assigned -= spaceFreed;
    pixelsInfo.remaining += spaceFreed;
  }
}


//******************************************           DEBUG           ************************************************//
void eepromReset() {
  sctsConfigRst();
}


void allOff() {
  for (uint8_t i = 0; i < getSctIndexTracker(); i++) {
    stripOFF(i);
  }
}
//******************************************           DEBUG           ************************************************//


//**********    GLOBAL FUNC DEFINITION   ************//





//**********    LOCAL FUNC DEFINITION   ************//


/// @brief Configure/Modify/Delete sections with info received through serial
/// @param sctIndex Index of the section
/// @param pxlCount Number of pixels to be included in section
void configSct(uint8_t sctIndex, uint8_t pxlCount) {
  if(sctIndex == getSctIndexTracker()) {
    setupSection(pxlCount);
  }
  else if(!pxlCount) {
    clearSection(sctIndex);
  }
  else {
    editSection(sctIndex, pxlCount);
  }
}


//**********    LOCAL FUNC DEFINITION   ************//