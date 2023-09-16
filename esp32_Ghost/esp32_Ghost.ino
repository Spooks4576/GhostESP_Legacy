#include <WiFi.h>
#include "flipperLED.h"
#include "CommandLine.h"
#include "YoutubeController.h"
#include "ESPmDNSHelper.h"
#include "Dial.h"

flipperLED led;

void YTConnect(const char* YTURL, const char* SSID, const char* Password) {
  YoutubeController* YtController = new YoutubeController();

  DIALClient* dial = new DIALClient(YTURL, SSID, Password, YtController);

  led.TurnPurple();

  dial->Execute();

  led.offLED();

  delete YtController;
  delete dial;
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

void handleStopCommand() {
    Serial.println("Rebooting...");
    delay(500);
    esp_restart();
}

void setup() {
  Serial.begin(115200);


  Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));
  Serial.println("Welcome to Ghost ESP Made by Spooky");
  led.RunSetup();
}

Command<const char*, const char*, const char*, const char*> cmd5("ChromeConnectYT", "Connect using YTChrome With Target. Usage: YTChromeConnect <SSID> <Password> <DeviceTarget> <ID>", YTChromeConnectToTarget);
Command<const char*, const char*, const char*> cmd1("YTVConnect", "Connect to YouTube. Usage: YTConnect <ID> <SSID> <Password>", YTConnect);
Command<const char*, const char*> cmd2("RickRollTV", "Rickroll a TV. Usage: RickRollTV <SSID> <Password>", RickRollTV);
Command<const char*, const char*, const char*> cmd4("ChromeConnectEZYT", "Connect to Youtube Easily Usage: YTChromeConnectEasy <SSID> <Password> <ID>", YTChromeConnectEasy);
Command<> cmd3("stop", "Reboots the ESP32.", handleStopCommand);
const int numCommands = 5;
CommandBase* commands[MAX_COMMANDS] = {&cmd1, &cmd2, &cmd3, &cmd4, &cmd5};

CommandLine commandli(commands, numCommands);

void loop() {
  commandli.loop();

  delay(100);
}
