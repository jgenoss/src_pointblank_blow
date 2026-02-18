#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__




/*-----------------------------------------------------------------------
Name : CUserSessionManager
2007 03 06 : 버그확인 : 없음 
2007 06 25 : 로비에 있는 유저리스트를 사용해야 함. 방에서 추천을 하기 위해서 변동 되어야 함.
-----------------------------------------------------------------------*/
#include "UserSession.h"
#include "RingBuffer.h"

typedef struct __LobbyUserInfo
{
	CUserSession * pSession; 
}LOBBY_USER_INFO;

class CUserSessionManager : public i3NetworkSessionManager
{
	I3_CLASS_DEFINE( CUserSessionManager ); 
protected: 
	//Check User 
	INT32				m_i32SessionCheckIdx;

	INT32				m_i32ActiveCount;
	INT32				m_i32OriMaxSessionIdx;		// 

	UINT32				m_ui32ChannelCount;
	UINT32				m_ui32EachChannelMaxCount;

	//Channel User 
	BOOL			*	m_pChannelUserMaxFlag; 
	i3List			**	m_pChannelUserList;			// ChannelList 차후 변경가능 지금은 카운트용으로 사용되고 있음(추후 작업이 있을것 같아 유지)
	i3Mutex			**	m_pcsChanneUser;			// User

	/////////////////////////////////////////////////////////////////////////
	//Lobby User
	i3List			**	m_pLobbyUserList;			// 카운트로만 사용함..나중에 사용할것 같음..
	i3Mutex			**	m_pcsLobbyUser;				// 

	/////////////////////////////////////////////////////////////////////////
	//채팅으로 추가됨
	i3Mutex			**	m_pcsLobbyChatting; 
	INT32			*	m_pLobbyUserInfoSide;
	LOBBY_USER_INFO	**	m_ppLobbyUserList0;
	UINT32			*	m_pLobbyUserCount0;
	LOBBY_USER_INFO	**	m_ppLobbyUserList1;
	UINT32			*	m_pLobbyUserCount1;	

	/////////////////////////////////////////////////////////////////////////
	//Channel Info Packet 
	INT32				m_i32UserListPacketNum;		//Send Number
	CHANNEL_LIST		m_stChannelList[ MAX_GAME_SERVER_COUNT ][ 2 ];
	INT32				m_i32TimeLogInUser;
	
	INT32				m_i32SendAliveUserIdx;
	INT32				m_i32SendAliveUserCycle;

	// announce buffer
	NSM_CRingBuffer *	m_pRingAnnounceMessage;
	i3NetworkPacket		m_AnnounceMessage;
	bool				m_bSendAnnounce;			// 보내는 중인 메시지가 있으면 TRUE,
	INT32				m_i32AnnouncedUserIdx;
	UINT32				m_ui32AnnounceLastSend;

	NSM_CRingBuffer *	m_pRingAnnounceMain;
	NSM_CRingBuffer *	m_pRingAnnounceControl;
	NSM_CRingBuffer *	m_pRingAnnounceTrans;

	//INT32				m_i32LastUpdate;

	UINT32				m_ui32SessionGameGuardUpdateTime;

	// 패킷 처리 테스트코드
public:
	REAL64				m_r64ParseTimeHigh;
	PROTOCOL			m_ui16ParseTimeHighProtocol;
	bool				m_bParseTimeReset;

	UINT32				m_ui32LinkLoginCount[ DEF_TRANS_LINK_COUNT_MAX ];
	UINT32				m_ui32LinkLoginTimeArg[ DEF_TRANS_LINK_COUNT_MAX ];

	// HeartBit2
	UINT32				m_ui32RecvHeartBit2Count;
	INT32				m_i32UpdateTimeMM;

protected:

	void				_OnInsertItem(		CUserSession * pSession, INVEN_BUFFER* pInvenItem, INVEN_BUFFER_SET* pInvenSet );
	UINT32				_OnDeleteItem(	INT32 i32ThreadIdx,	CUserSession * pSession, T_ItemDBIdx TItemWareIdx, INVEN_BUFFER_SET* pInvenSet  );

	void				_UpdateUserListPacket(void); 	

	void				_SetItemAbilityAuth_A( CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32ItemArg );
	void				_SendOptionPacket( TYPE_RING_OUT* pOut );

public:
	CUserSessionManager(); 
	virtual ~CUserSessionManager();

	virtual BOOL		OnCreate(i3List * pSessionList);	
	virtual void		OnUpdate(void); 
	virtual BOOL		OnDestroy(void);

     INT32				GetChannelUserCount(UINT32 Channel)		{ return m_pChannelUserList[Channel]->GetCount(); }

	void				SendListInfoPaket( INT32 i32ServerIdx, CUserSession * pUser ); 

	//TransServer에서 메시지를 받느다. 
	CUserSession *		LogInUser_A( INT32 i32SessionIdx, INT32 i32Rv, UINT32 ui32AuthKey, UINT32 ui32BlockDate, LOGIN_INFO* pLoginInfo );
	void				LogInWaitUser_A( INT32 i32SessionIdx, UINT32 ui32AuthKey, INT32 i32WaitCount, UINT32 ui32LoginTimeArg, UINT8 ui8LinkThreadIdx );

	void				GetAttendanceResult( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv ); // 출석체크 이벤트 완료 보상 요청에 대한 결과값을 받습니다. - 서동권
	void				GetAttendanceCheck( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, ATTENDANCE_USER_SVR* stAttendance ); // 출석체크 이벤트 출석도장 요청에 대한 결과값을 받습니다. - 서동권
	
	void				KickUser_A(		INT32 i32SessionIdx, T_UID i64UID, UINT8 ui8Reason ); 
	
	void				DestroyRoom_A(	INT32 i32SessionIdx ); 
	void				SendRoomNoticeMessage_A(INT32 i32SessionIdx, char * strMessage, UINT32 ui32len);
	void				SendUserNowInfo_A(INT32 i32SessionIdx); 

	//채널에 들어가기 & 나가기 
	INT32				OnEnterChannel(	CUserSession * pUser, UINT32 ui32Channel );
	void				OnLeaveChannel( CUserSession * pUser, UINT32 ui32Channel );	

	//로비에 들어가기 & 나가기 
	void				OnEnterLobby( CUserSession * pSession, UINT32 ui32Channel ); 
	void				OnLeaveLobby( CUserSession * pSession, UINT32 ui32Channel ); 	
	
	//유저리스트 요청하기 
	void				OnSendLobbyUserList(CUserSession * pSession, INT32 i32Channel, LOBBY_USER_INFO* pLobbyUserInfo, UINT32 ui32UserCount);
	//초대하기 
	void				OnInviteLobbyUser( CUserSession * pSession, UINT32 Channel, UINT32 InvitedUserIdx, char * pPass);								//초대하기 
	void				OnInviteUserServer(PACKET_TRANS_COMMUNITY_USER_INVITED_REQUEST_ACK* pPacketAck);													//유저 초대하기 다른서버(Auth에서 호출됨)
	void				OnSendInviteLobbyUserList(CUserSession * pSession, UINT32 ui32Channel);

	//상점
	
	void				OnShopUserItem(		INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem);
	void				UserServerEnter_A(	INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, i3NetworkPacket* pPacket );
	void				UserServerLeave_A(	INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv );
	void				OnGoodsBuy_A(		INT32 i32SessionIdx, T_UID i64UID, UINT32 ui32Point, UINT32 ui32Cash, UINT8 ui8GoodsCount, DB_GOODS* pstDBGoods, UINT8 ui8Action  );	// 구입하기 

	// Gift
	void				OnGiftAuth_A(	CUserSession * pSession, UINT8 ui8Type, UINT8 ui8Count, AUTH_GIFT_INVEN* paAuthGift = NULL );		// 선물인증
	void				OnGiftAuthFail_A(	CUserSession * pSession, PACKET_AUTH_SHOP_AUTH_GIFT_REQ* pPacketReq, INT32 i32Rv );		// 선물인증실패
	void				OnGoodsGift_A(	CUserSession * pSession, PACKET_AUTH_SHOP_GOODS_GIFT_ACK* pPacket  );	// 선물구입
	void				OnGoodsGiftFail_A(	CUserSession * pSession, PACKET_AUTH_SHOP_GOODS_GIFT_REQ* pPacketReq, INT32 i32Rv  );	// 선물구입실패
	void				OnGetGift_A(	CUserSession * pSession, T_RESULT TResult, PACKET_AUTH_SHOP_GET_GIFTLIST_ACK* pPacket );					// 선물리스트조회

	void				OnInsertItem_A(INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, INT8 i8RequestType, UINT8 ui8ItemCount, INVEN_BUFFER_DB *pItem, UINT32 ui32CapsuleItemID);										//아이템 넣기
	void				OnDeleteItem_A(INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, T_ItemDBIdx TItemWareIdx );

	void				OnItemAuth_A(INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, INVEN_BUFFER* pInvenItem );									//개수제 아이템 인증하기 	
	void				OnItemCheatAuth_A(INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, INVEN_BUFFER* pInvenItem);									//개수제 아이템 인증하기 	

	void				OnAuthUsedWeapon_A( INT32 i32SessionIdx, T_UID i64UID, UINT16 ui16InvenCount, INVEN_BUFFER* pInvenItem );

	

	CUserSession *	GetSession(INT32 i32SessionIdx, T_UID i64UID)
	{
		if( NULL == g_pContextMain ) return NULL;
		if( SERVER_STATE_START != g_eServerState ) return NULL;
		if(i32SessionIdx >= g_pContextMain->m_i32SessionCount || i32SessionIdx < 0 ) return NULL;  
		CUserSession * pSession = (CUserSession *)m_pSessionList[ i32SessionIdx ];
		if( NULL == pSession ) return NULL;
		if(!pSession->GetIsActive())return NULL;		//할 필요없다. 로그인하면서 다시 검사한다. 
		if(pSession->m_i64UID != i64UID)return NULL;		//잘못된 인덱스 입니다. 
		return pSession; 
	}

	USER_INFO_RECORD * OnGetRecord(INT32 i32SessionIdx )
	{
		if(i32SessionIdx >= g_pContextMain->m_i32SessionCount || i32SessionIdx < 0 ) return NULL;  //잘못된 정보를 요청한것 입니다. 간단한 예외처리를 합니다. 
		CUserSession * pSession = (CUserSession *)m_pSessionList[ i32SessionIdx ];
		return &pSession->m_UserInfoRecord; 
	}

	CUserSession *	GetSession(INT32 i32SessionIdx )
	{
		if(i32SessionIdx >= g_pContextMain->m_i32SessionCount || i32SessionIdx < 0 ) return NULL;  //잘못된 정보를 요청한것 입니다. 간단한 예외처리를 합니다. 
		CUserSession * pSession = (CUserSession *)m_pSessionList[ i32SessionIdx ];
		return pSession; 
	}

	void	OnSendLobbyChatting( CUserSession * pSender, TTCHAR * pMessage, UINT16 ui16size); 
	//공지 보내기 
	void	OnSendNoticeMessage( INT32 i32ThreadIdx, INT32 i32Type, TTCHAR * strMessage, INT32 i32RGB = 0 ); 
	// 모든 유저에게 보내기
	void	OnSendAllUser( i3NetworkPacket* pPacket );

	//관제툴에서 사용합니다. 
	void				KickAllSession(void); 
	INT32				GetActiveCount()					{	return m_i32ActiveCount;	};

	void	OnSendChannelUser( INT32 i32ChannelNum, i3NetworkPacket* pPacket );

	// 전체 메시지
	void	AnnounceMessagePush( INT32 i32ThreadIdx, UINT32 ui32Protocol, S2MOPacketBase & pstPacket, UINT8 ui8Target = ANNOUNCE_MESSAGE_ALL, UINT32 ui32ChannelNum = ANNOUNCE_MESSAGE_NULL_SET, UINT32 ui32RoomIdx = ANNOUNCE_MESSAGE_NULL_SET, UINT32 ui32SessionIdx = ANNOUNCE_MESSAGE_NULL_SET);
	void	AnnounceMessageCollect(NSM_CRingBuffer *pRingBuffer);
	bool	OnSendAnnounceMessage( ANNOUNCE_MESSAGE * messageInfo );
	void	UserSetBirthDay_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, UINT32 ui32BirthDay );

	CUserSession * GetSession( T_UID UID )	// UID로 세션 얻어오기. 
	{
		if( 0 > UID ) return NULL; //잘못된 정보를 요청한것 입니다. 간단한 예외처리를 합니다.

		CUserSession * pUserSession ;

		for( INT32 i = 0; i < g_pContextMain->m_i32SessionCount; ++i )
		{
			pUserSession = (CUserSession*)m_pSessionList[i];

			//if( pSession == NULL ) ++pSession; continue;
			if( NULL == pUserSession ) continue;
			if( pUserSession->m_i64UID == UID ) return pUserSession;
		}

		return NULL;
	}
	
	void	OnMedalReward_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, UINT8 ui8ItemCount, INVEN_BUFFER_DB *pItem,  MEDAL_REWARD_DETAIL_INFO* pMedalReward );

	void	OnSendClanMatchTeamAllUser( INT32 i32CurMember, i3NetworkPacket* pRecvPacket, S2MOPacketBase* pSendPacket );
	void	OnSendClanMatchRoomLeave( T_UID TLeaderUID, INT32 i32CurMember, i3NetworkPacket* pRecvPacket, INT32 i32ChannelIdx  );
	
}; 

extern CUserSessionManager * g_pUserManager;

#endif