#pragma once

namespace RE
{
    inline std::int32_t Actor_GetFactionRank(Actor* a_actor, TESFaction* a_faction, bool a_isPlayer)
    {
        using func_t = decltype(&Actor_GetFactionRank);
        REL::Relocation<func_t> func{ RELOCATION_ID(36668, 37676) };
        return func(a_actor, a_faction, a_isPlayer);
    }

    inline double Actor_GetMoveDirectionRelativeToFacing(Actor* a_actor)
    {
        using func_t = decltype(&Actor_GetMoveDirectionRelativeToFacing);
        REL::Relocation<func_t> func{ RELOCATION_ID(36935, 37960) };
        return func(a_actor);
    }

    inline bool Actor_HasShout(Actor* a_actor, TESShout* a_shout)
    {
        using func_t = decltype(&Actor_HasShout);
        REL::Relocation<func_t> func{ RELOCATION_ID(37829, 38783) };
        return func(a_actor, a_shout);
    }

    inline bool Actor_IsMoving(Actor* a_actor)
    {
        using func_t = decltype(&Actor_IsMoving);
        REL::Relocation<func_t> func{ RELOCATION_ID(36928, 37953) };
        return func(a_actor);
    }
}
