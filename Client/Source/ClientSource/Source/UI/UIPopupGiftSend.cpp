#include "pch.h"
#include "UIPopupGiftSend.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "BuyUtil.h"
#include "PurchaseMgr.h"

#include "ShopContext.h"
#include "../UserInfoContext.h"
#include "../CommunityContext.h"
#include "../GameGUI.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"


I3_CLASS_INSTANCE( UIPopupGiftSend);

extern "C" 
{
	int UIPopupGiftSend_SendGift( LuaState * L)
	{
		UIPopupGiftSend * pThis = static_cast<UIPopupGiftSend*>(GameUI::GetPopup(UPW_GIFT_SEND));
		if( pThis != nullptr)
			pThis->SendGift();
		return 0;
	}

	int UIPopupGiftSend_Close( LuaState * L)
	{
		UIPopupGiftSend * pThis = static_cast<UIPopupGiftSend*>(GameUI::GetPopup(UPW_GIFT_SEND));
		if( pThis != nullptr)
			pThis->OnClose();
		return 0;
	}

	int UIPopupGiftSend_ClickTab( LuaState * L)
	{
		INT32 i32Tab = i3Lua::GetIntegerArg(L, 1);
		UIPopupGiftSend * pThis = static_cast<UIPopupGiftSend*>(GameUI::GetPopup(UPW_GIFT_SEND));
		if( pThis != nullptr)
			pThis->ClickTab(i32Tab);

		return 0;
	}

	int UIPopupGiftSend_SetEditFocus( LuaState * L)
	{
		bool bSetFocus = i3Lua::GetBooleanArg(L, 1);
		UIPopupGiftSend * pThis = static_cast<UIPopupGiftSend*>(GameUI::GetPopup(UPW_GIFT_SEND));
		if( pThis != nullptr)
			pThis->SetEditFocus(bSetFocus ? true : false);

		return 0;
	}

	int UIPopupGiftSend_SetSelectIdx( LuaState * L)
	{
		INT32 i32SelectIdx = i3Lua::GetIntegerArg(L, 1);
		UIPopupGiftSend * pThis = static_cast<UIPopupGiftSend*>(GameUI::GetPopup(UPW_GIFT_SEND));
		if( pThis != nullptr)
			pThis->SetSelectIdx(i32SelectIdx);

		return 0;
	}
}

LuaFuncReg UIPopupGiftSend_Glue[] =
{
	{ "SendGift",					UIPopupGiftSend_SendGift},
	{ "Close",						UIPopupGiftSend_Close},
	{ "ClickTab",					UIPopupGiftSend_ClickTab},
	{ "SetEditFocus",				UIPopupGiftSend_SetEditFocus},
	{ "SetSelectIdx",				UIPopupGiftSend_SetSelectIdx},
	{ nullptr,							nullptr}
};

/////////////////////////////////////////////////////////////////////////////

void UIPopupGiftSend::GiftTargetUser::CopyFrom( const CLAN_MEMBER_NODE * pSrc)
{
	_i64UID = pSrc->_uid;
	_wstrNick = pSrc->_wstr_nick;
	_Rank = pSrc->_rank;
	_FriendState = (FRIEND_STATE)GET_FRIEND_STATE(pSrc->_state);
	_NickColor = *GameUI::GetNickNameColor( pSrc->_nickcolor);

	SetNickColot();
}

void UIPopupGiftSend::GiftTargetUser::CopyFrom( const FRIEND_NODE * pSrc)
{
	_i64UID = pSrc->_uid;
	_wstrNick = pSrc->_wstr_nick;
	_Rank = pSrc->_rank;
	_FriendState = (FRIEND_STATE)GET_FRIEND_STATE(pSrc->_state);

	_NickColor = *GameUI::GetNickNameColor( pSrc->_nickcolor);
	SetNickColot();
}

void UIPopupGiftSend::GiftTargetUser::SetNickColot(void)
{
	switch(_FriendState)
	{
	case FRIEND_STATE_ONLINE :
	case FRIEND_STATE_LOBBY:
	case FRIEND_STATE_ROOM :
		break;
	default:
		{
			// offline
			i3Color::Copy( &_NickColor, GameGUI::GetColor(GCT_DEFAULT));
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////

UIPopupGiftSend::UIPopupGiftSend(void) : m_i32Tab(TAB_FRIEND), m_i32SelectIdx(-1),
m_isDefaultMsg(false), m_pItemImgSet(nullptr), m_pEditBox(nullptr), m_pOKButton(nullptr)
{
}

UIPopupGiftSend::~UIPopupGiftSend(void)
{
	if(m_pItemImgSet != nullptr)
	{
		m_pItemImgSet->RestoreSizePos();
		I3_SAFE_RELEASE(m_pItemImgSet);
	}

	m_TargetUserList.clear();
}

void UIPopupGiftSend::_InitGiftPopup(void)
{
	_SetGiftItemInfo();
	_SetClanTabActive();

	m_isDefaultMsg = true;
}

void UIPopupGiftSend::_SetGiftItemInfo(void)
{
	const buy::DisplayedGoods* pDisplayedGoods = buy::GetDisplayedGoods_ByPurchaseRoute(buy::PurchaseRoute_Immediate, 0);
		
	const SHOP_GOODS * pGoods = pDisplayedGoods->GetCurrentGoods(); 

	I3ASSERT(pGoods != nullptr);

	if(nullptr != m_pItemImgSet)
		GetItemImageMgr()->SelectImage(m_pItemImgSet, pGoods);

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, pGoods);

	i3::wstring wstrPeriod;
	usf::GetShopPeriodCountString(pGoods, wstrPeriod);

	LuaState * L = _CallLuaFunction("SetGiftItemInfo");
	if(nullptr != L)
	{
		i3Lua::PushStringUTF16To8(L, wstrItemName);
		i3Lua::PushStringUTF16To8(L, wstrPeriod);
		_EndLuaFunction(L , 2);
	}
}

void UIPopupGiftSend::_SetClanTabActive(void)
{
	bool bClanMember = (UserInfoContext::i()->GetMyClanState() != CLAN_MEMBER_LEVEL_UNKNOWN);

	LuaState * L = _CallLuaFunction("SetClanTabActive");
	if(nullptr != L)
	{
		i3Lua::PushBoolean(L, bClanMember ? true : false);
		_EndLuaFunction(L , 1);
	}
}

void UIPopupGiftSend::_UpdateUserList(void)
{
	m_i32SelectIdx = -1;
	m_pOKButton->disable_btn();

	m_TargetUserList.clear();
	switch(m_i32Tab)
	{
	case TAB_FRIEND	: _BuildFriendList();	break;
	case TAB_CLAN	: _BuildClanList();		break;
	default			: break;
	}

	_SetUserSlot();
}

void UIPopupGiftSend::_BuildFriendList(void)
{
	for( UINT32 idx = 0; idx  < CCommunityContext::i()->GetFriendCount(); ++idx)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( idx);
		if(nullptr == node)	continue;
	
		// 친구 요청 대기중인 유저는 제외.
		if( FRIEND_STATE_OFFLINE > GET_FRIEND_STATE(node->_state) ) continue;

		GiftTargetUser info;
		info.CopyFrom( node);

		m_TargetUserList.push_back( info);
	}
}

void UIPopupGiftSend::_BuildClanList(void)
{
	I3ASSERT( CCommunityContext::i() != nullptr);

	for( UINT32 idx = 0; idx < CCommunityContext::i()->GetClanMemberCount(); ++idx)
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( idx);
		
		if(nullptr == node) continue;

		GiftTargetUser info;
		info.CopyFrom( node);

		m_TargetUserList.push_back( info );
	}
}

void UIPopupGiftSend::_SetUserSlot(void)
{
	size_t count = m_TargetUserList.size();

	LuaState *	L = _CallLuaFunction("ClearUserListBox");
	if( L != nullptr)
	{
		//if( count < LISTVIEW_MAX)
//			count = LISTVIEW_MAX;

		i3Lua::PushInteger(L, count);
		_EndLuaFunction( L, 1);
	}

	for( size_t idx = 0; idx < count; ++idx)
	{
		if( idx < m_TargetUserList.size())
		{
			i3::vector<GiftTargetUser>::iterator iter = &m_TargetUserList.at(idx);
			_SetUserData( idx, iter);
		}
		else
		{
			_SetUserData( idx, nullptr);
		}
	}
}

void UIPopupGiftSend::_SetUserData(INT32 idx, GiftTargetUser * pInfo)
{
	INT32	i32ConnectState = -1;
	INT32	i32Rank = -1;
	i3::rc_wstring	wstrRankToolTip;
	i3::rc_wstring	wstrNick;

	I3COLOR * pColor = GameGUI::GetColor( GCT_DEFAULT);

	if(nullptr != pInfo)
	{
		switch(pInfo->_FriendState)
		{
		case FRIEND_STATE_OFFLINE:
			i32ConnectState = 1;
			break;
		case FRIEND_STATE_ONLINE:
		case FRIEND_STATE_LOBBY:
		case FRIEND_STATE_ROOM:
			i32ConnectState = 0;
			break;
		}

		i32Rank = pInfo->_Rank;
		if( i32Rank >= RANK_MAX) i32Rank = RANK_MAX-1;

		wstrRankToolTip = GetCharaRank( i32Rank);
		wstrNick = pInfo->_wstrNick;
		pColor = &pInfo->_NickColor;
	}

	LuaState * L = _CallLuaFunction( "SetUserInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, idx);
		i3Lua::PushInteger( L, i32ConnectState);
		i3Lua::PushInteger( L, i32Rank);
		i3Lua::PushStringUTF16To8( L, wstrRankToolTip);
		i3Lua::PushStringUTF16To8( L, wstrNick);
		i3Lua::PushColor( L, pColor);
		_EndLuaFunction( L, 9);
	}
}

bool UIPopupGiftSend::_IsFilter(const i3::wliteral_range& wMessageRng)
{
	if( false == g_pFramework->IsFilterOK( wMessageRng ) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다.*/
		g_pFramework->SetFocus( m_pEditBox );
		return false;
	}

	return true;
}

/*virtual*/ void UIPopupGiftSend::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_pItemImgSet = GetItemImageMgr()->CreateImgSetCtrl(pScene , "BtnImgSet_Item");

	m_pEditBox = static_cast<i3UIEditBox*>(pScene->FindChildByName("EditBox_Msg"));
	I3ASSERT(m_pEditBox != nullptr);

	m_pOKButton = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "Btn_Send" );
	I3ASSERT(m_pOKButton != nullptr);
	m_pOKButton->set_disable_text_color(*GameGUI::GetColor(GCT_INVALID_C) );
}

/*virtual*/ void UIPopupGiftSend::_ClearAtUnload(i3UIScene * pScene)
{
	if(m_pItemImgSet != nullptr)
	{
		m_pItemImgSet->RestoreSizePos();
		I3_SAFE_RELEASE(m_pItemImgSet);
	}

	m_pEditBox = nullptr;
	m_pOKButton = nullptr;

	UIPopupBase::_ClearAtUnload(pScene);
}

/*virtual*/ void UIPopupGiftSend::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene("Scene/Popup/PBRe_PopUp_Gift_Send.i3UIs", UIPopupGiftSend_Glue);
}

/*virtual*/ bool UIPopupGiftSend::OnEntranceStart(void * pArg1 , void * pArg2)
{
	if(	UIPopupBase::OnEntranceStart(pArg1, pArg2) == false )
		return false;

	_InitGiftPopup();

	g_pFramework->SetFocus( this );

	return true;
}

/*virtual*/ void UIPopupGiftSend::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	m_i32Tab = TAB_FRIEND;
	_UpdateUserList();
}

void UIPopupGiftSend::ClickTab(INT32 i32Tab)
{
	m_i32Tab = i32Tab;
	_UpdateUserList();
}

void UIPopupGiftSend::SetEditFocus(bool isSetFocus)
{
	if(isSetFocus)
	{
		// SetFocus 처리 
		// 디폴트 메세지 표시중일때 클릭하면 EditBox Clear
		if(m_isDefaultMsg)
		{
			m_pEditBox->SetText( L"");
			m_pEditBox->GetIME()->SetInputText( L"", 0);
			m_isDefaultMsg = false;
		}
	}
	else
	{
		// Kill Focus 처리
		// 작성된 메세지가 없으면 디폴트 메세지로 설정
		if(i3::generic_string_size( m_pEditBox->getText() ) == 0)
		{
			
			i3::rc_wstring wstrDefaultMsg = i3::format_string(L"%s\n\n\n\n\n%s", GAME_RCSTRING("STR_POPUP_GIFT_SELECT_MESSAGE_DEFAULT"), GAME_RCSTRING("STR_POPUP_GIFT_SELECT_MESSAGE_NOTICE") );

			m_pEditBox->SetText(wstrDefaultMsg);
			m_isDefaultMsg = true;
		}
	}
}

void UIPopupGiftSend::SetSelectIdx(INT32 i32SelectIdx)
{
	if( (UINT32)i32SelectIdx < m_TargetUserList.size() )
	{
		m_i32SelectIdx = i32SelectIdx;
		m_pOKButton->enable_btn();
	}
	else
	{
		m_i32SelectIdx = -1;
		m_pOKButton->disable_btn();
	}
}

void UIPopupGiftSend::SendGift(void)
{
	if(m_i32SelectIdx < 0 || (UINT32)m_i32SelectIdx > m_TargetUserList.size() )
	{
		GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INPUT_RECEIVER") );
		return;
	}

	buy::DisplayedGoodsList purchase;
	buy::GatherDisplayedGoods_ByPurchaseRoute(buy::PurchaseRoute_Immediate, purchase);

	i3::vector<GiftTargetUser>::iterator iter = &m_TargetUserList.at(m_i32SelectIdx);

	i3::rc_wstring gift_message = GAME_RCSTRING("STR_POPUP_GIFT_SELECT_MESSAGE_DEFAULT");

	if(m_isDefaultMsg == false)
	{
		const i3::const_wchar_range& wstr_rng = m_pEditBox->getTextRange();

		gift_message.assign(wstr_rng.begin(), wstr_rng.end());
//		i3::utf16_to_mb( m_pEditBox->getText(), gift_message );
	}

	if( _IsFilter(gift_message) == true)
		PurchaseMgr::i()->Gift(this, buy::PurchaseRoute_Immediate, purchase, iter->_wstrNick, gift_message);
}

void UIPopupGiftSend::OnClose(void)
{
	GetMainFrame()->ClosePopup(UPW_GIFT_SEND);
}