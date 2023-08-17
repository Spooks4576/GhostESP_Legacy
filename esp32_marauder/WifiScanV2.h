#ifndef WiFiScanV2_h
#define WiFiScanV2_h




// Includes 

#include <ArduinoJson.h>
#ifdef HAS_BT
  #include <NimBLEDevice.h>
#endif
#include <WiFi.h>
#include <vector>
#include <math.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"


// If Includes

#ifdef HAS_BT
  #include "esp_bt.h"
#endif
#ifdef HAS_SCREEN
  #include "Display.h"
#endif
#ifdef HAS_SD
  #include "SDInterface.h"
#endif
#include "Buffer.h"
#ifdef HAS_BATTERY
  #include "BatteryInterface.h"
#endif
#ifdef HAS_TEMP_SENSOR
  #include "TemperatureInterface.h"
#endif
#include "settings.h"
#include "Assets.h"
#ifdef MARAUDER_FLIPPER
  #include "flipperLED.h"
#elif defined(XIAO_ESP32_S3)
  #include "xiaoLED.h"
#else
  #include "LedInterface.h"
#endif

// Constants or Global Defines.

#define MAX_CHANNEL 14


// Idk why this wasent a enum in the first place but anyway

enum EScanMode : uint8_t
{
  OTA_UPDATE = 100,
  SHOW_INFO = 101,
  ESP_UPDATE = 102,
  WIFI_SCAN_OFF = 0,
  WIFI_SCAN_PROBE = 1,
  WIFI_SCAN_AP = 2,
  WIFI_SCAN_PWN = 3,
  WIFI_SCAN_EAPOL = 4,
  WIFI_SCAN_DEAUTH = 5,
  WIFI_SCAN_ALL = 6,
  WIFI_PACKET_MONITOR = 7,
  WIFI_ATTACK_BEACON_SPAM = 8,
  WIFI_ATTACK_RICK_ROLL = 9,
  BT_SCAN_ALL = 10,
  BT_SCAN_SKIMMERS = 11,
  WIFI_SCAN_ESPRESSIF = 12,
  LV_JOIN_WIFI = 13,
  LV_ADD_SSID = 14,
  WIFI_ATTACK_BEACON_LIST = 15,
  WIFI_SCAN_TARGET_AP = 16,
  LV_SELECT_AP = 17,
  WIFI_ATTACK_AUTH = 18,
  WIFI_ATTACK_MIMIC = 19,
  WIFI_ATTACK_DEAUTH = 20,
  WIFI_ATTACK_AP_SPAM = 21,
  WIFI_SCAN_TARGET_AP_FULL = 22,
  WIFI_SCAN_ACTIVE_EAPOL = 23,
  WIFI_ATTACK_DEAUTH_MANUAL = 24,
  WIFI_SCAN_RAW_CAPTURE = 25,
  WIFI_SCAN_STATION = 26,
  WIFI_ATTACK_DEAUTH_TARGETED = 27,
  WIFI_SCAN_ACTIVE_LIST_EAPOL = 28,
  WIFI_SCAN_SIG_STREN = 29,
  WIFI_SCAN_EVIL_PORTAL = 30,
}


// Structs

struct Station {
  uint8_t mac[6];
  bool selected;
};

struct NetworkInfo {
  String SSID;
  int32_t RSSI;
  int8_t channel;
};


struct Packets
{
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


  uint8_t auth_packet[65] = {0xb0, 0x00, 0x3c, 0x00, 
                              0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                              0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                              0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                              0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
                              0x7f, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 
                              0x00, 0x40, 0xdd, 0x0b, 0x00, 0x17, 0xf2, 0x0a, 
                              0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0xdd, 
                              0x0a, 0x00, 0x10, 0x18, 0x02, 0x00, 0x00, 0x10, 
                              0x00, 0x00, 0x00};


  uint8_t prob_req_packet[128] = {0x40, 0x00, 0x00, 0x00, 
                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination
                                  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Dest
                                  0x01, 0x00, // Sequence
                                  0x00, // SSID Parameter
                                  0x00, // SSID Length
                                  /* SSID */
                                  };


  uint8_t deauth_frame_default[26] = {
                              0xc0, 0x00, 0x3a, 0x01,
                              0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0xf0, 0xff, 0x02, 0x00
                          };
}


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



struct LyricList
{
  char* rick_roll[8] = {
      "01 Never gonna give you up",
      "02 Never gonna let you down",
      "03 Never gonna run around",
      "04 and desert you",
      "05 Never gonna make you cry",
      "06 Never gonna say goodbye",
      "07 Never gonna tell a lie",
      "08 and hurt you"
    };
}



// Constants


const wifi_promiscuous_filter_t filt = {.filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};

LinkedList<ssid>* ssids;
LinkedList<AccessPoint>* access_points;
LinkedList<Station>* stations;

class WifiScanV2
{
    // Settings
    int channel_hop_delay = 1;
    bool force_pmkid = false;
    bool force_probe = false;
    bool save_pcap = false;
    bool do_break = false;
    bool wsl_bypass_enabled = false;


    struct LyricList;
    struct Packets;


    void initWiFi(EScanMode scan_mode);


}

#endif
