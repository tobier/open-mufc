#include "AJS37.h"

#include <Arduino.h>
#include <string.h>

#include <internal/Addresses.h>
#include <internal/ExportStreamListener.h>

#include <U8g2lib.h>

#include "Lights.h"
#include "MainDisplay.h"

#define CK37_DIGITS 6
#define CK37_X 14
#define CK37_Y 55

namespace
{
    char digits[CK37_DIGITS + 1] = "      ";
    bool ck37Dirty = false;
    bool active = false;

    void setDigit(uint8_t index, char value)
    {
        if (digits[index] == value)
        {
            return;
        }

        digits[index] = value;
        ck37Dirty = true;
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

    // Lamp state, mirrored from DCS. Held rather than written straight through
    // so activation can restore the panel without waiting for the next change.
    bool warnLeft = false;
    bool warnRight = false;
    bool attLit = false;
    bool hojdLit = false;
    bool lightsDirty = false;

    void applyLights()
    {
        // Huvudvarning is a left/right pair; the panel has one master caution,
        // so either lamp lights it.
        Lights::set(Lights::MasterCaution, active && (warnLeft || warnRight));

        // The Viggen has no A/A or A/G modes, so these two annunciators carry
        // real Viggen lamps instead.
        Lights::set(Lights::AirToAir, active && attLit);
        Lights::set(Lights::AirToGround, active && hojdLit);
    }

    void onHuvudvarningL(unsigned int value)
    {
        warnLeft = (value != 0);
        lightsDirty = true;
    }

    void onHuvudvarningR(unsigned int value)
    {
        warnRight = (value != 0);
        lightsDirty = true;
    }

    void onAttLamp(unsigned int value)
    {
        attLit = (value != 0);
        lightsDirty = true;
    }

    void onHojdLamp(unsigned int value)
    {
        hojdLit = (value != 0);
        lightsDirty = true;
    }

    DcsBios::IntegerBuffer huvudvarningL(AJS37_HUVUDVARNING_L, onHuvudvarningL);
    DcsBios::IntegerBuffer huvudvarningR(AJS37_HUVUDVARNING_R, onHuvudvarningR);
    DcsBios::IntegerBuffer attLamp(AJS37_ATT_LAMP, onAttLamp);
    DcsBios::IntegerBuffer hojdLamp(AJS37_HOJD_LAMP, onHojdLamp);
}

namespace AJS37
{
    void init()
    {
        memset(digits, ' ', CK37_DIGITS);
        digits[CK37_DIGITS] = '\0';
        ck37Dirty = false;

        warnLeft = false;
        warnRight = false;
        attLit = false;
        hojdLit = false;
        lightsDirty = true;
    }

    bool handles(const char *aircraftName)
    {
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
            ck37Dirty = true;
        }
        else
        {
            init();
        }

        // Takes the lamps live, or drives them all off on release.
        lightsDirty = true;
    }

    void update()
    {
        // Lamps are applied even when inactive, so releasing the module still
        // drives them off; applyLights() gates on active itself.
        if (lightsDirty)
        {
            applyLights();
            lightsDirty = false;
        }

        if (!active || !ck37Dirty)
        {
            return;
        }

        // The font is set here rather than in init() because idle() switches
        // the display to the message font whenever the mission state changes.
        MainDisplay::clearBuffer();
        MainDisplay::setFont(u8g2_font_inb46_mn);
        MainDisplay::drawText(CK37_X, CK37_Y, digits);
        MainDisplay::flush();

        ck37Dirty = false;
    }
}
