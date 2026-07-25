#pragma once

#include <stdint.h>

// For __FlashStringHelper, so callers can pass F("...") literals.
#include <Arduino.h>

namespace MainDisplay
{
    // Initializes the display over SPI. Call once from setup().
    void init();

    // Draws the idle screen: firmware banner on the first line, the given
    // status on the second. For when no aircraft module owns the display;
    // aircraft data uses the generic drawing below with the module's own font.
    // The F() overload keeps literals in flash; the char* one is for runtime
    // strings such as the aircraft name DCS-BIOS hands us in RAM.
    void idle(const char *status);
    void idle(const __FlashStringHelper *status);

    // Generic drawing. Aircraft modules own font choice and layout, since
    // both differ per airframe; this library only owns the panel itself.
    void setFont(const uint8_t *font);

    // Clears the off-screen buffer only. The panel keeps showing the previous
    // frame until flush(), which is what keeps redraws flicker-free.
    void clearBuffer();

    // Blanks the panel itself, immediately. For shutdown or mode changes,
    // not for the normal draw cycle.
    void clear();

    void drawText(uint8_t x, uint8_t y, const char *text);
    void flush();
}
