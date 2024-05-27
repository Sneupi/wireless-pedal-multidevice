#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <Arduino.h>

#include "ctrl_config.hpp"


// Sync related variables ------------------------------------------------------

extern SemaphoreHandle_t controlMutex = xSemaphoreCreateMutex();

// Control related variables --------------------------------------------------

extern bool en = true;
extern float factor = 0.0; // 0.0 - 1.0
extern uint16_t analogThreshMax = ANALOG_MAX;
extern uint16_t analogThreshMin = 0;

// Input related variables ----------------------------------------------------

extern uint16_t analogInput = 0; // 0-ANALOG_MAX



#endif // GLOBALS_HPP