#define DEBUG_ESP_HTTP_UPDATE

#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "Public/Configs/S2Config.h"
//#include "Public/Configs/S3Config.h"
//#include "Public/Configs/S3MiniConfig.h"
//#include "Public/Configs/WroomConfig.h" // DO NOT Include multiple configs only include one at a time
#include "Private/Features/Dial.cpp"

// Have to Include .cpp Files due to Arduino Not Liking Organization

#include "Private/Controllers/YoutubeController.cpp"
#include "Private/Services/YoutubeService.cpp"
#include "Private/Controllers/RokuController.cpp"
#include "Private/Controllers/NetflixController.cpp"

S2Config boardConfig; // Change this Based on board your compiling for

void SerialCheckTask(void *pvParameters) {
    while (1) {
        if (Serial.available() > 0) {
            String flipperMessage;
            flipperMessage = Serial.readString(); 

            Serial.println(flipperMessage);

            if (flipperMessage.indexOf("reset") != -1 || flipperMessage.indexOf("stop") != -1) {
                Serial.println("Reset tag found. Rebooting...");
                esp_restart(); // Restart the ESP32
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void setup() {
  boardConfig.init();
  boardConfig.blinkLed();
  boardConfig.TurnoffLed();

  xTaskCreate(SerialCheckTask, "SerialCheckTask", 2048, NULL, 1, NULL);

  Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));
  Serial.println(F("Welcome to Ghost ESP Made by Spooky"));
  Serial.println("GHOST GHOST GHOST GHOST GHOST GHOST");

  
}

void loop() {
    boardConfig.Loop();
}


