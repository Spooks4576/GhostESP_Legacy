#pragma once

#include <WiFiClientSecure.h>
#include <ESP_SSLClient.h>
#include <ArduinoJson.h>
#include "CastSerializer.h"
#include <ArduinoHttpClient.h>
#include "DialClient.h"
#include <functional>

#define MAX_BUFFER_SIZE 1024

int threshold = 50;
const uint8_t MAX_STRING_SIZE = 100;

byte hexCharToByte(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0;
}

bool isValidSessionId(const String& sessionId) {
  // Check if the sessionId is in the UUID format
  // e.g., 550e8400-e29b-41d4-a716-446655440000
  return (sessionId.length() == 36) && (sessionId[8] == '-') && (sessionId[13] == '-') && (sessionId[18] == '-') && (sessionId[23] == '-');
}

String bufferToHexString(const uint8_t* buffer, uint16_t length) {
  String result = "";
  for (uint16_t i = 0; i < length; i++) {
    char hex[3];  // Two characters for the byte and one for the null terminator
    sprintf(hex, "%02X", buffer[i]);
    result += hex;
  }
  return result;
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

uint16_t hexStringToBytes_U(const String& hexString, uint8_t* buffer, uint16_t bufferSize) {
  uint16_t byteCount = 0;
  for (uint16_t i = 0; i < hexString.length() && byteCount < bufferSize; i += 2) {
    char c1 = hexString[i];
    char c2 = hexString[i + 1];
    buffer[byteCount] = (hexCharToByte(c1) << 4) | hexCharToByte(c2);
    byteCount++;
  }
  return byteCount;
}

class Channel {
private:
  BSSL_TCP_Client& client;
  WiFiClient unsecureclient;
  WiFiClientSecure secureClient;
  String sourceId;
  String destinationId;
  String namespace_;
  String encoding;
  RID rid;
  std::function<void(String, String)> messageCallback;

public:
  String YTUrl;
  Channel(BSSL_TCP_Client& clientRef, String srcId, String destId, String ns, String enc = "")
    : client(clientRef), sourceId(srcId), destinationId(destId), namespace_(ns), encoding(enc), messageCallback(nullptr) {}


  String zx() {
    String result = "";
    const char* characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int charactersLength = strlen(characters);
    const int stringLength = 12;

    for (int i = 0; i < stringLength; i++) {
      char randomChar = characters[random(0, charactersLength)];
      result += randomChar;
    }

    return result;
  }

  String extractJSON(const String& response) {
    // Find the start of the JSON content (assuming it always starts with '[')
    int startIndex = response.indexOf("[[");
    if (startIndex == -1) return "";  // JSON not found

    // Find the end of the JSON content
    int endIndex = response.lastIndexOf("]");

    // Extract the JSON
    return response.substring(startIndex, endIndex + 1);
  }


  String generateUUID() {
    String uuid = "";

    randomSeed(analogRead(0) + millis());

    for (int i = 0; i < 4; i++) {
      uuid += String(random(0xFFFFFFF), HEX);
    }

    return uuid;
  }

  void BindSessionID(DIALClient::Device& Device) {
    const char* serverAddress = "www.youtube.com";
    const int port = 443;
    const char* endpoint = "/api/lounge/bc/bind";


    if (!secureClient.connect(serverAddress, port)) {
      Serial.println("Connection failed!");
      return;
    }

    Device.UUID = generateUUID();


    String urlParams = "device=REMOTE_CONTROL";
    urlParams += "&mdx-version=3";
    urlParams += "&ui=1";
    urlParams += "&v=2";
    urlParams += "&name=Flipper_0";
    urlParams += "&app=youtube-desktop";
    urlParams += "&loungeIdToken=" + Device.YoutubeToken;
    urlParams += "&id=" + Device.UUID;
    urlParams += "&VER=8";
    urlParams += "&CVER=1";
    urlParams += "&zx=" + zx();
    urlParams += "&RID=" + String(rid.next());


    String jsonData = "{count: 0 }";

    Serial.println(String(endpoint) + "?" + urlParams);


    secureClient.print("POST " + String(endpoint) + "?" + urlParams + " HTTP/1.1\r\n");
    secureClient.print("Host: " + String(serverAddress) + "\r\n");
    secureClient.print("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.45 Safari/537.36\r\n");
    secureClient.print("Content-Type: application/json\r\n");
    secureClient.print("Content-Length: " + String(jsonData.length()) + "\r\n");
    secureClient.print("Origin: https://www.youtube.com\r\n");
    secureClient.print("\r\n");
    secureClient.print(jsonData);


    while (!secureClient.available()) {
      delay(10);
    }


    String response = secureClient.readString();

    DynamicJsonDocument doc(4096);
    Serial.println(extractJSON(response));
    DeserializationError error = deserializeJson(doc, extractJSON(response));

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }

    JsonArray array = doc.as<JsonArray>();
    String gsession;
    String SID;
    String LID;

    for (JsonVariant v : array) {
      if (v[0].as<int>() == 0 && v[1][0].as<String>() == "c") {
        SID = v[1][1].as<String>();
      }
      if (v[0].as<int>() == 1 && v[1][0].as<String>() == "S") {
        gsession = v[1][1].as<String>();
      }
      if (v[0].as<int>() == 3 && v[1][0].as<String>() == "playlistModified") {
        LID = v[1][1]["listId"].as<String>();
      }
    }

    Serial.println("gsession: " + gsession);
    Serial.println("SID: " + SID);

    Device.gsession = gsession;
    Device.SID = SID;
    Device.listID = LID;

    secureClient.stop();
  }

  void sendYouTubeCommand(const String& command, const String& videoId, const DIALClient::Device& device) {
    const char* serverAddress = "www.youtube.com";
    const int port = 443;
    const char* endpoint = "/api/lounge/bc/bind";


    if (!secureClient.connect(serverAddress, port)) {
      Serial.println("Connection failed!");
      return;
    }

    String urlParams = "device=REMOTE_CONTROL";
    urlParams += "&loungeIdToken=" + device.YoutubeToken;
    urlParams += "&id=" + device.UUID;
    urlParams += "&VER=8";
    urlParams += "&zx=" + zx();
    urlParams += "&SID=" + device.SID;
    urlParams += "&RID=" + String(rid.next());
    urlParams += "&AID=" + String("5");
    urlParams += "&gsessionid=" + device.gsession;


    Serial.println(String(endpoint) + "?" + urlParams);

    String formData;
    formData += "count=1";
    formData += "&ofs=0";
    formData += "&req0__sc=" + command;
    formData += "&req0_videoId=" + videoId;
    formData += "&req0_listId=" + device.listID;

    Serial.println(formData);

    secureClient.print("POST " + String(endpoint) + "?" + urlParams + " HTTP/1.1\r\n");
    secureClient.print("Host: " + String(serverAddress) + "\r\n");
    secureClient.print("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.45 Safari/537.36\r\n");
    secureClient.print("Content-Type: application/x-www-form-urlencoded\r\n");
    secureClient.print("Content-Length: " + String(formData.length()) + "\r\n");
    secureClient.print("Origin: https://www.youtube.com\r\n");
    secureClient.print("\r\n");
    secureClient.print(formData);


    while (!secureClient.available()) {
      delay(10);
    }


    String response = secureClient.readString();
    Serial.println("Set playlist Response");
    Serial.println(response);
  }

  String getYouTubeToken(const String& screenId) {
    secureClient.setInsecure();
    const char* serverAddress = "www.youtube.com";
    const int port = 443;
    const char* endpoint = "/api/lounge/pairing/get_lounge_token_batch";

    if (secureClient.connect(serverAddress, port)) {
      Serial.println("Connected to server.");


      String postData = "screen_ids=" + screenId;
      secureClient.print("POST " + String(endpoint) + " HTTP/1.1\r\n");
      secureClient.print("Host: " + String(serverAddress) + "\r\n");
      secureClient.print("User-Agent: ESP32\r\n");
      secureClient.print("Content-Type: application/x-www-form-urlencoded\r\n");
      secureClient.print("Content-Length: " + String(postData.length()) + "\r\n");
      secureClient.print("\r\n");
      secureClient.print(postData);


      while (!secureClient.available()) {
        delay(10);
      }


      String entireResponse = secureClient.readString();


      int firstNewline = entireResponse.indexOf('\n');
      String statusLine = entireResponse.substring(0, firstNewline);
      int statusCode = statusLine.substring(9, 12).toInt();
      Serial.println("YouTube API response status: " + String(statusCode));

      String responseBody = "";


      int endOfHeaders = entireResponse.indexOf("\n\r\n");
      if (endOfHeaders != -1) {
        responseBody = entireResponse.substring(endOfHeaders + 3);
        Serial.println("YouTube API response content: " + responseBody);
      } else {
        Serial.println("Failed to parse response headers.");
      }


      int startOfJson = responseBody.indexOf('{');
      if (startOfJson != -1) {
        responseBody = responseBody.substring(startOfJson);
      } else {
        Serial.println("Failed to find start of JSON content.");
        return "";
      }

      if (statusCode == 200) {

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, responseBody);
        String loungeToken = doc["screens"][0]["loungeToken"].as<String>();
        Serial.println("Lounge Token: " + loungeToken);
        secureClient.stop();
        return loungeToken;
      } else {
        Serial.println("Failed to retrieve token. HTTP Response Code: " + String(statusCode));
        return "";
      }
    } else {
      Serial.println("Failed to connect to server.");
      return "";
    }
  }

  void setMessageCallback(std::function<void(String, String)> callback) {
    messageCallback = callback;
  }

  String Deserialize_Internal(String serializedData) {
    uint8_t* buffer = new uint8_t[1000];
    uint16_t byteCount = hexStringToBytes_U(serializedData, buffer, 1000);

    char* payloadUtf8 = new char[1000];
    ExpandedCastMessageSerializer::DeserializationResult result = ExpandedCastMessageSerializer::deserialize(buffer, byteCount, payloadUtf8);

    String returnString = "";
    if (result == ExpandedCastMessageSerializer::DESERIALIZATION_SUCCESS) {
      returnString = String(payloadUtf8);
    } else {
      Serial.println("Failed to Deserialize");
    }

    delete[] buffer;
    delete[] payloadUtf8;

    return returnString;
  }



  String Seralize_Internal(String Data) {

    ExpandedCastMessageSerializer::CastMessage message;
    message.protocol_version = ExpandedCastMessageSerializer::CASTV2_1_0;
    strncpy(message.source_id, sourceId.c_str(), MAX_STRING_SIZE - 1);
    message.source_id[MAX_STRING_SIZE - 1] = '\0';  // Ensure null-termination
    strncpy(message.destination_id, destinationId.c_str(), MAX_STRING_SIZE - 1);
    message.destination_id[MAX_STRING_SIZE - 1] = '\0';  // Ensure null-termination
    strncpy(message.namespace_, namespace_.c_str(), MAX_STRING_SIZE - 1);
    message.namespace_[MAX_STRING_SIZE - 1] = '\0';  // Ensure null-termination
    strncpy(message.payload_utf8, Data.c_str(), 500 - 1);
    message.payload_utf8[500 - 1] = '\0';  // Ensure null-termination
    message.payload_type = ExpandedCastMessageSerializer::STRING;
    memset(message.payload_binary, 0, 100);
    message.payload_binary_size = 0;

    Serial.println("Input Data length: " + String(Data.length()));

    uint8_t buffer[1000];
    uint16_t index = 0;
    ExpandedCastMessageSerializer::SerializationResult result = ExpandedCastMessageSerializer::serialize(message, buffer, index, sizeof(buffer));

    if (result == ExpandedCastMessageSerializer::SUCCESS) {

      String hex = bufferToHexString(buffer, index);
      return hex;
    } else {
      Serial.println("Serialization Error Occured");
      return "";
    }
  }

  void checkForMessages() {
    const int maxBytesToRead = 500;
    byte buffer[maxBytesToRead];
    int bytesRead = client.read(buffer, min(client.available(), maxBytesToRead));

    String serializedData = "";

    for (int i = 0; i < bytesRead; i++) {
      if (buffer[i] < 16) serializedData += '0';
      serializedData += String(buffer[i], HEX);
    }

    if (bytesRead > 0) {
      Serial.println("Received data: " + serializedData);
      String jsonData = Deserialize_Internal(serializedData);

      onMessage(sourceId, destinationId, namespace_, jsonData);

      if (jsonData != "") {
        Serial.println("Deserialized data: " + jsonData);
      } else {
        Serial.println("Failed to deserialize data or received empty response.");
      }
    } else {
      Serial.println("Received data length is not appropriate.");
    }
  }



  void onMessage(String srcId, String destId, String ns, String data) {
    // Check if the data is a valid session ID first
    if (data != "" && isValidSessionId(data)) {
      Serial.println("Session ID: " + data);
      messageCallback(data, data);
    }

    else if (data != "") {
      // Parse the JSON data
      DynamicJsonDocument doc(700);
      DeserializationError error = deserializeJson(doc, data);


      if (!error && doc["data"].containsKey("screenId")) {
        String screenId = doc["data"]["screenId"].as<String>();
        Serial.println("Found screenId: " + screenId);
        DIALClient::Device ccdevice;
        ccdevice.screenID = screenId;
        client.stop();
        secureClient.setInsecure();
        String YTToken = getYouTubeToken(screenId);
        ccdevice.YoutubeToken = YTToken;
        BindSessionID(ccdevice);

        if (YTUrl.isEmpty())
        {
          sendYouTubeCommand("setPlaylist", "dQw4w9WgXcQ", ccdevice);
        }
        else 
        {
          sendYouTubeCommand("setPlaylist", YTUrl.c_str(), ccdevice);
        }

        
      } else {
        Serial.println("Other valid data received: " + data);
      }
    } else {
      Serial.println("deviceId not found in nested JSON");
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
};