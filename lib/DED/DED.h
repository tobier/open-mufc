#pragma once

#include <Arduino.h>

namespace DED
{
    constexpr uint8_t Lines = 5;
    constexpr uint8_t Columns = 24;

    void draw(const char rows[Lines][Columns + 1]);

    namespace Font
    {
        // The font has no '@' or '^'; those slots hold symbols.
        constexpr uint8_t GlyphUpDownArrow = 0x40;
        constexpr uint8_t GlyphDegree = 0x5E;

        uint8_t normalize(uint8_t c);
        uint8_t inverseOf(uint8_t c);
        bool hasGlyph(uint8_t c);
    }
}