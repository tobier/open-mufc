#include <Arduino.h>

#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>

#include "Lights.h"
#include "MainDisplay.h"
#include "Modules.h"
#include "OptionDisplays.h"

#define ACFT_NAME_LEN 24

// The update counter increments every export frame, so its absence means DCS
// stopped talking rather than that the aircraft went away.
#define STALL_TIMEOUT_MS 1000

static char aircraft[ACFT_NAME_LEN + 1] = "";
static unsigned long lastFrameMs = 0;
static bool connected = false;
static bool stalled = false;

static void applyAircraft()
{
  // DCS-BIOS reports NONE when there is no cockpit, which is what you get after
  // crashing as well as between missions.
  const bool none = aircraft[0] == '\0' || aircraft[0] == ' ' ||
                    strcmp_P(aircraft, PSTR("NONE")) == 0;

  // A module that took the display owns it from here; only paint the idle
  // screen when nothing did.
  if (!none && Modules::select(aircraft))
  {
    return;
  }

  Modules::release();

  if (none)
  {
    MainDisplay::idle(F("waiting for mission"));
  }
  else
  {
    // Unsupported aircraft: show which one, so it is obvious why nothing else
    // is happening.
    MainDisplay::idle(aircraft);
  }
}

static void onAircraftNameChange(char *newValue)
{
  strncpy(aircraft, newValue, ACFT_NAME_LEN);
  aircraft[ACFT_NAME_LEN] = '\0';
  applyAircraft();
}

static void onUpdateCounterChange(unsigned int newValue)
{
  (void)newValue;

  lastFrameMs = millis();
  connected = true;

  if (stalled)
  {
    // DCS-BIOS only reports the name when it changes, so coming back to the same
    // aircraft would never re-arm the module without replaying the stored one.
    stalled = false;
    applyAircraft();
  }
}

DcsBios::StringBuffer<ACFT_NAME_LEN> aircraftNameBuffer(MetadataStart_ACFT_NAME_A, onAircraftNameChange);
DcsBios::IntegerBuffer updateCounterBuffer(MetadataEnd_UPDATE_COUNTER, onUpdateCounterChange);

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

  if (connected && !stalled && (millis() - lastFrameMs) > STALL_TIMEOUT_MS)
  {
    stalled = true;
    Modules::release();
    MainDisplay::idle(F("waiting for DCS-BIOS"));
  }

  Modules::update();
  OptionDisplays::update();
}
