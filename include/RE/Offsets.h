// ============================================================================
//                              Offsets.h
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

#pragma once

namespace RE::Offset
{
	/*
	extern uint64_t Unk00;
	extern uint64_t hkbClipGenerator_Generate;
	extern uint64_t g_AnimationFileManager;
	extern uint64_t QueuedRef_Unk;
	extern uint64_t TESObjectREFR_Unk;
	extern uint64_t WeaponAnimGraphMgrHolder_Unk;
	extern uint64_t SimpleAnimGraphMgrLoadingTask_Unk;

	typedef uint64_t (* _AnimationFileManager_Load)(uint64_t, hkbContext*, hkbClipGenerator*, uint64_t);
	extern _AnimationFileManager_Load AnimationFileManager_Load;
	*/

	/*
	std::vector<std::tuple<void*, uint64_t, uint64_t>> skyrim_addr
	{
		// 2 trampolines: ----------------
		{ &Unk00,                                        63846,       257  },     //   7  [0x00b371c0] => 0x00b371c0 + 0x101 = 0x00B372C1
		{ &hkbClipGenerator_Generate,                    59253,       128  },     //   8  [0x00a42e50] => 0x00a42e50 + 0x80 = 0x00A42ED0
		// -------------------------------

		// Unused by DARGH; used by DAR (see entry 41 below):
		{ &g_AnimationFileManager,                      407512,         0  },     //   9  [0x02fe6d60]

		// 4 (unused) trampolines: ------
		{ &QueuedRef_Unk,                                13034,       199  },     //  10  [0x001507b0] => 0x001507b0 + 0xc7 =  0x00150877
		{ &TESObjectREFR_Unk,                            20103,       274  },     //  11  [0x002b6cf0] => 0x002b6cf0 + 0x112 = 0x002B6E02
		{ &WeaponAnimGraphMgrHolder_Unk,                 32892,       313  },     //  12  [0x0050bc10] => 0x0050bc10 + 0x139 = 0x0050BD49
		{ &SimpleAnimGraphMgrLoadingTask_Unk,            32918,       281  },     //  13  [0x0050cdd0] => 0x0050cdd0 + 0x119 = 0x0050CEE9
		// ------------------------------

		// Used for evaluating conditions for conditional dynamic animations: ----
		{ &MagicTarget_HasMagicEffect,                   34517,         0  },     //  22  [0x00575370]
		{ &MagicTarget_HasMagicEffectWithKeyword,        34518,         0  },     //  23  [0x00575400]
		{ &Actor_HasPerk,                                37698,         0  },     //  24  [0x00632b00]
		{ &Actor_HasSpell,                               38782,         0  },     //  25  [0x0066a950]
		{ &Actor_HasShout,                               38783,         0  },     //  26  [0x0066a990]
		{ &Actor_GetLevel,                               37334,         0  },     //  27  [0x0060ca10]
		{ &g_theCalendar,                               400447,         0  },     //  28  [0x01f54780]
		{ &Calendar_GetCurrentGameTime,                  36303,         0  },     //  29  [0x005d96e0]
		{ &g_gameHoursPerGameDay,                       195681,         0  },     //  30  [0x0168f064]
		{ &Actor_IsRunning,                              37234,         0  },     //  32  [0x00607590]
		{ &Actor_IsOnMount,                              17965,         0  },     //  33  [0x00240740]
		{ &Actor_IsInAir,                                37243,         0  },     //  34  [0x00607ae0]
		{ &ObjectReference_GetCurrentLocation,           19812,         0  },     //  35  [0x002ad8e0]
		{ &TESObjectREFR_GetWorldSpace,                  19816,         0  },     //  36  [0x002adb70]
		{ &Actor_GetFactionRank,                         37676,         0  },     //  37  [0x006305b0]
		{ &Actor_IsMoving,                               37953,         0  },     //  39  [0x00642130]
		{ &Actor_GetMoveDirRelToFacing,                  37960,         0  },     //  40  [0x00642340]

		// Use this to avoid deriving it via g_AnimFileManager VFT (which is what DAR does):
		{ &AnimationFileManager_Load,                    63982,         0  }      //  41  [0x00B40D10]
		// ====================================================================================================
	};*/
}
