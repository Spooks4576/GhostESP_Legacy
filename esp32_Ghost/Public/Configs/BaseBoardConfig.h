#ifndef BASE_BOARD_CONFIG_H
#define BASE_BOARD_CONFIG_H

#include <Arduino.h>
#include <HTTPUpdate.h>
#include <Adafruit_NeoPixel.h>
#include <HTTPClient.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <esp_mac.h>
#include <wifi.h>
#include "../../Public/Services/APScannerService.h"
#include "../../Public/Controllers/YoutubeController.h"
#include "../../Public/Controllers/NetflixController.h"
#include "../../Public/Controllers/RokuController.h"
#include "../../Public/Features/Dial.h"

#define DELIMITER "-+"
const int MAX_CHANNELS = 12;

const wifi_promiscuous_filter_t filt = {.filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};

struct Wifianalyticsdata {
    unsigned long deauthCount[MAX_CHANNELS];
    unsigned long authCount[MAX_CHANNELS];
    unsigned long beaconCount[MAX_CHANNELS];
    unsigned long probeReqCount[MAX_CHANNELS];
    unsigned long probeRespCount[MAX_CHANNELS];
    unsigned long otherPktCount[MAX_CHANNELS];
    unsigned long startTime[MAX_CHANNELS];
    unsigned long endTime[MAX_CHANNELS];         
    String deviceMac;
};

#ifdef USE_BLUETOOTH
#include <NimBLEDevice.h>

// Define the struct for the buds models
static const struct {
    uint32_t value;
    const char* name;
} buds_models[] = {
    {0xEE7A0C, "Fallback Buds"},
    {0x9D1700, "Fallback Dots"},
    {0x39EA48, "Light Purple Buds2"},
    {0xA7C62C, "Bluish Silver Buds2"},
    {0x850116, "Black Buds Live"},
    {0x3D8F41, "Gray & Black Buds2"},
    {0x3B6D02, "Bluish Chrome Buds2"},
    {0xAE063C, "Gray Beige Buds2"},
    {0xB8B905, "Pure White Buds"},
    {0xEAAA17, "Pure White Buds2"},
    {0xD30704, "Black Buds"},
    {0x9DB006, "French Flag Buds"},
    {0x101F1A, "Dark Purple Buds Live"},
    {0x859608, "Dark Blue Buds"},
    {0x8E4503, "Pink Buds"},
    {0x2C6740, "White & Black Buds2"},
    {0x3F6718, "Bronze Buds Live"},
    {0x42C519, "Red Buds Live"},
    {0xAE073A, "Black & White Buds2"},
    {0x011716, "Sleek Black Buds2"},
};

#define NUM_MODELS (sizeof(buds_models) / sizeof(buds_models[0]))

void fill_model_bytes(uint8_t *array) {
    int randomIndex = rand() % NUM_MODELS;
    uint32_t value = buds_models[randomIndex].value;
    array[17] = (value >> 24) & 0xFF; // 17th byte
    array[18] = (value >> 16) & 0xFF;  // 18th byte
    array[20] = (value >> 8) & 0xFF; // 20th byte, note the change in order due to byte significance
}

struct SamsungTestBLEData {
  NimBLEAdvertisementData advertisementData;
  NimBLEAdvertisementData scanResponse;
};

SamsungTestBLEData GetSamsungTestBLE() {
  NimBLEAdvertisementData advertisementData = NimBLEAdvertisementData();
  NimBLEAdvertisementData scanResponse = NimBLEAdvertisementData();

  // Split your specific packet data into advertisement and scan response data
  uint8_t advertisementPacket[] = {
    0x02, 0x01, 0x18, 0x1B, 0xFF, 0x75, 0x00, 0x42, 0x09, 0x81, 0x02, 0x14,
    0x15, 0x03, 0x21, 0x01, 0x09, 0xEF, 0x0C, 0x01, 0x47, 0x06, 0x3C, 0x94, 0x8E,
    0x00, 0x00, 0x00, 0x00, 0xC7, 0x00
  };
  uint8_t scanResponsePacket[] = {
    0x10, 0xFF, 0x75, 0x00, 0x00, 0x63, 0x50, 0x8D, 0xB1, 0x17, 0x40, 0x46,
    0x64, 0x64, 0x00, 0x01, 0x04
  };

  fill_model_bytes(advertisementPacket);

  // Add the packet data to the advertisement data and scan response
  advertisementData.addData(std::string((char *)advertisementPacket, sizeof(advertisementPacket)));
  scanResponse.addData(std::string((char *)scanResponsePacket, sizeof(scanResponsePacket)));

  // Return both advertisement data and scan response
  return { advertisementData, scanResponse };
}

#endif

void retainLineExcludingKeywords(String &content, const String &keyword1, const String &keyword2) {
    int index1 = content.indexOf(keyword1);
    int index2 = content.indexOf(keyword2);
    
    // If neither keyword is found, return the content as-is
    if (index1 == -1 && index2 == -1) {
        return;
    }

    // Split the content into lines
    int startPos = 0;
    int endPos = content.indexOf('\n');
    while (endPos != -1) {
        String line = content.substring(startPos, endPos);
        if (line.indexOf(keyword1) == -1 && line.indexOf(keyword2) == -1 && line.length() > 0) { // Check for non-empty line
            content = line;
            return;
        }
        startPos = endPos + 1;
        endPos = content.indexOf('\n', startPos);
    }

    // Check the last line (if there's no newline at the end)
    if (startPos < content.length()) {
        String line = content.substring(startPos);
        if (line.indexOf(keyword1) == -1 && line.indexOf(keyword2) == -1 && line.length() > 0) { // Check for non-empty line
            content = line;
            return;
        }
    }

    content = "";  // If no suitable line is found, clear the content
}

// Forward Declarations

struct BaseBoardConfig;

namespace Functions
{
    void InitDialConnect(BaseBoardConfig* Config, String flipperMessage, HandlerType Type);

#ifdef USE_BLUETOOTH
    void InitBLEBuds(BaseBoardConfig* Config, String flipperMessage);
#endif

    void InitUpdate(BaseBoardConfig* Config, String flipperMessage);

    void executeCommand(BaseBoardConfig* Config, const String &commandLine);

    void InitDeauthDetector(BaseBoardConfig* Config, String Channel, String SSID, String Password, String WebHookUrl);

    void MeasureDistance(BaseBoardConfig* Config, String SSID, String RSS1Val);

}

typedef void (*CommandHandler)(BaseBoardConfig* Config, std::vector<String> params);

struct Command {
    String name;
    CommandHandler handler;
};

namespace Scripting
{
    void handleLedOn(BaseBoardConfig* Config, std::vector<String> params);
    void handleLedOff(BaseBoardConfig* Config, std::vector<String> params);
    void handleSetColor(BaseBoardConfig* Config, std::vector<String> params);
    void handlePrint(BaseBoardConfig* Config, std::vector<String> params);
    void handleSleep(BaseBoardConfig* Config, std::vector<String> params);
    void handleUpdate(BaseBoardConfig* Config, std::vector<String> params);
    void handleGalaxyBudSpam(BaseBoardConfig* Config, std::vector<String> params);
    void handleDialConnect(BaseBoardConfig* Config, std::vector<String> params);
    void handleDeauthDetector(BaseBoardConfig* Config, std::vector<String> params);
    void handleMeasureDistance(BaseBoardConfig* Config, std::vector<String> params);

    Command commands[] = {
    {"LED_ON", handleLedOn},
    {"PRINT", handlePrint},
    {"SLEEP", handleSleep},
    {"LED_OFF", handleLedOff},
    {"SET_LED_COLOR", handleSetColor},
    {"UPDATE", handleUpdate},
    {"GALAXY_BUD_SPAM", handleGalaxyBudSpam},
    {"DIAL_CONNECT", handleDialConnect},
    {"DETECT_DEAUTH", handleDeauthDetector},
    {"DETECT_WIFI_DISTANCE", handleMeasureDistance}
    };
}



struct BaseBoardConfig {
    bool RunningCommand;
    int ledPin_B = -1;
    int ledPin_G = -1;
    int ledPin_R = -1;
    bool LedOn = false;
    int bluetoothTxPin = -1;
    int bluetoothRxPin = -1;
    int neopixelPin = -1;
    int pixel_count = -1;
    int sdpin = -1;
    bool SupportsBluetooth = false;
    bool SupportsNeoPixel = false;
    std::vector<String> commandBuffer;
    bool isScriptMode = false;
#ifdef USE_BLUETOOTH
    NimBLEAdvertising* pAdvertising;
#endif
    String UpdateURL = "";
    Adafruit_NeoPixel strip;

    BaseBoardConfig() {

        if (SupportsNeoPixel && neopixelPin != -1 && pixel_count != -1)
        {
            Adafruit_NeoPixel instrip(pixel_count, neopixelPin, NEO_GRB + NEO_KHZ800);
            strip = instrip;
        }
    }

    virtual void init() {

        if (ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            pinMode(ledPin_B, OUTPUT);
            pinMode(ledPin_G, OUTPUT);
            pinMode(ledPin_R, OUTPUT);
        }

        if (sdpin != -1)
        {
            pinMode(sdpin, OUTPUT);
        }
    }

    virtual void blinkLed() {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, LOW);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, LOW);
            digitalWrite(ledPin_R, HIGH);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, LOW);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, HIGH);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                // Run the same color setup pattern for neopixel
                strip.setPixelColor(0, strip.Color(255, 0, 0));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 255, 0));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 0, 255));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
            }
        }
    }

    virtual void TurnoffLed()
    {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, HIGH);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                // Turn off the neopixel LED
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
            }
        }

        LedOn = false;
    }

    virtual void setLedColor(int r, int g, int b) {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            digitalWrite(ledPin_R, r != 0 ? LOW : HIGH);
            digitalWrite(ledPin_G, g != 0 ? LOW : HIGH);
            digitalWrite(ledPin_B, b != 0 ? LOW : HIGH);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                strip.setPixelColor(0, strip.Color(r, g, b));
                strip.show();
            }
        }

        LedOn = true;
    }

    String readSerialBuffer(bool &isHtml, bool &isAp) {
        String buffer = "";
        buffer = Serial.readStringUntil('\r\n');

        isHtml = buffer.startsWith("sethtml=");
        isAp = buffer.startsWith("setap=");

        return buffer;
    }



    virtual void Loop()
    {
        if (Serial.available() > 0 && !RunningCommand) {
            String flipperMessage;
            bool StartsWithHTML;
            bool StartsWithAP;
            flipperMessage = readSerialBuffer(StartsWithHTML, StartsWithAP);

            if (flipperMessage.startsWith("settings"))
            {
                retainLineExcludingKeywords(flipperMessage, "settings -s EnableLED enable", "settings -s SavePCAP enable");
            } // Hack for Marauder app. until i can produce my own flipper app

            // Dial Commands

            if (flipperMessage.startsWith("YTDialConnect")) {
                Functions::InitDialConnect(this, flipperMessage, HandlerType::YoutubeController);
                RunningCommand = true;
                return;
            }

            if (flipperMessage.startsWith("RokuConnect")) {
                Functions::InitDialConnect(this, flipperMessage, HandlerType::RokuController);
                RunningCommand = true;
                return;
            }
            
            if (flipperMessage.startsWith("LaunchNetflix")) {
                Functions::InitDialConnect(this, flipperMessage, HandlerType::NetflixController);
                RunningCommand = true;
                return;
            }

            if (flipperMessage.startsWith("Update"))
            {   
                Functions::InitUpdate(this, flipperMessage);
                RunningCommand = true;
                return;
            }


            if (flipperMessage.startsWith("<GHOST_SCRIPT_BEGIN>"))
            {
                isScriptMode = true;
                commandBuffer.clear();
            }
            else if (flipperMessage.startsWith("<GHOST_SCRIPT_END>")) {
                isScriptMode = false;
                RunningCommand = true;
                
                for (String &cmd : commandBuffer) {
                    Functions::executeCommand(this, cmd);
                }
                commandBuffer.clear();
                
            } else if (isScriptMode) {
                commandBuffer.push_back(flipperMessage);
            }


#ifdef USE_BLUETOOTH
            if (flipperMessage.startsWith("BLE_SamsungBuds") && SupportsBluetooth)
            {
                Functions::InitBLEBuds(this, flipperMessage);
            }
#endif
        }
        else 
        {

        }
    }
};

namespace Functions
{
    void InitDialConnect(BaseBoardConfig* Config, String flipperMessage, HandlerType Type)
    {
        Serial.println("Debug: " + flipperMessage); // Debugging line to check the message

        int firstSpace = flipperMessage.indexOf(DELIMITER);
        int secondSpace = flipperMessage.indexOf(DELIMITER, firstSpace + 2);

        // Debugging lines to check the positions of spaces
        Serial.println("First space at: " + String(firstSpace));
        Serial.println("Second space at: " + String(secondSpace));

        if (firstSpace != -1 && secondSpace != -1) {
            String YTURL = flipperMessage.substring(0, firstSpace);
            String SSID = flipperMessage.substring(firstSpace + 2, secondSpace);
            String Password = flipperMessage.substring(secondSpace + 2);


            Serial.println("YTURL: " + YTURL);
            Serial.println("SSID: " + SSID);
            Serial.println("Password: " + Password);

            YTURL.trim();
            SSID.trim();
            Password.trim();

            AppController* handler = nullptr;

            switch (Type)
            {
                case HandlerType::YoutubeController:
                {
                    YoutubeController* YtController = new YoutubeController();
                    handler = YtController;
                }
                case HandlerType::NetflixController:
                {
                    NetflixController* NfController = new NetflixController();
                    handler = NfController;
                }
                case HandlerType::RokuController:
                {
                    RokuController* RKController = new RokuController();
                    handler = RKController;
                }
            }

            
            DIALClient* dial = new DIALClient(YTURL.c_str(), SSID.c_str(), Password.c_str(), handler);
            Config->setLedColor(0, 1, 0);  

            dial->Execute();
            delete dial;
            delete handler;

            Config->TurnoffLed();
        } else {
            Serial.println("Error: Incorrect message format for YT Dial connection.");
        }
    }

#ifdef USE_BLUETOOTH

    void InitBLEBuds(BaseBoardConfig* Config, String flipperMessage)
    {
        srand(time(NULL));

        while (true) {
            NimBLEDevice::init("");
            NimBLEServer* pServer = NimBLEDevice::createServer();
            Config->pAdvertising = pServer->getAdvertising();


            SamsungTestBLEData advertisementData = GetSamsungTestBLE();


            Config->pAdvertising->setAdvertisementData(advertisementData.advertisementData);
            Config->pAdvertising->setScanResponseData(advertisementData.scanResponse);


            Config->pAdvertising->start();
            Serial.println("Sending Packet");
            delay(100);
            NimBLEDevice::deinit();

            uint8_t mac[6];
            for (int i = 0; i < 6; i++) {
            mac[i] = random(0, 256);
            }
            mac[0] = (mac[0] & 0xFC) | 0x02;

            esp_base_mac_addr_set(mac);
        }
    }
#endif

    unsigned long deauthCount = 0;
    unsigned long lastCheckTime = 0;
    unsigned long lastChannelCheckTime = 0;
    unsigned long startTime = 0;
    const unsigned long checkInterval = 1000;
    const unsigned long ChannelHopInterval = 5000;
    const unsigned long scanDuration = 2 * 60000;
    const unsigned long deauthThreshold = 20;
    Wifianalyticsdata* analyticsdata;
    bool ConfigHasChannel = false;
    int channel = 1;
    WiFiInfo TargetInfo;
    uint8_t BSSIDTarget[6];
    int strongestRSSI = INT_MIN;
    double strongestDistance = 0;
    bool targetFound = false;
    const double n = 2.5;
    double RSSI_0 = -40;

    void promiscuous_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
        if (type != WIFI_PKT_MGMT) return;

        const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
        const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
        const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;


        switch(hdr->frame_ctrl & 0xFC) { // Mask to get type and subtype only
            case 0xB0: // Authentication frame
                analyticsdata->authCount[channel]++;
                Serial.println("Got Auth Frame");
                break;
            case 0xC0: // Deauthentication frame
                analyticsdata->deauthCount[channel]++;
                deauthCount++;
                Serial.println("Got Deauth Frame");
                break;
            case 0x00: // Association request frame
                analyticsdata->probeReqCount[channel]++;
                Serial.println("Got Association request Frame");
                break;
            case 0x30: // Reassociation response frame
                analyticsdata->probeRespCount[channel]++;
                Serial.println("Got Reassociation Response Frame");
                break;
            case 0x40: // Probe request frame
                analyticsdata->probeReqCount[channel]++;
                Serial.println("Got Probe request frame");
                break;
            case 0x50: // Probe response frame
                analyticsdata->probeRespCount[channel]++;
                  Serial.println("Got Probe response frame");
                break;
            case 0x80: // Beacon frame
                analyticsdata->beaconCount[channel]++;
                break;
            default: 
                analyticsdata->otherPktCount[channel]++;
                break;
        }
    }

    void sendWifiAnalitics(BaseBoardConfig* Config, String Channel, String SSID, String Password, String WebHookUrl) {

        Config->setLedColor(0x1, 0x0, 0x1);

        if (!WebHookUrl.isEmpty())
        {
            WiFi.begin(SSID, Password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(1000);
                Serial.println("Connecting to WiFi...");
            }

            HTTPClient http;
            http.addHeader("Content-Type", "application/json");
            
            
            String message;
            
            unsigned long duration = analyticsdata->endTime - analyticsdata->startTime; 

            
            for (int channelIndex = 1; channelIndex < MAX_CHANNELS; channelIndex++) {
                
                String message = "{\"content\": \"\", \"embeds\": [{";
                message += "\"title\": \"WiFi Analytics Summary - Channel " + String(channelIndex) + "\",";
                message += "\"color\": 3447003,";
                message += "\"fields\": [";
                message += "{\"name\":\"Monitored Channel\",\"value\":\"" + String(channelIndex) + "\",\"inline\":true},";
                message += "{\"name\":\"Monitoring Duration\",\"value\":\"" + String((analyticsdata->endTime[channelIndex] - analyticsdata->startTime[channelIndex]) / 60000) + " minutes (" + String((analyticsdata->endTime[channelIndex] - analyticsdata->startTime[channelIndex]) / 1000) + " seconds)\",\"inline\":true},";
                message += "{\"name\":\"Deauthentication Packets\",\"value\":\"" + String(analyticsdata->deauthCount[channelIndex]) + "\",\"inline\":false},";
                message += "{\"name\":\"Authentication Packets\",\"value\":\"" + String(analyticsdata->authCount[channelIndex]) + "\",\"inline\":false},";
                message += "{\"name\":\"Beacon Packets\",\"value\":\"" + String(analyticsdata->beaconCount[channelIndex]) + "\",\"inline\":false},";
                message += "{\"name\":\"Probe Request Packets\",\"value\":\"" + String(analyticsdata->probeReqCount[channelIndex]) + "\",\"inline\":false},";
                message += "{\"name\":\"Probe Response Packets\",\"value\":\"" + String(analyticsdata->probeRespCount[channelIndex]) + "\",\"inline\":false},";
                message += "{\"name\":\"Other Packet Types\",\"value\":\"" + String(analyticsdata->otherPktCount[channelIndex]) + "\",\"inline\":false}";
                message += "]}]}";

                
                http.begin(WebHookUrl);
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(message);
                if (httpResponseCode > 0) {
                    Serial.print("Data for channel ");
                    Serial.print(channelIndex);
                    Serial.println(" sent successfully");
                } else {
                    Serial.print("Error sending data for channel ");
                    Serial.println(channelIndex);
                }
                http.end();

                
                delay(1000);
            }
            WiFi.disconnect();

        }


        Serial.println("WiFi Analytics Summary:");
        Serial.println("------------------------------");

        for (int channelIndex = 1; channelIndex < MAX_CHANNELS; channelIndex++) {
            Serial.print("Monitored Channel: ");
            Serial.println(channelIndex);
            Serial.print("Monitoring Duration: ");
            
            Serial.print((analyticsdata->endTime[channelIndex] - analyticsdata->startTime[channelIndex]) / 60000);
            Serial.print(" minutes (");
            Serial.print((analyticsdata->endTime[channelIndex] - analyticsdata->startTime[channelIndex]) / 1000);
            Serial.println(" seconds)");
            Serial.print("Deauthentication Packets: ");
            Serial.println(analyticsdata->deauthCount[channelIndex]);
            Serial.print("Authentication Packets: ");
            Serial.println(analyticsdata->authCount[channelIndex]);
            Serial.print("Beacon Packets: ");
            Serial.println(analyticsdata->beaconCount[channelIndex]);
            Serial.print("Probe Request Packets: ");
            Serial.println(analyticsdata->probeReqCount[channelIndex]);
            Serial.print("Probe Response Packets: ");
            Serial.println(analyticsdata->probeRespCount[channelIndex]);
            Serial.print("Other Packet Types: ");
            Serial.println(analyticsdata->otherPktCount[channelIndex]);
            Serial.println("------------------------------");
        }
        
        Config->setLedColor(0x0, 0x1, 0x0);

        delete analyticsdata;

        Serial.println("Command Finished");


        if (!ConfigHasChannel)
        {
            InitDeauthDetector(Config, Channel, SSID, Password, WebHookUrl);
        }
    }

    void DistanceCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
        wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
        wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
        wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
        int rssi = ppkt->rx_ctrl.rssi;

        if (memcmp(hdr->addr2, BSSIDTarget, 6) == 0) {
           strongestRSSI = rssi;
           strongestDistance = pow(10.0, (RSSI_0 - rssi) / (10 * n));
        } else {
            if (rssi > strongestRSSI) {
                strongestRSSI = rssi;
                strongestDistance = pow(10.0, (RSSI_0 - rssi) / (10 * n));
            }
        }
    }

    void reportAndReset(BaseBoardConfig* Config) {
        if (strongestRSSI != INT_MIN) {
            Serial.print("Strongest non-target RSSI: ");
            Serial.print(strongestRSSI);
            Serial.print(" dBm, Estimated Distance: ");
            Serial.print(strongestDistance);
            Serial.println(" meters");
        }

        if (strongestDistance < 2) {
            Config->setLedColor(false, true, false);
        } else if (strongestDistance >= 2 && strongestDistance <= 5) {
            Config->setLedColor(true, true, false);
        } else {
            Config->setLedColor(true, false, false);
        }
        
        targetFound = false;
        strongestRSSI = INT_MIN;
        strongestDistance = 0;
    }

    void MeasureDistance(BaseBoardConfig* Config, String SSID, String RSS1Val)
    {

        Serial.println("About to Measure Distance");

        if (!RSS1Val.startsWith("0"))
        {
            RSSI_0 = RSS1Val.toInt();
        }

        if (!SSID.startsWith("0"))
        {
            scanner.scanNetworks();

            TargetInfo = scanner.FindNetworkFromSSID(SSID);

            convertStringToBssid(TargetInfo.bssid.c_str(), BSSIDTarget);
            esp_wifi_set_channel(TargetInfo.channel, WIFI_SECOND_CHAN_NONE);
        }

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_filter(&filt);
        esp_wifi_set_promiscuous_rx_cb(DistanceCallback);
        


        while (true)
        {
            if (startTime == 0 || millis() - startTime >= 5000) {
                if (startTime != 0) {
                    reportAndReset(Config);
                }
                startTime = millis();

                if (SSID.startsWith("0"))
                {
                    channel = (channel % MAX_CHANNELS) + 1;
                    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
                    Serial.printf("Set Channel to %i", channel);
                }
            }
        }
        
    }

    void InitDeauthDetector(BaseBoardConfig* Config, String Channel, String SSID, String Password, String WebHookUrl) 
    {
        startTime = millis();

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        Channel.trim();

        analyticsdata = new Wifianalyticsdata();

        ConfigHasChannel = !Channel.isEmpty();

        if (ConfigHasChannel)
        {
            channel = Channel.toInt();
        }

        Serial.println("Channel");
        Serial.println(channel);

        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_filter(&filt);
        esp_wifi_set_promiscuous_rx_cb(promiscuous_cb);
        esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

        while (millis() - startTime < scanDuration) {
            if (millis() - lastCheckTime > checkInterval) {
                if (deauthCount > deauthThreshold) {
                    Serial.println("Deauth attack detected!");
                    Config->setLedColor(0x1, 0x0, 0x0);
                } else {
                    Serial.println("Normal network behavior.");
                    Config->setLedColor(0x0, 0x1, 0x0);
                }
                deauthCount = 0;
                lastCheckTime = millis();
            }

            if (millis() - lastChannelCheckTime > ChannelHopInterval && !ConfigHasChannel)
            {
                channel = (channel % MAX_CHANNELS) + 1;
                esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
                lastChannelCheckTime = millis();
            }
        }

        for (int channelIndex = 1; channelIndex < MAX_CHANNELS; channelIndex++) 
        {
            analyticsdata->endTime[channelIndex] = millis();
            analyticsdata->startTime[channelIndex] = startTime;
        }


        
        esp_wifi_set_promiscuous(false);
        sendWifiAnalitics(Config, Channel, SSID, Password, WebHookUrl);
    }

    void executeCommand(BaseBoardConfig* Config, const String &commandLine) {
        int openParenIndex = commandLine.indexOf('(');
        int closeParenIndex = commandLine.lastIndexOf(')');

        String commandName = commandLine.substring(0, openParenIndex);
        String paramString = commandLine.substring(openParenIndex + 1, closeParenIndex);

        commandName.trim();

        std::vector<String> params;
        bool inQuotes = false;
        String param = "";
        for (int i = 0; i < paramString.length(); i++) {
            char c = paramString[i];
            if (c == '\"') {
                inQuotes = !inQuotes; // Toggle state
            } else if (c == ',' && !inQuotes) {
                params.push_back(param);
                param = ""; // Reset for the next parameter
            } else {
                param += c; // Add character to the current parameter
            }
        }
        if (param.length() > 0) {
            params.push_back(param); // Add the last parameter
        }

        for (Command &cmd : Scripting::commands) {
            if (cmd.name == commandName) {
                cmd.handler(Config, params);
                return;
            }
        }
        Serial.println("Unknown command: " + commandName);
    }


    void InitUpdate(BaseBoardConfig* Config, String flipperMessage)
    {
        Serial.println("Debug: " + flipperMessage); 

        int firstSpace = flipperMessage.indexOf(DELIMITER);
        int secondSpace = flipperMessage.indexOf(DELIMITER, firstSpace + 2);

       
        Serial.println("First space at: " + String(firstSpace));
        Serial.println("Second space at: " + String(secondSpace));

        if (firstSpace != -1 && secondSpace != -1) {
            Config->setLedColor(0, 1, 0);
            String SSID = flipperMessage.substring(0, firstSpace);
            String Password = flipperMessage.substring(firstSpace + 2, secondSpace);

            Serial.println("SSID: " + SSID);
            Serial.println("Password: " + Password);

            SSID.trim();
            Password.trim();

            WiFi.begin(SSID, Password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(100);
                Serial.print(".");
            }
            Serial.println("\nConnected to Wi-Fi");
            Serial.println("\nSet Callback");

            WiFiClient client;

            t_httpUpdate_return ret = httpUpdate.update(client, Config->UpdateURL, "1.0.0", 0);

            switch(ret) {
                case HTTP_UPDATE_FAILED:
                    Config->setLedColor(0, 1, 1);
                    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
                    break;

                case HTTP_UPDATE_NO_UPDATES:
                    Config->setLedColor(0, 1, 1);
                    Serial.println("HTTP_UPDATE_NO_UPDATES");
                    break;

                case HTTP_UPDATE_OK:
                    Config->setLedColor(1, 0, 1);
                    Serial.println("HTTP_UPDATE_OK");
                    break;
            }
        }
    }
}

namespace Scripting
{
    void handleLedOn(BaseBoardConfig* Config, std::vector<String> params)
    {
        if (params.size() <= 0) return;
        Config->blinkLed();
    }
    void handleLedOff(BaseBoardConfig* Config, std::vector<String> params)
    {
        if (params.size() <= 0) return;
        Config->TurnoffLed();
    }
    void handleSetColor(BaseBoardConfig* Config, std::vector<String> params)
    {
        if (params.size() <= 0) return;
        int red = params[0].toInt();
        int green = params[1].toInt();
        int blue = params[2].toInt();

        Config->setLedColor(red, green, blue);
    }
    void handlePrint(BaseBoardConfig* Config, std::vector<String> params)
    {
        if (params.size() <= 0) return;
        Serial.println(params[0]);
    }
    void handleSleep(BaseBoardConfig* Config, std::vector<String> params)
    {
        if (params.size() <= 0) return;
        int DelayValue = params[0].toInt() / 1000;
        
        sleep(DelayValue);
    }
    void handleUpdate(BaseBoardConfig* Config, std::vector<String> params)
    {
        if (params.size() <= 0) return;
        String SSID = params[0];
        String Password = params[1];

        Functions::InitUpdate(Config, SSID + "-+" + Password + "-+");
    }
    void handleGalaxyBudSpam(BaseBoardConfig* Config, std::vector<String> params)
    {
#ifdef USE_BLUETOOTH
        Functions::InitBLEBuds(Config, "BLE_SamsungBuds");
#endif
    }

    void handleDeauthDetector(BaseBoardConfig* Config, std::vector<String> params)
    {
        if (params.size() <= 0) return;

        String channelarg = params[0];
        String SSIDarg = params[1];
        String Passwordarg = params[2];
        String WebHookUrl = params[3];

        Functions::InitDeauthDetector(Config, channelarg, SSIDarg, Passwordarg, WebHookUrl);
    }

    void handleMeasureDistance(BaseBoardConfig* Config, std::vector<String> params)
    {
        String SSIDarg = params[0];
        String RSSIarg = params[1];

        SSIDarg.trim();
        RSSIarg.trim();

        Functions::MeasureDistance(Config, SSIDarg, RSSIarg);
    }

    void handleDialConnect(BaseBoardConfig* Config, std::vector<String> params)
    {
        if (params.size() <= 0) return;

        String ControllerType = params[0];
        String SSID = params[1];
        String Password = params[2];
        String YTURL = params[3];

        HandlerType targettype = HandlerType::Base;

        if (ControllerType == "Youtube")
        {
            targettype = HandlerType::YoutubeController;
        }

        if (ControllerType == "Netflix")
        {
            targettype = HandlerType::NetflixController;
        }

        if (ControllerType == "Roku")
        {
            targettype = HandlerType::RokuController;
        }

        Functions::InitDialConnect(Config, YTURL + "-+" + SSID + "-+" + Password + "-+", targettype);
    }
}

#endif