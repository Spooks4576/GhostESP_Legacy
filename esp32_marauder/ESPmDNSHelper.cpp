#include "ESPmDNSHelper.h"
#include "CastChannel.h"

ESPmDNSHelper::ESPmDNSHelper(const char* inSsid, const char* inpaSsword) {
    mdns = new MDNSResponder();
    ssid = inSsid;
    password = inpaSsword;
    connectWiFi();
}

void HandleMessage(String message)
{
  Serial.println(message.c_str());
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

    if (initialize("ESP32S2-Device"))
    {
      queryServices("_googlecast", "_tcp", 2000);
    }
}

void ESPmDNSHelper::SendAuth()
{
  Channel ConnectChannel(client, "sender-0", "receiver-0", "urn:x-cast:com.google.cast.tp.connection", "JSON");
  Channel HeartBeat(client, "sender-0", "receiver-0", "urn:x-cast:com.google.cast.tp.heartbeat", "JSON");
  Channel RecieverChannel(client, "sender-0", "receiver-0", "urn:x-cast:com.google.cast.receiver", "JSON");

  RecieverChannel.setMessageCallback(HandleMessage);
  HeartBeat.setMessageCallback(HandleMessage);
  ConnectChannel.setMessageCallback(HandleMessage);


  StaticJsonDocument<200> Heartbeatdoc;

  Heartbeatdoc["type"] = "PING";

  String HeartBeatString;
  serializeJson(Heartbeatdoc, HeartBeatString);


  StaticJsonDocument<200> doc1;

  doc1["type"] = "CONNECT";

  String CONNECTSTRING;
  serializeJson(doc1, CONNECTSTRING);

  ConnectChannel.send(CONNECTSTRING);

  StaticJsonDocument<200> doc2;

  // Populate the JSON object
  doc2["type"] = "LAUNCH";
  doc2["appId"] = "YouTube";
  doc2["requestId"] = "1";

  // Convert JSON object to String
  String LaunchString;
  serializeJson(doc2, LaunchString);

  RecieverChannel.send(LaunchString);

  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {  // Loop for 5 seconds
    RecieverChannel.checkForMessages();
    HeartBeat.send(HeartBeatString);
    delay(1000);  // Short delay to prevent excessive CPU usage
  }

  Serial.println("Finished checking for messages.");
}



bool ESPmDNSHelper::initializeClient(const char* _host, uint16_t _port)
{
  host = _host;
  port = _port;

  BearSSLClient SSLClient(client);

  if (SSLClient.connect(host.c_str(), port)) {

    Serial.println("Connected");

    return true;

   
  } else {
      Serial.println("Failed to connect securely. Entirely");
      return false;
  }
}