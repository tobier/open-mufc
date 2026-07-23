#include "AJS37.h"

#include <Arduino.h>
#include <string.h>

// Not <DcsBios.h>: that header defines the transport (setup/loop/parser) as
// non-inline functions, so only main.cpp may include it. Modules that merely
// consume export data pull in the pieces they need instead.
#include <internal/Addresses.h>
#include <internal/ExportStreamListener.h>

#include <U8g2lib.h>

#include "MainDisplay.h"

#define CK37_DIGITS 6
#define CK37_X 14
#define CK37_Y 55

namespace
{
    char digits[CK37_DIGITS + 1] = "      ";
    bool dirty = false;
    bool active = false;

    void setDigit(uint8_t index, char value)
    {
        if (digits[index] == value)
        {
            return;
        }

        digits[index] = value;
        dirty = true;
    }

    // DCS-BIOS exports each CK37 drum as its own single-character field, two
    // bytes apart, so one listener per digit. Addresses come from the named
    // constants because they shift between DCS-BIOS versions.
    void onNavData1(char *value) { setDigit(0, value[0]); }
    void onNavData2(char *value) { setDigit(1, value[0]); }
    void onNavData3(char *value) { setDigit(2, value[0]); }
    void onNavData4(char *value) { setDigit(3, value[0]); }
    void onNavData5(char *value) { setDigit(4, value[0]); }
    void onNavData6(char *value) { setDigit(5, value[0]); }

    DcsBios::StringBuffer<1> navData1(AJS37_AJS37_NAV_INDICATOR_DATA_1_A, onNavData1);
    DcsBios::StringBuffer<1> navData2(AJS37_AJS37_NAV_INDICATOR_DATA_2_A, onNavData2);
    DcsBios::StringBuffer<1> navData3(AJS37_AJS37_NAV_INDICATOR_DATA_3_A, onNavData3);
    DcsBios::StringBuffer<1> navData4(AJS37_AJS37_NAV_INDICATOR_DATA_4_A, onNavData4);
    DcsBios::StringBuffer<1> navData5(AJS37_AJS37_NAV_INDICATOR_DATA_5_A, onNavData5);
    DcsBios::StringBuffer<1> navData6(AJS37_AJS37_NAV_INDICATOR_DATA_6_A, onNavData6);
}

namespace AJS37
{
    void init()
    {
        memset(digits, ' ', CK37_DIGITS);
        digits[CK37_DIGITS] = '\0';
        dirty = false;
    }

    bool handles(const char *aircraftName)
    {
        // PSTR keeps the name in flash. Verify this against what DCS-BIOS
        // actually reports for the Viggen.
        return strcmp_P(aircraftName, PSTR("AJS37")) == 0;
    }

    void setActive(bool value)
    {
        if (active == value)
        {
            return;
        }

        active = value;

        if (active)
        {
            // Repaint so the CK37 field replaces whatever idle() left behind.
            dirty = true;
        }
        else
        {
            init();
        }
    }

    void update()
    {
        if (!active || !dirty)
        {
            return;
        }

        // The font is set here rather than in init() because idle() switches
        // the display to the message font whenever the mission state changes.
        MainDisplay::clearBuffer();
        MainDisplay::setFont(u8g2_font_inb46_mn);
        MainDisplay::drawText(CK37_X, CK37_Y, digits);
        MainDisplay::flush();

        dirty = false;
    }
}
