// ============================================================================
//                           DARProjectRegistry.cpp
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

#include "DARProjectRegistry.h"

namespace DARGH
{
	// The single DAR Project Registry, used to store all of our
	// DAR map data.
	std::map<std::string, DARProject> g_DARProjectRegistry;

	// Whether the DAR data has been loaded into the registry yet.
	// The data is loaded at runtime by the SKSE callback function.
	bool g_isDARDataLoaded = false;

	DARProject* getDARProject(RE::hkRefPtr<RE::hkbProjectData> a_projData)
	{
		// ====================================================================
		//                         getDARProject
		// --------------------------------------------------------------------
		// Retrieves the first entry in our global project registry that
		// points to 'projData'. If no entries are found, or if DAR data
		// has not yet been loaded, returns NULL.
		// --------------------------------------------------------------------
		if (!a_projData || !g_isDARDataLoaded) {
			return nullptr;
		}

		for (auto& [path, proj] : DARGH::g_DARProjectRegistry) {
			if (proj.projData == a_projData) {
				return &proj;
			}
		}

		return nullptr;
	}

	void registerDARProject(std::string_view a_path)
	{
		// ====================================================================
		//                        registerDARProject
		// --------------------------------------------------------------------
		// Initialise a new empty entry in our global project registry, with
		// a key that is the parent directory of 'projectFilePath', if the
		// entry doesn't already exist.
		// ====================================================================
		if (!a_path.empty()) {
			std::string path{ a_path };
			std::transform(path.begin(), path.end(), path.begin(), [](uint8_t c)
				{ return static_cast<uint8_t>(std::tolower(c)); });

			// Only register a new project entry if there isn't already
			// one there.
			if (!g_DARProjectRegistry.contains(path)) {
				// Doesn't already exist in the map, so create and register
				// a new entry.
				DARProject darProj;
				darProj.projFolder = path.substr(0, path.find_last_of("\\"));
				g_DARProjectRegistry.insert({ path, darProj });
			}
		}
	}
}