#include "NetflixController.h"

void NetflixController::launchApp(const String& appUrl) {
    int startPos = appUrl.indexOf('/', 7);
    String basePath = (startPos != -1) ? appUrl.substring(startPos) : "/";

    String Netflixpath = basePath + "/Netflix";

    IPAddress extractedIp;
    uint16_t extractedPort;
    extractIPAndPort(appUrl, extractedIp, extractedPort);

    HttpClient httpc(client, extractedIp, extractedPort);

    httpc.beginRequest();

    int httpCode = httpc.post(Netflixpath);
    httpc.sendHeader("User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Mobile Safari/537.36");
    httpc.endRequest();

    if (httpCode == 0) { // 0 Means Success For Netflix
        Serial.println("Successfully launched the Netflix app.");
    } else {
        Serial.println("Failed to launch the Netflix app. HTTP Response Code: " + String(httpCode));
  }
}