#ifndef WROOM_CONFIG_H
#define WROOM_CONFIG_H

#include "BaseBoardConfig.h"

struct WroomConfig : public BaseBoardConfig {
    WroomConfig() {
        ledPin_B = 12; 
        ledPin_G = 13;
        ledPin_R = 14; 
        bluetoothTxPin = 21; 
        bluetoothRxPin = 22; 
        neopixelPin = 23;
        sdpin = 10;
        SupportsBluetooth = true; 
        SupportsNeoPixel = true; 
    }

    void init() override {
        BaseBoardConfig::init();
        Serial.begin(115200);
        Serial.println("ESP32 WROOM Initialization finished.");
    }

    void blinkLed() override {
        BaseBoardConfig::blinkLed();
    }
};

#endif // WROOM_CONFIG_H
