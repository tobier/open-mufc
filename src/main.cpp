#include <Arduino.h>

// DcsBios.h defines setup(), loop() and the parser as non-inline functions, so
// exactly one translation unit may declare the transport. Keep this here rather
// than in build_flags, or every module that includes DcsBios.h collides at link.
#define DCSBIOS_DEFAULT_SERIAL
#include <DcsBios.h>

#include "AJS37.h"
#include "Lights.h"
#include "MainDisplay.h"
#include "OptionDisplays.h"

// Boot self test runs unless platformio.ini defines ENABLE_SELF_TEST=0.
#ifndef ENABLE_SELF_TEST
#define ENABLE_SELF_TEST 1
#endif

#define SELF_TEST_HOLD_MS 2500

static void runSelfTest()
{
#if ENABLE_SELF_TEST
  MainDisplay::selfTest();
  OptionDisplays::selfTest();
  Lights::selfTest();

  delay(SELF_TEST_HOLD_MS);

  MainDisplay::clear();
  OptionDisplays::clearAll();
  OptionDisplays::flushAll();
#endif
}

static void onAircraftNameChange(char *newValue)
{
  const bool handled = AJS37::handles(newValue);
  AJS37::setActive(handled);

  // A module that took the display owns it from here; only paint the idle
  // screen when nothing did.
  if (handled)
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
  AJS37::init();

  runSelfTest();

  MainDisplay::idle(F("waiting for DCS-BIOS"));
}

void loop()
{
  DcsBios::loop();
  AJS37::update();
  OptionDisplays::update();
}
