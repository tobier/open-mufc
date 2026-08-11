#pragma once

namespace DCS::F16C
{
    void init();
    bool handles(const char *aircraftName);
    void setActive(bool active);
    void update();
}
