#if !defined ( _GAME_DEFINE_H__ )
#define	_GAME_DEFINE_H__

#include "../../../Common/CommonSource/WeaponDefine.h"

#define GAME_AGE_LIMITED

#define	GAME_DEVICE_KEYBOARD	0
#define	GAME_DEVICE_MOUSE		1
#define	GAME_DEVICE_JOYSTICK	2

#define BATTLE_CHARA_SET	"Gunplay"


// 메뉴등이 아닌 실제 게임에서 사용되는 Keyboard Input
#define GAME_KEY_MOVE_LEFT					0x00000001
#define GAME_KEY_MOVE_BACKWARD				0x00000002
#define GAME_KEY_MOVE_RIGHT					0x00000004
#define GAME_KEY_MOVE_FORWARD				0x00000008

#define	GAME_KEY_ACTION_JUMP				0x00000010
#define GAME_KEY_ACTION_RELOAD				0x00000020
#define GAME_KEY_ACTION_CROUCH				0x00000040
#define GAME_KEY_ACTION_FOOTSTEP			0x00000080
#define GAME_KEY_ACTION_ATTACK				0x00000100
#define GAME_KEY_ACTION_SUBFUNC				0x00000200				// 마우스 오른쪽
#define GAME_KEY_ACTION_SPECIAL				0x00000400				// 특수 버튼 (폭탄 해체등)
#define GAME_KEY_ACTION_EXTENSION_ACTIVE	0x0000008000000000		// 무기 Extension 활성화 토글
#define GAME_KEY_ACTION_DROP				0x0000010000000000
#define GAME_KEY_ACTION_TURNBACK			0x0000040000000000
#define GAME_KEY_ACTION_WEAPON_EXTENSION	0x0000800000000000		// 'B' 버튼

#define GAME_KEY_CHANGEWEAPON				0x0000020000000000
#define	GAME_KEY_CHANGEWEAPON1				0x00000800
#define GAME_KEY_CHANGEWEAPON2				0x00001000
#define GAME_KEY_CHANGEWEAPON3				0x00002000
#define GAME_KEY_CHANGEWEAPON4				0x00004000
#define	GAME_KEY_CHANGEWEAPON5				0x00008000
#define GAME_KEY_CHANGEWEAPON6				0x00010000
#define GAME_KEY_CHANGEWEAPON_QUICK			0x00020000
#define GAME_KEY_CHANGEWEAPON_PRE			0x00040000
#define GAME_KEY_CHANGEWEAPON_NEXT			0x00080000
#define GAME_KEY_CHANGEWEAPON7				0x0020000000000000
#define GAME_KEY_CHANGEWEAPON8				0x0040000000000000
#define GAME_KEY_CHANGEWEAPON9				0x0080000000000000

#define GAME_KEY_CMD_SHOWSCORE				0x00100000
#define GAME_KEY_CMD_CHAT					0x00200000
#define GAME_KEY_CMD_MOUSESENE_UP			0x00400000
#define GAME_KEY_CMD_MOUSESENE_DOWN			0x00800000
#define GAME_KEY_CMD_RESET					0x01000000
#define GAME_KEY_CMD_VOICECHAT				0x02000000
#define GAME_KEY_CMD_RADIO1					0x04000000
#define GAME_KEY_CMD_RADIO2					0x08000000
#define GAME_KEY_CMD_RADIO3					0x10000000

#define GAME_KEY_FUNC_F2					0x20000000
#define GAME_KEY_FUNC_F3					0x40000000
#define GAME_KEY_FUNC_F4					0x80000000
#define GAME_KEY_FUNC_SCREENSHOT			0x0000000100000000
#define GAME_KEY_FUNC_ESC					0x0000000200000000

#define GAME_KEY_PLUS						0x0000000400000000
#define GAME_KEY_MINUS						0x0000000800000000

#define GAME_KEY_FUNC_CHATSCROLL			0x0000001000000000
#define GAME_KEY_FUNC_TOGGLEMAP				0x0010000000000000

#define GAME_KEY_INGAME_SHOP				0x0001000000000000		// 'Y' 버튼이 디폴트.
#define GAME_KEY_FUNC_VIDEOCAPTURE			0x0002000000000000		// Scroll Lock 버튼이 디폴트
#define	GAME_KEY_SPECIAL_MISSION			0x0004000000000000		// 숫자'0'이 디폴트


// 게임이 아닌 메뉴에서 사용되는  Keyboard Input
#define	GAME_KEY_LEFT						0x00000001
#define GAME_KEY_RIGHT						0x00000002
#define GAME_KEY_UP							0x00000004
#define GAME_KEY_DOWN						0x00000008

#define GAME_KEY_TAB						0x00000010


// 사용하면 안되는 키입니다. 차후에 삭제
#define GAME_KEY_CMD_ALLDEATH				0x0000002000000000
#define GAME_KEY_CMD_ALLEXPLOSION			0x0000004000000000

//FLYCAMERA에서 사용할 PIVOT키 입니다.	CTRL + (J, L, I, K)
#define GAME_KEY_CMD_CAMPIVOT_LEFT			0x0000080000000000
#define GAME_KEY_CMD_CAMPIVOT_RIGHT			0x0000100000000000
#define GAME_KEY_CMD_CAMPIVOT_UP			0x0000200000000000
#define GAME_KEY_CMD_CAMPIVOT_DOWN			0x0000400000000000

// 라디오 메세지 변경키 작업을 위해 추가. 사용될 수 없는 키 입니다.
#define GAME_KEY_CMD_TEMP_1					0x0000800000000000
#define GAME_KEY_CMD_TEMP_2					0x0001000000000000

enum GAME_LAYER
{
	LAYER_WORLD = 0,
	LAYER_CHARAC,
	LAYER_OBJECT,
	LAYER_EFFECT,
	MAX_LAYER_COUNT,
};

//Collisionn Define
#define		CGRP_MASK_WORLD				0x80000000
#define		CGRP_MASK_WORLD_HERO		0x40000000
#define		CGRP_MASK_CHARA_LINE		0x00000001
#define		CGRP_MASK_CHARA_BOUND		0x00000002
#define		CGRP_MASK_OBJECT			0x00000004
#define		CGRP_MASK_ITEM				0x00000010
#define		CGRP_MASK_CAMERA			0x00000020
#define		CGRP_MASK_TEAM_BLUE_ROUGH	0x00000040
#define		CGRP_MASK_TEAM_BLUE			0x00000080
#define		CGRP_MASK_TEAM_BLUE_ATTACK	0x00000100
#define		CGRP_MASK_TEAM_RED_ROUGH	0x00000200
#define		CGRP_MASK_TEAM_RED			0x00000400
#define		CGRP_MASK_TEAM_RED_ATTACK	0x00000800
#define		CGRP_MASK_WORLD_ATTACK		0x00001000
#define		CGRP_MASK_ATTACK			0x00002000
#define		CGRP_MASK_LIGHTVOLUME		0x00004000
#define		CGRP_MASK_GRENADE			0x00008000
#define		CGRP_MASK_TRIGGER			0x00010000


#define		CGRP_WORLD					0
//#define		CGRP_CHARA_LINE				1		// 사용 안함
#define		CGRP_CHARA_BOUND			2		// Network 용 캐릭터 컨트롤러 컬리젼 그룹
#define		CGRP_OBJECT_WHOLE_DYNAMIC	3		// hit, 월드와 반응 ( no character collision)						[Dynamic object]
#define		CGRP_OBJECT					4		// only Character collision(simulate) ( no world collision)			[Static Object, Kinematic Object]
#define		CGRP_ITEM					5		// 물리용 아이템 컬리젼 (no character collision)					[무기]
//#define		CGRP_CAMERA					6
#define		CGRP_TEAM_BLUE				7		// 히트용 캐릭터 컬리젼
#define		CGRP_TEAM_BLUE_ROUGH		8		// 히트용 캐릭터
//#define		CGRP_TEAM_BLUE_ATTACK		9
#define		CGRP_TEAM_RED				10
#define		CGRP_TEAM_RED_ROUGH			11
//#define		CGRP_TEAM_RED_ATTACK		12
#define		CGRP_WORLD_ATTACK			13
//#define		CGRP_ATTACK					14
#define		CGRP_LIGHTVOLUME			15
#define		CGRP_CHARA_LIGHTVOL			16
#define		CGRP_ITEM_WHOLE				17		// 시뮬 & 레이용 아이템
#define		CGRP_OBJECT_DYNAMIC			18		// only world collision ( no character collision)					[Dynamic object]
#define		CGRP_OBJECT_HIT				19		// hit ( no world collision)										[Static Object, Kinematic object]
#define		CGRP_OBJECT_WHOLE			20		// only Character collision(Hit, Simulate) ( no world collision )	[Static object, Kinematic object]
#define		CGRP_DUMMY					21		// 아무런 컬리젼도 안한다.
#define		CGRP_MYCHARA_BOUND			22		// 자신의 캐릭터용 컨트롤러 컬리젼 그룹
#define		CGRP_DECAL					23		// 데칼용 그룹
#define		CGRP_OBJECT_LADDER			24
#define		CGRP_TRIGGER				25
#define		CGRP_OBJECT_ALL_NOHIT		26		// 물리용 오브젝트, 히트컬리젼은 안함
#define		CGRP_ITEM_HIT				27		// Raycast용 아이템													[C5]
#define		CGRP_EFFECT					28		// only world collision, no character collision (현재는 사용 안함)
#define		CGRP_OBJECT_ALL				29		// 히트, 캐릭터, 월드, 오브젝트와 반응								[Dynamic object]
#define		CGRP_TRIGGER_ALL			30		// Trigger : Character, Object, Weapon 모두 Hit될 수 있는 Trigger
#define		CGRP_TREX_BOUND				31		//

#define		CGRP_MASKGROUP_ATTACK			0
#define		CGRP_MASKGROUP_CHARA_RED		1
#define		CGRP_MASKGROUP_CHARA_BLUE		2
#define		CGRP_MASKGROUP_CHARA_ROUGH_RED	3
#define		CGRP_MASKGROUP_CHARA_ROUGH_BLUE	4
#define		CGRP_MASKGROUP_OBJECT			5
#define		CGRP_MASKGROUP_GRENADE			6
#define		CGRP_MASKGROUP_ONLYWORLDHIT		7

#define		CGRP_MASKGROUP_ALLATTACK_BLUE	8	// World, object, Blue Chara
#define		CGRP_MASKGROUP_ALLATTACK_RED	9	// World, object, Red chara
#define		CGRP_MASKGROUP_ENVIRONMENT		10

//////////////////////////////////////////////////////////////////////////
//								Network Event							//
// 작성일 2006. 8. 14
// 작성자 정순구

#define		EVENT_NORESPONSE 0x8000		// 응답을 기달릴 필요가 없는 메시지

enum GAME_EVENT
{
	EVENT_FIRST_RESPWAN_BATTLE = -1, //클라만 사용함..(게임 시작후 첫번째 리스폰)
	// Global event
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EVENT_NOTHING			= 0,		// 아무런 이벤트가 없습니다.

	EVENT_BASE_S			= EVENT_NOTHING,

	EVENT_LOG_IN,						// 로그인
	EVENT_LOG_IN_WEBKEY,				// 웹세션 로그인
	EVENT_LOG_OUT,						// 동일계정 접속 종료 요청
	EVENT_LOG_IN_WAIT,					// 로그인 데기열
	EVENT_DISCONNECT,					// 연결 끊기
	EVENT_ENTER_LOBBY,					// 로비 입장
	EVENT_LEAVE_LOBBY,					// 로비에서 나가기(서버선택으로)
	EVENT_ENTER_PROFILE,				// 1.1 프로필로 이동
	EVENT_LEAVE_PROFILE,
	



	EVENT_ENTER_INFO,					// 내정보보기 입장
	EVENT_LEAVE_INFO,					// 내정보보기 퇴장
	EVENT_ENTER_ROOM,					// 방 입장
	EVENT_LEAVE_ROOM,					// 방 퇴장
	EVENT_ENTER_SHOP,					// Shop으로 가기
	EVENT_LEAVE_SHOP,					// Shop에서 나가기 
	EVENT_GET_NEW_CHALLENGE_CARD,		//
	EVENT_REFRESH_MEDAL,				// 새 메달정보 확인 - NEW 표시 없에기
	EVENT_CONNECT_SERVER_GATEWAY,		// 게이트웨이 서버로 접속
	EVENT_CONNECT_SERVER_GAME,			// 컨넥션 완료 체크
	EVENT_GET_USERINFO,					// 유저 정보 읽기 (arg : UserDB index)
	EVENT_CREATE_CHARA,					// 캐릭터 생성 정보 읽기
	EVENT_SET_CHARA,					// 캐릭터 슬롯 설정
	EVENT_REVERT_CHARA,					// 캐릭터 정보 되돌리기
	EVENT_REVERT_PARTS,					// 캐릭터 정보 되돌리기
	EVENT_REPAIR,					// 캐릭터 정보 되돌리기
	EVENT_DELETE_CHARA,					// 캐릭터 삭제
	//EVENT_GET_SERVER_LIST,
	EVENT_GET_CHANNEL_LIST,				// 채널 리스트 읽어오기
	EVENT_SELECT_CHANNEL,				// 채널 선택
	EVENT_USER_LEAVE,					// 서버 이동 나가기
	EVENT_USER_ENTER,					// 서버 이동 들어오기
	EVENT_ENTER_PW,						// 서버 이동 패스워드
	EVENT_M_BLOCK_ACCOUNT,				// 블럭계정 로그인 시도
	EVENT_SHUTDOWN_TIME_LOGIN,			// 셧다운제 시간 로그인 시도
	
	//하동익 중국 2011-03-07
	EVENT_RECEIVE_REALNAME,				// 실명인증 요청 from Server
	EVENT_RECEIVE_REALNAME_SUCCESS,		// 실명인증 요청 성공 from Server
	
	EVENT_BUY_AUTH,						// ! 사용안함.
	EVENT_ITEM_REPAIR,					// 아이템 수리
	EVENT_REPAIR_EQUIPMENT,				// 착용한 장비 수리

	EVENT_GET_REMAIN_MONEY,				// 보유 포인트, 톨 받기

	EVENT_GAME_GUARD_AUTH2,				// 게임가드 인증키 요청.
	EVENT_GAME_GUARD_AUTH3,				// 게임가드 인증키 요청.
	EVENT_XTRAP_HACK_DETECTED,			// X-Trap hack detecte.

	// GM 명령
	EVENT_KICK_USER,					// 유저 강퇴
	EVENT_EXIT_USER,					// 방 유저 접속종료
	EVENT_BLOCK_USER,					// 유저 블록
	EVENT_DESTROY_ROOM,					// 방 파괴
	EVENT_LOBBY_EXIT_USER,				// 로비 유저 접속종료
	EVENT_GM_EXIT_USER,					// 유저 접속 종료 이벤트

	// 피로도 시스템
	EVENT_FCM_INFO_RETRY,				// 피로도 실명인증 확인 요청
	// 어뷰징(Abusing) 시스템
	EVENT_ABUSING_SET,					// 어뷰징(Abusing) 팝업 대기 완료 서버에 요청
	EVENT_LOADSTAGE,					// 스테이지 로드 및 Code네임 확인하기

	EVENT_EXIT_DAILYRECORD_SEND,		// 유저 일일 전적 보내기

	EVENT_READYBOX_MESSAGE,				// 보급 상자 메세지
	EVENT_TUTORIAL_ROUND_END,			// PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END 처리
	EVENT_KICK_CHEAT_USER,		//치트 유저를 혼내 주기 위해 서버에 알려준다.
	EVENT_BATTLE_ESCAPEBLOCK,				// 미션 위치에 도달했습니다.

	EVENT_GAME_END,						// 정상적인 게임 종료 
	EVENT_COUPON_AWARD,					// 쿠폰 보상 내려받기
	EVENT_USER_DETAIL_INFO,				// 유저 상세정보

	EVENT_PLAYTIME_ALARM,				// 플레이 시간 알림
	EVENT_SHUTDOWNTIME_ALARM,			// 셧다운 시간 알림
	EVENT_CLOSE_PLAYTIME_ALARM,			// 플레이타임 알람 닫기

	EVENT_TIMEOUT_CLIENT,				// TimeOut
	EVENT_TIMEOUT_HOST,					// TimeOut 방장

	EVENT_RUN_PARAM_CHECK,				// 실행 인자 체크하기.

	EVENT_WEBCONTROL_ERROR,				// URL Banner가 Error를 늦게 넘어와 Web배너를 끄는 이벤트
	EVENT_REQ_INGAME_URL,				//인게임 URL 요청

	EVENT_HACKTOOL_DETECTED,			// 해킹툴 탐지

	//----------------------------------------------------------------------//
	//							PB v 1.5 Event        						//
	//----------------------------------------------------------------------//

	// PB 1.0 = GET_MYINFO - 나중에 삭제합니다.
	EVENT_GET_USER_INFO,
	//EVENT_GET_CHARA_INFO,
	EVENT_GET_INVEN_INFO,
	EVENT_GET_SYSTEM_INFO,
	
	EVENT_SET_SERVERTAB_SELECT,
	EVENT_SET_CREATECHARA,				//캐릭터 생성 패킷

	EVENT_CHANGE_CHARA_NICK,

	// 유저 정보 요청 
	// ACK는 3가지 모두 공유하기때문에 PushEventGame은 EVENT_GET_USERINFO_SESSION만 사용
	EVENT_GET_USERINFO_SESSION,
	EVENT_GET_USERINFO_UID,
	EVENT_GET_USERINFO_NICK,	
	EVENT_GET_USERINFO_ROOM,
	EVENT_GET_USERINFO_SUBTASK,

	// 유저 상세 보기
	EVENT_GET_USER_DETAIL_INFO,

	// 일반유저 초대
	EVENT_INVITE_NORMAL_USER,
	EVENT_INVITED_NORMAL_USER_MESSAGE,

	// 주특기
	EVENT_SKILL_CHARA_SET,				// 주특기 캐릭터 설정
	EVENT_SKILL_CLASS_SET,				// 주특기 클래스 습득
	EVENT_SKILL_SLOT_LEARN,				// 주특기 슬롯 배우기
	EVENT_SKILL_SLOT_CANCEL,			// 주특기 슬롯 배우기 취소
	EVENT_SKILL_EQUIP_CHANGE,			// 스킬 장착 및 변경

	EVENT_USER_GUIDE,					// 유저가이드 시스템

	// 피시카페
	EVENT_PCCAFE_CHANGED_STATUS,		// 피시카페 상태변화 (실시간으로 변하는 피시카페혜택때문에 서버에서 아무때나 날라올수있음)

	//------------------------------------------------------------------------------//
	//									GM Option									//
	//------------------------------------------------------------------------------//

	EVENT_GM_AUTHORITY_REQ,					// GM 유저 관리 권한 요청
	EVENT_GM_AUTHORITY_POPUP_OPEN,			// 유저 관리 팝업 열기
	EVENT_GM_AUTHORITY_CHAT_SEND,			// GM 채팅 메세지 보내기
	EVENT_GM_AUTHORITY_CHAT_RECEIVE,		// GM 채팅 메시지 받기
	EVENT_GM_AUTHORITY_USER_PENALTY_APPLY,	// 패널티 적용 요청
	EVENT_GM_AUTHORITY_POPUP_CLOSE,			// 유저 관리 팝업 닫기

	EVENT_GM_CHEATKEY_RANDOMBOX,			// Randombox 치트키 GM 권한이 있어야 함
	//------------------------------------------------------------------------------//
	//									Medal event									//
	//------------------------------------------------------------------------------//

	EVENT_MEDAL_GET_NORMAL_INFO,		// 일반 메달 모든 정보 요청
	EVENT_MEDAL_GET_SET_CUR_INFO,		// 세트 메달(진행중인) 모든 정보 요청
	EVENT_MEDAL_GET_SET_COM_INFO,		// 세트 메달(완료된) 모든 정보 요청
	EVENT_MEDAL_GET_REWARD,				// 메달시스템 보상받기 요청
	EVENT_MEDAL_GET_ALL_REWARD,			// 메달시스템 모든 보상받기 요청
	EVENT_MEDAL_NOTIFY_REGISTER,		// 메달 알리미 등록 요청
	EVENT_MEDAL_COMMAND,				// 메달 커맨드
	EVENT_RANK_UP,

	//------------------------------------------------------------------------------//
	//									QA Command									//
	//------------------------------------------------------------------------------//

	EVENT_QA_COMMAND_INCREASE_KILL_COUNT,	// 킬카운트 증가
	EVENT_QA_COMMAND_INCREASE_PLAY_TIME,	// TS 시간 증가
	EVENT_QA_COMMAND_ENABLE_SOLO_PLAY,		// 1인 플레이 가능화
	EVENT_QA_COMMAND_TELEPORT_SOMEONE,		// 순간이동
	EVENT_QA_COMMAND_DAMAGE_TO_OBJECT,		// 오브젝트 데미지주기
	EVENT_QA_COMMAND_DETECT_HACK_OFF,		// 핵감지 끄기
	EVENT_QA_COMMAND_TIMER_GMPAUSE,			// 타이머로 GM Pause 하기
	EVENT_QA_COMMAND_CONTROL_ROUNDTIME,		// 라운드 시간 제어
	EVENT_QA_COMMAND_CLAN_WAR_MATCHING_POINT,		// 클랜 매치메이킹 - 매치팀 매칭 포인트 조회
	EVENT_QA_COMMAND_CLAN_WAR_MATCH_RESULT,			// 클랜 매치메이킹 - 매치 결과 조회

	//------------------------------------------------------------------------------//
	//									Designation 호칭							//
	//------------------------------------------------------------------------------//

	EVENT_DESIGNATION_GET,				// 호칭 획득
	EVENT_DESIGNATION_CHANGE,			// 호칭 변경
	EVENT_DESIGNATION_RELEASE,			// 호칭 해제

	EVENT_GET_STAGE,

	EVENT_BASE_E,

	EVENT_BASE_NS = EVENT_NORESPONSE + EVENT_BASE_E,
	EVENT_GET_MYOPTION_N,				// 옵션정보 받기
	EVENT_CHATTING_N,					// 채팅 
	EVENT_GET_SLOTINFO_N,				// 슬롯 정보를 얻는다
	EVENT_GET_USERINFO_N,				// 유저 정보를 얻는다
	EVENT_COMMUNITY_RECORD_DB_N,		// 커뮤니티에서 정보 요청
	EVENT_COMMUNITY_RECORD_SESSION_N, 	// 커뮤니티에서 정보 요청
	EVENT_COMMUNITY_RECORD_ALL_N,		// 유저 리스트에서의 전적 전체 정보
	EVENT_COMMUNITY_RECORD_ALL_DB_N,	// 커뮤니티 창에서의 전적 전체 정보
	EVENT_COMMUNITY_RECORD_CLAN_N,		// 커뮤니티에서 클랜정보 요청
	EVENT_OPTION_SAVE_N,				// Option 정보를 저장한다.
	//EVENT_OPTION_SAVE_MACRO_N,			// Option Macro 정보를 저장한다.

	EVENT_BASE_NE,

	//------------------------------------------------------------------------------//
	//									Shop Event									//
	//------------------------------------------------------------------------------//
	EVENT_SHOP_S = 0x100,

	EVENT_ITEM_AUTH,					// 개수제 아이템 인증하기 - 사용하기
	EVENT_ITEM_AUTH_DATA,				// 개수제 아이템 인증하기 데이타있음 - 사용하기
	EVENT_AUTH_DELETE,					// 기간제 아이템 삭제하기 - 사용하기
	EVENT_USE_ITEM_CHECK_NICK,			// 닉네임 중복검사
	EVENT_VIEW_OTHER_USER_ITEM_INFO,	// 상대방 정보 보기(아이템)

	EVENT_REQUEST_GOODS_LIST,			// 상품 리스트 요청
	EVENT_REFRESH_CASH,					// 캐쉬및 포인트 최신화
	
	EVENT_BUY_GOODS,					// 상품 구매
	EVENT_BUY_CHAR_GOODS,				// 케릭터 상품 구매
	EVENT_BUY_EXTEND_GOODS,				// 기간 연장

	EVENT_ENTER_GACHA,					// Gacha 입장
	EVENT_LEAVE_GACHA,					// Gacha 퇴장

	EVENT_ENTER_GACHA_INFO,				// Gacha 정보 요청
	EVENT_END_GACHA_STATE,				// Gacha 정보 최신화
	EVENT_START_GACHA,					// Gacha 시작
	EVENT_END_GACHA,					// Gacha 결과
	EVENT_GACHA_ANNOUNCE,				// Announce 최신화
	EVENT_GACHA_STATE_REQ,				// Gacha 정보 최신화 요청
	EVENT_GACHA_PURCHASE_COUNT,			// 첫 입장시 Purchase count 최신화 요청
	EVENT_ITEM_AUTH_DATA_CHAGNE,		// 사용중인 아이템 설정 변경 요청

	////////////////////////// Gift Shop /////////////////////////////
	EVENT_ENTER_GIFTSHOP,				// 선물 상점 입장
	EVENT_LEAVE_GIFTSHOP,				// 선물 상점 퇴장
	EVENT_FAIL_ENTER_GIFTSHOP,

	EVENT_GET_GIFT_LIST,				// 선물받은 리스트를 서버에 요청.
	EVENT_SEND_GIFT,					// 상품 선물하기
	EVENT_RECEIVE_GIFT,					// 선물을 받았다는 이벤트
	EVENT_AUTH_GIFT_GOODS,				// 상품 선물 받기 쪽지 확인
	EVENT_CHECK_GIFT_GOODS,				// 
	//////////////////////////////////////////////////////////////////

	EVENT_SHOP_E,

	//------------------------------------------------------------------------------//
	//								Inventory Event									//
	//------------------------------------------------------------------------------//
	EVENT_INVEN_S = 0x180,

	EVENT_ENTER_INVENTORY,				// 장비 설정 화면으로 가기
	EVENT_LEAVE_INVENTORY,				// 장비 설정 종료
	EVENT_INVEN_CHANGE_CHARA_EQUIP,			// 캐릭터 무기/파츠 슬롯 정보 변경
	EVENT_INVEN_E,

	//------------------------------------------------------------------------------//
	//									Clan Event									//
	//------------------------------------------------------------------------------//
	EVENT_CLAN_S = 0x280,

	EVENT_CLAN_ENTER,					// 들어가기
	EVENT_CLAN_LEAVE,					// 나가기
	EVENT_CLAN_LIST_CONTEXT,			// 클랜 목록을 위한 Context 요청
	EVENT_CLAN_CREATE,					// 생성하기
	EVENT_CLAN_DUPLICATE_NAME,			// 이름 중복검사 0 - 사용해도 됨,  >0 사용못함 , <0 에러
	EVENT_CLAN_DUPLICATE_ADDRESS,		// 아지트 주소 중복검사 0 - 사용해도 됨,  >0 사용못함 , <0 에러
	EVENT_CLAN_DUPLICATE_MARK,			// 클랜마크 중복검사 0 - 사용해도 됨,  >0 사용못함 , <0 에러
	EVENT_CLAN_JOIN_REQUEST,			// 가입 요청하기
	EVENT_CLAN_JOINCHECK_AUTHORITY,		// 가입권한 체크
	EVENT_CLAN_CANCEL_REQUEST,			// 가입신청 취소.
	EVENT_CLAN_MEMBER_CONTEXT,			// 멤버 목록 정보
	EVENT_CLAN_MEMBER_LIST,				// 멤버 목록 받기
	EVENT_CLAN_REQUEST_CONTEXT,			// 가입신청서 목록정보
	EVENT_CLAN_ACCEPT_REQUEST,			// 가입신청서 수락
	EVENT_CLAN_DENIAL_REQUEST,			// 가입신청서 거절
	EVENT_CLAN_SECESSION,				// 클랜 탈퇴
	EVENT_CLAN_COMMISSIONMASTER,		// 마스터 위임
	EVENT_CLAN_COMMISSIONSTAFF,			// 클랜 스탭 임명
	EVENT_CLAN_COMMISSION_REGULAR,		// 클랜 일반으로 강등
	EVENT_CLAN_DISMISSALMEMBER,			// 클랜원 제명
	EVENT_CLAN_DISSOLUTION,				// 가입 해체
	EVENT_CLAN_NOTICE_MODIFY,			// 공지사항 수정
	EVENT_CLAN_GUIDE_MODIFY,			// 클랜소개 수정
	EVENT_CLAN_ACCEPT_OPTION_MODIFY,	// 클랜 자동/수동 승인 변경
	EVENT_CLAN_MANAGEMENT,				// 클랜 기타 관리 수정
	EVENT_CLAN_CHANGE_MARK,				// 클랜마크 변경
	EVENT_CLAN_MEMBER_INSERT,			// 클랜맴버 추가
	EVENT_CLAN_INVITE,					// 클랜 초대
	EVENT_CLAN_NOTE,					// 클랜 전체쪽지
	EVENT_CLAN_DETAIL_INFO,				// 클랜 세부정보

	EVENT_CLAN_PERSON_PLUS,				// 클랜원 최대치 증가
	EVENT_CLAN_COMMUNITY_LIST,			// 클랜 커뮤니티 목록

	
	EVENT_CLAN_E,

	EVENT_CLAN_NS = EVENT_NORESPONSE + EVENT_CLAN_E,

	EVENT_CLAN_LIST_N,						// 리스트 요청
	EVENT_CLAN_LIST_DETAIL_INFO,			// 클랜 리스트내 상세정보
	EVENT_CLAN_CREATION_INFO,
	EVENT_CLAN_REQUEST_LIST_N,				// 가입신청서 목록보기
	EVENT_CLAN_DETAIL_INFO_N,				// 상세 정보 요청
	EVENT_CLAN_MEMBER_LIST_N,				// 멤버 목록 보기 
	EVENT_CLAN_REQUEST_INFO_N,				// 가입신청서 상세정보 보기
	EVENT_CLAN_INVITE_ACCEPT_N,				// 클랜 초대 수락
	
	EVENT_INVITE_CLAN,					// 클랜 초대하기

	EVENT_CLAN_MATCH_RESULT_CONTEXT,		// 클랜 매치 결과 요청
	EVENT_CLAN_MATCH_RESULT_LIST,			// 클랜 매치 결과 리스트 요청
	EVENT_CLAN_MATCH_RESULT_EMOTION,		// 클랜 매치 결과 이모션 변경

	EVENT_CLAN_WAR_PRESEASON_RANKING,		// 클랜전 지난 시즌 랭킹 확인
	EVENT_CLAN_WAR_ALL_MATCHING_INFO,		// 모든 서버의 클랜 매칭 정보 요청 (카운트)
	EVENT_CLAN_WAR_GET_TEAM_LIST,			// 내 클랜의 매치 팀 목록 받기
	EVENT_CLAN_WAR_CREATE_TEAM,				// 클랜전 팀 생성
	EVENT_CLAN_WAR_JOIN_TEAM,				// 클랜전 팀 입장
	EVENT_CLAN_WAR_LEAVE_TEAM,				// 클랜전 팀 퇴장
	EVENT_CLAN_WAR_CHANGE_OPERATION,		// 클랜전 팀 작전명 변경
	EVENT_CLAN_WAR_CHANGE_MAX_PERSON,		// 클랜전 팀 최대 인원 수 변경
	EVENT_CLAN_WAR_REGISTER_MATCHMAKING,	// 클랜전 팀 매치메이킹 등록
	EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING,	// 클랜전 팀 매치메이킹 등록취소

	EVENT_CLAN_WAR_GET_MERCENARY_LIST,		// 클랜전 용병 리스트 요청
	EVENT_CLAN_WAR_REGISTER_MERCENARY,		// 클랜전 용병 등록.
	EVENT_CLAN_WAR_REMOVE_MERCENARY,		// 클랜전 용병 등록 취소.
	EVENT_CLAN_WAR_SEND_INVITE_MERCENARY,	// 클랜전 용병 초대 요청(매치팀)
	EVENT_CLAN_WAR_RECV_INVITE_MERCENARY,	// 클랜전 용병 초대 받음.(용병)
	EVENT_CLAN_WAR_ACCEPT_INVITE,			// 클랜전 용병 초대 수락.
	EVENT_CLAN_WAR_DENIAL_INVITE,			// 클랜전 용병 초대 거절.
	EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO,	// 클랜전 용병 상세 정보.
	EVENT_CLAN_WAR_RESULT,					// 클랜 매치메이킹 결과 정보 요청
	EVENT_ENTER_TEMP,
	EVENT_LEAVE_TEMP,


	EVENT_CLAN_NE,
	

	//------------------------------------------------------------------------------//
	//									Community									//
	//------------------------------------------------------------------------------//
	EVENT_COMMUNITY_S = 0x300,

	EVENT_INSERT_FRIEND,				// 친구 추가
	EVENT_ACCEPT_FRIEND,				// 친구 수락
	EVENT_DELETE_FRIEND,				// 친구 삭제
	EVENT_INVITE_FRIEND,				// 친구 초대하기
	EVENT_INVITED_MESSAGE,				// 친구 초대받기
	EVENT_SEND_WHISPER,					// 귓속말
	EVENT_SEND_MEGAPHONE,				// 확성기
	EVENT_SEND_WHISPER_FIND_UID,		// Receiver UID로 귓속말
	EVENT_FIND_USER,					// 친구찾기
	EVENT_SEND_NOTE,					// 쪽지 보내기
	EVENT_SEND_NOTE_BY_UID,				// ReceiverUID로 쪽지 보내기
	EVENT_CHECK_READED_NOTE,			// 쪽지 읽음표시
	EVENT_DELETE_NOTE,					// 쪽지 삭제 요청
	EVENT_DELETE_NOTE_ASK,				// 쪽지 삭제 응답
	EVENT_RECEIVE_NOTE,					// 새쪽지 받기
	EVENT_INSERT_BLOCK,					// 차단 추가
	EVENT_DELETE_BLOCK,					// 차단 삭제

	EVENT_COMMUNITY_E,

	//------------------------------------------------------------------------------//
	//									Lobby event									//
	//------------------------------------------------------------------------------//
	EVENT_LOBBY_S = 0x400,

	EVENT_CREATE_NICK_NAME,				// 최초 닉네임 만들기	- 변경이 아니다
	EVENT_GET_FIRST_SUPPLY,				// 초도 보급
	EVENT_CREATE_ROOM,					// 방만들기							( Lobby )
	EVENT_JOIN_ROOM,					// 방에 입장						( Lobby )
	EVENT_JOIN_QUICK,					// 방을 랜덤으로 찾아 입장			( Lobby )
	EVENT_UPDATE_USER_LIST,				// 로비의 유저 리스트가 갱신됬다.	( Lobby )
	// Quest Event
	EVENT_QUEST_CHANGE_ACTIVE_IDX,		// Active Quest Index의 변경 : stageend이후 퀘스트 완료에의해 변경 및 유저가 카드나 카드셋을 변경하여 active index를 변경한경우 호출	
	EVENT_QUEST_FINISH,					// Quest의 완료				 : stage중에 완료되는 퀘스트에대해 서버에 알림
	EVENT_QUEST_BUY_CARD_SET,			// 카드셋 구입
	EVENT_QUEST_DELETE_CARD_SET,		// 카드셋 삭제
	
	// 출석 체크 추가
	//EVENT_ATTENDANCE_INFO,				// 출석체크 정보
	EVENT_ATTENDANCE_CHECK,				// 출석체크
	EVENT_ATTENDANCE_CLEAR,				// 출석체크 완료

	EVENT_LOBBY_E,

	EVENT_LOBBY_NS = EVENT_NORESPONSE + EVENT_LOBBY_E,

	EVENT_LOBBY_REFRESH_N,				// Lobby갱신
	EVENT_LOBBY_PAGE_PREV_N,			// Lobby에서의 페이지 감소
	EVENT_LOBBY_PAGE_NEXT_N,			// Lobby에서의 페이지 증가
	EVENT_GET_ROOM_ADDINFO_N,			//

	EVENT_LOBBY_NE,
	

	//------------------------------------------------------------------------------//
	//									Readyroom event								//
	//------------------------------------------------------------------------------//
	EVENT_ROOM_S = 0x500,

	EVENT_GET_LOBBY_USER,				// 레디룸에서 유저 정보 받기
	EVENT_GET_INVITE_LOBBY_USER,		// 초대목록 받기
	EVENT_INVITE_LOBBY_USER,			// 초대하기
	EVENT_SEARCH_HOST,					// 적합한 방장 후보를 찾는다
	EVENT_TRANSFER_HOST,				// 방장 넘기기
	EVENT_REQUEST_HOST_CHANGE,			// 방장변경 요청
	EVENT_TEAM_CHANGE,					// 진영변경
	EVENT_READY_BATTLE,					// 배틀 준비 완료
	EVENT_START_GAME,					// 게임 시작
	EVENT_BATTLE_COUNTDOWN,				// 배틀 시작 카운트다운
	EVENT_CHANGE_READYROOM_INFO,		// 방 정보 변경 이벤트
	EVENT_TEAM_BALANCE,					// 팀 벨런스 이벤트
	EVENT_GET_USER_EQUIPMENT_INFO,		// 사용자 장비 정보 받기

	EVENT_CHANGE_MAINCHARA_FROM_ROOM,	// 주 캐릭터를 바꾼다.

	EVENT_ROOM_E,

	EVENT_ROOM_NS = EVENT_NORESPONSE + EVENT_ROOM_E,

	EVENT_CHANGE_ROOM_ALLINFO_N,		// 방정보 모두변경					( ReadyRoom )
	EVENT_CHANGE_ROOMINFO_N,			// 방정보 변경						( ReadyRoom )
	EVENT_CHANGE_ROOM_PASSWORD_N,		// 룸패스워드 변경
	EVENT_CHANGE_SLOT_N,				// 슬롯을 열었다가 닫았다 한다. 
	EVENT_CHANGE_TEAM_N,				// 팀변경
	EVENT_CHANGE_OBSERVER_SLOT,			// 옵저버슬롯으로 이동
	EVENT_CHANGE_HIDDEN_SLOT,			// 히든슬롯으로 이동(GM만 가능)
	EVENT_CHANGE_ROOM_OPTION_N,			// 옵션정보 변경 
	EVENT_UNREADY_4VS4_N,				// 4대4 모드 관련 레디 상태 해제	
	EVENT_EQUIPMENT_TEMP_WEAPON_N,		// 임시무기 장착 및 탈착
	EVENT_GET_ROOM_SCORE, //방 스코어 정보를 요청

	EVENT_ROOM_NE,

	//------------------------------------------------------------------------------//
	//									QuickJoin event								//
	//------------------------------------------------------------------------------//
	EVENT_QUICKJOIN_S = 0x600,
	EVENT_QUICKJOIN_START,
	EVENT_QUICKJOIN_SECONDBEST,
	EVENT_QUICKJOIN_E,
	
	//------------------------------------------------------------------------------//
	//									Battle Event								//
	//------------------------------------------------------------------------------//
	EVENT_BATTLE_S = 0x700,

	EVENT_DEATH_BATTLE,					// 죽었습니다.							( Battle )
	EVENT_HOLEPUNCH_OK,					// 방장이 바뀌고 CN이 확인됨을 알려줌..	( Battle )
	EVENT_RESPAWN_BATTLE,				// 부활했습니다.						( Battle )
	EVENT_RESPAWN_BATTLE_FOR_AI,		// 훈련전 AI 리스폰
	
	// 제압모드
	EVENT_RESPAWN_DOMI_DINO,			// 제압 미션 Dino 리스폰
	EVENT_DEATH_DOMI_USER,				// 제압 미션 User Death
	EVENT_WAVE_PRE_START,				// 제압 미션 Wave PreStart
	EVENT_WAVE_END,

	EVENT_RENDEZVOUS_HOLEPUNCH_BATTLE,	// 랑데뷰 서버와 홀펀칭을 합니다.
	EVENT_BATTLE_ENTERANCE_USER,		// 다른 유저가 배틀에 진입한다.
	EVENT_PRESTART_BATTLE,				// UDP를 체크한다.						( Battle )	
	EVENT_START_BATTLE,					// 배틀 시작.							( Battle )	
	EVENT_GIVEUP_BATTLE,				// 배틀을 나간다.						( Battle )
	EVENT_CHANGE_DIFFICULTY_LEVEL,		// 훈련전 난이도 변경 통보
	EVENT_BATTLE_TOUCHDOWN,				// 미션 위치에 도달했습니다.
	EVENT_BATTLE_DEATHBLOW,				// 데스블로우
	EVENT_ARMEDBOMB_BATTLE,				// 폭파 미션에서 폭탄 설치 
	EVENT_DISARMBOMB_BATTLE,			// 폭파 미션에서 폭탄 설치
	EVENT_OBJECT_USE_BATTLE,			// 미션 오브젝트 사용
	EVENT_DAMAGE_DEFENCE_OBJECT_BATTLE,	// 방어 오브젝트 피해중

	EVENT_KICK_VOTE_SUGGEST_RESULT,		// 투표 시작 요청시 승인결과를 먼저 받습니다.
	EVENT_KICK_VOTE_START,				// 강제퇴장 투표 진행을 시작합니다.
	EVENT_KICK_VOTE,					// 투표하기	(Game->Context)
	EVENT_KICK_VOTE_COUNT,				// 투표 진행 표를 받습니다.(Context->Game)
	EVENT_KICK_VOTE_CANCEL,				// 강퇴후보자가 먼저 방을 나갔습니다.
	EVENT_KICK_VOTE_RESULT,				// 투표결과	(Context->Game)
	EVENT_KICK_VOTE_MY_KICKED,			// 게임중 투표 킥당했을때, 강퇴자가 받는메시지...

	EVENT_GETINFO_IGSHOP,				// 로비진입시,일반상점정보요청과 함께 보내고, 받을때는 야전정보요청이외에, UI오픈시에도 버전불일치시 오게됨..
	EVENT_OPEN_IGSHOP,					// 현재 야전상점은 EVENT_ENTER/LEAVE 대신 오픈시에 버전확인만 합니다...

	EVENT_BATTLE_GM_PAUSE,				// GM Pause 기능 on시
	EVENT_BATTLE_GM_RESUME,				// GM Pause 기능 off시
	EVENT_START_COUNTDOWN,				// 라운드 시작 카운트다운.
	EVENT_BATTLE_OPTION_SCOPETYPE,		// 스코프 변경(옵져버들을위한 옵션)


	EVENT_BATTLE_E,

	EVENT_BATTLE_NS = EVENT_NORESPONSE + EVENT_BATTLE_E,

	EVENT_INGAMESHOP_APPLY_ITEM,		// 인게임 상점 무기/장비/아이템 즉시 적용 
	EVENT_SENDPING_N,					// MS에 Ping 정보를 보냅니다.
	EVENT_DAMAGEOBJ_BATTLE_N,			// 파괴미션
	EVENT_HEART_BIT_N,
	EVENT_N_QUEST_END_NOTICE,				//이벤트 종료 노티스
	EVENT_BATTLE_REMAINTIME_N,				// 배틀 스테이지 내에서 현재 라운드 및 배틀 남은시간을 서버에 전송

	// 제압모드
	EVENT_STAGE_JUMP_ROUND_START,			// 스테이지 점프 후 라운드 스타트 이벤트
	EVENT_DOMI_SKILL,
	EVENT_DOMI_CHEAT_MESSAGE,
	EVENT_DOMI_INTERMISSION_READY,
	EVENT_DOMI_INTERMISSION_READY_CANCEL,
	EVENT_DOMI_USE_ITEM,

	EVENT_BATTLE_NE,





	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game -> Context
	EVENT_N = 0x700,
	EVENT_N_NORMAL_END,					//정상 종료
	EVENT_N_ABNORMAL_END,				//비정상 종료 
	//EVENT_ABNORMAL_LOG, //비정상 종료로그

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Context -> Game	
	EVENT_M_S = 0x800,					// 서버에서만 오는 메시지 

	EVENT_M_KICK_USER,					// 강퇴 
	EVENT_M_END_BATTLE,					// 게임 끝 
	EVENT_M_DRIVE_AWAY,					// 동일 계정 접속자 요청에 의한 강제종료
	EVENT_M_ERROR,						// 서버에서 접속을 끊어버리는 에러들
	EVENT_M_DISCONNECTION,				// DISCONNECTIONSUCCESS_ACK에 대한 처리
	EVENT_M_GAMEGUARD_ERROR,			// 게임가드 에러 
	EVENT_M_PRE_ROUNDSTART_BATTLE,		// 라운드 시작 준비
	EVENT_M_ROUNDSTART_BATTLE,			// 라운드 시작
	EVENT_M_ROUNDEND_BATTLE,			// 라운드 끝
	EVENT_M_HOSTLOST_BATTLE,			// 방장변경
	EVENT_M_INVALID_TIMESYNC,			// 서버에서 게임중 로컬타임과 서버타임이 맞지않는다라고 판단
	EVENT_M_DESTROYED_NETWORK,			// 네트워크 연결이 끊기다.
	EVENT_M_SHUTDOWNTIME,				// 셧다운제에 의한 연결 종료

	EVENT_M_CLAN_MEMBER_INFO,			// 커뮤니티클랜맴버 받기
	EVENT_M_CLAN_DEPROTATION,			// 클랜 제명
	EVENT_M_CLAN_CHANGE_GRADE,			// 클랜 멤버 등급 변경 응답

	EVENT_M_RECEIVE_USER_SLOT,			// (ReadyRoom) Slot 정보 받음

	EVENT_INVITED_CLAN_MESSAGE,			// 클랜 초대받기
	
	EVENT_INVEN_GETINFO,				// 예외적으로 인벤이 서버로부터 일방적으로 갱신될때 이 메세지로 처리...
	EVENT_DOMI_ENTER,					// 도미 난입 처리
	EVENT_FORCE_LEAVE_MATCHROOM,			// 클랜매치 - 매치되어 레디룸 대기중일때, 방장 퇴장시 팀원 전체도 퇴장

	EVENT_INVEN_REFRESH,				//인벤정보 최신화 
	EVENT_GET_INGAME_URL,				//인게임 URL 정보 얻은 뒤

	EVENT_M_E,

};


//P2P Battle에서 사용하는 이벤트 입니다. 상위 1바이트를 Commond로 사용합니다. 
//중요도가 좀 떨어지는 메시지에서 사용합니다.
#define EVENT_BATTLE_MASK			0xFF00000
#define EVENT_BATTLE_KILLCOUNT		0x0200000
#define EVENT_BATTLE_DEATHCOUNT		0x0300000

inline
REAL32	DEVIATION_TIME( REAL32 angle)
{
	return 1.0f; // + log10f( angle);
}

enum CHARA_HIT_PART_SIMPLE
{
	CHPS_UNDKNOWN = -1,
	CHPS_BODY,
	CHPS_ARM,
	CHPS_HAND,
	CHPS_LEG,
	CHPS_FOOT,
	CHPS_COUNT
};

enum ChallengeBulletType
{
	CBT_START = 0,

	CBT_NONE = CBT_START,
	CBT_CHALLENGING,
	CBT_COMPLATE,

	CBT_MAX
};

struct GAMETOCONTEXT_INFO_WEAPON_THROW
{
	T_ItemID			_ItemID = 0;
	WEAPON::ATTACK_TYPE	_AttackType = WEAPON::ATTACK_TYPE_L1;

	VEC3D *				_pPos = nullptr;
	VEC3D *				_pNormal = nullptr;
	INT32				_nBoundType = 0;
	UINT8				_nAddStateFlag = 0;
	UINT8				_ucRemainingTime = 0;
	bool				_bExtension = false;
	VEC3D *				_AttachedObjPos = nullptr;

	GAMETOCONTEXT_INFO_WEAPON_THROW(T_ItemID itemID, VEC3D * pPos, UINT8 State)
		: _ItemID(itemID), _pPos(pPos), _nAddStateFlag(State)
	{
	}
};


#endif
