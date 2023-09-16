#pragma once
#include "AppController.h"

class NetflixController : public AppController {
public:

  virtual void launchApp(const String& appUrl) override;
};