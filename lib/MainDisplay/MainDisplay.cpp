#include "MainDisplay.h"

#include <Arduino.h>

#include <U8g2lib.h>

#include "Splash.h"

// Pins for the SSD1322
#define SSD1322_DC_PIN 9
#define SSD1322_RS_PIN 8
#define SSD1322_CS_PIN 10

namespace
{
    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI display(U8G2_R2, SSD1322_CS_PIN, SSD1322_DC_PIN, SSD1322_RS_PIN);
}

namespace MainDisplay
{
    void init()
    {
        display.begin();

        display.clearBuffer();
        display.clearDisplay();
    }

    void splash()
    {
        display.clearBuffer();
        display.drawXBMP(0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, SplashBits);
        display.sendBuffer();
    }

    void idle()
    {
        display.clearBuffer();
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
