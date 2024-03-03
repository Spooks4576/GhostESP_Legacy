#define DEBUG_ESP_HTTP_UPDATE

#include <Arduino.h>
#include <WiFi.h>
//#include "Public/Configs/S2Config.h"
// #include "Public/Configs/S3Config.h"
#include "Public/Configs/WroomConfig.h" // DO NOT Include multiple configs only include one at a time
#include "Private/Features/Dial.cpp"

// Have to Include .cpp Files due to Arduino Not Liking Organization

#include "Private/Controllers/YoutubeController.cpp"
#include "Private/Services/YoutubeService.cpp"
#include "Private/Controllers/RokuController.cpp"

WroomConfig boardConfig; // Change this Based on board your compiling for

void SerialCheckTask(void *pvParameters) {
    while (1) {
        if (Serial.available() > 0) {
            String flipperMessage;
            flipperMessage = Serial.readString(); 

            Serial.println(flipperMessage);

            if (flipperMessage.startsWith("reset") || flipperMessage.startsWith("stop")) {
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

    Serial.println("Ghost ESP 2.2");
}

void loop() {
    boardConfig.Loop();
}


