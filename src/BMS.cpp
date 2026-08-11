#if COMPILE_FOR_BMS
#include "BMS.h"

#include "Arduino.h"

#include <FalconBMSArduinoConnector.h>

#include "MainDisplay.h"

namespace
{
    FalconBMSArduinoConnector bms;
}

namespace BMS
{
    void setup()
    {
        MainDisplay::splash();
    }

    void loop()
    {
    }
}
#endif // COMPILE_FOR_BMS