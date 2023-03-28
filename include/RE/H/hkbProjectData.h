// ============================================================================
//                         hkbProjectData.h
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
// For further info on Havok, see
//   http://web.archive.org/web/http://anarchy.cn/manual/12/HavokSdk_ProgrammersManual/index.html
//   https://github.com/Bewolf2/projectanarchy

#pragma once

#include "RE/H/hkRefPtr.h"
#include "RE/H/hkVector4.h"

namespace RE
{
	class hkbProjectStringData;

	class hkbProjectData : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkbProjectData;
		inline static constexpr auto VTABLE = VTABLE_hkbProjectData;

		// members
		hkVector4                      worldUpWS;         // 10
		hkRefPtr<hkbProjectStringData> stringData;        // 20
		std::uint8_t                   defaultEventNode;  // 28 (hkEnum<hkbTransitionEffect::EventMode, hkInt8>)
	};
	static_assert(sizeof(hkbProjectData) == 0x30);
}
