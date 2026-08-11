#include <Arduino.h>

#if COMPILE_FOR_DCS
#include "DCS.h"
#elif COMPILE_FOR_BMS
#include "BMS.h"
#endif

#include "Lights.h"
#include "MainDisplay.h"
#include "OptionDisplays.h"

void setup()
{
  MainDisplay::init();
  OptionDisplays::init();
  Lights::init();

#if COMPILE_FOR_DCS
  DCS::setup();
#elif COMPILE_FOR_BMS
  BMS::setup();
#endif
}

void loop()
{
#if COMPILE_FOR_DCS
  DCS::loop();
#elif COMPILE_FOR_BMS
  BMS::loop();
#endif
}