#include <Arduino.h>

#include "Lights.h"

void setup()
{
  Serial.begin(COM_BAUD_RATE);

  Lights::init();
  Lights::selfTest();

  Serial.println("Hello, from open-mufc!");
}

void loop()
{
  // TODO: loop code
}
