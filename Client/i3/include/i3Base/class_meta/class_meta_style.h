#pragma once

namespace i3
{
	enum class_meta_style
	{
		cms_normal,				
		cms_abstract,		// 추상함수에 의한 추상클래스라면 디폴트로 들어감. 파생클래스 중에 concrete가 존재하면 치환됨..
		cms_concrete,		// 추상클래스에 지정된 메타가 있다면, 이 값이 설정된 메타로 생성.
		cms_ghost,			// 백그라운드쓰레드내에서, 지정된 메타가 있으면 해당 클래스메타가 아닌 지정된 메타로 생성.  (쓰이지 않으나, 호환성을 위해 처리함)
		cms_max,			
	};
}
