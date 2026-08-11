#pragma once

namespace DCS::AJS37
{
    void init();
    bool handles(const char *aircraftName);
    void setActive(bool active);
    void update();
}
