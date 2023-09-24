#include <WiFi.h>
#include "flipperLED.h"
#include "CommandLine.h"
#include "YoutubeController.h"
#include "ESPmDNSHelper.h"
#include "WiFiTools.h"
#include "EvilPortal.h"

#include "Dial.h"

flipperLED led;
EvilPortal Portal;
WiFiTools wifi;



void YTConnect(const char* YTURL, const char* SSID, const char* Password) {
  YoutubeController* YtController = new YoutubeController();

  DIALClient* dial = new DIALClient(YTURL, SSID, Password, YtController);

  led.TurnPurple();

  dial->Execute();

  led.offLED();

  delete YtController;
  delete dial;
}

void StartEvilPortal()
{
  Portal.begin();
}

void StartHandShakeScan(const char* Channel)
{
  int value = atoi(Channel);

  wifi.RunRawScan(value);
}

void YTChromeConnectToTarget(const char* SSID, const char* Password, const char* DeviceTarget, const char* URL)
{
  led.TurnPurple();
  ESPmDNSHelper* CCTargeter = new ESPmDNSHelper(SSID, Password, DeviceTarget, URL, "233637DE");

  delete CCTargeter;

}

void YTChromeConnectEasy(const char* SSID, const char* Password, const char* URL)
{
  led.TurnPurple();
  ESPmDNSHelper* CCTargeter = new ESPmDNSHelper(SSID, Password, "", URL, "233637DE");

  delete CCTargeter;
}

void BeaconSpamRickRoll()
{
  Serial.println("Spamming Rick Roll");

  esp_wifi_start();

  led.TurnPurple();

  while (true)
  {
    for (int x = 0; x < (sizeof(rick_roll)/sizeof(char *)); x++)
    {
      wifi.broadcastSetSSID(rick_roll[x]);
    }
  }
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

void setup() {
  Serial.begin(115200);


  Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));
  Serial.println(F("Welcome to Ghost ESP Made by Spooky"));
  led.RunSetup();

  xTaskCreatePinnedToCore(LoopTask,"LoopTask",20000,NULL,1,NULL,1);
}

Command<const char*, const char*, const char*, const char*> cmd5("ChromeConnectYT", "Connect using YTChrome With Target. Usage: YTChromeConnect <SSID> <Password> <DeviceTarget> <ID>", YTChromeConnectToTarget);
Command<const char*> cmd6("HandShakeScan", "Scan for a 4 way handshake on a specific Channel", StartHandShakeScan);
Command<const char*, const char*, const char*> cmd1("YTVConnect", "Connect to YouTube. Usage: YTConnect <ID> <SSID> <Password>", YTConnect);
Command<const char*, const char*> cmd2("RickRollTV", "Rickroll a TV. Usage: RickRollTV <SSID> <Password>", RickRollTV);
Command<const char*, const char*, const char*> cmd3("ChromeConnectEZYT", "Connect to Youtube Easily Usage: YTChromeConnectEasy <SSID> <Password> <ID>", YTChromeConnectEasy);
Command<> cmd4("RickRollSpam", "Spam Access Points With a Rick Roll", BeaconSpamRickRoll);
const int numCommands = 6;
CommandBase* commands[MAX_COMMANDS] = {&cmd1, &cmd2, &cmd3, &cmd4, &cmd5, &cmd6};
CommandLine commandli(commands, numCommands);

void loop() {
  Portal.loop(commandli);
  delay(100);
}

void LoopTask(void *parameter) {
    while (true) {
      if (ShouldMultithread)
      {
        Portal.loop(commandli);
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);  // Run every second
    }
}
