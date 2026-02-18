#include "pch.h"
#include "UIPopupInviteLobbyUser_V11.h"

#include "UI/UIMainFrame.h"
#include "../UI/UIUtil.h"

#include "InviteContext.h"

static UIPopupInviteLobbyUser_V11 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPopupInviteLobbyUser_V11*) pFrame->GetPopup( UPW_INVITELOBBYUSER);
}


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupInviteLobbyUser_Close( LuaState * L)
	{

		GetThis()->GetMainFrame()->TogglePopup( UPW_INVITELOBBYUSER);
	
		return 0;
	}

	int	UIPopupInviteLobbyUser_SendInviteMessage( LuaState* L )
	{
		GetThis()->SendInviteMessage();
		return 0;
	}
}

LuaFuncReg UIPopupInviteLobbyUser_V11_Glue[] = {
	{"Close",		UIPopupInviteLobbyUser_Close},
	{"SendInviteMessage",	UIPopupInviteLobbyUser_SendInviteMessage},

	{nullptr,					nullptr}
};

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPopupInviteLobbyUser_V11);

UIPopupInviteLobbyUser_V11::UIPopupInviteLobbyUser_V11() : m_ListBox(0)
{
	m_allocCount = 0;
	m_memCount = 0;
	m_pUserInfo = nullptr;
	
	_ResizeUserInfoBuffer( 16);
}

UIPopupInviteLobbyUser_V11::~UIPopupInviteLobbyUser_V11()
{
	I3MEM_SAFE_FREE( m_pUserInfo);
}

void UIPopupInviteLobbyUser_V11::_ResizeUserInfoBuffer( INT32 size)
{
	if( size > m_memCount)
	{	
		INVITE_SLOTINFO * pNewInfo = (INVITE_SLOTINFO*) i3MemAlloc( sizeof( INVITE_SLOTINFO) * (m_memCount + 16));

		if( m_memCount > 0)
			i3mem::Copy( pNewInfo, m_pUserInfo, sizeof( INVITE_SLOTINFO) * m_memCount);

		m_memCount += 16;

		I3MEM_SAFE_FREE( m_pUserInfo);
		m_pUserInfo = pNewInfo;
	}

}

INVITE_SLOTINFO * UIPopupInviteLobbyUser_V11::_AllocUserInfo( void)
{
	INT32 freeIdx = _FindFreeUserInfo();
	if( freeIdx != -1)
		return &m_pUserInfo[ freeIdx];

	_ResizeUserInfoBuffer( m_allocCount + 1);

	INVITE_SLOTINFO * pPointer = &m_pUserInfo[m_allocCount];
	i3mem::FillZero( pPointer, sizeof(INVITE_SLOTINFO));
	pPointer->_Alloc = 1;

	m_allocCount++;

	return pPointer;
}

INT32 UIPopupInviteLobbyUser_V11::_FindFreeUserInfo( void)
{
	INT32 i;

	for( i = 0; i < m_allocCount; i++)
	{
		if( m_pUserInfo[ i]._Alloc == 0)
			return i;
	}

	return -1;
}

void UIPopupInviteLobbyUser_V11::_FreeUserInfo( INT32 idx)
{
	if( idx != -1)
	{
		m_pUserInfo[ idx]._Alloc = 0;
	}
}

INT32 UIPopupInviteLobbyUser_V11::_FindUserInfo( const USER_INFO_INVITE * pSrc)
{

	INVITE_SLOTINFO * pInfo;

	for(size_t i = 0; i < m_UserList.size(); i++)
	{
		pInfo = m_UserList[ i];

		if( pInfo->_Alloc == 0)
			continue;

		if( pInfo->_Info._idxSession == pSrc->_idxSession)
			return i;
	}

	return -1;
}

/*virtual*/ void UIPopupInviteLobbyUser_V11::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene );

	m_ListBox = (i3UIListView_Box *)(pScene->FindChildByName("i3UIListView_Box"));
}


/*virtual*/ void UIPopupInviteLobbyUser_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PointBlankRe_ReadyRoom_invite.i3UIs", UIPopupInviteLobbyUser_V11_Glue);
}

void UIPopupInviteLobbyUser_V11::ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	switch( event)
	{
	case EVENT_GET_INVITE_LOBBY_USER:	UpdateInviteList();	break;
	}
}

void UIPopupInviteLobbyUser_V11::OnEntranceEnd( void)
{
	UIPopupBase::OnEntranceEnd();

	UpdateInviteList();
}

static void s_ClanMarkProc( i3UIListView_Cell * pCell, void * pData)
{

}

bool UIPopupInviteLobbyUser_V11::OnExitStart()
{
	if (UIPopupBase::OnExitStart() == false)
		return false;

	m_allocCount = 0;
	return true;
}



void UIPopupInviteLobbyUser_V11::UpdateInviteList(void)
{
	
	LuaState * L;
	INVITE_SLOTINFO * pInfo;

	INT32 nCount = (INT32)(InviteContext::i()->GetUserInfoInviteCount());

	m_UserList.clear();

	if( nCount > 0)
	{	
		for( INT32 i = 0; i < nCount; i++)
		{
			const USER_INFO_INVITE* pUserInfo = (USER_INFO_INVITE*) InviteContext::i()->GetUserInfoInvite(i);

			INT32 userIdx = _FindUserInfo( pUserInfo);

			if( userIdx == -1)
			{
				INVITE_SLOTINFO * pNewInfo = _AllocUserInfo();

				i3mem::Copy( &pNewInfo->_Info, pUserInfo, sizeof( USER_INFO_INVITE));
				m_UserList.push_back( pNewInfo);
			}
			else
			{
				pInfo = m_UserList[ userIdx];
				pInfo->_Deleted = 0;
			}
		}

		for(size_t i = 0; i < m_UserList.size(); i++)
		{
			pInfo = m_UserList[ i];

			if( pInfo->_Deleted)
			{
				pInfo->_Alloc = 0;
			}
		}
	}

	L = _CallLuaFunction( "ClearList");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, MAX(nCount, INVITE_SLOTCOUNT));
		_EndLuaFunction( L, 1);
	}

	for(size_t i = 0; i < m_UserList.size(); i++)
	{
		pInfo = m_UserList[ i];
		pInfo->_Deleted = 1;
		pInfo->_Chk = 0;

		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, i);		// slot
			i3Lua::PushInteger(L, pInfo->_Info._rank);		// rank
			i3Lua::PushStringUTF16To8(L, pInfo->_Info._wstr_nick);	// nick
			i3Lua::PushInteger( L, (pInfo->_Chk?0:1));
			_EndLuaFunction( L, 4);
		}
	}

	for( size_t i = m_UserList.size(); i < INVITE_SLOTCOUNT; i++)
	{
		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, i);		// slot
			i3Lua::PushInteger(L, -1);		// rank
			i3Lua::PushStringUTF16To8(L, L"");	// nick
			i3Lua::PushInteger( L, -1);
			_EndLuaFunction( L, 4);
		}
	}

	
}

bool	UIPopupInviteLobbyUser_V11::OnKey_Enter( void)
{
	SendInviteMessage();
	return true;
}

void	UIPopupInviteLobbyUser_V11::SendInviteMessage()
{
	
	UINT32 inviteCount = 0;

	// 정보를 변경해도 최신화가 안되기때문에,
	// 정보 초기화 하고 셋팅합니다.
	InviteContext::i()->ClearInviteCheck();

	for(INT32 i = 0; i < (INT32)m_UserList.size(); i++)
	{
		if( m_ListBox->isCheckedData(i, 2) )
		{
			InviteContext::i()->CheckInviteUserInfo(i);
			inviteCount++;
		}
	}

	if (0 < inviteCount)
	{
		GameEventSender::i()->SetEvent(EVENT_INVITE_LOBBY_USER);

		i3::rc_wstring wstr_inviteMessage;

		i3::sprintf(wstr_inviteMessage, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_SEND_INVITE"), inviteCount);/*%d명에게 초대메시지를 전송하였습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_inviteMessage);
	}

	_OnClose();
}
