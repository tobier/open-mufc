#if COMPILE_FOR_DCS
#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>

#include "DED.h"
#include "Lights.h"
#include "MainDisplay.h"
#include "Modules.h"
#include "OptionDisplays.h"

// Paints a DED test pattern at boot so layout and glyphs can be checked with
// no sim running. Set to 0 for normal operation.
#define DED_SELF_TEST 0
#define DED_SELF_TEST_HOLD_MS 15000

#define SPLASH_HOLD_MS 5000

#define ACFT_NAME_LEN 24

// The update counter increments every export frame, so its absence means DCS
// stopped talking rather than that the aircraft went away.
#define STALL_TIMEOUT_MS 1000

namespace
{
    static char aircraft[ACFT_NAME_LEN + 1] = "";
    static unsigned long lastFrameMs = 0;
    static bool stalled = false;
    static bool frameReceived = false;

    static void applyAircraft()
    {
        // DCS-BIOS reports NONE when there is no cockpit, which is what you get after
        // crashing as well as between missions.
        const bool none = aircraft[0] == '\0' || aircraft[0] == ' ' ||
                          strcmp_P(aircraft, PSTR("NONE")) == 0;

        // A module that took the display owns it from here. Anything else is DCS-BIOS
        // talking with nothing for us to draw, which stays blank.
        if (!none && DCS::Modules::select(aircraft))
        {
            return;
        }

        DCS::Modules::release();
        MainDisplay::idle();
    }

    static void onAircraftNameChange(char *newValue)
    {
        strncpy(aircraft, newValue, ACFT_NAME_LEN);
        aircraft[ACFT_NAME_LEN] = '\0';
        applyAircraft();
    }

    static void onUpdateCounterChange(unsigned int newValue)
    {
        (void)newValue;

        lastFrameMs = millis();
        frameReceived = true;

        if (stalled)
        {
            // DCS-BIOS only reports the name when it changes, so coming back to the same
            // aircraft would never re-arm the module without replaying the stored one.
            stalled = false;
            applyAircraft();
        }
    }

    DcsBios::StringBuffer<ACFT_NAME_LEN> aircraftNameBuffer(MetadataStart_ACFT_NAME_A, onAircraftNameChange);
    DcsBios::IntegerBuffer updateCounterBuffer(MetadataEnd_UPDATE_COUNTER, onUpdateCounterChange);

#if DED_SELF_TEST
    // Copies text into a row and pads the rest, so the pattern below can be
    // edited without counting to 24.
    static void fillRow(char *row, const char *text)
    {
        uint8_t col = 0;
        while (col < DED::Columns && text[col] != '\0')
        {
            row[col] = text[col];
            col++;
        }

        while (col < DED::Columns)
        {
            row[col++] = ' ';
        }

        row[DED::Columns] = '\0';
    }

    static void dedSelfTest()
    {
        char rows[DED::Lines][DED::Columns + 1];

        // Reads 1..24 across the panel: a short or clipped row is visible at a
        // glance, and the last '4' proves the full width fits.
        fillRow(rows[0], "123456789012345678901234");

        fillRow(rows[1], "R2 ABCDEFGHIJKLMNOPQRST");

        // '~' stands in for the two symbol slots, patched in below.
        fillRow(rows[2], "R3 41~55.039' STPT ~1");
        for (uint8_t col = 0; col < DED::Columns; col++)
        {
            if (rows[2][col] != '~')
            {
                continue;
            }

            rows[2][col] = (char)(col < 10 ? DED::Font::GlyphDegree
                                           : DED::Font::GlyphUpDownArrow);
        }

        // Inverted through the end of the row, so the trailing spaces show the
        // cell block and expose any gap between cells.
        fillRow(rows[3], "R4 NORMAL / INVERSE");
        for (uint8_t col = 11; col < DED::Columns; col++)
        {
            rows[3][col] = (char)DED::Font::inverseOf((uint8_t)rows[3][col]);
        }

        fillRow(rows[4], "R5 BOTTOM ROW, NOT CUT");

        DED::draw(rows);
        delay(DED_SELF_TEST_HOLD_MS);
    }
#endif
}

namespace DCS
{
    void setup()
    {
#if DED_SELF_TEST
        dedSelfTest();
#endif

        DcsBios::setup();
        DCS::Modules::init();

        MainDisplay::splash();

        // Pumped rather than delayed so the splash gives way the moment DCS-BIOS has
        // something to show. Callbacks run from here, but nothing repaints until
        // Modules::update() does, so the splash stays up throughout.
        const unsigned long splashStart = millis();
        while (!frameReceived && (millis() - splashStart) < SPLASH_HOLD_MS)
        {
            DcsBios::loop();
        }

        // Blanks if nothing arrived, or hands the panel to whatever did.
        applyAircraft();

        // Start the clock here so a boot with no sim running still falls through to
        // the stall path, rather than needing a connection to have happened first.
        lastFrameMs = millis();
    }

    void loop()
    {
        DcsBios::loop();

        if (!stalled && (millis() - lastFrameMs) > STALL_TIMEOUT_MS)
        {
            stalled = true;
            DCS::Modules::release();
            MainDisplay::idle();
        }

        DCS::Modules::update();
        OptionDisplays::update();
    }
}
#endif // COMPILE_FOR_DCS