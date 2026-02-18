#include "pch.h"
#include "UIPopupDomiRound.h"

#include "UIMainFrame.h"
#include "ItemImageMgr.h"
#include "MyRoomInfoContext.h"
#include "UIUtil.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupDomiRound_Change( LuaState * L)
	{
		UIPopupDomiRound * pThis = static_cast<UIPopupDomiRound*>(GameUI::GetPopup(UPW_DOMI_ROUND));
		if( pThis != nullptr)
			pThis->ChangeRound();
		return 0;
	}

	int UIPopupDomi_Confirm( LuaState * L)
	{
		UIPopupDomiRound * pThis = static_cast<UIPopupDomiRound*>(GameUI::GetPopup(UPW_DOMI_ROUND));
		if( pThis != nullptr)
			pThis->Confirm();
		return 0;
	}

	int UIPopupDomi_Cancel( LuaState * L)
	{
		g_pFramework->GetUIMainframe()->ClosePopup( UPW_DOMI_ROUND);
		return 0;
	}
}
LuaFuncReg UIPopupDomiRound_Glue[] =
{
	{"change_round",	UIPopupDomiRound_Change },
	{"confirm",			UIPopupDomi_Confirm },
	{"cancel",			UIPopupDomi_Cancel },
	{nullptr,				nullptr					}
};

namespace Domination_Util
{
	//적절한 아이템을 찾는다.
	RoundItem FindDomiRoundItem( INT32 start_round)
	{
		//
		if( start_round == 1) 
			return RoundItem( nullptr, 0); 
		//

		i3::vector<RoundItem> list;

		CInvenList* pInven = CInvenList::i();
		UINT32 itemCnt = pInven->GetItemCount( INVEN::DB_CASHITEM);
		for( size_t i = 0; i < itemCnt; ++i)
		{//
			const INVEN_BUFFER* item = pInven->GetItem( INVEN::DB_CASHITEM, i);
			if( GET_ITEM_TYPE( item->_ui32ItemID) == ITEM_TYPE_DOMIITEM)
			{
				DomiShopItemInfo* item_info = (DomiShopItemInfo*)g_pShopItemInfoDataBase->getInfo( item->_ui32ItemID);
				I3ASSERT( item_info != nullptr);
				list.push_back( RoundItem( item, item_info->GetStartRound()));
			}
		}

		//sort 
		struct cmp
		{
			bool operator()( const RoundItem& i1, const RoundItem& i2)
			{
				return i1.round < i2.round;
			}
		};

		RoundItem find = RoundItem( nullptr, 0);
		i3::sort( list.begin(), list.end(), cmp());
		for( size_t i = 0; i < list.size(); ++i)
		{
			if( start_round <= list[i].round)
			{
				find = list[i];
				break;
			}
		}

		return find;
	}
};
namespace dui = Domination_Util;


I3_CLASS_INSTANCE( UIPopupDomiRound);

UIPopupDomiRound::UIPopupDomiRound() : m_img_ctrl(nullptr), m_cur_sel(0)
{
//	i3::safe_destroy_instance( m_img_ctrl);
}

UIPopupDomiRound::~UIPopupDomiRound()
{
}

void	UIPopupDomiRound::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);
}

void	UIPopupDomiRound::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);
}

/*virtual*/ void UIPopupDomiRound::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	i3UIScene * pScene = GetScene( 0);
	if( pScene)
	{
		LuaState* L = UILua::CallLuaFunction( pScene, "get_img_ctrl");
		i3UIButtonImageSet* btnset = reinterpret_cast< i3UIButtonImageSet* >(UILua::ReturnLuaPointer(L, 0));
		m_img_ctrl = GetItemImageMgr()->CreateImgSetCtrl( btnset);
	}
}

void	UIPopupDomiRound::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_img_ctrl);
	UIPopupBase::OnUnloadAllScenes();
}


/*virtual*/ void UIPopupDomiRound::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Domination/PBRe_Domipop_round.i3UIs", UIPopupDomiRound_Glue);
}

/*virtual*/ bool UIPopupDomiRound::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	//진행 가능 Round 검사.
	INT32 highest_round = 0;

	CInvenList* pInven = CInvenList::i();
	UINT32 itemCnt = pInven->GetItemCount( INVEN::DB_CASHITEM);
	for( size_t i = 0; i < itemCnt; ++i)
	{//
		const INVEN_BUFFER* item = pInven->GetItem( INVEN::DB_CASHITEM, i);
		if( GET_ITEM_TYPE( item->_ui32ItemID) == ITEM_TYPE_DOMIITEM)
		{
			DomiShopItemInfo* item_info = (DomiShopItemInfo*)g_pShopItemInfoDataBase->getInfo( item->_ui32ItemID);
			I3ASSERT( item_info != nullptr);

			if( highest_round <= item_info->GetStartRound())
				highest_round = item_info->GetStartRound();
		}
	}

	i3::stack_wstring wstr_item_list;
	i3::stack_wstring wbuf;

	for( INT32 i = 0; i <= (highest_round/10); ++i)
	{
		i3::sprintf( wbuf, L"Round %d ~", (i*10)+1);

		wstr_item_list += wbuf;
		wstr_item_list += ';';
	}

	wstr_item_list.erase( wstr_item_list.size() - 1);
		
	INT32 cur_sel = highest_round;
	LuaState* L = _CallLuaFunction("InitRoundListBox");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L , wstr_item_list );
		i3Lua::PushInteger(L , cur_sel/10);

		_EndLuaFunction( L, 2);
	}


	//------------------------------------------------------------------------------------------------
	RefreshFrame( cur_sel/10);

	return true;
}

/*virtual*/ void UIPopupDomiRound::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();
}

/*virtual*/ bool UIPopupDomiRound::OnExitStart( void)
{
	return UIPopupBase::OnExitStart();
}

/*virtual*/ bool UIPopupDomiRound::OnKey_Enter()
{ 
	Confirm();
	return true;
}

/*virtual*/ bool UIPopupDomiRound::OnKey_Escape()
{
	GameUI::ClosePopup(UPW_DOMI_ROUND);
	return true;
}

/*virtual*/ void UIPopupDomiRound::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent(event, Arg, UserData);
	
	switch( event )
	{
	case EVENT_CHANGE_READYROOM_INFO:
		GameEventSender::i()->SetEventOnce( EVENT_READY_BATTLE);
		GameUI::ClosePopup( UPW_DOMI_ROUND);
		break;
	}
}

void UIPopupDomiRound::RefreshFrame( INT32 cur_sel)
{
	bool have_item = false;

	i3::stack_wstring wstr_item_desc;

	dui::RoundItem find_item = dui::FindDomiRoundItem( (cur_sel*10) + 1);
	if( find_item.item)
	{
		have_item = true;
		i3::sprintf( wstr_item_desc, GAME_RCSTRING("STR_POPUP_READYROOM_TIP_4"), 1, find_item.item->_ui32ItemArg);

		//img
		if( m_img_ctrl)
			GetItemImageMgr()->SelectImage( m_img_ctrl, find_item.item->_ui32ItemID);
	}

	LuaState* L = _CallLuaFunction("InitItemFrame");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L , have_item);
		i3Lua::PushStringUTF16To8(L , wstr_item_desc.c_str(), wstr_item_desc.size());

		_EndLuaFunction( L, 2);
	}

	m_cur_sel = cur_sel;
}

void UIPopupDomiRound::ChangeRound()
{
	LuaState * L = _CallLuaFunction("GetCurSelRound");
	if( L != nullptr)
	{
		INT32 cur_sel = _ReturnLuaInteger(L , 0);
	
		RefreshFrame( cur_sel);
	}
}

void UIPopupDomiRound::Confirm()
{
	//방정보 변경 후 게임 시작??

	ROOM_INFO_BASIC InfoBasic;
	MyRoomInfoContext::i()->getMyRoomInfo(&InfoBasic);

	/*if( InfoBasic._i16BeginRound != ((m_cur_sel*10) + 1))
	{
		InfoBasic._i16BeginRound = static_cast<INT16>((m_cur_sel*10) + 1);
				
		GameEventSender::i()->SetEvent( EVENT_CHANGE_ROOMINFO_N, &InfoBasic);

		//변경 패킷 받은 후 팝업 창 닫습니다.
	}
	else
	{
		GameEventSender::i()->SetEventOnce( EVENT_READY_BATTLE);
		GameUI::ClosePopup( UPW_DOMI_ROUND);
	}*/
}