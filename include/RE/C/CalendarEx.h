#pragma once

namespace RE
{
    inline float Calendar_GetHoursPerDay()
    {
        REL::Relocation<float*> hours{ RELOCATION_ID(0, 195681) }; // TODO: Get SE Offset
        return *hours;
    }
}
