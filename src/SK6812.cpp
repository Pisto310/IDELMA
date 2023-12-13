/*
The LED pixel strips are instanciated here and scenes are also expanded upon
*/


#include "SK6812.h"


//************************************************************    LOCAL VARIABLES DECLARATION   **************************************************************//

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

//************************************************************    LOCAL VARIABLES DECLARATION   **************************************************************//










//************************************************************    GLOBAL VARIABLES DECLARATION   ************************************************************//



//************************************************************    GLOBAL VARIABLES DECLARATION   ************************************************************//









//************************************************************    LOCAL FUNCTIONS DECLARATION   **************************************************************//

// void updtPxlRgbwColor(pxl_metadata_t* pxlMetaDataPtr, uint32_t rgbwColor);
// void updtPxlHsvColor(pxl_metadata_t* pxlMetaDataPtr, uint32_t hsvColor);

// void pxlRgbwColorOut(Adafruit_NeoPixel* neoPxlObj, pxl_metadata_t* pxlMetaDataPtr, uint32_t rgbwColor);
// void pxlHsvColorOut(Adafruit_NeoPixel* neoPxlObj, pxl_metadata_t* pxlMetaDataPtr, uint32_t hsvColor);
// void pxlOFF(Adafruit_NeoPixel* neoPxlObj, pxl_metadata_t* pxlMetaDataPtr);

// void stripRgbwColorFill(Adafruit_NeoPixel* neoPxlObj, sct_metadata_t* sctMetaDataPtr, pxl_metadata_t* pxlMetaDataPtr, uint32_t rgbwColor);
// void stripHsvColorFill(Adafruit_NeoPixel* neoPxlObj, sct_metadata_t* sctMetaDataPtr, pxl_metadata_t* pxlMetaDataPtr, uint32_t hsvColor);
// void stripOFF(Adafruit_NeoPixel* neoPxlObj, sct_metadata_t* sctMetaDataPtr, pxl_metadata_t* pxlMetaDataPtr);

//************************************************************    LOCAL FUNCTIONS DECLARATION   **************************************************************//










//*************************************************************    LOCAL FUNCTIONS DEFINITION   *************************************************************//

/// @brief Setup board from a peviously saved configuration
void setupFromSave() {
  for (uint8_t idx = 0; idx < getChapStatusIndic(sctsMetaDataChap); idx++) {
    createSection(idx, sctsMetaDatasArr[idx]);
  }
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


/// @brief Update the RGBW color attribute of a pixel
/// @param pxlMetaDataPtr Pointer to a pxl_metadata_t obj 
/// @param rgbwColor RGBW formatted color to update pixel's metadata with
void updtPxlRgbwColor(pxl_metadata_t* pxlMetaDataPtr, uint32_t rgbwColor) {
  pxlMetaDataPtr->rgbwColor = rgbwColor;
}


/// @brief Update the HSV color attribute of a pixel
/// @param pxlMetaDataPtr Pointer to a pxl_metadata_t obj 
/// @param rgbwColor HSV formatted color to update pixel's metadata with
void updtPxlHsvColor(pxl_metadata_t* pxlMetaDataPtr, uint32_t hsvColor) {
  pxlMetaDataPtr->hsvColor = hsvColor;
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


/// @brief Output chosen RGBW formatted color to a pixel.
///        Gamma32 correction applied
/// @param neoPxlObj Pointer to a NeoPixel obj
/// @param pxlMetaDataPtr Pointer to a pxl_metadata_t obj
/// @param rgbwColor RGBW formatted color to update pixel's metadata with
void pxlRgbwColorOut(Adafruit_NeoPixel* neoPxlObj, pxl_metadata_t* pxlMetaDataPtr, uint32_t rgbwColor) {
  
  uint32_t hsvColor = rgbw2hsv(rgbwColor);
  uint32_t gammaColor = (*neoPxlObj).gamma32(rgbw2wrgb(rgbwColor));

  updtPxlRgbwColor(pxlMetaDataPtr, rgbwColor);
  updtPxlHsvColor(pxlMetaDataPtr, hsvColor);
  (*neoPxlObj).setPixelColor(pxlMetaDataPtr->pxlID, gammaColor);
  (*neoPxlObj).show();
}


/// @brief Output chosen HSV formatted color to a pixel.
///        Gamma32 correction applied
/// @param neoPxlObj Pointer to a NeoPixel obj
/// @param pxlMetaDataPtr Pointer to a pxl_metadata_t obj
/// @param hsvColor HSV formatted color to update pixel's metadata with
void pxlHsvColorOut(Adafruit_NeoPixel* neoPxlObj, pxl_metadata_t* pxlMetaDataPtr, uint32_t hsvColor) {
  
  uint32_t rgbwColor = wrgb2rgbw((*neoPxlObj).ColorHSV((uint16_t) (hsvColor >> 16), (uint8_t) (hsvColor >> 8), (uint8_t) hsvColor));
  uint32_t gammaColor = (*neoPxlObj).gamma32(rgbw2wrgb(rgbwColor));
  
  updtPxlRgbwColor(pxlMetaDataPtr, rgbwColor);
  updtPxlHsvColor(pxlMetaDataPtr, hsvColor);
  (*neoPxlObj).setPixelColor(pxlMetaDataPtr->pxlID, gammaColor);
  (*neoPxlObj).show();
}


/// @brief Turn off a single pixel. Only calls 'pxlRgbColorOut' with
///        the 'black (0x00000000)' color as an input variable
/// @param neoPxlObj Pointer to a NeoPixel obj
/// @param pxlMetaDataPtr Pointer to a pxl_metadata_t obj
void pxlOFF(Adafruit_NeoPixel* neoPxlObj, pxl_metadata_t* pxlMetaDataPtr) {
  pxlRgbwColorOut(neoPxlObj, pxlMetaDataPtr, 0x00000000);
}


/// @brief Light all the pixels in a section the given RGBW formatted color
/// @param neoPxlObj Pointer to the NeoPixel obj to color fill
/// @param sctMetaDataPtr Pointer to the sct_metadata_t obj to color fill
/// @param pxlMetaDataPtr Pointer to the first pxl_metadata_t obj of the
///                       section
/// @param rgbwColor RGBW formatted color to output to strip's pixels
void stripRgbwColorFill(Adafruit_NeoPixel* neoPxlObj, sct_metadata_t* sctMetaDataPtr, pxl_metadata_t* pxlMetaDataPtr, uint32_t rgbwColor) {
  
  uint32_t hsvColor = rgbw2hsv(rgbwColor);
  uint32_t gammaColor = (*neoPxlObj).gamma32(rgbw2wrgb(rgbwColor));
  
  for (uint8_t pxlIdx = 0; pxlIdx < sctMemBlocksUsage((*sctMetaDataPtr).pxlCount, (*sctMetaDataPtr).singlePxlCtrl); pxlIdx++) {
    updtPxlRgbwColor((pxlMetaDataPtr + pxlIdx), rgbwColor);
    updtPxlHsvColor((pxlMetaDataPtr + pxlIdx), hsvColor);
  }

  (*neoPxlObj).fill(gammaColor);
  (*neoPxlObj).show();
}


/// @brief Light all the pixels in a section the given HSV formatted color
/// @param neoPxlObj Pointer to the NeoPixel obj to color fill
/// @param sctMetaDataPtr Pointer to the sct_metadata_t obj to color fill
/// @param pxlMetaDataPtr Pointer to the first pxl_metadata_t obj of the
///                       section
/// @param rgbwColor HSV formatted color to output to strip's pixels
void stripHsvColorFill(Adafruit_NeoPixel* neoPxlObj, sct_metadata_t* sctMetaDataPtr, pxl_metadata_t* pxlMetaDataPtr, uint32_t hsvColor) {
  
  uint32_t rgbwColor = wrgb2rgbw((*neoPxlObj).ColorHSV((uint16_t) (hsvColor >> 16), (uint8_t) (hsvColor >> 8), (uint8_t) hsvColor));
  uint32_t gammaColor = (*neoPxlObj).gamma32((*neoPxlObj).ColorHSV((uint16_t) (hsvColor >> 16), (uint8_t) (hsvColor >> 8), (uint8_t) hsvColor));
  
  for (uint8_t pxlIdx = 0; pxlIdx < sctMemBlocksUsage((*sctMetaDataPtr).pxlCount, (*sctMetaDataPtr).singlePxlCtrl); pxlIdx++) {
    updtPxlHsvColor((pxlMetaDataPtr + pxlIdx), hsvColor);
    updtPxlRgbwColor((pxlMetaDataPtr + pxlIdx), rgbwColor);
  }

  (*neoPxlObj).fill(gammaColor);
  (*neoPxlObj).show();
}


// turn OFF all LEDs in a given strip (section)
void stripOFF(Adafruit_NeoPixel* neoPxlObj, sct_metadata_t* sctMetaDataPtr, pxl_metadata_t* pxlMetaDataPtr) {
  stripRgbwColorFill(neoPxlObj, sctMetaDataPtr, pxlMetaDataPtr, 0x00000000);
}


//************************************************************    LOCAL FUNCTIONS DEFINITION   ************************************************************//










//**************************************************************    GLOBAL FUNC DEFINITION   **************************************************************//


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
    stripRgbwColorFill(&neopxlObjArr[sctIdx], &sctsMetaDatasArr[sctIdx], pxlMetaDataPtrArr[sctIdx], 0x000000A9);
    // stripColorFill(sctIdx, 0x000000A9);
    //**debug**//
  }
}


/// @brief Called to edit an existing section's metadatas
/// @param sctIdx          Index of the section to edit
/// @param sctMetaDataPckt All data that has been edited about the sct is
///                        contained in this input param
void editSection(byte sctIdx, sct_metadata_t sctMetaDataPckt) {

  //**debug**//
  stripOFF(&neopxlObjArr[sctIdx], &sctsMetaDatasArr[sctIdx], pxlMetaDataPtrArr[sctIdx]);
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

  //**debug**//
  uint32_t color = (random(0x00, 0xFF) << 24 | random(0x00, 0xFF) << 16 | random(0x00, 0xFF) << 8 | 0x00);
  stripRgbwColorFill(&neopxlObjArr[sctIdx], &sctsMetaDatasArr[sctIdx], pxlMetaDataPtrArr[sctIdx], color);
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

  if (sctsMetaDatasArr[sctIdx].pxlCount && sctMemBlocksUsage(sctsMetaDatasArr[sctIdx].pxlCount, sctsMetaDatasArr[sctIdx].singlePxlCtrl)) {
    
    //**debug**//
    stripOFF(&neopxlObjArr[sctIdx], &sctsMetaDatasArr[sctIdx], pxlMetaDataPtrArr[sctIdx]);
    //**debug**//
    
    eraseSctMemBlocks(sctIdx, 0);
    updtNeoPxlObj(&neopxlObjArr[sctIdx], 0, 0);
    updtPxlsMgmtMetaData(0 - sctMemBlocksUsage(sctsMetaDatasArr[sctIdx].pxlCount, sctsMetaDatasArr[sctIdx].singlePxlCtrl));
    sectionsMgmtRemove();
    sctsMetaDatasArr[sctIdx] = sctMetaDataPckt;
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


/// @brief Function to get the address of the first index of
///        of the sctsMetaDatasArr
/// @return Pointer to the first item of sctsMetaDatasArr array
sct_metadata_t* getSctMetaDatasPtr() {
  return sctsMetaDatasArr;
}


/// @brief Indicates the number of memory block needed or used for
///        a given section
/// @param pxlCount Number of pixel to be lit up on the strip
/// @param singlePxlCtrl Indicating if whole strip is seen as a single pixel
///                      (Acting as a bool)
/// @return Required amount of blocks needed or used in heap
byte sctMemBlocksUsage(byte pxlCount, byte singlePxlCtrl) {
  return singlePxlCtrl ? singlePxlCtrl : pxlCount;
}


//**************************************************************    GLOBAL FUNC DEFINITION   **************************************************************//










//**************************************************************    DEBUG   **************************************************************//

/// @brief Simple reset eeprom sct infos chap (for debug purposes)
void sctsConfigRst() {
  if (getChapStatusIndic(sctsMetaDataChap)) {
    eepromReset(sctsMetaDataChap);
  }
}

void allOff() {
  for (uint8_t i = 0; i < *sctIdxTrackerPtr; i++) {
    stripOFF(&neopxlObjArr[i], &sctsMetaDatasArr[i], pxlMetaDataPtrArr[i]);
  }
}

//**************************************************************    DEBUG   **************************************************************//


// This is a temporary func to change a pxl hsvTarget attr.
// void updatingPixelAttr(uint8_t section, uint8_t pixel, uint32_t whatev) {
  
//   // Got to check if the pixel number asked for is part of the section
//   if(pixel < neopxlObjArr[section].numPixels()) {
//     (pxlMetaDataPtrArr[section] + pixel)->hsvTarget = whatev;
//   }
//   else {
//     Serial.println("pixel passed is out of range");
//   }
// }





// void pxlStateUpdt(uint8_t section, uint8_t pixel, pixel_state_t state) {
//   // First, maybe check if I have to reset the ongoing action times
//   if((pxlMetaDataPtrArr[section] + pixel)->pxlActionStart.actionOneStart) {
    
//     // func to erase action thingies

//   }
  
//   // changing state
//   (pxlMetaDataPtrArr[section] + pixel)->pxlState = state;
// }







// // //******   PIXEL ACTIONS SECTION   ******//



// // Initialize a fade action using the HSV color space for a pixel in a specific section
// // target color passed as argument is of the 0xRRGGBBWW format
// void hsvFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, int32_t fadeTime) {
  
//   // transition from RGB to HSV color space for actual and target color
//   uint32_t actualHSV = rgbw2hsv(stripsArrayOfPxl[section][pixel].rgbwColor);
//   uint32_t targetHSV = rgbw2hsv(targetRGB);

//   // extract hue, sat & val from actual and target colors
//   uint16_t actualHue = (uint16_t)((actualHSV & 0xFFFF0000) >> 16);
//   uint8_t  actualSat = (uint8_t) ((actualHSV & 0x0000FF00) >>  8);
//   uint8_t  actualVal = (uint8_t)  (actualHSV & 0x000000FF)       ;

//   uint16_t targetHue = (uint16_t)((targetHSV & 0xFFFF0000) >> 16);
//   uint8_t  targetSat = (uint8_t) ((targetHSV & 0x0000FF00) >>  8);
//   uint8_t  targetVal = (uint8_t)  (targetHSV & 0x000000FF)       ;

//   // Since hue is representative of a circle's angle, we want to find the way of rotation
//   // with the shorter length to minimize the variety of colors in the fade
//   int16_t hueDelta = 0;

//   if(targetHue - actualHue == 32768 || actualHue - targetHue == 32768) {
//     // means the target is exactly half a circle away, direction has no importance
//     hueDelta = targetHue - actualHue;
//   }
//   else if(targetHue > actualHue) {
//     if((targetHue - actualHue) < 32768) {
//       // clockwise rotation, delta is positive
//       hueDelta = targetHue - actualHue;
//     }
//     else if((targetHue - actualHue) > 32768) {
//       // counter-clockwise rotation, delta is negative
//       hueDelta = targetHue - actualHue - 65535;
//     }
//   }
//   else if(actualHue > targetHue) {
//     if((actualHue - targetHue) > 32768) {
//       // clockwise rotation, delta is positive
//       hueDelta = 65535 + targetHue - actualHue;
//     }
//     else if((actualHue - targetHue) < 32768) {
//       // counter-clockwise rotation, delta is negative
//       hueDelta = targetHue - actualHue;
//     }
//   }

//   // calculations for saturation and value deltas
//   int16_t satDelta = (int16_t)(targetSat - actualSat);
//   int16_t valDelta = (int16_t)(targetVal - actualVal);

//   // steps are calculated and expressed in ms/bit, except for hue, where the unit is ms/43bits
//   // since deltas may be negative, the values are signed 32-bit
//   int32_t hueSteps;
//   int32_t satSteps;
//   int32_t valSteps;

//   hueDelta ? hueSteps = ((float)fadeTime / hueDelta) * 43 : hueSteps = 0;
//   satDelta ? satSteps =         fadeTime / satDelta       : satSteps = 0;
//   valDelta ? valSteps =         fadeTime / valDelta       : valSteps = 0;

//   // assigning step time to pixel attributes
//   stripsArrayOfPxl[section][pixel].actionOneTime   = hueSteps;
//   stripsArrayOfPxl[section][pixel].actionTwoTime   = satSteps;
//   stripsArrayOfPxl[section][pixel].actionThreeTime = valSteps;

//   // assigning start time of each to pixel attributes
//   stripsArrayOfPxl[section][pixel].actionOneStart   = millis();
//   stripsArrayOfPxl[section][pixel].actionTwoStart   = millis();
//   stripsArrayOfPxl[section][pixel].actionThreeStart = millis();

//   // changing state of pixel and updating targetColor attribute
//   stripsArrayOfPxl[section][pixel].pxlState = HSV_FADE;
//   pxlColorUpdt(section, pixel, targetHSV, 1, 1);
// }


// // function called in the pixel iterator to update the hsv values
// void hsvFade(uint8_t section, uint8_t pixel) {

//   // extracting actual pixel color and assigning to the next HSV to output as starting point
//   uint32_t actualHSV = (stripsArrayOfPxl[section][pixel].hsvColor);
//   uint16_t nextHue   = (int16_t)((actualHSV & 0xFFFF0000) >> 16);
//   uint8_t  nextSat   = (int8_t) ((actualHSV & 0x0000FF00) >>  8);
//   uint8_t  nextVal   = (int8_t)  (actualHSV & 0x000000FF)       ;

//   if(millis() - stripsArrayOfPxl[section][pixel].actionOneStart >= absVar(stripsArrayOfPxl[section][pixel].actionOneTime) && stripsArrayOfPxl[section][pixel].actionOneTime != 0) {
//     uint16_t targetHue = (uint16_t)((stripsArrayOfPxl[section][pixel].hsvTarget & 0xFFFF0000) >> 16);
//     if(stripsArrayOfPxl[section][pixel].actionOneTime & 0x80000000) {
//       nextHue -= 43;                                                              // the steps are negative, we need to decrement
//       if(absVar(nextHue - targetHue) <= 43) {
//         stripsArrayOfPxl[section][pixel].actionOneTime = 0;                       // target is reached, no need to come back in statement again
//         nextHue = targetHue;
//       }
//       else {
//         stripsArrayOfPxl[section][pixel].actionOneStart = millis();
//       }
//     }
//     else {
//       nextHue += 43;                                                              // steps are positive, we increment
//       if(absVar(nextHue - targetHue) <= 43) {
//         stripsArrayOfPxl[section][pixel].actionOneTime = 0;                       // target is reached, no need to come back in statement again
//         nextHue = targetHue;
//       }
//       else {
//         stripsArrayOfPxl[section][pixel].actionOneStart = millis();
//       }
//     }
//   }

//   if(millis() - stripsArrayOfPxl[section][pixel].actionTwoStart >= absVar(stripsArrayOfPxl[section][pixel].actionTwoTime) && stripsArrayOfPxl[section][pixel].actionTwoTime != 0) {
//     uint8_t targetSat = (uint8_t)((stripsArrayOfPxl[section][pixel].hsvTarget & 0x0000FF00) >> 8);
//     nextColorVal(&nextSat, &stripsArrayOfPxl[section][pixel].actionTwoTime, &stripsArrayOfPxl[section][pixel].actionTwoStart, targetSat);
//   }

//   if(millis() - stripsArrayOfPxl[section][pixel].actionThreeStart >= absVar(stripsArrayOfPxl[section][pixel].actionThreeTime) && stripsArrayOfPxl[section][pixel].actionThreeTime != 0) {
//     uint8_t targetVal = (uint8_t)(stripsArrayOfPxl[section][pixel].hsvTarget & 0x000000FF);
//     nextColorVal(&nextVal, &stripsArrayOfPxl[section][pixel].actionThreeTime, &stripsArrayOfPxl[section][pixel].actionThreeStart, targetVal);
//   }
//   // outputting color to strip
//   pxlColorOut(section, pixel, (uint32_t)nextHue << 16 | (uint32_t)nextSat << 8 | (uint32_t)nextVal, 1);
// }


// // fade from color to color in the RGB space using linear interpolation between actual and target value
// // it was decided this func would not treat the white bits (8-LSB) of the input target color since it isn't ncessary
// // for a white fade, use the whtFade func
// void rgbFadeInit(uint8_t section, uint8_t pixel, uint32_t targetRGB, int32_t fadeTime) {

//   // extracting individual R, G & B values from the actual color attri. of the pixel
//   uint8_t actualRed = stripsArrayOfPxl[section][pixel].rgbwColor >> 24;
//   uint8_t actualGrn = stripsArrayOfPxl[section][pixel].rgbwColor >> 16;
//   uint8_t actualBlu = stripsArrayOfPxl[section][pixel].rgbwColor >>  8;

//   // extracting individual R, G & B values from the target color
//   uint8_t targetRed = targetRGB >> 24;
//   uint8_t targetGrn = targetRGB >> 16;
//   uint8_t targetBlu = targetRGB >>  8;

//   // calculating the deltas
//   int16_t redDelta = targetRed - actualRed;
//   int16_t grnDelta = targetGrn - actualGrn;
//   int16_t bluDelta = targetBlu - actualBlu;

//   // with incrementing by a bit, each step is then expressed as a number of ms/bit
//   int32_t redSteps;
//   int32_t grnSteps;
//   int32_t bluSteps;
//   redDelta ? redSteps = fadeTime / redDelta : redSteps = 0;
//   grnDelta ? grnSteps = fadeTime / grnDelta : grnSteps = 0;
//   bluDelta ? bluSteps = fadeTime / bluDelta : bluSteps = 0;

//   // updating timing attributes of the pixel, its state and target color
//   stripsArrayOfPxl[section][pixel].actionOneTime   = redSteps;
//   stripsArrayOfPxl[section][pixel].actionTwoTime   = grnSteps;
//   stripsArrayOfPxl[section][pixel].actionThreeTime = bluSteps;

//   stripsArrayOfPxl[section][pixel].actionOneStart   = millis();
//   stripsArrayOfPxl[section][pixel].actionTwoStart   = millis();
//   stripsArrayOfPxl[section][pixel].actionThreeStart = millis();

//   pxlColorUpdt(section, pixel, rgbw2rgb(targetRGB), 0, 1);
//   stripsArrayOfPxl[section][pixel].pxlState = RGB_FADE;
// }


// void rgbFade(uint8_t section, uint8_t pixel) {
//   // using each actual R, G & B value as starting point for the next value to output
//   uint8_t nextRed = stripsArrayOfPxl[section][pixel].rgbwColor >> 24;
//   uint8_t nextGrn = stripsArrayOfPxl[section][pixel].rgbwColor >> 16;
//   uint8_t nextBlu = stripsArrayOfPxl[section][pixel].rgbwColor >>  8;

//   if(millis() - stripsArrayOfPxl[section][pixel].actionOneStart >= absVar(stripsArrayOfPxl[section][pixel].actionOneTime) && stripsArrayOfPxl[section][pixel].actionOneTime != 0) {
//     uint8_t targetRed = stripsArrayOfPxl[section][pixel].rgbwTarget >> 24;
//     nextColorVal(&nextRed, &stripsArrayOfPxl[section][pixel].actionOneTime, &stripsArrayOfPxl[section][pixel].actionOneStart, targetRed);
//   }

//   if(millis() - stripsArrayOfPxl[section][pixel].actionTwoStart >= absVar(stripsArrayOfPxl[section][pixel].actionTwoTime) && stripsArrayOfPxl[section][pixel].actionTwoTime != 0) {
//     uint8_t targetGrn = stripsArrayOfPxl[section][pixel].rgbwTarget >> 16;
//     nextColorVal(&nextGrn, &stripsArrayOfPxl[section][pixel].actionTwoTime, &stripsArrayOfPxl[section][pixel].actionTwoStart, targetGrn);
//   }

//   if(millis() - stripsArrayOfPxl[section][pixel].actionThreeStart >= absVar(stripsArrayOfPxl[section][pixel].actionThreeTime) && stripsArrayOfPxl[section][pixel].actionThreeTime != 0) {
//     uint8_t targetBlu = stripsArrayOfPxl[section][pixel].rgbwTarget >> 8;
//     nextColorVal(&nextBlu, &stripsArrayOfPxl[section][pixel].actionThreeTime, &stripsArrayOfPxl[section][pixel].actionThreeStart, targetBlu);
//   }
//   // outputting to strip
//   pxlColorOut(section, pixel, wrgb2rgbw(neopxlObjArr[section].Color(nextRed, nextGrn, nextBlu, 0)));
// }

// // func that takes care of handling the next color value for fade actions (HSV or RGB)
// void nextColorVal(uint8_t *nextColor, int32_t *actionTime, uint32_t *actionStart, uint8_t targetColor, uint8_t incrDecr) {
//   if(*actionTime & 0x80000000) {
//     *nextColor -= incrDecr;                             // the steps are negative, we need to decrement
//     if(*nextColor <= targetColor) {
//       *actionTime = 0;                                  // target is reached, no need to come back in statement again
//       *nextColor = targetColor;
//     }
//     else {
//       *actionStart = millis();
//     }
//   }
//   else {
//     *nextColor += incrDecr;                             // steps are positive, we increment
//     if(*nextColor >= targetColor) {
//       *actionTime = 0;                                  // target is reached, no need to come back in statement again
//       *nextColor = targetColor;
//     }
//     else {
//       *actionStart = millis();
//     }
//   }
// }

// void pixelActionsHandler(void) {
//   for(uint8_t sct = 0; sct < sctIndexTracker; sct++) {
//     for(uint8_t pxl = 0; pxl < sctsMetaDatasArr[sct].pxlCount; pxl++) {
//       switch ((pxlMetaDataPtrArr[sct] + pxl)->pxlState) {
      
//       default:
//         // FSM comes here if the pixel state is IDLE
//         break;
//       }
//     }
//   }
// }


// void pxlIterator(uint8_t sectionIndex) {
//   for(uint8_t section = 0; section < sectionIndex; section++) {
//     for(uint8_t pixel = 0; pixel < neopxlObjArr[section].numPixels(); pixel++) {
//       switch (stripsArrayOfPxl[section][pixel].pxlState) {
//       case HSV_FADE:
//         if(stripsArrayOfPxl[section][pixel].hsvColor != stripsArrayOfPxl[section][pixel].hsvTarget) {
//           hsvFade(section, pixel);
//         }
//         else {
//           stripsArrayOfPxl[section][pixel].pxlState = IDLE;
//           Serial.println("here");
//         }
//         break;

//       case RGB_FADE:
//         if(stripsArrayOfPxl[section][pixel].rgbwColor != stripsArrayOfPxl[section][pixel].rgbwTarget) {
//           rgbFade(section, pixel);
//         }
//         else {
//           stripsArrayOfPxl[section][pixel].pxlState = IDLE;
//           Serial.println("there");
//         }
//         break;

//       case BLINK_ONCE:
//         if(millis() - stripsArrayOfPxl[section][pixel].actionOneStart >= absVar(stripsArrayOfPxl[section][pixel].actionOneTime)) {
//         pxlOFF(section, pixel);
//         stripsArrayOfPxl[section][pixel].pxlState = IDLE;
//         }

//       case SPARKLE:
//         sparkleSct(section, pixel);
//         break;
      
//       default:
//         stripsArrayOfPxl[section][pixel].pxlState = IDLE;       // check if statement is useful
//         break;
//       }
//     }
//   }
// }
