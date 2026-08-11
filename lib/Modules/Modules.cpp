#include "Modules.h"

#include <stdint.h>

#include "AJS37.h"
#include "F16C.h"
#include "FA18C.h"

namespace
{
    // The registry. Adding an airframe means writing the module and adding one
    // row here; nothing else in the firmware needs to know it exists.
    const Modules::Module registry[] = {
        {AJS37::handles, AJS37::init, AJS37::setActive, AJS37::update},
        {F16C::handles, F16C::init, F16C::setActive, F16C::update},
        //{FA18C::handles, FA18C::init, FA18C::setActive, FA18C::update}, TODO: enable when ready
    };

    const uint8_t ModuleCount = sizeof(registry) / sizeof(registry[0]);

    // Index into registry, or NoModule when DCS is between missions or flying
    // something unsupported. Tracked here so a switch can release the outgoing
    // module without every module having to work that out for itself.
    const uint8_t NoModule = 0xFF;
    uint8_t activeModule = NoModule;
}

namespace Modules
{
    void init()
    {
        for (uint8_t i = 0; i < ModuleCount; i++)
        {
            registry[i].init();
        }

        activeModule = NoModule;
    }

    bool select(const char *aircraftName)
    {
        uint8_t match = NoModule;

        for (uint8_t i = 0; i < ModuleCount; i++)
        {
            if (registry[i].handles(aircraftName))
            {
                match = i;
                break;
            }
        }

        // DCS-BIOS re-sends the aircraft name on every mission reload, so the
        // common case is no change at all. Returning early keeps that from
        // tearing down and rebuilding the active module's screen.
        if (match == activeModule)
        {
            return activeModule != NoModule;
        }

        // Release before activating: the outgoing module drops its lamps and
        // stale digits, so the incoming one paints over a known state.
        if (activeModule != NoModule)
        {
            registry[activeModule].setActive(false);
        }

        activeModule = match;

        if (activeModule != NoModule)
        {
            registry[activeModule].setActive(true);
        }

        return activeModule != NoModule;
    }

    void release()
    {
        if (activeModule == NoModule)
        {
            return;
        }

        registry[activeModule].setActive(false);
        activeModule = NoModule;
    }

    void update()
    {
        for (uint8_t i = 0; i < ModuleCount; i++)
        {
            registry[i].update();
        }
    }
}
