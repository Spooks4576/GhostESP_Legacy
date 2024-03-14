#pragma once
#include <Arduino.h>
#include "esp_wifi_types.h"
#include "esp_wifi.h"
#include "WiFi.h"

struct WiFiInfo {
  String ssid;
  int32_t rssi;
  String bssid;
  int channel;
  bool secure;
  bool wpsEnabled;
};

typedef struct {
  unsigned frame_ctrl:16;
  unsigned duration_id:16;
  uint8_t addr1[6];
  uint8_t addr2[6];
  uint8_t addr3[6];
  unsigned sequence_ctrl:16;
  uint8_t addr4[6];
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0];
} wifi_ieee80211_packet_t;

class WiFiScanner {
public:
  WiFiInfo* networks;
  int numberOfNetworks;
  int maxNetworks;
  bool InitilizedForWPS;

public:
  WiFiScanner(int maxNetworks = 10) : maxNetworks(maxNetworks) {
    networks = new WiFiInfo[maxNetworks];
    numberOfNetworks = 0;
  }

  ~WiFiScanner() {
    delete[] networks;
  }

  void scanNetworks();

  WiFiInfo FindFirstWPSEnabledNetwork();

  void printNetworks();

  WiFiInfo FindNetworkFromSSID(String SSID);
};

WiFiScanner scanner;

void convertStringToBssid(const char* string, uint8_t* bssid) {
    for (int i = 0; i < 6; i++) {
        bssid[i] = (uint8_t)strtol(string + i * 3, NULL, 16);
    }
}


void wps_packet_handler(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT) return;
    
    const wifi_promiscuous_pkt_t *pk = (const wifi_promiscuous_pkt_t *)buf;
    const wifi_ieee80211_packet_t *ipkt = (const wifi_ieee80211_packet_t *)pk->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    
    if (hdr->frame_ctrl != 0x8000) return;

    
    int rssi = pk->rx_ctrl.rssi;
    char bssid[18];
    snprintf(bssid, sizeof(bssid), "%02X:%02X:%02X:%02X:%02X:%02X",
             hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
             hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);


    int pos = 36;
    String ssid = "";
    bool wpsEnabled = false;
    int total_length = pk->rx_ctrl.sig_len;
    const int HEADER_LENGTH = 24;
    int payload_length = total_length - HEADER_LENGTH;
    while (pos + 2 < payload_length) {
        int tag_type = pk->payload[pos];
        int tag_length = pk->payload[pos + 1];
        
        if (tag_type == 0xDD) {
            // Check for WPS OUI (00:50:F2:04)
            if (tag_length >= 4 && pk->payload[pos + 2] == 0x00 && pk->payload[pos + 3] == 0x50 &&
                pk->payload[pos + 4] == 0xF2 && pk->payload[pos + 5] == 0x04) {
                wpsEnabled = true;
            }
        } else if (tag_type == 0) {
            ssid = String((char *)(pk->payload + pos + 2), tag_length);
        }
        
        pos += 2 + tag_length;
    }


    for (int i = 0; i < 10; i++) {
        if (scanner.networks[i].bssid == String(bssid)) {
            scanner.networks[i].rssi = rssi;
            scanner.networks[i].wpsEnabled = wpsEnabled;
            scanner.networks[i].ssid = ssid;
            break;
        }
    }
}