#include "F16C.h"

#include <Arduino.h>
#include <string.h>

#include <internal/Addresses.h>
#include <internal/ExportStreamListener.h>

#include "DED.h"
#include "Lights.h"
#include "OptionDisplays.h"

// Unrecognised characters draw as '?' so a missing symbol gets reported rather
// than passing for a plausible capital. 0 folds them to a capital instead.
#define DED_PROBE_SYMBOLS 1

#define CMDS_AMOUNT 4

namespace
{
    DcsBios::StringBuffer<DED::Columns> dedText[DED::Lines] = {
        {F_16C_50_DED_L1_A, NULL},
        {F_16C_50_DED_L2_A, NULL},
        {F_16C_50_DED_L3_A, NULL},
        {F_16C_50_DED_L4_A, NULL},
        {F_16C_50_DED_L5_A, NULL},
    };

    DcsBios::StringBuffer<DED::Columns> dedFormat[DED::Lines] = {
        {F_16C_50_DED_L1_FORMAT_A, NULL},
        {F_16C_50_DED_L2_FORMAT_A, NULL},
        {F_16C_50_DED_L3_FORMAT_A, NULL},
        {F_16C_50_DED_L4_FORMAT_A, NULL},
        {F_16C_50_DED_L5_FORMAT_A, NULL},
    };

    bool active = false;
    bool dedDirty = false;
    bool lightsDirty = false;

    bool aaBtn = false;
    bool agBtn = false;
    bool masterCaution = false;

    void applyLights()
    {
        Lights::set(Lights::MasterCaution, active && masterCaution);
        Lights::set(Lights::AirToAir, active && aaBtn);
        Lights::set(Lights::AirToGround, active && agBtn);
    }

    void onIcpAaModeBtnChange(unsigned int newValue)
    {
        aaBtn = (newValue != 0);
        lightsDirty = true;
    }

    void onIcpAgModeBtnChange(unsigned int newValue)
    {
        agBtn = (newValue != 0);
        lightsDirty = true;
    }

    void onLightMasterCautionChange(unsigned int newValue)
    {
        masterCaution = (newValue != 0);
        lightsDirty = true;
    }

    DcsBios::IntegerBuffer icpAaModeBtnBuffer(F_16C_50_ICP_AA_MODE_BTN, onIcpAaModeBtnChange);
    DcsBios::IntegerBuffer icpAgModeBtnBuffer(F_16C_50_ICP_AG_MODE_BTN, onIcpAgModeBtnChange);
    DcsBios::IntegerBuffer lightMasterCautionBuffer(F_16C_50_LIGHT_MASTER_CAUTION, onLightMasterCautionChange);

    char cmdsO1[CMDS_AMOUNT + 1] = "";
    char cmdsO2[CMDS_AMOUNT + 1] = "";
    char cmdsCh[CMDS_AMOUNT + 1] = "";
    char cmdsFl[CMDS_AMOUNT + 1] = "";

    bool panelsDirty = false;

    void copyTrimmed(char *dest, const char *src)
    {
        while (*src == ' ')
        {
            src++;
        }

        uint8_t len = strlen(src);
        while (len > 0 && src[len - 1] == ' ')
        {
            len--;
        }

        memcpy(dest, src, len);
        dest[len] = '\0';
    }

    void applyPanels()
    {
        OptionDisplays::setLabel(OptionDisplays::Opt1, "");
        OptionDisplays::setLabel(OptionDisplays::Opt2, active ? cmdsO1 : "");
        OptionDisplays::setLabel(OptionDisplays::Opt3, active ? cmdsO2 : "");
        OptionDisplays::setLabel(OptionDisplays::Opt4, active ? cmdsCh : "");
        OptionDisplays::setLabel(OptionDisplays::Opt5, active ? cmdsFl : "");
    }

    void onCmdsO1AmountChange(char *newValue)
    {
        copyTrimmed(cmdsO1, newValue);
        panelsDirty = true;
    }

    void onCmdsO2AmountChange(char *newValue)
    {
        copyTrimmed(cmdsO2, newValue);
        panelsDirty = true;
    }

    void onCmdsChAmountChange(char *newValue)
    {
        copyTrimmed(cmdsCh, newValue);
        panelsDirty = true;
    }

    void onCmdsFlAmountChange(char *newValue)
    {
        copyTrimmed(cmdsFl, newValue);
        panelsDirty = true;
    }

    DcsBios::StringBuffer<CMDS_AMOUNT> cmdsO1Buffer(F_16C_50_CMDS_O1_AMOUNT_A, onCmdsO1AmountChange);
    DcsBios::StringBuffer<CMDS_AMOUNT> cmdsO2Buffer(F_16C_50_CMDS_O2_AMOUNT_A, onCmdsO2AmountChange);
    DcsBios::StringBuffer<CMDS_AMOUNT> cmdsChBuffer(F_16C_50_CMDS_CH_AMOUNT_A, onCmdsChAmountChange);
    DcsBios::StringBuffer<CMDS_AMOUNT> cmdsFlBuffer(F_16C_50_CMDS_FL_AMOUNT_A, onCmdsFlAmountChange);

    uint8_t symbolSlotFor(uint8_t c)
    {
        switch (c)
        {
        // DCS spells DED symbols as lowercase letters.
        case 'a':
            return DED::Font::GlyphUpDownArrow; // as in "STPT <up/down> 1"
        case 'o':
            return DED::Font::GlyphDegree; // as in "LAT N 41<deg>55.039'"
        default:
            return c;
        }
    }

    uint8_t glyphFor(char text, char format)
    {
        uint8_t c = symbolSlotFor((uint8_t)text);

#if DED_PROBE_SYMBOLS
        // NUL is the export buffers before the first frame, not a symbol.
        if (!DED::Font::hasGlyph(c))
        {
            c = '?';
        }
#endif

        c = DED::Font::normalize(c);

        if (format == 'i')
        {
            return DED::Font::inverseOf(c);
        }

        return c;
    }

    bool hasNewDedData()
    {
        for (uint8_t line = 0; line < DED::Lines; line++)
        {
            if (dedText[line].hasUpdatedData() || dedFormat[line].hasUpdatedData())
            {
                return true;
            }
        }

        return false;
    }

    void drawDed()
    {
        char rows[DED::Lines][DED::Columns + 1];

        for (uint8_t row = 0; row < DED::Lines; row++)
        {
            const char *text = dedText[row].getData();
            const char *format = dedFormat[row].getData();

            for (uint8_t col = 0; col < DED::Columns; col++)
            {
                rows[row][col] = (char)glyphFor(text[col], format[col]);
            }
            rows[row][DED::Columns] = '\0';
        }

        DED::draw(rows);
    }
}

namespace DCS::F16C
{
    void init()
    {
        dedDirty = false;

        aaBtn = false;
        agBtn = false;
        masterCaution = false;
        lightsDirty = true;

        cmdsO1[0] = '\0';
        cmdsO2[0] = '\0';
        cmdsCh[0] = '\0';
        cmdsFl[0] = '\0';
        panelsDirty = true;
    }

    bool handles(const char *aircraftName)
    {
        return strcmp_P(aircraftName, PSTR("F-16C_50")) == 0;
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
            // Repaint so the DED replaces whatever idle() left behind, even when
            // no line has changed since the last time we flew the Viper.
            dedDirty = true;
        }

        // Takes the lamps and panels live, or clears them on release.
        lightsDirty = true;
        panelsDirty = true;
    }

    void update()
    {
        // Before the active check, so a release still reaches the lamps.
        if (lightsDirty)
        {
            applyLights();
            lightsDirty = false;
        }

        if (panelsDirty)
        {
            applyPanels();
            panelsDirty = false;
        }

        if (!active)
        {
            return;
        }

        dedDirty |= hasNewDedData();
        if (!dedDirty)
            return;

        drawDed();
        dedDirty = false;
    }
}
