#ifndef BASE_BOARD_CONFIG_H
#define BASE_BOARD_CONFIG_H

#include <Arduino.h>
#include <HTTPUpdate.h>
#include <Adafruit_NeoPixel.h>
#include <wifi.h>
#include "../../Public/Controllers/YoutubeController.h"
#include "../../Public/Features/Dial.h"

#define DELIMITER ","

#ifdef USE_BLUETOOTH
#include <NimBLEDevice.h>

// Define the struct for the buds models
static const struct {
    uint32_t value;
    const char* name;
} buds_models[] = {
    {0xEE7A0C, "Fallback Buds"},
    {0x9D1700, "Fallback Dots"},
    {0x39EA48, "Light Purple Buds2"},
    {0xA7C62C, "Bluish Silver Buds2"},
    {0x850116, "Black Buds Live"},
    {0x3D8F41, "Gray & Black Buds2"},
    {0x3B6D02, "Bluish Chrome Buds2"},
    {0xAE063C, "Gray Beige Buds2"},
    {0xB8B905, "Pure White Buds"},
    {0xEAAA17, "Pure White Buds2"},
    {0xD30704, "Black Buds"},
    {0x9DB006, "French Flag Buds"},
    {0x101F1A, "Dark Purple Buds Live"},
    {0x859608, "Dark Blue Buds"},
    {0x8E4503, "Pink Buds"},
    {0x2C6740, "White & Black Buds2"},
    {0x3F6718, "Bronze Buds Live"},
    {0x42C519, "Red Buds Live"},
    {0xAE073A, "Black & White Buds2"},
    {0x011716, "Sleek Black Buds2"},
};

#define NUM_MODELS (sizeof(buds_models) / sizeof(buds_models[0]))

void fill_model_bytes(uint8_t *array) {
    int randomIndex = rand() % NUM_MODELS;
    uint32_t value = buds_models[randomIndex].value;
    array[17] = (value >> 24) & 0xFF; // 17th byte
    array[18] = (value >> 16) & 0xFF;  // 18th byte
    array[20] = (value >> 8) & 0xFF; // 20th byte, note the change in order due to byte significance
}

struct SamsungTestBLEData {
  NimBLEAdvertisementData advertisementData;
  NimBLEAdvertisementData scanResponse;
};

SamsungTestBLEData GetSamsungTestBLE() {
  NimBLEAdvertisementData advertisementData = NimBLEAdvertisementData();
  NimBLEAdvertisementData scanResponse = NimBLEAdvertisementData();

  // Split your specific packet data into advertisement and scan response data
  uint8_t advertisementPacket[] = {
    0x02, 0x01, 0x18, 0x1B, 0xFF, 0x75, 0x00, 0x42, 0x09, 0x81, 0x02, 0x14,
    0x15, 0x03, 0x21, 0x01, 0x09, 0xEF, 0x0C, 0x01, 0x47, 0x06, 0x3C, 0x94, 0x8E,
    0x00, 0x00, 0x00, 0x00, 0xC7, 0x00
  };
  uint8_t scanResponsePacket[] = {
    0x10, 0xFF, 0x75, 0x00, 0x00, 0x63, 0x50, 0x8D, 0xB1, 0x17, 0x40, 0x46,
    0x64, 0x64, 0x00, 0x01, 0x04
  };

  fill_model_bytes(advertisementPacket);

  // Add the packet data to the advertisement data and scan response
  advertisementData.addData(std::string((char *)advertisementPacket, sizeof(advertisementPacket)));
  scanResponse.addData(std::string((char *)scanResponsePacket, sizeof(scanResponsePacket)));

  // Return both advertisement data and scan response
  return { advertisementData, scanResponse };
}

#endif

void retainLineExcludingKeywords(String &content, const String &keyword1, const String &keyword2) {
    int index1 = content.indexOf(keyword1);
    int index2 = content.indexOf(keyword2);
    
    // If neither keyword is found, return the content as-is
    if (index1 == -1 && index2 == -1) {
        return;
    }

    // Split the content into lines
    int startPos = 0;
    int endPos = content.indexOf('\n');
    while (endPos != -1) {
        String line = content.substring(startPos, endPos);
        if (line.indexOf(keyword1) == -1 && line.indexOf(keyword2) == -1 && line.length() > 0) { // Check for non-empty line
            content = line;
            return;
        }
        startPos = endPos + 1;
        endPos = content.indexOf('\n', startPos);
    }

    // Check the last line (if there's no newline at the end)
    if (startPos < content.length()) {
        String line = content.substring(startPos);
        if (line.indexOf(keyword1) == -1 && line.indexOf(keyword2) == -1 && line.length() > 0) { // Check for non-empty line
            content = line;
            return;
        }
    }

    content = "";  // If no suitable line is found, clear the content
}

struct BaseBoardConfig {
    int ledPin_B = -1;
    int ledPin_G = -1;
    int ledPin_R = -1;
    int bluetoothTxPin = -1;
    int bluetoothRxPin = -1;
    int neopixelPin = -1;
    int pixel_count = -1;
    int sdpin = -1;
    bool SupportsBluetooth = false;
    bool SupportsNeoPixel = false;
#ifdef USE_BLUETOOTH
    NimBLEAdvertising* pAdvertising;
#endif
    String UpdateURL = "";
    Adafruit_NeoPixel strip;

    BaseBoardConfig() {

        if (SupportsNeoPixel && neopixelPin != -1 && pixel_count != -1)
        {
            Adafruit_NeoPixel instrip(pixel_count, neopixelPin, NEO_GRB + NEO_KHZ800);
            strip = instrip;
        }
    }

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
            // TODO Add Bluetooth Initilization
        }

        
        Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));
        Serial.println(F("Welcome to Ghost ESP Made by Spooky"));
    }

    virtual void blinkLed() {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, LOW);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, LOW);
            digitalWrite(ledPin_R, HIGH);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, LOW);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, HIGH);
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
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, HIGH);
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
        buffer = Serial.readStringUntil('\r\n');

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

            Serial.println(flipperMessage.c_str());

            if (flipperMessage.startsWith("settings"))
            {
                retainLineExcludingKeywords(flipperMessage, "settings -s EnableLED enable", "settings -s SavePCAP enable");
            } // Hack for Marauder app. until i can produce my own flipper app

            Serial.println(flipperMessage.c_str());

            if (flipperMessage.startsWith("YTDialConnect")) {
                flipperMessage.remove(0, 14); // Remove "YTDialConnect" from the message
    
                Serial.println("Debug: " + flipperMessage); // Debugging line to check the message

                int firstSpace = flipperMessage.indexOf(DELIMITER);
                int secondSpace = flipperMessage.indexOf(DELIMITER, firstSpace + 1);

                // Debugging lines to check the positions of spaces
                Serial.println("First space at: " + String(firstSpace));
                Serial.println("Second space at: " + String(secondSpace));

                if (firstSpace != -1 && secondSpace != -1) {
                    String YTURL = flipperMessage.substring(0, firstSpace);
                    String SSID = flipperMessage.substring(firstSpace + 1, secondSpace);
                    String Password = flipperMessage.substring(secondSpace + 1);

                    // Debugging lines to check the parsed parts
                    Serial.println("YTURL: " + YTURL);
                    Serial.println("SSID: " + SSID);
                    Serial.println("Password: " + Password);

                    YTURL.trim();
                    SSID.trim();
                    Password.trim();


                    YoutubeController* YtController = new YoutubeController();
                    DIALClient* dial = new DIALClient(YTURL.c_str(), SSID.c_str(), Password.c_str(), YtController);
                    setLedColor(0, 1, 0);


                    dial->Execute();
                    delete dial; // Clean up after execution
                    delete YtController;

                    TurnoffLed();
                } else {
                    // Handle error: message format incorrect
                    Serial.println("Error: Incorrect message format for YT Dial connection.");
                }
            }
            
            if (flipperMessage.startsWith("YTChromeConnect"))
            {
                flipperMessage.remove(0, 16); // Remove "YTChromeConnect" from the message + Space
    
                Serial.println("Debug: " + flipperMessage); // Debugging line to check the message

                int firstSpace = flipperMessage.indexOf(DELIMITER);
                int secondSpace = flipperMessage.indexOf(DELIMITER, firstSpace + 1);

                // Debugging lines to check the positions of spaces
                Serial.println("First space at: " + String(firstSpace));
                Serial.println("Second space at: " + String(secondSpace));

                if (firstSpace != -1 && secondSpace != -1) {
                    String YTURL = flipperMessage.substring(0, firstSpace);
                    String SSID = flipperMessage.substring(firstSpace + 1, secondSpace);
                    String Password = flipperMessage.substring(secondSpace + 1);

                    // Debugging lines to check the parsed parts
                    Serial.println("YTURL: " + YTURL);
                    Serial.println("SSID: " + SSID);
                    Serial.println("Password: " + Password);

                    YTURL.trim();
                    SSID.trim();
                    Password.trim();
                }
            }

            if (flipperMessage.startsWith("Update"))
            {
                flipperMessage.remove(0, 7);
    
                Serial.println("Debug: " + flipperMessage); // Debugging line to check the message

                int firstSpace = flipperMessage.indexOf(DELIMITER);
                int secondSpace = flipperMessage.indexOf(DELIMITER, firstSpace + 1);

                // Debugging lines to check the positions of spaces
                Serial.println("First space at: " + String(firstSpace));
                Serial.println("Second space at: " + String(secondSpace));

                if (firstSpace != -1 && secondSpace != -1) {
                    String SSID = flipperMessage.substring(0, firstSpace);
                    String Password = flipperMessage.substring(firstSpace + 1, secondSpace);

                    Serial.println("SSID: " + SSID);
                    Serial.println("Password: " + Password);

                    SSID.trim();
                    Password.trim();

                    WiFi.begin(SSID, Password);
                    while (WiFi.status() != WL_CONNECTED) {
                        delay(100);
                        Serial.print(".");
                    }
                    Serial.println("\nConnected to Wi-Fi");
                    Serial.println("\nSet Callback");

                    WiFiClient client;

                    t_httpUpdate_return ret = httpUpdate.update(client, UpdateURL, "1.0.0", 0);

                    switch(ret) {
                        case HTTP_UPDATE_FAILED:
                            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
                            break;

                        case HTTP_UPDATE_NO_UPDATES:
                            Serial.println("HTTP_UPDATE_NO_UPDATES");
                            break;

                        case HTTP_UPDATE_OK:
                            Serial.println("HTTP_UPDATE_OK");
                            break;
                    }
                }
            }

#ifdef USE_BLUETOOTH

            if (flipperMessage.startsWith("BLE_SamsungWatch") && SupportsBluetooth)
            {
                flipperMessage.remove(0, 16);

                srand(time(NULL));

                while (true) {
                    NimBLEDevice::init("");
                    NimBLEServer* pServer = NimBLEDevice::createServer();
                    pAdvertising = pServer->getAdvertising();


                    SamsungTestBLEData advertisementData = GetSamsungTestBLE();


                    pAdvertising->setAdvertisementData(advertisementData.advertisementData);
                    pAdvertising->setScanResponseData(advertisementData.scanResponse);


                    pAdvertising->start();
                    Serial.println("Sending Packet");
                    delay(100);
                    NimBLEDevice::deinit();

                    uint8_t mac[6];
                    for (int i = 0; i < 6; i++) {
                    mac[i] = random(0, 256);
                    }
                    mac[0] = (mac[0] & 0xFC) | 0x02;

                    esp_base_mac_addr_set(mac);
                }
            }
#endif
        }
    }
};

#endif