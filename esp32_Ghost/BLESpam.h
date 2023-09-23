#pragma once
#include <Arduino.h>
#include <ArduinoBLE.h>
#include <BLEAdvertisingData.h>

class BLESpam {
  public:
  void AppleBLESpamInit() {
    Serial.println("Begin Advertising");

    if (!BLE.begin()) {
      Serial.println("Failed to start BLE!");
      return;
    }

    BLE.setLocalName("ESP32");

    while (true) 
    {
      if (Serial.available())
      {
          String Buffer = Serial.readString();

          // Convert the String to uint8_t array
          uint8_t rawData[MAX_AD_DATA_LENGTH];
          memcpy(rawData, Buffer.c_str(), min((int)Buffer.length(), MAX_AD_DATA_LENGTH));
          
          // Create a BLEAdvertisingData object
          BLEAdvertisingData adData;
          
          // Set raw data
          adData.setRawData(rawData, min((int)Buffer.length(), MAX_AD_DATA_LENGTH));

          // TODO: Find the correct way to stop advertising using ArduinoBLE API
          // BLE.advertise(false);
          
          // Set the advertising data and restart advertising
          BLE.setAdvertisingData(adData);
          BLE.advertise();

          Serial.println("Advertising started...");
          delay(300);
      }
    }
  }
};