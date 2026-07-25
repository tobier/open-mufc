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
}
