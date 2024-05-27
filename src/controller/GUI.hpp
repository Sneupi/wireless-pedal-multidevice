#ifndef GUI_HPP
#define GUI_HPP

//--------------------------------------------------------------------------------
// INCLUDES
//--------------------------------------------------------------------------------

#include <Arduino.h>

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

#include "ctrl_config.hpp"
#include "globals.hpp"
#include "helpers.hpp"


//--------------------------------------------------------------------------------
// PRIVATE VARIABLES
//--------------------------------------------------------------------------------

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL


//--------------------------------------------------------------------------------
// FUNCTIONS
//--------------------------------------------------------------------------------

void gui_init() {
  display.init();
  display.flipScreenVertically();
}

void draw_main() {

  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);  
  
  // Take control mutex to avoid task conflict
  if (xSemaphoreTake(controlMutex, portMAX_DELAY) == pdTRUE) {
    
    // draw en/disabled status
    if (en) {
        display.drawString(0, 0, "ON");
    } else {
        display.drawString(0, 0, "OFF");
    }
    
    // draw factor
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    char factorStr[10];
    sprintf(factorStr, "%d%%", (int)(factor * 100));
    display.drawString(128, 0, factorStr);

    // draw progress bar
    uint8_t percent = (uint8_t)((threshed(analogInput, analogThreshMax, analogThreshMin) * factor / (float)ANALOG_MAX) * 100.0);
    display.drawProgressBar(0, 40, 120, 10, (en) ? percent : 0);
   
    //display
    display.display();

    // Release control mutex
    xSemaphoreGive(controlMutex);
  }
}

void draw_edit_thresh() {
  
  char tempStr[10];

  display.clear();
  display.setFont(ArialMT_Plain_10);

  // Take control mutex to avoid task conflict
  if (xSemaphoreTake(controlMutex, portMAX_DELAY) == pdTRUE) {

    // draw threshold max
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    sprintf(tempStr, "MAX: %d", analogThreshMax);
    display.drawString(0, 0, tempStr);

    // draw threshold min
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    sprintf(tempStr, "MIN: %d", analogThreshMin);
    display.drawString(128, 0, tempStr);

    // draw progress bar (raw analog input)
    uint8_t percent = (uint8_t)((analogInput * 100) / ANALOG_MAX);
    display.drawProgressBar(0, 40, 120, 10, percent);

    display.display();

    // Release control mutex
    xSemaphoreGive(controlMutex);
  }
}


#endif // GUI_HPP