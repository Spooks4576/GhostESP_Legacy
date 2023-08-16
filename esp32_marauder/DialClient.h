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

struct MACAddress {
    uint8_t bytes[6];
};

struct MagicPacket {
    uint8_t header[6];
    MACAddress payload[16];
};

const int retryCount = 5;
const int delayBetweenRetries = 1000; // milliseconds

class RID {
private:
    uint32_t number;
    
public:
    RID() {
        reset();
    }

    void reset() {
        number = random(10000, 99999);
    }

    uint32_t next() {
        return ++number;
    }
};


class WakeOnLan {
public:
    WiFiUDP udp;

    MagicPacket makeMagicPacket(const uint8_t addr[6]) {
    MagicPacket magicPacket;


    for (int i = 0; i < 6; i++) {
        magicPacket.header[i] = 0xFF;
    }


    for (int i = 0; i < 16; i++) {
        memcpy(magicPacket.payload[i].bytes, addr, 6);
    }

    return magicPacket;
  }

  uint8_t* marshalMagicPacket(const MagicPacket& packet) {

    uint8_t* serialized = new uint8_t[sizeof(MagicPacket)];

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
    int timeoutInMilliseconds = timeoutInSeconds * 1000;

    for(int attempt = 0; attempt < retries; attempt++) {
        udp.beginMulticast(broadcastIP, port);
        udp.beginPacket(broadcastIP, port);
        udp.write(serializedPacket, sizeof(MagicPacket));
        udp.endPacket();

        delay(timeoutInMilliseconds);
    }

    delete[] serializedPacket;
  }
};

class DIALClient {
public:
    String ssid;
    String password;
    String App;
    WiFiClient client;
    WiFiClientSecure secureClient;
    WiFiUDP multicastClient;
    WakeOnLan wol;
    RID rid;
public:

    struct Device {
        String uniqueServiceName;
        String location;
        String applicationUrl;
        String friendlyName;
        String wakeup;
        String screenID;
        String YoutubeToken;
    };

    uint32_t getNextRID() {
        return rid.next();
    }

    DIALClient(String& ssid, String& password, String& App);
    ~DIALClient();
    void connectWiFi();
    void Execute();
    String getDialApplicationUrl(const String& locationUrl);
    std::vector<Device> discoverDevices();
    void exploreNetwork();
    bool parseSSDPResponse(const String& response, Device& device);
    bool convertMacStringToBytes(const String& macString, uint8_t* macBytes);
    String extractApplicationURL(HttpClient &httpc);
    void extractIPAndPort(const String& appUrl, IPAddress& ip, uint16_t& port);

    // Netflix
    void launchNetflixApp(const String& appUrl);

    // Youtube
    int checkYouTubeAppStatus(const String& appUrl, Device& device);
    String getYouTubeToken(const String& screenId);
    void sendYouTubeCommand(const String& command, const String& videoId, const String& loungeToken);
    String extractScreenId(const String& xmlData);
    bool fetchScreenIdWithRetries(const String& applicationUrl, Device& device);
    void launchYouTubeApp(const String& appUrl);
};

#endif