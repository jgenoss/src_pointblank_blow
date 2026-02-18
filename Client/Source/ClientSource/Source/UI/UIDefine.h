#if !defined( __UI_DEFINE_H__)
#define __UI_DEFINE_H__

#include "CSI_Stage.h"

//Framework Login Phase 생성합니다. 
enum	LOGIN_UIPHASE
{
	LOGIN_UIPHASE_START = -1,

	LOGIN_UIPHASE_LOGIN,
	LOGIN_UIPHASE_NATION,

	LOGIN_UIPHASE_MAX
};

// Framework에서 주는 Event들입니다.
// 각 UI에서 처리됨.
enum UIEVENT
{
	UIEVENT_START = I3_EVT_USER + 100100,

	UIEVENT_CHANGE_AVATAR,
	UIEVENT_CHANGE_WEAPON,
	UIEVENT_CHANGE_PARTS,	// 미리보기
	UIEVENT_EQUIP_PARTS,	// 장비
	UIEVENT_UPDATE_CHARA_SLOT,		// 슬롯 업데이트
	UIEVENT_UPDATE_ABILITY,			// Ability update
	UIEVENT_SMENU_CHANGE_ITEM,		// Side Menu Equipment update

	UIEVENT_OPEN_POPUP,		// Popup을 Open
	UIEVENT_CLOSE_POPUP,	// Popup을 Close
	UIEVENT_SET_FOCUS_POPUP,
	UIEVENT_KILL_FOCUS_POPUP,

	UIEVENT_NONEBATTLE_MAX,

	UIEVENT_BATTLE,
	UIEVENT_BATTLE_WEAPONCHANGE,		// 무기 교체창에서 무기 교환
	UIEVENT_BATTLE_WEAPONCHANGE_CANCEL,
	UIEVENT_BATTLE_MAX
};

// 바탕이 되는 Phase
// 반드시 한 Phase를 유지하고 있어야 한다.
enum	UIPHASE
{
	UIPHASE_START = -1,
	
	UIPHASE_SERVER_CHANNEL_LIST,
	UIPHASE_CREATE_CHARA,
	UIPHASE_LOBBY,
	UIPHASE_CLANLOBBY,
	UIPHASE_READY_ROOM,

	UIPHASE_MAX
};

// Sub Phase
// 언제든지 호출되는 Phase 형태
enum	UISUBPHASE
{
	USP_NONE = -1,
	USP_SHOP,
	USP_INVENTORY,
	USP_GACHA,
	USP_GIFTSHOP,
	USP_PROFILE,
	USP_TEMP,

	USP_MAX
};

// 순서 중요합니다.
// depth에 따라 rendering 순서가 결정되므로 값이 높을수록 상위에 위치합니다.
enum	UIFLOATWIN
{
	UFW_CHARACTER = 0,		///<< Character View
	UFW_MINIPROFILE,		///<< 좌측하단 개인정보창
	UFW_READYROOMINFO,		///<< ReadyRoom 게임정보창
	UFW_LOBBYMAPINFO,		///<< Room Info 창
	UFW_CHAT,				///<< chatting window
	UFW_USERLIST,			///<< UserList
	UFW_PLAYTIMEALARM,		///<< 플레이타임 알림

	UFW_CHANNEL_LIST,		///<< Channel List

	// 배너입니다.
	UFW_SCBANNER,			///<< Server & Channel banner
	UFW_LOBBYBANNER,		///<< Lobby Banner
	UFW_NEWANDHOT,			///< New&Hot
	UFW_CLANLOBBY,
	UFW_MERCENARY,			///<< 용병
	UFW_MISSIONBOARD,		///<< 미션카드 플로우
	UFW_READYROOM_USERINFO,	///<< 레디룸 유저 인포(V10)
	UFW_READYROOM_CHAT,		///<< 1.0은 레디룸 Chat을 따로 씁니다.
	UFW_COMMUNITY,			///커뮤니티

	UFW_MAX
};

// Popup Window
enum	UIPOPUPWIN
{
	UPW_NULL = -1,
	UPW_START = 0,
	UPW_OPTION = UPW_START,
	
	UPW_BUYITEM,			// BuyItem

	UPW_SHOPEXTENSION,
	UPW_COUPONLIST,			// Coupon List
	UPW_CREATEROOM_EX,		// Create Room Ex

	UPW_MAILBOX,			// MailBox
	UPW_MAILREV,			// Mail ReceiveMail Popup
	UPW_QUICKJOINOPT,		// Quick Join Option
	UPW_USERPROFILE,		// User Profile
	UPW_INVITELOBBYUSER,	// invite lobby user
	UPW_QUERY_PASSWORD,		// Query Pass word
	UPW_REPORT,				// Report 신고하기
	UPW_CLAN_CREATE,		// Clan 생성하기
	UPW_CLAN_NAME_CHECK,    // Clan Name 중복 체크 화면 추가  2011-06-29 : 김재진
	UPW_CLAN_WARNING,		// Clan 생성시 경고창로 추가  2011-06-29 : 김재진
	UPW_CLAN_JOIN,          // Clan 가입창 뜨기로 추가    2011-06-29 : 김재진
	UPW_LINK_SHOP,
	UPW_CREATENICKNAME,		// Nick 생성
	UPW_RANK_UP,			// RankUp 알림창
	UPW_PCCAFE,				// PC방 알림창
	UPW_ADDBLOCK,			// Add Block list
	UPW_PLAYWITH,			// Play With,

	// 여기부터 -->
	UPW_QUERY_ITEM,			// 아이템 사용자 문의 팝업
	UPW_RESULT_USEITEM,			// 아이템 사용 결과 팝업

	UPW_CHANGENICK,				//	닉네임 변경
	UPW_CHANGE_COLOR_NICK,		// 컬러 닉네임 설정 or ClanName
	UPW_CHANGE_CROSSHAIR_COLOR,		//	컬러 크로스헤어
	UPW_DISGUISE_RANK,			//	위장 계급
	UPW_CHANGE_CLAN_MARK,		// 클랜마크 변경
	UPW_COLOR_FIRE_MUZZLE_EFFECT,		// 컬러 총구 화염 변경권
	// --> 여기까지 순서 바꾸면 안됩니다. swoongoo

	UPW_COUPON_INPUT,

	UPW_DISCOUNT_COUPON,	// 할인 쿠폰

	UPW_EVENT,				// 이벤트아이템

	UPW_EXIT_POPUP,			// (New)게임 종료창
	UPW_TS_REWARD,	//TS 보상

	UPW_VISIT_EVENT,			// 기간 내 1회 지급		
	UPW_ATTENDANCE_CHECK,		// 출석 체크
	UPW_ATTENDANCE_COMPLETE,	// 출석 체크 완료

	UPW_PROFILE,

	UPW_CLANHONOR,	//클랜명예전 이팩트

	UPW_CLAN_NON_MEMBER,
	UPW_CLAN_MEMBER,

	UPW_MEDAL_REWARD,
	UPW_MEDAL_QUICK_REWARD,	// 빠른 메달 보상

	UPW_BREAK_INTO_GAME, //난입 스코어
	UPW_WEB_BOX,

	UPW_DOMI_SCORE,
	UPW_DOMI_ROUND,
	UPW_DOMI_ENTER,
	UPW_GACHA_WEAPON_DETAIL,
	UPW_GACHA_RESULT,
	UPW_GACHA_HELP_GUIDE,		// 뽑기상점2 Help 가이드
	UPW_MAILREP,			// Mail SendPopup after Receive
#if legacy_gui_scene
	UPW_ERROR,				// Error
	//UPW_MAILREP,			// Mail SendPopup after Receive
	UPW_ABUSE,				// Abuse 알림창
	UPW_CLAN_MARKCHANGE_OLD,// 클랜관리 클랜마크 예전 GUI 
	UPW_CHANGE_CLAN_NAME,		// 클랜관리 이름 변경
#endif

	UPW_QUICK_JOIN,				// 빠른입장 메인
	UPW_QUICK_JOIN_SECOND_BEST, // 빠른입장 차선책

	UPW_INVEN_DELETE_ITEM_NEW,	// 인벤 아이템 삭제 신규 UI


	// Key : GM 강화 기능 추가
	UPW_GM_OPTION,				// GM 강화 기능 (GM)
	UPW_GM_CHATBOX,				// GM 채팅창 (유저)

	UPW_MEDAL_ITEM,				// 매달 구매제한 추가 
	
	UPW_GIFT_SEND,				// 선물 보내기 팝업

	UPW_SKILLOPEN_ITEM,			// 스킬 오픈 아이템 
	UPW_SKILLOPEN_RESULT,		// 스킬 오픈 아이템 결과

	UPW_CLAN_SEASON_OVERALL_RANKING,	// 클랜 - 시즌종료 전체 순위
	UPW_CLAN_SEASON_INDIVISUAL_RANKING,	// 클랜 - 시즌종료 개인 순위

	// Key : 트위치 티비 기능 추가
	UPW_TWITCHLOGIN,			//트위치 티비 로그인 팝업.
	UPW_TWITCHOPTION,			//트위치 티비 옵션 팝업.
	
	UPW_USERACTION,				// 1.1 기능 추가 : User Interaction Popup
	UPW_SENDMAIL,				// 1.1 기능 추가 : Send Mail
	UPW_BASKET,					// 1.1 기능 추가
	UPW_QUERY_USEITEM,			// 1.1 기능 추가 
	UPW_REPAIR,			// 1.1 기능 추가 
	
	UPW_MISSIONINFO,			// 미션카드 팝업 (카드정보)
	UPW_MISSIONSHOP,			// 미션카드 팝업 (카드구매)
	UPW_REWARDITEM,				// 아마도 미션카드 보상팝업일듯..
	UPW_REWARDITEM_FINALITEM,	// 1.0에서는 통합되어 있지 않고 나누어 사용합니다. 1.1도 동일하게 갈 듯 싶습니다.
	UPW_REWARDITEM_FINALMASTER,	// 나누어 진 최종 팝업

	UPW_SEND_CLANMAIL,			// 클랜원에게 전체메세지 보내기
	UPW_COMMUNITY,				// 커뮤니티
	UPW_USERSEARCH,				// 1.0 유저 찾기 팝업

	UPW_MONTH_ATTENDANCE,		// 31일 출석체크 팝업
	UPW_DESIG_SELECT,			// 호칭 선택 팝업
	UPW_DORMANT_EVENT,			// 휴면계정 이벤트 팝업

	UPW_MAX
};

enum UITOOLTIP
{
	UTT_NONE = -1,
	//UTT_ROOMINFO,

	UTT_MAX
};

enum SS_MAINTYPE
{
	SS_MAINTYPE_BEGINNER = 0,		// 초보 서버
	SS_MAINTYPE_BEGINNER2,			// 초보 서버 2
	SS_MAINTYPE_NORMAL,				// 일반 서버
	SS_MAINTYPE_EXPERT,				// 고수 서버
	SS_MAINTYPE_EXPERT2,			// 고수 서버 2
	SS_MAINTYPE_CLAN,				// 클랜 서버
	SS_MAINTYPE_CHAMPIONSHIP,		// 대회 서버
	SS_MAINTYPE_NATIONAL,			// 중남미 국가추가
	SS_MAINTYPE_MIDDLE,				// 중수 서버

	SS_MAINTYPE_EVENT0,				// 이벤트 서버
	SS_MAINTYPE_EVENT1,
	SS_MAINTYPE_EVENT2,
	SS_MAINTYPE_EVENT3,
	SS_MAINTYPE_EVENT4,
	SS_MAINTYPE_EVENT5,
	SS_MAINTYPE_EVENT6,
	SS_MAINTYPE_EVENT7,
	SS_MAINTYPE_EVENT8,
	SS_MAINTYPE_EVENT9,

	SS_MAINTYPE_GWARNET_SILVER,	// 인니 Sliver	// revision 40858
	SS_MAINTYPE_GWARNET_GOLD,	// 인니 골드
	SS_MAINTYPE_GWARNET_CLAN,	// 인니 클랜

	SS_MAINTYPE_MAX,
};

enum SS_SUBTYPE
{
	SS_SUBTYPE_CATEGORY_TEXT = 0,			//채널 제목
	SS_SUBTYPE_CATEGORY_SLOT_TEXT,			//채널 안 슬롯
	SS_SUBTYPE_CATEGORY_SLOT_FULL_TEXT,		//채널 안 슬롯 가득 찼을 경우
	SS_SUBTYPE_CATEGORY_READY_SLOT_TEXT,	//로비방에 채널 제목

	SS_SUBTYPE_COUNT,
};



enum UI_BGM
{
	UI_BGM_NONE = -1,
	UI_BGM_0,

	UI_BGM_MAX
};

enum UI_MODE_TYPE
{
	UMT_NONE = -1,
	UMT_BOMBMISSION		= 0,	// 0폭파미션
	UMT_ANNIHILATION	= 1,	// 1 섬멸전
	UMT_DEATHMATCH		= 2,	//2 단체전
	UMT_DESTROY			= 3,	//3 파괴미션
	UMT_AI				= 4,	//4 훈련전
	UMT_ESCAPE			= 5,	//5 탈출미션
	UMT_DEFENCE			= 6,	//6 방어미션
	UMT_SUDDENDEATH		= 7,	//7 서든데스
	UMT_HEADHUNTER		= 8,	//8 헤드헌터
	UMT_CLANMATCH		= 9,	//9 클랜전
	UMT_KNUCKLE,		//10 너클전		// UI만 있는 것으로 실제 값으로 사용되면 안됩니다.
	UMT_SNIPER,			//11 저격전
	UMT_SHOTGUN,		//12 샷건전
	UMT_TUTORIAL		= 13,	//13 튜토리얼
	UMT_DOMINATION		= 14,	//14 제압미션
	UMT_CCMODE			= 15,	//15 CC모드(공룡vs인간)
	UMT_CONVOY			= 16,	//16 정규 탈취 모드 
	UMT_RUN_AWAY		= 17,	//17 겨울모드
	UMT_CHAOS			= 18,	//18 카오스모드
	UMT_SPACE			= 19,   //19 스페이스모드
	UMT_MADDNES			= 20,   //메드니스모드
	UMT_HARDCORE_BOMB	= 21,   //하드코어 폭파
	UMT_HARDCORE_CONVOY = 22,   //하드코어 탈취
};

enum UI_CREATEROOMMODE_TYPE
{
	UMT_CR_NONE = -1,
	UMT_CR_BOMBMISSION		= 0,	// 0폭파미션
	UMT_CR_ANNIHILATION		= 1,	// 1 섬멸전
	UMT_CR_DEATHMATCH		= 2,	//2 단체전
	UMT_CR_DESTROY			= 3,	//3 파괴미션
	UMT_CR_AI				= 4,	//4 훈련전
	UMT_CR_ESCAPE			= 5,	//5 탈출미션
	UMT_CR_DEFENCE			= 6,	//6 방어미션
	UMT_CR_SUDDENDEATH		= 7,	//7 서든데스
	UMT_CR_HEADHUNTER		= 8,	//8 헤드헌터
	UMT_CR_CLANMATCH		= 9,	//9 클랜전
	UMT_CR_KNUCKLE,					//10 너클전		// UI만 있는 것으로 실제 값으로 사용되면 안됩니다.
	UMT_CR_SNIPER,					//11 저격전
	UMT_CR_SHOTGUN,					//12 샷건전
	UMT_CR_TUTORIAL			= 13,	//13 튜토리얼
	UMT_CR_DOMINATION		= 14,	//14 제압미션
	UMT_CR_CCMODE			= 15,	//15 CC모드(공룡vs인간)
	UMT_CR_SEASON			= 16,	//16 시즌 이벤트
	UMT_CR_CONVOY			= 17,	//17 정규 탈취 모드 
	UMT_CR_RUN_AWAY			= 18,	//18 겨울모드
	UMT_CR_MADNESS			= 19,
	UMT_CR_CHAOS			= 20,	//카오스모드
	UMT_CR_DIEHARD			= 21,   //다이하드모드
};

enum UI_LIMIT_MODE
{
	ULM_NONE = -1,
	ULM_KNIFE,			// 칼전
	ULM_PISTOL,			// 권총전
	ULM_ROCKET,			// RPG전
	ULM_SNIPER,			// 스나전
	ULM_SHOTGUN,		// 샷건전
	ULM_KNUCKLE,		// 너클전
};

enum USERLIST_TAB
{
	USERLIST_TAB_NONE = -1,
	USERLIST_TAB_CURRENT,		// 현재는 로비입니다.
	USERLIST_TAB_FRIEND,		//	친구 
	USERLIST_TAB_CLAN,			//	클랜
	USERLIST_TAB_SEARCH,		//	유저 검색

	USERLIST_TAB_BLOCK_V11,		// UI 1.1 을 위한 Tab
	
	USERLIST_TAB_MAX
};

enum UI_EVENT_MARK
{
	UIEM_NONE = -1,
	UIEM_PCCAFE,
	UIEM_EVENT,
	UIEM_ITEM,
	UIEM_PCCAFE_PLUS,
	UIEM_PCCAFE_PLUS_SYMBOL,
	UIEM_GARENA_GCA,

	UIEM_MAX
};

struct UIEVENT_OPEN_POPUP_INFO
{
	UIPOPUPWIN		_Type;
	void *			_pParam1;
	void *			_pParam2;
};

enum PhaseState
{
	PS_START = 0,

	PS_IDLE = PS_START,
	PS_ENTRANCE_START,
	PS_ENTRANCING,
	PS_ENTRANCE_END,
	PS_PLAYING,
	PS_EXIT_START,
	PS_EXITING,
	PS_EXIT_END,

	PS_MAX
};

// 내구도 색상
#define REPAIR_COLOR_BEST	(0xff0ABE5A) // 최적 상태
#define REPAIR_COLOR_NORMAL (0xff96DC5A) // 보통 상태
#define REPAIR_COLOR_BAD	(0xffF0780F) // 악화 상태
#define REPAIR_COLOR_BROKEN	(0xffD20000) // 불량 상태







#define UI_LOBBY_ROOMLIST_VIEWCOUNT 10

#define UI_LOBBY_BUTTON_COUNT 5



enum UI_LOBBY_ROOM_STATE
{
	ULRS_NONE = -1,
	ULRS_ADMISSION,		// 입장 가능
	ULRS_PLAY,			// 게임중
	ULRS_NO_ADMISSION,	// 입장 불가
};

struct SORT_ORDER_INFO
{
	INT32	_nType;
	bool	_bAscending;
};

struct LOBBY_ROOM_INFO
{
	bool				_bModify;
	ROOM_INFO_BASIC 	_RoomInfo;
};


#ifndef NC_BUILD
#include "ItemChecker.h"

namespace
{
	void AutoCheckEquipments()
	{
		static bool has_been_visited_lobby = false;
		if(has_been_visited_lobby==false)
		{
			CheckerEquipments()();
			has_been_visited_lobby = true;
		}
	}
}
#endif

namespace SortRoom
{
	enum SORT_LIMIT_TYPE
	{
		SLT_NONE = 0,
		SLT_SHOTGUN,
		SLT_SNIPER,
		SLT_PISTOL,
		SLT_KNUCKLE,
		SLT_KNIFE,
	
		SLT_MAX
	};

	static char priority[129];
	void InitPriority();

	SortRoom::SORT_LIMIT_TYPE GetSortLimitType(UINT8 weaponFlag);

	INT32 value_compare(INT32 lValue, INT32 rValue);

	INT32 priority_compare(const TTCHAR* lhs, const TTCHAR* rhs);

	INT32 SortbyNumber(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);

	INT32 SortbyRoomName(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);
	INT32 SortbyMode(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);
	
	INT32 SortbyUIMode(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);

	INT32 SortbyStageName(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);

	INT32 SortbyRoomState(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);

	INT32 SortbyPerson(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);
}
#endif
