#ifndef __MODULE_TRANS_H__
#define __MODULE_TRANS_H__

class CUserSession;
class CModuleTransSocket;

class CModuleTrans : public i3Thread
{
	I3_CLASS_DEFINE( CModuleTrans );
private:
	BOOL						m_bRunning;
	UINT32						m_ui32ThreadCount;

	INT32						m_i32LastUpdateTime;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AUTH	
	CModuleTransSocket	*		m_pTransSocket;
	UINT32						m_ui32AuthConnectTime; 	

	SERVER_NET_BUFFER			m_sTransMainMessage;
	SERVER_NET_BUFFER			m_sTransTransMessage;
	SERVER_NET_BUFFER			m_sTransClanMessage;
	SERVER_NET_BUFFER			m_sTransMessMessage;
	SERVER_NET_BUFFER		*	m_pTransUserMessage;

protected:
	INT32						_TransConnect(void);
	INT32						_TransUpdate(void);

	BOOL						_TransSendUserInfo(		i3NetworkPacket* pNetMessage, CUserSession* pSession );													// Trans 저장용보내는 패킷을 만듭니다.

public: 
	CModuleTrans(void); 
	virtual ~CModuleTrans(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	INT32						OnCreate( UINT32 ui32ThreadCount );
	void						OnDestroy(void);
	virtual UINT32				OnRunning( void * pUserData);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Trans
	BOOL						AuthSendLogIn(			INT32 i32WorkIdx, CUserSession * pSession, char * pPass );												// 로그인(일반)
	BOOL						AuthSendLogInWEB(		INT32 i32WorkIdx, CUserSession * pSession );															// 로그인(WEB)
	BOOL						AuthSendLogInRussia(	INT32 i32WorkIdx, CUserSession * pSession, char * pPass );												// 로그인(러시아)
	BOOL						AuthSendLogInThai(		INT32 i32WorkIdx, CUserSession * pSession, INT16 i16TokenSize, char * pToken );							// 로그인(태국_가레나)
	BOOL						AuthSendLogOut(			INT32 i32WorkIdx, CUserSession * pSession );															// 로그아웃
	BOOL						AuthSendGetMyInfo(		INT32 i32WorkIdx, CUserSession * pSession );															// 유저 정보 세팅
	BOOL						AuthSendServerEnter(	INT32 i32WorkIdx, CUserSession * pSession );															// 서버입장(서버이동)
	BOOL						AuthSendServerLeave(	INT32 i32WorkIdx, CUserSession * pSession );															// 서버나가기(서버이동)
	BOOL						AuthSendCreateNickName(	INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName );										// 닉네임 생성
	BOOL						AuthSendUserNowInfo(	INT32 i32WorkIdx, INT32 i32SessionIdx, INT32 i32ChannelIdx, INT32 i32RoomIdx, INT32 i32SlotIdx);		// 유저정보 요청하기 
	BOOL						AuthSendAliveUser(		INT32 i32WorkIdx, UINT8 ui8Count, CUserSession ** ppSession );											// 살아있는 유저 패킷 보내기
	BOOL						AuthSendUserRecord(		INT32 i32WorkIdx, CUserSession * pSession, TUID i64FUID );												// 전적 최소 정보 요청

	BOOL						AuthSendShopVersion(	INT32 i32WorkIdx );																						// 상점 버전 요청
	BOOL						AuthSendShopList(		INT32 i32WorkIdx );																						// 상점 리스트 요청

	BOOL						AuthSendUserItem(		INT32 i32WorkIdx, CUserSession * pSession );															// 로그인시 유저 아이템 요청
	BOOL						AuthSendBuyGoods(		INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32GoodsID, UINT8 ui8BuyType, UINT32 ui32CouponID, INT64 i64CouponIdx );		// 상점에서 구입하기
	BOOL						AuthSendBuyGoodsBasket( INT32 i32WorkIdx, CUserSession * pSession, BUY_BASKET *pGoodsData, UINT8 ui8GoodsCount );				// 상점에서 장바구니로 구입하기
	BOOL						AuthSendInsertItem(		INT32 i32WorkIdx, CUserSession * pSession, ITEM_INSERT_REQUEST_TYPE eRequestType, UINT32 ui32ItemID, UINT32 ui32AuthDay );		//상점에서 아이템 넣기 
	BOOL						AuthSendDeleteItem(		INT32 i32WorkIdx, CUserSession * pSession, TUID i64ItemWareIdx );										//상점에서 아이템 삭제
	BOOL						AuthSendRepairItem(		INT32 i32WorkIdx, CUserSession * pSession, TUID i64ItemWareIdx, UINT32 ui32Point );						//상점에서 아이템 삭제
	BOOL						AuthSendAuthItem(		INT32 i32WorkIdx, CUserSession * pSession, TUID i64ItemWareIdx, INT32 i32ItemArg );						//상점에서 아이템 인증
	BOOL						AuthSendAuthItemData(	INT32 i32WorkIdx, CUserSession * pSession, TUID i64ItemWareIdx, UINT32 ui32ItemID, UINT8 ui8Ability, void* pAbility );	// 인증 아이템중 데이터 포함
	BOOL						AuthSendGiftGoods(		INT32 i32WorkIdx, CUserSession * pSession, char* pstrReceiverNick, UINT32 ui32GoodsID, char* pstrMessage );				//선물하기
 	BOOL						AuthSendUserGift(		INT32 i32WorkIdx, CUserSession * pSession );															//선물함확인
	BOOL						AuthSendAuthGift(		INT32 i32WorkIdx, CUserSession * pSession, TGiftDxIdx i32GiftWareIdx );									//선물받기
	BOOL						AuthSendGetRemainMoney(	INT32 i32WorkIdx, CUserSession * pSession );															//보유 포인트, 톨 받기	
	BOOL						AuthSendChangeRank(		INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32Rank );											//계급 변경 통보
	BOOL						AuthSendChangeColorNick(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Color );											//칼라 닉네임
	BOOL						AuthSendUsedWeapon(		INT32 i32WorkIdx, CUserSession * pSession, UINT16 ui16ItemCount, USED_ITEM* pUsedItemList );			//사용한 아이템 인증 요청
	BOOL						AuthSendCheckNickName(	INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName );										//닉네임 중복검사.	
	BOOL						AuthSendAttendanceItemReq( INT32 i32WorkIdx, INT64 i64UID, UINT32 ui8AttendanceNum, UINT8 ui8AttendanceItemNum );				//출석체크 이벤트 완료 보상 요청
	BOOL						AuthSendAttendanceInfoReq( INT32 i32WorkIdx, INT64 i64UID );																	//출석체크 이벤트 완료 보상 요청
	BOOL						AuthSendAttendanceReq( INT32 i32WorkIdx, INT64 i64UID, UINT32 ui32AttendanceNum );
	BOOL						TransSendAccountKick(	INT32 i32WorkIdx, TUID i64UID );
	BOOL						TransSendGMBlockUser(	INT32 i32WorkIdx, TUID i64UID, const char *pstrComment);

#ifdef USE_TRANS_FIND
	BOOL						AuthSendFindNickName(	INT32 iWorkIdx, CUserSession * pSession, char* strNickName );											// 유저 찾기.
#endif

	//---------------------------------------------------------------------------------
	// 보급상자
	//---------------------------------------------------------------------------------
	BOOL						BattleSendSupplyBoxResultReq( INT32 i32WorkIdx, INT32 i32ChannelNum, INT32 i32RoomIdx, UINT32 ui32RoomStateCheckTime, INT8 i8BattleMemberCount, USER_RANK_INFO *stUserRankInfo );	// 보급상자 지급 여부 요청
	
	//---------------------------------------------------------------------------------
	// Award Item : 보상 내려받기
	//---------------------------------------------------------------------------------
	BOOL						AuthSendCouponAwardItemReq(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8CouponNumSize, char* pstrCouponNum );

	//---------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------
	// 뽑기 상점
	//---------------------------------------------------------------------------------
	UINT8						m_ui8RSEnable;	// 0 : 뽑기 상점 시스템 사용 안함. 1 : 뽑기 상점 시스템 사용.
	RS_ALL_ITEM_INFO			m_stRSItemInfo; // 클라이언트에게 내려줄 뽑기상점 상품 구성 정보

	BOOL						AuthSendRSStartReq(  INT32 i32WorkIdx, CUserSession * pSession, RS_ROULETTE_INFO* pRSRouletteInfo, BOOL bIsHidden );			// 뽑기 시작 요청.
	BOOL						AuthSendRSEnterReq( INT32 i32WorkIdx, CUserSession * pSession  );																// 뽑기 상점 입장 요청.
	BOOL						AuthSendRSItemInfoReq( INT32 i32WorkIdx, CUserSession * pSession  );															// 아이템 정보 요청.
	BOOL						AuthSendRSJackPotNotifyReq( INT32 i32WorkIdx, CUserSession * pSession, char * pstrNickName  );									// 잭팟 당첨 공지 요청.

	BOOL						AuthSendSetBirthDay(	INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32BirthDay );										// 닉네임 생성

	//----------------------------------------------------------------------------------
	// 2차 계정 중복확인/생성
	//----------------------------------------------------------------------------------
	BOOL						AuthSendCheckSID( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8IDSize, char* pstrSecurityID );
	BOOL						AuthSendCreateSID( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8IDSize, char* pstrSecurityID, UINT8 ui8PWSize, char* pstrSecurityPW );
	BOOL						AuthSendTestCreateSID( INT32 i32WorkIdx );
};

extern CModuleTrans * g_pModuleTrans; 

#endif
