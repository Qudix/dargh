// ============================================================================
//                       hkbCharacterStringData.h
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
#include "RE/H/hkStringPtr.h"

namespace RE
{
	class hkbAssetBundleStringData;

	class hkbCharacterStringData : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkbCharacterStringData;
		inline static constexpr auto VTABLE = VTABLE_hkbCharacterStringData;

		struct FileNameMeshNamePair
		{
			hkStringPtr fileName;
			hkStringPtr meshName;
		};

		// members
		hkArray<FileNameMeshNamePair>     deformableSkinNames;                 // 10
		hkArray<FileNameMeshNamePair>     rigidSkinNames;                      // 20
		hkArray<hkbAssetBundleStringData> animationNames;                      // 30
		hkArray<hkbAssetBundleStringData> animationBundleFilenameData;         // 40 - unused by Skyrim?
		hkArray<hkStringPtr>              characterPropertyNames;              // 50
		hkArray<hkStringPtr>              retargetingSkeletonMapperFilenames;  // 60
		hkArray<hkStringPtr>              lodNames;                            // 70
		hkArray<hkStringPtr>              mirroredSyncPointSubstringsA;        // 80
		hkArray<hkStringPtr>              mirroredSyncPointSubstringsB;        // 90
		hkStringPtr                       name;                                // A0
		hkStringPtr                       rigName;                             // A8
		hkStringPtr                       ragdollName;                         // B0
		hkStringPtr                       behaviorFilename;                    // B8
	};
	static_assert(sizeof(hkbCharacterStringData) == 0xC0);
}
