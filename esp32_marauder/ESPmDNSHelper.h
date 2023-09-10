#ifndef ESP_MDNS_HELPER_H
#define ESP_MDNS_HELPER_H

#include <Arduino.h>
#include <vector>
#if defined(ESP32)
    #include <WiFiClientSecure.h>
    #include <ESPmDNS.h>
    #include <ArduinoHttpClient.h>
    #include <ArduinoJson.h>
    #include <BearSSLClient.h>
    #include <WebSocketClient.h>
#else
    #error "This code is intended for ESP32 only!"
#endif


struct Devices
{
  String HostName;
  IPAddress serviceIP;
  uint16_t servicePort;
};

class ESPmDNSHelper {
public:
    ESPmDNSHelper(const char* inSsid, const char* inpaSsword);
    ~ESPmDNSHelper();

    WiFiClient unsecureclient;
    WiFiClientSecure client;
    String host;
    uint16_t port;
    const char* ssid;
    const char* password;

    void connectWiFi();

    bool initialize(const char* hostName);
    void queryServices(const char* serviceType, const char* proto, uint32_t timeout);

    bool initializeClient(const char* _host, uint16_t _port);

    void SendAuth();

    void sendMessageToChromecast(const char* hexData);

    void CheckMessageLoop();
private:
    MDNSResponder* mdns;
    std::vector<Devices> NetDevices;
};

#endif // ESP_MDNS_HELPER_H
