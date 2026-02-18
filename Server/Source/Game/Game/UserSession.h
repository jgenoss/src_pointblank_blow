#ifndef __USERSESSION_H
#define __USERSESSION_H

#include "ggsrv30.h"
#include "AntiCpXSvr.h"
#include "Xtrap_S_Interface.h"
#include "GPK\\Include\\GPKitSvr.h"

#include "../../../../Common/CommonSource/Gacha/GachaPurchase.h"
#include "Design/DesignBase.h"
#include "Design/DesignV1.h"
#include "Design/DesignV2.h"

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
#define CHECKVALID_CONNECT	if( GAME_TASK_CONNECT != m_i32MainTask )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_CONNECT, ProtocolID ); \
								return FALSE; \
							}

#define CHECKVALID_DBINDEX	if( m_i64UID == SERVER_VALUE_UNKNOWN )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_LOGIN, ProtocolID); \
								return FALSE; \
							}

#define CHECKVALID_LOGIN	if( GAME_TASK_LOGIN > m_i32MainTask  )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_LOGIN, ProtocolID); \
								return FALSE; \
							}

#define CHECKVALID_INFO		if( GAME_TASK_INFO > m_i32MainTask )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_INFO, ProtocolID); \
								return FALSE; \
							}

#define CHECKVALID_CHANNEL	if( GAME_TASK_CHANNEL > m_i32MainTask )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_CHANNEL, ProtocolID); \
								return FALSE; \
							}

#define CHECKVALID_LOBBY	if( GAME_TASK_LOBBY > m_i32MainTask || SERVER_VALUE_UNKNOWN == m_i32ChannelNum )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_LOBBY, ProtocolID ); \
								return FALSE; \
							}

#define CHECKVALID_ROOM		if( SERVER_VALUE_UNKNOWN == m_i32ChannelNum || m_i32RoomIdx == SERVER_VALUE_UNKNOWN || m_i32SlotIdx == SERVER_VALUE_UNKNOWN) \
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_ROOM, ProtocolID); \
								return FALSE; \
							}\
							if( GAME_TASK_BATTLE == m_i32MainTask && m_pRoom->IsGMPause()) \
							{\
								return FALSE; \
							}

#define CHECKVALID_ROOM_GM_PAUSE		if(m_i32RoomIdx == SERVER_VALUE_UNKNOWN || m_i32SlotIdx == SERVER_VALUE_UNKNOWN) \
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_ROOM_GM_PAUSE, ProtocolID); \
								return FALSE; \
							}

#define CHECKVALID_BATTLE	CHECKVALID_ROOM;\
							if( GAME_TASK_BATTLE != m_i32MainTask )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_BATTLE, ProtocolID ); \
								return FALSE; \
							}

#define CHECKVALID_NON_BATTLE	if( GAME_TASK_BATTLE == m_i32MainTask && m_i32ChannelNum > SERVER_VALUE_UNKNOWN && m_i32RoomIdx > SERVER_VALUE_UNKNOWN )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_NON_BATTLE, ProtocolID ); \
								return FALSE; \
							}

#define CHECKVALID_QUICK_JOIN	if( m_i32MainTask < GAME_TASK_INFO || m_i32MainTask > GAME_TASK_LOBBY )\
							{\
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_QUICK_JOIN, ProtocolID); \
								return FALSE; \
							}

#define CHECK_HAVE_MATCH_TEAM	if( m_TTeamID == MATCH_TEAM_INVALID_IDX )\
							{\
								return FALSE; \
							}

#define CHECK_HAVE_NO_MATCH_TEAM	if( m_TTeamID != MATCH_TEAM_INVALID_IDX )\
							{\
								return FALSE; \
							}

#define CHECK_MERCENARY		if( m_TMerID == MERCENARY_INVALID_IDX )\
							{\
								return FALSE; \
							}

#define CHECK_NOT_MERCENARY	if( m_TMerID != MERCENARY_INVALID_IDX )\
							{\
								return FALSE; \
							}

// 클랜 방문자는 클랜의 기능을 사용하지 못하게 해야 한다. 만약 클랜 기능을 사용하려하면 핵으로 인지.
#define CHECKVALID_CLAN_PAGE	if( m_bVisitClanPageEnter == true )return FALSE;

// 닉네임 생성 없이 구매를 하려고 하면 핵으로 처리한다.(이유 : 닉네임이 없으면 로그아웃시 저장이 안된다. 포인트차감되는 기능의 경우 치명적, 반드시 넣어야 함)
#define CHECKVALID_SHOP		if( '\0' == m_UserInfoBasic.m_strNick[0] )\
{\
	_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID, KILL_REASON_HACK_PACKET); \
	return FALSE; \
}


//////////////////////////////////////////////////////////////////////////
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
	bool					m_bUse;
	UINT8					m_ui8Type;
	T_ItemDBIdx				m_TWareDBIdx;
	UINT32					m_ui32ATUsed;

	void UseStart( UINT8 ui8Type, T_ItemDBIdx TWardDBIdx, UINT32 ui32UsedStartTime )
	{
		m_bUse				= true;
		m_ui8Type			= ui8Type;
		m_TWareDBIdx		= TWardDBIdx;
		m_ui32ATUsed		= ui32UsedStartTime;
	}

	void UseStop()
	{
		m_bUse = false;
	}
};

struct TYPE_ROOM_INFO;

/*-----------------------------------------------------------------------
Name : CUserSession
2007 03 06 : 버그확인 (UserSessionProtocol은 제외) 버그없음 
2007 07 06 : 메니저를 통과할 필요없는 것들은 바로 유저가 호출하도록 변경 
-----------------------------------------------------------------------*/

class CUserSession : public i3NetworkSession
{
	I3_CLASS_DEFINE( CUserSession ); 
private:
//	void PrintProtocolName(PROTOCOL protocol);
protected :	

	///////////////////////////////////////////////////////////////////////////
	// GameGuard
	///////////////////////////////////////////////////////////////////////////
	// NProtect
#ifdef USE_NPROTECT
	CCSAuth3					m_pCsa;
#endif

	// HACKSHIELD
	AHNHS_CLIENT_HANDLE			m_hClient;
	AHNHS_TRANS_BUFFER			m_stRequestBuf;
	AHNHS_TRANS_BUFFER			m_stResuestBuf;

	/// GAMEGUARD_XTRAP
	char						m_pXtrapSessionBuf[ XTRAP_CS4_BUFSIZE_SESSION ];		// Session Buffer ( Send to Client )

#ifdef USE_GPK					// GAMEGUARD_GPK
	INT32						m_i32CodeIdx;
#endif

	BOOL						m_bHaveInfo; 
	INT32						m_i32MainTask;								// 위치
	INT32						m_i32SubTask;								// 서브위치
	
	UINT32						m_ui32EncriptKey;							//암호화 키 입니다.
	BOOL						m_bSendPacket;
	INT32						m_i32SendErrorCount;
	BOOL						m_bEnterServer;								// 체널에 한번이라도 접속했는지를 체크

	//  패킷 정리 시작
	UINT32						m_ui32BirthDay;								// 생년 월일 

	bool						m_abGetInfoToServer[ GET_INFO_TO_SERVER_MAX ];	// 정보를 받았는지에 대한 Flag

public:

	// HeartBit2 받았다
	bool						m_bIsRecvHeartBit2;

	// 패킷 처리 테스트코드
	REAL64						m_r64ParseTimeHigh;
	PROTOCOL					m_ui16ParseTimeHighProtocol;

	//초당 패킷 확인 
	UINT32						m_ui32ATPacketSecond; 
	UINT32						m_iPacketSecondCount; 

	// 패킷 정리 시작

	// ----------------------------------------------------------------------------------------
	// 게임서버 내부 사용
	UINT32						m_ui32LoginKey;								// 로그인 비밀키
	UINT32						m_ui32CreateData;							// 생성 날짜이긴 한데 태국때문에 있습니다. 수정 요망	-- 현우 일해라 --

	INT32						m_i32SelectChannelNum;						// 다른 서버의 ACK를 받은 후에 채널 이동할 경우 선택한 채널 번호를 보관 
	INT32						m_i32ChannelNum;							// 채널 번호
	INT32						m_i32LogChannelNum;							// (서버*10)+(m_i32ChannelNum+1)

	CRoom			*			m_pRoom;									// 방 포인터 // 직접 호출 하는것으로 변경 20070706	//이값에는 절대로 NULL를 넣으면 않됨
	INT32						m_i32RoomIdx;								// 방 번호
	INT32						m_i32SlotIdx;								// 방에서 슬롯 번호	

	UINT16						m_ui16UsedItemCount;						// 한 게임에서 사용했던 아이템 개수
	// T_UID						m_i64UsedItemList[MAX_INVEN_COUNT];			// 한 게임에서 사용했던 아이템 리스트
	USED_ITEM_LIST				m_aUsedItemList[ MAX_INVEN_COUNT ];			// Item list in a game match
	UINT8						m_aui8WeaponUseCount[ CHAR_EQUIPMENT_WEAPON_COUNT ];	// 한 게임에서 사용했던 Type 별 무기 개수

	UINT64						m_MacAddress; 
	UINT8						m_ui8LocaleCode;


	UINT8						m_ui8Country;

	bool						m_bWriteHackLog;							// 해킹 ZLog 기록여부. 접속 기준 유저당 1번만 기록하기위해.
	char						m_strDXDllMD5[DX_DLL_MD5_SIZE];
	HACK_CHECK_LOGIN			m_stHackCheckLogin;
	char						m_strEndReason[MAX_CLIENT_END_REASON];

	UINT64						m_ui64PingTime;									// 로그아웃시 평균키를 남기기 위한 핑 보관 변수
	UINT32						m_ui32PingCount;

	// ----------------------------------------------------------------------------------------
	// 클라이언트와 공동 사용

	T_UID						m_i64UID;									// DataBase idx		
	char						m_strId[NET_ID_SIZE];						// 아이디				-- 현우 일해라 --
	//char						m_strPasswd[NET_PW_SIZE];					// 패스워드				-- 현우 일해라 --	필요한가 몰라서 지웁니다.

	char						m_WebKey[NET_SESSION_KEY_SIZE + 1];			// 웹 세션 키

	UINT8						m_ui8Gender;								// 성별
	UINT8						m_ui8Age;									// 나이

	UINT32						m_i32ChangeDBFlage;							// 유저 정보 저장 유무 	
	UINT64						m_ui64ChangeCharaDBFlage;					// 유저 캐릭터 정보 저장 유무 	

	//UINT32						m_ui32LoginBattleTime;						// 로그인 후부터의 배틀 타임
	//UINT32						m_ui32LastBuyCash;							// 마지막 구매 날짜.

	UINT32						m_ui32ViewRank;								// 현재표시되는 계급

	UINT8						m_ui8FindUserRank;							// 상세정보 보기한 유저의 랭크(친구등록 할때 사용)
	UINT8						m_ui8FindUserNickColor;							// 상세정보 보기한 유저의 컬러닉네임(친구등록 할때 사용)

	// 내 정보 
	USER_INFO_BASIC				m_UserInfoBasic;							// User 기본 정보
	USER_INFO_SERVER			m_UserInfoServer;							// User 서버 정보
	USER_INFO_RECORD			m_UserInfoRecord;							// User 전적 정보 
	USER_INFO_INVITEM_DATA		m_UserInfoInvItemData;						// User 아이템 실제 정보
	UINT8						m_ui8UserScopeType;							// User 스코프 정보

	CMultiSlot					m_MultiSlot;
	
	USER_INFO_DAILY				m_UserInfoDaily;							// User 당일 전적 정보	
	USER_INFO_LAST_GAME			m_UserInfoLastGame;							// User 마지막 게임정보

	UINT32						m_ui32BasicEquipCharaDino;

	CInventory					m_Inventory;

	//UINT16					m_ui16InventoryCount;
	//INVEN_BUFFER				m_aInventory[ MAX_INVEN_COUPON_COUNT ];		// Inventory Item

	// 메달정보
	UINT32						m_ui32Miniature;							//약장
	UINT32						m_ui32Insignia;								//휘장
	UINT32						m_ui32Order;								//훈장
	UINT32						m_ui32Master;								//마스터
	
	// 퀘스트
	UINT32						m_ui32QuestEventDate;													// 이벤트 임무카드 날짜
	UINT8						m_aaui8QuestCtxActive[MAX_CARDSET_PER_USER][MAX_QUEST_PER_CARDSET_NEW];	// 임무카드 진행 횟수 ( 3/4 ) 이런식으로 몇개까지 진행 했는지를 나타냅니다.
	QUESTING_INFO				m_QuestIngInfo;															// 임무카드 총 진행 상태
	UINT8						m_aui8QuestCtxActiveNow[MAX_QUEST_PER_CARD_NEW];						// 현재 임무카드 진행 횟수 한 Room 에서는 하나의 임무카드 셋만 깰수 있기 때문에 4개만 가지고 있습니다.
	UINT16						m_ui16ActiveIndexCardNow;												// 현재 Room 내에서 임무카드 완료 여부


	// 호칭
	UINT64						m_ui64GainUserTitle;							// 획득한 호칭 (Bit 단위입니다)
	UINT8						m_aui8EquipUserTitle[MAX_EQUIP_USER_TITLE];		// 장비된 호칭 Id
	INT32						m_i32LimitUserTitleCount;						// 사용가능 호칭 장비 슬롯 갯수
	
	ITEM_INFO					m_aTBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_COUNT ];			// User 장비 정보 Weapon 
	ENDURANCE_USED_WEAPON		m_aEnduranceUsedWeapon[ CHAR_EQUIPMENT_WEAPON_COUNT ];		// 무기는 장비슬롯 대신 무기 사용타입으로 따로 기록하고 있어...
	UINT8						m_aUseWeaponEndurance[ CHAR_EQUIPMENT_WEAPON_COUNT ];			// 무기 내구도 수치 ( Pri, Sec...)

	ITEM_INFO					m_aTBattleEquipParts[ CHAR_EQUIPMENT_PARTS_COUNT ];			// 배틀에서 사용하는 캐릭터 리스트 (배틀중 캐릭터 교체하여 m_UserInfoEquipmentChara와 달라질 수 있음)
	ENDURANCE_USED_WEAPON		m_aEnduranceUsedChara[ CHAR_EQUIPMENT_PARTS_COUNT ];			// 무기는 장비슬롯 대신 무기 사용타입으로 따로 기록하고 있어...
	UINT8						m_aui8CharaUseCount[ CHAR_EQUIPMENT_PARTS_COUNT ];			// 한 게임에서 사용했던 슬롯별 캐릭터 개수

	//LogD Temp 변수
	INT32						m_i32RespawnCount;							// 리스폰 카운트
	BOOL						m_bForceRoomLeave;							// 강퇴당한 유저 입니다.
	
	//LogD에서 필요함	
	UINT32						m_ui32AccConnectTime;						// 누적 접속시간입니다.
	UINT32						m_ui32AccConnectCount;						// 접속 카운트 입니다. 

	// 게임가드 연동
	UINT32						m_ui32ATGameGuardSend;						// 게임가드 인증 타임
	UINT32						m_ui32ATGameGuardRecv;						// 게임가드 인증 타임
	UINT32						m_ui32ATLastRecv;							// 마지막 받은 타임 (서버 타임)

	BOOL						m_bHSCheck;									// 핵실드 서버 연동 진행 여부 ( 태국 가레나에서만 확인하여 동작 ) - 서동권
		
	DATE32						m_dt32LoginDate;
	UINT32						m_ui32ATLogin;								// 로그인 요청한 타임
	UINT32						m_ui32ATKillDelay;							// 서버에서 죽인 패킷 전송 시간 ( 해킹일 경우에만 ) - 서동권
	UINT32						m_ui32ErrorCode;
	wchar_t						m_wstrHackNotice[ DEF_SIZE_HACK_NOTICE ];	// 서버에서 죽인 이유 (String )
	UINT32						m_ui32ATIsKill;								// 서버에서 죽일 시간
	UINT32						m_ui32KillReason;							// 서버에서 죽일 이유
	BOOL						m_bHackKill;								// 서버에서 처리 여부 ( 해킹일 경우에만 ) - 서동권
	DATE32						m_dt32LastLoginTime;						// 로그인시 ZLog 에 남길 마지막 로그인 시간( 지금 로그인 하기 전 )

	// Lobby용 방 리스트
	UINT32						m_ui32UpdateRoomListPage;					// 예전과 다른방식으로 사용
	BOOL						m_bRequestRoomList; 
	BOOL						m_bRequestUserList; 
	UINT32						m_UpdateUserListPage;						//예전과 다른방식으로 사용

	// 유저찾기(찾기용 계정)
	T_UID						m_i64FindUID;
	TTCHAR						m_strFindNick[NET_NICK_NAME_SIZE];


	// 친구
	UINT8						m_ui8FriendCount; 
	
	// 차단 유저
	UINT8						m_ui8BlockCount; 

	
	// 클랜 매치.
	T_TeamID					m_TTeamID;		// 입장중인 매치팀 ID.
	T_MerID						m_TMerID;		// 용병 ID.
	bool						m_bForceLeaveMatchRoom;	// 리더에 의해 클랜 매치룸에서 퇴장당함.
	bool						m_bMerPenalty;	// 용병 이탈 패널티 체크용 플래그
	DATE32						m_dt32MerPenaltyTime;	// 용병 페널티 시간.( 이탈로 인한 )
	UINT8						m_ui8MerClass;			// 용병 등록시 주무기 타입.	WEAPON_CLASS_TYPE
	UINT8						m_ui8MerRank;			// 용병 랭크. 

	UINT8						m_ui8Ping;

	PROTOCOL					m_ui16BackProtocolID;						// 로그 추적용 Back Protocol 입니다. 추후에 삭제할 예정

	UINT32						m_ui32RTBattleCheck; 

	BOOL						m_bIsBattleComplete;

	// 로그인 시간 체크
	UINT32						m_ui32LoginST;								// 로그인 Req 받은 시간
	UINT32						m_ui32ATInfo;								// 로그인 Req 받은 시간
	UINT8						m_ui8LoadingStart;							// 로딩 시작 패킷을 보냈는지 확인 체크를 위한 변수
	
	INT16						m_i16PacketRandSeed;						// 패킷 랜덤 시드값

	// 어뷰징 체크
	UINT32						m_ui32HoldAccExp;							// 어뷰징 유저 보류 EXP
	UINT32						m_ui32HoldAccPoint;							// 어뷰징 유저 보류 POINT

	UINT8						m_ui8AbusingLevel;							// 어뷰징 레벨
	UINT32						m_ui32AbusingPopupTime;						// 어뷰징 팝업 시간

	UINT32						m_ui32RTBattleStart;						// 배틀시작시간. 어뷰징 체크에 사용. 비난입 유저는 0이고, 난입 유저는 1이상의 값을 갖는다.
	UINT32						m_ui32BattleStartInvenTime;					// 배틀시작시 인벤시간. 배틀시작시간과 같이 기록되나, 표현방식이 달라 따로 저장.

	UINT32						m_ui32RTBattleDeath;						// 배틀 중 죽은 시간. 리스폰 해킹 방지에 사용
	UINT32						m_ui32RespawnDelay;							// 리스폰 딜레이 시간 (리스폰 단축이 적용된 딜레이 시간)
 
	UINT32						m_ui32LastPlayTime;							// 마지막 방에서 플레이 시간 ( 강퇴당한 게임도 처리, m_ui32PlayTime 는 강퇴시엔 값을 세팅해주지 않아서 만듬 )
	UINT32						m_ui32LastRoomUserCount;					// 마지막 방에서 플레이 시 방 유저 수
	STAGE_MODE					m_eLastStageType;							// 마지막으로 진행한 모드 ( 맵이 아니라 모드 입니다. )
	UINT16						m_ui16PrevExpRank;							// 이전 방에서 얻은 경험치
	// ZLog 로그를 위하여 추가합니다 - 서동권 2011.07.15
	UINT32						m_ui32RoomBattleCount;						// 한 방에서 몇판을 했는지
	UINT32						m_ui32PlayTime;								// 이전 방 플레이 시간

	UINT32						m_ui32RTAccActualBattle;					// 현재 진행중인 방에서 실제 배틀에 소비한 시간
	UINT32						m_ui32RTRespawn;							// 리스폰 시간
	UINT32						m_ui32FirstObserverTime;					// 난입 후 첫 관전시간
	BOOL						m_bFirstObserver;							// 난입 후 첫 관전인지

	BOOL						m_bCheckLogout;								// 정상적으로 로그아웃 되었는지 판별.

	// 중복채팅 방지 - 김용수
	DATE32						m_dt32HackChatBlockDate;					// 채팅 제재 제한 시간.
	DATE32						m_dt32ChatEndTime;							// 채팅 체크 종료 시간.
	INT32						m_i32ChatCount;								// CHAT_HACK_TIME 안에 중복입력을 시도한 횟수
	CHAT_LIMIT_STATE			m_eChatLimitState;							// 채팅 제재 단계
	UINT32						m_ui32ChatLimitTime;						// 채팅 제재 제한 시간
	UINT32						m_ui32ChatWarningTime;						// 채팅 제재 경고 시간

	UINT8						m_ui8LoginLinkThreadIdx;					// Trans 서버의 Link Thread Idx
	UINT32						m_ui32LoginWaitCount;						// 나의 로그인 순번
	UINT32						m_ui32ATLoginWaitSend;						// 로그인 패킷의 Send Time

	// GM Pause
	UINT32						m_ui32ATCanRespawnPacket;					// Pause중에 온 리스폰요청은 버려지지만, Pause중이라도 '이값 < 현재절대시간 < 이값 + 3초' 일 경우에는 수신받는다.

	//////////////////////////////////////////////////////////////////////////
	// Test
	UINT32						m_ui32TestLogLoginStartTime;

	//////////////////////////////////////////////////////////////////////////

	// 출석 체크
	ATTENDANCE_USER_SVR			m_stAttendance[ATTENDANCE_TYPE_MAX];
	ATTENDANCE_INFO				m_stAttendanceInfo[ATTENDANCE_TYPE_MAX];

	MEDAL_ALL_INFO				m_MedalAllInfo;			

	// ----------------------------------------------------------------------------------------
	bool						m_bVisitClanPageEnter;						// 자기 클랜 방문(0)한 것인지 클랜 방문자(1)인지 체크(클라입력무시용으로 쓰임)
	UINT32						m_ui32VisitCUID;							// 방문한 클랜의 고유번호
	UINT32						m_ui32ClanRankingDate;
	// ----------------------------------------------------------------------------------------
	
	QUICKJOIN_INFO				m_aQuickJoinInfo[MAX_QUICK_JOIN_INFO_COUNT];		// 빠른입장 설정정보.

	CGachaPurchase				m_GachaPurchase;


	// 중독방지시스템 : 유저가 게임서버 컨넥(진입)시 트랜스서버로부터 받는 정보
	bool						m_bAASDailyUsageWay;		// 하루이용량 제한 방식에 대상인지 여부값
	bool						m_bAASAppointedWay;			// 지정시간 금지 방식에 대상인지 여부값
	UINT32						m_aui32AASBanTime[DAY_COUNT_IN_WEEK];	// 계정별 지정요일&시간 금지 대 값 [0-일요일, 1-월요일, 2-화요일 … 5-금요일, 6-토요일]
	UINT16						m_ui16AASAccMin;

	// 중독방지 시스템 : 유저가 게임서버 컨넥(진입)시 설정되는 정보
	UINT16						m_ui16AASConnectTime;		// 트랜스서버로부터 정보받았을때의 AAS타임

	// GM기능강화 - 1:1채팅 중인 GM관련 저장 정보
	T_UID						m_i64GMUID;

	// QA자동화 : QA ID가 어떤 치트키를 ON/OFF 했는지 정보를 가지고 있음. Toggle 정보는 QA_AUTO_로 검색 - 안국정
	UINT8						m_ui8QAAutoFlag;

	bool						m_bGiftAutoAuth;	// 선물함에 선물이 인벤토리에 자동지급 됬는지 여부

	UINT32						m_ui32ReVoteTime;			// 강제퇴장투표 할 수 있는 시간 (마지막 투표 발의한 시간으로부터 +60초)
	UINT32						m_ui32GuardTime;			// 피신고자가 다시 신고당할수 있는 시간
	
	// 실시간 PC방 혜택 제거 / 추가
	UINT8						m_ui8OldPCCafe;
	UINT8						m_ui8NewPCCafe;

	// 확성기 사용시간 기록
	DATE32						m_dt32MegaPhoneUseTime;

	UINT32						m_ui32MatchCountCheckTime;			// 클랜 매치팀 카운트 정보 받은 시간.

	// 장성계급 실시간 반영 - 적용될 장성계급
	UINT32						m_ui32ChangedRank;

	UINT8						m_ui8PBVer;
	CDesignBase*				m_pDesign;		// 
	CDesignV1*					m_pDesignV1;	// v1.0 기획 적용
	CDesignV2*					m_pDesignV2;	// v1.5 기획 적용

	USER_INFO_PCSPECS			m_stPCSpec;

protected:
	void						_OnLeaveChnnel();
	bool						_PacketDecript( i3NetworkPacket* pPacket );
	
	void						_SetChannelBonusPacket( UINT8 * pui8ChannelType , i3NetworkPacket * pPacket );

	//void						_SendErrorMessage(UINT32 ErrorCode, INT32 i32KillReason = KILL_REASON_HACK_PACKET, INT32 Arg = 0);		// 에러 패킷 보내기
	BOOL						_SendShopInfo(char* strMD5Key );								// Shop 정보 보내기
	void						_SendFieldShopInfo(UINT32 ui32FieldShopVersion);

	void						_GetMyInfoPacket( INT32 i32ThreadIdx, i3NetworkPacket* pPacket, BOOL bClan );	// Trans 서버로 부터 받는 나의 정보에 대한 패킷을 파싱합니다.

	void						_SetUserRecord();												// 유저의 전적을 세팅해준다.
	void						_SetCalcLimitTitleCount();										// 호칭장착 가능 슬롯을 계산합니다.
	void						_SetAge();														// 나이를 설정합니다.
	void						_ExceptionInsert( T_ItemID ui32ItemID, UINT8 ui8ItemType = ITEM_ATTR_TYPE_BASIC, INVEN_BUFFER_SET* pInvenSet = NULL );
	
	
	
	INT32						_BuyGoods( UINT8 ui8GoodsCount, BUY_BASKET *stBasket, SPEND_TYPE eBuyType = SPEND_TYPE_BUY_GOODS, TTCHAR* pNick = NULL, TTCHAR* pMsg = NULL);			// 물건을 구입합니다.
	INT32						_CheckBuyRestriction( UINT32 ui32GoodsID );						// 무기일 경우 호칭이 있는지를 체크합니다.
	
	// 장착된 무기를 체크하고 내구도일경우 사용처리해준다.
	INT32						_CheckWeaponEquip(	BOOL bUseItem, UINT8 ui8WeaponFlag, ITEM_INFO* aWeaponList, INVEN_BUFFER* aWeaponInven, UINT32 ui32InvenTime, INT32 i32CheckSkipFlag = 0 );	
	void						_SetWeaponEquip(	INVEN_BUFFER* aWeaponInven, INT32 i32CheckSkipFlag = 0  );	

	// 장착된 케릭터를 체크하고 내구도일경우 사용처리해준다.
	INT32						_CheckCharEquip(	BOOL bUsedItem, UINT32 ui32Team, ITEM_INFO* aCharaList, INVEN_BUFFER* aCharaInven, UINT32 ui32InvenTime, INT32 i32CheckSkipFlag = 0 );		
	void						_SetCharEquip(	INVEN_BUFFER* aCharaInven, INT32 i32CheckSkipFlag = 0  );	

	void						_SetUsedItem( T_ItemDBIdx TItemWareDBIdx, UINT32 ui32ItemID, UINT8 ui8Type, UINT32 ui32UsedDurablity, UINT8 ui8CheckSlotInfo );				// 아이템 사용처리를 해준다.

	INT32						_CharChangeEquipment( PACKET_CHAR_CHANGE_EQUIP_REQ* pPacket );
	INT32						_CheckEquipment(void);											// 장착된 아이템과 보유중인 무기를 검사하여 값이 이상할 경우 초기값으로 리셋해 줍니다.

	BOOL						_CheckCharEquipment( INT32 i32SlotIdx, S2MO_CHAR_EQUIP_INFO* pCharEquipInfo, UINT32& ui32Check );
	BOOL						_CheckCommonEquipment( ITEM_INFO* pEquipment, UINT32& ui32Check);

	BOOL						_CheckItemAuth( INVEN_BUFFER* pItem );
	BOOL						_CheckItemPositionCommon( UINT32 ui32Item, CHAR_EQUIPMENT_COMMON eSlot );

	// 케릭터
	T_RESULT					_CheckCreateChar( PACKET_CHAR_CREATE_CHARA_REQ* pCreateChar );
	// 호칭
	BOOL						_ChangeUserTitle( UINT8 userTitleId, LOG_GET_TITLE* pstLogGetTitle );	// 호칭을 획득합니다.
	
	REAL32						_GetRepairPremium( UINT32 ui32Endurance );
	
	// Quest
	INT32						_LastQuestItem( QUEST_ITEM_INFO_NEW* pItemInfo, QUEST_ITEM_INFO_NEW* pLastItemInfo, BOOL bLastGet );
	
	void						_DeleteQuestCardSet( UINT8 ui8CardSet );						// 임무카드를 삭제합니다.
	void						_SetQuestEvent();												// 이벤트 임무카드를 설정합니다.

	// 아이템 인증 
	INT32						_SetItemAbilityAuth( INVEN_BUFFER* pBuffer );					// 아이템 인증 게임서버단 처리부분
	INT32						_SetItemAbilityAuth( INVEN_BUFFER* pBuffer, void* pData );		// 아이템 인증 게임서버단 처리부분 능력 소유
	INT32						_SetItemAbilityChange( INVEN_BUFFER* pBuffer, void* pData );	// 사용중인 아이템 옵션 변경

	void						_ResetRecord_MAU( void);										//전적 초기화	
	void						_ResetDisconnect_MAU( void );									//이탈률 초기화 
	void						_ResetKillDeath_MAU( void );									//킬데스 초기화	
	
	// UserSessionException.cpp 
	void						_ExceptionEquip( void );							// 서버 인벤 지급 
	void						_ExceptionChara(T_ItemID TCharaID, UINT8 ui8ItemType );					// 서버 인벤 지급 - Chara
	void						_ExceptionSpecial( void );						// 서버 인벤 지급 - Special
	void						_ExceptionTitleItem( INT32 i32ThreadIdx, UINT8 ui8userTitleId );	// 호칭획득에 따른 무기 지급입니다.

	//Find Quest Slot
	INT8						_FindInsertQuestSlot(UINT8 ui8CardType);						// 0, 1, 2, 3 

	T_RESULT					_CheckUseNickName( TTCHAR* strNick );

	BOOL						_CheckClanName( TTCHAR* pClanName );

	BOOL						_CheckIsSameClan( INT32 i32SessionIdx );						// 해당 유저와 동일한 클랜인지 체크.

	void						_EquipmentInBattle( ITEM_INFO* pItemInfo );

	// 채팅 유효성 검사
	BOOL						_CheckChat(TTCHAR* strChat, INT32 i32ChatType, INT16 i16Size, PROTOCOL ui16Protocol = 0);					// 중복채팅인지, GM Pause중의 채팅인지

	INT32						_CheckBuyGoods( SHOP_GOODS_EXPANSION* pGoods, BUY_PRICE *pBuyPrice );
	INT32						_ExtendItem( EXTEND_BASKET* stExtendItemData );						// 소유한 물건의 기간을 연장한다.(중복되는 물풀들 때문에 생김)
	void						_FailDeleteChara(INT32 i32Rv, UINT8 ui8CharaSlotIdx );
	INT32						_OnJoinRoomByQuickJoin( INT32 i32ChannelIdx, INT32 i32RoomIdx ); // 빠른입장 방입장.

	INT32						_RepairItem( T_ItemDBIdx TWareDBIdx, UINT32* pui32Point, UINT32* pui32Cash, UINT32* pui32AuthArg,  T_ItemID* pTItemID );

	BOOL						_CheckCompleteMedal(MEDAL_TYPE eMedalType, UINT16 ui16MedalIdx, UINT8 ui8Level );	// 메달 달성 여부 체크

	INT32						_ChaeckCreateRoom( void );					 

	BOOL						_CheckUlsanGuard(INT32 i32ThreadIdx, PROTOCOL	ProtocolID, HACK_TYPE_BASE* pUSGBase, UINT64 *pui64DXFunc, char *pID);


	void						_SyncEquipIdx(BOOL bCommonOnly = FALSE);		// 장착 DB IDX로 ItemID를 셋팅 합니다.
	T_RESULT					_CheckCharaDelete(INT32 i32DeleteSlotIdx);		// 캐릭터 삭제 조건 검사
	BOOL						_CheckBeretTitle(INT32 i32ItemID);		//호칭 베레모 장착 가능 여부 확인
	BOOL						_CheckBeretMatchtitle(UINT8 ui8Title);	//호칭 베레모와 호칭 확인

	void						_SendEquipedInfo(void);	// 현재 장착중인 정보( 멀티슬롯 ) 전달

public: 
	INT32						CreateRoom( INT32 i32ThreadIdx, ROOM_INFO_CREATE* pInfoCreate );					// 방을 만듭니다.

	void						LobbyEnterReq( INT32 i32WorkIdx, i3NetworkPacket* pSendPacket, S2MO_ROOM_ENTER eEnterType = S2MO_ROOM_ENTER_NORMAL );	// 로비 입장 패킷
	INT32						CheckEnterChannel( UINT32 ui32Channel );	// 입장 가능한 체널인지 체크합니다.
	void						_SendErrorMessage(INT32 i32WorkIdx, UINT32 ui32ErrorCode, PROTOCOL ProtocolID=0, INT32 i32KillReason=KILL_REASON_HACK_PACKET, wchar_t* wstrMessage=NULL );		// 에러 패킷 보내기
	void						_SendErrorMessage(INT32 i32WorkIdx, EVENT_ERROR eErrorCode, PROTOCOL ProtocolID=0, INT32 i32KillReason=KILL_REASON_HACK_PACKET, wchar_t* wstrMessage=NULL )		// 에러 패킷 보내기
	{
		_SendErrorMessage( i32WorkIdx, (UINT32)eErrorCode, ProtocolID, i32KillReason, wstrMessage );
	}
	void						DisconnectUserDelay( UINT32 ui32ErrorCode, INT32 i32KillReason=KILL_REASON_HACK_PACKET, wchar_t* wstrMessage=NULL );
	void						DisconnectUserDelay( EVENT_ERROR eErrorCode, INT32 i32KillReason=KILL_REASON_HACK_PACKET, wchar_t* wstrMessage=NULL )
	{
		DisconnectUserDelay( (UINT32)eErrorCode, i32KillReason, wstrMessage );
	}

	void						CheckQuestItem2();
	void						_SendQuestInfoPacket( INT8 i8CardSolt );						// 임무카드를 보냅니다.

	TTCHAR *					GetNickName(void)		{ return m_UserInfoBasic.m_strNick; }
	UINT8						GetNickColor()			{ return m_UserInfoInvItemData._ui8NickColor;	};
	char *						GetUserId(void)			{ return m_strId; }
	INT32						GetChannelNumber(void)	{ return m_i32ChannelNum; }				//이거 차후 삭제 
	UINT32						GetClanIdx(void)		{ return m_UserInfoBasic.m_ui32Clanidx; }	
	T_UID						GetUID(void)			{ return m_i64UID; }	

	GAME_TASK_SUB				GetSubTask()			{	return (GAME_TASK_SUB)m_i32SubTask; };	// 서브 위치 반환	
	USER_INFO_BASIC*			GetUserInfoBasic()		{	return &m_UserInfoBasic;	};
	UINT8*						GetEquipUserTitle()		{ return m_aui8EquipUserTitle;	};

	INT32						GetSessionIndex( void )		{ return m_SessionIdx; }
	INT32						GetPosition(void)			{ return m_i32MainTask; }

	void						CheckInventory_MAU(INT32 i32ThreadIdx, INVEN_CHECK eCheckType = INVEN_CHECK_DEL_PERIOD);

	void						ChangeDataBaseInfo(INT32 i32ChangeType)			{ m_i32ChangeDBFlage |= i32ChangeType; }	// DB에 저장될 것들의 플래그를 담습니다.
	void						ChangeCharaDataBaseInfo(UINT64 ui64ChangeType)	
	{ 
		m_ui64ChangeCharaDBFlage |= ui64ChangeType; 
	}	// DB에 저장될 것들의 플래그를 담습니다.
	void						CheckEquipment();												// 장착된 무기를 검사합니다.	내부에서 _CheckEquipment 를 호출한 후 변경된 사항이 있으면 클라이언트에 패킷을 보냅니다.

	void						SetPosition(INT32 i32Position);

	BOOL						CheckGameGuard_M( UINT32 ui32GameGuardTimeOut, UINT32 ui32GameGuardSendTime );
	void						SendGameGuardPacket_MU( INT32 i32WorkThread );					//게임 가드 패킷 보내기 

	void						SendChangeInven( INVEN_BUFFER_SET* pInvenSet );
	
	void						CheckRankUp( INT32 i32WorkThread, RANKUP_CHECK eCheckType = RANKUP_CHECK_ABSOLUTE );
	
	void						CheckEndurance();												// 내구도 사용 여부
	void						CharUseAllStop();												// 모든 장비 사용 정지

	void						KickUser_Module(T_UID i64UID, UINT8 Reason);	
	void						SetChangeNick( INT32 i32WorkThreadIdx, TTCHAR* pstrOrgNick, TTCHAR* pstrNewNick );
	void						SetChangeRank( INT32 i32WorkThreadIdx, UINT32 ui32ItemID );
	void						SetRankUp( INT32 i32WorkThreadIdx);
	void						SetChangeColorNick( INT32 i32WorkThreadIdx, UINT8 ui8Color );
	void						SetChangeColorMuzzleFlash();
	void						InvItemData();

	void						LogIn_MAU( INT32 i32WorkThread, INT32 i32Rv, UINT32 ui32BlockDate, LOGIN_INFO* pLoginInfo );

	void						GetMyInfo_A( INT32 i32WorkThread, INT32 i32Rv, i3NetworkPacket* pPacket );
	void						SendMyInfo( INT32 i32WorkThread, INT32 i32Result );				// 나의 정보를 보냅니다.
	void						ServerEnter_A( INT32 i32Rv, i3NetworkPacket* pPacket );
	void						ServerLeave_A( INT32 i32Rv );
	void						CreateNickName_A( INT32 i32Rv, UINT32 ui32NewPoint, TTCHAR* strNickName );
	void						CreateChar_A( PACKET_CHAR_CREATE_CHARA_ACK* pPacket, T_GoodsID TGoodsID , INVEN_BUFFER* pCharaInven);
	void						DeleteChar_A( INT32 i32WorkThread, T_RESULT i32Result, UINT8 ui8DeleteSlotIdx);
	void						ChangeStateChar_A( T_RESULT i32Result, UINT8 ui8SlotIdx, UINT8 ui8SlotState );

	void						SetClan_C( INT32 i32Result, UINT32 ui32CID, UINT8 ui8ClanState, UINT8 ui8ExpRank, UINT8 ui8PerRank, UINT32 ui32Mark, TTCHAR* strClanName );

	bool						CheckAuthLevel( AUTH_LEVEL eLevel = AUTH_LEVEL_MOD )
	{	
		if( m_UserInfoBasic.m_ui8AuthLevel < eLevel ) return false;	
		return true;
	}

	void						_SetRankByAuthLevel()
	{	
		switch ( m_UserInfoBasic.m_ui8AuthLevel ) 
		{ 
		case AUTH_LEVEL_MOD :	m_UserInfoBasic.m_ui32Rank = RANK_MOD;			break;
		case AUTH_LEVEL_GM :	m_UserInfoBasic.m_ui32Rank = RANK_GAMEMASTER;	break;
		default : return ;
		}
		ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);
	}

	BOOL						CheckSpecialAbility( SPECIAL_ABILITY eAbility )
	{	
		if( eAbility <= SA_NONE )	return FALSE;
		if( eAbility >= SA_MAX )	return FALSE;
		INT32 i32Check = 1 << (eAbility-1);
		if( i32Check & m_UserInfoBasic.m_ui8SpecialAbility ) return TRUE;	
		return FALSE;
	}

	UINT16						GetAASAccMin()
	{
		return m_ui16AASAccMin + (g_pContextMain->m_ui16AASCurTime - m_ui16AASConnectTime);
	}

	void						UpdateAASAccMin()
	{
		if ( !g_pContextMain->IsContentsEnabled( CONTENTS_SHUTDOWN ) )						return;
		if ( !(g_pContextMain->m_AASOption.m_ui8AASEnableFlag & AAS_METHOD_DAILY_USAGE) )	return;
		
		m_ui16AASAccMin = GetAASAccMin();
		ChangeDataBaseInfo( SAVE_DATABASE_AAS );
	}

	void						_GetPCCafeBonusValue( UINT8 ui8LocaleCode, UINT8 ui8PCCafe, REAL32* pr32Exp, REAL32* pr32Point );

	void						SetBirthDay_A( INT32 i32Rv, UINT32 ui32BirthDay );
	void						SetCharaSkill( UINT8 ui8Slot, UINT8 ui8Class );

	void						SendAttendanceItemResult( INT32 i32Rv );									// 출석체크
	void						SendAttendanceCheckResult( INT32 i32Rv, ATTENDANCE_USER_SVR* pstAttendance );	// 출석체크

	UINT32					GetUseItemInRoom();
	void					GetItemBonusInRoom( INT32 i32Hour, REAL32& r32AddExpItem, REAL32& r32AddPotItem, REAL32& r32AddSkillItem, UINT8& ui8ResultIcon );
	INVEN_BUFFER*			GetItemByWareDBIdx( T_ItemDBIdx& TWareDBIdx );					// 유저의 인벤에서 WareDBIdx 로 아이템을 검색한다.
	INVEN_BUFFER*			GetItemByItemID( T_ItemID TItemID );

	AAS_CATCH_METHOD		CheckAAS();

	void					AddPingTime(UINT16 ui16PingTime);
	UINT32					GetAveragePing();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//친구 패킷
	void		SendFriendList( UINT8 ui8FriendCount, FRIEND_INFO_STRUCT* pFriendInfo );		//친구 리스트 보내기 
	void		SendFriendAcceptError(INT32	i32ErrorCode);										//친구등록 에러 패킷 보내기 
	void		SendFriendInsertError(INT32 i32ErrorCode);										//친구추가 에러 패킷 보내기 
	void		SendFriendDeleteError(INT32 i32ErrorCode);										//친구삭제 에러 패킷 보내기 
	void		SetFriendChangeInfo_A(UINT8 ui8FriendIdx, UINT8 ui8State, UINT8 ui8NickNameSize, TTCHAR  * NickName, FRIEND_INFO * pInfo);	//친구 변경된 정보 보내기 
	void		SendFriendInvitedError(INT32 i32ErrorCode);
	void		SendFriendInvited(UINT8 ui8FriendIdx);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//차단 패킷
	void		SendBlockList( UINT8 ui8BlockCount, BLOCK_INFO_STRUCT* pBlockInfo );			//차단 리스트 보내기 
	void		SendBlockInsertResult(INT32 i32Result,  INT32 i32Idx = 0, UINT8 ui8Rank = 0, UINT8 ui8NickSize = 0, TTCHAR *strNick = 0, UINT8 ui8CommentSize = 0, TTCHAR *strComment = 0); //차단추가 결과 패킷 보내기 
	void		SendBlockDeleteResult(INT32 i32Result, INT32 i32Idx );							//차단삭제 결과 패킷 보내기
	
	void		OnInsertItem( INVEN_BUFFER* pInvenItem, INVEN_BUFFER_SET* pInvenSet = NULL );
	UINT32		OnDeleteItem( INT32 i32ThreadIdx, T_ItemDBIdx& TItemWareIdx, INVEN_BUFFER_SET* pInvenSet );
	UINT32		OnDeleteItemID( INT32 i32ThreadIdx, T_ItemID ui32ItemID, INVEN_BUFFER_SET* pInvenSet );
	void		SendNewEventCardSet(void);

	void		SendWhisper(TTCHAR* senderNick, TTCHAR* message, UINT16 messageSize, bool bIsGMUser,UINT8 ui8NickColor);				// 귓속말 받기
	void		SendWhisperError(INT32 i32result, TTCHAR* receiverNick, UINT8 ui8NickColor);							// 귓속말 보내기 실패
	void		SendFindUser(INT32 i32Result, NEW_FIND_USER_INFO* pNewFindUserInfo = NULL );	// New 유저 찾기.

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//클랜 패킷 
	void		SendClanListDetailInfo(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo);		// 클랜 리스트에서 특정 클랜 선택시 해당 클랜의 상세정보 요청
	void		SendClanDetailInfo(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo, CLAN_TOP_MEMBER * pTopMember, CLAN_NEWS * pNews);			//디테일 정보 보내기 
	void		SendClanMemberInfo( UINT8 ui8TotalCount, CLANMEMBER_INFO_STRUCT* pClanMember );	// 멤버 목록 요청
	void		SendClanMemberInfoChange(  T_UID i64UID, UINT64 ui64State );						// 멤버 상태 변경
	void		SendClanMemberInfoInsert( CLANMEMBER_INFO_STRUCT* pClanMember );				// 멤버 추가
	void		SendClanMemberInfoDelete( T_UID i64UID );										// 멤버 삭제
	void		SendClanCreate_C(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo);				//생성	
	void		SendClanJoinRequest(INT32 i32Result, INT32 i32ClanIdx);							//조인 
	void		SendClanCancleRequest(INT32 i32Result);											//캔슬	
	void		SendClanMemberContext(INT32 i32Result, CLAN_MEMBER_CONTEXT * pMemberContext);	//멤버 
	void		SendClanMemberList(INT32 i32Result, UINT8 section, UINT8 count, CLAN_MEMBER_BASIC_INFO * pMemberlist);		//멤버 리스트 
	void		SendClanMemberDetailInfo(INT32 i32Result, USER_INFO_RECORD* pRecord );			//멤버 상세정보
	void		SendClanMatchResultContext(PACKET_H_CLAN_ACK* pPacketAckH, PACKET_D_CS_CLAN_MATCH_RESULT_CONTEXT* pPacketAckD);													//클랜 매치 결과 리스트
	void		SendClanMatchResultList(PACKET_H_CLAN_ACK* pPacketAckH, PACKET_D_CS_CLAN_MATCH_RESULT_LIST_CNT* pPacketAckD, PACKET_D_CS_CLAN_MATCH_RESULT_LIST* pPacketAckD2);	//클랜 매치 결과 정보
	void		SendClanRequestContext(INT32 i32Result, CLAN_REQUEST_CONTEXT * pMemberContext);	//요청 멤버 
	void		SendClanRequestList(INT32 i32Result, UINT8 ui8section, UINT8 ui8count, CLAN_REQUEST_BASIC_INFO * pMemberlist);	//요청 멤버 리스트 
	void		SendClanRequestDetailInfo(INT32 i32Result, CLAN_REQUEST_DETAIL_INFO * pClanRequestDetailInfo);				//요청 멤버 정보 디테일
	void		SendClanAcceptRequest(INT32 i32Result);											//가입신청서 수락
	void		SendClanDenialRequest(INT32 i32Result);											//가입신청서 거절
	void		SendClanChatingRequest( UINT8 ui8NickSize, TTCHAR* pstrNick, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor );			// 클랜 체팅
	void		SendClanPageChatingRequest( UINT8 ui8NickSize, TTCHAR* pstrNick, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser,  UINT8 ui8NickColor  );
	void		SendClanSecessionRequest(INT32 i32Result);										//클랜 탈퇴
	void		SendClanCloseRequest(INT32 i32Result);											//클랜 헤체
	void		SendClanCommissionMasterRequest( INT32 i32Result);								//클랜장 위임
	void		SendClanCommissionStaffRequest( INT32 i32Result);								//클랜장 위임
	void		SendClanCommissionRegularRequest( INT32 i32Result);								//클랜장 위임
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
	void		SendClanReplaceNameResultRequest( TTCHAR* pstrName );
	void		SendClanReplaceMarkRequest( INT32 i32Result );
	void		SendClanReplacePersonMaxRequest( INT32 i32Result, UINT8 ui8ClanMaxCount );
	void		SendClanReplaceManagementRequest( INT32 i32Result );	
	void		SendClanCheckJoinAuthorityRequest( INT32 i32Result );
	void		SendClanRecordResetRequest( INT32 i32Result );
	void		SendClanAcceptRequestError( INT32 i32Result );									// 가입 신청 결과 에러	
	void		SendClanAcceptResultRequest( CLAN_DETAIL_INFO* pDetailInfo );					//가입신청 수락 결과
	void		SendClanDenialResultRequest( );													//가입신청 거절 결과
	void		SendClanDeportationResultRequest( );											//강제추방 결과
	void		SendClanMasterCommissionResultRequest( );										//강제추방 결과
	void		SendClanStaffCommissionResultRequest( );
	void		SendClanRegularCommissionResultRequest( );
	void		SendClanRoomInvitedRequest( INT32 i32Result );
	void		SendClanRoomInvitedResultRequest( T_UID TSenderUID );
	void		SendClanReplaceMemberBasicResultRequest( CLAN_MEMBER_BASIC_INFO* pMemberBasicInfo );	
	void		SendClanRecordResetResultRequest( );
	void		SendClanPointResetResultRequest();
	void		SendClanReplacColorNameResultRequest( UINT8 ui8Color );

	void		SendClanMatchResultEmotionSave(PACKET_H_CLAN_ACK* pPacketAckH, PACKET_D_CS_CLAN_MATCH_RESULT_EMOTION_SAVE* pPacketAckD);	// 클랜전 매치 정보 감정표현(좋아요) 응답
	void		SendClanMatchResultEmotionSaveFail(PACKET_H_CLAN_ACK* pPacketAckH);															// 클랜전 매치 정보 감정표현(좋아요) 응답 실패
	void		SendClanRequestAutoAcceptOption(PACKET_H_CLAN_ACK* pPacketAckH);

	void		SendClanMatchTeamContextRequest( CLAN_BATTLE_TEAM_CONTEXT* pTeamContext );
	void		SendClanMatchAllTeamContextRequest( CLAN_BATTLE_ALL_TEAM_CONTEXT* pAllTeamContext );
	void		SendClanMatchAllTeamListRequest( UINT16 ui16TotalCount, UINT16 ui16SectionCount, UINT16 ui16Section, UINT8 ui8Count, CLAN_BATTLE_TEAM_INFO* pBattleTeamInfo );
	void		SendClanMatchTeamDetailRequest( INT32 i32Result, CLAN_DETAIL_INFO* pDetail );
	void		SendClanMatchTeamCreateRequest( INT32 i32Success, UINT32 ui32Idx, CLAN_BATTLE_TEAM* pTeam );
	void		SendClanMatchTeamLeaveRequest( INT32 i32Result );
	void		SendClanMatchTeamChangePerRequest( INT32 i32Result, UINT8 ui8PerMax );
	void		SendClanMatchTeamChangePerResultRequest( UINT8 ui8PerMax );
	//void		SendClanMatchTeamInfoRequest( CLAN_BATTLE_TEAM* pTeam, CLAN_BATTLE_MEMBER* pMemberInfo );
	void		SendClanMatchFightRequestRequest( INT32 i32Result );
	void		SendClanMatchChatingRequest(UINT8 ui8NickSize, TTCHAR* pstrNick, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor );
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
	void		SendMessengerNoteList(UINT8 ui8pageCount, UINT8 ui8noteCount, NOTE_HEADER * noteHeaderList, NOTE_BODY * noteBodyList);
	void		SendMessengerCheckReadedNote(UINT8 ui8readedCount, INT32* pi32noteIdList);
	void		SendMessengerDeleteNote(INT32 i32result, UINT8 ui8deleteCount, INT32* pi32noteIdList);
	void		SendMessengerReceiveNote(NOTE_HEADER noteHeader, UINT8 ui8senderSize, UINT8 ui8messageSize, TTCHAR * sender, TTCHAR * pstrMessage);
	void		SendGetRemainMoney( INT32 i32result, UINT32 ui32Point, UINT32 ui32Cash );
	void		SendGetPoint( INT32 i32ThreadIdx, UINT32 ui32Point, UINT8 ui8GetRoute );
	void		SendCheckNickName( INT32 i32Rv );
	void		SendNickName( INT32 i32WorkIdx, TTCHAR* pstrNickName );
	void		SendUserRecord( USER_INFO_RECORD* pUserRecord );

	//////////////////////////////////////////////////////////////////////////
	// Gacha System Packet
	void		SendGachaPurchaseAck( PACKET_GACHA_SS_RESULT_HEADER* pResult, PACKET_GACHA_RESULT_DATA* pDatas, UINT8 ui8ItemCount = 0, INVEN_BUFFER_DB *pItem = NULL );

	//////////////////////////////////////////////////////////////////////////
	// GM 1:1 Chat & Management Packet
	void		SendGMChatUserNoti( UINT8 ui8PenaltyType, INT32	i32PenaltyAmount );

public:
	CUserSession();
	virtual ~CUserSession();

	virtual BOOL			Create();
	virtual BOOL			OnConnect(SOCKET Socket, struct sockaddr_in * pAddr);
	virtual BOOL			OnConnectInit();
	virtual BOOL			OnDisconnect(BOOL bForceMainThread = FALSE);

	virtual INT32			PacketParsing(char * pPacketBuffer, INT32 i32Size);

	void					OnDestroy();

	BOOL					_PacketParsingLogin(		i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled );
	BOOL 					_PacketParsingAuth_server(	i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingShop(		i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingTest(			i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingAsc(			i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingClan(			i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingClanBattle(	i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingServerMessage( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingBase(			i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingLobby(		i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingInventory(	i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingRsIgs(			i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingRoom(			i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL 					_PacketParsingBattle(		i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL					_PacketParsingMedal(		i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL					_PacketParsingSkill(		i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL					_PacketParsingChar(			i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL					_PacketParsingQuickJoin(	i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled );
	BOOL					_PacketParsingMyInfo(i3NetworkPacket* pRecvPacket, i3NetworkPacket* pSendPacket, BOOL &bIsKilled);
	BOOL					_PacketParsingGacha(i3NetworkPacket* pRecvPacket, i3NetworkPacket* pSendPacket, BOOL& bIsKilled);
	BOOL					_PacketParsingGMChat(i3NetworkPacket* pRecvPacket, i3NetworkPacket* pSendPacket, BOOL& bIsKilled);
	BOOL					_PacketParsingCheat(i3NetworkPacket* pRecvPacket, i3NetworkPacket* pSendPacket, BOOL& bIsKilled);
	BOOL					_PacketParsingClanWar(i3NetworkPacket* pRecvPacket, i3NetworkPacket* pSendPacket, BOOL& bIsKilled);

	virtual	BOOL			SendPacketMessage(i3NetworkPacket * pPacket);
	BOOL					S2MOSendEvent( S2MOPacketBase* pPacket );

	void					SendBuyQuestCardResult( INT32 i32Rv, UINT32 ui32GoodsID, INT32 i32ThreadIdx, T_ItemDBIdx TItemWareDBIdx, UINT8 ui8BuyType );	// 임무카드 구입  결과 보내기

	void					SendMyItemInfo( CUserSession * pSession, UINT8 ui8Type );							// 특정유저에게 나의 아이템 정보 전달( 받을유저, 타입 : 1 - 로비, 2 - 룸 )
	
	void					SetLastUserCount( TYPE_ROOM_INFO* pRoom );

	//////////////////////////////////////////////////////////////////////////
	// Clan
	// 클랜 방문자 상태 ON
	void	SetVisitClanPage(UINT32 ui32CID)
	{
		m_ui32VisitCUID			= ui32CID;
		if(m_UserInfoBasic.m_ui32Clanidx != ui32CID)
		{
			m_bVisitClanPageEnter	= true;
		}		
		else
		{
			m_bVisitClanPageEnter	= false;
		}
	}
	// 클랜 방문자 상태 OFF
	void	InitVisitClanPage()
	{
		m_bVisitClanPageEnter	= false;
		m_ui32VisitCUID			= 0;
	}
	// 방문자가 아닌 경우만 검사
	INT32	IsVisitClanNo(UINT32 ui32CID)
	{
		INT32 i32Result = EVENT_ERROR_SUCCESS;
		if(false == m_bVisitClanPageEnter)
		{
			if( m_UserInfoBasic.m_ui32Clanidx != ui32CID)					{ i32Result = SERVER_VALUE_UNKNOWN; }
			if( CLAN_MEMBER_LEVEL_UNKNOWN == m_UserInfoBasic.m_ui32ClanState )	{ i32Result = SERVER_VALUE_UNKNOWN; }
		}

		return i32Result;
	}

	void	SendMedalInfo(void);		// 일반 메달 정보 내려주긔.
	void	SendCurMedalSetInfo(void);	// 진행중인 메달 셋트 정보 내려주긔.
	void	SendComMedalSetInfo(void);	// 완료된 메달 셋트 정보 내려주긔.
	
	MEDAL_GT*	GetMedal_UserDataType(UINT16 ui16MedalIdx, MEDAL_MC_CHECK_POSITION eType);	// 메달 고유 인덱스로 유저의 메달 데이터 가져오기. = 타입별 
	MEDAL_GT*	GetMedal_UserDataAll(UINT16 ui16MedalIdx);	// 메달 고유 인덱스로 유저의 메달 데이터 가져오기. = 일반 메달 전체
	
	CUR_MEDAL_SET*		GetCurMedalSet_UserData(UINT16 ui16MedalSetIdx);	// 메달 셋트 고유 인덱스로 유저의 메달 셋트 데이터 가져오기. = 진행중인 셋트.
	//COM_MEDAL_SET*		GetComMedalSet_UserData(UINT16 ui16MedalSetIdx);	// 메달 셋트 고유 인덱스로 유저의 메달 셋트 데이터 가져오기. = 완료된 셋트

	void	SendGetMedalReward( PACKET_H_GET_MEDAL_REWARD_ACK* pPacketAckH, PACKET_D_GET_MEDAL_REWARD* pPacketAckD );	// 메달 보상 지급 Ack 패킷 보내기.

	void	MedalMissionCheck_Shop( INT32 i32ThreadIdx, UINT8 ui8BuyType, UINT32 ui32ItemID   ); // 메달 미션 체크 - 상점.
	BOOL	CheckMissionBuy( MMC_INFO* pMMCInfo );

	void	RefreshDailyMedalSet();	// 일일 메달 셋트 갱신.
	void	GetEventMedalSet();		// 이벤트 메달 셋트 갱신.
	void	GetDailyMedalSet();		// 일일 메달 셋트 지급.
	INT32	FindCurMedalSetSlot(MEDAL_SET_TYPE eMedalSetType); // 진행중인 메달 셋트 슬롯 찾기
	
	void	RefreshCurMedalSet(UINT8 ui8ArrIdx, MEDAL_SET_TYPE eMedalSetType, UINT16 ui16MedalSetIdx, MEDAL_SET_GET_TYPE eGetType );	// 진행중인 메달 셋트 갱신.
	
	void	SendFailMedalNotifySetting();	// 알리미 메달 등록 실패
	void	UpdateNotufyMedalCount(void);	// 알리미 메달 진행 횟수 업데이트

	void	RecvMedalAllInfoFromTrans( i3NetworkPacket* pPacket );	// Trans서버로 부터 받은 메달 정보를 유저에 셋팅.

	void	SendRepairResult( UINT8 ui8RepairItemCount, S2MO_SHOP_REPAIR_RESULT *pInfo );

	//////////////////////////////////////////////////////////////////////////
	// 캐릭터 주특기
	//////////////////////////////////////////////////////////////////////////
	BOOL	CheckSkillSetting( PACKET_SKILL_SET_REQ* pInfo );
	BOOL	SetCharaSkillClass( UINT8 ui8SlotIdx, UINT8 ui8SkillClass, BOOL bFirst );
	
	void	SendFindDetailUserInfo( T_RESULT TResult, S2_USER_DETAIL_INFO* pUserInfo );
	void	_SetUserDetailInfo( S2_USER_DETAIL_INFO* pstInfo, CUserSession * pSession );
	void	OnRepairItem( S2MO_REPAIR_INFO *pstInfo );

	void	DaliyPlayReward( INT32 i32WorkIdx, BOOL bFreeMove, UINT32 ui32PlayTime );

	
	INT32	GetKillDeathRate();	// 0~100%
	T_RESULT	CheckMercenaryRegist();

	INT32		JoinClanMatchRoom(INT32 i32ChannelIndex, INT32 i32RoomIndex, ROOM_INFO_JOIN* pInfoJoin);

	//////////////////////////////////////////////////////////////////////////
	// 페이레터 쿠폰
	//////////////////////////////////////////////////////////////////////////
	BOOL	_CheckCouponNumber( char* pstrCoupon );

	// 로비 유저 확인
	BOOL	IsInLobby();

	// 실시간 PC방 혜택 제거 / 추가
	void	ChangePCCafeBonus_A( UINT8 ui8PCCafe );

	void	CheckInvenItemByUserInfo(INT32 i32ThreadIdx);

	void	CheckUserInfoByInvenItem(INT32 i32ThreadIdx, T_ItemID TDeletedItemID);

	UINT32	GetChatBlockDate(void) { return m_UserInfoServer.m_ui32ChatBlockDate; }
	void	SetChatBlockDate(UINT32 ui32ChatBlockDate ) { m_UserInfoServer.m_ui32ChatBlockDate = ui32ChatBlockDate; }

	void	SendSeasonRankingClan(void);

	T_TeamID	GetMatchTeamId(void)			{	return m_TTeamID; }
	void	SetMatchTeamId( T_TeamID TTeamID )	{	m_TTeamID = TTeamID; }

	BOOL	IsJoinMatchTeam(void ) 
	{ 
		return m_TTeamID == MATCH_TEAM_INVALID_IDX ? FALSE : TRUE; 
	} 

	BOOL	IsMercenary(void) 
	{ 
		return m_TMerID == MERCENARY_INVALID_IDX ? FALSE : TRUE; 
	} 

	T_MerID	GetMercenaryId(void)			{	return m_TMerID; }
	void	SetMercenaryId( T_MerID TMerID ) {	m_TMerID = TMerID; }

	void	SendClanMatchTeamCount(INT32 i32ChannelNum);

	bool	GetForceLeaveMatchRoom(void)				{	return m_bForceLeaveMatchRoom; }
	void	SetForceLeaveMatchRoom( bool bForce )	{	m_bForceLeaveMatchRoom = bForce; }
	
	void	ResetClanInfo(void);

	void	SetMerPenaltyFlag( bool bPenalty )
	{
		if( !IsMercenary() ) return;

		m_bMerPenalty = bPenalty;
	}

	bool	GetMerPenaltyFlag(void) { return m_bMerPenalty; }

	void	SetMerPenaltyTime( DATE32 *pPenaltyTime )	{ m_dt32MerPenaltyTime = *pPenaltyTime; }
	void	SetMerPenaltyTime( INT32 i32WorkIdx );
	bool	IsMerPenalty(void );

	void	SetMerClass(UINT8 ui8MerClass)	{ m_ui8MerClass = ui8MerClass; }
	void	SetMerRank(UINT8 ui8MerRank)	{ m_ui8MerRank = ui8MerRank; }

	UINT8	GetMerClass(void)				{ return m_ui8MerClass; }
	UINT8	GetMerRank(void)				{ return m_ui8MerRank; }

	BOOL    CheckExistItemInvenByItemID(UINT32 ui32ItemID);
	void	InvenInsertPriority(INVEN_BUFFER *aInvenItem, INVEN_BUFFER_SET * InvenSet, UINT16 ui16ItemCount);
	INT32	CheckExistItemInven(BUY_BASKET* stBasket, UINT8 ui8GoodsCount);

	void	SendGiftNoteByUID(T_UID i64RecvUID, UINT8 ui8MessageSize, TTCHAR *strMessage);
	void	SendBannerURLList(void);
};

#endif
