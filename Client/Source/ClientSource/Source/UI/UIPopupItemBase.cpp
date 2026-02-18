#include "pch.h"
#include "UIPopupItemBase.h"

#include "ItemConsumer.h"
#include "UIUtil.h"

//- UIPopupItemBase ----------------------------//
I3_CLASS_INSTANCE( UIPopupItemBase);

UIPopupItemBase::UIPopupItemBase()
{
	m_Type = USE_ITEM_UNKNOWN;
	m_ItemID = 0;
	m_AuthWareDBIndex = 0;
}

UIPopupItemBase::~UIPopupItemBase()
{
}

/*virtual*/ LuaState * UIPopupItemBase::_SetItemInfo( void)
{
	i3::rc_wstring wstrItemName;

	// 아이템 이름
	g_pShopItemInfoDataBase->GetItemName( m_ItemID, wstrItemName);

	LuaState * L = _CallLuaFunction( "SetItemInfo");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrItemName);
	}
	return L;
}

/*virtual*/ bool UIPopupItemBase::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( pArg1 == nullptr || pArg2 == nullptr) return false; //이 코드는 필요 없다.

	m_ItemID = *static_cast<INT32*>(pArg1);
	m_Type = *static_cast<USE_ITEM_POPUP_TYPE*>(pArg2);
	m_AuthWareDBIndex = CInvenList::i()->FindWareDBIdx_ByItemID(m_ItemID);

	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( _SetItemInfo() == nullptr)
		return false;

	GetSlide()->ForceEnd();

	return true;
}

/*virtual*/ void UIPopupItemBase::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent( event, Arg, UserData);

	if( event == EVENT_ITEM_AUTH)
	{
		// 아이템 사용 실패
		if (EV_FAILED(Arg))
		{
			// 아이템을 사용을 실패하였습니다.
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_3"), 
				&GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		}
	}
}
