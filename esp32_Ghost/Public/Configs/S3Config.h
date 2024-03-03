#ifndef S3_CONFIG_H
#define S3_CONFIG_H
#define USE_BLUETOOTH

#include "BaseBoardConfig.h"

struct S3Config : public BaseBoardConfig {
    S3Config() {
        ledPin_B = 32; 
        ledPin_G = 33;
        ledPin_R = 25; 
        bluetoothTxPin = 26; 
        bluetoothRxPin = 27; 
        neopixelPin = 14;
        sdpin = 4;
        SupportsBluetooth = true; 
        SupportsNeoPixel = true; 
        UpdateURL = "http://cdn.spookytools.com/assets/ghostesps3.bin";
    }

    void init() override {
        BaseBoardConfig::init();
        Serial.begin(115200);
        Serial.println("ESP32 S3 Initialization finished.");
    }

    void blinkLed() override {
        BaseBoardConfig::blinkLed();
    }
};

#endif // S3_CONFIG_H
