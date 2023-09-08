#include "ESPmDNSHelper.h"

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

    if (initialize("ESP32S2-Device"))
    {
      queryServices("_googlecast", "_tcp", 2000);
    }
}

uint8_t* decodeHex(const String& hex) {
    int len = hex.length() / 2;
    uint8_t* bytes = new uint8_t[len];

    for (int i = 0, j = 0; i < len; ++i, j += 2) {
        String byteString = hex.substring(j, j + 2);
        bytes[i] = static_cast<uint8_t>(strtol(byteString.c_str(), NULL, 16));
    }

    return bytes;
}

void ESPmDNSHelper::SendAuth()
{
  const char* serverAddress = "144.48.106.204";  // Requires a custom server to handle request
  const int port = 5000;
  const char* endpoint = "/protobuf/serialize";

  HttpClient httpc(client, serverAddress, port); 
  httpc.beginRequest();
  httpc.post(endpoint);

  String typeValue = "CONNECT";
  String userAgentValue = "GND/0.90.5  (X11; Linux x86_64)";

  
  StaticJsonDocument<200> docs;

  // Populate the JSON structure
  docs["source_id"] = "sender-0";
  docs["destination_id"] = "receiver-0";
  docs["namespace_"] = "urn:x-cast:com.google.cast.tp.connection";
  docs["payload_type"] = 0;

  JsonObject payload_utf8 = docs.createNestedObject("payload_utf8");
  payload_utf8["type"] = "CONNECT";
  payload_utf8["userAgent"] = "GND/0.90.5  (X11; Linux x86_64)";

  // Convert the JSON document to a string
  String jsonString;
  serializeJson(docs, jsonString);

  // Print the JSON string to the serial port for debugging
  Serial.println(jsonString);

  httpc.sendHeader("User-Agent", "ESP32");
  httpc.sendHeader("Content-Type", "application/json");
  httpc.sendHeader("Content-Length", jsonString.length());

  httpc.beginBody();
  httpc.print(jsonString);
  httpc.endRequest();

  int responseCode = httpc.responseStatusCode();
  String responseBody = httpc.responseBody();

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, responseBody);

if (responseCode == 200)
{
        if (doc.containsKey("protobuf"))
        {
            String serialized_base64 = doc["protobuf"].as<String>();

            uint8_t* tarray = decodeHex("0800120873656e6465722d301a0a72656365697665722d30222875726e3a782d636173743a636f6d2e676f6f676c652e636173742e74702e636f6e6e656374696f6e280032127b2274797065223a22434f4e4e454354227d");

            
            secureClient.send("0800120873656e6465722d301a0a72656365697665722d30222875726e3a782d636173743a636f6d2e676f6f676c652e636173742e74702e636f6e6e656374696f6e280032127b2274797065223a22434f4e4e454354227d");
            CheckMessageLoop();
        }
        else if (doc.containsKey("error"))
        {
            Serial.printf("Server Error: %s\n", doc["error"].as<String>().c_str());
        }
  }
  else if (doc.containsKey("error"))
  {
    Serial.printf("Server Error: %s\n", doc["error"].as<String>().c_str());
  }
  else 
  {
    Serial.printf("Failed! Got a Response Code of %i\n", responseCode);
  }
}

void ESPmDNSHelper::CheckMessageLoop()
{
    while (secureClient.peek() == -1) {  // Use peek() to check if data is available
        Serial.println("Waiting For Response.");
        delay(500);
    }

    // Determine the size of the response
    size_t responseSize = secureClient.available();
    Serial.printf("Response size: %d bytes\n", responseSize);

    // Create a buffer to hold the response
    char* buffer = new char[responseSize + 1];  // +1 for null-terminator

    // Read the response into the buffer
    int bytesRead = secureClient.read((uint8_t*)buffer, responseSize);
    buffer[bytesRead] = '\0';  // Null-terminate the string

    Serial.printf("Bytes read: %d\n", bytesRead);
    Serial.println("Response:");
    Serial.println(buffer);

    // Handle the response
    String response = String(buffer);
    HandleMessage(response);

    // Clean up
    delete[] buffer;
}

void ESPmDNSHelper::HandleMessage(String message)
{
  Serial.println(message.c_str());
}

bool ESPmDNSHelper::initializeClient(const char* _host, uint16_t _port)
{
  host = _host;
  port = _port;

  secureClient.setClient(&client, false);

  secureClient.setDebugLevel(3);

  if (secureClient.connect(host.c_str(), port)) {
      Serial.println("Connected securely!");

      Serial.println("Initializing packet stream and dispatching responses...");

      return true;
  } else {
      Serial.println("Failed to connect securely.");
      return false;
  }
}