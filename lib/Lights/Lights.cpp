#include "Lights.h"

#include <Arduino.h>

#define MC_BACKLIGHT_PIN 4
#define AA_BACKLIGHT_PIN 5
#define AG_BACKLIGHT_PIN 7

namespace
{
    // Indexed by Lights::Light, so the enum order and this table must match.
    // Deliberately unsized: the assert below then catches a forgotten pin,
    // which would otherwise zero-fill and silently drive pin 0 (RX0).
    const uint8_t PINS[] = {
        MC_BACKLIGHT_PIN,
        AA_BACKLIGHT_PIN,
        AG_BACKLIGHT_PIN,
    };

    static_assert(sizeof(PINS) / sizeof(PINS[0]) == Lights::Count,
                  "PINS must have exactly one entry per Lights::Light");

    uint8_t dimLevel = 255;

    // Index-based writer, so the loops below don't need to cast back to Light.
    void write(uint8_t index, bool on)
    {
        analogWrite(PINS[index], on ? dimLevel : 0);
    }
}

namespace Lights
{
    void init()
    {
        for (uint8_t i = 0; i < Count; ++i)
        {
            pinMode(PINS[i], OUTPUT);
            write(i, false);
        }
    }

    void set(Light light, bool on)
    {
        if (light >= Count)
        {
            return;
        }

        write(light, on);
    }

    void off()
    {
        for (uint8_t i = 0; i < Count; ++i)
        {
            write(i, false);
        }
    }

    void selfTest(uint16_t stepMs)
    {
        for (uint8_t i = 0; i < Count; ++i)
        {
            write(i, true);
            delay(stepMs);
        }

        delay(stepMs);
        off();
    }
}
