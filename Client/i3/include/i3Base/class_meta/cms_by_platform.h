#pragma once

#include "class_meta_style.h"
#include "../../i3Sound/i3SoundConfig.h"
#include "../i3Platform.h"

namespace i3
{
	// 구체클래스 후보가 여러개 존재하는데, 플랫폼별로 하나만 선택한다.. 관련된 메타템플릿이 필요하다.
	struct cms_concrete_d3d9
	{
#if defined(I3G_DX)
		static const class_meta_style value = cms_concrete;
#else
		static const class_meta_style value = cms_normal;
#endif	
	};

	struct cms_concrete_xbox3d
	{
#if defined(I3G_XBOX)
		static const class_meta_style value = cms_concrete;
#else
		static const class_meta_style value = cms_normal;
#endif		
	};

	struct cms_concrete_psp3d
	{
#if defined(I3G_PSP)
		static const class_meta_style value = cms_concrete;
#else
		static const class_meta_style value = cms_normal;
#endif		
	};

	struct cms_concrete_fmod
	{
#if defined(I3SND_FMOD)
		static const class_meta_style value = cms_concrete;
#else
		static const class_meta_style value = cms_normal;
#endif
	};

	struct cms_concrete_dsound
	{
#if defined(I3SND_DSOUND)
		static const class_meta_style value = cms_concrete;
#else
		static const class_meta_style value = cms_normal;
#endif
	};
			
}