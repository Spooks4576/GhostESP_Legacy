#include <WiFi.h>
#include "flipperLED.h"
#include "CommandLine.h"
#include "YoutubeController.h"
#include "Dial.h"

flipperLED led;

void YTConnect(const String& YTURL, const String& SSID, const String& Password) {
  String mutableSSID = SSID;
  String mutablePassword = Password;
  YoutubeController* YtController = new YoutubeController();

  String ActualURL = YTURL;

  DIALClient* dial = new DIALClient(ActualURL, mutableSSID, mutablePassword, YtController);

  dial->Execute();

  delete YtController;
  delete dial;
}

void RickRollTV(const String& SSID, const String& Password) {
  String mutableSSID = SSID;
  String mutablePassword = Password;


  YoutubeController* YtController = new YoutubeController();

  String EmptyURL = "";

  DIALClient* dial = new DIALClient(EmptyURL, mutableSSID, mutablePassword, YtController);

  dial->Execute();

  delete YtController;
  delete dial;
}

void setup() {
  Serial.begin(115200);


  Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));
  Serial.println("Welcome to Ghost ESP Made by Spooky");
  led.RunSetup();
}

Command<const String&, const String&, const String&> cmd1("YTConnect", "Connect to YouTube. Usage: YTConnect <YTURL> <SSID> <Password>", YTConnect);
Command<const String&, const String&> cmd2("RickRollTV", "Rickroll a TV. Usage: RickRollTV <SSID> <Password>", RickRollTV);
std::vector<CommandBase*> commands = { &cmd1, &cmd2 };

CommandLine commandli(commands);

void loop() {
  commandli.loop();
}
