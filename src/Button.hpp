#ifndef BUTTON_HPP
#define BUTTON_HPP

//-----------------------------------------------------------------------------
// INLCUDES
//-----------------------------------------------------------------------------

#include <Arduino.h>

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

#define DEBOUNCE 350 // buttons have lots of jitter, so we'll ignore changes for ms
#define PRESSED HIGH

//-----------------------------------------------------------------------------
// BUTTON CLASS
//-----------------------------------------------------------------------------
class Button {

private:

  uint8_t buttonState = LOW;
  uint8_t previousButtonState = LOW;
  uint8_t pin;
  long lastPress;

public:

  Button(uint8_t pin) : pin(pin) {
    pinMode(pin, INPUT);
    lastPress = millis();
  }

  bool read() {
    bool ret = false;
    
    buttonState = digitalRead(pin);
    
    if (buttonState == HIGH && previousButtonState == LOW && (millis() - lastPress > DEBOUNCE)) {
      lastPress = millis();
      ret = true;
    }

    previousButtonState = buttonState;
    return ret;
  }

};

#endif // BUTTON_HPP