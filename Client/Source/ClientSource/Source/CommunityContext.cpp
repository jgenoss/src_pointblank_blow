#include "pch.h"
#include "CommunityContext.h"
#include "GlobalVariables.h"
#include "ServerInfo.h"

#include "UI/UIUtil.h"

#include "./StageBattle/NetworkContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"


//////////////////////////////////////////////////////////////////////////

CCommunityContext::CCommunityContext()
{
	m_InvitedUserNode.Reset();
	
	i3mem::FillZero(&m_userInfoRecord, sizeof(USER_INFO_RECORD));

	m_UserSimpleInfo.Reset();
	m_UserDetailInfo.Reset();

	// 1.1 유저 상세정보
	i3mem::FillZero(&m_FindUserInfo, sizeof(NEW_FIND_USER_INFO));
	
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_AnotherUserInfo[i].Reset();
	}

#if 0	
	MakeDummyFriend();
	MakeDummyClanMember();
#endif
}

bool CCommunityContext::InsertFriend(const i3::rc_wstring& wstr_nick)
{
	if (m_friendCount >= FRIEND_MAX_COUNT) return false;
	if (NET_NICK_NAME_SIZE <= i3::generic_string_size(wstr_nick)) return false;

	GameEventSender::i()->SetEvent(EVENT_INSERT_FRIEND, wstr_nick.c_str() );

	return true;
}

bool CCommunityContext::FindUser(char* nick)
{
	if (nullptr == nick) return false;
	if (NET_NICK_NAME_SIZE <= i3::generic_string_size(nick)) return false;
	
	GameEventSender::i()->SetEvent(EVENT_FIND_USER, nick);

	return true;
}

bool CCommunityContext::AcceptViewFriend(UINT32 idx)
{
	if (idx >= m_friendCount) return false;	

	FRIEND_NODE * pNode = m_viewFriend[idx]._node;
	UINT32 nMappingIndex = m_viewFriend[idx]._realIndex;

	if (FRIEND_STATE_HOLD != GET_FRIEND_STATE(pNode->_state)) return false;

	GameEventSender::i()->SetEvent(EVENT_ACCEPT_FRIEND, &nMappingIndex);

	return true;
}

bool CCommunityContext::DeleteViewFriend(UINT32 idx)
{
	if (idx >= m_friendCount) return false;

	// Delete 성공시 friendArray에 공백이 생기는 것을 막기위해 전체 목록을 갱신한다.
	// 따라서, 성공 통보를 위한 nickname을 확보해둔다.

	FRIEND_NODE * pNode = m_viewFriend[idx]._node;
	UINT32 nMappingIndex = m_viewFriend[idx]._realIndex;
	if(pNode == nullptr)
	{
		I3TRACE("Input Twice.\n");
		return true;
	}
	i3mem::Copy(&m_lastDeleteFriend, pNode, sizeof(FRIEND_NODE));

	GameEventSender::i()->SetEvent(EVENT_DELETE_FRIEND, &nMappingIndex);
	
	m_viewFriend[idx]._node = nullptr;

	return true;
}

bool CCommunityContext::InviteViewFriend(UINT32 idx)
{
	if (idx >= m_friendCount) return false;

	UINT32 nMappingIndex = m_viewFriend[idx]._realIndex;
	
	if( GET_FRIEND_STATE(m_viewFriend[idx]._node->_state) == FRIEND_STATE_CANT_INVITE )
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_ROULETTE_ERROR_INVATE_POPUP_3"));	//MSG : 해당 유저는 초대가 불가능한 상태입니다.
		return false;
	}

	GameEventSender::i()->SetEvent(EVENT_INVITE_FRIEND, &nMappingIndex);

	return true;
}

bool CCommunityContext::InviteViewClan(UINT32 idx)
{
	// 친구와 달리 유저 인덱스를 직접 보냅니다.
	if (idx >= m_clanMemberCount) return false;

	if( GET_FRIEND_STATE(m_viewClanMember[idx]._node->_state) == FRIEND_STATE_CANT_INVITE )
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_ROULETTE_ERROR_INVATE_POPUP_3"));	//MSG : 해당 유저는 초대가 불가능한 상태입니다.
		return false;
	}

	GameEventSender::i()->SetEvent(EVENT_INVITE_CLAN, &m_viewClanMember[idx]._node->_uid);

	return true;
}

bool CCommunityContext::InviteNormalUser(void)
{
	switch( m_UserSimpleInfo._UserInfo.m_ui8SubTask )
	{
	case GAME_TASK_SUB_GACHA:
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_ROULETTE_ERROR_INVATE_POPUP_3"));	//MSG : 해당 유저는 초대가 불가능한 상태입니다.
		return false;
		break;
	}

	PACKET_COMMUNITY_USER_INVITED_REQ ReqPacket;

	ReqPacket.m_i64UID = NetworkContext::i()->UID;
	ReqPacket.m_SessionIdx = NetworkContext::i()->GetSessionIndex();

	ReqPacket.m_i64InviteUser = m_UserSimpleInfo._UserInfo.m_i64FindUID;

	GameEventSender::i()->SetEvent(EVENT_INVITE_NORMAL_USER, &ReqPacket);

	return true;
}

bool CCommunityContext::AddFriend(const FRIEND_NODE* node)
{
	if (nullptr == node) return false;
	if (m_friendCount >= FRIEND_MAX_COUNT) return false;

	m_friend[m_friendCount] = *node;

	//	ViewFriend
	m_viewFriend[m_friendCount].Set( m_friendCount, &m_friend[m_friendCount]);

	m_friendCount++;

	//	노드가 변경되면 자동 정렬
	SortViewFriend();
	
	return true;
}

bool CCommunityContext::AddBlockUser(const BLOCK_USER* blockUser)
{
	if (blockUser == nullptr)	return false;
	if (m_nBlockUserCount >= BLOCK_MAX_COUNT)	return false;

//	i3mem::Copy(&m_blockUser[m_nBlockUserCount], (BLOCK_USER*)blockUser, sizeof(BLOCK_USER));
	i3::copy_1( blockUser, &m_blockUser[m_nBlockUserCount]);

	m_nBlockUserCount++;
	return true;
}

bool CCommunityContext::InsertBlockUser(const BLOCK_USER* blockUser)
{
	INT32 _index;
	if (blockUser == nullptr)	return false;
	if (m_nBlockUserCount > BLOCK_MAX_COUNT)	return false;
	if (Find_InsertIndex(&_index) == false)	return false;
	m_nBlockUserCount++;
	m_blockAdded = &m_blockUser[_index];
//	i3mem::Copy(&m_blockUser[_index], (BLOCK_USER*)blockUser, sizeof(BLOCK_USER));
	i3::copy_1(blockUser, &m_blockUser[_index]);

	return true;
}

bool CCommunityContext::DeleteBlockUser(INT32 blockindex)
{
	I3ASSERT_RETURN(blockindex >= 0, false);
	m_blockUser[blockindex]._Activated = false;
	m_nBlockUserCount--;	
	return true;
}

bool CCommunityContext::Find_InsertIndex(INT32 *index)
{
	for(INT32 i=0; i<BLOCK_MAX_COUNT; i++)
	{
		const BLOCK_USER* t_BlockUser = GetBlockUser(i);
		if(t_BlockUser->_Activated == false)
		{
			*index = i;
			return true;
		}
	}
	return false;
}

INT32 CCommunityContext::Find_DeleteIndex(INT32 index)
{
	for(INT32 i=0; i<BLOCK_MAX_COUNT; i++)
	{
		const BLOCK_USER* t_BlockUser = GetBlockUser(i);
		if(t_BlockUser->_blockInfo._Idx == index)
		{
			return i;
		}
	}
	return -1;
}

bool CCommunityContext::CheckBlockUser(const i3::wliteral_range& wstrNick) const
{
	for(INT32 i=0; i < m_nBlockUserCount; i++)
	{
		if( i3::generic_is_equal(m_blockUser[i]._wstrNick , wstrNick) )
			return true;
	}
	return false;
}

const BLOCK_USER * CCommunityContext::GetBlockUser(const i3::rc_wstring& wstrNick ) const
{
	for(INT32 i=0; i < m_nBlockUserCount; i++)
	{
		if( m_blockUser[i]._wstrNick == wstrNick )
			return &m_blockUser[i];
	}
	return nullptr;
}

UINT32 CCommunityContext::GetFriendCount(void) const
{
	return m_friendCount;
}

const FRIEND_NODE* CCommunityContext::GetFriend(UINT32 idx) const
{
	if (idx >= m_friendCount) return nullptr;

	return &m_friend[idx];
}

const FRIEND_NODE * CCommunityContext::GetViewFriend(UINT32 idx) const
{
	if (idx >= m_friendCount) return nullptr;

	return m_viewFriend[idx]._node;
}

const FRIEND_NODE * CCommunityContext::GetViewFriendByNick(const i3::wliteral_range& wNickRng) const
{
	for(UINT32 i=0; i < m_friendCount; i++)
	{
		if(i3::generic_is_iequal(m_viewFriend[i]._node->_wstr_nick, wNickRng) )
			return m_viewFriend[i]._node;
	}
	return nullptr;
}

const FRIEND_NODE * CCommunityContext::GetViewFriendByUID(INT64 uid) const
{
	for(UINT32 i=0; i < m_friendCount; i++)
	{
		if(m_viewFriend[i]._node->_uid == uid)
			return m_viewFriend[i]._node;
	}
	return nullptr;
}

UINT32 CCommunityContext::GetRealIndexOfViewFriend(UINT32 idx) const
{
	return m_viewFriend[idx]._realIndex;
}

INT32 CCommunityContext::GetViewIndexOfRealFriend(UINT32 idx) const
{
	for(UINT32 i=0; i<m_friendCount; i++)
	{
		if( m_viewFriend[i]._realIndex == idx)
			return i;
	}

	return -1;
}

INT32 CCommunityContext::GetViewIndexOfUID(T_UID ui64UID) const
{
	for(UINT32 i=0; i<m_friendCount; i++)
	{
		if( m_viewFriend[i]._node->_uid == (T_UID)ui64UID)
			return i;
	}

	return -1;
}

bool CCommunityContext::SetFriend(UINT32 idx,const FRIEND_NODE* node)
{
	if (idx >= m_friendCount) return false;
	if (nullptr == node) return false;

	m_friend[idx] = *node;

	//	노드가 변경되면 자동 정렬
	SortViewFriend();

	return true;
}

void CCommunityContext::SortViewFriend(void)
{
	UINT32 count[2];

	//	친구 상태에 따라 리스트 정렬 (2008.01.10 komet)
	for(count[0]=0; count[0]<m_friendCount-1; count[0]++)
	{
		for(count[1]=count[0]+1; count[1]<m_friendCount; count[1]++)
		{
			INT32 priority[2];

			for(INT32 i=0; i<2; i++)
			{
				switch(GET_FRIEND_STATE(m_viewFriend[count[i]]._node->_state))
				{
				case FRIEND_STATE_HOLD:			priority[i] = 1;	 break;	//	가장 높음				
				case FRIEND_STATE_ROOM:			priority[i] = 2;	 break;
				case FRIEND_STATE_ONLINE:		priority[i] = 3;	 break;				
				case FRIEND_STATE_LOBBY:		priority[i] = 4;	 break;
				case FRIEND_STATE_OFFLINE:		priority[i] = 5;	 break;
				case FRIEND_STATE_REQUEST:		priority[i] = 6;	 break;
				case FRIEND_STATE_CANT_INVITE:	priority[i] = 7;	 break;
				default: priority[i] = 999;	break;
				}
			}

			//	수치가 낮으면 우선순위가 높다.
			if( priority[0] > priority[1])
			{
				VIEW_FRIEND_NODE swap;
				
				m_viewFriend[count[0]].CopyTo( &swap);
				m_viewFriend[count[1]].CopyTo( &m_viewFriend[count[0]]);
				swap.CopyTo( &m_viewFriend[count[1]]);
			}
		}
	}
}

INT32 CCommunityContext::GetClanMemberIdx( T_UID i64DBIdx ) const
{
	for( UINT32 i = 0 ; i < m_clanMemberCount ; i++ )
	{
		if( m_clanMember[ i ]._uid == i64DBIdx ) return i;
	}

	return -1;
}

bool	CCommunityContext::AddClanMember(CLAN_MEMBER_NODE * node)
{
	if (nullptr == node) return false;
	if (m_clanMemberCount >= MAX_MEMBER_COUNT) return false;


	m_clanMember[m_clanMemberCount] = *node;

	//	ViewFriend
	m_viewClanMember[m_clanMemberCount].Set( m_clanMemberCount, &m_clanMember[m_clanMemberCount]);

	m_clanMemberCount++;

	//	노드가 변경되면 자동 정렬
	SortViewClanMember();
	
	return true;
}

bool	CCommunityContext::DeleteClamMember( T_UID i64UID )
{
	if (m_clanMemberCount == 0)
		return false;

	for ( UINT32 a = 0 ; a < m_clanMemberCount ; ++a )
	{
		if ( static_cast<T_UID>(m_clanMember[a]._uid) != i64UID )
			continue;

		for ( UINT32 b = a ; b < m_clanMemberCount ; ++b )
		{
			if ( (b+1) < MAX_MEMBER_COUNT)
				m_clanMember[b] = m_clanMember[b+1];
		}

		break;
	}

	for( INT32 i = 0 ; i < MAX_MEMBER_COUNT ; i++ )
	{
		m_viewClanMember[i]._node = &m_clanMember[i];
	}

	m_clanMemberCount -= 1;

	SortViewClanMember();

	return true;
}

void	CCommunityContext::ChangeClanMember( INT64 i64UID, UINT64 ui64State )
{
  	for( UINT32 i = 0 ; i < m_clanMemberCount ; i++ )
	{
		if( m_clanMember[ i ]._uid != i64UID )
			continue;

		// 클랜원 로그인 메시지
		switch( GET_FRIEND_STATE( m_clanMember[i]._state ) )
		{
		case FRIEND_STATE_OFFLINE:
			if( GET_FRIEND_STATE( ui64State ) == FRIEND_STATE_ONLINE )
			{
				i3::stack_wstring wstrLoginMsg;
				i3::sprintf( wstrLoginMsg, GAME_RCSTRING("STR_TBL_GUI_COMMUNITY_USER_LOGIN"), m_clanMember[i]._wstr_nick );		
				g_pFramework->getChatBox()->PutSystemChatting( wstrLoginMsg );
			}
			break;
		case FRIEND_STATE_ONLINE:
		case FRIEND_STATE_LOBBY:
		case FRIEND_STATE_ROOM:
			if( GET_FRIEND_STATE( ui64State ) == FRIEND_STATE_OFFLINE )
			{
				i3::stack_wstring wstrLoginMsg;
				i3::sprintf( wstrLoginMsg, GAME_RCSTRING("STR_TBL_GUI_COMMUNITY_USER_LOGOUT"), m_clanMember[i]._wstr_nick );
				g_pFramework->getChatBox()->PutSystemChatting( wstrLoginMsg );
			}
			break;
		}

		// 클랜원 로그인 메시지
		m_clanMember[i]._state				= ui64State;

		SortViewClanMember();
		return;
	}
}

void	CCommunityContext::ChangeBasicInfoClanMember( CLAN_MEMBER_BASIC_INFO* pMemberBasicInfo )
{
	i3::wstring wstrNick;

  	for( UINT32 i = 0 ; i < m_clanMemberCount ; i++ )
	{
		if( m_clanMember[ i ]._uid != pMemberBasicInfo->_i64UID )
			continue;

		wstrNick = pMemberBasicInfo->_nick;
		m_clanMember[i]._wstr_nick = wstrNick;

		m_clanMember[i]._rank	= pMemberBasicInfo->_rank;
		m_clanMember[i]._nickcolor = pMemberBasicInfo->_color;
		return;
	}
}


const CLAN_MEMBER_NODE* CCommunityContext::GetClanMember(UINT32 idx) const
{
	if (idx >= m_clanMemberCount) return nullptr;

	return &m_clanMember[idx];
}

const CLAN_MEMBER_NODE * CCommunityContext::GetViewClanMember(UINT32 idx) const
{
	if (idx >= m_clanMemberCount) return nullptr;

	return m_viewClanMember[idx]._node;
}

UINT32 CCommunityContext::GetRealIndexOfViewClanMember(UINT32 idx) const
{
	return m_viewClanMember[idx]._realIndex;
}

INT32 CCommunityContext::GetViewIndexOfRealClanMember(INT32 idx) const
{
	for(UINT32 i=0; i<m_clanMemberCount; i++)
	{
		if( m_viewClanMember[i]._realIndex == (UINT32)idx)
			return i;
	}

	return -1;
}

INT32 CCommunityContext::GetViewIndexOfUIDClanMember(UINT64 ui64UID) const
{
	for(UINT32 i=0; i<m_clanMemberCount; i++)
	{
		if( m_viewClanMember[i]._node->_uid == (T_UID)ui64UID)
			return i;
	}

	return -1;
}

bool CCommunityContext::SetClanMember(UINT32 idx,const CLAN_MEMBER_NODE* node)
{
	if (idx >= m_clanMemberCount) return false;
	if (nullptr == node) return false;

	m_clanMember[idx] = *node;

	//	노드가 변경되면 자동 정렬
	SortViewClanMember();

	return true;
}

void CCommunityContext::SortViewClanMember(void)
{
	if(m_clanMemberCount < 2)
		return;

	UINT32 count[2];

	//	친구 상태에 따라 리스트 정렬 (2008.01.10 komet)
	for(count[0]=0; count[0]<m_clanMemberCount-1; count[0]++)
	{
		for(count[1]=count[0]+1; count[1]<m_clanMemberCount; count[1]++)
		{
			INT32 priority[2];

			for(INT32 i=0; i<2; i++)
			{
				switch(GET_FRIEND_STATE(m_viewClanMember[count[i]]._node->_state))
				{
				case FRIEND_STATE_HOLD:			priority[i] = 1;	 break;	//	가장 높음				
				case FRIEND_STATE_ROOM:			priority[i] = 2;	 break;
				case FRIEND_STATE_ONLINE:		priority[i] = 3;	 break;				
				case FRIEND_STATE_LOBBY:		priority[i] = 4;	 break;
				case FRIEND_STATE_OFFLINE:		priority[i] = 5;	 break;
				case FRIEND_STATE_REQUEST:		priority[i] = 6;	 break;
				case FRIEND_STATE_CANT_INVITE:	priority[i] = 7;	 break;
				default: priority[i] = 999;	break;
				}
			}

			//	수치가 낮으면 우선순위가 높다.
			if( priority[0] > priority[1])
			{
				VIEW_CLAN_MEMBER_NODE swap;
				
				m_viewClanMember[count[0]].CopyTo( &swap);
				m_viewClanMember[count[1]].CopyTo( &m_viewClanMember[count[0]]);
				swap.CopyTo( &m_viewClanMember[count[1]]);
			}
		}
	}
}

void CCommunityContext::MakeDummyFriend(void)
{
	FRIEND_NODE dummy[FRIEND_MAX_COUNT] =
	{
		{	1,	MAKE_FRIEND_STATE(1, 0, 0, 0),		0,	0,	L"Dummy1"},
		{	2,	MAKE_FRIEND_STATE(2, 0, 0, 0),		1,	0,	L"Dummy2"},
		{	3,	MAKE_FRIEND_STATE(1, 0, 0, 0),		1,	0,	L"Dummy3"},
		{	4,	MAKE_FRIEND_STATE(2, 0, 0, 0),		1,	0,	L"Dummy4"},
		{	5,	MAKE_FRIEND_STATE(3, 0, 0, 0),		2,	0,	L"Dummy5"},
		{	6,	MAKE_FRIEND_STATE(3, 0, 0, 0),		3,	0,	L"Dummy6"},
		{	7,	MAKE_FRIEND_STATE(3, 0, 0, 0),		3,	0,	L"Dummy7"},
		{	8,	MAKE_FRIEND_STATE(3, 0, 0, 0),		4,	0,	L"Dummy8"},
		{	9,	MAKE_FRIEND_STATE(3, 0, 0, 0),		5,	0,	L"Dummy9"},
		{	10,	MAKE_FRIEND_STATE(4, 0, 0, 0),		6,	0,	L"Dummy10"},
		{	11,	MAKE_FRIEND_STATE(4, 0, 0, 0),		7,	0,	L"Dummy11"},
		{	12,	MAKE_FRIEND_STATE(4, 0, 0, 0),		8,	0,	L"Dummy12"},
		{	13,	MAKE_FRIEND_STATE(4, 0, 0, 0),		9,	0,	L"Dummy13"},
		{	14,	MAKE_FRIEND_STATE(4, 0, 0, 0),		10,	0,	L"Dummy14"},
		{	15,	MAKE_FRIEND_STATE(5, 0, 0, 0),		11,	0,	L"Dummy15"},
		{	16,	MAKE_FRIEND_STATE(5, 0, 0, 1),		20,	0,	L"Dummy16"},
		{	17,	MAKE_FRIEND_STATE(5, 0, 0, 2),		21,	0,	L"Dummy17"},
		{	18,	MAKE_FRIEND_STATE(5, 0, 3, 99),		33,	0,	L"Dummy18"},
		{	19,	MAKE_FRIEND_STATE(5, 0, 9, 299),	40,	0,	L"Dummy19"},
		{	20,	MAKE_FRIEND_STATE(4, 0, 0, 0),		41,	0,	L"Dummy20"},
	};

	m_friendCount = 20;

//	i3mem::Copy(m_friend, dummy, sizeof(FRIEND_NODE) * m_friendCount);
	i3::copy(dummy, dummy + m_friendCount, m_friend);

	for(UINT32 i=0; i<m_friendCount; i++)
	{
		m_viewFriend[i].Set( i, &m_friend[i]);
	}

	SortViewFriend();
}

void CCommunityContext::MakeDummyClanMember(void)
{
	CLAN_MEMBER_NODE dummy[MAX_MEMBER_COUNT] =
	{
		{	1,	MAKE_FRIEND_STATE(1, 0, 0, 0),		0,	0,	L"Dummy1"},
		{	2,	MAKE_FRIEND_STATE(2, 0, 0, 0),		1,	0,	L"Dummy2"},
		{	3,	MAKE_FRIEND_STATE(1, 0, 0, 0),		1,	0,	L"Dummy3"},
		{	4,	MAKE_FRIEND_STATE(2, 0, 0, 0),		1,	0,	L"Dummy4"},
		{	5,	MAKE_FRIEND_STATE(3, 0, 0, 0),		2,	0,	L"Dummy5"},
		{	6,	MAKE_FRIEND_STATE(3, 0, 0, 0),		3,	0,	L"Dummy6"},
		{	7,	MAKE_FRIEND_STATE(3, 0, 0, 0),		3,	0,	L"Dummy7"},
		{	8,	MAKE_FRIEND_STATE(3, 0, 0, 0),		4,	0,	L"Dummy8"},
		{	9,	MAKE_FRIEND_STATE(3, 0, 0, 0),		5,	0,	L"Dummy9"},
		{	10,	MAKE_FRIEND_STATE(4, 0, 0, 0),		6,	0,	L"Dummy10"},
		{	11,	MAKE_FRIEND_STATE(4, 0, 0, 0),		7,	0,	L"Dummy11"},
		{	12,	MAKE_FRIEND_STATE(4, 0, 0, 0),		8,	0,	L"Dummy12"},
		{	13,	MAKE_FRIEND_STATE(4, 0, 0, 0),		9,	0,	L"Dummy13"},
		{	14,	MAKE_FRIEND_STATE(4, 0, 0, 0),		10,	0,	L"Dummy14"},
		{	15,	MAKE_FRIEND_STATE(5, 0, 0, 0),		11,	0,	L"Dummy15"},
		{	16,	MAKE_FRIEND_STATE(5, 0, 0, 1),		20,	0,	L"Dummy16"},
		{	17,	MAKE_FRIEND_STATE(5, 0, 0, 2),		21,	0,	L"Dummy17"},
		{	18,	MAKE_FRIEND_STATE(5, 0, 3, 99),		33,	0,	L"Dummy18"},
		{	19,	MAKE_FRIEND_STATE(5, 0, 9, 299),	40,	0,	L"Dummy19"},
		{	20,	MAKE_FRIEND_STATE(4, 0, 0, 0),		41,	0,	L"Dummy20"},
	};

	m_clanMemberCount = 20;
	
	i3::copy( dummy, dummy + m_clanMemberCount, m_clanMember);
//	i3mem::Copy(m_clanMember, dummy, sizeof(CLAN_MEMBER_NODE) * m_clanMemberCount);

	for(UINT32 i=0; i<m_clanMemberCount; i++)
	{
		m_viewClanMember[i].Set( i, &m_clanMember[i]);
	}

	SortViewClanMember();
}

void CCommunityContext::SetCommnunityInfoNick(const i3::rc_wstring& wstr_nick)
{
	m_wstr_userInfoNick = wstr_nick;
}


const CLAN_MEMBER_NODE * CCommunityContext::FindClanMemberByUserID( INT64 i64UID) const
{
	UINT32 i;
	for( i = 0; i < GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE * pClanMember = GetViewClanMember( i);
		if( pClanMember->_uid == i64UID )
			return pClanMember;
	}

	return nullptr;
}

const FRIEND_NODE * CCommunityContext::FindFriendByUserID( INT64 i64UID) const
{
	UINT32 i;

	for( i = 0; i < GetFriendCount(); i++)
	{
		const FRIEND_NODE * pFriend = GetFriend( i);
		if( pFriend->_uid == i64UID )
			return pFriend;
	}

	return nullptr;
}

////////////* 1.5 유저 정보 보기 **/////////////

// 간략 정보 설정
void CCommunityContext::SetUserSimpleInfo( const PACKET_BASE_USER_BASIC_INFO_ACK * pInfo)
{
	m_UserSimpleInfo.CopyFromUserInfo( const_cast<PACKET_BASE_USER_BASIC_INFO_ACK*>(pInfo) );
}

// 유저 위치 설정
void CCommunityContext::SetUserPosInfo( const PACKET_BASE_USER_POSITION_ACK * pInfo)
{
	m_UserSimpleInfo.CopyFromUserPos( const_cast<PACKET_BASE_USER_POSITION_ACK*>(pInfo) );
}

// 유저 서브테스크 설정
void CCommunityContext::SetUserSubTaskInfo( const PACKET_H_BASE_USER_SUBTASK_ACK * pInfo)
{
	m_UserSimpleInfo.CopyFromUserSubTask( const_cast<PACKET_H_BASE_USER_SUBTASK_ACK*>(pInfo) );
}

// 상세 정보 설정
void CCommunityContext::SetUserDetailInfo( const PACKET_H_BASE_USER_DETAIL_INFO_ACK * pInfo)
{
	m_UserDetailInfo.CopyFromUserInfo( const_cast<PACKET_H_BASE_USER_DETAIL_INFO_ACK*>(pInfo) );
}

// 유저 캐릭터 설정
void CCommunityContext::AddUserDetailInfoChara(PACKET_D_BASE_USER_DETAIL_INFO_CHARA & pInfo)
{ 
	m_UserDetailInfo.AddCharaInfo(pInfo);
}

// 유저 따라가기 / 초대하기
bool CCommunityContext::SetMovingToUser(UINT64 ui64State)
{
	if(IsMovingToUser()) 
		return false;

	m_MoveTargetInfo.SetState(ui64State);

	return true;
}

bool CCommunityContext::SetMovingToNormalUser(UINT64 ui64State)
{
	if(IsReceivedInvite())
		return false;

	return SetMovingToUser(ui64State);
}

bool CCommunityContext::SetMovingToFriendUser(INT32 i32Idx)
{
	if(IsReceivedInvite())
		return false;

	I3ASSERT_RETURN(i32Idx >= 0, false);

	FRIEND_NODE  * pNode = m_viewFriend[i32Idx]._node;

	I3ASSERT_RETURN(pNode != nullptr, false);

	m_ui32MoveTargetUserIdx = i32Idx;

	UINT64 istate = SET_FRIEND_MATCH(pNode->_state, MATCH_TEAM_INVALID_IDX);

	return SetMovingToUser(istate);
}

bool CCommunityContext::SetMovingToClanUser(INT32 i32Idx)
{
	if(IsReceivedInvite())
		return false;

	I3ASSERT_RETURN(i32Idx >= 0, false);

	CLAN_MEMBER_NODE * pNode = m_viewClanMember[i32Idx]._node;

	m_ui32MoveTargetUserIdx = i32Idx;

	return SetMovingToUser(pNode->_state);
}

bool CCommunityContext::StartMovingToUser(void)
{
	m_bIsMovingToUser = true;

	return true;
}

void CCommunityContext::CompleteMovingToUser(void)
{
	if(m_bIsMovingToUser) 
		m_bIsMovingToUser = false;

	m_bIsReceivedInvite = false; 
	m_InvitedUserNode.Reset();
}

const FRIEND_NODE * CCommunityContext::GetFriendNodeOfInviteUser(void) const
{
	if(m_ui32MoveTargetUserIdx >= m_friendCount)
		return nullptr;

	return m_viewFriend[m_ui32MoveTargetUserIdx]._node;
}

const CLAN_MEMBER_NODE * CCommunityContext::GetClanMemberNodeOfInviteUser(void) const
{
	if (m_ui32MoveTargetUserIdx >= m_clanMemberCount) 
		return nullptr;

	return m_viewClanMember[m_ui32MoveTargetUserIdx]._node;
}

void CCommunityContext::SetInvitedNormalUser(PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK * pInfo)
{
	m_InvitedUserNode.CopyFromUserInfo(pInfo);
}

void CCommunityContext::SetDelayedInvite(const bool bFlag, INVITE_TYPE type)
{
	m_bIsDelayedInvite = bFlag;
	m_eInviteType = type;
}

void CCommunityContext::SetFindUserInfo( NEW_FIND_USER_INFO& info )
{
	m_FindUserInfo = info;
}

void CCommunityContext::SetDefaultFindUserData( void )
{
	i3mem::FillZero(&m_FindUserInfo, sizeof(NEW_FIND_USER_INFO));

	m_FindUserInfo.m_i32State = MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0); // 오프라인입니다.

	/*m_FindUserInfo.m_Chara.SetRedNum(DEFAULT_CHARA_RED_TEAM);
	m_FindUserInfo.m_Chara.SetBlueNum(DEFAULT_CHARA_BLUE_TEAM);
	m_FindUserInfo.m_Chara.SetHairNum(DEFAULT_EQUIP_HEAD);
	m_FindUserInfo.m_Chara.SetDinoNum(DEFAULT_CHARA_DINO);*/

	m_FindUserInfo.m_Weapon._noprim = DEFAULT_WEAPON_ASSAULT;
	m_FindUserInfo.m_Weapon._nosub = DEFAULT_WEAPON_HANDGUN;
	m_FindUserInfo.m_Weapon._nomelee = DEFAULT_WEAPON_KNIFE;
	m_FindUserInfo.m_Weapon._nothrow = DEFAULT_WEAPON_THROWING1;
	m_FindUserInfo.m_Weapon._noitem = DEFAULT_WEAPON_ITEM;
}
