#include "OptionDisplays.h"

#include <Arduino.h>
#include <Wire.h>

#include <U8g2lib.h>

#include <string.h>

#define TCA_ADDR 0x70

// Inset so the text is not flush against the bezel.
#define LABEL_RIGHT_MARGIN 4

namespace
{
    // Mux channel per panel, indexed by OptionDisplays::Panel.
    // Unsized on purpose so the assert catches a missing entry.
    const uint8_t MUX_CHANNELS[] = {0, 1, 2, 3, 4};

    static_assert(sizeof(MUX_CHANNELS) / sizeof(MUX_CHANNELS[0]) == OptionDisplays::Count,
                  "MUX_CHANNELS must have exactly one entry per OptionDisplays::Panel");

    U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display(U8G2_R2, U8X8_PIN_NONE);

    const uint8_t *font = u8g2_font_VCR_OSD_mf;

    char labels[OptionDisplays::Count][OptionDisplays::MaxLabel + 1];
    uint8_t dirty = 0; // bit per panel
    uint8_t nextPanel = 0;

    void selectChannel(uint8_t channel)
    {
        Wire.beginTransmission(TCA_ADDR);
        Wire.write(1 << channel);
        Wire.endTransmission();
    }

    void render(uint8_t index)
    {
        selectChannel(MUX_CHANNELS[index]);

        display.clearBuffer();
        display.setFont(font);

        const char *text = labels[index];
        const uint8_t w = display.getStrWidth(text);
        const uint8_t ascent = display.getAscent();

        // These panels are only 32 px tall and the fonts that fit at 1x leave
        // half of that empty, so double up whenever there is room for it.
        const bool doubled = (uint16_t)w * 2 <= display.getDisplayWidth() &&
                             (uint16_t)ascent * 2 <= display.getDisplayHeight();
        const uint8_t scale = doubled ? 2 : 1;

        // Right justified, as the readouts in the aircraft are, so a value keeps
        // its digits in place as it gains and loses characters.
        const uint16_t used = (uint16_t)w * scale + LABEL_RIGHT_MARGIN;
        const uint8_t x = used < display.getDisplayWidth()
                              ? display.getDisplayWidth() - used
                              : 0;
        const uint8_t y = (display.getDisplayHeight() + ascent * scale) / 2;

        if (doubled)
        {
            display.drawStrX2(x, y, text);
        }
        else
        {
            display.drawStr(x, y, text);
        }

        display.sendBuffer();
    }
}

namespace OptionDisplays
{
    void init()
    {
        Wire.begin();
        display.setBusClock(400000);

        clearAll();

        for (uint8_t i = 0; i < Count; ++i)
        {
            selectChannel(MUX_CHANNELS[i]);
            display.begin();
        }

        dirty = 0;
    }

    void setLabel(Panel panel, const char *text)
    {
        if (panel >= Count)
        {
            return;
        }

        strncpy(labels[panel], text, MaxLabel);
        labels[panel][MaxLabel] = '\0';
        dirty |= (1 << panel);
    }

    void setFont(const uint8_t *newFont)
    {
        if (newFont == NULL || newFont == font)
        {
            return;
        }

        font = newFont;
        dirty = (1 << Count) - 1;
    }

    void clearAll()
    {
        for (uint8_t i = 0; i < Count; ++i)
        {
            labels[i][0] = '\0';
            dirty |= (1 << i);
        }
    }

    void update()
    {
        if (dirty == 0)
        {
            return;
        }

        // Round-robin from where we left off so no panel starves.
        for (uint8_t i = 0; i < Count; ++i)
        {
            const uint8_t index = (nextPanel + i) % Count;

            if (dirty & (1 << index))
            {
                render(index);
                dirty &= ~(1 << index);
                nextPanel = (index + 1) % Count;
                return;
            }
        }
    }

    void flushAll()
    {
        // update() renders one dirty panel per call and returns early once
        // none are left, so Count passes always drains the queue.
        for (uint8_t i = 0; i < Count; ++i)
        {
            update();
        }
    }
}
