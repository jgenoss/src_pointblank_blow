#include "pch.h"
#include "UIPopupTSReward.h"

#include "UIMainFrame.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UIMath.h"
#include "UIUtil.h"
#include "UserInfoContext.h"

#include "i3Base/ColorString.h"

namespace
{
	UINT32 ts_item_count = 0;
};


I3_CLASS_INSTANCE( UIPopupTSReward);//, UIPopupBase);

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupTSReward_Confirm(LuaState * L)
	{
		UIPopupTSReward * pThis = static_cast<UIPopupTSReward*>(GameUI::GetPopup( UPW_TS_REWARD));
		if( pThis != nullptr)
			pThis->Confirm();
		return 0;
	}
	int UIPopupTSReward_Select(LuaState * L)
	{
		INT32 inx = i3Lua::GetIntegerArg( L, 1);
		UIPopupTSReward * pThis = static_cast<UIPopupTSReward*>(GameUI::GetPopup( UPW_TS_REWARD));
		if( pThis != nullptr)
			pThis->SelectItem(inx);
		return 0;
	}
}

LuaFuncReg	UIPopupTSReward_Glue[] =
{
	{ "Confirm",	UIPopupTSReward_Confirm },
	{ "SelectItem",	UIPopupTSReward_Select },
	{ nullptr,			nullptr }
};



//------------------------------------------------------------------------------//
UIPopupTSReward::UIPopupTSReward()
{
}

UIPopupTSReward::~UIPopupTSReward()
{
}

//------------------------------------------------------------------------------//
/*virtual*/ void UIPopupTSReward::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene("Scene/Popup/PBRe_PopUp_TS.i3UIs", UIPopupTSReward_Glue);

}

/*virtual*/ void UIPopupTSReward::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_ItemContext[0].frame = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd2");
	m_ItemContext[1].frame = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd3");
}

/*virtual*/ bool UIPopupTSReward::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	ts_item_count = 1;

	GetSlide()->ForceEnd();

	return true;
}

/*virtual*/ void UIPopupTSReward::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	m_ItemContext[0].img_box = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet");
	m_ItemContext[1].img_box = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet0");
}

/*virtual*/ void UIPopupTSReward::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();

	m_bEffect = false;

	SetLabel();
	SetItem();
	EffectItem();
	PlaySound(UISND_USER_GUIDE_START4);

	m_SelectInx = 0;

	LuaState * L = _CallLuaFunction("OnEntranceEnd");
	_EndLuaFunction(L, 0);
}

/*virtual*/ void UIPopupTSReward::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();

	m_bEffect = false;

	for(size_t i=0;i<MAX_ITEM;++i)
	{
		m_ItemContext[i].img_box->RestoreSizePos();
		I3_SAFE_RELEASE(m_ItemContext[i].img_box);
	}
}

/*virtual*/ void UIPopupTSReward::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate( rDeltaSeconds);

	if( m_bEffect )
	{
		m_AccrueTime += rDeltaSeconds;

		//1초 반 동안에 움직인다.
		if( m_AccrueTime <= 1.5f )	
		{
			for( UINT32 i = 0; i< ts_item_count; ++i)
			{
				//선택된 경우엔 증가
				if( m_SelectInx == i )
					m_ItemContext[i].factor += m_AccrueTime;
				else
					m_ItemContext[i].factor -= m_AccrueTime;

				//강제로 0.5 ~ 1.0 값으로 수정한다. (좋지 못 한 코드)
				m_ItemContext[i].factor = uiMath::limited( m_ItemContext[i].factor, 0.5f, 1.0f );
			}

			EffectItem();
		}
		else
		{
			m_bEffect = false;
		}
	}
}

void UIPopupTSReward::SelectItem(int inx)
{
	m_bEffect = true;
	m_AccrueTime = 0.0f;
	m_SelectInx = inx;

	ChangeItemDisplay();
}

void UIPopupTSReward::Confirm(void)
{
	m_bEffect = false;

	GetMainFrame()->ClosePopup(UPW_TS_REWARD);

	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_REWARD_ITEM3"));
	}
	/*else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_REWARD_ITEM2"));
	}*/
}

void UIPopupTSReward::EffectItem()
{
	for(UINT32 i = 0; i < ts_item_count; ++i)
	{
		m_ItemContext[i].img_box->Scale( m_ItemContext[i].factor );
		m_ItemContext[i].img_box->SetAlpha( m_ItemContext[i].factor );
	}
}

void UIPopupTSReward::ChangeItemDisplay()
{
	for(UINT32 i = 0; i < ts_item_count; ++i)
	{
		//아이템 이름
		i3::rc_wstring wstr( m_ItemContext[i].name );

		if( i == m_SelectInx )
		{
			::ColorString(wstr, 255, 255, 255);
		}
		else
		{
			::ColorString(wstr, 105, 105, 105);
		}

		LuaState * L = _CallLuaFunction("SetItemName");
		i3Lua::PushInteger(L, i);
		i3Lua::PushStringUTF16To8(L, wstr.c_str(), wstr.size() );
		_EndLuaFunction(L, 2);
	}
}

void UIPopupTSReward::SetItem()
{
	for(UINT32 i = 0; i < ts_item_count; ++i)
	{
		m_ItemContext[i].factor = 1.0f;
		m_ItemContext[i].frame->EnableCtrl( true );

		T_ItemID itemid =	UserInfoContext::i()->GetTSRewardItemID(); 		
		
		//이미지 처리
		GetItemImageMgr()->SelectImage(m_ItemContext[i].img_box, itemid );

		usf::GetShopItemName(m_ItemContext[i].name, itemid);
	}

	ChangeItemDisplay();

	if( ts_item_count == 1 )
	{
		//중앙에 위치한다.
		VEC3D cen, pos;
		m_pPopupFrame->getAccumulatedCenPos( &cen );

		m_ItemContext[0].frame->getAccumulatedCenPos( &pos );

		cen.y = pos.y;
		m_ItemContext[0].frame->moveCenter( &cen );

		m_ItemContext[1].frame->EnableCtrl( false );
	}
	else
	{
		m_ItemContext[1].factor = 0.5f;
	}
}

void UIPopupTSReward::SetLabel()
{
	i3::rc_wstring wstr_title = GAME_RCSTRING("STR2_TS_REWARD_TITLE");
	::ColorString( wstr_title, 235,177, 82 );

	LuaState * L = _CallLuaFunction("SetString");
	i3Lua::PushStringUTF16To8(L, wstr_title.c_str(), wstr_title.size() );

	i3Lua::PushStringUTF16To8(L, L"" );
	i3Lua::PushStringUTF16To8(L, L"" );

	_EndLuaFunction(L, 3);
}