#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

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

uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, //Frame Control, Duration
                    /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address 
                    /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
                    /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
                    /*22*/  0xc0, 0x6c, //Seq-ctl
                    /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
                    /*32*/  0x64, 0x00, //Beacon interval
                    /*34*/  0x01, 0x04, //Capability info
                    /* SSID */
                    /*36*/  0x00
};    

class WiFiTools {

public:

  void broadcastSetSSID(const char* ESSID) {
    int set_channel = random(1, 12);
    esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
    delay(1);

    for (int i = 10; i < 22; i++) {
      packet[i] = random(256);
    }
    memcpy(&packet[16], &packet[10], 6);  // BSSID is the same as the source address

    int ssidLen = strlen(ESSID);
    
    packet[37] = ssidLen;  // Set the SSID length

    // Insert SSID
    memcpy(&packet[38], ESSID, ssidLen);

    uint8_t postSSID[13] = { 
      0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,  //supported rates
      0x03, 0x01, set_channel  // DSSS (Current Channel)
    };
    
    memcpy(&packet[38 + ssidLen], postSSID, 13);  // Copy postSSID after the SSID

    int totalPacketLen = 38 + ssidLen + 13;  // Total length of the packet

    
    for (int i = 0; i < 3; i++) {
      esp_wifi_80211_tx(WIFI_IF_AP, packet, totalPacketLen, false);
    }
  }
};