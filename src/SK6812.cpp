/*
The LED pixel strips are instanciated here and scenes are also expanded upon
*/

// My set-up, when using the object's setPixelColor method is set as a GRBW string

#include "SK6812.h"

//**********    GLOBAL VARIABLES DECLARATION   ************//

// volatile pxl_metadata_t stripsArrayOfPxl[SCT_COUNT][LED_COUNT_MAX];

//**********    GLOBAL VARIABLES DECLARATION   ************//





//**********    LOCAL VARIABLES DECLARATION   ************//

pxl_metadata_t* pxlMetaDataPtr = (pxl_metadata_t*)calloc(PXLINFO_HEAP_SIZE, sizeof(pxl_metadata_t));
pxl_metadata_t* pxlMetaDataPtrArr[MAX_NO_SCTS];

// Filling the neopxl Objects array
Adafruit_NeoPixel neopxlObjArr[MAX_NO_SCTS] = {
  Adafruit_NeoPixel(0,  PIN_SCT_0, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_1, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_2, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_3, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_4, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_5, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_6, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_7, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_8, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0,  PIN_SCT_9, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0, PIN_SCT_10, NEO_GRBW + NEO_KHZ800),
  Adafruit_NeoPixel(0, PIN_SCT_11, NEO_GRBW + NEO_KHZ800),
};

sct_metadata_t sctsMetaDatasArr[MAX_NO_SCTS] = {{ 0 }};

static byte* sctIdxTrackerPtr = &(getBrdMgmtMetaDatasPtr().sctsMgmtMetaDataPtr)->assigned;

eeprom_chapter_t sctsMetaDataChap = {
  .tocStatusIndic = sctIdxTrackerPtr,
  .firstPgeNbr    = EEPROM_SCTS_METADATA_FIRST_PAGE,
  .bytesCount     = (sizeof(sct_metadata_t) * MAX_NO_SCTS)
};

//**********    LOCAL VARIABLES DECLARATION   ************//






//**********    LOCAL FUNCTIONS DECLARATION   ************//


// void editPxlCount(uint8_t sctID, uint8_t newPxlCount);

void setupFromSave();

// void eraseSctMemBlocks(uint8_t section, uint8_t sctNewBlockCount);
// void writeSctMemBlocks(uint8_t section, uint8_t sctNewBlockCount);

// Following funcs to be moved to other files later
void pxlStateUpdt(uint8_t section, uint8_t pixel, pixel_state_t state);
void pxlColorUpdt(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat = 0, bool targetUpdt = 0);
void pxlColorOut(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat = 0);
void pxlOFF(uint8_t section, uint8_t pixel);

void stripColorFill(uint8_t section, uint32_t color, bool hsvFormat = 0);


//**********    LOCAL FUNCTIONS DECLARATION   ************//




//**********    LOCAL FUNCTIONS DEFINITION   ************//


/// @brief Indicates the number of memory block needed or used for
///        a given section
/// @param pxlCount Number of pixel to be lit up on the strip
/// @param singlePxlCtrl Indicating if whole strip is seen as a single pixel
///                      (Acting as a bool)
/// @return Required amount of blocks needed or used in heap
byte sctMemBlocksUsage(byte pxlCount, byte singlePxlCtrl) {
  return singlePxlCtrl ? singlePxlCtrl : pxlCount;
}


/// @brief Update the necessary pxl_metadata_t parameters for each pixels of a section
///        when creating a section or after having edited an existing one 
/// @param sectionIndex     Index of the section for which to set the pxls metadata
/// @param neededBlockSpace Number of memory blocks to set
/// @param pxlStartIndex    Index of the pixel at which to start the updating of params
void updtPxlsMetaData(uint8_t sectionIndex, uint8_t neededBlockSpace, uint8_t pxlStartIdx = 0) {
  for (uint8_t pxlIdx = pxlStartIdx; pxlIdx < neededBlockSpace; pxlIdx++) {
    (pxlMetaDataPtrArr[sectionIndex] + pxlIdx)->pxlSctID = sectionIndex;
    (pxlMetaDataPtrArr[sectionIndex] + pxlIdx)->pxlID    = pxlIdx;
    (pxlMetaDataPtrArr[sectionIndex] + pxlIdx)->pxlState = IDLE;
  }
}


/// @brief Update the necessary Neopixel object attributes through method calling when
///        creating a section or after having edited an existing one
/// @param sectionIndex Index of the section for which to edit attr Neopixel attributes
// void updtNeopxlObj(uint8_t sectionIndex) {
//   if (sctsMetaDatasArr[sectionIndex].pxlCount != neopxlObjArr[sectionIndex].numPixels()) {
//     neopxlObjArr[sectionIndex].updateLength((uint16_t) sctsMetaDatasArr[sectionIndex].pxlCount);
//   }

//   if (sctsMetaDatasArr[sectionIndex].brightness != neopxlObjArr[sectionIndex].getBrightness()) {
//     neopxlObjArr[sectionIndex].setBrightness(sctsMetaDatasArr[sectionIndex].brightness);
//   }
// }

/// @brief Update the necessary Neopixel object attributes through method calling when
///        creating a section or after having edited an existing one
/// @param neoPxlObj     A NeoPixel object to check for attributes modifications
/// @param pixelCount    Pixel count with which to compare actual pixel count of NeoPxl obj
/// @param brightnessVal Brightness value with which to compare actual value of NeoPxl obj
void updtNeoPxlObj(Adafruit_NeoPixel* neoPxlObj, uint8_t pixelCount, uint8_t brightnessVal) {
  if (pixelCount != (*neoPxlObj).numPixels()) {
    (*neoPxlObj).updateLength((uint16_t) pixelCount);
  }
  if (brightnessVal != (*neoPxlObj).getBrightness()) {
    (*neoPxlObj).setBrightness(brightnessVal);
  }
}


/// @brief Update the board management metadatas when creating a section or editing
///        an existing one. 
/// @param usedMemBlocks Signed number indicating count of memory blocks freed or 
///                      used in the creation or editing process (signage indicates
///                      action to undertake)
void updtPxlsMgmtMetaData(int8_t usedMemBlocks) {
  if (usedMemBlocks & 0x80) {
    pixelsMgmtRemove(~usedMemBlocks + 1);
  }
  else if (usedMemBlocks) {
    pixelsMgmtAdd(usedMemBlocks);
  }
}


/// @brief Removes mem blocks in any given section. It's done in a bare-metal kind of way,
///        which is by shifting affected bytes in the heap. The number of bytes to shift
///        is dependent upon block size (pxl_metadata_t byte size) and how many blocks
///        there are to shift. When reaching the new address of pxlMetaDataPtr, all bytes
///        are then overwritten with 0x00 as a way to reset them for future use.
///        Note that this 0x00 byte reset isn't executed when erasing the last sct
///        because it will be updated when the next one is created
/// @param sctID Index of the section from which to remove memory blocks
/// @param sctNewBlockCount New block count (must be lower than original)
void eraseSctMemBlocks(uint8_t sctID, uint8_t sctNewBlockCount) {  
  
  uint8_t actualBlockCount = sctMemBlocksUsage(sctsMetaDatasArr[sctID].pxlCount, sctsMetaDatasArr[sctID].singlePxlCtrl);

  uint8_t  freedBlocks = actualBlockCount - sctNewBlockCount;
  uint16_t freedBytes  = freedBlocks * sizeof(pxl_metadata_t) * BYTE_SIZE;

  byte *overWriteDestAddr   = (byte*)(pxlMetaDataPtrArr[sctID] + sctNewBlockCount);
  byte *overWriteSourceAddr = (byte*)(pxlMetaDataPtrArr[sctID] + sctNewBlockCount) + freedBytes;
  byte *memClearAddr        = (byte*)pxlMetaDataPtr - freedBytes;

  uint8_t blocksToShift = 0;

  pxlMetaDataPtr = (pxl_metadata_t*) memClearAddr;

  // Calculating number of pxl_metadata_t obj to shift in heap and then to how many bytes that amounts
  // While iterating, also changing the ptr addr contained in the array of ptr to pixel info
  for (uint8_t i = sctID + 1; i < *sctIdxTrackerPtr; i++) {
    blocksToShift += sctMemBlocksUsage(sctsMetaDatasArr[i].pxlCount, sctsMetaDatasArr[i].singlePxlCtrl);
    // blocksToShift += sctsMetaDatasArr[i].pxlCount;
    pxlMetaDataPtrArr[i] -= freedBlocks;
  }
  
  uint16_t bytesToShift = blocksToShift * sizeof(pxl_metadata_t) * BYTE_SIZE;

  // This where the overwriting is done
  while (bytesToShift) {
    *overWriteDestAddr = *overWriteSourceAddr;
    if (overWriteSourceAddr == memClearAddr) {
      *memClearAddr = 0x00;
      memClearAddr += BYTE_SIZE;
    }
    overWriteDestAddr += BYTE_SIZE;
    overWriteSourceAddr += BYTE_SIZE;
    bytesToShift--;
  }
}


/// @brief Adds mem blocks in any given section. It's done in a bare-metal kind of way,
///        which is by shifting affected bytes in the heap. The number of bytes to shift
///        is dependent upon block size (pxl_metadata_t byte size) and how many blocks
///        there are to shift. Once the last address of the new pixel blocks is reached,
///        existing bytes are overwritten with 0x00 to prevent any strange behavior.
/// @param sctID Index of the section to add memory blocks to
/// @param sctNewBlockCount New increased block count (must be higher than original)
void writeSctMemBlocks(uint8_t sctID, uint8_t sctNewBlockCount) {

  uint8_t actualBlockCount = sctMemBlocksUsage(sctsMetaDatasArr[sctID].pxlCount, sctsMetaDatasArr[sctID].singlePxlCtrl);

  uint8_t  assignedBlocks = sctNewBlockCount - actualBlockCount;
  uint16_t assignedBytes  = assignedBlocks * sizeof(pxl_metadata_t) * BYTE_SIZE;

  byte *overWriteDestAddr   = (byte*)pxlMetaDataPtr + assignedBytes - BYTE_SIZE;                   // This addr should be the last of the heap where there will be data
  byte *overWriteSourceAddr = (byte*)pxlMetaDataPtr - BYTE_SIZE;                                   // Last addr where there is actual info
  byte *memClearAddr        = (byte*)(pxlMetaDataPtrArr[sctID] + sctNewBlockCount) - BYTE_SIZE;    // First addr where to erase the bytes to make room

  uint8_t blocksToShift = 0;

  pxlMetaDataPtr = (pxl_metadata_t*) (overWriteDestAddr + BYTE_SIZE);

  // Calculating number of pxl_metadata_t obj to shift in heap and then to how many bytes that amounts
  for(uint8_t i = sctID + 1; i < *sctIdxTrackerPtr; i++) {
    blocksToShift += sctMemBlocksUsage(sctsMetaDatasArr[i].pxlCount, sctsMetaDatasArr[i].singlePxlCtrl);
    // blocksToShift += sctsMetaDatasArr[i].pxlCount;
    pxlMetaDataPtrArr[i] += assignedBlocks;
  }

  uint16_t bytesToShift = blocksToShift * sizeof(pxl_metadata_t) * BYTE_SIZE;

  // This where the overwriting is done
  while(bytesToShift) {
    *overWriteDestAddr = *overWriteSourceAddr;
    if(overWriteSourceAddr == memClearAddr) {
      *memClearAddr = 0x00;
      memClearAddr -= BYTE_SIZE;
    }
    overWriteDestAddr -= BYTE_SIZE;
    overWriteSourceAddr -= BYTE_SIZE;
    bytesToShift--;
  }
  updtPxlsMetaData(sctID, sctNewBlockCount, actualBlockCount);
}


//**********    LOCAL FUNCTIONS DEFINITION   ************//





//**********    GLOBAL FUNC DEFINITION   ************//


/// @brief Create section according to data contained in packet
/// @param sctIdx Index of the section to create (follows the 'assigned'
///               attribute of the sctMgmtMetaData)
/// @param sctMetaDataPckt Necessary info to initiate section
void createSection(byte sctIdx, sct_metadata_t sctMetaDataPckt) {
  
  uint8_t memBlocks = (uint8_t) sctMemBlocksUsage(sctMetaDataPckt.pxlCount, sctMetaDataPckt.singlePxlCtrl);
  
  if (remainingHeapSpace(memBlocks) && remainingSctsPins() && sctIdx == *sctIdxTrackerPtr) {
    pxlMetaDataPtrArr[sctIdx] = pxlMetaDataPtr;
    pxlMetaDataPtr += memBlocks;

    if (!sctsMetaDatasArr[sctIdx].pxlCount) {
      sctsMetaDatasArr[sctIdx] = sctMetaDataPckt;
    }

    updtPxlsMetaData(sctIdx, memBlocks);
    updtNeoPxlObj(&neopxlObjArr[sctIdx], sctsMetaDatasArr[sctIdx].pxlCount, sctsMetaDatasArr[sctIdx].brightness);
    updtPxlsMgmtMetaData((int8_t) memBlocks);
    sectionsMgmtAdd();

    neopxlObjArr[sctIdx].begin();

    //**debug**//
    stripColorFill(sctIdx, 0x000000A9);
    //**debug**//
  }
}


/// @brief Called to edit an existing section's metadatas
/// @param sctIdx          Index of the section to edit
/// @param sctMetaDataPckt All data that has been edited about the sct is
///                        contained in this input param
void editSection(byte sctIdx, sct_metadata_t sctMetaDataPckt) {

  //**debug**//
  stripOFF(sctIdx);
  //**debug**//

  uint8_t newMemBlockCount = sctMemBlocksUsage(sctMetaDataPckt.pxlCount, sctMetaDataPckt.singlePxlCtrl);
  uint8_t actualMemBlockCount = sctMemBlocksUsage(sctsMetaDatasArr[sctIdx].pxlCount, sctsMetaDatasArr[sctIdx].singlePxlCtrl);
  int8_t blockCountDiff = newMemBlockCount - actualMemBlockCount;
  uint8_t blockOpSignBit = (blockCountDiff & 0x80) >> 7;

  void (*memBlocksModFuncPtrArr[2]) (uint8_t, uint8_t) = {
    writeSctMemBlocks,
    eraseSctMemBlocks
  };

  bool blockOpCondCheck[2] = {
    remainingHeapSpace(blockCountDiff),
    (~blockCountDiff + 1) <= getBrdMgmtMetaDatasPtr().pxlsMgmtMetaDataPtr->assigned
  };

  if (blockOpCondCheck[blockOpSignBit]) {
    memBlocksModFuncPtrArr[blockOpSignBit] (sctIdx, newMemBlockCount);
    sctsMetaDatasArr[sctIdx] = sctMetaDataPckt;
    updtNeoPxlObj(&neopxlObjArr[sctIdx], sctMetaDataPckt.pxlCount, sctMetaDataPckt.brightness);
    updtPxlsMgmtMetaData(blockCountDiff);
  }

  // if ((blockCountDiff & 0x80) && (~blockCountDiff + 1) <= getBrdMgmtMetaDatasPtr().pxlsMgmtMetaDataPtr->assigned) {
  //   eraseSctMemBlocks(sctIdx, newMemBlockCount);
  // }
  // else if (blockCountDiff && remainingHeapSpace(blockCountDiff)) {
  //   writeSctMemBlocks(sctIdx, newMemBlockCount);
  // }
  // sctsMetaDatasArr[sctIdx] = sctMetaDataPckt;
  // updtNeoPxlObj(&neopxlObjArr[sctIdx], sctMetaDataPckt.pxlCount, sctMetaDataPckt.brightness);
  // updtPxlsMgmtMetaData(blockCountDiff);

  //**debug**//
  uint32_t color = (random(0x00, 0xFF) << 24 | random(0x00, 0xFF) << 16 | random(0x00, 0xFF) << 8 | 0x00);
  stripColorFill(sctIdx, color);
  //**debug**//
}


/// @brief Delete an existing section. By doing so, all dowstream sections
///        are down shifted so as not to create a hole in the lighting strip
///        and for easier indexing. For example, if there are 5 sections and
///        the user decides to erase section at index 2 (middle one), then scts
///        w/ idxs 3 & 4 are kept by editing the attributes of sct idx 2 w/ those
///        of sct idx 3 and so on. The delete is really only done on the last existing 
///        sct, the one w/ idx = 4
/// @param sctIdx Index of the section to erase
/// @param sctMetaDataPckt Data representing a section erasure (all attr. are set to 0).
///                        Really only used for consistency w/ createSection & editSection
///                        functions.
void deleteSection(byte sctIdx, sct_metadata_t sctMetaDataPckt) {
  
  if (sctIdx < *sctIdxTrackerPtr && sctMemBlocksUsage(sctsMetaDatasArr[sctIdx].pxlCount, sctsMetaDatasArr[sctIdx].singlePxlCtrl)) {
    
    //**debug**//
    stripOFF(sctIdx);
    //**debug**//
    
    eraseSctMemBlocks(sctIdx, 0);
    
    neopxlObjArr[sctIdx].updateLength((uint16_t) 0);
    pixelsMgmtRemove(sctsMetaDatasArr[sctIdx].pxlCount);
    sctsMetaDatasArr[sctIdx].pxlCount = 0;
    
    sectionsMgmtRemove();
  }
}


//**********    GLOBAL FUNC DEFINITION   ************//







/// @brief Edit the pixel count attribute of a particular section
/// @param sctID Affected section's index
/// @param newPxlCount Pixel count to be updated in affected section
// void editPxlCount(uint8_t sctID, uint8_t newPxlCount) {
//   if(sctID < *sctIdxTrackerPtr && remainingHeapSpace(newPxlCount)) {
//     if(newPxlCount < sctsMetaDatasArr[sctID].pxlCount) {
//       eraseSctMemBlocks(sctID, newPxlCount);
//     }
//     else if(newPxlCount > sctsMetaDatasArr[sctID].pxlCount) {
//       writeSctMemBlocks(sctID, newPxlCount);
//     }
//   }
// }


/// @brief Function that returns the sct_metadata_t obj at the 
///        position given by the index parameter
/// @param index position of the sct_metadata_t obj to return
/// @return a (pointer) sct_metadata_t obj
sct_metadata_t getSctMetaDatas(uint8_t index) {
  return sctsMetaDatasArr[index];
}


/// @brief Function to get the address of the first index of
///        of the sctsMetaDatasArr
/// @return Pointer to the first item of sctsMetaDatasArr array
sct_metadata_t* getSctMetaDatasPtr() {
  return sctsMetaDatasArr;
}


// This is a temporary func to change a pxl hsvTarget attr.
void updatingPixelAttr(uint8_t section, uint8_t pixel, uint32_t whatev) {
  
  // Got to check if the pixel number asked for is part of the section
  if(pixel < neopxlObjArr[section].numPixels()) {
    (pxlMetaDataPtrArr[section] + pixel)->hsvTarget = whatev;
  }
  else {
    Serial.println("pixel passed is out of range");
  }
}


/// @brief Save user configuration into EEPROM for future
///        boot-up use.
void sctsConfigSave() {
  eepromWriteChap(sctsMetaDataChap, (byte*) sctsMetaDatasArr);
}


/// @brief Reads and extract the saved sections configuration
///        in EEPROM if there is one. Called at boot-up.
void sctsConfigRead() {
  if (getChapStatusIndic(sctsMetaDataChap)) {
    eepromReadChap(sctsMetaDataChap, (byte*) sctsMetaDatasArr);
    setupFromSave();
  }
}


/// @brief Simple reset eeprom sct infos chap (for debug purposes)
void sctsConfigRst() {
  if (getChapStatusIndic(sctsMetaDataChap)) {
    eepromReset(sctsMetaDataChap);
  }
}


/// @brief Setup board from a peviously saved configuration
void setupFromSave() {
  for (uint8_t idx = 0; idx < getChapStatusIndic(sctsMetaDataChap); idx++) {
    createSection(idx, sctsMetaDatasArr[idx]);
    // uint8_t memBlocks = (uint8_t) sctMemBlocksUsage(sctsMetaDatasArr[idx].pxlCount, sctsMetaDatasArr[idx].singlePxlCtrl);
    // updtPxlInst(memBlocks);
    // updtMgmtMetaData(memBlocks);
  }
}












// Setup section pixel light-up indicator
// //**debug**//
// for(uint8_t i = 0; i < pxlCount; i++) {
//   pxlColorOut(sctIndexTracker, i, 0x3B659C00);
// }
// //**debug**//


// Clear section pixel light-up indicator
// //**debug**//
// stripOFF(section);

// for(uint8_t i = 0; i < sctsMetaDatasArr[section].pxlCount; i++) {
//   pxlColorOut(section, i, (pxlMetaDataPtrArr[section])->rgbwColor);
// }
// //**debug**//


// Edit section REMOVING pixel light-up indicator
// //**debug**//
// stripOFF(section);
// //**debug**//

// eraseSctMemBlocks(section, newPxlCount);

// //**debug**//
// for(uint8_t i = 0; i < sctsMetaDatasArr[section].pxlCount; i++) {
//   pxlColorOut(section, i, 0xFC7F0300);
// }
// //**debug**//


// Edit section ADDING pixel light-up indicator
// //**debug**//
// stripOFF(section);

// for(uint8_t i = 0; i < sctsMetaDatasArr[section].pxlCount; i++) {
//   pxlColorOut(section, i, 0x0000005F);
// }
// //**debug**//











//************************************ FOLLOWING TO BE MOVED INTO 'LED_ACTIONS' FILES ************************************//


//******   PIXEL ACTIONS SECTION   ******//


void pxlStateUpdt(uint8_t section, uint8_t pixel, pixel_state_t state) {
  // First, maybe check if I have to reset the ongoing action times
  if((pxlMetaDataPtrArr[section] + pixel)->pxlActionStart.actionOneStart) {
    
    // func to erase action thingies

  }
  
  // changing state
  (pxlMetaDataPtrArr[section] + pixel)->pxlState = state;
}

// Function that can be called to update either one of the color attribute of a pixel
// If the HSV color format is passed, the associated bool should be passed as True
// Func updates the actual color attr by default. For updating the target color attr, bool should be passed as True
void pxlColorUpdt(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat, bool targetUpdt) {
  
  if(hsvFormat) {
    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;

    if(targetUpdt) {
      (pxlMetaDataPtrArr[section] + pixel)->rgbwTarget = wrgb2rgbw(neopxlObjArr[section].ColorHSV(hue, sat, val));
      (pxlMetaDataPtrArr[section] + pixel)->hsvTarget = color;
    }
    else {
      (pxlMetaDataPtrArr[section] + pixel)->rgbwColor = wrgb2rgbw(neopxlObjArr[section].ColorHSV(hue, sat, val));
      (pxlMetaDataPtrArr[section] + pixel)->hsvColor = color;
    }
  }

  else {
    if(targetUpdt) {
      (pxlMetaDataPtrArr[section] + pixel)->rgbwTarget = color;
      (pxlMetaDataPtrArr[section] + pixel)->hsvTarget = rgbw2hsv(color);
    }
    else {
      (pxlMetaDataPtrArr[section] + pixel)->rgbwColor = color;
      (pxlMetaDataPtrArr[section] + pixel)->hsvColor = rgbw2hsv(color);
    }
  }
}

// simple function to output a chosen color to a pixel
// color can be of either HSV or RGB format
// gamma32 correction is applied on the color outputted to the strip
void pxlColorOut(uint8_t section, uint8_t pixel, uint32_t color, bool hsvFormat) {
  
  if(hsvFormat) {
    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;

    pxlColorUpdt(section, pixel, color, hsvFormat);
    neopxlObjArr[section].setPixelColor(pixel, neopxlObjArr[section].gamma32(neopxlObjArr[section].ColorHSV(hue, sat, val)));
    neopxlObjArr[section].show();
  }
  else {
    pxlColorUpdt(section, pixel, color);
    neopxlObjArr[section].setPixelColor(pixel, neopxlObjArr[section].gamma32(rgbw2wrgb(color)));
    neopxlObjArr[section].show();
  }
}

// turn off a single pixel
void pxlOFF(uint8_t section, uint8_t pixel) {
  pxlColorUpdt(section, pixel, 0x00000000);
  neopxlObjArr[section].setPixelColor(pixel, 0x00000000);
  neopxlObjArr[section].show();
}


// //******   PIXEL ACTIONS SECTION   ******//



// //******   STRIP ACTIONS SECTION   ******//


// lights a whole strip with the color passed as input
// gamma32 correction is applied on the color outputted to the strip
void stripColorFill(uint8_t section, uint32_t color, bool hsvFormat) {
  
  if(hsvFormat) {  
    uint16_t hue = (uint16_t)((color & 0xFFFF0000) >> 16);
    uint8_t  sat = (uint8_t) ((color & 0x0000FF00) >>  8);
    uint8_t  val = (uint8_t)  (color & 0x000000FF)       ;
    
    for(uint8_t pixel = 0; pixel < sctMemBlocksUsage(sctsMetaDatasArr[section].pxlCount, sctsMetaDatasArr[section].singlePxlCtrl); pixel++) {
      pxlColorUpdt(section, pixel, color, hsvFormat);
    }
    neopxlObjArr[section].fill(neopxlObjArr[section].gamma32(neopxlObjArr[section].ColorHSV(hue, sat, val)));
    neopxlObjArr[section].show();
  }
  else {
    for(uint8_t pixel = 0; pixel < sctMemBlocksUsage(sctsMetaDatasArr[section].pxlCount, sctsMetaDatasArr[section].singlePxlCtrl); pixel++) {
      pxlColorUpdt(section, pixel, color);
    }
    neopxlObjArr[section].fill(neopxlObjArr[section].gamma32(rgbw2wrgb(color)));
    neopxlObjArr[section].show();
  }
}

// turn OFF all LEDs in a given strip (section)
void stripOFF(uint8_t section) {
  for(uint8_t pixel = 0; pixel < sctMemBlocksUsage(sctsMetaDatasArr[section].pxlCount, sctsMetaDatasArr[section].singlePxlCtrl); pixel++) {
      pxlColorUpdt(section, pixel, 0x00000000);
  }
  neopxlObjArr[section].clear();
  neopxlObjArr[section].show();  
}


// //******   STRIP ACTIONS SECTION   ******//
