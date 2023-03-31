#pragma once

namespace RE
{
    inline bool MagicTarget_HasMagicEffectWithKeyword(MagicTarget* a_this, BGSKeyword* a_kywd, std::uint64_t a_arg2)
    {
        using func_t = decltype(&MagicTarget_HasMagicEffectWithKeyword);
        REL::Relocation<func_t> func{ RELOCATION_ID(33734, 34518) };
        return func(a_this, a_kywd, a_arg2);
    }
}
