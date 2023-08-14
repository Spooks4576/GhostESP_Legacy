#ifndef DIALCLIENT_H
#define DIALCLIENT_H

#define LOGGING

#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>
#include <TinyXML.h>
#include <set>
#include <ArduinoJson.h>
#include <vector>

struct HTTPResponse_C {
    int statusCode;
    String headers;
    String body;
};

struct MACAddress {
    uint8_t bytes[6];
};

struct MagicPacket {
    uint8_t header[6];
    MACAddress payload[16];
};

const int retryCount = 5;
const int delayBetweenRetries = 1000; // milliseconds




class WakeOnLan {
public:
    WiFiUDP udp;

    // Construct the magic packet from a given MAC address
    MagicPacket makeMagicPacket(const uint8_t addr[6]) {
    MagicPacket magicPacket;

    // Fill the header with 0xFF
    for (int i = 0; i < 6; i++) {
        magicPacket.header[i] = 0xFF;
    }

    // Repeat the MAC address 16 times
    for (int i = 0; i < 16; i++) {
        memcpy(magicPacket.payload[i].bytes, addr, 6);
    }

    return magicPacket;
  }

  uint8_t* marshalMagicPacket(const MagicPacket& packet) {
    // Allocate memory for the serialized packet
    uint8_t* serialized = new uint8_t[sizeof(MagicPacket)];

    // Here, we can just do a direct memory copy since our structure
    // is a direct representation of the packet format
    memcpy(serialized, &packet, sizeof(MagicPacket));

    return serialized;
  }
public:
    WakeOnLan() {}

    // Send the Wake-on-LAN magic packet
  void send(const uint8_t mac[6], IPAddress broadcastIP, uint16_t port = 9, int retries = 3, int timeoutInSeconds = 3) {
    MagicPacket magicPacket = makeMagicPacket(mac);
    uint8_t* serializedPacket = marshalMagicPacket(magicPacket);

    timeoutInSeconds = 3;
    int timeoutInMilliseconds = timeoutInSeconds * 1000; // Convert to milliseconds

    for(int attempt = 0; attempt < retries; attempt++) {
        udp.beginMulticast(broadcastIP, port);
        udp.beginPacket(broadcastIP, port);
        udp.write(serializedPacket, sizeof(MagicPacket));  // Use the size of the struct
        udp.endPacket();

        delay(timeoutInMilliseconds);
        // Check for a response or some confirmation mechanism here.
        // If confirmation is received, break out of the loop.
    }

    delete[] serializedPacket; // Make sure to free the allocated memory
  }
};

class DIALClient {
public:
    String ssid;
    String password;
    String yturl;
    WiFiClient client;
    WiFiClientSecure secureClient;
    WiFiUDP multicastClient;
    WakeOnLan wol;
public:

    struct AppInfo {
        String name;
        String state;
        bool allowStop;
        String rel;
        String href;
        String additionalData;
    };

    struct Device {
        String uniqueServiceName;
        String location;
        String applicationUrl;
        String friendlyName;
        String wakeup;
        AppInfo appinfo;
        String screenID;
        String YoutubeToken;
    };

    DIALClient(String& ssid, String& password, String& YTUrl);
    void connectWiFi();
    void Execute();
    String getDialApplicationUrl(const String& locationUrl);
    std::vector<Device> discoverDevices();
    void exploreNetwork();
    bool parseSSDPResponse(const String& response, Device& device);
    bool convertMacStringToBytes(const String& macString, uint8_t* macBytes);
    String extractApplicationURL(HttpClient &httpc);
    int checkYouTubeAppStatus(const String& appUrl, Device& device);
    void extractIPAndPort(const String& appUrl, IPAddress& ip, uint16_t& port);
    void launchYouTubeApp(const String& appUrl);
    void sendYouTubeCommand(const String& command, const String& videoId, const String& loungeToken);
    String extractScreenId(const String& xmlData);
    bool fetchScreenIdWithRetries(const String& applicationUrl, Device& device);
    String getYouTubeToken(const String& screenId);
};

// Define a global variable to hold the current app info being parsed
DIALClient::AppInfo currentAppInfo;

#endif