#pragma once

#include <WiFiClientSecure.h>
#include <ESP_SSLClient.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

#define MAX_BUFFER_SIZE 1024


int threshold = 50;


byte hexCharToByte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}


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
    void (*messageCallback)(BSSL_TCP_Client, String, String);

  public:
    Channel(BSSL_TCP_Client& clientRef, String srcId, String destId, String ns, String enc = "")
      : client(clientRef), sourceId(srcId), destinationId(destId), namespace_(ns), encoding(enc), messageCallback(nullptr) {}

    void setMessageCallback(void (*callback)(BSSL_TCP_Client, String, String)) {
      messageCallback = callback;
    }

    String Deserialize_Internal(String serializedData) {
      const char* serverAddress = "144.48.106.204";  // Your server address
      const int port = 5000;
      const char* endpoint = "/protobuf/deserialize";

      String Payload;
      DynamicJsonDocument serializeddoc(1024);
      serializeddoc["protobuf"] = serializedData;
      serializeJson(serializeddoc, Payload);

      HttpClient httpc(unsecureclient, serverAddress, port); 
      httpc.beginRequest();
      httpc.post(endpoint);

      httpc.sendHeader("User-Agent", "ESP32");
      httpc.sendHeader("Content-Type", "application/json");
      httpc.sendHeader("Content-Length", serializedData.length());

      httpc.beginBody();
      httpc.print(Payload);
      httpc.endRequest();

      int responseCode = httpc.responseStatusCode();
      String responseBody = httpc.responseBody();

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, responseBody);

      if (responseCode == 200) {
        if (doc.containsKey("data")) {
          String jsonData = doc["data"].as<String>();
          return jsonData;
        } else if (doc.containsKey("error")) {
          Serial.printf("Server Error: %s\n", doc["error"].as<String>().c_str());
          return "";
        }
      } else {
        Serial.printf("Failed! Got a Response Code of %i\n", responseCode);
        return "";
      }
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


      StaticJsonDocument<800> docs;


      docs["source_id"] = sourceId;
      docs["destination_id"] = destinationId;
      docs["namespace_"] = namespace_;
      docs["payload_type"] = 0;


      DynamicJsonDocument payloadDoc(1024);
      deserializeJson(payloadDoc, Data);

      docs["payload_utf8"] = payloadDoc;

      
      String jsonString;
      serializeJson(docs, jsonString);

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
      const int maxBytesToRead = 256;
      byte buffer[maxBytesToRead];
      int bytesRead = client.read(buffer, min(client.available(), maxBytesToRead));

      String serializedData = "";

      for (int i = 0; i < bytesRead; i++) {
          if (buffer[i] < 16) serializedData += '0'; 
          serializedData += String(buffer[i], HEX);
      }

      if (bytesRead > 0) 
      {
          Serial.println("Received data: " + serializedData);
          String jsonData = Deserialize_Internal(serializedData);

          onMessage(sourceId, destinationId, namespace_, jsonData);

          if (jsonData != "") {
              Serial.println("Deserialized data: " + jsonData);
          } else 
          {
              Serial.println("Failed to deserialize data or received empty response.");
          }
      }
      else 
      {
          Serial.println("Received data length is not appropriate.");
      }
    }



    void onMessage(String srcId, String destId, String ns, String data) {
        String decodedData = decode(data);

        
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, decodedData);

        
        String Session = doc["sessionId"].as<String>();

        
        if (messageCallback) {
            messageCallback(client, Session, decodedData);
        }
    }

    void send(String Data) {
      String serializedData = Seralize_Internal(Data);
      byte buffer[4 + serializedData.length() / 2];  

      
      uint32_t dataLength = serializedData.length() / 2;
      buffer[0] = (dataLength >> 24) & 0xFF;
      buffer[1] = (dataLength >> 16) & 0xFF;
      buffer[2] = (dataLength >> 8) & 0xFF;
      buffer[3] = dataLength & 0xFF;

      
      hexStringToBytes(serializedData, buffer + 4, dataLength);

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