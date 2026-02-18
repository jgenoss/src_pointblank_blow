#include "pch.h"
#include "UIBattlePopupDeleteItem.h"

#include "ItemImageMgr.h"
#include "UIBattleFrame.h"
#include "Useable_Item_Def.h"
#include "ItemConsumer.h"
#include "UIShopFunction.h"
#include "UIUtil.h"

I3_CLASS_INSTANCE( UIBattlePopupDeleteItem);

namespace inner
{
	enum { OK, CANCEL, };
	UIBattlePopupDeleteItem* pThis = 0;
}

/***********************//***********************/
extern "C" 
{
	int battle_query_item_click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		inner::pThis->clicked( value );
		return 0;
	}
}

LuaFuncReg battle_query_item_glue[] =
{
	{ "Click", battle_query_item_click	},
	{ nullptr, nullptr}
};

/***********************//***********************/
void UIBattlePopupDeleteItem::clicked(int idx)
{
	switch( idx )
	{
	case inner::OK: remove_item(); break;
	case inner::CANCEL: break;
	}

	UIBattleFrame::i()->ClosePopup(UBP_DELETEITEM);
}


void UIBattlePopupDeleteItem::remove_item()
{
	T_ItemDBIdx db_index = CInvenList::i()->FindWareDBIdx_ByItemID( m_item_id );
	if( db_index == INVALID_WAREDB_INDEX)
	{
		/*사용할 수 없는 아이템입니다.*/ /*아이템 사용 실패*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		return;
	}

	GameEventSender::i()->SetEvent(EVENT_AUTH_DELETE, &db_index);
}

/***********************//***********************/
UIBattlePopupDeleteItem::UIBattlePopupDeleteItem(void)
{
	inner::pThis = this;
}

UIBattlePopupDeleteItem::~UIBattlePopupDeleteItem(void)
{
}

/*virtual*/ void UIBattlePopupDeleteItem::OnCreate( i3GameNode * pParent)
{
	UIBattlePopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_ItemQuery.i3UIs", battle_query_item_glue);
}

/*virtual*/ bool UIBattlePopupDeleteItem::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIBattlePopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	m_item_id = *( static_cast<T_ItemID*>( pArg1 ) );

	setting_ui();

	GetSlide()->ForceEnd();

	return true;
}

void	UIBattlePopupDeleteItem::OnLoadAllScenes()
{
	UIBattlePopupBase::OnLoadAllScenes();

	m_pImageCtrl = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet" );
}

void	UIBattlePopupDeleteItem::OnUnloadAllScenes()
{
	UIBattlePopupBase::OnUnloadAllScenes();

	m_pImageCtrl->RestoreSizePos();
	I3_SAFE_RELEASE(m_pImageCtrl);
}

void UIBattlePopupDeleteItem::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	ItemConsumer::instance()->process_game_event_in_query(event, arg, UserData);
}


bool UIBattlePopupDeleteItem::OnKey_Enter() 
{ 
	clicked( inner::OK );  

	return UIBattlePopupBase::OnKey_Enter();
}

bool UIBattlePopupDeleteItem::OnKey_Escape() 
{ 
	clicked( inner::CANCEL );  

	return UIBattlePopupBase::OnKey_Escape();
}


/***********************//***********************/
void UIBattlePopupDeleteItem::setting_ui()
{
	LuaState * L = 0;

	L = _CallLuaFunction( "SetDeleteMode");
	i3Lua::PushBoolean( L, true);
	_EndLuaFunction( L, 1);

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, m_item_id);

	i3::wstring wstrTermCount;
	usf::GetShopPeriodCountString_only_Item(m_item_id, wstrTermCount);	

	i3::rc_wstring wstrItemDescription;
	usf::GetShopItemDescription(wstrItemDescription, m_item_id);

	L = _CallLuaFunction( "SetItemInfo");
	i3Lua::PushStringUTF16To8(L, wstrItemName );
	i3Lua::PushStringUTF16To8(L, wstrTermCount );
	i3Lua::PushStringUTF16To8(L, wstrItemDescription.c_str(), wstrItemDescription.size() );
	_EndLuaFunction( L, 3);

	GetItemImageMgr()->SelectImage(m_pImageCtrl, m_item_id);
}