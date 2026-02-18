#include "pch.h"
#include "UIPopupFocus.h"

#include "UIPopupBase.h"
#include "UIPhaseNonClanMember.h"
#include "UI_V10/UIPopupUserProfile_V10.h"
#include "UIPopupExit.h"
#include "UI_V10/UIPhaseTemp_V10.h"
#include "UIPopupOption.h"
//#include "../StageGui/Popup/PopupBase.h"


class UIPopupFocusCallback_Null : public UIPopupFocusCallback
{
	virtual void	ProcessInputDisableMainUI(bool bEnable) override { }
	virtual void	ProcessInputDisableFloatWindow(bool bEnable) override { }
} g_cb_null;




UIPopupFocus::UIPopupFocus() : m_cb(&g_cb_null)
{
	
}

UIPopupFocus::~UIPopupFocus()
{

}

void	UIPopupFocus::SetCallback( UIPopupFocusCallback* cb) 
{
	m_cb = cb;
}

void	UIPopupFocus::ClearCallback()
{
	m_cb = &g_cb_null;
}

bool	UIPopupFocus::IsActivePopup( UIPopupBase* popup) const
{
	const size_t count = m_active_popup_list.size();

	// 빨리 찾으려면 거꾸로 돌리는게 낫겠는데..
	for (size_t i = 0 ; i < count ; ++i )
	{
		UIPopupBase* p = m_active_popup_list[count - 1 - i ];
		if ( p == popup ) 
			return true;
	}
	return false;
}

bool UIPopupFocus::IsTopPopup( UIPopupBase* popup) const
{
	const size_t count = m_active_popup_list.size();
	if(count == 0) return false;
	UIPopupBase* p = m_active_popup_list[count - 1];

	if ( p == popup ) 
		return true;

	return false;
}


void	UIPopupFocus::AddFocusPopup( UIPopupBase* popup)
{
// 팝업 검증 루틴.. (기존에 있다면, 아무 처리하지 않는 것으로 간주...)
	if ( UIPopupFocus::IsActivePopup(popup) ) 
		return;
	
	const size_t cnt = m_active_popup_list.size();
	
	if (cnt == 0)
	{
		//1.5버전이 아닌경우에는 클랜쪽 팝업이 뜰때 MainUI를 사용할수 있어야 함
		if( g_pEnvSet->m_nUIVersion != UFV_1_5 )
		{
			if( !i3::same_of<UIPhaseNonClanMember*>(popup) && !i3::same_of<UIPhaseClanMember*>(popup)
				&& !i3::same_of<UIPopupUserProfile_V10*>(popup))
				m_cb->ProcessInputDisableMainUI(true);
		}
		else
		{
			m_cb->ProcessInputDisableMainUI(true);
		}
	}
	else
	{
		UIPopupBase* ett_last = m_active_popup_list.back();

		if( (i3::same_of<UIPhaseNonClanMember*>(ett_last) || i3::same_of<UIPhaseClanMember*>(ett_last)) &&
			g_pEnvSet->m_nUIVersion != UFV_1_5)
		{
			m_cb->ProcessInputDisableMainUI(true);
		}
		else
		{
			ett_last->SetInputDisable(true);
		}
	}
	
	m_active_popup_list.push_back(popup);
	
}

void	UIPopupFocus::RemoveFocusPopup( UIPopupBase* popup)
{
	size_t cnt = m_active_popup_list.size();
	if (cnt == 0) return;

	//I3TRACE( "FocusPopup Disable %d\n", cnt);
	
	UIPopupBase* last_ett = m_active_popup_list.back();
	
	if ( last_ett == popup )
	{
		last_ett->SetInputDisable(false);
		i3::vu::erase_index(m_active_popup_list, cnt - 1 );
		cnt = 	m_active_popup_list.size();		
		
		if ( cnt == 0)
		{
			m_cb->ProcessInputDisableMainUI(false);
		}
		else
		{
			last_ett = m_active_popup_list.back();
			if( (i3::same_of<UIPhaseNonClanMember*>(last_ett) || i3::same_of<UIPhaseClanMember*>(last_ett)) &&
				g_pEnvSet->m_nUIVersion != UFV_1_5 )
			{
				m_cb->ProcessInputDisableMainUI(false);
			}
			else
			{
				last_ett->SetInputDisable(false);
			}
		}
	}
	else
	{
		for (size_t i = 1 ; i < cnt ; ++i )
		{
			INT32   idx = cnt - 1 - i;
			UIPopupBase* ett = m_active_popup_list[idx];
			if ( ett == popup ) 
			{
				ett->SetInputDisable(false);
				i3::vu::erase_index(m_active_popup_list, idx);
				break;
			}
		}
	}
}

UIPopupBase* UIPopupFocus::GetLastestPopup() const
{
	const size_t count = m_active_popup_list.size();
	if(count == 0) return 0;

	return m_active_popup_list[count - 1];
}
