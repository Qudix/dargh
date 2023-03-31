// ============================================================================
//                                Hooks.cpp
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
#include "DARProjectRegistry.h"

// Turn this on if you want to trace & debug the hooks
// (CAUTION: only use for debugging - this will generate large dargh.log files
//  and degrade performance)
//#define DEBUG_TRACE_HOOKS

// HOOK 1: finish constructor for hkbCharacterStringData
uint64_t hkbCharacterStringData_fctor_Orig;
typedef void (*hkbCharacterStringData_fctor)(RE::hkbCharacterStringData*, RE::hkFinishLoadedObjectFlag);

// HOOK 2: finish constructor for hkbProjectData
uint64_t hkbProjectData_fctor_Orig;
typedef void (*hkbProjectData_fctor)(RE::hkbProjectData*, RE::hkFinishLoadedObjectFlag);

// HOOK 3: hkbClipGenerator::Activate(hkbContext* context)
uint64_t hkbClipGenerator_activate_Orig;
typedef void (*hkbClipGenerator_activate)(RE::hkbClipGenerator*, RE::hkbContext*);

RE::BSSpinLock lock;
std::unordered_map<RE::hkbCharacterStringData*, RE::hkArray<RE::hkbAssetBundleStringData>*> g_animHashmap;

void cacheModifiedCharStringData(RE::hkbCharacterStringData* a_hkbCharStringData)
{
	RE::BSSpinLockGuard locker(lock);
	g_animHashmap.insert({ a_hkbCharStringData, &a_hkbCharStringData->animationNames });
}

struct hkbCharacterStringDataHook
{
	static void dtor(RE::hkbCharacterStringData* a_this, RE::hkFinishLoadedObjectFlag a_flag)
	{
		// Finish constructor for a hkbCharacterStringData object.
		// In the Havok system, finish constructors are the very last step in order to make
		// an object usable. It's at this point that we actually replace the animation names,
		// and then remove the entry from our cache (g_animHashmap).
		// TODO: Should we free any other memory at this point? Memory leaks?

#ifdef DEBUG_TRACE_HOOKS
		logger::info("========= HOOK 1: hkbCharacterStringData::dtor ========");
#endif
		RE::BSSpinLockGuard locker(lock);
		const auto search = g_animHashmap.find(a_this);
		if (search != g_animHashmap.end()) {
			// Found it.
			auto animationNames = search->second;
			a_this->animationNames._data = animationNames->_data;
			a_this->animationNames._size = animationNames->_size;
			a_this->animationNames._capacityAndFlags = animationNames->_capacityAndFlags;

			// Clear the entry in the map.
			g_animHashmap.erase(search);
		}

#ifdef DEBUG_TRACE_HOOKS
		logger::info("Unlocking and passing control back to orig function...");
#endif

		_dtor(a_this, a_flag);
	}

	static inline REL::Relocation<decltype(dtor)> _dtor;

	static void Install()
	{
		logger::info("Installing Hook 1: hkbCharacterStringData::dtor...");
		REL::Relocation<std::uintptr_t> vtbl{ RE::hkbCharacterStringData::VTABLE[0] };
		logger::info("  1. Redirecting fctor VFT pointer at {:#x}.", vtbl.address());         // 0x418044a8
		logger::info("  2. Before hooking, it points to {:#x}.", *(uint64_t*)vtbl.address()); // 0x40a3cfa0
		_dtor = vtbl.write_vfunc(0x0, dtor);
		logger::info("  3. After hooking, it points to {:#x}.", *(uint64_t*)vtbl.address());  // 0x31bf1bc0
	}
};

struct hkbProjectDataHook
{
	static void dtor(RE::hkbProjectData* a_this, RE::hkFinishLoadedObjectFlag a_flag)
	{
		// Finish constructor for a hkbProjectData object.
		// We can now clear any refs to this object in our project registry.
		// TODO: Should we free any other memory at this point? Memory leaks?

#ifdef DEBUG_TRACE_HOOKS
		logger::info("========= HOOK 2: hkbProjectData::dtor ========");
#endif

		if (DARGH::g_isDARDataLoaded) {
			// Nullify all refs to this object in our registry.
			for (auto& [path, proj] : DARGH::g_DARProjectRegistry) {
				if (proj.projData.get() == a_this) {
					proj.projData.reset();
				}
			}
		}

#ifdef DEBUG_TRACE_HOOKS
		logger::info("Passing control back to orig function...");
#endif

		_dtor(a_this, a_flag);
	}

	static inline REL::Relocation<decltype(dtor)> _dtor;

	static void Install()
	{
		logger::info("Installing Hook 2: hkbProjectData::dtor...");
		REL::Relocation<std::uintptr_t> vtbl{ RE::hkbProjectData::VTABLE[0] };
		logger::info("  1. Redirecting fctor VFT pointer at {:#x}.", vtbl.address());         // 0x4180ac48
		logger::info("  2. Before hooking, it points to {:#x}.", *(uint64_t*)vtbl.address()); // 0x40a5b060
		_dtor = vtbl.write_vfunc(0x0, dtor);
		logger::info("  3. After hooking, it points to {:#x}.", *(uint64_t*)vtbl.address());  // 0x31bf1df0
	}
};

struct hkbClipGeneratorHook
{
	static void Activate(RE::hkbClipGenerator* a_this, RE::hkbContext* a_context)
	{
		// Activate a clip generator. This resets the clip generator (which is a type
		// of behaviour graph node) to its initial state, ready for reuse. E.g. inter alia
		// it resets the animation clip to the beginning.

#ifdef DEBUG_TRACE_HOOKS
		logger::info("========= HOOK 3: hkbClipGenerator::Activate IN ========");
#endif

		std::int16_t origIndex = a_this->animationBindingIndex; // CommonLibSSE offset for this member is wrong, fix it upstream (06C -> 070)
		if (origIndex == -1) {
			return _Activate(a_this, a_context);
		}

		DARProject* darProj = DARGH::getDARProject(a_context->character->projectData);
		if (!darProj) {
			return _Activate(a_this, a_context);
		}

		RE::BShkbAnimationGraph* animGraph = (RE::BShkbAnimationGraph*)((uint64_t)a_context->character - 0xC0);
		RE::Actor* actor = animGraph->holder;
		if (!actor) {
			return _Activate(a_this, a_context);
		}

		if (!actor->Is(RE::FormType::ActorCharacter)) {
			return _Activate(a_this, a_context);
		}

		std::int16_t newIndex = DARGH::getNewAnimIndex(darProj, origIndex, actor);
		if (newIndex == -1) {
			return _Activate(a_this, a_context);
		}

		// Temporarily change the animation index to point to the replacement, then activate
		// the associated clip generator.
		a_this->animationBindingIndex = newIndex;
		_Activate(a_this, a_context);
		a_this->animationBindingIndex = origIndex;
	}

	static inline REL::Relocation<decltype(Activate)> _Activate;

	static void Install()
	{
		logger::info("Installing Hook 3: ClipGenerator::Activate...");
		REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_hkbClipGenerator[0] };
		logger::info("  1. Redirecting fctor VFT pointer at {:#x}.", (vtbl.address() + 0x20));         // 0x41809f18
		logger::info("  2. Before hooking, it points to {:#x}.", *(uint64_t*)(vtbl.address() + 0x20)); // 0x40a42760
		_Activate = vtbl.write_vfunc(0x4, Activate);
		logger::info("  3. After hooking, it points to {:#x}.", *(uint64_t*)(vtbl.address() + 0x20));  // 0x31bf1e80
	}
};

bool install_hooks()
{
	// The addresses in the comments are those we would expect to see
	// for 1.6.659 (GOG edition). They are intended to facilitate basic
	// sanity checks.

	hkbCharacterStringDataHook::Install();
	hkbProjectDataHook::Install();
	hkbClipGeneratorHook::Install();

	logger::info("Successfully installed hooks.");

	return true;
}
