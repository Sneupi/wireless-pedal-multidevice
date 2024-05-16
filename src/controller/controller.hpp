#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

//TODO: make as RTOS, code buttons, and threshold adjuster mode

// Include the correct display library

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

int buttonPin = 34;
int buttonState = 0;
int previousButtonState = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  pinMode(buttonPin, INPUT_PULLUP);

}

long timeSince = 0;
bool en = true;
uint8_t percentScale = 0; // 0-100
uint16_t analogInput = 0; // 0-4095
uint16_t analogThreshMax = 4095;
uint16_t analogThreshMin = 0;

// 0 - 100
uint16_t scaled(uint16_t val) {
  uint16_t ret = 0;
  if (val > analogThreshMax) {
    ret = 100;
  } else if (val < analogThreshMin) {
    ret = 0;
  } else {
    ret = (val - analogThreshMin) * 100 / (analogThreshMax - analogThreshMin);
  }
  return ret;
}

void drawGUI() {
  
  display.clear();

  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  if (en) {
    display.drawString(0, 0, "ON");
  } else {
    display.drawString(0, 0, "OFF");
  }
  
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, String(percentScale) + "%");

  display.drawProgressBar(0, 40, 120, 10, (uint8_t)(scaled(analogInput)));

  display.display();
}

long lastPress = millis();
const long debounce = 800; // buttons have lots of jitter, so we'll ignore changes for 800 ms
  
void loop() {

  
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH && previousButtonState == LOW && (millis() - lastPress > debounce)) {
    en = !en;
    lastPress = millis();
    Serial.println("Button Pressed");
  }
  
  previousButtonState = buttonState;

  if (millis() - timeSince > 250) {
    percentScale = (percentScale >= 100) ? 0 : percentScale + 10;
    analogInput = (analogInput >= 4095) ? 0 : analogInput + 100;
    timeSince = millis();
  }
  drawGUI();
}

#endif // CONTROLLER_HPP