#ifndef WROOM_CONFIG_H
#define WROOM_CONFIG_H
#define USE_BLUETOOTH

#include "BaseBoardConfig.h"

struct WroomConfig : public BaseBoardConfig {
    WroomConfig() {
        ledPin_B = -1; 
        ledPin_G = -1;
        ledPin_R = -1; 
        bluetoothTxPin = 21; 
        bluetoothRxPin = 22; 
        neopixelPin = -1;
        sdpin = 10;
        SupportsBluetooth = true; 
        SupportsNeoPixel = false; 
        UpdateURL = "http://cdn.spookytools.com/assets/wroom.bin";
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
