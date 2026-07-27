#include "FA18C.h"

#include <Arduino.h>
#include <string.h>

#include "MainDisplay.h"

namespace FA18C
{
    void init()
    {
    }

    bool handles(const char *aircraftName)
    {
        return strcmp_P(aircraftName, PSTR("FA-18C_hornet")) == 0;
    }

    void setActive(bool active)
    {
        if (active)
        {
            MainDisplay::idle();
        }
    }

    void update()
    {
        // No cockpit data driven yet.
    }
}
