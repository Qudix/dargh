// ============================================================================
//                             DebugUtils.cpp
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

#include "DebugUtils.h"

void dumpBytes(char* a_ptr, std::size_t a_nbytes)
{
	// nbytes should be a multiple of 8
	// Print in form
	//    <address>:  <byte0> ... <byte7>   <asc0> ... <asc7>
	//                             ...
	if (!a_ptr) {
		logs::info("a_ptr is nullptr, can't dump bytes.");
		return;
	}

	std::string s1;
	std::string s2;
	char buf[64];
	for (int i = 0; i < a_nbytes; i += 8) {
		sprintf_s(buf, sizeof(buf), "%08x : ", stl::unrestricted_cast<uint32_t>(a_ptr));
		s1.assign(buf);
		s2 = "";
		for (int j = 0; j < 8; j++, a_ptr++) {
			sprintf_s(buf, sizeof(buf), " %02x", static_cast<uint8_t>(*a_ptr));
			s1.append(buf);
			if (*a_ptr > 31 && *a_ptr < 127) {
				sprintf_s(buf, sizeof(buf), " %c", static_cast<uint8_t>(*a_ptr));
				s2.append(buf);
			} else {
				s2 += "  ";
			}
		}

		s1 += s2;
		logs::info("{}", s1);
	}
}

void dumpHkArrayStringPtr(std::string a_header, RE::hkArray<RE::hkStringPtr> a_strings)
{
	if (!a_strings.empty()) {
		logs::info("{}: ({} strings)", a_header, a_strings.size());
		int32_t i{ 0 };
		for (auto& str : a_strings) {
			if (str.empty())
				logs::info("  [{}] <NULL PTR>", i++);
			else
				logs::info("  [{}] '{}'", i++, str.c_str());
		}
	}
}

void dumpHkArrayAssetBundle(std::string a_header, RE::hkArray<RE::hkbAssetBundleStringData> a_bundles)
{
	if (!a_bundles.empty()) {
		logs::info("{}: ({} asset bundles)", a_header, a_bundles.size());
		int32_t i{ 0 };
		for (auto& bundle : a_bundles) {
			logs::info("  [{}] '{}'", i++, bundle.bundleName.c_str());
		}
	}
}

void dumpHkArrayFileNameMeshNamePair(std::string a_header, RE::hkArray<RE::hkbCharacterStringData::FileNameMeshNamePair> a_pairs)
{
	if (!a_pairs.empty()) {
		logs::info("{}: ({} filename/meshname pairs)", a_header, a_pairs.size());
		int32_t i{ 0 };
		for (auto& pair : a_pairs) {
			logs::info("  [{}] ('{}', '{}')", i++, pair.fileName.c_str(), pair.meshName.c_str());
		}
	}
}

void dumpHkbCharacterStringData(RE::hkbCharacterStringData* a_data)
{
	logs::info("\n--------------------------------------------------------------");
	//logs::info("           fctor: hkbCharacterStringData object [{}]", fmt::ptr(a_data));
	logs::info("                         thread ID: {:08X}", REX::W32::GetCurrentThreadId());
	logs::info("-----------------------------------------------------------------");

	//	dumpBytes((char*)a_data, 192);

	dumpHkArrayFileNameMeshNamePair("deformableSkinNames", a_data->deformableSkinNames);
	dumpHkArrayFileNameMeshNamePair("rigidSkinNames", a_data->rigidSkinNames);
	//dumpHkArrayAssetBundle("animationNames", a_data->animationNames);
	dumpHkArrayAssetBundle("animationBundleFilenameData", a_data->animationBundleFilenameData);
	dumpHkArrayStringPtr("characterPropertyNames", a_data->characterPropertyNames);
	dumpHkArrayStringPtr("retargetingSkeletonMapperFilenames", a_data->retargetingSkeletonMapperFilenames);
	dumpHkArrayStringPtr("lodNames", a_data->lodNames);
	dumpHkArrayStringPtr("mirroredSyncPointSubstringsA", a_data->mirroredSyncPointSubstringsA);
	dumpHkArrayStringPtr("mirroredSyncPointSubstringsB", a_data->mirroredSyncPointSubstringsB);

	if (!a_data->name.empty())
		logs::info("name:             '{}'", a_data->name.c_str());

	if (!a_data->rigName.empty())
		logs::info("rigName:          '{}'", a_data->rigName.c_str());

	if (!a_data->ragdollName.empty())
		logs::info("ragdollName:      '{}'", a_data->ragdollName.c_str());

	if (!a_data->behaviorFilename.empty())
		logs::info("behaviorFilename: '{}'", a_data->behaviorFilename.c_str());
}

void dumpHkbProjectData(RE::hkbProjectData* a_data)
{
	logs::info("\n--------------------------------------------------------------");
	//logs::info("             fctor: hkbProjectData object [{}]", fmt::ptr(a_data));
	logs::info("                         thread ID: {:08X}", REX::W32::GetCurrentThreadId());
	logs::info("-----------------------------------------------------------------");

	//dumpBytes((char*)thisObj, 1024);

	auto& data = a_data->stringData;
	if (!data->animationPath.empty())
		logs::info("animationPath:    '{}'", data->animationPath.c_str());

	if (!data->behaviorPath.empty())
		logs::info("behaviorPath:     '{}'", data->behaviorPath.c_str());

	if (!data->characterPath.empty())
		logs::info("characterPath:    '{}'", data->characterPath.c_str());

	if (!data->scriptsPath.empty())
		logs::info("scriptsPath:      '{}'", data->scriptsPath.c_str());

	if (!data->fullPathToSource.empty())
		logs::info("fullPathToSource: '{}'", data->fullPathToSource.c_str());

	if (!data->rootPath.empty())
		logs::info("rootPath:         '{}'", data->rootPath.c_str());

	dumpHkArrayStringPtr("animationFilenames", data->animationFilenames);
	dumpHkArrayStringPtr("behaviorFilenames", data->behaviorFilenames);
	dumpHkArrayStringPtr("characterFilenames", data->characterFilenames);
	dumpHkArrayStringPtr("eventNames", data->eventNames);
}

void dumpHkbClipGenerator(RE::hkbClipGenerator* a_gen)
{
	logs::info("\n--------------------------------------------------------------");
	//logs::info("             hkbClipGenerator::Activate [{}]", fmt::ptr(a_gen));
	logs::info("                         thread ID: {:08X}", REX::W32::GetCurrentThreadId());
	logs::info("-----------------------------------------------------------------");

	//logs::info("flag.m_finishing = %d", flag.m_finishing);

	if (!a_gen->name.empty())
		logs::info("name:                         '{}'", a_gen->name.c_str());

	logs::info("id:                           {}", a_gen->id);

	if (!a_gen->animationName.empty())
		logs::info("animationName:                '{}'", a_gen->animationName.c_str());

	//if (!a_gen->triggers)
	//	logs::info("triggers:                     {}", fmt::ptr(a_gen->triggers.get()));

	//logs::info("cropStartAmountLocalTime:     {}", a_gen->cropStartAmountLocalTime);
	logs::info("cropEndAmountLocalTime:       {}", a_gen->cropEndAmountLocalTime);
	logs::info("startTime:                    {}", a_gen->startTime);
	logs::info("playbackSpeed:                {}", a_gen->playbackSpeed);
	logs::info("enforcedDuration:             {}", a_gen->enforcedDuration);
	logs::info("userControlledTimeFraction:   {}", a_gen->userControlledTimeFraction);
	logs::info("animationBindingIndex:        {}", a_gen->animationBindingIndex);

	using PlaybackMode = RE::hkbClipGenerator::PlaybackMode;
	switch (a_gen->mode.get()) {
	case PlaybackMode::kModeSinglePlay:
		logs::info("mode: kModeSinglePlay");
		break;
	case PlaybackMode::kModeLooping:
		logs::info("mode: kModeLooping");
		break;
	case PlaybackMode::kModeUserControlled:
		logs::info("mode: kModeUserControlled");
		break;
	case PlaybackMode::kModePingPong:
		logs::info("mode: kModePingPong");
		break;
	default:
		logs::info("mode: Unknown");
		break;
	}

	logs::info("flags:                  {}", a_gen->flags);
	logs::info("Passing control back to orig function...");
}
