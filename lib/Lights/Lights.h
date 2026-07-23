#pragma once

#include <stdint.h>

namespace Lights
{
    enum Light : uint8_t
    {
        MasterCaution,
        AirToAir,
        AirToGround,
        Count
    };

    // Configures the pins and drives every light off. Call once from setup().
    void init();

    // Individual control.
    void set(Light light, bool lit);

    // Bulk control.
    void off();

    // Blocking power-on cascade: lights each one in turn, holds, then clears.
    // Only safe to call while nothing else needs servicing, i.e. from setup().
    void selfTest(uint16_t stepMs = 500);
}
