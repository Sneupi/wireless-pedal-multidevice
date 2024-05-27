#ifndef WIRELESS_HPP
#define WIRELESS_HPP

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "../struct_message.h"
#include "../config.hpp"
#include "globals.hpp"
#include "helpers.hpp"

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

struct_message myData;

esp_now_peer_info_t peerInfo;

//-----------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  // Check if the received MAC is PEDAL_MAC
  if (memcmp(mac, PEDAL_MAC, 6) == 0) {
    memcpy(&myData, incomingData, sizeof(myData));

    // Take the controlMutex
    if (xSemaphoreTake(controlMutex, portMAX_DELAY) == pdTRUE) {
        
        // update the analog input value
        analogInput = (uint16_t)myData.value;

        // if sending enabled, translate & send to RECEIVER_MAC. Else, send 0.
        myData.value = (!en) ? 0 : (uint16_t)(threshed(analogInput, analogThreshMax, analogThreshMin) * factor);
        
        // Send to receiver
        esp_err_t result = esp_now_send(RECEIVER_MAC, (uint8_t *) &myData, sizeof(myData));
        if (result != ESP_OK) {
            #ifdef DEBUG
                Serial.println("OnDataRecv(): Error sending the data");
            #endif
        }
        else {
            #ifdef DEBUG
                Serial.println("OnDataRecv(): Sent OK");
            #endif
        }
        

        // release the mutex
        xSemaphoreGive(controlMutex);
    }
    else {
        #ifdef DEBUG
            Serial.println("OnDataRecv(): Error taking controlMutex");
        #endif
    }
  }
}


void wireless_init() {
  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer (RECEIVER_MAC)
  memcpy(peerInfo.peer_addr, RECEIVER_MAC, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer (RECIEVER_MAC)
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}


#endif // WIRELESS_HPP