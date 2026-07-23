#include <Arduino.h>

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

void setup()
{
  Serial.begin(COM_BAUD_RATE);

  MainDisplay::init();
  OptionDisplays::init();
  Lights::init();

  runSelfTest();
}

void loop()
{
  OptionDisplays::update();
}
