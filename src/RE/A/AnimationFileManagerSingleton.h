#pragma once

#include "RE/B/BSTSingleton.h"
#include "RE/H/hkbContext.h"
#include "RE/I/IAnimationClipLoaderSingleton.h"

namespace RE
{
	class AnimationFileManagerSingleton :
		public IAnimationClipLoaderSingleton,
		public BSTSingletonSDM<AnimationFileManagerSingleton>
	{
	public:
		inline static auto RTTI = RTTI_AnimationFileManagerSingleton;
		inline static auto VTABLE = VTABLE_AnimationFileManagerSingleton;

		virtual ~AnimationFileManagerSingleton() override;  // 00

		// override (IAnimationClipLoaderSingleton)
		virtual void          Unk_01(void) override;                                                                                // 01
		virtual std::uint64_t Load(const hkbContext& a_context, hkbClipGenerator* a_clipGenerator, std::uint64_t a_arg4) override;  // 02
		virtual void          Unk_03(void) override;                                                                                // 03
		virtual void          Unk_04(void) override;                                                                                // 04
		virtual void          Unk_05(void) override;                                                                                // 05
	};

	inline std::uint64_t AnimationFileManagerSingleton::Load(const hkbContext& a_context, hkbClipGenerator* a_clipGenerator, std::uint64_t a_arg4)
	{
		using func_t = decltype(&AnimationFileManagerSingleton::Load);
		REL::Relocation<func_t> func{ RELOCATION_ID(63070, 63982) };
		return func(this, a_context, a_clipGenerator, a_arg4);
	}
}
