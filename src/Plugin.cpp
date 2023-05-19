// ============================================================================
//                               Plugin.cpp
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

#include "Plugin.h"
#include "DARProjectRegistry.h"
#include "DARProject.h"
#include "Utilities.h"

namespace Plugin
{
	void HandleSKSEMessage(SKSE::MessagingInterface::Message* a_msg)
	{
		if (a_msg->type != SKSE::MessagingInterface::kDataLoaded)
			return;

		// --------------------------------------------------------------------
		//  1. Get the data handler.
		// --------------------------------------------------------------------
		const auto dh = RE::TESDataHandler::GetSingleton();
		if (!dh) {
			logs::error("couldn't get TESDataHandler");
			return;
		}

		// --------------------------------------------------------------------
		//  2. Register two projects (male & female) for each loaded race.
		// --------------------------------------------------------------------
		const auto& races = dh->GetFormArray<RE::TESRace>();
		for (const auto race : races) {
			if (race) {
				// First for the males, second for the females.
				// E.g.
				//    [0] => 'Actors\Character\DefaultMale.hkx'
				//    [1] => 'Actors\Character\DefaultFemale.hkx'

				for (auto& graph : race->behaviorGraphs) {
					if (const auto model = graph.GetModel()) {
						DARGH::registerDARProject(model);
					}
				}
			}
		}

		// --------------------------------------------------------------------
		//  3. Register the 1st person project specified in Skyrim's INI file.
		// --------------------------------------------------------------------
		const auto ini = RE::INISettingCollection::GetSingleton();
		if (const auto setting = ini->GetSetting("strPlayerCharacterBehavior1stPGraph:Animation")) {
			if (const auto name = setting->GetString()) {
				DARGH::registerDARProject(name);
			}
		}

		// --------------------------------------------------------------------
		//  4. Load applicable DAR mappings for the registered projects.
		// --------------------------------------------------------------------
		for (auto& [path, proj] : DARGH::g_DARProjectRegistry) {
			auto dir = fmt::format("data\\meshes\\{}\\animations\\DynamicAnimationReplacer", proj.projFolder);

			// ... i.e.
			//     "data\meshes\actors\(project folder)\
			//        animations\DynamicAnimationReplacer"

			DARGH::loadDARMaps_ActorBase(proj, dir);
			DARGH::loadDARMaps_Conditional(proj, dir);
		}

		DARGH::g_isDARDataLoaded = true;
	}
}
