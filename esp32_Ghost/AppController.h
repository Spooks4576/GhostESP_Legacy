#pragma once
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include "IRemoteService.h"
#include <Arduino.h>

class AppController {
public:

  WiFiClient client;


  virtual ~AppController() = default;


  virtual void launchApp(const String& appUrl) = 0;


  virtual int checkAppStatus(const String& appUrl, Device& device) = 0;

  void extractIPAndPort(const String& appUrl, IPAddress& ip, uint16_t& port) {
    Serial.println("Entering extractIPAndPort");
    
    int portStartIndex = appUrl.lastIndexOf(':');
    if (portStartIndex != -1 && portStartIndex < appUrl.length() - 1) {
        String portStr = appUrl.substring(portStartIndex + 1);
        port = portStr.toInt();
        Serial.println("Parsed port: " + String(port));
    }

    String ipStr = appUrl.substring(7, portStartIndex);
    Serial.println("Parsed IP string: " + ipStr);
    if (!ip.fromString(ipStr)) {
        Serial.println("Failed to parse IP");
    }

    Serial.println("Exiting extractIPAndPort");
  }
};
