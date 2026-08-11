#pragma once

namespace DCS::FA18C
{
    void init();
    bool handles(const char *aircraftName);
    void setActive(bool active);
    void update();
}
