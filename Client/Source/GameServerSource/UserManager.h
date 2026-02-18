#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__




/*-----------------------------------------------------------------------
Name : CUserSessionManager
2007 03 06 : 버그확인 : 없음 
2007 06 25 : 로비에 있는 유저리스트를 사용해야 함. 방에서 추천을 하기 위해서 변동 되어야 함.
-----------------------------------------------------------------------*/
#include "UserSession.h"
#include "RingBuffer.h"

struct LOBBY_USER_INFO
{
	CUserSession * pSession; 
};

class CUserSessionManager : public i3NetworkSessionManager
{
	I3_CLASS_DEFINE( CUserSessionManager ); 
protected: 
	//Check User 
	INT32				m_i32SessionCheckIdx;

	INT32				m_i32ActiveCount;

	UINT32				m_ui32ChannelCount; 
	UINT32				m_ui32EachChannelMaxCount; 

	UINT32				m_ui32ChannelCountPer;

	//Channel User 
	BOOL			*	m_pChannelUserMaxFlag; 
	i3List			**	m_ppChannelUserList;		// ChannelList 차후 변경가능 지금은 카운트용으로 사용되고 있음(추후 작업이 있을것 같아 유지)
	i3Mutex			**	m_ppcsChanneUser;			// User

	/////////////////////////////////////////////////////////////////////////
	//Lobby User
	i3List			**	m_ppLobbyUserList;			// 카운트로만 사용함..나중에 사용할것 같음..
	i3Mutex			**	m_ppcsLobbyUser;			// 

	/////////////////////////////////////////////////////////////////////////
	//채팅으로 추가됨
	i3Mutex			**	m_ppcsLobbyChatting; 
	INT32			*	m_pi32LobbyUserInfoSide;
	LOBBY_USER_INFO	**	m_ppLobbyUserList0;
	UINT32			*	m_pui32LobbyUserCount0;
	LOBBY_USER_INFO	**	m_ppLobbyUserList1;
	UINT32			*	m_pui32LobbyUserCount1;	

	/////////////////////////////////////////////////////////////////////////
	//Channel Info Packet 
	INT32				m_i32UserListPacketNum;		//Send Number
	i3NetworkPacket		m_aUserListPacket[2];		//Info Packet 
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

	INT32				m_i32LastUpdate;

	UINT32				m_ui32SessionGameGuardUpdateTime;

	// 패킷 처리 테스트코드
public:
	DWORD				m_dwParseTimeHigh;
	PROTOCOL			m_ui16ParseTimeHighProtocol;
	bool				m_bParseTimeReset;

	UINT32				m_aui32LinkLoginCount[ DEF_TRANS_LINK_COUNT_MAX ];
	UINT32				m_aui32LinkLoginTimeArg[ DEF_TRANS_LINK_COUNT_MAX ];

	// HeartBit2
	UINT32				m_ui32RecvHeartBit2Count;

protected:

	void				_OnInsertItem(		CUserSession * pSession, INVEN_BUFFER* pInvenItem, INVEN_BUFFER_SET* pInvenSet );
	UINT32				_OnDeleteItem(		CUserSession * pSession, TUID i64ItemWareIdx, INVEN_BUFFER_SET* pInvenSet  );

	void				_UpdateUserListPacket(void); 

	void				_SetItemAbilityAuth_A( CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32ItemArg );
	void				_SendOptionPacket( TYPE_RING_OUT* pOut );

public:
	CUserSessionManager(); 
	virtual ~CUserSessionManager();

	virtual BOOL		OnCreate(i3List * pSessionList);
	virtual void		OnUpdate(void); 
	virtual BOOL		OnDestroy(void);

	INT32				GetChannelUserCount(UINT32 ui32Channel)		{ return m_ppChannelUserList[ui32Channel]->GetCount(); }

	void				SendListInfoPaket(	CUserSession * pUser ); 

	//TransServer에서 메시지를 받느다. 
	CUserSession *		LogInUser_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32AuthKey, char* pstrId, char* pstrCustom, char* pstrUID, UINT8 ui8LinkThreadIdx, UINT32 ui32LoginDelayTime, UINT8 ui8Popup );
	void				LogInWaitUser_A( INT32 i32SessionIdx, UINT32 ui32AuthKey, INT32 i32WaitCount, UINT32 ui32LoginTimeArg, UINT8 ui8LinkThreadIdx );

	void				GetMyInfo_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, i3NetworkPacket* pPacket );
	void				GetAttendanceInfo( INT32 i32SessionIdx, TUID i64UID, ATTENDANCE_USER* pstAttendance, ATTENDANCE_INFO* pstInfo ); // 출석체크 이벤트 정보를 받습니다. - 서동권
	void				GetAttendanceResult( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv ); // 출석체크 이벤트 완료 보상 요청에 대한 결과값을 받습니다. - 서동권
	void				GetAttendanceCheck( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, ATTENDANCE_USER* stAttendance ); // 출석체크 이벤트 출석도장 요청에 대한 결과값을 받습니다. - 서동권
	
	void				KickUser_A(		INT32 i32SessionIdx, TUID i64UID, UINT8 ui8Reason ); 
	
	void				DestroyRoom_A(	INT32 i32SessionIdx ); 
	void				SendRoomNoticeMessage_A(INT32 i32SessionIdx, char * pstrMessage, UINT32 ui32len);
	void				SendUserNowInfo_A(INT32 i32SessionIdx); 

	//채널에 들어가기 & 나가기 
	INT32				OnEnterChannel(	CUserSession * pUser, UINT32 ui32Channel );
	void				OnLeaveChannel( CUserSession * pUser, UINT32 ui32Channel );	

	//로비에 들어가기 & 나가기 
	void				OnEnterLobby( CUserSession * pSession, UINT32 ui32Channel ); 
	void				OnLeaveLobby( CUserSession * pSession, UINT32 ui32Channel ); 
	
	//유저리스트 요청하기 
	void				OnSendLobbyUserList(CUserSession * pSession, UINT32 ui32Channel); 
	//void				OnSendLobbyUserListNew(CUserSession * pSession, UINT32 ui32Channel, i3NetworkPacket* pPacket);
	//초대하기 
	void				OnInviteLobbyUser( CUserSession * pSession, UINT32 ui32Channel, UINT32 ui32InvitedUserIdx, char * pPass);								//초대하기 
	void				OnFriendInviteUserServer(UINT32 ui32SessionIdx, TUID i64UID, UINT8 ui8FriendIdx);													//친구 초대하기 다른서버(Auth에서 호출됨)

	//상점
	
	void				OnShopUserItem(		INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem);
	void				UserServerEnter_A(	INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, i3NetworkPacket* pPacket );
	void				UserServerLeave_A(	INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv );
	void				UserCreateNickName_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, char* pstrNickName );
	void				OnGoodsBuy_A(		INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32GoodsID, UINT8 ui8BuyType, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem, UINT32 ui32Point, UINT32 ui32Cash, char* pstrRedirectUrl );	// 구입하기 
	void				OnGoodsGift_A(		INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32GoodsID, UINT32 ui32Point, UINT32 ui32Cash );	// 선물하기
	void				OnGiftAuth_A(		INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem);						// 선물인증

	void				OnInsertItem_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, INT8 i8RequestType, UINT32 ui32InsertArg, INVEN_BUFFER* pInvenItem );										//아이템 넣기
	void				OnDeleteItem_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, TWareDxIdx i64ItemWareIdx );
	void				OnItemRepair_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, INVEN_BUFFER* pInvenItem, UINT32 ui32Point, UINT32 ui32Cash );

	void				OnItemAuth_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, INVEN_BUFFER* pInvenItem );									//개수제 아이템 인증하기 	
	void				OnGiftList_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, USER_GIFT_LIST UserGiftList );							//창고 리스트

	void				OnAuthUsedWeapon_A( INT32 i32SessionIdx, TUID i64UID, UINT16 ui16InvenCount, INVEN_BUFFER* pInvenItem );

	void				OnGiftNotify_A(INT32 i32SessionIdx, TUID i64UID, GIFT_BUFFER GiftInfo );	// 선물 받음 알림.

	//2차계정
	void				SendRetCheckSID(	INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv );
	void				SendRetCreateSID(	INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv );

	CUserSession *	GetSession(INT32 i32SessionIdx, TUID i64UID)
	{
		if( NULL == g_pContextMain ) return NULL;
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

	void	OnSendLobbyChatting( CUserSession * pSender, char * pstrMessage, UINT16 ui16size); 
	//공지 보내기 
	void	OnSendNoticeMessage( INT32 i32ThreadIdx, INT32 i32Type, char * pstrMessage, UINT32 ui32len ); 
	// 모든 유저에게 보내기
	void	OnSendAllUser( i3NetworkPacket* pPacket );

	//관제툴에서 사용합니다. 
	void				KickAllSession(void); 
	INT32				GetActiveCount()					{	return m_i32ActiveCount;	};

	//보급 상자
	void	OnSendSupplyBoxAnnounceMessage( INT32 i32ThreadIdx, SUPPLY_BOX_ANNOUNCE_SETTING *pstSupplyBoxAnnounce );
	void	OnSendSupplyBoxPresentMessage( INT32 i32ThreadIdx, UINT8 ui8PresentMessage, UINT32 ui32ServerIdx, UINT32 ui32ChannelNum, UINT32 ui32RoomIdx, UINT32 ui32ItemID, UINT8 ui8Size, char *strNick );

	void	OnSendChannelUser( INT32 i32ChannelNum, i3NetworkPacket* pPacket );

	// 전체 메시지
	void	AnnounceMessagePush( INT32 i32ThreadIdx, UINT32 ui32Protocol, UINT32 ui32BufferSize, const char * pBuffer, UINT8 ui8Target = ANNOUNCE_MESSAGE_ALL, UINT32 ui32ChannelNum = ANNOUNCE_MESSAGE_NULL_SET, UINT32 ui32RoomIdx = ANNOUNCE_MESSAGE_NULL_SET, UINT32 ui32SessionIdx = ANNOUNCE_MESSAGE_NULL_SET);
	void	AnnounceMessageCollect(NSM_CRingBuffer *pRingBuffer);
	bool	OnSendAnnounceMessage( ANNOUNCE_MESSAGE * pmessageInfo );

	// 쿠폰번호를 이용한 보상 아이템 내려받기
	void	OnCouponAwardItem_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32ItemID, char* pstrCouponNum, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem );


	void	OnRSGoodsBuy_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, RS_TAB eTabIdx, UINT32 ui32TabGoodsID, UINT32 ui32PrizeGoodsID, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem,  UINT32 ui32Cash, char* pstrRedirectUrl, RS_ITEM_GRADE eItemGrade, UINT8 ui8RsIdx,  UINT8 ui8TabUseCount, UINT32 ui32RSLastBaseInfoTime, RS_BASE_INFO* pRsBaseInfo, RS_RESULT_TYPE eResultType );
	void	OnRSEnter_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT8 ui8Tab1UseCount, UINT8 ui8Tab2UseCount, UINT8 ui8Tab3UseCount, RS_BASE_INFO* pRsBaseInfo );
	void	OnRSSendItemInfo_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, RS_ALL_ITEM_INFO* pItemInfo );

	void	UserSetBirthDay_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32BirthDay );

	CUserSession * GetSession( TUID UID )	// UID로 세션 얻어오기. 
	{
		if( 0 > UID ) return NULL; //잘못된 정보를 요청한것 입니다. 간단한 예외처리를 합니다.

		CUserSession * pUserSession ;

		for( INT32 i = 0; i < g_pContextMain->m_i32SessionCount; ++i )
		{
			pUserSession = (CUserSession*)m_pSessionList[i];

			if( NULL == pUserSession ) continue;
			if( pUserSession->m_i64UID == UID ) return pUserSession;
		}

		return NULL;
	}


}; 

extern CUserSessionManager * g_pUserManager;

#endif