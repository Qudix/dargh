// ============================================================================
//                               Main.cpp
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

#include "Hooks.h"
#include "Trampolines.h"
#include "Plugin.h"
#include "Utilities.h"

void ProcessDARINIFile()
{
	char value[256];

	// DAR INI path.
	const auto darINIPath{ "data\\skse\\plugins\\DynamicAnimationReplacer.ini" };

	// Attempt to open it and read its contents.
	//
	// N.B. DAR appears to use GetPrivateProfileSectionA, create a string
	// vector from the null-delimited results and then iterate over that
	// to find the value for "AnimationLimit", if it exists. No other
	// keys are used. Given we are only interested in the one key, we
	// instead just use GetPrivateProfileStringA.
	static_cast<void>(REX::W32::GetPrivateProfileStringA("Main", "AnimationLimit", 0, value, 256, darINIPath));

	// Print this just to be completely consistent with DAR output :-)
	logs::info("Main");
	if (std::strcmp(value, "")) {
		int iMaxAnimFiles = std::stoi(value, nullptr, 0);
		if (iMaxAnimFiles >= 0) {
			Plugin::MAX_ANIMATION_FILES = iMaxAnimFiles;
			logs::info("  > AnimationLimit  =  {}", iMaxAnimFiles);
		}
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(128);

	ProcessDARINIFile();

	logs::info("Installing hooks and trampolines");
	if (!install_hooks() || !install_trampolines())
		return false;

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(Plugin::HandleSKSEMessage);

	return true;
}
