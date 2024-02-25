#ifndef S2_CONFIG_H
#define S2_CONFIG_H

#include "BaseBoardConfig.h"

struct S2Config : public BaseBoardConfig {
    S2Config() {
        ledPin_B = 4; 
        ledPin_G = 5;
        ledPin_R = 6; 
        bluetoothTxPin = 17; 
        bluetoothRxPin = 16; 
        neopixelPin = 18;
        sdpin = 10;
        SupportsBluetooth = false; 
        SupportsNeoPixel = false; 
    }

    void init() override {
        BaseBoardConfig::init();
        Serial.begin(115200);
        Serial.println("ESP32 S2 Initialization finished.");
    }

    void blinkLed() override {
        BaseBoardConfig::blinkLed();
       
    }
};

#endif // S2_CONFIG_H
