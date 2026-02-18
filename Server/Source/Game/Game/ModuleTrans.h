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

	CONNECT_SERVER_NET_BUFFER	m_sTransMainMessage;
	CONNECT_SERVER_NET_BUFFER	m_sTransTransMessage;
	CONNECT_SERVER_NET_BUFFER	m_sTransClanMessage;
	CONNECT_SERVER_NET_BUFFER	m_sTransMessMessage;
	CONNECT_SERVER_NET_BUFFER*	m_pTransUserMessage;

protected:
	INT32						_TransConnect(void);
	INT32						_TransUpdate(void);

	BOOL						_TransSendUserInfo(		i3NetworkPacket* pNetMessage, CUserSession* pSession );									// Trans 저장용보내는 패킷을 만듭니다.
	BOOL						_TransSendCharaInfo(	INT32 i32WorkIdx, CUserSession* pSession );												// Trans 저장용보내는 패킷을 만듭니다.
	BOOL						_TransSendCharaPacket(	UINT8 ui8Count, INT32* pi32CharaSlot, INT32 i32WorkIdx, CUserSession* pSession );												// Trans 저장용보내는 패킷을 만듭니다.

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
	BOOL						AuthSendLogIn(			INT32 i32WorkIdx, CUserSession * pSession, char* strID, char * strPass, UINT16 ui16SessionSize, char* pSessionKey, UINT8 ui8LinkMethod );	// 로그인(일반)
	BOOL						AuthSendLogOut(			INT32 i32WorkIdx, CUserSession * pSession );														// 로그아웃
	BOOL						AuthSendGetUserInfo(	INT32 i32WorkIdx, CUserSession * pSession );														// 유저 정보 세팅
	BOOL						AuthSendCreateChar(		INT32 i32WorkIdx, CUserSession * pSession, PACKET_CHAR_CREATE_CHARA_REQ* pCreateChar, T_ItemDBIdx TCharaDBIdx );				// 유저 케릭터 생성
	BOOL						AuthSendServerEnter(	INT32 i32WorkIdx, CUserSession * pSession );														// 서버입장(서버이동)
	BOOL						AuthSendServerLeave(	INT32 i32WorkIdx, CUserSession * pSession );														// 서버나가기(서버이동)
	BOOL						AuthSendChannelEnter(	INT32 i32WorkIdx, CUserSession * pSession );														// 채널입장
	BOOL						AuthSendChannelLeave(	INT32 i32WorkIdx, CUserSession * pSession );														// 채널나오기
	BOOL						AuthSendRoomEnter(		INT32 i32WorkIdx, CUserSession * pSession );														// 방 입장
	BOOL						AuthSendRoomLeave(		INT32 i32WorkIdx, CUserSession * pSession );														// 방 나오기
	BOOL						AuthSendCreateNickName(	INT32 i32WorkIdx, CUserSession * pSession, TTCHAR* strNickName );										// 닉네임 생성
	BOOL						AuthSendUserNowInfo(	INT32 i32WorkIdx, INT32 i32SessionIdx, INT32 i32ChannelIdx, INT32 i32RoomIdx, INT32 i32SlotIdx);	// 유저정보 요청하기 
	BOOL						AuthSendAliveUser(		INT32 i32WorkIdx, UINT8 ui8Count, CUserSession ** ppSession );										// 살아있는 유저 패킷 보내기
	BOOL						AuthSendUserRecord(		INT32 i32WorkIdx, CUserSession * pSession, T_UID i64FUID );											// 전적 최소 정보 요청

	BOOL						AuthSendShopVersion(	INT32 i32WorkIdx );																					// 상점 버전 요청
	BOOL						AuthSendShopList(		INT32 i32WorkIdx );																					// 상점 리스트 요청

	BOOL						AuthSendUserItem(		INT32 i32WorkIdx, CUserSession * pSession );														// 로그인시 유저 아이템 요청
	BOOL						AuthSendBuyGoodsBasket( INT32 i32WorkIdx, CUserSession * pSession, BUY_BASKET *stGoodsData, UINT8 ui8GoodsCount );			// 상점에서 장바구니로 구입하기

	// Goods 지급이면 AuthSendRewardItem() 함수 호출해주세요!!. 
	BOOL						AuthSendInsertItem(		INT32 i32WorkIdx, CUserSession * pSession, ITEM_INSERT_REQUEST_TYPE eRequestType, UINT8 ui8ItemCount, ST_ITEM* pItem );			// 상점에서 아이템 넣기 

	BOOL						AuthSendRewardItem( INT32 i32WorkIdx, CUserSession * pSession, REWARD_ITEM *pReward );										// 보상아이템 선물함 지급
	BOOL						AuthSendDeleteItem(		INT32 i32WorkIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx );							// 상점에서 아이템 삭제
	BOOL						AuthSendRepair(			INT32 i32WorkIdx, CUserSession * pSession, S2MO_REPAIR_INFO *pstInfo );
	BOOL						AuthSendAuthItem(		INT32 i32WorkIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx, INT32 i32ItemArg, UINT8 ui8CharaSlotIdx = 0, UINT32 ui32ItemID = 0);			// 상점에서 아이템 인증
	BOOL						AuthSendAuthItemData(	INT32 i32WorkIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx, UINT32 ui32ItemID, UINT8 ui8Ability, void* pAbility );	// 인증 아이템중 데이터 포함
	BOOL						AuthSendAuthCountItem(		INT32 i32WorkIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx, INT32 i32ItemArg, TLoopCnt ui16ItemCount, UINT8 ui8CharaSlotIdx = 0, UINT32 ui32ItemID = 0);			// 상점에서 아이템 인증
	BOOL						AuthSendGiftGoods(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8GiftCount, BUY_BASKET *pGift, TTCHAR* strReceiverNick, TTCHAR* pstrMessage );	// 선물하기
 	BOOL						AuthSendUserGift(		INT32 i32WorkIdx, CUserSession * pSession );														// 선물함확인
	BOOL						AuthSendAuthGift(		INT32 i32WorkIdx, CUserSession * pSession, PACKET_AUTH_SHOP_AUTH_GIFT_REQ *pPacket );				// 선물인증
	BOOL						AuthSendGetRemainMoney(	INT32 i32WorkIdx, CUserSession * pSession );														// 보유 포인트, 톨 받기		
	BOOL						AuthSendChangeColorNick(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Color );										// 칼라 닉네임
	BOOL						AuthSendUsedWeapon(		INT32 i32WorkIdx, CUserSession * pSession, UINT16 ui16ItemCount, USED_ITEM* pUsedItemList );		// 사용한 아이템 인증 요청
	BOOL						AuthSendCheckNickName(	INT32 i32WorkIdx, CUserSession * pSession, TTCHAR* pstrNickName );									// 닉네임 중복검사.	
	BOOL						AuthSendAttendanceItemReq( INT32 i32WorkIdx, T_UID i64UID, UINT32 ui8AttendanceNum, UINT8 ui8AttendanceItemNum, UINT8 ui8AttendanceType );			// 출석체크 이벤트 완료 보상 요청
	BOOL						AuthSendAttendanceReq(	INT32 i32WorkIdx, T_UID i64UID, UINT32 ui32AttendanceNum, UINT8 ui8AttendanceType );
	BOOL						TransSendAccountKick(	INT32 i32WorkIdx, T_UID i64UID );
	BOOL						TransSendGMBlockUser(	INT32 i32WorkIdx, T_UID i64UID, const TTCHAR *pstrComment);
	BOOL						AuthSendExtendItem(		INT32 i32WorkIdx, CUserSession * pSession, EXTEND_BASKET* stExtendItemData );						// 기간 연장
	BOOL						AuthSendGetUserInfoDetailInfo( INT32 i32WorkIdx, CUserSession * pSession, T_UID i64UID );
	BOOL						AuthSendChangeClanMark( INT32 i32WorkIdx, UINT32 ui32ClanMark, UINT8 ui8SendUserCount, T_UID* pTUIDList);
	BOOL						AuthSendChangeClanName( INT32 i32WorkIdx, TTCHAR* pstrClanName, UINT8 ui8SendUserCount, T_UID* pTUIDList );
	
#ifdef USE_TRANS_FIND
	BOOL						AuthSendFindNickName(	INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName );									// 유저 찾기.
#endif

	BOOL						AuthSendSetBirthDay(		INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32BirthDay );								// 닉네임 생성
	BOOL						AuthSendCharaDelete(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8CharaSlotIdx, T_ItemDBIdx TItemDBIdx );								// 캐릭터 삭제
	BOOL						AuthSendBasicUserInfoNick(	INT32 i32WorkIdx, CUserSession * pSession, TTCHAR * pstrNickName );								// 유저 기본 정보 요청
	BOOL						AuthSendBasicUserInfoUID(	INT32 i32WorkIdx, CUserSession * pSession, T_UID i64UID );										// 유저 기본 정보 요청

	BOOL						CommunitySendInviteUser(	INT32 i32WorkIdx, PACKET_COMMUNITY_USER_INVITED_REQ* pPacketReq);

	BOOL						SendGachaPurchase(		INT32 i32WorkIdx, INT64 i64UID, UINT8 ui8BuyType, UINT32 TGoodsID, INT32 i32ItemIdx );
	BOOL						AuthSendMedalReward(	INT32 i32WorkIdx, CUserSession * pSession, MEDAL_REWARD_INFO* pMedalRewadInfo );

	BOOL						AuthSendApplyPenalty(	INT32 i32WorkIdx, CUserSession * pSession, PACKET_GMCHAT_APPLY_PENALTY_REQ* pApplyPenaltyReq );
	BOOL						AuthSendUseCoupon(		INT32 i32WorkIdx, CUserSession * pSession, char* pstrCoupon );										// 페이레터 쿠폰 사용

	BOOL						AuthSendExpireDeleteItem(INT32 i32WorkIdx, CUserSession * pSession, UINT16 ui16ItemCount, T_ItemDBIdx  * pTItemDBIdx);
};

extern CModuleTrans * g_pModuleTrans; 

#endif
