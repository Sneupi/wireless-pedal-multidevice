#ifndef FSM_HPP
#define FSM_HPP

// Includes -------------------------------------------------------------------

#include <Arduino.h>

// Button debouncing library
#include <Bounce2.h>

#include "GUI.hpp"


// Defines --------------------------------------------------------------------

#define DEBOUNCE_INTERVAL 40


// Enumerations ---------------------------------------------------------------

enum State {
    MAIN,
    EDIT_THRESH
};


// Private Variables ----------------------------------------------------------

Bounce2::Button button1 = Bounce2::Button();
Bounce2::Button button2 = Bounce2::Button();
Bounce2::Button button3 = Bounce2::Button();

State state;


// Private Functions ----------------------------------------------------------


void state_main() {

    // Check if updating control values
    if (button1.pressed() || button2.pressed()) {

        // Take control mutex
        if (xSemaphoreTake(controlMutex, portMAX_DELAY) == pdTRUE) {

            // Update control values
            if (button1.pressed()) {
                en = !en;
                #ifdef DEBUG
                    Serial.println("en <- " + String(en));
                #endif
            }
            if (button2.pressed()) {
                factor = (factor > 1.0 || equal(factor, 1.0)) ? 0 : factor + 0.1;
                #ifdef DEBUG
                    Serial.println("factor <- " + String(factor));
                #endif
            }

            // Release control mutex
            xSemaphoreGive(controlMutex);

        }
        else {
            #ifdef DEBUG
                Serial.println("state_main(): Failed to take controlMutex");
            #endif
        }
    }

    // Check if change GUI state
    if (button3.pressed()) {
        state = EDIT_THRESH;
        #ifdef DEBUG
            Serial.println("state <- EDIT_THRESH");
        #endif
    }

}

void state_edit_thresh() {
    
    // Check if updating threshold values
    if (button1.pressed() || button2.pressed()) {

        // Take control mutex
        if (xSemaphoreTake(controlMutex, portMAX_DELAY) == pdTRUE) {

            // Update threshold values
            if (button1.pressed()) {
                analogThreshMax = analogInput;
                #ifdef DEBUG 
                    Serial.println("analogThreshMax <- " + String(analogThreshMax));
                #endif
            }
            if (button2.pressed()) {
                analogThreshMin = analogInput;
                #ifdef DEBUG
                    Serial.println("analogThreshMin <- " + String(analogThreshMin));
                #endif
            }

            // Release control mutex
            xSemaphoreGive(controlMutex);
        }
        else {
            #ifdef DEBUG
                Serial.println("state_edit_thresh(): Failed to take controlMutex");
            #endif
        }
    } 

    // Check if change GUI state
    if (button3.pressed()) {
        state = MAIN;
        #ifdef DEBUG
            Serial.println("state <- MAIN");
        #endif
    }

}

void fsm_init(int button1_pin, int button2_pin, int button3_pin) {

    // Init state
    state = MAIN;

    // Init GUI
    gui_init();

    // Init buttons
    button1.attach(button1_pin, INPUT);
    button2.attach(button2_pin, INPUT);
    button3.attach(button3_pin, INPUT);
    button1.interval(DEBOUNCE_INTERVAL);
    button2.interval(DEBOUNCE_INTERVAL);
    button3.interval(DEBOUNCE_INTERVAL);
    button1.setPressedState(HIGH);
    button2.setPressedState(HIGH);
    button3.setPressedState(HIGH);

}


// Public Functions -----------------------------------------------------------


// RTOS task for 3-button FSM (Finite State Machine) for wireless controller
// pvParameters: int array to button pins
void TaskFSM(void *pvParameters) {
    
    // Cast pvParameters to an int pointer
    int *button_pins = (int *)pvParameters;
    
    // Init FSM using button pins
    fsm_init(button_pins[0], button_pins[1], button_pins[2]);

    // Forever
    for(;;) {
        
        // Refresh buttons
        button1.update();
        button2.update();
        button3.update();

        // State machine
        switch (state) {
            case MAIN:
                state_main();
                draw_main();
                break;
            case EDIT_THRESH:
                state_edit_thresh();
                draw_edit_thresh();
                break;
        }

        delay(10); // allow other tasks to run
    }
}


#endif // FSM_HPP