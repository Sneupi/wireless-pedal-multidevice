#ifndef PEDAL_HPP
#define PEDAL_HPP

#include <Arduino.h>

//-----------------------------------------------------------------------------
// FUNCTION DECLARATIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SETUP
//-----------------------------------------------------------------------------

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  
  //set the resolution to 12 bits (0-4096)
  analogReadResolution(12);
}

//-----------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------

void loop() {
  // read the analog / millivolts value for pin 2:
  int analogValue = analogRead(2);
  int analogVolts = analogReadMilliVolts(2);
  
  // print out the values you read:
  Serial.printf("ADC = %d\n",analogValue);
  //Serial.printf("ADC millivolts value = %d\n",analogVolts);
  
  delay(100);  // delay in between reads for clear read from serial
}


//-----------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------


#endif // PEDAL_HPP