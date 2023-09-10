#pragma once

#include <WiFiClientSecure.h>
#include <ESP_SSLClient.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

#define MAX_BUFFER_SIZE 1024


byte hexCharToByte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0; // Not a valid hex character
}

// Function to convert a hex string to bytes
void hexStringToBytes(const String& hex, byte* buffer, int bufferSize) {
    int byteCount = 0;
    for (int i = 0; i < hex.length() && byteCount < bufferSize; i += 2) {
        char c1 = hex.charAt(i);
        char c2 = hex.charAt(i + 1);
        buffer[byteCount] = hexCharToByte(c1) * 16 + hexCharToByte(c2);
        byteCount++;
    }
}

class Channel {
  private:
    BSSL_TCP_Client& client;
    WiFiClient unsecureclient;
    String sourceId;
    String destinationId;
    String namespace_;
    String encoding;
    void (*messageCallback)(String);  // Callback function to handle received messages

  public:
    Channel(BSSL_TCP_Client& clientRef, String srcId, String destId, String ns, String enc = "")
      : client(clientRef), sourceId(srcId), destinationId(destId), namespace_(ns), encoding(enc), messageCallback(nullptr) {}

    void setMessageCallback(void (*callback)(String)) {
      messageCallback = callback;
    }

    String Seralize_Internal(String Data)
    {
      const char* serverAddress = "144.48.106.204";  // Requires a custom server to handle request
      const int port = 5000;
      const char* endpoint = "/protobuf/serialize";

      HttpClient httpc(unsecureclient, serverAddress, port); 
      httpc.beginRequest();
      httpc.post(endpoint);

      String typeValue = "CONNECT";
      String userAgentValue = "GND/0.90.5  (X11; Linux x86_64)";


      StaticJsonDocument<200> docs;


      docs["source_id"] = sourceId;
      docs["destination_id"] = destinationId;
      docs["namespace_"] = namespace_;
      docs["payload_type"] = 1;

      //JsonObject payload_utf8 = docs.createNestedObject("payload_utf8");
      docs["payload_utf8"] = Data;

      
      String jsonString;
      serializeJson(docs, jsonString);

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

          return serialized_base64;
        }
        else if (doc.containsKey("error"))
        {
            Serial.printf("Server Error: %s\n", doc["error"].as<String>().c_str());
            return "";
        }
        }
        else if (doc.containsKey("error"))
        {
          Serial.printf("Server Error: %s\n", doc["error"].as<String>().c_str());
          return "";
        }
        else 
        {
          Serial.printf("Failed! Got a Response Code of %i\n", responseCode);
          return "";
        }
      }

    void checkForMessages() {
      if (client.available()) 
      {

        int bytes = client.available();
        Serial.println("Received Bytes...");

        for (int i = 0; i < bytes; i++) 
        {
            byte b = client.read(); // Read one byte
            if (b < 16) Serial.print('0'); // Print leading zero for single-digit hex values
            Serial.print(b, HEX);
            Serial.print(" "); // Optional: Add space between bytes for clarity
        }
        Serial.println();
      }
    }

    void onMessage(String srcId, String destId, String ns, String data) {
      if (srcId != destinationId) return;
      if (destId != sourceId && destId != "*") return;
      if (ns != namespace_) return;

      String decodedData = decode(data);
      if (messageCallback) {
        messageCallback(decodedData);
      }
    }

    void send(String Data) {
      String serializedData = Seralize_Internal(Data);
      byte buffer[serializedData.length() / 2];  // Each byte is represented by 2 hex chars

      hexStringToBytes(serializedData, buffer, sizeof(buffer));

      Serial.println(serializedData);

      client.write(buffer, sizeof(buffer));
    }

    String encode(String data) {
      if (encoding == "JSON") {
        DynamicJsonDocument doc(1024);
        doc["data"] = data;
        String result;
        serializeJson(doc, result);
        return result;
      }
      return data;
    }

    String decode(String data) {
      if (encoding == "JSON") {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data);
        return doc["data"].as<String>();
      }
      return data;
    }
};