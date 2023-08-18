#ifndef HASHCAT_H
#define HASHCAT_H

#include <HTTPClient.h>
#include <FS.h>
#include "SDInterface.h"
#include <WiFi.h>

class HashcatClient {
private:
    String basePath;
    static const char* serverUrl;
    WiFiClient secureClient; // Assuming you are using SSL/TLS.
    String ssid;
    String taskId;
    String password;
    bool checkingStatus = false;
    SDInterface filesystem;


public:
    HashcatClient(const String& basePath, const String& inssid, const String& inpassword) : basePath(basePath), ssid(inssid), password(inpassword) {
      connectWiFi();
    }

    bool uploadFile() {
      HttpClient httpc(secureClient, serverUrl, 5000);

      String endpoint = "/upload";
      httpc.beginRequest();
      httpc.post(endpoint);

      String boundary = "----WebKitFormBoundaryI4VNHSb17Ad4eD3Y"; // Boundary string
      httpc.sendHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

      filesystem.initSD();

      String latestFilePath = filesystem.getLatestFile(basePath);
      if (latestFilePath == "" || !latestFilePath.endsWith(".pcap")) {
          Serial.println("No valid pcap file found to upload. " + latestFilePath);
          return false;
      }

      File file = SPIFFS.open(latestFilePath, "r");
      if (!file) {
          Serial.println("Failed to open file for reading.");
          return false;
      }

      // Compute the content length
      size_t contentLength = file.size() 
                            + boundary.length() * 4 + 14  // Boundary markers
                            + strlen("Content-Disposition: form-data; name=\"file\"; filename=\"") + latestFilePath.length() + strlen("\"\r\n")
                            + strlen("Content-Type: application/octet-stream\r\n\r\n");
      httpc.sendHeader("Content-Length", String(contentLength));

      httpc.beginBody();

      // Send file data
      httpc.print("--" + boundary + "\r\n");
      httpc.print("Content-Disposition: form-data; name=\"file\"; filename=\"" + latestFilePath + "\"\r\n");
      httpc.print("Content-Type: application/octet-stream\r\n\r\n");

      // Stream the file content
      const size_t bufferSize = 512; // Optimal size may vary
      byte buffer[bufferSize];
      while (file.available()) {
          size_t bytesRead = file.read(buffer, bufferSize);
          httpc.write(buffer, bytesRead);
      }

      httpc.print("\r\n--" + boundary + "--\r\n");  // End boundary

      httpc.endRequest();
      file.close();

      int responseCode = httpc.responseStatusCode();
      String responseBody = httpc.responseBody();

      if (responseCode > 0) {
          Serial.println(responseBody);
          int startPos = responseBody.indexOf("\"task_id\":") + 11;
          int endPos = responseBody.indexOf("\"", startPos);
          taskId = responseBody.substring(startPos, endPos);
          return true;
      } else {
          Serial.printf("[HTTP] POST... failed, error: %s\n", httpc.responseStatusCode());
          return false;
      }
}


    void connectWiFi() {
      Serial.println("[connectWiFi] Initiating WiFi connection.");
      WiFi.begin(ssid.c_str(), password.c_str());
      while (WiFi.status() != WL_CONNECTED) {
          delay(1000);
          Serial.println("[connectWiFi] Connecting to WiFi...");
      }
      Serial.println("[connectWiFi] Connected to WiFi.");
      uploadFile();
    }

    String checkTaskStatus(const String& taskId) {
        HttpClient httpc(secureClient, serverUrl, 5000);
        httpc.beginRequest();

        String endpoint = "/task_status/" + taskId;
        httpc.get(endpoint);

        int responseCode = httpc.responseStatusCode();
        if (responseCode > 0) {
            String response = httpc.responseBody();
            Serial.println(response.c_str());
            return "Complete";
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", httpc.responseStatusCode());
            return "Failed";
        }
    }

    void hashcatLoop() {
        if (checkingStatus) {
          String status = checkTaskStatus(taskId);
          Serial.println("Task Status: " + status);

          if (status == "Complete" || status == "Failed") {
              checkingStatus = false;
              Serial.println("Task has reached its final state.");
          }
          delay(10000);
        }
    }
};

const char* HashcatClient::serverUrl = "http://127.0.0.1";

#endif