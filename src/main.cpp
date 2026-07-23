#include <Arduino.h>

#include "Lights.h"
#include "MainDisplay.h"
#include "OptionDisplays.h"

void setup()
{
  Serial.begin(COM_BAUD_RATE);

  MainDisplay::init();
  MainDisplay::selfTest();

  OptionDisplays::init();
  OptionDisplays::selfTest();

  Lights::init();
  Lights::selfTest();

  delay(2500);

  MainDisplay::clear();
  OptionDisplays::clearAll();
  OptionDisplays::flushAll();
}

void loop()
{
  OptionDisplays::update();
}
