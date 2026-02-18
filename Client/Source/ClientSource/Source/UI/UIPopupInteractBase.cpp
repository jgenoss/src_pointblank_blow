#include "pch.h"
#include "UIPopupInteractBase.h"
#include "i3Base/string/ext/mb_to_utf16.h"

//I3_ABSTRACT_CLASS_INSTANCE( UIPopupInteractBase, UIPopupBase);
I3_CLASS_INSTANCE( UIPopupInteractBase);

UIPopupInteractBase::UIPopupInteractBase()
{
	m_SelectedIndex = 0;
}

UIPopupInteractBase::~UIPopupInteractBase()
{
}

/*virtual*/ bool UIPopupInteractBase::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( pArg1 != nullptr)
	{
		// pArg1은 wchar_t 타입으로 아직 바뀌지 않은 것으로 가정한다.
		// TODO : 언젠가 바뀔수도 있기 때문에 나중에 재확인..(2014.07.10.수빈)

		// 재확인 결과 이 클래스의 Arg1은 현재 쓰이고 있지 않은듯 하다...(2014.09.01.수빈)
		// Unicode로 변환 후 이 클래스 Send Message를 위해 사용합니다. (2015.05.22 -> Jinsik.kim)
		i3::safe_string_copy(m_wstrNick, (const wchar_t*)pArg1, NET_NICK_NAME_SIZE);

		//i3::mb_to_utf16((const char*)pArg1, m_wstrNick);
//		i3::safe_string_copy( m_szNick, (const char*) pArg1, NET_NICK_NAME_SIZE );
	}
	else
		m_wstrNick.clear(); 

	if( pArg2 != nullptr)
		m_SelectedIndex = *static_cast<INT32*>(pArg2);
	else
		m_SelectedIndex = -1;

	return true;
}


