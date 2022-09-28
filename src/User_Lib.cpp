/*
cpp file to  expand on functions declared in associate header file
*/

#include "User_Lib.h"

/*!
  @brief   Homemade delay function using millis() of Arduino lib
  @param   msDelay  the desired delay in milliseconds
  @return  none
*/
void homeMadeDelay(uint16_t msDelay) {
    // storing actual time in a var
    uint32_t startTime = millis();
    uint32_t time = startTime;
    while(time <= (startTime + msDelay)) {
        time = millis();
    }
}

void serialReadToArray(HardwareSerial &serialToRead, char *recvMess, uint8_t messLen) {
  // can only run function if the buffer of the serial to read is filled
  if(serialToRead.available()) {
    
    // the loop to empty the serial buffer
    for(uint8_t i = 0; i < messLen; i++) {
      recvMess[i] = serialToRead.read();
    }
  }
}

byte convertAsciiToHex(char ascii) {
  if(ascii > 0x39) {
    ascii -= 7;
  }
  return(ascii & 0x0f);
}

void mcuHeartBeat(uint16_t heartBeat) {
  static uint8_t ledState = 0;                  //starts at OFF
  static uint32_t saveTime = millis();

  if((millis() - saveTime) >= heartBeat) {
    saveTime = millis();
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
}

// function that returns the absolute of a 32-bit signed variable
uint32_t absVar(int32_t val) {
  if(val & 0x80000000) {
    return(~val + 1);
  }
  else {
    return(val);
  }
}

uint8_t tenTimesByteMultiplier(byte initialVal) {
  // The idea is that 10 = 8 + 2, so two left shift, one by 3 and the other by 1
  initialVal = (uint8_t) initialVal;
  return((initialVal << 3) + (initialVal << 1));
}

uint8_t hundredTimesByteMultiplier(byte initialVal) {
  // The idea is that 100 = 64 + 32 + 4. One left shift of 6 bits, one of 5 bits and the final one of one bit
  initialVal = (uint8_t) initialVal;
  return((initialVal << 6) + (initialVal << 5) + (initialVal << 1));
}

void digitSeparator(uint8_t number, uint8_t *divContainer, uint8_t *modContainer) {
  // Taking care of the division by 10
  *divContainer = pgm_read_byte(&_divByTenLookUp[number]);
  *modContainer = number - (*divContainer * 10);
}