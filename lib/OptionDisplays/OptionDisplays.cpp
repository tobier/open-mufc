#include "OptionDisplays.h"

#include <Arduino.h>
#include <Wire.h>

#include <U8g2lib.h>

#include <string.h>

#define TCA_ADDR 0x70

namespace
{
    // Mux channel per panel, indexed by OptionDisplays::Panel.
    // Unsized on purpose so the assert catches a missing entry.
    const uint8_t MUX_CHANNELS[] = {0, 1, 2, 3, 4};

    static_assert(sizeof(MUX_CHANNELS) / sizeof(MUX_CHANNELS[0]) == OptionDisplays::Count,
                  "MUX_CHANNELS must have exactly one entry per OptionDisplays::Panel");

    U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R2, U8X8_PIN_NONE);

    const uint8_t *const LabelFont = u8g2_font_helvB18_tr;

    char labels[OptionDisplays::Count][OptionDisplays::MaxLabel + 1];
    uint8_t dirty = 0; // bit per panel
    uint8_t nextPanel = 0;

    void selectChannel(uint8_t channel)
    {
        Wire.beginTransmission(TCA_ADDR);
        Wire.write(1 << channel);
        Wire.endTransmission();
    }

    // Renders one panel's label and pushes it. Blocking, ~25 ms at 400 kHz.
    void render(uint8_t index)
    {
        selectChannel(MUX_CHANNELS[index]);

        display.clearBuffer();
        display.setFont(LabelFont);

        const char *text = labels[index];
        const uint8_t x = (display.getDisplayWidth() - display.getStrWidth(text)) / 2;
        const uint8_t y = (display.getDisplayHeight() + display.getAscent()) / 2;
        display.drawStr(x, y, text);

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

    void selfTest(uint16_t stepMs)
    {
        static const char *const banner[Count] = {"OPEN", "UFC", FIRMWARE_VERSION, "OPT", "OK"};

        // One panel per pass so they light up in sequence rather than at once.
        for (uint8_t i = 0; i < Count; ++i)
        {
            setLabel(static_cast<Panel>(i), banner[i]);
            update();
            delay(stepMs);
        }
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
