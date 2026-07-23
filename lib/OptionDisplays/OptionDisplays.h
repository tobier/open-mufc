#pragma once

#include <stdint.h>

namespace OptionDisplays
{
    enum Panel : uint8_t
    {
        Opt1,
        Opt2,
        Opt3,
        Opt4,
        Opt5,
        Count
    };

    // Longest label a panel will show; anything longer is truncated.
    static const uint8_t MaxLabel = 8;

    // Initializes Wire, the mux and every panel. Call once from setup().
    void init();

    // Draws firmware info across the panels, one at a time top to bottom.
    // Blocking; only safe to call while nothing else needs servicing.
    void selfTest(uint16_t stepMs = 250);

    void setLabel(Panel panel, const char *text);
    void clearAll();

    void update();

    void flushAll();
}
