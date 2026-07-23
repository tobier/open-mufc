#pragma once

namespace AJS37
{
    // Resets the CK37 readout to blanks. Call once from setup().
    void init();

    // True if this module drives the given DCS-BIOS aircraft name.
    bool handles(const char *aircraftName);

    // Takes or releases ownership of the main display. Releasing blanks the
    // stored digits so a later mission does not start with stale values.
    void setActive(bool active);

    // Redraws the CK37 digits if any changed since the last call. Cheap when
    // nothing moved, so call it every loop().
    void update();
}
