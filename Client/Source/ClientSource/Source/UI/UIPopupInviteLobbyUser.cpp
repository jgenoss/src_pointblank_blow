#include "pch.h"
#include "UIPopupInviteLobbyUser.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "InviteContext.h"


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupInviteLobbyUser_SetCheckUser( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg( L, 1);

		UIPopupInviteLobbyUser * pThis = static_cast<UIPopupInviteLobbyUser*>(GameUI::GetPopup(UPW_INVITELOBBYUSER));
		if( pThis != nullptr)
		{
			bool bAllCheck = pThis->SetCheckUser( slot);
			i3Lua::PushInteger( L, bAllCheck ? 1 : 0 );
			return 1;
		}
		return 0;
	}

	int UIPopupInviteLobbyUser_CheckAllUser(LuaState * L )
	{
		bool bChecked = i3Lua::GetBooleanArg(L , 1);
		UIPopupInviteLobbyUser * pThis = static_cast<UIPopupInviteLobbyUser*>(GameUI::GetPopup(UPW_INVITELOBBYUSER));
		if( pThis != nullptr)
			pThis->CheckAllUser(bChecked);
		return 0;
	}
}

LuaFuncReg UIPopupInviteLobbyUser_Glue[] = {
	{"SetCheckUser",		UIPopupInviteLobbyUser_SetCheckUser},
	{"CheckAllUser",		UIPopupInviteLobbyUser_CheckAllUser},

	{nullptr,					nullptr}
};

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPopupInviteLobbyUser);//, UIPopupBase);

UIPopupInviteLobbyUser::UIPopupInviteLobbyUser()
{
	m_allocCount = 0;
	m_memCount = 0;
	if(g_pEnvSet->GetUIVersion() == UFV_1_5)
	{
		m_InviteSlotCount = 9;
	}
	else
	{
		m_InviteSlotCount = 8;
	}
	m_pUserInfo = nullptr;
	
	_ResizeUserInfoBuffer( 16);
}

UIPopupInviteLobbyUser::~UIPopupInviteLobbyUser()
{
	delete [] m_pUserInfo;
}

void UIPopupInviteLobbyUser::_ResizeUserInfoBuffer( INT32 size)
{
	if( size > m_memCount)
	{	
		INVITE_SLOTINFO * pNewInfo = new INVITE_SLOTINFO[ m_memCount + 16  ];

		//	(INVITE_SLOTINFO*) i3MemAlloc( sizeof( INVITE_SLOTINFO) * (m_memCount + 16));

		if( m_memCount > 0)
		{
//			i3mem::Copy( pNewInfo, m_pUserInfo, sizeof( INVITE_SLOTINFO) * m_memCount);
			i3::copy(m_pUserInfo, m_pUserInfo + m_memCount, pNewInfo);
		}

		m_memCount += 16;

		delete [] m_pUserInfo;
		m_pUserInfo = pNewInfo;
	}

}

INVITE_SLOTINFO * UIPopupInviteLobbyUser::_AllocUserInfo( void)
{
	INT32 freeIdx = _FindFreeUserInfo();
	if( freeIdx != -1)
		return &m_pUserInfo[ freeIdx];

	_ResizeUserInfoBuffer( m_allocCount + 1);

	INVITE_SLOTINFO * pPointer = &m_pUserInfo[m_allocCount];
	pPointer->Reset();
	pPointer->_Alloc = 1;

	m_allocCount++;

	return pPointer;
}

INT32 UIPopupInviteLobbyUser::_FindFreeUserInfo( void)
{
	INT32 i;

	for( i = 0; i < m_allocCount; i++)
	{
		if( m_pUserInfo[ i]._Alloc == 0)
			return i;
	}

	return -1;
}

void UIPopupInviteLobbyUser::_FreeUserInfo( INT32 idx)
{
	if( idx != -1)
	{
		m_pUserInfo[ idx]._Alloc = 0;
	}
}

INT32 UIPopupInviteLobbyUser::_FindUserInfo( const USER_INFO_INVITE * pSrc)
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

bool UIPopupInviteLobbyUser::_IsCheckedAllUser(void)
{
	INT32 nCount = (INT32)(InviteContext::i()->GetUserInfoInviteCount());

	INVITE_SLOTINFO * pInfo = nullptr;
	for(INT32  i = 0 ; i < nCount ; ++i)
	{
		pInfo = m_UserList[ i];
		
		if(pInfo->_Chk == 0)
			return false;
	}

	return true;
}

/*virtual*/ void UIPopupInviteLobbyUser::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PBRe_PopUp_Invite.i3UIs", UIPopupInviteLobbyUser_Glue);
}

void UIPopupInviteLobbyUser::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	switch( event)
	{
	case EVENT_GET_INVITE_LOBBY_USER:	UpdateInviteList();	break;
	}
}

void UIPopupInviteLobbyUser::OnEntranceEnd( void)
{
	UIPopupBase::OnEntranceEnd();

	UpdateInviteList();
}

static void s_ClanMarkProc( i3UIListView_Cell * pCell, void * pData)
{

}

void UIPopupInviteLobbyUser::UpdateInviteList(void)
{

	LuaState * L;
	INVITE_SLOTINFO * pInfo;

	INT32 nCount = (INT32)(InviteContext::i()->GetUserInfoInviteCount());

	m_UserList.clear();

	if( nCount > 0)
	{	
		for(INT32 i = 0; i < nCount; i++)
		{
			const USER_INFO_INVITE* pUserInfo = (USER_INFO_INVITE*) InviteContext::i()->GetUserInfoInvite(i);

			INT32 userIdx = _FindUserInfo( pUserInfo);

			if( userIdx == -1)
			{
				INVITE_SLOTINFO * pNewInfo = _AllocUserInfo();
				pNewInfo->_Info = *pUserInfo;
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

	const INT32 invitecount = m_InviteSlotCount;
	L = _CallLuaFunction( "ClearList");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, MAX(nCount, invitecount));
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
			i3Lua::PushInteger(L, i);						// slot
			i3Lua::PushInteger( L, (pInfo->_Chk ? 1 : 0 ));	// Check Box State
			i3Lua::PushInteger(L, pInfo->_Info._rank);		// rank
			i3Lua::PushStringUTF16To8(L, pInfo->_Info._wstr_nick.c_str(), pInfo->_Info._wstr_nick.size());	// nick

			_EndLuaFunction( L, 4);
		}
	}

	for( size_t i = m_UserList.size(); i < (size_t)invitecount; i++)
	{
		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, INT32(i) );		// slot
			i3Lua::PushInteger( L, -1);				// Check Box State
			i3Lua::PushInteger(L, -1);				// rank
			i3Lua::PushStringUTF16To8(L, L"");			// nick
			_EndLuaFunction( L, 4);
		}
	}	
}

bool	UIPopupInviteLobbyUser::OnKey_Enter( void)
{
	// 기본클래스 호출 하지 않음..

	if( UIPopupInviteLobbyUser::OnOKButton())
	{
		_OnClose();
		return true;
	}

	return false;
}


/*virtual*/ bool UIPopupInviteLobbyUser::OnOKButton( void)
{
	
	UINT32 inviteCount = 0;

	for(size_t i = 0; i < m_UserList.size(); i++)
	{
		INVITE_SLOTINFO * pInfo = m_UserList[ i];

		if( pInfo->_Chk != 0)
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

	return true;
}

bool UIPopupInviteLobbyUser::SetCheckUser( INT32 slot)
{
	if( slot > -1 && slot < (INT32)m_UserList.size())
	{
		INVITE_SLOTINFO * pInfo = m_UserList[ slot];

		pInfo->_Chk = pInfo->_Chk ? 0 : 1;
	}

	return _IsCheckedAllUser();
}

void UIPopupInviteLobbyUser::CheckAllUser( bool bChecked)
{
	INT32 nCount = (INT32)(InviteContext::i()->GetUserInfoInviteCount());
	
	LuaState * L = _CallLuaFunction("setCheckAllUser");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L , nCount);
		i3Lua::PushBoolean(L , bChecked);
		_EndLuaFunction(L , 2);
	}

	// 전체를 제어하기때문에 이렇게합니다.
	for(INT32 i = 0 ; i < nCount ; i++)
	{
		INVITE_SLOTINFO * pInfo = m_UserList[ i];
		pInfo->_Chk = bChecked ? 1 : 0;
	}
}