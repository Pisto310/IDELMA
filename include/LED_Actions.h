/*
Created on: December 7 2023
Author : J-C Rodrigue
Description : Lighting actions related to LEDs (No scenes, only single actions)
*/

#include "Board.h"
#include "Color_Formatting.h"

#ifndef LEDS_ACTIONS_H_
#define LEDS_ACTIONS_H_


//**********    GLOBAL TYPES DECLARATION   ************//

typedef enum PixelStates{
  IDLE,                                     // LED is ready for action
  HSV_FADE,                                 // LED is in the process of fading in HSV color space
  RGB_FADE,                                 // LED is in the process of fading in RGB color space
  BLINK_ONCE,                               // LED is set to blink only once
  BLINK,                                    // LED is blinking
  SPARKLE                                   // LED is in the process of sparkling
}pixel_state_t;


typedef struct PixelActionTime {
  int32_t actionOneTime;                    // time between each step for hue (hueFade) or red (rgbFade). Also, default param. for other timed actions
  int32_t actionTwoTime;                    // time between each step for sat (hueFade) or grn (rgbFade)
  int32_t actionThreeTime;                  // time between each step for val (hueFade) or blu (rgbFade)
}pixel_actionTime_t;


typedef struct PixelActionStartTime {
  uint32_t actionOneStart;                  // start time for action one
  uint32_t actionTwoStart;                  // start time for action two
  uint32_t actionThreeStart;                // start time for action three
}pixel_actionStart_t;

//**********    GLOBAL TYPES DECLARATION   ************//



//**********    GLOBAL FUNC DECLARATION   *************//


void pixelActionsHandler(void);


//**********    GLOBAL FUNC DECLARATION   *************//


#endif /*LEDS_ACTIONS_H_*/