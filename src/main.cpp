#include <Arduino.h>

#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>

#include "Lights.h"
#include "MainDisplay.h"
#include "Modules.h"
#include "OptionDisplays.h"

static void onAircraftNameChange(char *newValue)
{
  // A module that took the display owns it from here; only paint the idle
  // screen when nothing did.
  if (Modules::select(newValue))
  {
    return;
  }

  if (newValue[0] == '\0' || newValue[0] == ' ')
  {
    MainDisplay::idle(F("waiting for mission"));
  }
  else
  {
    // Unsupported aircraft: show which one, so it is obvious why nothing else
    // is happening.
    MainDisplay::idle(newValue);
  }
}

DcsBios::StringBuffer<16> aircraftNameBuffer(MetadataStart_ACFT_NAME_A, onAircraftNameChange);

void setup()
{
  DcsBios::setup();

  MainDisplay::init();
  OptionDisplays::init();
  Lights::init();
  Modules::init();

  MainDisplay::idle(F("waiting for DCS-BIOS"));
}

void loop()
{
  DcsBios::loop();
  Modules::update();
  OptionDisplays::update();
}
