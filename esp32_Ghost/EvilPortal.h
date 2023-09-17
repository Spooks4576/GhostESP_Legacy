#include <WiFi.h>
#include <DNSServer.h>
#include "ESPAsyncWebSrv.h"
#include <AsyncTCP.h>

AsyncWebServer server(80);
DNSServer dnsServer;

class EvilPortal {
private:
  const char *ssid;
  const char *password;

public:
  EvilPortal(const char* SSID = "Free Wifi"){ssid = SSID;};

  void Handler(AsyncWebServerRequest* Request)
  {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->print("</body></html>");
    request->send(response);
  }

  void begin() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    WiFi.softAPConfig(WiFi.softAPIP(), WiFi.softAPIP(), IPAddress(255, 255, 255, 0));
    dnsServer.start(53, "*", WiFi.softAPIP());
    AsyncCallbackWebHandler* HandlerRoot = server->on("/", std::bind(&EvilPortal::Handler, this));
    AsyncCallbackWebHandler* HandlerWifi = server->on("/wifi", std::bind(&EvilPortal::Handler, this, true));
    AsyncCallbackWebHandler* Handler0Wifi = server->on("/0wifi", std::bind(&EvilPortal::Handler, this, false));
    AsyncCallbackWebHandler* Handler0WifiSave = server->on("/wifisave", std::bind(&EvilPortal::Handler, this));
    AsyncCallbackWebHandler* HandlerAndroid = server->on("/generate_204", std::bind(&EvilPortal::Handler, this));
    AsyncCallbackWebHandler* HandlerMicrosoft = server->on("/fwlink", std::bind(&EvilPortal::Handler, this)); 
    AsyncCallbackWebHandler* HandlerNotFound = server->onNotFound(std::bind(&EvilPortal::Handler, this));
    server.addHandler(HandlerRoot);
    server.addHandler(HandlerWifi);
    server.addHandler(Handler0Wifi);
    server.addHandler(Handler0WifiSave);
    server.addHandler(HandlerAndroid);
    server.addHandler(HandlerMicrosoft);
    server.addHandler(HandlerNotFound);
    server.begin();
    Serial.printf("Evil Portal Started at %s", WiFi.softAPIP().toString().c_str());
  }

  void loop() {
    dnsServer.processNextRequest();
    yield();
  }
};