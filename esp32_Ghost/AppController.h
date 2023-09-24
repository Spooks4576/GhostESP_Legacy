#pragma once
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include "IRemoteService.h"
#include <Arduino.h>

inline String UserAgent = F("Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Mobile Safari/537.36");

class AppController {
public:

  WiFiClient client;


  virtual ~AppController() = default;


  virtual void launchApp(const String& appUrl) = 0;


  virtual int checkAppStatus(const String& appUrl, Device& device) = 0;

  void extractIPAndPort(const String& appUrl, IPAddress& ip, uint16_t& port) {
    Serial.println(F("Entering extractIPAndPort"));
    
    int portStartIndex = appUrl.lastIndexOf(':');
    if (portStartIndex != -1 && portStartIndex < appUrl.length() - 1) {
        String portStr = appUrl.substring(portStartIndex + 1);
        port = portStr.toInt();
        Serial.println("Parsed port: " + String(port));
    }

    String ipStr = appUrl.substring(7, portStartIndex);
    Serial.println("Parsed IP string: " + ipStr);
    if (!ip.fromString(ipStr)) {
        Serial.println(F("Failed to parse IP"));
    }

    Serial.println(F("Exiting extractIPAndPort"));
  }
};
