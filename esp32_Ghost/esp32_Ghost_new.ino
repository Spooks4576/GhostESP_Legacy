#include <Arduino.h>

#include "../Public/Configs/S2Config.h"
#include "../Public/Configs/S3Config.h"
#include "../Public/Configs/WroomConfig.h"

WroomConfig boardConfig; // Change this Based on board your compiling for

void setup() {
    boardConfig.init();
    boardConfig.blinkLed();
}

void loop() {
    
}


