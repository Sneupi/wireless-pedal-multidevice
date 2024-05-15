#ifndef PEDAL_HPP
#define PEDAL_HPP

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

#define ANALOG_PIN 36

//-----------------------------------------------------------------------------
// FUNCTION DECLARATIONS
//-----------------------------------------------------------------------------

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

int int_value;
float float_value;
bool bool_value = true;
 
struct_message myData;
 
esp_now_peer_info_t peerInfo;

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
 
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, RECEIVER_MAC, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  //set analog read resolution to 12 bits (0-4096)
  analogReadResolution(12);
}
 
//-----------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------


void loop() {

  // Read analog value
  myData.value = analogRead(ANALOG_PIN);
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(RECEIVER_MAC, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
}

//-----------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

#endif // PEDAL_HPP
