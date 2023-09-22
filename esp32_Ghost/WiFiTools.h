#pragma once
#include <esp_wifi.h>
#include <Arduino.h>
#include "Buffer.h"

const char* rick_roll[8] = {
  "Never gonna give you up",
  "Never gonna let you down",
  "Never gonna run around",
  "and desert you",
  "Never gonna make you cry",
  "Never gonna say goodbye",
  "Never gonna tell a lie",
  "and hurt you"
};

uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00,                                 //Frame Control, Duration
                        /*4*/ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,               //Destination address
                        /*10*/ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,              //Source address - overwritten later
                        /*16*/ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,              //BSSID - overwritten to the same as the source address
                        /*22*/ 0xc0, 0x6c,                                      //Seq-ctl
                        /*24*/ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00,  //timestamp - the number of microseconds the AP has been active
                        /*32*/ 0x64, 0x00,                                      //Beacon interval
                        /*34*/ 0x01, 0x04,                                      //Capability info
                                                                                /* SSID */
                        /*36*/ 0x00 };

typedef struct
{
  int16_t fctl;
  int16_t duration;
  uint8_t da;
  uint8_t sa;
  uint8_t bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) WifiMgmtHdr;

typedef struct {
  uint8_t payload[0];
  WifiMgmtHdr hdr;
} wifi_ieee80211_packet_t;


Buffer buffer_obj;

class WiFiTools {

public:

  static void rawSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t* snifferPacket = (wifi_promiscuous_pkt_t*)buf;
    WifiMgmtHdr* frameControl = (WifiMgmtHdr*)snifferPacket->payload;
    wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
    uint32_t len = snifferPacket->rx_ctrl.sig_len;

    int fctl = ntohs(frameControl->fctl);
    int frameType = (fctl & 0x0C) >> 2;     // Extracting the type field
    int frameSubType = (fctl & 0xF0) >> 4;  // Extracting the subtype field

    
    if (frameType == 2 && frameSubType == 8) {
      const wifi_ieee80211_packet_t* ipkt = (wifi_ieee80211_packet_t*)snifferPacket->payload;
      const WifiMgmtHdr* hdr = &ipkt->hdr;

      Serial.println("4 Way Handshake Found");
      Serial.print("fctl: ");
      Serial.println(fctl);
      Serial.print("duration: ");
      Serial.println(hdr->duration);
      Serial.print("da: ");
      Serial.println(hdr->da, HEX);
      Serial.print("sa: ");
      Serial.println(hdr->sa, HEX);
      Serial.print("bssid: ");
      Serial.println(hdr->bssid, HEX);
      Serial.print("seqctl: ");
      Serial.println(hdr->seqctl);
      Serial.println();
    }

    buffer_obj.addPacket((uint8_t*)snifferPacket, len);
  }

  void RunRawScan(int Channel) {
    buffer_obj.InitSD("RawScan");

    const wifi_promiscuous_filter_t filt = { .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA };

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_filter(&filt);
    esp_wifi_set_promiscuous_rx_cb(&rawSnifferCallback);
    esp_wifi_set_channel(Channel, WIFI_SECOND_CHAN_NONE);
  }

  void broadcastSetSSID(const char* ESSID) {
    int set_channel = random(1, 12);
    esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
    delay(1);

    for (int i = 10; i < 22; i++) {
      packet[i] = random(256);
    }
    memcpy(&packet[16], &packet[10], 6);

    int ssidLen = strlen(ESSID);

    packet[37] = ssidLen;

    // Insert SSID
    memcpy(&packet[38], ESSID, ssidLen);

    uint8_t postSSID[13] = {
      0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,
      0x03, 0x01, set_channel
    };

    memcpy(&packet[38 + ssidLen], postSSID, 13);

    int totalPacketLen = 38 + ssidLen + 13;


    for (int i = 0; i < 3; i++) {
      esp_wifi_80211_tx(WIFI_IF_AP, packet, totalPacketLen, false);
    }
  }
};