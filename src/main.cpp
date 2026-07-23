#include <Arduino.h>

#include "Lights.h"
#include "MainDisplay.h"

void setup()
{
  Serial.begin(COM_BAUD_RATE);

  MainDisplay::init();
  MainDisplay::selfTest();

  Lights::init();
  Lights::selfTest();

  delay(2500);
  MainDisplay::clear();
}

void loop()
{
  // TODO: loop code
}
