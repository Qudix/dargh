// ============================================================================
//                          hkbCharacterData.h
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
#include "RE/H/hkReferencedObject.h"
#include "RE/H/hkVector4.h"
#include "RE/H/hkbCharacterControllerSetup.h"

namespace RE
{
	class hkbCharacterStringData;
	class hkbFootIkDriverInfo;
	class hkbHandIkDriverInfo;
	class hkbVariableInfo;
	class hkbVariableValueSet;

	class hkbCharacterData : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkbCharacterData;
		inline static constexpr auto VTABLE = VTABLE_hkbCharacterData;

		// members
		std::uint8_t                     characterControllerSetup[24];  // 10 (hkbCharacterControllerSetup)
		float                            modelUpMS[4];                  // 28 (hkVector4)
		float                            modelForwardMS[4];             // 38 (hkVector4)
		float                            modelRightMS[4];               // 48 (hkVector4)
		hkArray<hkbVariableInfo>         characterPropertyInfos;        // 58
		hkArray<std::int32_t>            numBonesPerLod;                // 68
		hkRefPtr<hkbVariableValueSet>    characterPropertyValues;       // 78
		hkRefPtr<hkbFootIkDriverInfo>    footIkDriverInfo;              // 80
		hkRefPtr<hkbHandIkDriverInfo>    handIkDriverInfo;              // 88
		hkRefPtr<hkReferencedObject>     aiControlDriverInfo;           // 90
		hkRefPtr<hkbCharacterStringData> stringData;                    // 98
	};
	static_assert(sizeof(hkbCharacterData) == 0xA0);  // B0?
}
