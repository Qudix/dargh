#pragma once

namespace RE
{
    inline std::uint64_t AnimationFileManager_Load(std::uint64_t a_arg1, hkbContext* a_context, hkbClipGenerator* a_arg3, std::uint64_t a_arg4)
    {
        using func_t = decltype(&AnimationFileManager_Load);
        REL::Relocation<func_t> func{ RELOCATION_ID(0, 63982) }; // TODO: Get SE Offset
        return func(a_arg1, a_context, a_arg3, a_arg4);
    }
}
