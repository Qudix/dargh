#pragma once

namespace RE
{
    inline float Calendar_GetHoursPerDay()
    {
        REL::Relocation<float*> hours{ RELOCATION_ID(241610, 195681) };
        return *hours;
    }
}
