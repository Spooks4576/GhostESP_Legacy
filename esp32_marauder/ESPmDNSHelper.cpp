#include "ESPmDNSHelper.h"
#include <string>

ESPmDNSHelper::ESPmDNSHelper(const char* inSsid, const char* inpaSsword) {
    mdns = new MDNSResponder();
    ssid = inSsid;
    password = inpaSsword;
    connectWiFi();
}

ESPmDNSHelper::~ESPmDNSHelper() {
    delete mdns;
}

bool ESPmDNSHelper::initialize(const char* hostName) {
    if (mdns->begin(hostName)) {
        Serial.println("MDNS responder started");
        return true;
    }
    return false;
}

void ESPmDNSHelper::queryServices(const char* serviceType, const char* proto, uint32_t timeout) {
    int n = mdns->queryService(serviceType, proto);
    if (n == 0) {
        Serial.println("no services found");
    } else {
        for (int i = 0; i < n; ++i) {
            String serviceName = mdns->hostname(i);
            IPAddress serviceIP = mdns->IP(i);
            uint16_t servicePort = mdns->port(i);

            Serial.println("Service Name: " + serviceName);
            Serial.println("Service IP: " + serviceIP.toString());
            Serial.println("Service Port: " + String(servicePort));
            Serial.println("----------------------------");
            if (i == 1)
            {
              if (initializeClient(serviceIP.toString().c_str(), servicePort))
              {
                SendAuth();
              }
            }
            
        }
    }
    delay(timeout);
}

void ESPmDNSHelper::connectWiFi() {
    Serial.println("[connectWiFi] Initiating WiFi connection.");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("[connectWiFi] Connecting to WiFi...");
    }
    Serial.println("[connectWiFi] Connected to WiFi.");
    secureClient.setInsecure();

    if (initialize("ESP32S2-Device"))
    {
      queryServices("_googlecast", "_tcp", 2000);
    }
}

void ESPmDNSHelper::SendAuth()
{
  
  String buffer;
  
  webSocket->beginMessage(2);
  //webSocket->write(, );
  webSocket->endMessage();


  CheckMessageLoop();
}

void ESPmDNSHelper::CheckMessageLoop()
{
  int result = webSocket->parseMessage();
  while (result == 0) {
    Serial.println("Waiting For Response.");
    delay(500);
  }

  HandleMessage(webSocket->readString());
}

void ESPmDNSHelper::HandleMessage(String message)
{
  Serial.println(message.c_str());
}

bool ESPmDNSHelper::initializeClient(const char* _host, uint16_t _port)
{
  host = _host;
  port = _port;
  
  secureClient.setInsecure();

  if (secureClient.connect(host.c_str(), port)) {
      Serial.println("Connected securely!");

      Serial.println("Initializing packet stream and dispatching responses...");

      webSocket = new WebSocketClient(secureClient, host.c_str());

      Serial.println("WebSocket Initlized");

      return true;
  } else {
      Serial.println("Failed to connect securely.");
      return false;
  }
}