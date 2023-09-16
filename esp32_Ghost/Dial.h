#pragma once
#include "AppController.h"
#include "NetflixController.h"
#include "YoutubeController.h"
#include <ArduinoHttpClient.h>
#include <set>
#include <vector>

class DIALClient {
public:
    DIALClient(String& YUrl, String& ssid, String& password, AppController* appHandler)
        : ssid(ssid), password(password), appHandler(appHandler), YTurl(YUrl) {}

    void connectWiFi();
    void Execute();
    String getDialApplicationUrl(const String& locationUrl);
    std::vector<Device> discoverDevices();
    void exploreNetwork();
    bool parseSSDPResponse(const String& response, Device& device);
    String extractApplicationURL(HttpClient &httpc);
    bool fetchScreenIdWithRetries(const String& applicationUrl, Device& device, YoutubeController* YTController);
private:
    String ssid, password, YTurl;
    AppController* appHandler;
    WiFiUDP multicastClient;
};