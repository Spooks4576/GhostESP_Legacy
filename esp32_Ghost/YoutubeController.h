#pragma once
#include "AppController.h"
#include "YoutubeService.h"

class YoutubeController : public AppController {
public:

  virtual void launchApp(const String& appUrl) override;
  virtual int checkAppStatus(const String& appUrl, Device& device) override;

  String extractScreenId(const String& xmlData) {
    String startTag = "<screenId>";
    String endTag = "</screenId>";

    int startIndex = xmlData.indexOf(startTag);
    if (startIndex == -1) {
      Serial.println("Start tag not found.");
      return "";
    }
    startIndex += startTag.length();

    int endIndex = xmlData.indexOf(endTag, startIndex);
    if (endIndex == -1) {
      Serial.println("End tag not found.");
      return "";
    }

    String extractedId = xmlData.substring(startIndex, endIndex);
    if (extractedId.length() == 0) {
      Serial.println("Extracted screenId is empty.");
      return "";
    }

    Serial.println("Extracted screenId: " + extractedId);
    return extractedId;
  }

  YouTubeService YTService;
};