#pragma once
#include <Arduino.h>
#include <SD.h>
#include <HardwareSerial.h>

namespace RabbitTests 
{

    inline void listFiles(File dir) {
      while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
          // no more files in the directory
          break;
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
          Serial.println("/");
          listFiles(entry);  // recursive call to list files in the directory
        } else {
          Serial.println();
        }
        entry.close();
      }
    }


    inline static void SDCardTest() {
      pinMode(12, OUTPUT);

      delay(10);

      if (!SD.begin(12)) {
        Serial.println(F("Failed to mount SD Card"));
      } else {
        Serial.println(F("SD Card Successfully Mounted"));

        listFiles(SD.open("/"));
      }
    }
}