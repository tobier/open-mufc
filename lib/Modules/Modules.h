#pragma once

namespace Modules
{
    // How the dispatcher sees an aircraft module. Modules stay plain
    // namespaces with free functions; this is only the shape they must expose
    // to be registered in Modules.cpp.
    struct Module
    {
        // True if this module drives the given DCS-BIOS aircraft name.
        bool (*handles)(const char *aircraftName);

        // Resets cached state. Called once from Modules::init().
        void (*init)();

        // Takes or releases ownership of the panel hardware.
        void (*setActive)(bool active);

        // Called every loop(), active or not.
        void (*update)();
    };

    // Initializes every registered module. Call once from setup().
    void init();

    // Points the dispatcher at the aircraft DCS-BIOS reports. Releases the
    // outgoing module before activating the incoming one, so ownership of the
    // display and lamps never overlaps. Returns false when no module claims
    // the name, leaving the caller to paint the idle screen.
    bool select(const char *aircraftName);

    // Drops the active module without naming a replacement, for when the sim
    // goes away rather than swapping aircraft. Idempotent.
    void release();

    // Drives every module, not just the active one: a module that was just
    // released still needs a tick to walk its lamps back down. Call every
    // loop().
    void update();
}
