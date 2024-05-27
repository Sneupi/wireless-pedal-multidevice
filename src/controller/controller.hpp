#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

#include <Arduino.h>
#include "fsm.hpp"

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

static int button_pins[] = {CTRL_BUTTON1, CTRL_BUTTON2, CTRL_BUTTON3};

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void TaskWireless(void *pvParameters) {
  for (;;) {
    xSemaphoreTake(controlMutex, portMAX_DELAY);
    analogInput = analogRead(35); //FIXME: testing, remove
    xSemaphoreGive(controlMutex);
    delay(10); // allow other tasks to run
  }
}

//-----------------------------------------------------------------------------
// SETUP
//-----------------------------------------------------------------------------

void setup() {

  // Init Serial Monitor
  Serial.begin(115200);
  while (!Serial) delay(10);

  // init potentiometer.... FIXME testing, remove
  pinMode(35, INPUT);
  analogReadResolution(12);

  // Init FSM task
  xTaskCreatePinnedToCore(
    TaskFSM               // The function containing the task
    ,  "FSM"               // A name just for humans
    ,  10000               // The stack size
    ,  (void *)button_pins // Pass reference to a variable describing the task number
    ,  1                   // priority
    ,  NULL                // Task handle is not used here - simply pass NULL
    , 0                    // Run on core 0
    );

  // Init Wireless task
  xTaskCreatePinnedToCore(
    TaskWireless           // The function containing the task
    ,  "Wireless"          // A name just for humans
    ,  2048                // The stack size
    ,  NULL                // Pass reference to a variable describing the task number
    ,  1                   // priority
    ,  NULL                // Task handle is not used here - simply pass NULL
    , 1                    // Run on core 1
    );

    #ifdef DEBUG
        Serial.println("Setup complete");
    #endif
}

//-----------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------

void loop() {
  // All tasks are handled by RTOS
}


#endif // CONTROLLER_HPP