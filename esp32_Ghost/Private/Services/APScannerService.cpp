#include "../../Public/Services/APScannerService.h"


void WiFiScanner::scanNetworks()
{
    Serial.println("Scanning WiFi networks...");
    int n = WiFi.scanNetworks();
    Serial.println("Scan done.");
    if (n == 0) {
      Serial.println("No networks found.");
    } else {
      Serial.print(n);
      Serial.println(" networks found:");
      for (int i = 0; i < n; ++i) {
        
        if (i < maxNetworks) {
          networks[i].ssid = WiFi.SSID(i);
          networks[i].rssi = WiFi.RSSI(i);
          networks[i].bssid = WiFi.BSSIDstr(i);
          networks[i].channel = WiFi.channel(i);
          networks[i].secure = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
        }
        
        Serial.println("SSID: " + WiFi.SSID(i) + ", RSSI: " + WiFi.RSSI(i));
      }
      numberOfNetworks = min(n, maxNetworks);
    }
}

void WiFiScanner::printNetworks()
{
    for (int i = 0; i < numberOfNetworks; ++i) {
        Serial.println("Network " + String(i) + ": " + networks[i].ssid + ", " + networks[i].rssi + " dBm, Channel: " + networks[i].channel + ", Secure: " + (networks[i].secure ? "Yes" : "No"));
    }
}

WiFiInfo WiFiScanner::FindFirstWPSEnabledNetwork()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(wps_packet_handler);

    delay(10000);

    esp_wifi_set_promiscuous(false);

    WiFiInfo network;

    for (int i = 0; i < numberOfNetworks; ++i) {
        if (networks[i].wpsEnabled)
        {
            network = networks[i];
            break;
        }
    }

    if (network.ssid.isEmpty())
    {
        Serial.println("No WPS Networks Found Please Try Again...");
        return WiFiInfo();
    }

    return network;
}


WiFiInfo WiFiScanner::FindNetworkFromSSID(String SSID)
{
    WiFiInfo targetinfo;

    for (int i = 0; i < maxNetworks; ++i) {
      if (networks[i].ssid == SSID)
      {
        targetinfo = networks[i];
        break;
      }
    }

    return targetinfo;
}

