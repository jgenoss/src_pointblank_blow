#ifndef _CommunityContext_h
#define _CommunityContext_h

#include "UserLocationInfo.h"
#include "UI/UIUtil.h"

//	[array initializer] Server 목록과 같은 배열의 친구 리스트
struct FRIEND_NODE
{
	INT64 _uid;
	UINT32 _state;
	UINT8 _rank;
	UINT8 _nickcolor;
	i3::rc_wstring _wstr_nick;

	void CopyTo(FRIEND_NODE * pDst)
	{
		pDst->_uid = _uid;
		pDst->_state = _state;
		pDst->_rank = _rank;
		pDst->_nickcolor = _nickcolor;
		pDst->_wstr_nick = _wstr_nick;
	}

	void Reset( void)
	{
		_uid = _state = _rank = _nickcolor = 0;
		_wstr_nick.clear(); 
	}
};

//	Display용 (정렬 하기 위한)
struct VIEW_FRIEND_NODE
{
	UINT32 _realIndex = 0;			//	실제 m_friend 배열의 번호 (Server로 보내는 번호)
	FRIEND_NODE * _node = nullptr;	//	포인터

	void CopyTo(VIEW_FRIEND_NODE * pDest)
	{
		pDest->_node = _node;
		pDest->_realIndex = _realIndex;
	}

	void Set(UINT32 idx, FRIEND_NODE * node)
	{
		_realIndex = idx;
		_node = node;
	}

} ;

struct BLOCK_USER
{
	i3::rc_wstring	_wstrNick;
	bool _Activated = false;
	i3::rc_wstring _wstr_comment;
	BLOCK_INFO _blockInfo;
};

//	[array initializer] Server 목록과 같은 배열의 친구 리스트
struct CLAN_MEMBER_NODE
{
	INT64 _uid;
	UINT64 _state;
	UINT8 _rank;
	UINT8 _nickcolor;
	i3::rc_wstring _wstr_nick;

	void CopyTo(CLAN_MEMBER_NODE * pDst)
	{
		pDst->_uid = _uid;
		pDst->_state = _state;
		pDst->_rank = _rank;
		pDst->_nickcolor = _nickcolor;
		pDst->_wstr_nick = _wstr_nick;
	}

	void Reset( void)
	{
		_state = 0;
		_uid = _rank = _nickcolor = 0;
		_wstr_nick.clear();
	}
};

//	Display용 (정렬 하기 위한)
struct VIEW_CLAN_MEMBER_NODE
{
	UINT32 _realIndex = 0;				//	실제 m_friend 배열의 번호 (Server로 보내는 번호)
	CLAN_MEMBER_NODE * _node = nullptr;	//	포인터

	void CopyTo(VIEW_CLAN_MEMBER_NODE * pDest)
	{
		pDest->_node = _node;
		pDest->_realIndex = _realIndex;
	}

	void Set(UINT32 idx, CLAN_MEMBER_NODE * node)
	{
		_realIndex = idx;
		_node = node;
	}

} ;

// 유저 간략 정보 팝업
struct USER_SIMPLE_INFO
{
	PACKET_BASE_USER_BASIC_INFO_ACK _UserInfo;

	void CopyFromUserInfo(PACKET_BASE_USER_BASIC_INFO_ACK * pSrc)
	{
		i3mem::Copy(&_UserInfo, pSrc, sizeof(PACKET_BASE_USER_BASIC_INFO_ACK));
	}

	void CopyFromUserPos(PACKET_BASE_USER_POSITION_ACK * pSrc)
	{
		_UserInfo.m_State = pSrc->m_State;
		_UserInfo.m_ServerIdx = pSrc->m_ServerIdx;
		_UserInfo.m_ChannelNum = pSrc->m_ChannelNum;
		_UserInfo.m_RoomIdx = pSrc->m_RoomIdx;
	}

	void CopyToUserInfo(USER_INFO_BASIC * pDest) const
	{
		i3::safe_string_copy(pDest->m_strNick,		_UserInfo.m_nickName, NET_NICK_NAME_SIZE );
		pDest->m_ui32Rank = _UserInfo.m_rank;
		pDest->m_ui32Exp = _UserInfo.m_exp;

		pDest->m_ui32ClanMark = _UserInfo.m_clanMark;
		i3::safe_string_copy(pDest->m_strClanName,	_UserInfo.m_strClanName, NET_CLAN_NAME_SIZE );
	}

	void CopyFromUserSubTask(PACKET_H_BASE_USER_SUBTASK_ACK * pSrc)
	{
		_UserInfo.m_ui8SubTask = pSrc->m_Subtask;
	}

	void Reset(void)
	{
		i3mem::FillZero(&_UserInfo, sizeof(PACKET_BASE_USER_BASIC_INFO_ACK));
	}
};

struct USER_DETAIL_INFO
{
	PACKET_H_BASE_USER_DETAIL_INFO_ACK _UserInfo;
	i3::vector< PACKET_D_BASE_USER_DETAIL_INFO_CHARA > _vecChara;

	void CopyFromUserInfo(PACKET_H_BASE_USER_DETAIL_INFO_ACK * pSrc)
	{
		i3mem::Copy(&_UserInfo, pSrc, sizeof(PACKET_H_BASE_USER_DETAIL_INFO_ACK));
	}

	void _Clear(void)
	{
		_vecChara.clear();
	}

	void AddCharaInfo(PACKET_D_BASE_USER_DETAIL_INFO_CHARA & pInfo)
	{
		_vecChara.push_back(pInfo);
	}

	const CHARA_SLOT_SKILL * GetCharaSkill(INT32 nSlot) const
	{
		I3_BOUNDCHK(nSlot, _UserInfo.m_iCount);			

		return &(_vecChara[nSlot].m_CharaSkill);
	}


	UINT32 GetCharaItemID(INT32 nSlot) const
	{
		I3_BOUNDCHK(nSlot, _UserInfo.m_iCount);
		
		return _vecChara[nSlot].m_ui32CharaItemID;
	}

	const TTCHAR* GetCharaName(INT32 nSlot) const
	{
		I3_BOUNDCHK(nSlot, _UserInfo.m_iCount);
		
		return _vecChara[nSlot].m_strCharaName;
	}

	UINT32 GetCreateDate(INT32 nSlot) const
	{
		I3_BOUNDCHK(nSlot, _UserInfo.m_iCount);
		
		return _vecChara[nSlot].m_ui32CreateDate;
	}

	UINT8 GetPlayType(INT32 nSlot) const
	{
		I3_BOUNDCHK(nSlot, _UserInfo.m_iCount);
		
		return _vecChara[nSlot].m_ui8SlotNum;
	}

	INT32 GetCharaCount(void) const
	{
		return _UserInfo.m_iCount;
	}

	void Reset(void)
	{
		i3mem::FillZero(&_UserInfo, sizeof(PACKET_BASE_USER_BASIC_INFO_ACK));
		_Clear();
	}
};

struct INVITED_USER_NODE
{
	PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK _UserInfo;

	void CopyFromUserInfo(PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK * pSrc)
	{
		i3mem::Copy(&_UserInfo, pSrc, sizeof(PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK));		
	}

	void Reset(void)
	{
		i3mem::FillZero(&_UserInfo, sizeof(PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK));
	}
};

enum INVITE_TYPE
{
	INVITE_TYPE_NONE = -1,
	INVITE_TYPE_NORMAL,
	INVITE_TYPE_FRIEND,
	INVITE_TYPE_CLAN,

	INVITE_TYPE_MAX
};

struct USERINFO// : i3::class_common_pool<USERINFO>
{
	INT32	_Session = -1;						// DB ID
	INT64	_i64UID = 0;						// UID 
	UINT8	_Rank = 0;							// 랭킹
	i3::rc_wstring	_wstrNick;				// 닉네임
	i3::rc_wstring	_szPosition;			// 위치
	I3COLOR _NickColor = { 255, 255, 255, 255 };
	FRIEND_STATE _friendState = FRIEND_STATE_REQUEST;
	i3::rc_wstring	_wstrBlockReason;		//	BLOCK_COMMENT_MAX

	void	Reset( void)
	{
		_Session	= -1;
		_Rank		= 0;
		_wstrNick.clear();
		i3Color::Set( &_NickColor, (UINT8) 255, 255, 255, 255);
		_friendState	= FRIEND_STATE_REQUEST;
	}

	void	CopyFromLobbyUserByLobby( const LOBBY_USER_INFO_LIST * pSrc)
	{
		_Session = pSrc->_Session;

		_Rank = pSrc->_Rank;
		_wstrNick = pSrc->_Nick;

		const I3COLOR * pNickColor = GameUI::GetNickNameColor( pSrc->_NickColor);
		if( pNickColor != nullptr)
			i3mem::Copy( &_NickColor, pNickColor, sizeof( _NickColor) );
	}

	// 대기방에서 로비 사용자 복사
	void	CopyFromLobbyUserByReadyRoom(const USER_INFO_INVITE * pSrc )
	{
		_Session = pSrc->_idxSession;

		_Rank = (UINT8)pSrc->_rank;
		_wstrNick = pSrc->_wstr_nick;
#ifdef USE_COLOR_NICK2
		const I3COLOR * pNickColor = GameUI::GetNickNameColor( pSrc->_nickColor);
		if( pNickColor != nullptr)
			i3mem::Copy( &_NickColor, pNickColor, sizeof( _NickColor) );
#endif
	}

	void	CopyToLobbyUser( LOBBY_USER_INFO_LIST * pDest)
	{
		pDest->_Session = _Session;
		pDest->_Rank = _Rank;

		const i3::rc_wstring& strNick = _wstrNick;

		i3::safe_string_copy(	pDest->_Nick, strNick, NET_NICK_NAME_SIZE);

		const I3COLOR * pNickColor = GameUI::GetNickNameColor( pDest->_NickColor);
		if( pNickColor != nullptr)
			i3mem::Copy( &_NickColor, pNickColor, sizeof( _NickColor) );
	}

	void	CopyFromClanUser( const CLAN_MEMBER_NODE * pSrc)
	{
		_i64UID = pSrc->_uid;
		_wstrNick = pSrc->_wstr_nick;   //NET_NICK_NAME_SIZE);
		_Rank = pSrc->_rank;
		_NickColor = *GetColorCashItem(pSrc->_nickcolor);

		i3::wstring		_wPosition;
		GameUI::GetTextClanState(_wPosition, pSrc->_state);
		_szPosition = _wPosition;
		_friendState = (FRIEND_STATE)GET_FRIEND_STATE(pSrc->_state);
	}

	void	CopyFromFriend( const FRIEND_NODE * pSrc)
	{
		_i64UID = pSrc->_uid;
		_wstrNick = pSrc->_wstr_nick;
		_Rank = pSrc->_rank;
		_NickColor = *GetColorCashItem(pSrc->_nickcolor);

		i3::wstring		_wPosition;
		GameUI::GetTextFriendState(_wPosition, pSrc->_state);
		_szPosition = _wPosition;
		_friendState = (FRIEND_STATE)GET_FRIEND_STATE(pSrc->_state);
	}
	void	CopyFromBlock( const BLOCK_USER * pSrc)
	{
		_wstrNick = pSrc->_wstrNick; 
		_wstrBlockReason = pSrc->_wstr_comment;
		_Rank = pSrc->_blockInfo._rank;
	}

	bool	IsSameUser( USERINFO * pDest)
	{
		if( _Session != pDest->_Session)	return false;
		if( _wstrNick != pDest->_wstrNick ) return false;

		return true;
	}
};
class CCommunityContext: public i3::shared_ginst<CCommunityContext>
{
public:
	CCommunityContext();

	bool	InsertFriend(const i3::rc_wstring& wstr_nick);
	bool	FindUser(char* nick);

	bool	AcceptViewFriend(UINT32 idx);
	bool	DeleteViewFriend(UINT32 idx);
	bool	InviteViewFriend(UINT32 idx);
	bool	InviteViewClan(UINT32 idx);
	bool	InviteNormalUser(void);

public:

	///////////////////////////////////////////////
	//	BlockUser Control
	bool	AddBlockUser(const BLOCK_USER*);
	bool	InsertBlockUser(const BLOCK_USER*);
	bool	DeleteBlockUser(INT32);
	bool	Find_InsertIndex(INT32 *);
	INT32	Find_DeleteIndex(INT32);
	void	SetBlockUserCount(INT32 blockCount)			{ m_nBlockUserCount = blockCount;}
	INT32	GetBlockUserCount()	const					{ return m_nBlockUserCount; }
	const BLOCK_USER * GetLastAdded() const				{ return m_blockAdded;}
	const BLOCK_USER * GetBlockUser(INT32 index) const	{ return &m_blockUser[index];}
	bool	CheckBlockUser(const i3::wliteral_range& wstrNick) const;
	const BLOCK_USER * GetBlockUser(const i3::rc_wstring& wstrNick) const;

	///////////////////////////////////////////////
	//	Friend
	bool	AddFriend(const FRIEND_NODE* node);	
	UINT32	GetFriendCount(void) const;
	const	FRIEND_NODE* GetFriend(UINT32 idx) const;	
	const	FRIEND_NODE* GetViewFriend(UINT32 idx) const;	
	const	FRIEND_NODE* GetViewFriendByNick(const i3::wliteral_range& wNickRng) const;
	const	FRIEND_NODE* GetViewFriendByUID(INT64 uid) const;
	UINT32	GetRealIndexOfViewFriend(UINT32 idx) const;		//	view index -> array index
	INT32	GetViewIndexOfRealFriend(UINT32 idx) const;		//	array index -> view index
	INT32	GetViewIndexOfUID(T_UID UID) const;				//  UID Find -> return ArrayIdx
	bool	SetFriend(UINT32 idx, const FRIEND_NODE* node);	
	
	const FRIEND_NODE* GetLastDeleteFriend(void) const	{ return &m_lastDeleteFriend;}
	void	SortViewFriend(void);
	void	MakeDummyFriend(void);
	void	ClearFriend(void)			{ m_friendCount = 0;}

	///////////////////////////////////////////////
	//	Clan
	INT32	GetClanMemberIdx( INT64 i64UID ) const;
	bool	AddClanMember( CLAN_MEMBER_NODE * node);
	bool	DeleteClamMember( INT64 i64UID );
	void	ChangeClanMember( INT64 i64UID, UINT64 ui64State );
	void	ChangeBasicInfoClanMember( CLAN_MEMBER_BASIC_INFO* pMemberBasicInfo );	
	UINT32	GetClanMemberCount(void) const			{return m_clanMemberCount;}
	const CLAN_MEMBER_NODE* GetClanMember(UINT32 idx) const;	
	const CLAN_MEMBER_NODE* GetViewClanMember(UINT32 idx) const;	
	UINT32	GetRealIndexOfViewClanMember(UINT32 idx) const;		//	view index -> array index
	INT32	GetViewIndexOfRealClanMember(INT32 idx) const;		//	array index -> view index
	INT32	GetViewIndexOfUIDClanMember(UINT64 UID) const;		//  UID Find -> return ArrayIdx
	bool	SetClanMember(UINT32 idx, const CLAN_MEMBER_NODE* node);	

	void	SortViewClanMember(void);
	void	MakeDummyClanMember(void);
	void	ClearClanMember(void)				{m_clanMemberCount = 0;}

	// 유저정보 보기
	void	SetCommnunityInfoNick(const i3::rc_wstring& wstr_nick);
	const i3::rc_wstring& GetCommnunityInfoNick(void) const			{return m_wstr_userInfoNick;}
	void	SetCommnunityConnectState(UINT32 connectState)	{m_userInfoConnectState = connectState;}
	UINT32	GetCommnunityConnectState(void) const			{return m_userInfoConnectState;}
	USER_INFO_RECORD* SetCommnuitRecord(void)				{return &m_userInfoRecord;}
	const USER_INFO_RECORD* GetCommunityRecord(void) const	{return &m_userInfoRecord;}
	void	SetCommunityUserInfo(bool beUserInfo)			{m_beUserInfo = beUserInfo;}
	bool	BeCommunityUserInfo(void) const					{return m_beUserInfo;}

	// 1.5 버전에 사용되는 유저 정보 보기 
	// 간략정보
	void	SetUserSimpleInfo( const PACKET_BASE_USER_BASIC_INFO_ACK * pInfo);
	void	SetUserPosInfo( const PACKET_BASE_USER_POSITION_ACK * pInfo);
	void	SetUserSubTaskInfo( const PACKET_H_BASE_USER_SUBTASK_ACK* pInfo);

	// 상세보기 캐릭터 정보( 레디룸 )
	// 1.1 로비의 경우 0번껄로 씁니다. 룸은 슬롯 정보로 저장 및 사용.
	// 자기 슬롯 꺼는 보장 안합니다. 디폴트 0 번
	UI_CHARA_INFO *					SetAnotherUserInfo(INT32 slot = 0)				{I3_BOUNDCHK( slot, SLOT_MAX_COUNT); return m_AnotherUserInfo + slot; }
	const UI_CHARA_INFO *			GetAnotherUserInfo(INT32 slot = 0) const		{I3_BOUNDCHK( slot, SLOT_MAX_COUNT); return m_AnotherUserInfo + slot; }

	const USER_SIMPLE_INFO * GetUserSimpleInfo(void)				{ return &m_UserSimpleInfo; }

	// 상세정보
	void	SetUserDetailInfo( const PACKET_H_BASE_USER_DETAIL_INFO_ACK * pInfo);
	USER_DETAIL_INFO *	SetUserDetailInfo( void)					{ return &m_UserDetailInfo; }
	void	AddUserDetailInfoChara(PACKET_D_BASE_USER_DETAIL_INFO_CHARA & pInfo);

	const USER_DETAIL_INFO * GetUserDetailInfo(void) const	{ return &m_UserDetailInfo; }
	const USER_INFO_RECORD	* GetUserRecordInfo(void) const	{ return &m_UserDetailInfo._UserInfo.m_UserInfoRecord ;}
	//////////////////////////////////////
	const CLAN_MEMBER_NODE *	FindClanMemberByUserID( INT64 i64UID) const;
	const FRIEND_NODE *			FindFriendByUserID( INT64 i64UID) const;


	// 유저리스트 친구 탭 알람 (로그인 후 최초 1회만 알람)
	void	SetFirstFriendAlarm(bool bFirst)	{ m_bFirstFriendAlarm = bFirst; }
	bool	isFirstFriendAlarm( void) const		{ return m_bFirstFriendAlarm; }
	
	//////////////////////////////////////////////////////////////////////
	// 유저 따라가기 / 초대하기
	bool	SetMovingToUser(UINT64 ui64State);
	bool	SetMovingToNormalUser(UINT64 ui64State);
	bool	SetMovingToFriendUser(INT32 i32Idx);
	bool	SetMovingToClanUser(INT32 i32Idx);

	// 일반 유저 초대 노드 설정
	void	SetInvitedNormalUser(PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK * pInfo);

	bool	StartMovingToUser(void);
	void	CompleteMovingToUser(void);

	const FRIEND_NODE *			GetFriendNodeOfInviteUser(void) const;
	const CLAN_MEMBER_NODE *	GetClanMemberNodeOfInviteUser(void) const;
	const INVITED_USER_NODE *	GetInvitedNormalUserNode(void) const { return &m_InvitedUserNode; }

	const CUserLocationInfo & GetMoveTargetInfo() const {return m_MoveTargetInfo;}
	UINT32	GetMoveTargetServer(void) const		{ return m_MoveTargetInfo.GetServerNumber();}
	UINT32	GetMoveTargetChannel(void) const	{ return m_MoveTargetInfo.GetChannelNumber();}
	UINT32	GetMoveTargetRoom(void) const		{ return m_MoveTargetInfo.GetRoomNumber();}
	UINT32	GetMoveTargetMatch(void) const		{ return m_MoveTargetInfo.GetMatch();	};

	bool	IsMovingToUser(void) const			{ return m_bIsMovingToUser; }
	bool	IsReceivedInvite(void) const		{ return m_bIsReceivedInvite; }
	void	SetRecivedInvite(const bool bFlag)	{ m_bIsReceivedInvite = bFlag; }

	void	SetDelayedInvite(const bool bFlag, INVITE_TYPE type);
	bool	IsDleayedInvite(void) const			{ return m_bIsDelayedInvite; }
	INVITE_TYPE	GetInviteType(void) const		{ return m_eInviteType; }
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	void	SetFindUserInfo(NEW_FIND_USER_INFO& info);
	const NEW_FIND_USER_INFO * GetFindUserInfo(void)	{ return &m_FindUserInfo; }

	const USER_INFO_RECORD *			GetFindUserRecord( void)	{ return &m_FindUserInfo.m_Record; }
	const USER_INFO_EQUIPMENT_WEAPON *	GetFindUserWeapon( void)	{ return &m_FindUserInfo.m_Weapon; }
	const USER_INFO_EQUIPMENT_CHARA *	GetFIndUserChara( void)		{ return &m_FindUserInfo.m_Chara; }	

	void	SetDefaultFindUserData(void);

	void	SetFindUser(bool bFind)	{ m_bIsFindUser = bFind; }
	bool	IsFindUser(void)			{ return m_bIsFindUser; }

	bool	IsFindUserOffLine(void)	{ return GET_FRIEND_STATE(m_FindUserInfo.m_i32State) == FRIEND_STATE_OFFLINE ; }
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	void	SetRoomFindUserBasicInfo(USER_INFO_BASIC* info) { m_RoomFindUserInfo = *info; }
	const USER_INFO_BASIC * GetRoomFindUserBasicInfo() { return &m_RoomFindUserInfo; }
	
	void	SetRoomFindUserRecord(USER_INFO_RECORD* info) { m_RoomFindUserRecord = *info; }
	const USER_INFO_RECORD * GetRoomFindUserRecord() { return &m_RoomFindUserRecord;}

private:
	USER_INFO_BASIC m_RoomFindUserInfo;
	USER_INFO_RECORD m_RoomFindUserRecord;

	//////////////////////////////////////////////////////////////////////////

private:
	UINT32 m_friendCount = 0;
	FRIEND_NODE			m_friend[FRIEND_MAX_COUNT];
	VIEW_FRIEND_NODE	m_viewFriend[FRIEND_MAX_COUNT];
	FRIEND_NODE m_lastDeleteFriend;

	bool	m_bBlockSuccess = false;
	INT32	m_nBlockUserCount = 0;
	BLOCK_USER			m_blockUser[BLOCK_MAX_COUNT];
	BLOCK_USER*			m_blockAdded = nullptr;

	UINT32 m_clanMemberCount = 0;
	CLAN_MEMBER_NODE		m_clanMember[MAX_MEMBER_COUNT];
	VIEW_CLAN_MEMBER_NODE	m_viewClanMember[MAX_MEMBER_COUNT];

	// 유저정보 보기
	bool m_beUserInfo = false;
	i3::rc_wstring			m_wstr_userInfoNick;
	USER_INFO_RECORD		m_userInfoRecord;
	UINT32 m_userInfoConnectState = 0;

	// 1.5 버전에 사용되는 유저 정보 보기
	USER_SIMPLE_INFO		m_UserSimpleInfo;
	USER_DETAIL_INFO		m_UserDetailInfo;

	// 1.1 사용하는 상세정보
	NEW_FIND_USER_INFO		m_FindUserInfo;
	bool					m_bIsFindUser = false;

	// 상세보기 캐릭터 정보( 레디룸 )
	// 1.1 로비의 경우 0번껄로 씁니다. 룸은 슬롯 정보로 저장 및 사용.
	// 자기 슬롯 꺼는 보장 안합니다.
	UI_CHARA_INFO			m_AnotherUserInfo[ SLOT_MAX_COUNT];

	// 유저리스트 친구 탭 알람 (로그인 후 최초 1회만 알람)
	bool					m_bFirstFriendAlarm = false;

	//////////////////////////////////////////////////////////////////////
	// 유저 따라가기 / 초대하기
	CUserLocationInfo		m_MoveTargetInfo;
	bool					m_bIsMovingToUser = false;
	bool					m_bIsReceivedInvite = false;
	UINT32					m_ui32MoveTargetUserIdx = 0;

	bool					m_bIsDelayedInvite = false;
	INVITE_TYPE				m_eInviteType = INVITE_TYPE_NONE;

	INVITED_USER_NODE		m_InvitedUserNode;
	//////////////////////////////////////////////////////////////////////
};

#endif
