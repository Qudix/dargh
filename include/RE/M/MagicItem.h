// ============================================================================
//                              MagicItem.h
// ----------------------------------------------------------------------------
// Part of the open-source Dynamic Animation Replacer (DARGH).
// 
// Copyright (c) 2023 Nox Sidereum
// Copyright (c) 2018 Ryan - rsm - McKenzie
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
// Note from Nox Sidereum:
// 
//     In writing DARGH and reverse engineering DAR, I have attempted to be
//     consistent with CommonLibSSE structures and variable names. This
//     should facilitate any future porting of this code to 100% CommonLibSSE
//     (which I'd encourage).
// 
//     Many thanks to Ryan and the authors of CommonLibSSE.
// 
// This particular file is based on
//    https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/M/MagicItem.h
#pragma once
#include "RE/B/BGSKeywordForm.h"
#include "RE/E/EffectSetting.h"
#include "RE/T/TESBoundObject.h"
#include "RE/T/TESFullName.h"

struct MagicItem
{
	// Override:
	TESBoundObject    boundObj;                // 00 (public TESBoundObject)
	TESFullName       fullName;                // 30 (public TESFullName)
	BGSKeywordForm    bgsKeywordForm;          // 40 (public BGSKeywordForm)

	// Add:
	uint64_t          effects[3];		       // 58 (BSTArray<Effect*>)
	int32_t	          hostileCount;	           // 70
	uint32_t	      pad74;			       // 74
	EffectSetting*    avEffectSetting;         // 78
	uint32_t	      preloadCount;	           // 80
	uint32_t	      pad84;			       // 84
	uint64_t          preloadedItem;           // 88 (BSTSmartPointer<QueuedFile>)
};
static_assert(sizeof(MagicItem) == 0x90);