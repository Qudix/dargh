// ============================================================================
//                          hkbCharacterSetup.h
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
// For further info on Havok and the Havok Animation Studio, see
//   http://web.archive.org/web/http://anarchy.cn/manual/12/HavokSdk_ProgrammersManual/index.html
//   https://github.com/Bewolf2/projectanarchy

#pragma once

#include "RE/H/hkArray.h"
#include "RE/H/hkRefPtr.h"

namespace RE
{
	class hkaMirroredSkeleton;
	class hkaSkeleton;
	class hkaSkeletonMapper;
	class hkbAnimationBindingSet;
	class hkbCharacterData;
	class hkbSymbolIdMap;
	class hkCriticalSection;

	class hkbCharacterSetup : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkbCharacterSetup;
		inline static constexpr auto VTABLE = VTABLE_hkbCharacterSetup;

		// members
		hkArray<hkRefPtr<const hkaSkeletonMapper>> retargetingSkeletonMappers;        // 10
		hkRefPtr<const hkaSkeleton>                animationSkeleton;                 // 20
		hkRefPtr<const hkaSkeletonMapper>          ragdollToAnimationSkeletonMapper;  // 28
		hkRefPtr<const hkaSkeletonMapper>          animationToRagdollSkeletonMapper;  // 30
		hkRefPtr<hkbAnimationBindingSet>           animationBindingSet;               // 38
		hkRefPtr<hkbCharacterData>                 data;                              // 40
		hkRefPtr<const hkaSkeleton>                unscaledAnimationSkeleton;         // 48
		mutable hkRefPtr<hkaMirroredSkeleton>      mirroredSkeleton;                  // 50
		hkRefPtr<hkbSymbolIdMap>                   characterPropertyIdMap;            // 58
		mutable hkCriticalSection*                 criticalSection;                   // 60
	};
	static_assert(sizeof(hkbCharacterSetup) == 0x68);
}
