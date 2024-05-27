#ifndef RECEIVER_HPP
#define RECEIVER_HPP

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "../struct_message.h"
#include "../config.hpp"

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FUNCTION DECLARATIONS
//-----------------------------------------------------------------------------

// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

struct_message myData;

//-----------------------------------------------------------------------------
// SETUP
//-----------------------------------------------------------------------------

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}

//-----------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------

void loop() {}

//-----------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (memcmp(mac, CONTROLLER_MAC, 6) == 0) {
    memcpy(&myData, incomingData, sizeof(myData));
    // Serial.print("Data received: ");
    // Serial.println(len);
    // Serial.print("Value: ");
    Serial.println(myData.value);
    // Serial.print(myData.value / 4095.0 * 100.0);
    // Serial.println(" %");
  }
}

#endif // RECEIVER_HPP