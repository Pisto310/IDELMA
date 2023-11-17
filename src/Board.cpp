/*
Created on: november 20 2021
Author : J-C Rodrigue
Description : Everything associated to the board, from user-defined serial number to saved set-up configs
*/

#include "Board.h"
#include "SK6812.h"
#include "Serial_lib.h"

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

typedef struct ConfigBrdDataPckt {
  byte           subCmd;
  byte           sctIdx;
  sct_metadata_t configData;
}config_brd_data_t;

const uint32_t sn = SERIAL_NUMBER;

const firmware_t fwVersion = {
  .majorNum = FW_VERSION_MAJOR,
  .minorNum = FW_VERSION_MINOR,
  .patchNum = FW_VERSION_PATCH
};

brdMgmt_attr_t sctsMgmtMetaData = {
  .capacity = MAX_NO_SCTS,
  .remaining = MAX_NO_SCTS,
  .assigned = 0
};

brdMgmt_attr_t pxlsMgmtMetaData = {
  .capacity = PXLINFO_HEAP_SIZE,
  .remaining = PXLINFO_HEAP_SIZE,
  .assigned = 0
};

brd_metadata_ptrs_t brdMgmtMetaDatasPtr = {
  .serialNumPtr = &sn,
  .fwVersionPtr = &fwVersion,
  .sctsMgmtMetaDataPtr = &sctsMgmtMetaData,
  .pxlsMgmtMetaDataPtr = &pxlsMgmtMetaData
};

void (*configBrdFuncPtr[3]) (byte, sct_metadata_t) = {
  createSection,
  editSection,
  deleteSection
};

//**********    LOCAL VARIABLES   ************//





//**********    LOCAL FUNCTIONS DECLARATION   ************//



//**********    LOCAL FUNCTIONS DECLARATION   ************//






//**********    GLOBAL FUNC DEFINITION   ************//


/// @brief Every function to run at boot-up are to be inserted
///        in this function called at power-up (setup in main.cpp)
void bootUp() {
  sctsConfigRead();
}


/// @brief Called when a board configuration command is received through serial
///        (in SerialRequestHandler, Serial_lib.cpp). After the info as been
///        extracted from the received packet, the appropriate function expressed 
///        in the sub-command byte is called and necessary data is passed onto it.
/// @param serialBuffer Parsed and decoded message contained in serial buffer [byte array]
/// @param mssgLen Length of RX message (without command byte)
void configBrd(byte serialBuffer[], uint8_t mssgLen) {
  
  config_brd_data_t dataPckt;
  sct_metadata_t* sctMetaDataPtr = &dataPckt.configData;
  byte* sctMetaDataBytePtr = (byte*) sctMetaDataPtr;

  for (uint8_t i = 0; i < mssgLen; i += sizeof(config_brd_data_t)) {
    dataPckt.subCmd = serialBuffer[i];
    dataPckt.sctIdx = serialBuffer[i + 1];
    
    for (uint8_t j = 0; j < sizeof(sct_metadata_t); j++) {
      *(sctMetaDataBytePtr + j) = serialBuffer[i + j + (sizeof(config_brd_data_t) - sizeof(sct_metadata_t))];
    }
    (*configBrdFuncPtr[dataPckt.subCmd]) (dataPckt.sctIdx, dataPckt.configData);
  }
}


/// @brief Fetch the struct containing all pointers to the board metadata
/// @return The local brdMgmtMetaDatasPtr struct
brd_metadata_ptrs_t getBrdMgmtMetaDatasPtr() {
  return brdMgmtMetaDatasPtr;
}


/// @brief Indicate if there is enough heap space to create required pixels
/// @param spaceNeeded Number of pixels to be created
/// @return True if enough space; False otherwise
bool remainingHeapSpace(uint8_t spaceNeeded) {
  return(pxlsMgmtMetaData.remaining >= spaceNeeded);
}


/// @brief Indicate if there are still pins available to create a new sct
/// @return True if enough space; False otherwise
bool remainingSctsPins() {
  return(sctsMgmtMetaData.assigned < sctsMgmtMetaData.capacity);
}


/// @brief Update the sectionInfo attributes when a section is added
void sectionsMgmtAdd() {
  if(sctsMgmtMetaData.remaining) {
    sctsMgmtMetaData.assigned++;
    sctsMgmtMetaData.remaining--;
  }
}


/// @brief Updates the pixelInfo attributes with how many pixels
///        were created along with a new section. Also called if
///        pixels were added to an existing section
/// @param spaceFilled Pixels added to new section or existing one 
void pixelsMgmtAdd(uint8_t spaceFilled) {
  if(pxlsMgmtMetaData.remaining) {
    pxlsMgmtMetaData.assigned += spaceFilled;
    pxlsMgmtMetaData.remaining -= spaceFilled;
  }
}


/// @brief Updates the sectionInfo attributes when a section is removed
void sectionsMgmtRemove() {
  if(sctsMgmtMetaData.assigned) {
    sctsMgmtMetaData.assigned--;
    sctsMgmtMetaData.remaining++;
  }
}


// Only called within editSection() and deleteSection() funcs

/// @brief Updates the pixelInfo attributes with how many pixels
///        were freed from removing pixels from a section. Since 
///        clearing a section removes all its pixels, it also
///        is invoked in a section deletion
/// @param spaceFreed 
void pixelsMgmtRemove(uint8_t spaceFreed) {
  if(pxlsMgmtMetaData.assigned && (pxlsMgmtMetaData.assigned - spaceFreed >= 0)) {
    pxlsMgmtMetaData.assigned -= spaceFreed;
    pxlsMgmtMetaData.remaining += spaceFreed;
  }
}


//******************************************           DEBUG           ************************************************//
void eepromReset() {
  sctsConfigRst();
}


void allOff() {
  for (uint8_t i = 0; i < sctsMgmtMetaData.assigned; i++) {
    stripOFF(i);
  }
}
//******************************************           DEBUG           ************************************************//


//**********    GLOBAL FUNC DEFINITION   ************//





//**********    LOCAL FUNC DEFINITION   ************//



//**********    LOCAL FUNC DEFINITION   ************//