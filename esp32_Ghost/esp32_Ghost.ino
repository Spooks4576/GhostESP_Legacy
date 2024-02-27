#include <Arduino.h>
#include <WiFi.h>
#include "Public/Configs/S2Config.h"
#include "Public/Configs/S3Config.h"
#include "Public/Configs/WroomConfig.h"
#include "Private/Features/Dial.cpp"

// Have to Include .cpp Files due to Arduino Not Liking Organization

#include "Private/Controllers/YoutubeController.cpp"
#include "Private/Services/YoutubeService.cpp"
#include "Private/Controllers/RokuController.cpp"

S2Config boardConfig; // Change this Based on board your compiling for

void setup() {
    boardConfig.init();
    boardConfig.blinkLed();
    boardConfig.TurnoffLed();
}

void loop() {
    boardConfig.Loop();
}


