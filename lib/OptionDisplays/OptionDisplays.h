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

    void setLabel(Panel panel, const char *text);

    // Aircraft modules own font choice, as with MainDisplay. Applies to every
    // panel; takes effect on the next redraw.
    void setFont(const uint8_t *font);
    void clearAll();

    void update();

    void flushAll();
}
