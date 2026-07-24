#include "F16C.h"

#include <Arduino.h>
#include <string.h>

#include "MainDisplay.h"

namespace F16C
{
    void init()
    {
    }

    bool handles(const char *aircraftName)
    {
        return strcmp_P(aircraftName, PSTR("F-16C_50")) == 0;
    }

    void setActive(bool active)
    {
        if (active)
        {
            MainDisplay::idle(F("F-16C: not implemented"));
        }
    }

    void update()
    {
        // No cockpit data driven yet.
    }
}
