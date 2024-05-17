#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

//TODO: make as RTOS

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

#include <Arduino.h>

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

// Button debouncing library
#include <Bounce2.h>

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

#define ANALOG_MAX 4096

#define CONTROLLER_BUTTON1 34
#define CONTROLLER_BUTTON2 39
#define CONTROLLER_BUTTON3 36

#define DEBOUNCE_INTERVAL 40

//-----------------------------------------------------------------------------
// DATA STRUCTURES
//-----------------------------------------------------------------------------

enum State {
  MAIN,
  EDIT_THRESH
};

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------

void draw_main();
void draw_edit_thresh();
void state_main();
void state_edit_thresh();

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL

Bounce2::Button button1 = Bounce2::Button();
Bounce2::Button button2 = Bounce2::Button();
Bounce2::Button button3 = Bounce2::Button();

long timeSince = 0;
bool en = true;
float factor = 0.0; // 0.0 - 1.0
uint16_t analogInput = 4095; // 0-ANALOG_MAX
uint16_t analogThreshMax = ANALOG_MAX;
uint16_t analogThreshMin = 0;
State state = MAIN;

#define MAX_SCREEN 2

int screen = 0;

//-----------------------------------------------------------------------------
// SETUP
//-----------------------------------------------------------------------------

void setup() {

  // Initialize the buttons

  button1.attach(CONTROLLER_BUTTON1, INPUT);
  button2.attach(CONTROLLER_BUTTON2, INPUT);
  button3.attach(CONTROLLER_BUTTON3, INPUT);
  button1.interval(DEBOUNCE_INTERVAL);
  button2.interval(DEBOUNCE_INTERVAL);
  button3.interval(DEBOUNCE_INTERVAL);
  button1.setPressedState(HIGH);
  button2.setPressedState(HIGH);
  button3.setPressedState(HIGH);

  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Init Display
  display.init();
  display.flipScreenVertically();

  // init potentiometer
  pinMode(35, INPUT); //FIXME testing, remove
  analogReadResolution(12); //FIXME testing, remove

}

//-----------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------

void loop() {

  // read analog input
  analogInput = analogRead(35); //FIXME: testing, remove

  // state machine

  switch (state) {

    case MAIN:
      state_main();
      break;

    case EDIT_THRESH:
      state_edit_thresh();
      break;
  }


}

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------


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

void draw_main() {
  
  display.clear();

  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

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
  display.drawProgressBar(0, 40, 120, 10, percent);

  display.display();
}

void draw_edit_thresh() {
  
  char tempStr[10];

  display.clear();
  display.setFont(ArialMT_Plain_10);

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
}

void state_main() {

  button1.update();
  button2.update();
  button3.update();

  if (button1.pressed()) {
    en = !en;
  }
  if (button2.pressed()) {
    factor = (factor > 1.0 || equal(factor, 1.0)) ? 0 : factor + 0.1;
  }
  if (button3.pressed()) {
    state = EDIT_THRESH;
  }
  
  draw_main();

}

void state_edit_thresh() {

  button1.update();
  button2.update();
  button3.update();

  if (button1.pressed()) {
    analogThreshMax = analogInput;
    //Serial.println("MAX updated to " + String(analogThreshMin)); //FIXME: remove
  }

  if (button2.pressed()) {
    analogThreshMin = analogInput;
    //Serial.println("MIN updated to " + String(analogThreshMin)); //FIXME: remove
  }

  if (button3.pressed()) {
    state = MAIN;
  }

  draw_edit_thresh();

}


#endif // CONTROLLER_HPP