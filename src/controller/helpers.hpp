#ifndef HELPERS_HPP
#define HELPERS_HPP

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------

#include <Arduino.h>
#include "ctrl_config.hpp"


//------------------------------------------------------------------------------
// FUNCTION
//------------------------------------------------------------------------------

// translates raw analog input 0-ANALOG_MAX to 0-ANALOG_MAX based on min and max thresholds
uint16_t threshed(uint16_t analog, uint16_t threshMax, uint16_t threshMin) {

  if (threshMax == threshMin) return 0; // avoid division by zero

  // clean up thresholds
  if (threshMax < threshMin) { // swap values
    uint16_t temp = threshMax;
    threshMax = threshMin;
    threshMin = temp;
  };
  if (threshMax > ANALOG_MAX) threshMax = ANALOG_MAX; 
  if (threshMin < 0) threshMin = 0; 

  // scale according to thresholds
  if (analog > threshMax) {
    return ANALOG_MAX;
  } else if (analog < threshMin) {
    return 0;
  } else {
    return (analog - threshMin) * ANALOG_MAX / (threshMax - threshMin);
  }
  
}

bool equal(float a, float b) {
  return abs(a - b) < 0.0001;
}


#endif // HELPERS_HPP