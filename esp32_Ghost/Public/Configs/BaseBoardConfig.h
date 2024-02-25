#ifndef BASE_BOARD_CONFIG_H
#define BASE_BOARD_CONFIG_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

struct BaseBoardConfig {
    int ledPin_B = -1;
    int ledPin_G = -1;
    int ledPin_R = -1;
    int bluetoothTxPin = -1;
    int bluetoothRxPin = -1;
    int neopixelPin = -1;
    int sdpin = -1;
    bool SupportsBluetooth = false;
    bool SupportsNeoPixel = false;

    BaseBoardConfig() {}

    virtual void init() {
        if (ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            pinMode(ledPin_B, OUTPUT);
            pinMode(ledPin_G, OUTPUT);
            pinMode(ledPin_R, OUTPUT);
        }

        if (sdpin != -1)
        {
            pinMode(sdpin, OUTPUT);
        }

        if (SupportsBluetooth && bluetoothTxPin != -1 && bluetoothRxPin != -1) {
        }

        if (SupportsNeoPixel && neopixelPin != -1) {
        }
    }

    virtual void loop() {
        Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));
        Serial.println(F("Welcome to Ghost ESP Made by Spooky"));
    }

    virtual void blinkLed() {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, LOW);
            digitalWrite(ledPin_R, LOW);
            delay(700);
            digitalWrite(ledPin_B, LOW);
            digitalWrite(ledPin_G, LOW);
            digitalWrite(ledPin_R, HIGH);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, LOW);
            digitalWrite(ledPin_R, LOW);
            delay(700);
            digitalWrite(ledPin_B, LOW);
            digitalWrite(ledPin_G, LOW);
            digitalWrite(ledPin_R, LOW);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                // Run the same color setup pattern for neopixel
                strip.setPixelColor(0, strip.Color(255, 0, 0));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 255, 0));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 0, 255));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
            }
        }
    }

    virtual void TurnoffLed()
    {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            digitalWrite(ledPin_B, LOW);
            digitalWrite(ledPin_G, LOW);
            digitalWrite(ledPin_R, LOW);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                // Turn off the neopixel LED
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
            }
        }
    }

    virtual void setLedColor(int r, int g, int b) {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            analogWrite(ledPin_B, r);
            analogWrite(ledPin_G, g);
            analogWrite(ledPin_R, b);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                strip.setPixelColor(0, strip.Color(r, g, b));
                strip.show();
            }
        }
    }

    String readSerialBuffer(bool &isHtml, bool &isAp) {
        String buffer = "";
        buffer = Serial.readString();

        isHtml = buffer.startsWith("sethtml=");
        isAp = buffer.startsWith("setap=");

        return buffer;
    }

    virtual void Loop()
    {
        if (Serial.available() > 0) {
            String flipperMessage;
            bool StartsWithHTML;
            bool StartsWithAP;
            flipperMessage = readSerialBuffer(StartsWithHTML, StartsWithAP);

            if (flipperMessage.startsWith("reset") || flipperMessage.startsWith("stop"))
            {
                Serial.println("reset Tag Found Rebooting");
                esp_restart();
            }
            else {
                Serial.println("Regular Command Called");
            }
        }
    }
};

#endif