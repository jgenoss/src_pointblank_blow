#include "pch.h"
#include "UITopMenu.h"

#include "UIMainFrame.h"
#include "UIPhaseBase.h"
#include "UIPhaseSubBase.h"
#include "UIUtil.h"

#include "UIMath.h"
#include "UIFloatCharacterView.h"
#include "Batmanager.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "MailBoxContext.h"
#include "UserInfoContext.h"
#include "../BattleSlotContext.h"
#include "ClanGameContext.h"

#define BACKBUTTON_CLICK_DELAY 1.f

I3_CLASS_INSTANCE( UITopMenu); //, UIBase);

UITopMenu::UITopMenu() : m_bTemporaryNotice(false), m_bAlarmCommunity(false), m_bAlarmFlag(false),
							m_Mode(TOPMENU_MODE_NONE)
{
	SetSlideNotice_Permanant(g_pFramework->getPermNotice());
	SetSlideNotice_Temporary(g_pFramework->getTempNotice());

}

UITopMenu::~UITopMenu() 
{

}

void UITopMenu::SetMode( TOPMENU_MODE mode)
{
	if( m_Mode == mode ) return;

	m_Mode = mode;

	switch( mode)
	{
	case TOPMENU_MODE_SERVER	:_SetServerMode();	break;
	case TOPMENU_MODE_CHANNEL	:_SetChannelMode();	break;
	case TOPMENU_MODE_READYROOM	:
	case TOPMENU_MODE_SHOP		:
	case TOPMENU_MODE_READY		:_SetReadyMode();	break;
	}
}

void UITopMenu::GotoShopFromNewAndHot()
{
	i3UIScene * pScene = GetScene( "PointBlankRe_Main");
	LuaState* L = nullptr;
	
	L = UILua::CallLuaFunction( pScene, "goto_Shop");
	UILua::EndLuaFunction(L , 0);
}

void UITopMenu::SetSlideNotice_Permanant( const i3::rc_wstring& wStr)
{
	if( MAX_STRING_COUNT > i3::generic_string_size( wStr))
	{
		// 기존 공지와 다를 경우만 바꾼다
		if ( m_strPermanatNotice != wStr)
		{
			m_strPermanatNotice = wStr;		
		}
	}
}

void UITopMenu::SetSlideNotice_Temporary(const i3::rc_wstring& wStr)
{
	if (MAX_STRING_COUNT > i3::generic_string_size( wStr))
	{
		// 기존 공지와 다를 경우만 바꾼다
		if (m_strTemporaryNotice != wStr)
		{
			m_strTemporaryNotice = wStr;		
			m_bTemporaryNotice = true;
		}
	}
}

void UITopMenu::_UpdateFlowAnnounce(REAL32 rDeltaSeconds)
{
	if( m_FlowEditBox.isReadyToFlow() )
	{
		if(m_FlowEditBox.GetElpasedTime() > 120.0f)
		{
			m_bTemporaryNotice = false;
			i3::generic_string_copy(m_strTemporaryNotice, m_strPermanatNotice);
		}

		const i3::rc_wstring&	wstrText = (m_bTemporaryNotice) ? m_strTemporaryNotice : m_strPermanatNotice;

		if( !m_FlowEditBox.isSameText(wstrText) )
		{
			m_FlowEditBox.SetFlowTextData(wstrText);
			m_FlowEditBox.PlayEditTextFlow(TFD_HORIZONTAL, false, true);
		}
	}

	m_FlowEditBox.OnUpdate(rDeltaSeconds);
}

void UITopMenu::_UpdateMailBoxAlaram(REAL32 rDeltaSeconds)
{
	_CheckUnReadMail();

	if(m_bAlarmCommunity == false)
		return;

	m_rAlarmTerm += rDeltaSeconds;

	if(m_rAlarmTerm > 0.25f)
	{
		m_rAlarmTerm = 0.0f;
		m_bAlarmFlag = !m_bAlarmFlag;
	}

	LuaState * L = _CallLuaFunction("SetMailAlarm");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L, m_bAlarmFlag);
		_EndLuaFunction(L , 1);
	}
}

void UITopMenu::_CheckUnReadMail()
{
	const NOTE_RECEIVE * pNote = nullptr;

	INT32 nCount = MailBoxContext::i()->GetNoteReceiveCount();
	for (INT32 i = 0; i < nCount; ++i)
	{
		pNote = MailBoxContext::i()->GetReceiveNote(i);
		if (pNote->header._state == NS_UNREAD)
		{
			if (!m_bAlarmCommunity) StartHighlightMailbox();
			return;
		}
	}

	nCount = MailBoxContext::i()->GetNoteGiftCount();
	for (INT32 i = 0; i < nCount; ++i)
	{
		pNote = MailBoxContext::i()->GetNoteGiftNote(i);
		if (pNote->header._state == NS_UNREAD)
		{
			if (!m_bAlarmCommunity) StartHighlightMailbox();
			return;
		}
	}

	if(m_bAlarmCommunity)
		_EndHighlightMailbox();
}

void UITopMenu::_EndHighlightMailbox()
{
	m_bAlarmCommunity = false;
	m_bAlarmFlag = false;

	// 알람 기능 해제
	LuaState * L = _CallLuaFunction("SetMailAlarm");
	if (L != nullptr)
	{
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace inner
{
	const REAL32 perido_time = 0.1f, perido_gauge = 0.02f;

	REAL32 origin_logo_shape_width = 0.f;
	REAL32 origin_logo_x = 0.f, origin_logo_y = 0.f, origin_logo_width = 0.f, origin_logo_height = 0.f;
	static REAL32 delta_sec = 0.f;
	REAL32 target_factor = 0.f, curr_factor = 0.f;
	INT32 img_set_idx = 0;
	INT32 lmb_clicked_count = 0;
	bool enable_moving_gauge = false;
}

void PBGaugeBar::init(i3UIScene * pScene)
{
	logo_title = (i3UIFrameWnd *) pScene->FindChildByName( "fwdTitle2");
	logo_imgset = (i3UIButtonImageSet *) pScene->FindChildByName( "i3UIButtonImageSet");

	if( inner::origin_logo_shape_width == 0.f ) //battle 종료 후에 값이 변경 된다. 왜 그럴까?
	{
		i3::pack::RECT uv = *logo_imgset->getShape(0)->getTextureCoord(); //x1,y1, x2, y2이다. (w,h가 아니다.)
		inner::origin_logo_shape_width = uv.right;
	}
	inner::origin_logo_x = logo_imgset->getAccumulatedPos()->x;
	inner::origin_logo_y = logo_imgset->getAccumulatedPos()->y;
	inner::origin_logo_width = logo_imgset->getSize()->x;
	inner::origin_logo_height = logo_imgset->getSize()->y;

	reset(); //초기화

	//툴팁용 버튼으로 희미하게 만든다.
	i3UIButton* btn = (i3UIButton*) pScene->FindChildByName( "i3UIButton_Tooltip");
	if ( nullptr != btn )
	{
		COLOR col = *btn->getColor();
		col.a = 0;
		btn->setColor(&col);
	}
}

void PBGaugeBar::update_gauge(REAL32 rDeltaSeconds)
{
	if( inner::delta_sec > inner::perido_time )
	{
		inner::delta_sec = 0.f;

		if( inner::img_set_idx < MAX_IMG_ANI-1)
			++inner::img_set_idx;
		else
			inner::img_set_idx = 0;

		logo_imgset->SetShapeIdx( inner::img_set_idx );

		if( inner::enable_moving_gauge == true )
		{
			if( inner::target_factor - inner::curr_factor <= inner::perido_gauge )
			{
				resize(inner::target_factor);
				inner::enable_moving_gauge = false;
			}
			else
			{
				resize(inner::curr_factor + inner::perido_gauge );
			}
		}
	}
	else
	{
		inner::delta_sec += rDeltaSeconds;
	}
}

#include "UI/UISideMenu.h"
void PBGaugeBar::update(REAL32 rDeltaSeconds)
{
	(this->*fn_update)(rDeltaSeconds);

#if defined(I3_DEBUG)
	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ) &&
		g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ) )
	{
		POINT pt = i3UI::getManager()->getCurrentMousePosition();

		pt.x -= (LONG)inner::origin_logo_x; pt.y -= (LONG)inner::origin_logo_y;
		if (pt.x >= 0 && pt.x <= inner::origin_logo_width)
		{
			if (pt.y >= 0 && pt.y <= inner::origin_logo_height)
			{
				if( GameUI::IsMouseStroked(I3I_MOUSE_LBUTTON) == true )
				{
					++inner::lmb_clicked_count;
					if( inner::lmb_clicked_count == 3 )
					{
						UISideMenu::instance()->open_fly_menu();
						inner::lmb_clicked_count = 0;
					}
				}
			}
			else
			{
				inner::lmb_clicked_count = 0;
			}
		}
		else
		{
			inner::lmb_clicked_count = 0;
		}
	}
#endif
}

void PBGaugeBar::resize(REAL32 factor)
{
	if( logo_title == nullptr)
		return;

	if( factor == 0.f )
	{
		logo_title->EnableCtrl(false);
	}
	else
	{
		if( inner::curr_factor == factor ) return;

		logo_title->EnableCtrl(true);

		REAL32 shape_w = uiMath::interpolate(factor, 0.f, inner::origin_logo_shape_width);
		for(INT32 i = 0; i<MAX_IMG_ANI; ++i)
		{
			i3UIShape & shape = *logo_imgset->getShape(i);
			shape.setCoordW( shape_w );
		}

		VEC2D* size = logo_imgset->getSize();
		REAL32 logo_w = uiMath::interpolate(factor, 0.f, inner::origin_logo_width);
		size->x = logo_w;
		logo_imgset->setSize(size);
	}

	inner::curr_factor = factor;
}

void PBGaugeBar::reset()
{
	resize(0.f);

	inner::enable_moving_gauge = false;

	fn_update = &PBGaugeBar::update_none;
}

void PBGaugeBar::set_factor(REAL32 unit_factor)
{
	inner::target_factor = unit_factor;
	inner::img_set_idx = 0; 
	inner::delta_sec = 0.f;
	inner::enable_moving_gauge = true;

	fn_update = &PBGaugeBar::update_gauge;
}

void PBGaugeBar::set_lastest_target_factor()
{
	resize(inner::target_factor);
}