#include "MainDisplay.h"

#include <Arduino.h>

#include <U8g2lib.h>

// Pins for the SSD1322
#define SSD1322_DC_PIN 9
#define SSD1322_RS_PIN 8
#define SSD1322_CS_PIN 10

// Line baselines for the 13 px message font on the 64 px panel.
#define MESSAGE_LINE_1 13
#define MESSAGE_LINE_2 28
#define MESSAGE_LINE_3 43
#define MESSAGE_LINE_4 58

namespace
{
    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI display(U8G2_R2, SSD1322_CS_PIN, SSD1322_DC_PIN, SSD1322_RS_PIN);

    // Small proportional font used for boot info and debug text.
    const uint8_t *const MessageFont = u8g2_font_7x13_tf;

    // Shared by both idle() overloads. Leaves the buffer ready for line 2.
    void drawBanner()
    {
        display.clearBuffer();
        display.setFont(MessageFont);

        display.setCursor(0, MESSAGE_LINE_1);
        display.print(F("open-mufc " FIRMWARE_VERSION));
    }
}

namespace MainDisplay
{
    void init()
    {
        display.begin();
        display.setFont(MessageFont);

        display.clearBuffer();
        display.clearDisplay();
    }

    void selfTest()
    {
        display.clearBuffer();
        display.setFont(MessageFont);

        display.setCursor(0, MESSAGE_LINE_1);
        display.print(F("open-mufc " FIRMWARE_VERSION));

        display.setCursor(0, MESSAGE_LINE_2);
        display.print(F("built " __DATE__ " " __TIME__));

        display.setCursor(0, MESSAGE_LINE_3);
        display.print(F("Main display: OK"));

        display.sendBuffer();
    }

    void idle(const char *status)
    {
        drawBanner();
        display.drawStr(0, MESSAGE_LINE_2, status);
        display.sendBuffer();
    }

    void idle(const __FlashStringHelper *status)
    {
        drawBanner();
        display.setCursor(0, MESSAGE_LINE_2);
        display.print(status);
        display.sendBuffer();
    }

    void setFont(const uint8_t *font)
    {
        display.setFont(font);
    }

    void clearBuffer()
    {
        display.clearBuffer();
    }

    void clear()
    {
        display.clear();
    }

    void drawText(uint8_t x, uint8_t y, const char *text)
    {
        display.drawStr(x, y, text);
    }

    void flush()
    {
        display.sendBuffer();
    }
}
