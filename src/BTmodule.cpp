/*
This sketch Configures the name and baud rate of an HC 06 Bluetooth module 
*/

// for debugging purposes
// Serial.println(

#include "BTmodule.h"
#include "User_Lib.h"

uint16_t baudRateFinder(HardwareSerial &serialPort) {
  uint32_t baudArray[8] = {BAUD1, BAUD2, BAUD3, BAUD4, BAUD5, BAUD6, BAUD7, BAUD8};

  // declaring an array to hold the module's reply
  // the HC-06 doesn't include the NULL terminating char
  char moduleAck[strlen(AT_REPLY)];
  
  for(uint8_t i = 0; i < 8; i++) {
    // initialize the BT serial port
    serialPort.begin(baudArray[i]);
    serialPort.write(AT_SEND);

    // HC06 requires 500 msec for reply
    homeMadeDelay(600);
    serialReadToArray(serialPort, moduleAck, strlen(AT_REPLY));
      
    if(!strncmp(moduleAck, AT_REPLY, strlen(AT_REPLY))) {
      return(baudArray[i]);
    }
  }
  return(0);
}

void setupBT(HardwareSerial &serialPort, uint16_t *defaultBaud, uint16_t setBaud = BAUD4, char moduleName[7] = "IDELMA", char modulePin[5] = "3003") {
  // first is to check if defaultBaud and setBaud are not the same
  if(*defaultBaud != setBaud) {

    uint32_t baudRateArray[8] = {BAUD1, BAUD2, BAUD3, BAUD4, BAUD5, BAUD6, BAUD7, BAUD8};

    char *baudSetArray[8] = {AT_BAUD1_SET, AT_BAUD2_SET, AT_BAUD3_SET, AT_BAUD4_SET, AT_BAUD5_SET, AT_BAUD6_SET, AT_BAUD7_SET, AT_BAUD8_SET};

    char *baudReplyArray[8] = {AT_BAUD1_REPLY, AT_BAUD2_REPLY, AT_BAUD3_REPLY, AT_BAUD4_REPLY, AT_BAUD5_REPLY, AT_BAUD6_REPLY, AT_BAUD7_REPLY, AT_BAUD8_REPLY};

    for(uint8_t i = 0; i < 8; i++) {
      if(baudRateArray[i] == setBaud) {
        serialPort.write(baudSetArray[i]);

        // the set Baud has to become the default Baud
        *defaultBaud = setBaud;

        char replyBaudSet[strlen(baudReplyArray[i]) + 2];
        //serialPort.begin(*defaultBaud);

        // HC06 requires 500 msec for reply
        homeMadeDelay(600);
        serialReadToArray(serialPort, replyBaudSet, strlen(baudReplyArray[i]));

        if(!strncmp(replyBaudSet, baudReplyArray[i], strlen(baudReplyArray[i]))) {
          Serial.println(replyBaudSet);
          serialPort.end();

          // Do something else if doesn't work???

        }
      }
    }
  }
  // now to take care of the module's name
  char nameCommand[28] = AT_NAME_SET;
  char replyNameSet[strlen(AT_NAME_REPLY)];

  // let's append the name to our char array
  strncat(nameCommand, moduleName, 20);

  // sending the new name to the module
  serialPort.begin(*defaultBaud);
  homeMadeDelay(600);   // small delay between transmission
  serialPort.write(nameCommand);

  // expecting response
  homeMadeDelay(600);   // HC06 requires 500 msec for reply
  serialReadToArray(serialPort, replyNameSet, strlen(AT_NAME_REPLY));
  
  if(!strncmp(replyNameSet, AT_NAME_REPLY, strlen(AT_NAME_REPLY))) {
    Serial.println(replyNameSet);

    // Do something here???

  }
  
  // now to take care of the module's PIN
  char pinCommand[12] = AT_PIN_SET;
  char replyPinSet[strlen(AT_PIN_REPLY)];

  // let's append the pin to our char array
  strncat(pinCommand, modulePin, 5);

  // sending the new PIN to the module
  homeMadeDelay(600);   // dsmall delay between transmission
  serialPort.write(pinCommand);

  // expecting response
  homeMadeDelay(600);   // HC06 requires 500 msec for reply
  serialReadToArray(serialPort, replyPinSet, strlen(AT_PIN_REPLY));
  
  if(!strncmp(replyPinSet, AT_PIN_REPLY, strlen(AT_PIN_REPLY))) {
    Serial.println(replyPinSet);

    // Do something here???

  }
}