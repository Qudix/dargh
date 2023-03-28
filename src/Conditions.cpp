// ============================================================================
//                            Conditions.cpp
// ----------------------------------------------------------------------------
// Part of the open-source Dynamic Animation Replacer (DARGH).
//
// Copyright (c) 2023 Nox Sidereum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the �Software�), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// (The MIT License)
// ============================================================================

#include "Conditions.h"

#include <numbers>

constexpr auto TWO_PI{ 2.0 * std::numbers::pi };

// Turn this on if you want to trace & debug animation condition calls
// (CAUTION: only use for debugging - this will generate large dargh.log files
//  and degrade performance)
// #define DEBUG_TRACE_CONDITIONS

// ============================================================================
//                          HELPER FUNCTIONS
// ============================================================================

static const RE::BGSKeyword* g_kwWarhammer = nullptr;

bool readGlobalVars(float* a_values, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat, int32_t a_nArgs)
{
    // ---------------------------------------------------------------------------------------------
    // Read 'a_nArgs' global variables or direct values (depending on whether the corresponding bit
    // is set in 'a_bmArgIsFloat') from 'a_args' into 'a_values'. We expect that 'a_values' has sufficient
    // memory allocated.
    // ---------------------------------------------------------------------------------------------
    // Dereference the global variables as necessary (or store the direct values).
    int32_t powerOfTwo = 1;
    for (int32_t i = 0; i < a_nArgs; i++) {
        if ((powerOfTwo & a_bmArgIsFloat) != 0) {
            // Argument is a float.
            a_values[i] = std::get<float>(a_args[i]);
        } else {
            // Argument should be a global variable.
            // Dereference it and get its current value.
            const auto formID = std::get<uint32_t>(a_args[i]);
            const auto form = RE::TESForm::LookupByID<RE::TESGlobal>(formID);
            if (!form)
                return false;

            a_values[i] = form->value;
        }

        powerOfTwo *= 2;
    }

    return true;
}

int32_t getEquippedFormType(RE::TESForm* a_form)
{
    // From DAR documentation:
    //
    // "Item types are as follows.
    //   -1 = Others
    //    0 = Fists
    //    1 = Swords
    //    2 = Daggers
    //    3 = War Axes
    //    4 = Maces
    //    5 = Greatswords
    //    6 = Battleaxes
    //    7 = Bows
    //    8 = Staff
    //    9 = Crossbows
    //    10 = Warhammers
    //    11 = Shields
    //    12 = Alteration Spells
    //    13 = Illusion Spells
    //    14 = Destruction Spells
    //    15 = Conjuration Spells
    //    16 = Restoration Spells
    //    17 = Scrolls
    //    18 = Torches"
    if (!a_form) {
        return 0;
    }

    // Spell form type
    if (a_form->Is(RE::FormType::Spell)) {
        if (const auto effect = a_form->As<RE::MagicItem>()->avEffectSetting) {
            const auto skill = effect->GetMagickSkill();
            if (skill == RE::ActorValue::kAlteration) {
                // Alteration spells
                return 12;
            }
            if (skill == RE::ActorValue::kConjuration) {
                // Conjuration spells
                return 15;
            }
            if (skill == RE::ActorValue::kDestruction) {
                // Destruction spells
                return 14;
            }
            if (skill == RE::ActorValue::kIllusion) {
                // Illusion spells
                return 13;
            }
            if (skill == RE::ActorValue::kRestoration) {
                // Restoration spells
                return 16;
            }
        }

        // Others
        return -1;
    }

    // Scroll form type
    if (a_form->Is(RE::FormType::Scroll)) {
        // Scrolls
        return 17;
    }

    if (a_form->Is(RE::FormType::Armor)) {
        if ((*(uint64_t*)(a_form + 0x1B8) & 0x200) != 0) {
            // Shields
            return 11;
        }
    } else {
        if (a_form->Is(RE::FormType::Light)) {
            // Torches
            return 18;
        } else {
            if (!a_form->Is(RE::FormType::Weapon)) {
                // Others
                return -1;
            }

            const auto weapon = a_form->As<RE::TESObjectWEAP>();

            // What type of weapon?
            auto animType = weapon->GetWeaponType();
            if (animType > RE::WEAPON_TYPE::kCrossbow) {
                // Not recognised - set to Others
                return -1;
            }

            if (animType != RE::WEAPON_TYPE::kTwoHandAxe) {
                return static_cast<int32_t>(animType);
            }

            // The base form for warhammers
            if (!g_kwWarhammer)
                g_kwWarhammer = RE::TESForm::LookupByID<RE::BGSKeyword>(0x6D930);

            // Have we got a reference to a warhammer?
            // Call the fourth function virtual function in BGSKeywordForm's VFT,
            // i.e. virtual bool HasKeyword(const BGSKeyword* a_keyword) const;  // 04
            if (weapon->HasKeyword(g_kwWarhammer)) {
                // Warhammers
                return 10;
            } else {
                // No, so return 6 (two-handed axe)
                return static_cast<int32_t>(animType);
            }
        }
    }

    // Others
    return -1;
}

bool hasKeyword(RE::TESForm* a_form, const RE::BGSKeyword* a_keyword)
{
    RE::BGSKeywordForm* keywordForm = nullptr;
    switch (a_form->formType.get()) {
        case RE::FormType::Race:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0x70);
            break;
        case RE::FormType::MagicEffect:
        case RE::FormType::Enchantment:
        case RE::FormType::Spell:
        case RE::FormType::Scroll:
        case RE::FormType::Ingredient:
        case RE::FormType::AlchemyItem:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0x40);
            break;
        case RE::FormType::Activator:
        case RE::FormType::TalkingActivator:
        case RE::FormType::Flora:
        case RE::FormType::Furniture:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0x90);
            break;
        case RE::FormType::Armor:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0x1D8);
            break;
        case RE::FormType::Book:
        case RE::FormType::Ammo:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0xF8);
            break;
        case RE::FormType::Misc:
        case RE::FormType::Apparatus:
        case RE::FormType::KeyMaster:
        case RE::FormType::SoulGem:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0xE8);
            break;
        case RE::FormType::Weapon:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0x140);
            break;
        case RE::FormType::NPC:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0x110);
            break;
        case RE::FormType::Location:
            keywordForm = (RE::BGSKeywordForm*)((char*)a_form + 0x30);
            break;
        default:
            keywordForm = skyrim_cast<RE::BGSKeywordForm*>(a_form);
            break;
    }

    if (keywordForm) {
        // Call the fourth function virtual function in BGSKeywordForm's VFT,
        // i.e. virtual bool HasKeyword(const BGSKeyword* a_keyword) const;  // 04
        return keywordForm->HasKeyword(a_keyword);
    } else {
        // Call the virtual function in TESForm's VFT at position 0x2C (== 0x160 / 0x8),
        // i.e. virtual const TESObjectREFR* AsReference2() const;   // 2C
        if (const auto refr = a_form->AsReference()) {
            // Now call the virtual function in TESObjectREFR's VFT at position 0x40 (== 0x240 / 0x8)
            // i.e. virtual bool HasKeywordHelper(const BGSKeyword* a_keyword) const;  // 0x48
            return refr->HasKeywordHelper(a_keyword);
        }
    }

    return false;
}

bool hasKeywordBoundObj(RE::TESBoundObject* a_obj, std::variant<uint32_t, float>* a_args)
{
    if (a_obj) {
        const auto formID = std::get<uint32_t>(a_args[0]);
        if (const auto keyword = RE::TESForm::LookupByID<RE::BGSKeyword>(formID)) {
            if (hasKeyword(a_obj, keyword)) {
                return true;
            }
        }
    }

    return false;
}

float getActorValPct(RE::Actor* a_actor, uint32_t a_value)
{
    const auto owner = a_actor->AsActorValueOwner();
    const float fActorValue = owner->GetActorValue(static_cast<RE::ActorValue>(a_value));
    const float fPermActorValue = owner->GetPermanentActorValue(static_cast<RE::ActorValue>(a_value));
    if (fPermActorValue <= 0.0) {
        return 1.0;
    }

    if (fActorValue > 0.0) {
        if (fActorValue >= fPermActorValue) {
            return 1.0;
        }

        return fActorValue / fPermActorValue;
    }

    return 0.0;
}

// ============================================================================
//                          CONDITION FUNCTIONS
// ============================================================================
bool IsEquippedRight(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // -------------------------------------------------------------------
    // IsEquippedRight(Form item)
    // Does the actor have the specified item equipped to his right hand?
    // -------------------------------------------------------------------
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsEquippedRight({:08X})", a_args[0]);
#endif

    if (const auto process = a_actor->GetActorRuntimeData().currentProcess) {
        if (const auto equipped = process->GetEquippedRightHand()) {
            const auto formID = std::get<uint32_t>(a_args[0]);
            if (equipped->GetFormID() == formID) {
                return true;
            }
        }
    }

    return false;
}

bool IsEquippedRightType(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsEquippedRightType(GlobalVariable type)
    // Is the item equipped to the actor's right hand the specified type?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsEquippedRightType({:08X})", a_args[0]);
#endif

    float arg;
    if (!readGlobalVars(&arg, a_args, a_bmArgIsFloat, 1)) {
        return false;
    }

    if (const auto process = a_actor->GetActorRuntimeData().currentProcess) {
        if (const auto equipped = process->GetEquippedRightHand()) {
            return getEquippedFormType(equipped) == arg;
        }
    }

    return false;
}

bool IsEquippedRightHasKeyword(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsEquippedRightHasKeyword(Keyword keyword)
    // Does the item equipped to the actor's right hand have the specified keyword?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsEquippedRightHasKeyword({:08X})", a_args[0]);
#endif

    if (const auto process = a_actor->GetActorRuntimeData().currentProcess) {
        if (const auto equipped = process->GetEquippedRightHand()) {
            const auto formID = std::get<uint32_t>(a_args[0]);
            const auto keyword = RE::TESForm::LookupByID<RE::BGSKeyword>(formID);
            return keyword && hasKeyword(equipped, keyword);
        }
    }

    return false;
}

bool IsEquippedLeft(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsEquippedLeft(Form item)
    // Does the actor have the specified item equipped to his left hand?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsEquippedLeft({:08X})", a_args[0]);
#endif

    if (const auto process = a_actor->GetActorRuntimeData().currentProcess) {
        if (const auto equipped = process->GetEquippedLeftHand()) {
            const auto formID = std::get<uint32_t>(a_args[0]);
            if (equipped->GetFormID() == formID) {
                return true;
            }
        }
    }

    return false;
}

bool IsEquippedLeftType(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsEquippedLeftType(GlobalVariable type)
    // Is the item equipped to the actor's left hand the specified type?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsEquippedLeftType({:08X})", a_args[0]);
#endif

    float arg;
    if (!readGlobalVars(&arg, a_args, a_bmArgIsFloat, 1)) {
        return false;
    }

    if (const auto process = a_actor->GetActorRuntimeData().currentProcess) {
        if (const auto equipped = process->GetEquippedLeftHand()) {
            return getEquippedFormType(equipped) == arg;
        }
    }

    return false;
}

bool IsEquippedLeftHasKeyword(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsEquippedLeftHasKeyword(Keyword keyword)
    // Does the item equipped to the actor's left hand have the specified keyword?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsEquippedLeftHasKeyword({:08X})", a_args[0]);
#endif

    if (const auto process = a_actor->GetActorRuntimeData().currentProcess) {
        if (const auto equipped = process->GetEquippedLeftHand()) {
            const auto formID = std::get<uint32_t>(a_args[0]);
            const auto keyword = RE::TESForm::LookupByID<RE::BGSKeyword>(formID);
            return keyword && hasKeyword(equipped, keyword);
        }
    }

    return false;
}

bool IsEquippedShout(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsEquippedShout(Form shout)
    // Does the actor currently have the specified shout?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsEquippedShout({:08X})", a_args[0]);
#endif

    const auto power = a_actor->GetActorRuntimeData().selectedPower;
    const auto formID = std::get<uint32_t>(a_args[0]);
    return power && (power->GetFormID() == formID);
}

bool IsWorn(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t)
{
    // IsWorn(Form item)
    // Is the actor wearing the specified item?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsWorn({:08X})", a_args[0]);
#endif 
    (void)(a_args);

    // Get the actor's inventory.
    if (const auto invChanges = a_actor->GetInventoryChanges()) {
        if (const auto entryList = invChanges->entryList) {
            for (const auto entry : *entryList) {
                return entry && entry->IsWorn();
            }
        }
    }

    return false;
}

bool IsWornHasKeyword(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsWornHasKeyword(Keyword keyword)
    // Is the actor wearing anything with the specified keyword?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsWornHasKeyword({:08X})", a_args[0]);
#endif

    // Get the actor's inventory.
    if (const auto invChanges = a_actor->GetInventoryChanges()) {
        if (const auto entryList = invChanges->entryList) {
            for (const auto entry : *entryList) {
                if (entry && hasKeywordBoundObj(entry->object, a_args)) {
                    return entry->IsWorn();
                }
            }
        }
    }

    return false;
}

bool IsFemale(RE::Actor* a_actor)
{
    // IsFemale()
    // Is the actor female?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsFemale()");
#endif

    const auto base = a_actor->GetActorBase();
    return base && base->IsFemale();
}

bool Is_Child(RE::Actor* a_actor)
{
    // IsChild()
    // Is the actor a child?
    // Need to name this function Is_Child rather than IsChild to avoid
    // clobbering Windows IsChild function defined in WinUser.h
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsChild()");
#endif

    return a_actor->IsChild();
}

bool IsPlayerTeammate(RE::Actor* a_actor)
{
    // IsPlayerTeammate()
    // Is the actor currently a teammate of the player?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsPlayerTeammate()");
#endif

    return a_actor->IsPlayerTeammate();
}

bool IsInInterior(RE::Actor* a_actor)
{
    // IsInInterior()
    // Is the actor in an interior cell ?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsInInterior()");
#endif

    const auto cell = a_actor->GetParentCell();
    return cell->IsInteriorCell();
}

bool IsInFaction(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsInFaction(Faction faction)
    // Is the actor in the specified faction?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsInFaction({:08X})", a_args[0]);
#endif

    const auto formID = std::get<uint32_t>(a_args[0]);
    const auto faction = RE::TESForm::LookupByID<RE::TESFaction>(formID);
    return faction && a_actor->IsInFaction(faction);
}

bool HasKeyword(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // HasKeyword(Keyword keyword)
    // Does the actor have the specified keyword?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("HasKeyword({:08X})", a_args[0]);
#endif

    const auto formID = std::get<uint32_t>(a_args[0]);
    const auto keyword = RE::TESForm::LookupByID<RE::BGSKeyword>(formID);
    return keyword && a_actor->HasKeyword(keyword);
}

bool HasMagicEffect(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // HasMagicEffect(MagicEffect magiceffect)
    // Is the actor currently being affected by the given Magic Effect?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("HasMagicEffect({:08X})", a_args[0]);
#endif

    const auto formID = std::get<uint32_t>(a_args[0]);
    const auto effect = RE::TESForm::LookupByID<RE::EffectSetting>(formID);
    const auto target = a_actor->GetMagicTarget();
    return effect && target && target->HasMagicEffect(effect);
}

bool HasMagicEffectWithKeyword(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // HasMagicEffectWithKeyword(Keyword keyword)
    // Is the actor currently being affected by a Magic Effect with the given Keyword?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("HasMagicEffectWithKeyword({:08X})", a_args[0]);
#endif

    const auto formID = std::get<uint32_t>(a_args[0]);
    const auto keyword = RE::TESForm::LookupByID<RE::BGSKeyword>(formID);
    const auto target = a_actor->GetMagicTarget();
    return keyword && target && RE::MagicTarget_HasMagicEffectWithKeyword(target, keyword, 0);
}

bool HasPerk(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // HasPerk(Perk perk)
    // Does the actor have the given Perk ?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("HasPerk({:08X})", a_args[0]);
#endif

    const auto formID = std::get<uint32_t>(a_args[0]);
    const auto perk = RE::TESForm::LookupByID<RE::BGSPerk>(formID);
    return perk && a_actor->HasPerk(perk);
}

bool HasSpell(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // HasSpell(Form spell)
    // Does the actor have the given Spell or Shout?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("HasSpell({:08X})", a_args[0]);
#endif

    const auto formID = std::get<uint32_t>(a_args[0]);
    if (const auto form = RE::TESForm::LookupByID(formID)) {
        if (form->Is(RE::FormType::Spell))
            return a_actor->HasSpell(form->As<RE::SpellItem>());
        if (form->Is(RE::FormType::Shout))
            return RE::Actor_HasShout(a_actor, form->As<RE::TESShout>());
    }

    return false;
}

bool IsActorValueEqualTo(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsActorValueEqualTo(GlobalVariable id, GlobalVariable value)
    // Is the ActorValue of the specified ID equal to the value?
    // Temporarily disabled because it's crashing.
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorValueEqualTo({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    const auto owner = a_actor->AsActorValueOwner();
    const auto value = owner->GetActorValue(static_cast<RE::ActorValue>(args[0]));
    return (value == args[1]);
}

bool IsActorValueLessThan(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsActorValueLessThan(GlobalVariable id, GlobalVariable value)
    // Is the ActorValue of the specified ID less than the value?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorValueLessThan({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    const auto owner = a_actor->AsActorValueOwner();
    const auto value = owner->GetActorValue(static_cast<RE::ActorValue>(args[0]));
    return (value < args[1]);
}

bool IsActorValueBaseEqualTo(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsActorValueBaseEqualTo(GlobalVariable id, GlobalVariable value)
    // Is the base ActorValue of the specified ID equal to the value?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorValueBaseEqualTo({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    const auto owner = a_actor->AsActorValueOwner();
    const auto value = owner->GetBaseActorValue(static_cast<RE::ActorValue>(args[0]));
    return (value == args[1]);
}

bool IsActorValueBaseLessThan(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsActorValueBaseLessThan(GlobalVariable id, GlobalVariable value)
    // Is the base ActorValue of the specified ID less than the value?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorValueBaseLessThan({:08X}, {:08X})", args[0], args[1]);
#endif

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    const auto owner = a_actor->AsActorValueOwner();
    const auto value = owner->GetBaseActorValue(static_cast<RE::ActorValue>(args[0]));
    return (value < args[1]);
}

bool IsActorValueMaxEqualTo(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsActorValueMaxEqualTo(GlobalVariable id, GlobalVariable value)
    // Is the max ActorValue of the specified ID equal to the value?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorValueMaxEqualTo({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    const auto owner = a_actor->AsActorValueOwner();
    const auto value = owner->GetPermanentActorValue(static_cast<RE::ActorValue>(args[0]));
    return (value == args[1]);
}

bool IsActorValueMaxLessThan(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsActorValueMaxLessThan(GlobalVariable id, GlobalVariable value)
    // Is the max ActorValue of the specified ID less than the value?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorValueMaxLessThan({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    const auto owner = a_actor->AsActorValueOwner();
    const auto value = owner->GetPermanentActorValue(static_cast<RE::ActorValue>(args[0]));
    return (value < args[1]);
}

bool IsActorValuePercentageEqualTo(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsActorValuePercentageEqualTo(GlobalVariable id, GlobalVariable value)
    // Is the percentage ActorValue of the specified ID equal to the value?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorValuePercentageEqualTo({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    const auto value = getActorValPct(a_actor, static_cast<uint32_t>(args[0]));
    return (value == args[1]);
}

bool IsActorValuePercentageLessThan(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsActorValuePercentageLessThan(GlobalVariable id, GlobalVariable value)
    // Is the percentage ActorValue of the specified ID less than the value?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorValuePercentageLessThan({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    const auto value = getActorValPct(a_actor, static_cast<uint32_t>(args[0]));
    return (value < args[1]);
}

bool IsLevelLessThan(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsLevelLessThan(GlobalVariable level)
    // Is the actor's current level less than the specified level?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsLevelLessThan({:08X})", a_args[0]);
#endif

    float arg;
    if (!readGlobalVars(&arg, a_args, a_bmArgIsFloat, 1)) {
        return false;
    }

    const auto level = a_actor->GetLevel();
    return (level < arg);
}

bool IsActorBase(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsActorBase(ActorBase actorbase)
    // Is the actorbase for the actor the specified actorbase?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsActorBase({:08X})", a_args[0]);
#endif

    const auto base = a_actor->GetActorBase();
    const auto formID = std::get<uint32_t>(a_args[0]);
    return base && (base->GetFormID() == formID);
}

bool IsRace(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsRace(Race race)
    // Is the actor's race the specified race?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsRace({:08X})", a_args[0]);
#endif

    const auto race = a_actor->GetRace();
    const auto formID = std::get<uint32_t>(a_args[0]);
    return race && (race->GetFormID() == formID);
}

bool CurrentWeather(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // CurrentWeather(Weather weather)
    // Is the current weather the specified weather?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("CurrentWeather({:08X})", a_args[0]);
#endif
    (void)(a_actor);

    if (const auto sky = RE::Sky::GetSingleton()) {
        if (const auto weather = sky->currentWeather) {
            const auto formID = std::get<uint32_t>(a_args[0]);
            if (weather->GetFormID() == formID) {
                return true;
            }
        }
    }

    return false;
}

bool CurrentGameTimeLessThan(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // CurrentGameTimeLessThan(GlobalVariable time)
    // Is the current game time less than the specified time?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("CurrentGameTimeLessThan({:08X})", a_args[0]);
#endif
    (void)(a_actor);

    float arg;
    if (!readGlobalVars(&arg, a_args, a_bmArgIsFloat, 1)) {
        return false;
    }

    if (const auto calendar = RE::Calendar::GetSingleton()) {
        const float gameDaysPassed = calendar->GetCurrentGameTime();
        // This gives the fractional part of the game time, i.e. proportion of the current
        // day that has passed. We then multiply it by the number of game hours per day
        // to get the current time of the day:

        float f = 0.0;
        const auto pctCurDayPassed = std::modff(gameDaysPassed, &f);
        return arg > (pctCurDayPassed * RE::Calendar_GetHoursPerDay());
    }

    return false;
}

bool ValueEqualTo(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // ValueEqualTo(GlobalVariable value1, GlobalVariable value2)
    // Is the value1 equal to the value2?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("ValueEqualTo({:08X}, {:08X})", a_args[0], a_args[1]);
#endif
    (void)(a_actor);

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    return (args[0] == args[1]);
}

bool ValueLessThan(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // ValueLessThan(GlobalVariable value1, GlobalVariable value2)
    // Is the value1 less than the value2?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("ValueLessThan({:08X}, {:08X})", a_args[0], a_args[1]);
#endif
    (void)(a_actor);

    float args[2];
    if (!readGlobalVars(args, a_args, a_bmArgIsFloat, 2)) {
        return false;
    }

    return (args[0] < args[1]);
}

bool Random(RE::Actor*, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // Random(GlobalVariable percentage)
    // The probability of the specified percentage (from 0 to 1).
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("Random({:08X})", a_args[0]);
#endif

    float arg;
    if (!readGlobalVars(&arg, a_args, a_bmArgIsFloat, 1)) {
        return false;
    }

    // Some sanity checks before we proceed...
    if (arg < 0.0 || arg > 1.0) {
        // Invalid values. Just return false.
        return false;
    } else if (arg == 1.0) {
        // Just in case, given our random number generator
        // below will never return 1. Although calling this
        // function with a pct of 1, although valid would be
        // silly as then we have certainty and the condition is
        // thus completely redundant.
        return true;
    }

    // Ok, continue.
    // Generate a random number in the range [0, 1).
    std::random_device device;
    std::mt19937 gen(device());
    float prob = std::generate_canonical<float, 10>(gen);

    return (arg > prob);
}

bool IsUnique(RE::Actor* a_actor)
{
    // IsUnique()
    // Is the actor flagged as unique?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsUnique()");
#endif

    const auto base = a_actor->GetActorBase();
    return base && base->IsUnique();
}

bool IsClass(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsClass(Class class)
    // Is the actor's class the specified class?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsClass({:08X})", a_args[0]);
#endif

    if (const auto base = a_actor->GetActorBase()) {
        if (const auto cls = base->npcClass) {
            const auto formID = std::get<uint32_t>(a_args[0]);
            if (cls->GetFormID() == formID) {
                return true;
            }
        }
    }

    return false;
}

bool IsCombatStyle(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsCombatStyle(CombatStyle combatStyle)
    // Is the actor's CombatStyle the specified CombatStyle?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsCombatStyle({:08X})", a_args[0]);
#endif

    if (const auto base = a_actor->GetActorBase()) {
        if (const auto style = base->combatStyle) {
            const auto formID = std::get<uint32_t>(a_args[0]);
            if (style->GetFormID() == formID) {
                return true;
            }
        }
    }

    return false;
}

bool IsVoiceType(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsVoiceType(VoiceType voiceType)
    // Is the actor's VoiceType the specified VoiceType?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsVoiceType({:08X})", a_args[0]);
#endif

    if (const auto base = a_actor->GetActorBase()) {
        if (const auto type = base->voiceType) {
            const auto formID = std::get<uint32_t>(a_args[0]);
            if (type->GetFormID() == formID) {
                return true;
            }
        }
    }

    return false;
}

bool IsAttacking(RE::Actor* a_actor)
{
    // IsAttacking()
    // Is the actor currently attacking?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsAttacking()");
#endif

    return a_actor->IsAttacking();
}

bool IsRunning(RE::Actor* a_actor)
{
    // IsRunning()
    // Is the actor currently running?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsRunning()");
#endif

    return a_actor->IsRunning();
}

bool IsSneaking(RE::Actor* a_actor)
{
    // IsSneaking()
    // Is the actor currently sneaking ?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsSneaking()");
#endif

    return a_actor->IsSneaking();
}

bool IsSprinting(RE::Actor* a_actor)
{
    // IsSprinting()
    // Is the actor currently sprinting?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsSprinting()");
#endif

    return a_actor->AsActorState()->IsSprinting();
}

bool IsInAir(RE::Actor* a_actor)
{
    // IsInAir()
    // Is the actor in the air?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsInAir()");
#endif

    return a_actor->AsActorState()->IsFlying();
}

bool IsInCombat(RE::Actor* a_actor)
{
    // IsInCombat()
    // Is the actor in combat?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsInCombat()");
#endif

    return a_actor->IsInCombat();
}

bool IsWeaponDrawn(RE::Actor* a_actor)
{
    // IsWeaponDrawn()
    // Does the actor have his equipped weapon and/or magic spell drawn?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsWeaponDrawn()");
#endif

    return a_actor->AsActorState()->IsWeaponDrawn();
}

bool IsInLocation(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsInLocation(Location location)
    // Is the actor in the specified location or a child of that location?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsInLocation({:08X})", a_args[0]);
#endif

    const auto formID = std::get<uint32_t>(a_args[0]);
    if (const auto location = RE::TESForm::LookupByID<RE::BGSLocation>(formID)) {
        if (const auto curLocation = a_actor->GetCurrentLocation()) {
            if (location == curLocation)
                return true;

            return location->IsChild(curLocation);
        }
    }

    return false;
}

bool HasRefType(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // HasRefType(LocationRefType refType)
    // Does the actor have the specified LocationRefType attached?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("HasRefType({:08X})", a_args[0]);
#endif

    const auto formID = std::get<uint32_t>(a_args[0]);
    if (const auto locRefType = RE::TESForm::LookupByID<RE::BGSLocationRefType>(formID)) {
        const auto extra = a_actor->extraList.GetByType<RE::ExtraLocationRefType>();
        return extra && (extra->locRefType == locRefType);
    }

    return false;
}

bool IsParentCell(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsParentCell(Cell cell)
    // Is the actor in the specified cell?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsParentCell({:08X})", a_args[0]);
#endif

    const auto cell = a_actor->GetParentCell();
    const auto formID = std::get<uint32_t>(a_args[0]);
    return cell && (cell->GetFormID() == formID);
}

bool IsWorldSpace(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args)
{
    // IsWorldSpace(WorldSpace worldSpace)
    // Is the actor in the specified WorldSpace?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsWorldSpace({:08X})", a_args[0]);
#endif
    const auto worldspace = a_actor->GetWorldspace();
    const auto formID = std::get<uint32_t>(a_args[0]);
    return worldspace && (worldspace->GetFormID() == formID);
}

bool IsFactionRankEqualTo(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsFactionRankEqualTo(GlobalVariable rank, Faction faction)
    // Is the actor's rank in the specified faction equal to the specified rank?
    //
    // From the DAR documentation:
    //   "The actor's rank in the specified faction:
    //      => -2 if the Actor is not in the faction.
    //      => -1 if the Actor is in the faction, with a rank set to -1.
    //            (By convention, this means they may eventually become a member
    //             of this faction.)
    //      => A non-negative number equal to the actor's rank in the faction."
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsFactionRankEqualTo({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float arg;
    if (!readGlobalVars(&arg, a_args, a_bmArgIsFloat, 1)) {
        return false;
    }

    const auto formID = std::get<uint32_t>(a_args[1]);
    const auto faction = RE::TESForm::LookupByID<RE::TESFaction>(formID);
    if (!faction || !a_actor->IsInFaction(faction)) {
        return false;
    }

    const bool isPlayer = a_actor->IsPlayer();
    const auto rank = RE::Actor_GetFactionRank(a_actor, faction, isPlayer);
    return (static_cast<float>(rank) == arg);
}

bool IsFactionRankLessThan(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsFactionRankLessThan(GlobalVariable rank, Faction faction)
    // Is the actor's rank in the specified faction less than the specified rank?
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsFactionRankLessThan({:08X}, {:08X})", a_args[0], a_args[1]);
#endif

    float arg;
    if (!readGlobalVars(&arg, a_args, a_bmArgIsFloat, 1)) {
        return false;
    }

    const auto formID = std::get<uint32_t>(a_args[1]);
    const auto faction = RE::TESForm::LookupByID<RE::TESFaction>(formID);
    if (!faction || !a_actor->IsInFaction(faction)) {
        return false;
    }

    const bool isPlayer = a_actor->IsPlayer();
    const auto rank = RE::Actor_GetFactionRank(a_actor, faction, isPlayer);
    return (static_cast<float>(rank) < arg);
}

bool IsMovementDirection(RE::Actor* a_actor, std::variant<uint32_t, float>* a_args, uint32_t a_bmArgIsFloat)
{
    // IsMovementDirection(GlobalVariable direction)
    // Is the actor moving in the specified direction?
    //
    // From the DAR documentation:
    //   "Movement direction:
    //      => 0 = Standing Still
    //      => 1 = Forward
    //      => 2 = Right
    //      => 3 = Back
    //      => 4 = Left"
#ifdef DEBUG_TRACE_CONDITIONS
    logger::info("IsMovementDirection({:08X})", a_args[0]);
#endif

    float arg;
    if (!readGlobalVars(&arg, a_args, a_bmArgIsFloat, 1)) {
        return false;
    }

    float dir = 0.0;
    if (RE::Actor_IsMoving(a_actor)) {
        double angle = RE::Actor_GetMoveDirectionRelativeToFacing(a_actor);
        // Ensure movement angle (in radians) is within the range [0, 2*PI]
        for (; angle < 0.0; angle += TWO_PI);
        for (; angle > TWO_PI; angle -= TWO_PI);

        // Get nearest boundary of circular quadrant containing this angle.
        // Ex. 1: If movement angle is 0.4 * PI (i.e. in 1st quadrant, closest
        //        to 0.5*PI boundary), first calc evaluates to:
        //         y = ((0.4*PI / 2*PI) + 1/8) * 4.0 / 4.0
        //           = [(0.2 + 0.125) * 4.0] / 4.0
        //           = fmodf(1.3, 4.0) = 1.3 - (1.3 / 4.0 = 0.325 => 0) * 4 = 1.3
        //        Second calc then evaluates as 1.0 + 1.0 = 2 (Right)
        // Ex. 2. For movement angle 1.1 * PI (i.e. in 3rd quadrant, closest
        //        to PI boundary), calcs evaluate to:
        //        [1]:  y = ((1.1*PI / 2*PI) + 1/8) * 4.0 / 4.0
        //                = fmodf(2.7, 4.0) = 2.7 - (2.7 / 4.0 = 0.675 => 0) * 4 = 2.7
        //        [2]:  2.0 + 1.0 = 3 (Back).
        // As expected.
        float y = std::fmodf((static_cast<float>(angle / TWO_PI) + 0.125f) * 4.0f, 4.0f);
        dir = std::floorf(y) + 1.0f;
    }

    return (dir == arg);
}

// The DAR defined actor state functions
// These all test an actor for a specific state and return TRUE or FALSE.
std::vector<std::pair<std::string, FuncInfo>> v
{
    // ==============================================================================================
    // Function Name                           {  address of function,      nArgs, argTypeMask  }
    // ----------------------------------------------------------------------------------------------
    { "IsEquippedRight",                FuncInfo{ &IsEquippedRight,                1, 0 } },  // 0
    { "IsEquippedRightType",            FuncInfo{ &IsEquippedRightType,            1, 1 } },  // 1
    { "IsEquippedRightHasKeyword",      FuncInfo{ &IsEquippedRightHasKeyword,      1, 0 } },  // 2
    { "IsEquippedLeft",                 FuncInfo{ &IsEquippedLeft,                 1, 0 } },  // 3
    { "IsEquippedLeftType",             FuncInfo{ &IsEquippedLeftType,             1, 1 } },  // 4
    { "IsEquippedLeftHasKeyword",       FuncInfo{ &IsEquippedLeftHasKeyword,       1, 0 } },  // 5
    { "IsEquippedShout",                FuncInfo{ &IsEquippedShout,                1, 0 } },  // 6
    { "IsWorn",                         FuncInfo{ &IsWorn,                         1, 0 } },  // 7
    { "IsWornHasKeyword",               FuncInfo{ &IsWornHasKeyword,               1, 0 } },  // 8
    { "IsFemale",                       FuncInfo{ &IsFemale,                       0, 0 } },  // 9
    { "IsChild",                        FuncInfo{ &Is_Child,                       0, 0 } },  // 10
    { "IsPlayerTeammate",               FuncInfo{ &IsPlayerTeammate,               0, 0 } },  // 11
    { "IsInInterior",                   FuncInfo{ &IsInInterior,                   0, 0 } },  // 12
    { "IsInFaction",                    FuncInfo{ &IsInFaction,                    1, 0 } },  // 13
    { "HasKeyword",                     FuncInfo{ &HasKeyword,                     1, 0 } },  // 14
    { "HasMagicEffect",                 FuncInfo{ &HasMagicEffect,                 1, 0 } },  // 15
    { "HasMagicEffectWithKeyword",      FuncInfo{ &HasMagicEffectWithKeyword,      1, 0 } },  // 16
    { "HasPerk",                        FuncInfo{ &HasPerk,                        1, 0 } },  // 17
    { "HasSpell",                       FuncInfo{ &HasSpell,                       1, 0 } },  // 18
    { "IsActorValueEqualTo",            FuncInfo{ &IsActorValueEqualTo,            2, 3 } },  // 19
    { "IsActorValueLessThan",           FuncInfo{ &IsActorValueLessThan,           2, 3 } },  // 20
    { "IsActorValueBaseEqualTo",        FuncInfo{ &IsActorValueBaseEqualTo,        2, 3 } },  // 21
    { "IsActorValueBaseLessThan",       FuncInfo{ &IsActorValueBaseLessThan,       2, 3 } },  // 22
    { "IsActorValueMaxEqualTo",         FuncInfo{ &IsActorValueMaxEqualTo,         2, 3 } },  // 23
    { "IsActorValueMaxLessThan",        FuncInfo{ &IsActorValueMaxLessThan,        2, 3 } },  // 24
    { "IsActorValuePercentageEqualTo",  FuncInfo{ &IsActorValuePercentageEqualTo,  2, 3 } },  // 25
    { "IsActorValuePercentageLessThan", FuncInfo{ &IsActorValuePercentageLessThan, 2, 3 } },  // 26
    { "IsLevelLessThan",                FuncInfo{ &IsLevelLessThan,                1, 1 } },  // 27
    { "IsActorBase",                    FuncInfo{ &IsActorBase,                    1, 0 } },  // 28
    { "IsRace",                         FuncInfo{ &IsRace,                         1, 0 } },  // 29
    { "CurrentWeather",                 FuncInfo{ &CurrentWeather,                 1, 0 } },  // 30
    { "CurrentGameTimeLessThan",        FuncInfo{ &CurrentGameTimeLessThan,        1, 1 } },  // 31
    { "ValueEqualTo",                   FuncInfo{ &ValueEqualTo,                   2, 3 } },  // 32
    { "ValueLessThan",                  FuncInfo{ &ValueLessThan,                  2, 3 } },  // 33
    { "Random",                         FuncInfo{ &Random,                         1, 1 } },  // 34
    { "IsUnique",                       FuncInfo{ &IsUnique,                       0, 0 } },  // 35
    { "IsClass",                        FuncInfo{ &IsClass,                        1, 0 } },  // 36
    { "IsCombatStyle",                  FuncInfo{ &IsCombatStyle,                  1, 0 } },  // 37
    { "IsVoiceType",                    FuncInfo{ &IsVoiceType,                    1, 0 } },  // 38
    { "IsAttacking",                    FuncInfo{ &IsAttacking,                    0, 0 } },  // 39
    { "IsRunning",                      FuncInfo{ &IsRunning,                      0, 0 } },  // 40
    { "IsSneaking",                     FuncInfo{ &IsSneaking,                     0, 0 } },  // 41
    { "IsSprinting",                    FuncInfo{ &IsSprinting,                    0, 0 } },  // 42
    { "IsInAir",                        FuncInfo{ &IsInAir,                        0, 0 } },  // 43
    { "IsInCombat",                     FuncInfo{ &IsInCombat,                     0, 0 } },  // 44
    { "IsWeaponDrawn",                  FuncInfo{ &IsWeaponDrawn,                  0, 0 } },  // 45
    { "IsInLocation",                   FuncInfo{ &IsInLocation,                   1, 0 } },  // 46
    { "HasRefType",                     FuncInfo{ &HasRefType,                     1, 0 } },  // 47
    { "IsParentCell",                   FuncInfo{ &IsParentCell,                   1, 0 } },  // 48
    { "IsWorldSpace",                   FuncInfo{ &IsWorldSpace,                   1, 0 } },  // 49
    { "IsFactionRankEqualTo",           FuncInfo{ &IsFactionRankEqualTo,           2, 1 } },  // 50
    { "IsFactionRankLessThan",          FuncInfo{ &IsFactionRankLessThan,          2, 1 } },  // 51
    { "IsMovementDirection",            FuncInfo{ &IsMovementDirection,            1, 1 } },  // 52
    // ==============================================================================================
};

std::unordered_map<std::string, FuncInfo> g_DARConditionFuncs(v.begin(), v.end());
