#ifndef __USERSESSION_H
#define __USERSESSION_H

#include "ggsrv30.h"
#include "AntiCpXSvr.h"
#include "Xtrap_S_Interface.h"
#include "GPK\\Include\\GPKitSvr.h"
//using namespace SGPK

enum	LOAD_START_TYPE
{
	LOAD_START_NONE = 0,			// 초기값
	LOAD_START_CREATE,				// 룸생성
	LOAD_START_ENTER,				// 방입장
	LOAD_START_GIVEUP_BATTLE_END,	// 기브업
	LOAD_START_BATTLE_END,			// 배틀종료

	LOAD_START						// 배틀시작
};

class CRoom; 

// 밑에는 삭제 될 겁니다.  -- 현우 일해라 --
#define SAVE_DATABASE_USER_TITLE	0x20
// 위에는 삭제 될 겁니다.

//이 메시지를 보내고 Disconnect 해야 합니다. 

#define CHECKVALID_LOGIN	if( m_bLogin )\
								{\
									_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_LOGIN, ProtocolID); \
									return FALSE; \
								}

#define CHECKVALID_DBINDEX	if( m_i64UID == SERVER_VALUE_UNKNOWN )\
								{\
									_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_REQUEST_LOGIN, ProtocolID); \
									return FALSE; \
								}

#define CHECKVALID_MYINFO	if( FALSE == m_bMyinfo )\
								{\
									_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_INFO, ProtocolID); \
									return FALSE; \
								}

#define CHECKVALID_CHANNEL	if( m_i32ChannelNum == SERVER_VALUE_UNKNOWN )\
								{\
									_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_REQUEST_CHANNEL, ProtocolID); \
									return FALSE; \
								}

#ifdef USE_GM_PAUSE
#define CHECKVALID_ROOM		if(m_i32RoomIdx == SERVER_VALUE_UNKNOWN || m_i32SlotIdx == SERVER_VALUE_UNKNOWN) \
								{\
									_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_REQUEST_ROOM, ProtocolID); \
									return FALSE; \
								}\
								if( GAME_TASK_BATTLE == m_i32MainTask && m_pRoom->IsGMPause()) \
								{\
									return FALSE; \
								}

#define CHECKVALID_ROOM_GM_PAUSE		if(m_i32RoomIdx == SERVER_VALUE_UNKNOWN || m_i32SlotIdx == SERVER_VALUE_UNKNOWN) \
								{\
									_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_REQUEST_ROOM, ProtocolID); \
									return FALSE; \
								}
#else

#define CHECKVALID_ROOM		if(m_i32RoomIdx == SERVER_VALUE_UNKNOWN || m_i32SlotIdx == SERVER_VALUE_UNKNOWN) \
								{\
									_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID); \
									return FALSE; \
								}
#endif
// 닉네임 생성 없이 구매를 하려고 하면 핵으로 처리한다.(이유 : 닉네임이 없으면 로그아웃시 저장이 안된다. 포인트차감되는 기능의 경우 치명적, 반드시 넣어야 함)
#define CHECKVALID_SHOP		if( '\0' == m_UserInfoBasic._nick[0] )\
{\
	_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID, KILL_REASON_HACK_PACKET); \
	return FALSE; \
}

//////////////////////////////////////////////////////////////////////////
struct INVEN_BUFFER_SET
{
	UINT8			ui8ChangeType;
	UINT32			ui32WeaponCount;
	UINT32			ui32CharaCount;
	UINT32			ui32ItemCount;
	INVEN_BUFFER	aWeaponBuffer[ MAX_INVEN_COUNT ];
	INVEN_BUFFER	aCharaBuffer[ MAX_INVEN_COUNT ];
	// 쿠폰까지 포함된 버퍼. 쿠폰과 인벤아이템을 함께 들고있지만 클라이언트로 인벤 리스트를 보낼땐 배열을 검사하여 쿠폰과 인벤아이템은 따로 전송합니다.
	INVEN_BUFFER	aItemBuffer[ MAX_INVEN_COUPON_COUNT ];

	INVEN_BUFFER_SET()
	{
		ui32WeaponCount = 0;
		ui32CharaCount	= 0;
		ui32ItemCount	= 0;
	}
};

enum USED_CHECK_TYPE
{
	USED_CHECK_NONE,
	USED_CHECK_WEAPON,
	USED_CHECK_CHARA,
};

#define MAKE_USED_CHECK_SLOTINFO( type, slot )	((type << 6)+(slot))
#define GET_USED_CHECK_TYPE( slotinfo )			((slotinfo & 0xC0) >> 6)
#define GET_USED_CHECK_SLOT( slotinfo )			(slotinfo & 0x3F)

struct ENDURANCE_USED_WEAPON
{
	bool		m_bUse;
	UINT8		m_ui8Type;
	TUID		m_TWareDBIdx;
	UINT32		m_ui32UsedTime;

	void UseStart( UINT8 ui8Type, TUID TWardDBIdx, UINT32 ui32UsedStartTime )
	{
		m_bUse				= true;
		m_ui8Type			= ui8Type;
		m_TWareDBIdx		= TWardDBIdx;
		m_ui32UsedTime		= ui32UsedStartTime;
	}

	void UseStop()
	{
		m_bUse = false;
	}
};

/*-----------------------------------------------------------------------
Name : CUserSession
2007 03 06 : 버그확인 (UserSessionProtocol은 제외) 버그없음 
2007 07 06 : 메니저를 통과할 필요없는 것들은 바로 유저가 호출하도록 변경 
-----------------------------------------------------------------------*/

class CUserSession : public i3NetworkSession
{
	I3_CLASS_DEFINE( CUserSession ); 
private:

protected :	

#ifdef USE_NPROTECT				// GAMEGUARD_NPROTECT
	CCSAuth3					m_pCsa;
#endif
#ifdef USE_AHNHS				// GAMEGUARD_HACKSHIELD
	AHNHS_CLIENT_HANDLE			m_hClient;
	AHNHS_TRANS_BUFFER			m_stRequestBuf;
	AHNHS_TRANS_BUFFER			m_stResuestBuf;
#endif
#ifdef USE_XTRAP				// GAMEGUARD_XTRAP
	char						m_pXtrapSessionBuf[ XTRAP_CS4_BUFSIZE_SESSION ];		// Session Buffer ( Send to Client )
#endif
#ifdef USE_GPK					// GAMEGUARD_GPK
	INT32						m_i32CodeIdx;
#endif

	BOOL						m_bHaveInfo; 
	INT32						m_i32MainTask;								// 위치
	INT32						m_i32SubTask;								// 서브위치
	
	bool						m_bIsRealIp;								// 리얼 아이피인지.

	UINT32						m_ui32EncriptKey;							//암호화 키 입니다.
	BOOL						m_bSendPacket;
	INT32						m_i32SendErrorCount;
	BOOL						m_bGMUser;									// GM 인지 확인

	BOOL						m_bMODUser;									// 터키의 경우 MOD 권한 추가

	BOOL						m_bEnterServer;								// 체널에 한번이라도 접속했는지를 체크

	//  패킷 정리 시작
	UINT32						m_ui32BirthDay;								// 생년 월일 

	bool						m_abGetInfoToServer[ GET_INFO_TO_SERVER_MAX ];	// 정보를 받았는지에 대한 Flag

public:

	// HeartBit2 받았다
	bool						m_bIsRecvHeartBit2;

	// 패킷 처리 테스트코드
	DWORD						m_dwParseTimeHigh;
	PROTOCOL					m_ui16ParseTimeHighProtocol;

	UINT32						m_ui32ConnectServerTime;					//서버 접속 시간.

	//초당 패킷 확인 
	UINT32						m_ui32ATPacketSecond; 
	UINT32						m_ui32PacketSecondCount; 

	// 패킷 정리 시작

	// ----------------------------------------------------------------------------------------
	// 게임서버 내부 사용
	UINT32						m_ui32LoginKey;								// 로그인 비밀키
	UINT32						m_ui32CreateData;							// 생성 날짜이긴 한데 태국때문에 있습니다. 수정 요망	-- 현우 일해라 --

	INT32						m_i32ChannelNum;							// 채널 번호
	INT32						m_i32LogChannelNum;							// (서버*10)+(m_i32ChannelNum+1)

	CRoom			*			m_pRoom;									// 방 포인터 // 직접 호출 하는것으로 변경 20070706	//이값에는 절대로 NULL를 넣으면 않됨
	INT32						m_i32RoomIdx;								// 방 번호
	INT32						m_i32SlotIdx;								// 방에서 슬롯 번호	

	UINT16						m_ui16UsedItemCount;						// 한 게임에서 사용했던 아이템 개수
	USED_ITEM_LIST				m_aUsedItemList[ MAX_INVEN_COUNT ];			// Item list in a game match
	UINT8						m_aui8WeaponUseCount[ WEAPON_USAGE_COUNT ];	// 한 게임에서 사용했던 Type 별 무기 개수

	UINT64						m_ui64MacAddress; 
	UINT8						m_ui8LocaleCode;

	UINT8						m_ui8OtherCountry;

#if defined( USE_INDONESIA_PCBANG )
	char						m_strNesiaPCbangKey[MAX_INDONESIA_PCBANG_KEY];
	UINT8						m_ui8NesiaPCbangHWInfo[DATASIZE_NO_CHECK];
	DWORD						m_dwNesiaPCbangHWKey;
#endif

	// ----------------------------------------------------------------------------------------
	// 클라이언트와 공동 사용

	TUID						m_i64UID;									// DataBase idx	
	char						m_strId[NET_ID_SIZE];						// 아이디				-- 현우 일해라 --

	BOOL						m_bLogin;
	BOOL						m_bMyinfo;									// Login 정보 받았는지에 대한 체크

	char						m_strWebKey[NET_SESSION_KEY_SIZE + 1];		// 웹 세션 키

	UINT8						m_ui8Gender;								// 성별
	UINT8						m_ui8Age;									// 나이

	UINT8						m_ui8ChangeDBFlage;							// 유저 정보 저장 유무 	

	// 내 정보 
	USER_INFO_BASIC				m_UserInfoBasic;							// User 기본 정보
	USER_INFO_RECORD			m_UserInfoRecord;							// User 전적 정보 
	USER_INFO_EQUIPMENT_WEAPON	m_UserInfoEquipmentWeapon;					// User 장비 정보 Weapon 
	USER_INFO_EQUIPMENT_CHARA	m_UserInfoEquipmentChara;					// User 장비 정보 Chara
	USER_INFO_INVITEM_DATA		m_UserInfoInvItemData;						// User 아이템 실제 정보
	
	USER_INFO_DAILY				m_UserInfoDaily;							// User 당일 전적 정보

	// Inven 정보
	INT32						m_i32InvCharaCount;
	INT32						m_i32InvWeaponCount;
	INT32						m_i32InvItemCount;
	INVEN_BUFFER				m_aInvChara[MAX_INVEN_COUNT];				// Inventory Char
	INVEN_BUFFER				m_aInvWeapon[MAX_INVEN_COUNT];				// Inventory Weapon
	
	INVEN_BUFFER				m_aInvItem[MAX_INVEN_COUPON_COUNT];			// Inventory Item

	// 메달정보
	UINT32						m_ui32Miniature;							//약장
	UINT32						m_ui32Insignia;								//휘장
	UINT32						m_ui32Order;								//훈장
	UINT32						m_ui32Master;								//마스터
	
	// 퀘스트
	UINT32						m_ui32QuestEventDate;						// 이벤트 임무카드 날짜
	UINT8						m_aaui8QuestCtxActive[MAX_CARDSET_PER_USER][MAX_QUEST_PER_CARDSET_NEW];	// 임무카드 진행 횟수 ( 3/4 ) 이런식으로 몇개까지 진행 했는지를 나타냅니다.
	QUESTING_INFO				m_QuestIngInfo;															// 임무카드 총 진행 상태
	UINT8						m_aui8QuestCtxActiveNow[MAX_QUEST_PER_CARD_NEW];						// 현재 임무카드 진행 횟수 한 Room 에서는 하나의 임무카드 셋만 깰수 있기 때문에 4개만 가지고 있습니다.
	UINT16						m_ui16ActiveIndexCardNow;												// 현재 Room 내에서 임무카드 완료 여부


	// 호칭
	UINT64						m_ui64GainUserTitle;						// 획득한 호칭 (Bit 단위입니다)
	UINT8						m_aui8EquipUserTitle[MAX_EQUIP_USER_TITLE];	// 장비된 호칭 Id
	INT32						m_i32LimitUserTitleCount;					// 사용가능 호칭 장비 슬롯 갯수
	
	UINT32						m_aui32UseWeaponList[ WEAPON_USAGE_COUNT ];		// 배틀에서 사용하는 무기 리스트 : 초창기 시스템 때문에 생긴 오류..차차후 수정..
	ENDURANCE_USED_WEAPON		m_aEnduranceUsedWeapon[ WEAPON_USAGE_COUNT ];
	UINT8						m_aui8UseWeaponEndurance[ WEAPON_USAGE_COUNT ];	// 무기 내구도 수치 ( Pri, Sec...)

	USER_INFO_EQUIPMENT_CHARA	m_UseCharaList;								// 배틀에서 사용하는 캐릭터 리스트 (배틀중 캐릭터 교체하여 m_UserInfoEquipmentChara와 달라질 수 있음)
	ENDURANCE_USED_WEAPON		m_aEnduranceUsedChara[ EQMIPEMENT_SLOT_COUNT - (WEAPON_USAGE_COUNT-1) ];	// 무기는 장비슬롯 대신 무기 사용타입으로 따로 기록하고 있어...
	UINT8						m_aui8CharaUseCount[ EQMIPEMENT_SLOT_COUNT - (WEAPON_USAGE_COUNT-1) ];	// 한 게임에서 사용했던 슬롯별 캐릭터 개수

	// Gip Temp 변수
	INT32						m_i32GIP_LastUseGender; 
	INT32						m_i32GIP_LastUseTeam;

	//LogD Temp 변수
	INT32						m_i32LogD_RespawnCount;						// 로그디용 리스폰 카운트
	BOOL						m_bForceRoomLeave;							// 강퇴당한 유저 입니다.
	
	//LogD에서 필요함	
	UINT32						m_ui32AccConnectTime;						// 누적 접속시간입니다.
	UINT32						m_ui32AccConnectCount;						// 접속 카운트 입니다. 

	// 게임가드 연동
	UINT32						m_ui32GameGuardSendST;						// 게임가드 인증 타임
	UINT32						m_ui32GameGuardRecvST;						// 게임가드 인증 타임
	UINT32						m_ui32ATLastRecv;							// 마지막 받은 타임 (서버 타임)

	UINT32						m_ui32GameGuardCheckCount;

	BOOL						m_bHSCheck;									// 핵실드 서버 연동 진행 여부 ( 태국 가레나에서만 확인하여 동작 ) - 서동권
	
	UINT64						m_ui64LoginTime;							// 로그아웃시 ZLog에 남길 현재 로그인 시간
	UINT32						m_ui32LogInST;								// 로그인 요청한 타임
	UINT32						m_ui32ATSendKill;							// 서버에서 죽인 패킷 전송 시간 ( 해킹일 경우에만 ) - 서동권
	UINT32						m_ui32HackKillReason;						// 서버에서 죽일 이유 ( 해킹일 경우에만 ) - 서동권
	char						m_strHackNotice[ DEF_SIZE_HACK_NOTICE ];	// 서버에서 죽인 이유 (String )
	UINT32						m_ui32ATIsKill;								// 서버에서 죽일 시간
	UINT32						m_ui32KillReason;							// 서버에서 죽일 이유
	BOOL						m_bHackKill;								// 서버에서 처리 여부 ( 해킹일 경우에만 ) - 서동권
	UINT32						m_ui32LastLoginTime;						// 로그인시 ZLog 에 남길 마지막 로그인 시간( 지금 로그인 하기 전 )

	BOOL						m_bGetEventItem;							// 이벤트 아이템 지급 여부

	NET_UDP_INFO				m_UdpNetInfo;								// Udp 정보 

	// 진급 포인트 시스템
	BOOL						m_bIsRankUp;								// 랭크 업 했는지
	UINT32						m_ui32rankUpPoint;							// 지급 받은 진급 포인트

	// Lobby용 방 리스트
	UINT32						m_ui32UpdateRoomListPage;					// 예전과 다른방식으로 사용
	BOOL						m_bRequestRoomList; 
	BOOL						m_bRequestUserList; 
	UINT32						m_ui32UpdateUserListPage;					//예전과 다른방식으로 사용

	// 친구
	UINT8						m_ui8FriendCount; 

	// 차단 유저
	UINT8						m_ui8BlockCount; 

	// 매치방에 있는지 없는지를 체크
	BOOL						m_bMatchRoom;

	UINT8						m_ui8Ping;

	PROTOCOL					m_ui16BackProtocolID;						// 로그 추적용 Back Protocol 입니다. 추후에 삭제할 예정

	UINT32						m_ui32BattleCheckTime; 

	BOOL						m_bIsBattleComplete;

	// 로그인 시간 체크
	UINT32						m_ui32LoginST;								// 로그인 Req 받은 시간
	UINT32						m_ui32ATInfo;								// 로그인 Req 받은 시간

	INT32						m_i32AlarmHourCount;						// 피로도 경고를 위한 시간단위 누적 카운터
	INT32						m_i32AlarmMinuteCount;						// 피로도 경고를 위한 분단위 누적 카운터

	UINT8						m_ui8LoadingStart;							// 로딩 시작 패킷을 보냈는지 확인 체크를 위한 변수
	
	INT16						m_i16PacketRandSeed;						// 패킷 랜덤 시드값

	// 어뷰징 체크
	UINT32						m_ui32HoldAccExp;							// 어뷰징 유저 보류 EXP
	UINT32						m_ui32HoldAccPoint;							// 어뷰징 유저 보류 POINT

	UINT8						m_ui8AbusingLevel;							// 어뷰징 레벨
	UINT32						m_ui32AbusingPopupTime;						// 어뷰징 팝업 시간

	UINT32						m_ui32BattleStartTime;						// 배틀시작시간. 어뷰징 체크에 사용
	UINT32						m_ui32BattleStartInvenTime;					// 배틀시작시 인벤시간. 배틀시작시간과 같이 기록되나, 표현방식이 달라 따로 저장.

	UINT32						m_ui32BattleDeathTime;						// 배틀 중 죽은 시간. 리스폰 해킹 방지에 사용
	UINT32						m_ui32RespawnDelay;							// 리스폰 딜레이 시간 (리스폰 단축이 적용된 딜레이 시간)

	UINT32						m_ui32GrenadeMaxCount;						// 수류탄 던질수 있는 최대 개수
	UINT32						m_ui32GrenadeCount;							// 수류탄 던지고 있는 개수
 
	UINT16						m_ui16PrevExpRank;							// 이전 방에서 얻은 경험치
	// ZLog 로그를 위하여 추가합니다 - 서동권 2011.07.15
	UINT32						m_ui32RoomBattleCount;						// 한 방에서 몇판을 했는지
	UINT32						m_ui32PlayTime;								// 이전 방 플레이 시간
	//UINT32						m_ui32TotalBattleTime;					// 현재 접속 후 총 플레이 시간
	UINT32						m_ui32LoginBattleTime;						// 로그인 후부터의 배틀 타임

	UINT32						m_ui32AccActualBattleTime;					// 현재 진행중인 방에서 실제 배틀에 소비한 시간
	//UINT32						m_ui32ActualBattleTime;					// 계정 생성후 누적된 실제 배틀에 소비한 시간

	UINT32						m_ui32RespawnTime;							// 리스폰 시간

	// 뽑기 상점
	UINT32						m_ui32RSLastBaseInfoTime;					// 뽑기 상점 기본 정보 최근 업데이트 시간. ( YYMMDDHHmm )
	INT16						m_i16RSTab1UseCount;						// 뽑기 상점 1번탭 이용 횟수. 테스트 코드. 추후 Trans서버로 이동.
	INT16						m_i16RSTab2UseCount;						// 뽑기 상점 1번탭 이용 횟수. 테스트 코드. 추후 Trans서버로 이동.
	INT16						m_i16RSTab3UseCount;						// 뽑기 상점 1번탭 이용 횟수. 테스트 코드. 추후 Trans서버로 이동.
	
	UINT32						m_ui32FirstObserverTime;					// 난입 후 첫 관전시간
	BOOL						m_bFirstObserver;							// 난입 후 첫 관전인지
#if defined( USE_QUICK_JOIN )
	// 빠른 입장 설정 정보 - 서동권
	QUICK_JOIN_INFO		m_stQuickInfo;
#endif
	// 중동 UAE 이벤트
	UINT8						m_ui8EventCountry;
	// 신고하기 - 서동권
	UINT8						m_ui8ReportCount;							// 신고하기 - 스크린샷 1일 이용 횟수

	// 핵 사용 방지 처리를 위해 - 서동권
	UINT8						m_ui8Hack_Count;							// 자기 위치를 숨기고 이상한데서 계속 죽이는 핵 때문에 임시 방편 추가 - 서동권
	REAL32						m_r32Hack_Position_X;						// 자기 위치를 숨기고 이상한데서 계속 죽이는 핵 때문에 임시 방편 추가 - 서동권
	REAL32						m_r32Hack_Position_Y;						// 자기 위치를 숨기고 이상한데서 계속 죽이는 핵 때문에 임시 방편 추가 - 서동권
	REAL32						m_r32Hack_Position_Z;						// 자기 위치를 숨기고 이상한데서 계속 죽이는 핵 때문에 임시 방편 추가 - 서동권

	// 데드케이션 서버일 경우 자석핵을 유저마다 확인 가능하여 추가하였습니다.
	UINT8						m_ui8MHack_Count;							// 자석핵 때문에 임시 방편 추가 - 서동권
	REAL32						m_r32MHack_Position_X;						// 자석핵 때문에 임시 방편 추가 - 서동권
	REAL32						m_r32MHack_Position_Y;						// 자석핵 때문에 임시 방편 추가 - 서동권
	REAL32						m_r32MHack_Position_Z;						// 자석핵 때문에 임시 방편 추가 - 서동권

	BOOL						m_bCheckLogout;								// 정상적으로 로그아웃 되었는지 판별.

	// 중복채팅 방지 - 김용수
	UINT32						m_ui32ChatTime;								// 채팅REQ를 받은 시간
	UINT8						m_ui8ChatCount;								// 1초 안에 중복입력을 시도한 횟수
	INT32						m_i32LimitTime;								// 적용된 제한 시간
	UINT32						m_ui32LimitStartTime;						// 제한 적용 시작 시간

	UINT8						m_ui8LoginLinkThreadIdx;					// Trans 서버의 Link Thread Idx
	UINT32						m_ui32LoginWaitCount;						// 나의 로그인 순번
	UINT32						m_ui32LoginWaitSendTime;					// 로그인 패킷의 Send Time

	BOOL						m_bCompleteTS;
	UINT32						m_aui32TSGoodsID[2];

	TWareDxIdx					m_i64ChangeClanMarkDBIdx;

	UINT32						m_ui32ATCanRespawnPacket;					// Pause중에 온 리스폰요청은 버려지지만, Pause중이라도 '이값 < 현재절대시간 < 이값 + 3초' 일 경우에는 수신받는다.

	bool						m_bHasSID;									// 2차 계정 소유여부

	//////////////////////////////////////////////////////////////////////////
protected:
	
	bool						_PacketDecript( i3NetworkPacket* pPacket );
	
	void						_SetChannelBonusPacket( UINT8 * pui8ChannelType , i3NetworkPacket * pPacket );

	void						_SendShopInfo(UINT32 ui32ShopVersion);													// Shop 정보 보내기
	void						_SendFieldShopInfo(UINT32 ui32FieldShopVersion);

	void						_SendEquipment( void );																	// 장비 패킷 보내기

	UINT8						_GetMyInfoPacket( INT32 i32ThreadIdx, i3NetworkPacket* pPacket, BOOL bClan );			// Trans 서버로 부터 받는 나의 정보에 대한 패킷을 파싱합니다.

	void						_SetUserRecord();																		// 유저의 전적을 세팅해준다.
	void						_SetCalcLimitTitleCount();																// 호칭장착 가능 슬롯을 계산합니다.
	void						_SetAge();																				// 나이를 설정합니다.
	void						_SetGMUser();																			// 닉네임을 이용해 GM 인지 확인합니다.
	
	BOOL						_ExceptionInsert( T_ItemID ui32ItemID, UINT8 ui8ItemType = ITEM_ATTR_TYPE_BASIC );	// 인벤에 무기 밀어넣기
	BOOL						_ExceptionWeaponInsert( UINT32 ui32ItemID, UINT8 ui8ItemType = ITEM_ATTR_TYPE_BASIC );	// 인벤에 무기 밀어넣기
	BOOL						_ExceptionCharaInsert( UINT32 ui32ItemID, UINT8 ui8ItemType = ITEM_ATTR_TYPE_BASIC );	// 인벤에 케릭터 밀어넣기
	BOOL						_ExceptionItemInsert( UINT32 ui32ItemID, UINT8 ui8ItemType = ITEM_ATTR_TYPE_BASIC );	// 인벤에 아이템 밀어넣기
	void						_ExceptionInsert( INVEN_BUFFER* pBuffer, UINT32 ui32ItemID, UINT8 ui8ItemType = ITEM_ATTR_TYPE_BASIC );			// 인벤에 밀어넣기
	
	INT32						_SelectChannel( UINT32 ui32Channel );													// 입장 가능한 체널인지 체크합니다.
	INT32						_CreateRoom( ROOM_INFO_CREATE* pInfoCreate );											// 방을 만듭니다.

	INT32						_BuyGoods( UINT32 ui32GoodsID, UINT8 ui8BuyType, UINT32 ui32CouponID, INT64 i64CouponIdx );		// 물건을 구입합니다.
	INT32						_BuyGoodsBasket( BUY_BASKET *stBasket, UINT8 ui8GoodsCount );							// 장바구니로 물건을 구입합니다.
	BOOL						_CheckBuyUserTitle( UINT32 ui32GoodsID );												// 무기일 경우 호칭이 있는지를 체크합니다.
	
	BOOL						_CheckWeaponEquip(BOOL bUsedItem, UINT8 ui8WeaponFlag, UINT32* aui32WeaponList, UINT32 ui32InvenTime);	// 장착된 무기를 체크하고 내구도일경우 사용처리해준다.
	INT32						_CheckCharEquip( BOOL bUsedItem, UINT32 ui32Team, USER_INFO_EQUIPMENT_CHARA* pChara, UINT32 ui32InvenTime );		// 장착된 케릭터를 체크하고 내구도일경우 사용처리해준다.
	void						_SetUsedItem( TUID i64ItemWareDBIdx, UINT32 ui32ItemID, UINT8 ui8Type, UINT32 ui32UsedDurablity, UINT8 ui8CheckSlotInfo );				// 아이템 사용처리를 해준다.

	INT32						_CheckEquipment(void);																	// 장착된 아이템과 보유중인 무기를 검사하여 값이 이상할 경우 초기값으로 리셋해 줍니다.
	BOOL						_CheckEquipmentChara( USER_INFO_EQUIPMENT_CHARA * pEquipment, UINT32* pui32Check );		// 보유중인 케릭터를 검사합니다.
	BOOL						_CheckEquipmentWeapon( USER_INFO_EQUIPMENT_WEAPON * pEquipment, UINT32* pui32Check );	// 보유중인 무기를 검사합니다.

	BOOL						_CheckItemAuth( INVEN_BUFFER* pItem, BOOL bEnduranceItemCheckPass = TRUE );
	BOOL						_CheckItemEquipment( UINT32 ui32Item, EQMIPEMENT_SLOT eSlot );

	// 호칭
	BOOL						_ChangeUserTitle( UINT8 userTitleId, LOG_GET_TITLE* pstLogGetTitle );					// 호칭을 획득합니다.
	
	float						_GetRepairPremium( UINT32 ui32Endurance );
	
	// Quest
	INT32						_LastQuestItem( QUEST_ITEM_INFO_NEW* pItemInfo, QUEST_ITEM_INFO_NEW* pLastItemInfo, BOOL bLastGet );
	
	void						_DeleteQuestCardSet( UINT8 ui8CardSet );					// 임무카드를 삭제합니다.
	void						_SetQuestEvent();											// 이벤트 임무카드를 설정합니다.

	// 아이템 인증 
	INT32						_SetItemAbilityAuth( INVEN_BUFFER* pBuffer );				// 아이템 인증 게임서버단 처리부분
	INT32						_SetItemAbilityAuth( INVEN_BUFFER* pBuffer, void* pData );	// 아이템 인증 게임서버단 처리부분 능력 소유

	void						_ResetRecord_MAU( void);									//전적 초기화	
	void						_ResetDisconnect_MAU( void );								//이탈률 초기화 
	void						_ResetKillDeath_MAU( void );								//킬데스 초기화	
	
	// UserSessionException.cpp 
	void						_ExceptionWeapon( INT32 i32ThreadIdx );						// 서버 인벤 지급 - 총기
	void						_ExceptionCharacter( INT32 i32ThreadIdx );					// 서버 인벤 지급 - 케릭터
	void						_ExceptionSpecial( INT32 i32ThreadIdx );					// 서버 인벤 지급 - Special
	void						_ExceptionTitleItem( INT32 i32ThreadIdx, UINT8 ui8userTitleId );	// 호칭획득에 따른 무기 지급입니다.

	//Find Quest Slot
	INT8						_FindInsertQuestSlot(UINT8 ui8CardType);							// 0, 1, 2, 3 

	BOOL						_CheckClanName( char* pClanName );

	BOOL						_CheckIsSameClan( INT32 i32SessionIdx );												// 해당 유저와 동일한 클랜인지 체크.

	void						_EquipmentInBattle( UINT32 ui32ItemID );

	BOOL	CheckSaleCouponDate( INT64 i64CouponIdx, SALE_COUPON * pCoupon );											// 할인쿠폰의 유효기간을 확인합니다.
	BOOL	CheckUseCoupon( SALE_COUPON *pCoupon, SHOP_GOODS_BASE *pGoods, UINT32 &ui32Cash );							// 할인쿠폰을 사용 가능한지 확인합니다.

	// 도배방지 방지 시스템
	UINT32						_CheckDuplicateChat(char* pstrChat, INT32 i32ChatType, PROTOCOL ui16Protocol);			// 중복채팅인지 아닌지 체크

	// 2차계정 생성기능의 ID/PW검사
	BOOL						_CheckSID_ID( char* strSecurityID, UINT8 ui8IDSize );
	BOOL						_CheckSID_Password( UINT8 ui8PWSize )
	{
		if ( ui8PWSize < 8 + 1 )	return FALSE;
		if ( ui8PWSize > 16 + 1)	return FALSE;

		return TRUE;
	}

public:
	void						_SendErrorMessage(INT32 i32WorkIdx, UINT32 ui32ErrorCode, PROTOCOL ProtocolID=0, INT32 i32KillReason=KILL_REASON_HACK_PACKET, char* pstrMessage=NULL );		// 에러 패킷 보내기

	void						CheckQuestItem2();
	void						_SendQuestInfoPacket( INT8 i8CardSolt );												// 임무카드를 보냅니다.

	char *						GetNickName(void)		{ return m_UserInfoBasic._nick; }
	UINT8						GetNickColor()			{ return m_UserInfoBasic._nickcolor;	};
	char *						GetUserId(void)			{ return m_strId; }
	INT32						GetChannelNumber(void)	{ return m_i32ChannelNum; }										//이거 차후 삭제 
	UINT32						GetClanIdx(void)		{ return m_UserInfoBasic._clanidx; }	
	TUID						GetUID(void)			{ return m_i64UID; }	

	GAME_TASK_SUB				GetSubTask()			{	return (GAME_TASK_SUB)m_i32SubTask; };						// 서브 위치 반환	
	USER_INFO_BASIC*			GetUserInfoBasic()		{	return &m_UserInfoBasic;	};
	UINT8*						GetEquipUserTitle()		{ return m_aui8EquipUserTitle;	};
	INT32						GetCheckCashitem( UINT32 ui32ItemNum );													// 캐쉬 아이템을 가지고 있는지 체크
	INVEN_BUFFER*				GetItemByItemID( UINT32 ui32ItemID );
	INVEN_BUFFER*				GetItemByWareDBIdx( TWareDxIdx i64WareDBIdx );											// 유저의 인벤에서 WareDBIdx 로 아이템을 검색한다.

	INT32						GetSessionIndex( void )		{ return m_SessionIdx; }
	INT32						GetPosition(void)			{ return m_i32MainTask; }

	void						CheckInventory_MAU();

	void						ChangeDataBaseInfo(UINT8 ui8ChangeType)		{ m_ui8ChangeDBFlage |= ui8ChangeType; }	// DB에 저장될 것들의 플래그를 담습니다.
	void						CheckEquipment();																		// 장착된 무기를 검사합니다.	내부에서 _CheckEquipment 를 호출한 후 변경된 사항이 있으면 클라이언트에 패킷을 보냅니다.

	void						SetPosition(INT32 i32Position);

	BOOL						CheckGameGuard_M( UINT32 ui32GameGuardTimeOut, UINT32 ui32GameGuardSendTime );
	void						SendGameGuardPacket_MU( INT32 i32WorkThread );											//게임 가드 패킷 보내기 

	void						SendChangeInven( INVEN_BUFFER_SET* pInvenSet );
	
	void						CheckRankUp( INT32 i32WorkThread );
 
	void						CheckGeneralRankUp( INT32 i32WorkThread );
	void						GeneralRankUp( INT32 i32WorkThread );
 
	void						CheckEndurance();																		// 내구도 사용 여부
	void						CharUseAllStop();																		// 모든 장비 사용 정지

	void						KickUser_Module(TUID i64UID, UINT8 Reason);	
	void						SetChangeNick( INT32 i32WorkThreadIdx, char* pstrOrgNick, char* pstrNewNick );
	void						SetChangeRank( INT32 i32WorkThreadIdx, UINT32 &ui32OldRank, UINT32 &ui32NewRank );
	void						SetRankUp( INT32 i32WorkThreadIdx);
	void						SetChangeColorNick( INT32 i32WorkThreadIdx, UINT8 ui8Color );	
	void						InvItemData();

	void						LogIn_MAU( INT32 i32WorkThread, INT32 i32Rv, TUID i64UID, char* pstrId, char* pstrCustom, char* pstrUID = NULL, UINT8 ui8LinkThreadIdx = 0, UINT32 ui32LoginDelayTime = 0, UINT8 ui8Popup = LOGIN_AFTER_POPUP_NONE );
	void						GetMyInfo_A( INT32 i32WorkThread, INT32 i32Rv, i3NetworkPacket* pPacket );
	void						SendMyInfo( INT32 i32WorkThread, INT32 i32Result );										// 나의 정보를 보냅니다.
	void						ServerEnter_A( INT32 i32Rv, i3NetworkPacket* pPacket );
	void						ServerLeave_A( INT32 i32Rv );
	void						CreateNickName_A( INT32 i32Rv, char* pstrNickName );

	void						SetClan_C( INT32 i32Result, UINT32 ui32CID, UINT8 ui8ClanState, UINT8 ui8ExpRank, UINT8 ui8PerRank, UINT32 ui32Mark, char* pstrClanName );

	bool						IsRealIp(void)					{	return m_bIsRealIp; }
	BOOL						IsGMUser()						{	return m_bGMUser;	}
	BOOL						IsMODUser()						{	return m_bMODUser;	}

	void						_GetPCCafeBonusValue( UINT8 ui8PCCafe, REAL32* pr32Exp, REAL32* pr32Point );

	void						SetBirthDay_A( INT32 i32Rv, UINT32 ui32BirthDay );

	void						SendAttendanceItemResult( INT32 i32Rv ); // 출석체크
	void						SendAttendanceCheckResult( INT32 i32Rv, ATTENDANCE_USER* pstAttendance );				// 출석체크
	void						SendAttendanceInfoResult( ATTENDANCE_USER* pstAttendance, ATTENDANCE_INFO* pstInfo );	// 출석체크

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//친구 패킷
	void		SendFriendList( UINT8 ui8FriendCount, FRIEND_INFO_STRUCT* pFriendInfo );								//친구 리스트 보내기 
	void		SendFriendAcceptError(INT32	i32ErrorCode);		//친구등록 에러 패킷 보내기 
	void		SendFriendInsertError(INT32 i32ErrorCode);		//친구추가 에러 패킷 보내기 
	void		SendFriendDeleteError(INT32 i32ErrorCode);		//친구삭제 에러 패킷 보내기 
	void		SetFriendChangeInfo_A(UINT8 ui8FriendIdx, UINT8 ui8State, UINT8 ui8NickNameSize, char  * pstrNickName, FRIEND_INFO * pInfo);	//친구 변경된 정보 보내기 
	void		SendFriendInvitedError(INT32 i32ErrorCode);
	void		SendFriendInvited(UINT8 ui8FriendIdx);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//차단 패킷
	void		SendBlockList( UINT8 ui8BlockCount, BLOCK_INFO_STRUCT* pBlockInfo );									//차단 리스트 보내기 
	void		SendBlockInsertResult(INT32 i32Result,  INT32 i32Idx = 0, UINT8 ui8Rank = 0, UINT8 ui8NickSize = 0, char *pstrNick = 0, UINT8 ui8CommentSize = 0, char *strComment = 0); //차단추가 결과 패킷 보내기 
	void		SendBlockDeleteResult(INT32 i32Result, INT32 i32Idx ); //차단삭제 결과 패킷 보내기
	
	void		OnInsertItem( INVEN_BUFFER* pInvenItem, INVEN_BUFFER_SET* pInvenSet );
	UINT32		OnDeleteItem( TUID i64ItemWareIdx, INVEN_BUFFER_SET* pInvenSet );
	void		SendNewEventCardSet(void);

	void		SendWhisper(char* pstrsenderNick, char* pstrmessage, UINT16 ui16messageSize);							// 귓속말 받기
	void		SendWhisperError(INT32 i32result, char* pstrreceiverNick);												// 귓속말 보내기 실패
	void		SendFindUser(INT32 i32Result, NEW_FIND_USER_INFO* pNewFindUserInfo = NULL );							// New 유저 찾기.

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//클랜 패킷 
	void		SendClanDetailInfo(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo);									//디테일 정보 보내기 
	void		SendClanMemberInfo( UINT8 ui8TotalCount, CLANMEMBER_INFO_STRUCT* pClanMember );							// 멤버 목록 요청
	void		SendClanMemberInfoChange(  TUID i64UID, UINT64 ui64State );												// 멤버 상태 변경
	void		SendClanMemberInfoInsert( CLANMEMBER_INFO_STRUCT* pClanMember );										// 멤버 추가
	void		SendClanMemberInfoDelete( TUID i64UID );																// 멤버 삭제
	void		SendClanCreate_C(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo);										//생성	
	void		SendClanJoinRequest(INT32 i32Result, INT32 i32ClanIdx);													//조인 
	void		SendClanCancleRequest(INT32 i32Result);																	//캔슬	
	void		SendClanMemberContext(INT32 i32Result, CLAN_MEMBER_CONTEXT * pMemberContext);							//멤버 
	void		SendClanMemberList(INT32 i32Result, UINT8 section, UINT8 count, CLAN_MEMBER_BASIC_INFO * pMemberlist);	//멤버 리스트 
	void		SendClanMemberDetailInfo(INT32 i32Result, USER_INFO_RECORD* pRecord );									//멤버 상세정보
	void		SendClanRequestContext(INT32 i32Result, CLAN_REQUEST_CONTEXT * pMemberContext);							//요청 멤버 
	void		SendClanRequestList(INT32 i32Result, UINT8 section, UINT8 count, CLAN_REQUEST_BASIC_INFO * pMemberlist);//요청 멤버 리스트 
	void		SendClanRequestDetailInfo(INT32 i32Result, CLAN_REQUEST_DETAIL_INFO * pClanRequestDetailInfo);			//요청 멤버 정보 디테일
	void		SendClanAcceptRequest(INT32 i32Result);																	//가입신청서 수락
	void		SendClanDenialRequest(INT32 i32Result);																	//가입신청서 거절
	void		SendClanChatingRequest( UINT8 ui8NickSize, char* pstrNick, UINT8 ui8ChatSize, char* pstrChating );		// 클랜 체팅
	void		SendClanPageChatingRequest( UINT8 ui8NickSize, char* pstrNick, UINT8 ui8ChatSize, char* pstrChating );
	void		SendClanSecessionRequest(INT32 i32Result);																//클랜 탈퇴
	void		SendClanCloseRequest(INT32 i32Result);																	//클랜 헤체
	void		SendClanCommissionMasterRequest( INT32 i32Result);														//클랜장 위임
	void		SendClanCommissionStaffRequest( INT32 i32Result);														//클랜장 위임
	void		SendClanCommissionRegularRequest( INT32 i32Result);														//클랜장 위임
	void		SendClanDeportationRequest( INT32 i32Result);	
	void		SendClanReplaceNoticeRequest( INT32 i32Result );
	void		SendClanReplaceIntroRequest( INT32 i32Result );
	void		SendClanNoteRequest( INT32 i32Result );
	void		SendClanReplaceNameRequest( INT32 i32Result );
	void		SendClanCheckMarkRequest( INT32 i32Result );
	void		SendClanChangeExpRankRequest( UINT8 ui8Rank );
	void		SendClanChangePerRankRequest( UINT8 ui8Rank );
	void		SendClanReplaceMarkResltRequest( UINT32 ui32Mark );
	void		SendClanReplaceManagementResltRequest( UINT8 ui8Authority );
	void		SendClanReplaceNameResultRequest( char* pstrName );
	void		SendClanReplaceMarkRequest( INT32 i32Result );
	void		SendClanReplacePersonMaxRequest( UINT8 ui8ClanMaxCount );
	void		SendClanReplaceManagementRequest( INT32 i32Result );	
	void		SendClanCheckJoinAuthorityRequest( INT32 i32Result );
	void		SendClanRecordResetRequest( INT32 i32Result );
	void		SendClanAcceptRequestError( INT32 i32Result );															// 가입 신청 결과 에러	
	void		SendClanAcceptResultRequest( CLAN_DETAIL_INFO* pDetailInfo );											//가입신청 수락 결과
	void		SendClanDenialResultRequest( );																			//가입신청 거절 결과
	void		SendClanDeportationResultRequest( );																	//강제추방 결과
	void		SendClanMasterCommissionResultRequest( );																//강제추방 결과
	void		SendClanStaffCommissionResultRequest( );
	void		SendClanRegularCommissionResultRequest( );
	void		SendClanRoomInvitedRequest( INT32 i32Result );
	void		SendClanRoomInvitedResultRequest( TUID TSenderUID );
	void		SendClanReplaceMemberBasicResultRequest( CLAN_MEMBER_BASIC_INFO* pMemberBasicInfo );	
	void		SendClanRecordResetResultRequest( );
	void		SendClanPointResetResultRequest();
	void		SendClanReplacColorNameResultRequest( UINT8 ui8Color );

	void		SendClanMatchTeamContextRequest( CLAN_BATTLE_TEAM_CONTEXT* pTeamContext );
	void		SendClanMatchAllTeamContextRequest( CLAN_BATTLE_ALL_TEAM_CONTEXT* pAllTeamContext );
	void		SendClanMatchTeamListRequest( UINT8 ui8TotalCount, UINT8 ui8SectionCount, UINT8 ui8Section, UINT8 ui8Count, CLAN_BATTLE_TEAM_LIST* pBattleTeamList );
	void		SendClanMatchAllTeamListRequest( UINT16 ui16TotalCount, UINT16 ui16SectionCount, UINT16 ui16Section, UINT8 ui8Count, CLAN_BATTLE_TEAM_INFO* pBattleTeamInfo );
	void		SendClanMatchTeamDetailRequest( INT32 i32Result, CLAN_DETAIL_INFO* pDetail );
	void		SendClanMatchTeamCreateRequest( INT32 i32Success, UINT32 ui32Idx, CLAN_BATTLE_TEAM* pTeam );
	void		SendClanMatchTeamJoinRequest( INT32 i32Success, CLAN_BATTLE_TEAM_INFO* pTeamInfo, CLAN_BATTLE_MEMBER* pMemberInfo );
	void		SendClanMatchTeamLeaveRequest( INT32 i32Result );
	void		SendClanMatchTeamChangePerRequest( INT32 i32Result, UINT8 ui8PerMax );
	void		SendClanMatchTeamChangePerResultRequest( UINT8 ui8PerMax );
	void		SendClanMatchTeamInfoRequest( CLAN_BATTLE_TEAM* pTeam, CLAN_BATTLE_MEMBER* pMemberInfo );
	void		SendClanMatchFightRequestRequest( INT32 i32Result );
	void		SendClanMatchChatingRequest(UINT8 ui8NickSize, char* pstrNick, UINT8 ui8ChatSize, char* pstrChating );
	void		SendClanMatchFightResultRequest( CLAN_BATTLE_TEAM_INFO* pTeamInfo );
	void		SendClanMatchFightAccectRequest( INT32 i32Result );
	void		SendClanMatchFightAccectResultRequest( INT32 i32Result );
	void		SendClanMatchFightIfoResultRequest( CLAN_BATTLE_TEAM* pBattleTeam, CLAN_BATTLE_CLAN* pBattleClan);
	void		SendClanBattleChangeClanInfo( CLAN_BATTLE_CLAN* pBattleClan );

	void		SendClanBattleRoomCreate( UINT32 ui32TeamIdx, UINT32 ui32RequsetTeamIdx, UINT8 ui8PerMax );
	void		SendClanBattleRoomJoin( UINT32 ui32RoomIdx, UINT32 ui32MatchTeam );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//메신저 패킷
	void		SendMessengerSendNote(INT32 i32result);
	void		SendMessengerClanNote(INT32 i32result);
	void		SendMessengerNoteList(UINT8 ui8pageCount, UINT8 ui8noteCount, NOTE_HEADER * noteHeaderList, NOTE_BODY * pnoteBodyList);
	void		SendMessengerCheckReadedNote(UINT8 ui8readedCount, INT32* pi32noteIdList);
	void		SendMessengerDeleteNote(INT32 i32result, UINT8 ui8deleteCount, INT32* pi32noteIdList);
	void		SendMessengerReceiveNote(NOTE_HEADER noteHeader, UINT8 ui8senderSize, UINT8 ui8messageSize, char * psender, char * pstrmessage);
	void		SendGetRemainMoney( INT32 i32result, UINT32 ui32Point, UINT32 ui32Cash );
	void		SendGetRemainMoneyNCSA( INT32 i32Result, UINT32 ui32Point, UINT32 ui32NCCoin, UINT32 ui32HappyCoin );
	void		SendGetPoint( UINT32 ui32Point, 	BOOL	bIsRSJackPot );
	void		SendCheckNickName( INT32 i32Rv );
	void		SendNickName( INT32 i32WorkIdx, char* pstrNickName );
	void		SendUserRecord( USER_INFO_RECORD* pUserRecord );
#ifdef USE_UNDECIDED_DESIGN_TEST
	void		SendChannelEnterResult(INT32 i32result);
#endif

public:
	CUserSession();
	virtual ~CUserSession();

	virtual BOOL OnCreate(SOCKET Socket, struct sockaddr_in * pAddr);
	virtual BOOL OnDestroy(BOOL bForceMainThread = FALSE);

	virtual INT32 PacketParsing(char * pPacketBuffer, INT32 i32Size); 

	BOOL _PacketParsingOper( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingAuth_server( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );	
	BOOL _PacketParsingAuthShop( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingTest( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );	
	BOOL _PacketParsingAsc( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );	
	BOOL _PacketParsingClan( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );	
	BOOL _PacketParsingClanBattle( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingServerMessage( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingBase( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingLobby( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingInventory( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingShop( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingRoom( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL _PacketParsingBattle( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );

	

	virtual	BOOL SendPacketMessage(i3NetworkPacket * pPacket);

	void	SendBuyQuestCardResult( INT32 i32Rv, UINT32 ui32GoodsID, INT32 i32ThreadIdx, TWareDxIdx i64ItemWareDBIdx, UINT8 ui8BuyType );	// 임무카드 구입  결과 보내기

	void	SendMyItemInfo( CUserSession * pSession, UINT8 ui8Type );	// 특정유저에게 나의 아이템 정보 전달( 받을유저, 타입 : 1 - 로비, 2 - 룸 )
	
};

#endif
