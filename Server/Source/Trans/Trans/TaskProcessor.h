#ifndef _TASKPROCESSOR_H
#define _TASKPROCESSOR_H

#include "CapsuleItem.h"
#include "UserManager.h"
#include "LocalRingBuffer.h"
#include "ModuleDBNick.h"
#include "ModuleDBUID.h"
#include "ModuleDBUserLoad.h"
#include "ModuleDBUserSave.h"
#include "ModuleDBStats.h"
#include "ModuleDBUserInfo.h"
#include "ModuleDBKickList.h"
#include "ListIndexingLink.h"
#include "UserEvent.h"

#pragma pack( push, 1 )
#define MAX_CAPSULE_REWARD_COUNT 5

struct GSM_ACCOUNT_KICK
{
	unsigned char	_strGusID[NET_NCGUSID_SIZE];
	UINT8			_ui8Reason;
	INT32			_i32UID;
};//INT32->INT64

struct GSM_ALARM_COIN
{
	unsigned char	_strGusID[NET_NCGUSID_SIZE];
	INT32			_i32Paidcoind;
	INT32			_i32Freecoin;
};

#pragma pack( pop )

class CUserSessionManager;
class CPacketLocker;
class CGateway;
class CLinkManager; 
class CModuleCash;
class CModulePHCash;
class CModuleRUCash;
class CModuleItalyCash;
class CLinkBilling_Brazil;
class CIPChecker;
class CGeoIP2;
class CGeneralRankUP;

class CTaskProcessor: public i3Thread
{
	I3_CLASS_DEFINE(CTaskProcessor);
private:
	CSystemTimer*			m_pSystemTimer;
	i3NetworkServerContext*	m_pServerContext;
	CUserSessionManager*	m_pSessionManager;

	INT32					m_i32LogIdx;

	BOOL					m_bIsRunning;
	UINT32					m_ui32ReadedLength;	
	UINT32					m_ui32GoodsUpdateTime;
	UINT32					m_ui32CheckUserIdx;
	UINT32					m_ui32ShopTime;

	UINT32					m_ui32AutoSaveTime;
	UINT32					m_ui32AutoSaveUserIdx;

	INT32					m_i32StOldMonth;
	INT32					m_i32StOldDay;
	INT32					m_i32StOldHour;
	INT32					m_i32StOldMinute;

	CModuleDBNick*			m_pDBNick;
	CModuleDBUID*			m_pDBUID;
	CModuleDBUserLoad*		m_pDBUserLoad;
	CModuleDBUserSave*		m_pDBUserSave;
	CModuleDBStats*			m_pDBStats;
	CModuleDBUserInfo*		m_pDBUserInfo;
	CModuleDBKickList*		m_pDBKickList;

	S2RingBuffer*			m_pRingSIAPop;

	CIPChecker*				m_pIPChecker;
	CGeoIP2*				m_pGeoIP2;
	
	STATS_INFO				m_StatsInfo;

	CGeneralRankUP*			m_pGeneralRankUP;

	BOOL					m_bFirstLogin;
	UINT32					m_ui32LoginTimeArg;

	WORK_PROFILE		m_stWorkProfile;
	

#ifdef I3_DEBUG
	// Debug 하기 위해 만든 인덱스입니다.
	INT32*					m_pi32CapsuleIdx;
#endif
	INT32					m_i32CapsuleCount;
	CCapsuleItem**			m_ppCapsule;

	// Web 세션 명령을 받는 버퍼
	Local_CRingBuffer*		m_pRingASCWeb;

	void					_UpdateStatsTime();
	
	//For Log
	UINT32					m_ui32WorkCount_Net;
	UINT32					m_ui32WorkCount_Link;
	UINT32					m_ui32WorkCount_DB_Item;
	UINT32					m_ui32WorkCount_DB_Buy;
	UINT32					m_ui32WorkCount_DB_Inven;
	UINT32					m_ui32WorkCount_DB_Shop;
	UINT32					m_ui32WorkCount_DB_Gift;
	UINT32					m_ui32WorkCount_DB_Repair;
	UINT32					m_ui32WorkCount_DB_Coupon;
	UINT32					m_ui32WorkCount_DB_Gacha;

	UINT32					m_ui32SleepCount;
	UINT32					m_ui32WorkCount;
	UINT32					m_ui32LogFileHour;
	UINT32					m_ui32UseCash;
	UINT32					m_ui32TodayUseCash;

	UINT32					m_ui32UseingThread;

	void ResetCounter(void)
	{
		m_ui32WorkCount_Net = 0;
		m_ui32WorkCount_Link = 0;
		m_ui32WorkCount_DB_Item = 0;
		m_ui32WorkCount_DB_Buy = 0;
		m_ui32WorkCount_DB_Inven = 0;
		m_ui32WorkCount_DB_Shop = 0;
		m_ui32WorkCount_DB_Gift = 0;
		m_ui32WorkCount_DB_Repair = 0;
		m_ui32WorkCount_DB_Coupon = 0;
		m_ui32WorkCount_DB_Gacha = 0;
		m_ui32UseCash = 0;
		m_ui32SleepCount = 0;
		m_ui32WorkCount  = 0;
	}

	//For ShopInfoUpdate 
	UINT32					m_ui32ShopInfoUpdateTime;

	UINT32					m_ui32GachaShopUpdateTime;

	UINT32					m_ui32GeneralRankupSendCount;

	INT32				m_i32CapsuleLoadFlag;
	INT32				m_i32CapsuleLoadMax;

private:
	//외부 서버로부터의 패킷 처리 
	void					_ParsingGame( UINT32 ui32idxBuffer, i3NetworkPacket* pPacket );
		
	void 					_PlusPoint( INT32 i32ThreadIdx, INT32 i32Serveridx, INT32 i32Sessionidx, T_UID T_DBIdx, UINT32 ui32Point, GET_POINT_ROUTE eGetRoute );
	void 					_AlarmCoin( INT32 i32Serveridx, INT32 i32Sessionidx, T_UID i64DBIdx, INT32 i32Result, UINT32 ui32Point, UINT32 ui32Cash );
	void					_UserLogin( ST_PACKET_LOGIN_REQ* pLoginReq );
	void					_SendLogInPacket( INT32 i32Result, INT32 i32ServerIdx, INT32 i32SessionIdx, UINT32 ui32AuthKey, UINT32 ui32BlockDate, LOGIN_INFO* pLoginInfo );
	void					_SendLoginBlock(UID_OUT* pUidOut);
	void					_LogoutCommand( USER_NODE * pNode );

	void					_GetUserSaveData( i3NetworkPacket* pPacket, USER_SAVE_DATA* pstUserSaveData );
	void					_GetCharaSaveData( UINT8 ui8Count, i3NetworkPacket* pPacket, USER_SAVE_CHARA_DATA* pstUserSaveData );

	INT32 					_SetItemAbilityAuth( USER_NODE * pNode, UINT32 ui32ItemID, T_ItemDBIdx TItemWareIdx, UINT8 ui8Ability, void* pAbility );
	void					_CheckaleItem( USER_NODE* pNode );	

	BOOL					_SaveUserInfo(USER_NODE * pNode, i3NetworkPacket * pPacket, INT32 i32State, UINT32 ui32ServerIdx ); 
	BOOL					_SaveCharaInfo(USER_NODE * pNode, UINT8 ui8Count, i3NetworkPacket * pPacket, UINT32 ui32ServerIdx ); 

	BOOL 					_AccountKick( T_UID i64DBIdx, INT32 i32Reason );
	void 					_ASCWeb_UserInfoLocationReq( ASC_WEB_REQ_INFO *pReqInfo );

	// TaskUpdate
	bool					_UpdateSIA();

	bool					_UpdateLinkRecv();

	bool					_UpdateUIDOut();
	void					_DoTaskUIDOut( UID_OUT * pUIDOut );

	bool					_UpdateNick();
	
	BOOL					_UpdateCreateNick();
	BOOL					_UpdateCreateChar();
	BOOL					_UpdateExitSave();
	bool					_UpdateUserLoad();
	BOOL					_UpdateKickList();

	void					_UserExit( USER_NODE * pNode );

	bool					_UpdateASCWeb();

	void					_UpdateCheckUser();
	bool					_UpdateAutoSave();

	bool					_UpdateGeneralRankResult();

	BOOL					_UpdateDBUserInfo();				// 유저 정보 보기

	bool					_CheckAttendanceTarget( USER_NODE * pNode, ATTENDANCE_INFO* pEventInfo, UINT8 ui8Type, bool bChangeEvent );	// 출석 체크 이벤트 대상인지 확인
	void					_SetAttendanceInfo( USER_NODE * pNode );																	// 클라에게 내려줄 출첵 진행 정보
	INT32					_CheckAttendanceMark( USER_NODE* pNode, UINT32 ui32Version, UINT8 ui8Type );								// 출석체크 도장찍기 요청시 조건 검사
	INT32					_CheckAttendanceInfo( USER_NODE* pNode, UINT32 ui32Version, UINT8 ui8ItemNum, UINT8 ui8Type );				// 출석체크 보상 지급
	
	T_RESULT				_PacketCreateChara(USER_NODE* pNode, i3NetworkPacket* pPacket );
	INT32					_CreateChar(  RING_CREATE_CHAR* pCreateChar );

	bool					_UpdateShop();

	T_RESULT				_BuyGoods( USER_NODE* pNode, DB_SHOP_GOODS* pDBShopBuy );

	// 인벤토리에 아이템 추가시 사용. 
	INT32					_ItemInsertDB( T_UID TUID, ITEM_INSERT_REQUEST_TYPE eRequestType, UINT8 ui8Itemcount, ST_ITEM* pDBItem, UINT32 ui32CapsuleItemID = 0, MEDAL_REWARD_INFO* pMedalReward = NULL ); 
	
	// 선물함에 굿즈 추가시 사용.  
	INT32					_GiftInsertDB( SEND_GIFT *pGift, ITEM_INSERT_REQUEST_TYPE eType =  ITEM_INSERT_REQUEST_EVENT_NEWUSER );
	float					_GetRepairPremium( UINT32 ui32Endurance );
	
	// MAIN_STREAM_BUFFER 할당 전 실패한 경우
	void					_SendDataRepair( UINT32 ui32ServerIdx,  INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, UINT32 ui32TotalPoint );

	// 타입별 실패.
	void					_SendDataFail_Type( ENUM_TYPE_SHOP eType, UINT32 ui32ServerIdx, T_UID i64UID, INT32 i32Result, INT32 i32SessionIdx );
	void					_SendDataRepair( T_UID i64UID, S2MO_REPAIR_INFO* pstInfo );

	void					_SetUserDetailInfo( S2_USER_DETAIL_INFO& stInfo, USER_NODE* pNode );

	void					_MakeUserInfoPacket( i3NetworkPacket* pPacket, USER_NODE *pNode, BOOL bClan );
	void					_MakeMedalInfoPacket( i3NetworkPacket *pPacket, USER_NODE *pNode );

	void					_SendUserInvenPacket( USER_NODE *pNode, UINT16 ui16InvenCount );
	void					_SendUserCharaPacket( USER_NODE *pNode );
	
	INT32					_CheckAndSet_Goods( T_UID TUID, DB_GOODS* pDBGoods, UINT8 ui8ItemInsertType );	// Goods정보가 유효한지 검사하고, Goods리스트의 Item정보 검사 및 셋팅합니다.

	void					_SendDataShopBuy( USER_NODE *pNode, DB_SHOP_GOODS *pShopBuy, TYPE_CURRENT_USER_CASH eCashType = TYPE_CURRENT_USER_CASH_NONE, UINT32 ui32LinkUserCash = 0 );
	
	void					_SendFailCreateChara(USER_NODE *pNode, INT32 i32Rv );

	// 캐쉬 결제후 유저 캐쉬 업데이트 및 캐쉬 로그 기록.
	void					_UpdateCash( USER_NODE *pNode, UINT32 ui32GoodsPrice, UINT32 ui32LinkUserCash ,TYPE_CURRENT_USER_CASH eCashType = TYPE_CURRENT_USER_CASH_NONE );

	void					_SendGiftAuth( USER_NODE* pNode, DB_GIFT_OUT* pGift  );
	void					_SendGiftRecv( T_UID TUID, SEND_GIFT* pGift );	// 선물 받는 사람에게 Send
	void					_SendGiftRecv_Inven(T_UID TUID, SEND_GIFT* pGift);	// 선물 받는 사람에게 Send(Inven Insert)
	void					_SendGiftSender( USER_NODE *pNode, SEND_GIFT* pGift, TYPE_CURRENT_USER_CASH eCashType = TYPE_CURRENT_USER_CASH_NONE, UINT32 ui32LinkUserCash = 0 ); // 선물 구매한(보낸) 사람
	
	GACHA_ERROR				RecvGachaPurchaseReq( USER_NODE *pUser, PACKET_GACHA_SS_PURCHASE_REQ* pReq );
	void					SendGachaPurchaseAck( USER_NODE *pUser, GACHA_ERROR eErrorCode, DB_GACHA* pGacha );
	void					SendGachaShopState( bool bAllServer, INT32 i32ServerIdx = 0 );
	GACHA_ERROR				ProvideGachaItem( USER_NODE* pUser, DB_GACHA* pGacha );
	void					SendGachaWinningUser( GACHA_RESULT eGachaResult, TTCHAR* szUserName, UINT32 ui32LuckyGoodsID, INT32 i32ServerIdx = -1 );

	void					DirectUserPenalty( UINT32 ui32ServerIdx, INT32 i32SessionIdx, T_UID i64UID, INT8 i8PenaltyType, INT32 i32PenaltyAmount );

	void					SendPCCafeStatusAck( USER_NODE* pNode );

	void					_SendDeleteChara( USER_NODE *pNode, INT32 i32Rv, UINT8 ui8DeleteSlot ); 
	void					_CreateDefaultChara( USER_NODE *pNode );

	void					_SendCreateChara( USER_NODE *pNode, RING_CREATE_CHAR* pChara, S2MO_CHAR_SKILL_INFO* pSkill ); 
	void					_SyncChara(USER_NODE *pNode, INVEN_BUFFER* pInvenItem, INT64 &i64AddCharaSlot );

	void					_CheckCapsuleMatching();
	void					_InsertItemCountBuffer(T_UID uid, ITEM_INSERT_REQUEST_TYPE req, ST_ITEM * pItemArr, UINT8 ui8BufferCount, UINT8 ui8BufferMaxCnt, UINT32 ui32CapsuleItemID);
public:	

	CTaskProcessor();
	virtual ~CTaskProcessor();

	INT32					Create( UINT32 ui32DataSize );
	void					OnUpdate();

	BOOL					SendGamePacket(UINT32 idx,i3NetworkPacket* pPacket);
	BOOL					SendAllGamePacket(i3NetworkPacket* pPacket);
	
	virtual UINT32			OnRunning(void* pUserData);
	
	void					SendNewVersionAll( void);
	void					SendNewVersion( UINT32 ui32ServerIdx );
	
	void					SendFieldShop( UINT32 ui32ServerIdx );
	void					SendGoodsList( UINT32 ui32ServerIdx );
	void					UserInsertItem( USER_NODE* pUserNode, INVEN_BUFFER* pInvenItem, INT64 &i64CharaSync );
	UINT32					UserDeleteItem( USER_NODE* pUserNode, T_ItemDBIdx TItemWareIdx );
	void					_ItemAbility( USER_NODE* pUserNode, UINT32 ui32ItemID );			// 필요한지 확인합니다.
	
	void					UsedWeaponList_T(	 USER_NODE* pNode, UINT16 ui16Count, USED_ITEM* pUsedItemList );

	BOOL					CreateCapsule();
	BOOL					LoadCapsule( i3IniParser* pParser, CCapsuleItem* pCapsule );
	void					SetCapsule( USER_NODE* pUserNode, UINT32 ui32ItemID );
	void					SetCapsuleCount(USER_NODE* pUserNode, UINT32 ui32ItemID, TLoopCnt tCount);
	void					SetPointItem( USER_NODE* pUserNode, UINT32 ui32ItemID , GET_POINT_ROUTE eGetRoute );
	void					SetPointItemCount(USER_NODE* pUserNode, UINT32 ui32ItemID, GET_POINT_ROUTE eGetRoute, TLoopCnt tCount);

	void					AllUserKick();

	void					BlockUser( T_UID i64UID, const TTCHAR *pstrComment );


	///////////////////////////
	S2RingBuffer*			GetRingSIAPop()					{	return m_pRingSIAPop;	}

	STATS_INFO*				GetStatsInfo()					{	return &m_StatsInfo;	}

	BOOL					GetRunningFlag()				{	return m_bIsRunning;	}

	void					InsertEventItemOldUser( USER_NODE * pNode );	// 기존 유저 이벤트 아이템 지급
	void					InsertEventItemNewUser( USER_NODE * pNode );	// 신규 유저 이벤트 아이템 지급

	ASC_WEB_REQ_INFO *		GetASCWebReqInfoInsertPtr()		{ 	return (ASC_WEB_REQ_INFO *)m_pRingASCWeb->PushPointer();	}
	void					GetASCWebReqInfoInsertIdx()		{	m_pRingASCWeb->PushPointerIdx();		}
	INT32					GetASCWebReqInfoCount()			{	return m_pRingASCWeb->GetBufferCount();	}

	void					SetMedal(MEDAL* pRecvMedal, USER_NODE* pNode );	// 일반 메달 고유인덱스로 배열 인덱스 찾기. - 유저노드

	UINT32					GetUseCash()					{	return m_ui32UseCash;			};
	void					SetUseCash( UINT32 ui32Cash )	{	m_ui32UseCash = ui32Cash;		};
	void					AddUseCash( UINT32 ui32Cash )	{	m_ui32UseCash += ui32Cash;		};
	void					UserRepairItem( USER_NODE* pUserNode, S2MO_REPAIR_INFO *pstInfo );

	void					SetIPBlockOpenList( BOOL bUsed, INT32 i32IPCount, IPBLOCK_IP* pOpenIP, INT32 i32NationCount, UINT8* pui8OpenNation );

	INT32					GetActiveSessionCount()			{	return m_pSessionManager?m_pSessionManager->GetSessionCount():0;		};

	void					WriteGiftLog( SEND_GIFT *pGift, ENUM_TYPE_SHOP eType = TYPE_SHOP_GIFT_BUY );	// 주의 TASK 쓰레드에서만 호출해야함

	INT32					DeleteCharaDB(USER_NODE* pUserNode, T_ItemDBIdx TDBIdx);
	INT32					DeleteCharaDB(T_UID TUID, INT32 i32SlotIdx, T_ItemDBIdx TDBIdx);

	BOOL					IsUseGiftBoxContents(UINT8 ui8InserrType);

	void				SetCapsuleLoad(CAPSULE_LOAD_TYPE eType) { m_i32CapsuleLoadFlag |= (0x01 << eType); };
	void				ReSetCapsuleLoad(CAPSULE_LOAD_TYPE eType) { m_i32CapsuleLoadFlag &= ~(0x01 << eType); };

	void					SetGeneralRankUpStartHour(INT32 hour);

	void				CopyWorkProFile(WORK_PROFILE* pWorkProfile)
	{
		m_stWorkProfile.CopyTo(pWorkProfile);
	}
};

extern CTaskProcessor* g_pTaskProcessor;

#endif //_TASKPROCESSOR_H
