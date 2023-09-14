#include "ESPmDNSHelper.h"
#include "CastChannel.h"

ESPmDNSHelper::ESPmDNSHelper(const char* inSsid, const char* inpaSsword) {
    mdns = new MDNSResponder();
    ssid = inSsid;
    password = inpaSsword;
    connectWiFi();
}

void LoadMedia(Channel MediaChannel, String contentId, String contentType, String title, String imageUrl, bool autoplay = false, float currentTime = 0.0, String repeatMode = "REPEAT_OFF") {

  StaticJsonDocument<600> mediaDoc;
  mediaDoc["contentId"] = contentId.c_str();
  mediaDoc["contentType"] = contentType.c_str();
  mediaDoc["streamType"] = "BUFFERED";
  JsonObject metadata = mediaDoc.createNestedObject("metadata");
  metadata["type"] = 0;
  metadata["metadataType"] = 0;
  metadata["title"] = title.c_str();
  JsonArray images = metadata.createNestedArray("images");
  JsonObject image1 = images.createNestedObject();
  image1["url"] = imageUrl.c_str();

  StaticJsonDocument<700> loadDoc;
  loadDoc["type"] = "LOAD";
  loadDoc["autoplay"] = autoplay;
  loadDoc["currentTime"] = currentTime;
  loadDoc["repeatMode"] = repeatMode.c_str();
  loadDoc["media"] = mediaDoc;
  loadDoc["requestId"] = 1;

  String loadString;
  serializeJson(loadDoc, loadString);
  MediaChannel.send(loadString);
}


void HandleMessage(BSSL_TCP_Client SSLClient, String Session, String Data)
{
  if (!Session.isEmpty())
  {
    Channel ConnectChannel(SSLClient, "sender-0", Session, "urn:x-cast:com.google.cast.tp.connection", "JSON");
    Channel MediaChannel(SSLClient, "sender-0",  Session, "urn:x-cast:com.google.cast.media", "JSON");

    StaticJsonDocument<200> doc1;

    doc1["type"] = "CONNECT";

    String CONNECTSTRING;
    serializeJson(doc1, CONNECTSTRING);

    ConnectChannel.send(CONNECTSTRING);

    delay(1500);

    LoadMedia(MediaChannel, "https://cdn.discordapp.com/attachments/1044679579509461003/1150983763984121966/h.mp4", "video/mp4", "AH", "https://cdn.discordapp.com/attachments/1044679579509461003/1150951024899657829/image.png", true);

  }

}

ESPmDNSHelper::~ESPmDNSHelper() {
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
            
          if (serviceIP.toString() == "192.168.1.165")
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
  Channel ConnectChannel(SSLClient, "sender-0", "receiver-0", "urn:x-cast:com.google.cast.tp.connection", "JSON");
  Channel HeartBeat(SSLClient, "sender-0", "receiver-0", "urn:x-cast:com.google.cast.tp.heartbeat", "JSON");
  Channel RecieverChannel(SSLClient, "sender-0", "receiver-0", "urn:x-cast:com.google.cast.receiver", "JSON");
  Channel MediaChannel(SSLClient, "sender-0",  "receiver-0", "urn:x-cast:com.google.cast.media", "JSON");


  RecieverChannel.setMessageCallback(HandleMessage);
  HeartBeat.setMessageCallback(HandleMessage);
  ConnectChannel.setMessageCallback(HandleMessage);
  MediaChannel.setMessageCallback(HandleMessage);


  StaticJsonDocument<200> Heartbeatdoc;

  Heartbeatdoc["type"] = "PING";

  String HeartBeatString;
  serializeJson(Heartbeatdoc, HeartBeatString);


  StaticJsonDocument<200> doc1;

  doc1["type"] = "CONNECT";

  String CONNECTSTRING;
  serializeJson(doc1, CONNECTSTRING);

  ConnectChannel.send(CONNECTSTRING);


  HeartBeat.send(HeartBeatString);

  StaticJsonDocument<200> doc2;
  doc2["type"] = "LAUNCH";
  doc2["appId"] = "CC1AD845";
  doc2["requestId"] = 1;
  String LaunchString;
  serializeJson(doc2, LaunchString);
  RecieverChannel.send(LaunchString);

  unsigned long startTime = millis();
  while (millis() - startTime < 20000) {
    RecieverChannel.checkForMessages();
      HeartBeat.send(HeartBeatString);
    delay(1000);
  }

  Serial.println("Finished checking for messages.");
}



bool ESPmDNSHelper::initializeClient(const char* _host, uint16_t _port)
{
  host = _host;
  port = _port;

  SSLClient.setClient(&unsecureclient);

  SSLClient.allowSelfSignedCerts();

  SSLClient.setDebugLevel(4);

  if (SSLClient.connect(host.c_str(), port)) {

    Serial.println("Connected");

    SSLClient.setInsecure();

    SSLClient.connectSSL();

    return true;
  } else {
      Serial.println("Failed to connect securely. Entirely");
      return false;
  }
}