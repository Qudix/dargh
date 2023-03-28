#pragma once

namespace RE
{
    inline std::int32_t Actor_GetFactionRank(Actor* a_actor, TESFaction* a_faction, bool a_isPlayer)
    {
        // This is technically in Character but even BGS doesn't care
        using func_t = decltype(&Actor_GetFactionRank);
        REL::Relocation<func_t> func{ RELOCATION_ID(0, 37676) }; // TODO: Get SE Offset
        return func(a_actor, a_faction, a_isPlayer);
    }

    inline double Actor_GetMoveDirectionRelativeToFacing(Actor* a_actor)
    {
        using func_t = decltype(&Actor_GetMoveDirectionRelativeToFacing);
        REL::Relocation<func_t> func{ RELOCATION_ID(0, 37960) }; // TODO: Get SE Offset
        return func(a_actor);
    }

    inline bool Actor_HasShout(Actor* a_actor, TESShout* a_shout)
    {
        using func_t = decltype(&Actor_HasShout);
        REL::Relocation<func_t> func{ RELOCATION_ID(0, 38783) }; // TODO: Get SE Offset
        return func(a_actor, a_shout);
    }

    inline bool Actor_IsMoving(Actor* a_actor)
    {
        using func_t = decltype(&Actor_IsMoving);
        REL::Relocation<func_t> func{ RELOCATION_ID(0, 37953) }; // TODO: Get SE Offset
        return func(a_actor);
    }
}
