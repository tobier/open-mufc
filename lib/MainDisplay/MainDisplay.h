#pragma once

#include <stdint.h>

// For __FlashStringHelper, so callers can pass F("...") literals.
#include <Arduino.h>

namespace MainDisplay
{
    // Initializes the display over SPI. Call once from setup().
    void init();

    // Draws the boot splash, from assets/splash.png via scripts/splash.py.
    void splash();

    // Blanks the panel, for when no aircraft module owns it.
    void idle();

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
