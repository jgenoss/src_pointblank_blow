#include "pch.h"
#include "GameContextScript.h"

#if defined( USE_LUA_UI)

bool	GameContextScript::InitGameDefine(lua_State *L)
{
	bool bReturn = false;
	// Stage Type
	bReturn = i3Lua::AddEnumToLua(L, "STAGE_TYPE",
		"NA", STAGE_TYPE_NA,
		"DEATHMATCH", STAGE_TYPE_DEATHMATCH,
		"BOMB", STAGE_TYPE_BOMB,
		"DESTROY", STAGE_TYPE_DESTROY,
		"ANNIHILATION", STAGE_TYPE_ANNIHILATION,
		"DEFENCE", STAGE_TYPE_DEFENCE,
		"SUDDENDEATH", STAGE_TYPE_SUDDEN_DEATH,
		"BOSS", STAGE_TYPE_ESCAPE,
		"HEADHUNTER", STAGE_TYPE_HEAD_HUNTER,
		"HEADKILLER", STAGE_TYPE_HEAD_KILLER,
		"MAX", STAGE_TYPE_MAX,
		0);

	if( bReturn == false)
	{
		I3FATAL("Add Enum to Lua failed");
		return false;
	}

	// Game Event
	// Global event
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bReturn = i3Lua::AddEnumToLua(L, "pbGameEvent",
	"NOTHING",					EVENT_NOTHING,		// 아무런 이벤트가 없습니다.
	"ENTER_LOBBY",				EVENT_ENTER_LOBBY,					// 로비 입장
	"LEAVE_LOBBY",				EVENT_LEAVE_LOBBY,					// 로비에서 나가기(서버선택으로)
	"ENTER_INVENTORY",			EVENT_ENTER_INVENTORY,				// 장비 설정 화면으로 가기
	"LEAVE_INVENTORY",			EVENT_LEAVE_INVENTORY,				// 장비 설정 종료
	"ENTER_INFO",				EVENT_ENTER_INFO,					// 내정보보기 입장
	"LEAVE_INFO",				EVENT_LEAVE_INFO,					// 내정보보기 퇴장
	"ENTER_SHOP",				EVENT_ENTER_SHOP,					// Shop으로 가기
	"LEAVE_SHOP",				EVENT_LEAVE_SHOP,					// Shop에서 나가기 
	"GET_NEW_CHALLENGE_CARD",	EVENT_GET_NEW_CHALLENGE_CARD,		//
	"REFRESH_MEDAL",			EVENT_REFRESH_MEDAL,				// 새 메달정보 확인 - NEW 표시 없에기
	"CONNECT_SERVER_GATEWAY",	EVENT_CONNECT_SERVER_GATEWAY,		// 게이트웨이 서버로 접속
	"CONNECT_SERVER_GAME",		EVENT_CONNECT_SERVER_GAME,				// 컨넥션 완료 체크
	"GET_USERINFO",				EVENT_GET_USERINFO,					// 유저 정보 읽기 (arg : UserDB index)
	"GET_SERVER_LIST",			EVENT_GET_SERVER_LIST,
	"GET_CHANNEL_LIST",			EVENT_GET_CHANNEL_LIST,				// 채널 리스트 읽어오기
	"SELECT_CHANNEL",			EVENT_SELECT_CHANNEL,				// 채널 선택
	"LOG_IN",					EVENT_LOG_IN,						// 로그인
	"LOG_IN_WEBKEY",			EVENT_LOG_IN_WEBKEY,				// 웹세션 로그인
	"LOG_OUT",					EVENT_LOG_OUT,						// 동일계정 접속 종료 요청
	"USER_LEAVE",				EVENT_USER_LEAVE,					// 서버 이동 나가기
	"USER_ENTER",				EVENT_USER_ENTER,					// 서버 이동 들어오기
	"ENTER_PW",					EVENT_ENTER_PW,						// 서버 이동 패스워드
	"ITEM_AUTH",				EVENT_ITEM_AUTH,					// 개수제 아이템 인증하기 - 사용하기
	"ITEM_AUTH_DATA",			EVENT_ITEM_AUTH_DATA,				// 개수제 아이템 인증하기 데이타있음 - 사용하기
	"AUTH_DELETE",				EVENT_AUTH_DELETE,					// 기간제 아이템 삭제하기 - 사용하기
	"USE_ITEM_CHECK_NICK",		EVENT_USE_ITEM_CHECK_NICK,			// 닉네임 중복검사
	"VIEW_OTHER_USER_ITEM_INFO",EVENT_VIEW_OTHER_USER_ITEM_INFO,	// 상대방 정보 보기(아이템)

	"REQUEST_GOODS_LIST",		EVENT_REQUEST_GOODS_LIST,			// 상품 리스트 요청
	"BUY_GOODS",				EVENT_BUY_GOODS,					// 상품 구매
	"BUY_AUTH",					EVENT_BUY_AUTH,						// ! 사용안함.
	"AUTH_GIFT_GOODS",			EVENT_AUTH_GIFT_GOODS,				// 상품 선물하기
	"ITEM_REPAIR",				EVENT_ITEM_REPAIR,					// 아이템 수리
	"REQ_GIFT_LIST",			EVENT_REQ_GIFT_LIST,				// 선물받은 리스트를 서버에 요청.
	"GET_GIFT",					EVENT_GET_GIFT,						// 선물을 받았다는 이벤트
	"CHECK_GIFT_GOODS",			EVENT_CHECK_GIFT_GOODS,				// 상품 선물 받기 쪽지 확인

	"GET_REMAIN_MONEY",			EVENT_GET_REMAIN_MONEY,				// 보유 포인트, 톨 받기

//	"RENDEZVOUS_CONNECT",		EVENT_RENDEZVOUS_CONNECT,			// 랑데뷰 서버에 접속한다.
//	"RENDEZVOUS_NAT_STATE",		EVENT_RENDEZVOUS_NAT_STATE,			// 게임 서버에 NAT 상태를 알려준다.
//	"RENDEZVOUS_HOLEPUNCH",		EVENT_RENDEZVOUS_HOLEPUNCH,			// 게임 서버에 홀펀칭 완료 상태를 알려준다.

	//	Clan
	"CLAN_ENTER",				EVENT_CLAN_ENTER,					// 들어가기
	"CLAN_LEAVE",				EVENT_CLAN_LEAVE,					// 나가기
	"CLAN_LIST_CONTEXT",		EVENT_CLAN_LIST_CONTEXT,			// 클랜 목록을 위한 Context 요청
	"CLAN_CREATE",				EVENT_CLAN_CREATE,					// 생성하기
	"CLAN_DUPLICATE_NAME",		EVENT_CLAN_DUPLICATE_NAME,			// 이름 중복검사 0 - 사용해도 됨,  >0 사용못함 , <0 에러
	"CLAN_DUPLICATE_ADDRESS",	EVENT_CLAN_DUPLICATE_ADDRESS,		// 아지트 주소 중복검사 0 - 사용해도 됨,  >0 사용못함 , <0 에러
	"CLAN_DUPLICATE_MARK",		EVENT_CLAN_DUPLICATE_MARK,			// 클랜마크 중복검사 0 - 사용해도 됨,  >0 사용못함 , <0 에러
	"CLAN_JOIN_REQUEST",		EVENT_CLAN_JOIN_REQUEST,			// 가입 요청하기
	"CLAN_JOINCHECK_AUTHORITY", EVENT_CLAN_JOINCHECK_AUTHORITY,		// 가입권한 체크
	"CLAN_CANCEL_REQUEST",		EVENT_CLAN_CANCEL_REQUEST,			// 가입신청 취소.
	"CLAN_MEMBER_CONTEXT",		EVENT_CLAN_MEMBER_CONTEXT,			// 멤버 목록 정보
	"CLAN_MEMBER_LIST",			EVENT_CLAN_MEMBER_LIST,				// 멤버 목록 받기
	"CLAN_REQUEST_CONTEXT",		EVENT_CLAN_REQUEST_CONTEXT,			// 가입신청서 목록정보
	"CLAN_ACCEPT_REQUEST",		EVENT_CLAN_ACCEPT_REQUEST,			// 가입신청서 수락
	"CLAN_DENIAL_REQUEST",		EVENT_CLAN_DENIAL_REQUEST,			// 가입신청서 거절
	"CLAN_SECESSION",			EVENT_CLAN_SECESSION,				// 클랜 탈퇴
	"CLAN_COMMISSIONMASTER",	EVENT_CLAN_COMMISSIONMASTER,		// 마스터 위임
	"CLAN_COMMISSIONSTAFF",		EVENT_CLAN_COMMISSIONSTAFF,			// 클랜 스탭 임명
	"CLAN_COMMISSION_REGULAR",	EVENT_CLAN_COMMISSION_REGULAR,		// 클랜 일반으로 강등
	"CLAN_DISMISSALMEMBER",		EVENT_CLAN_DISMISSALMEMBER,			// 클랜원 제명
	"CLAN_DISSOLUTION",			EVENT_CLAN_DISSOLUTION,				// 가입 해체
	"CLAN_NOTICE_MODIFY",		EVENT_CLAN_NOTICE_MODIFY,			// 공지사항 수정
	"CLAN_GUIDE_MODIFY",		EVENT_CLAN_GUIDE_MODIFY,			// 클랜소개 수정
	"CLAN_MENAGEMENT",			EVENT_CLAN_MENAGEMENT,				// 클랜 기타 관리 수정
	"CLAN_CHANGE_MARK",			EVENT_CLAN_CHANGE_MARK,				// 클랜마크 변경
	"CLAN_MEMBER_INSERT",		EVENT_CLAN_MEMBER_INSERT,			// 클랜맴버 추가
	"CLAN_INVITE",				EVENT_CLAN_INVITE,					// 클랜 초대
	"CLAN_NOTE",				EVENT_CLAN_NOTE,					// 클랜 전체쪽지
	"MATCH_TEAM_CREATE",		EVENT_MATCH_TEAM_CREATE,			// 클랜전 팀 만들기
	"MATCH_TEAM_JOIN",			EVENT_MATCH_TEAM_JOIN,				// 클랜전 팀 참가
	"MATCH_TEAM_LEAVE",			EVENT_MATCH_TEAM_LEAVE,				// 클랜전 팀 떠나기
	"MATCH_TEAM_CHANGE_PER",	EVENT_MATCH_TEAM_CHANGE_PER,		// 클랜전 인원수 변경
	"MATCH_TEAM_CONTEXT",		EVENT_MATCH_TEAM_CONTEXT,			// 클랜전 팀목록 Context
	"MATCH_TEAM_ALL_CONTEXT",	EVENT_MATCH_TEAM_ALL_CONTEXT,		// 클랜전 팀목록 Context
	"MATCH_TEAM_DETAIL_INFO",	EVENT_MATCH_TEAM_DETAIL_INFO,		// 클랜 상세 정보 받기
	"MATCH_FIGHT_QUICK_REQUEST", EVENT_MATCH_FIGHT_QUICK_REQUEST,	// 클랜전 빠른 신청
	"MATCH_FIGHT_REQUEST",		EVENT_MATCH_FIGHT_REQUEST,			// 클랜전 전투신청
	"MATCH_FIGHT_ACCECT",		EVENT_MATCH_FIGHT_ACCECT,			// 클랜전 전투신청 수락

	// Community
	"INSERT_FRIEND",			EVENT_INSERT_FRIEND,				// 친구 추가
	"ACCEPT_FRIEND",			EVENT_ACCEPT_FRIEND,				// 친구 수락
	"DELETE_FRIEND",			EVENT_DELETE_FRIEND,				// 친구 삭제
	"INVITE_FRIEND",			EVENT_INVITE_FRIEND,				// 친구 초대하기
	"INVITED_MESSAGE",			EVENT_INVITED_MESSAGE,				// 친구 초대받기
	"SEND_WHISPER",				EVENT_SEND_WHISPER,					// 귓속말
	"SEND_WHISPER_FIND_UID",	EVENT_SEND_WHISPER_FIND_UID,		// Receiver UID로 귓속말
	"FIND_USER",				EVENT_FIND_USER,					// 친구찾기
	"SEND_NOTE",				EVENT_SEND_NOTE,					// 쪽지 보내기
	"SEND_NOTE_BY_UID",			EVENT_SEND_NOTE_BY_UID,				// ReceiverUID로 쪽지 보내기
	"CHECK_READED_NOTE",		EVENT_CHECK_READED_NOTE,			// 쪽지 읽음표시
	"DELETE_NOTE",				EVENT_DELETE_NOTE,					// 쪽지 삭제 요청
	"DELETE_NOTE_ASK",			EVENT_DELETE_NOTE_ASK,				// 쪽지 삭제 응답
	"RECEIVE_NOTE",				EVENT_RECEIVE_NOTE,					// 새쪽지 받기

	"INSERT_BLOCK",				EVENT_INSERT_BLOCK,					// 차단 추가
	"DELETE_BLOCK",				EVENT_DELETE_BLOCK,					// 차단 삭제

	// Lobby event
	"CREATE_NICK_NAME",			EVENT_CREATE_NICK_NAME,				// 최초 닉네임 만들기	- 변경이 아니다
	"GET_FIRST_SUPPLY",			EVENT_GET_FIRST_SUPPLY,				// 초도 보급
	"CREATE_ROOM",				EVENT_CREATE_ROOM,					// 방만들기							( Lobby )
	"JOIN_ROOM",				EVENT_JOIN_ROOM,					// 방에 입장						( Lobby )
	"JOIN_QUICK",				EVENT_JOIN_QUICK,					// 방을 랜덤으로 찾아 입장			( Lobby )
	"UPDATE_USER_LIST",			EVENT_UPDATE_USER_LIST,				// 로비의 유저 리스트가 갱신됬다.	( Lobby )
	// Readyroom event
	"GET_LOBBY_USER",			EVENT_GET_LOBBY_USER,				// 초대목록 받기
	"INVITE_LOBBY_USER",		EVENT_INVITE_LOBBY_USER,			// 초대하기
	"SEARCH_HOST",				EVENT_SEARCH_HOST,					// 적합한 방장 후보를 찾는다
	"TRANSFER_HOST",			EVENT_TRANSFER_HOST,				// 방장 넘기기
	"REQUEST_HOST_CHANGE",		EVENT_REQUEST_HOST_CHANGE,			// 방장변경 요청

	"ARMEDBOMB_BATTLE",			EVENT_ARMEDBOMB_BATTLE,				// 폭파 미션에서 폭탄 설치 
	"DISARMBOMB_BATTLE",		EVENT_DISARMBOMB_BATTLE,			// 폭파 미션에서 폭탄 설치 

	// 방어 미션(디펜스미션)
	"DAMAGE_DEFENCE_OBJECT_BATTLE", EVENT_DAMAGE_DEFENCE_OBJECT_BATTLE,	// 방어 오브젝트 피해중

	// Quest Event
	"QUEST_CHANGE_ACTIVE_IDX",	EVENT_QUEST_CHANGE_ACTIVE_IDX,		// Active Quest Index의 변경 : stageend이후 퀘스트 완료에의해 변경 및 유저가 카드나 카드셋을 변경하여 active index를 변경한경우 호출	
	"QUEST_FINISH",				EVENT_QUEST_FINISH,					// Quest의 완료				 : stage중에 완료되는 퀘스트에대해 서버에 알림
//	"QUEST_BUY_ONE_CARD",		EVENT_QUEST_BUY_ONE_CARD,			// 카드 한장 구입
	"QUEST_BUY_CARD_SET",		EVENT_QUEST_BUY_CARD_SET,			// 카드셋 구입
	"QUEST_DELETE_CARD_SET",	EVENT_QUEST_DELETE_CARD_SET,		// 카드셋 삭제

	//// 1.5 Check Designation
	//// 호칭
	//"DESIGNATION_GET",			EVENT_DESIGNATION_GET,				// 호칭 획득
	//"DESIGNATION_CHANGE",		EVENT_DESIGNATION_CHANGE,			// 호칭 변경
	//"DESIGNATION_RELEASE",		EVENT_DESIGNATION_RELEASE,			// 호칭 해제

	"GAME_GUARD_AUTH2",			EVENT_GAME_GUARD_AUTH2,				// 게임가드 인증키 요청.

	"READY_BATTLE",				EVENT_READY_BATTLE,					// 배틀 준비 완료					( ReadyRoom )
	"BATTLE_COUNTDOWN",			EVENT_BATTLE_COUNTDOWN,				// 배틀 시작 카운트다운
    
	"DEATH_BATTLE",				EVENT_DEATH_BATTLE,					// 죽었습니다.							( Battle )
	"HOLEPUNCH_OK",				EVENT_HOLEPUNCH_OK,					// 방장이 바뀌고 CN이 확인됨을 알려줌..	( Battle )
	"DISCONNECT",				EVENT_DISCONNECT,					// 연결 끊기
	"TEAM_CHANGE",				EVENT_TEAM_CHANGE,					// 진영변경
	"RESPAWN_BATTLE",			EVENT_RESPAWN_BATTLE,				// 부활했습니다.						( Battle )
	"RESPAWN_BATTLE_FOR_AI",	EVENT_RESPAWN_BATTLE_FOR_AI,		// 훈련전 AI 리스폰
	// Battle event
	"RENDEZVOUS_HOLEPUNCH_BATTLE", EVENT_RENDEZVOUS_HOLEPUNCH_BATTLE,	// 랑데뷰 서버와 홀펀칭을 합니다.
	"PRESTART_BATTLE",			EVENT_PRESTART_BATTLE,				// UDP를 체크한다.						( Battle )	
	"START_BATTLE",				EVENT_START_BATTLE,					// 배틀 시작.							( Battle )	
	"GIVEUP_BATTLE",			EVENT_GIVEUP_BATTLE,				// 배틀을 나간다.						( Battle )
	"CHANGE_DIFFICULTY_LEVEL",	EVENT_CHANGE_DIFFICULTY_LEVEL,		// 훈련전 난이도 변경 통보
	"BATTLE_TOUCHDOWN",			EVENT_BATTLE_TOUCHDOWN,				// 미션 위치에 도달했습니다.

	"INVITE_CLAN",				EVENT_INVITE_CLAN,					// 클랜 초대하기


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game -> Context
	"N",						EVENT_N,						// 응답을 기달릴 필요가 없는 메시지
	"GET_MYOPTION_N",			EVENT_GET_MYOPTION_N,				// 옵션정보 받기
	"LOBBY_REFRESH_N",			EVENT_LOBBY_REFRESH_N,				// Lobby갱신
	"LOBBY_PAGE_PREV_N",		EVENT_LOBBY_PAGE_PREV_N,			// Lobby에서의 페이지 감소
	"LOBBY_PAGE_NEXT_N",		EVENT_LOBBY_PAGE_NEXT_N,			// Lobby에서의 페이지 증가
	"CHANGE_ROOM_ALLINFO_N",	EVENT_CHANGE_ROOM_ALLINFO_N,		// 방정보 모두변경					( ReadyRoom )
	"CHANGE_ROOMINFO_N",		EVENT_CHANGE_ROOMINFO_N,			// 방정보 변경						( ReadyRoom )
	"CHANGE_ROOM_PASSWORD_N",	EVENT_CHANGE_ROOM_PASSWORD_N,		// 룸패스워드 변경
	"CHANGE_SLOT_N",			EVENT_CHANGE_SLOT_N,				// 슬롯을 열었다가 닫았다 한다. 
	"CHATTING_N",				EVENT_CHATTING_N,					// 채팅 
	"CHANGE_TEAM_N",			EVENT_CHANGE_TEAM_N,				// 팀변경
	"CHANGE_OBSERVER_SLOT",		EVENT_CHANGE_OBSERVER_SLOT,			// 옵저버슬롯으로 이동
	"CHANGE_HIDDEN_SLOT",		EVENT_CHANGE_HIDDEN_SLOT,			// 히든슬롯으로 이동(GM만 가능)

	"GET_SLOTINFO_N",			EVENT_GET_SLOTINFO_N,				// 슬롯 정보를 얻는다
	"GET_USERINFO_N",			EVENT_GET_USERINFO_N,				// 유저 정보를 얻는다
	"EQUIPMENT_TEMP_WEAPON_N",	EVENT_EQUIPMENT_TEMP_WEAPON_N,		// 임시무기 장착 및 탈착
	"CHANGE_ROOM_OPTION_N",		EVENT_CHANGE_ROOM_OPTION_N,			// 옵션정보 변경 
	"CHANGE_EQUIPMENT_N",		EVENT_CHANGE_EQUIPMENT_N,			// 아이템 교체						(예정)
	"GET_ROOM_ADDINFO_N",		EVENT_GET_ROOM_ADDINFO_N,			//
	"UNREADY_4VS4_N",			EVENT_UNREADY_4VS4_N,				// 4대4 모드 관련 레디 상태 해제	
	"SENDPING_N",				EVENT_SENDPING_N,					// MS에 Ping 정보를 보냅니다.
	"DAMAGEOBJ_BATTLE_N",		EVENT_DAMAGEOBJ_BATTLE_N,			// 파괴미션
	"OPTION_SAVE_N",			EVENT_OPTION_SAVE_N,				//
	"COMMUNITY_RECORD_DB_N",	EVENT_COMMUNITY_RECORD_DB_N,		// 커뮤니티에서 정보 요청
	"COMMUNITY_RECORD_SESSION_N", EVENT_COMMUNITY_RECORD_SESSION_N, 	// 커뮤니티에서 정보 요청
	"COMMUNITY_RECORD_ALL_N",	EVENT_COMMUNITY_RECORD_ALL_N,		// 유저 리스트에서의 전적 전체 정보
	"COMMUNITY_RECORD_ALL_DB_N", EVENT_COMMUNITY_RECORD_ALL_DB_N,	// 커뮤니티 창에서의 전적 전체 정보
	"COMMUNITY_RECORD_CLAN_N", EVENT_COMMUNITY_RECORD_CLAN_N,		// 커뮤니티에서 클랜정보 요청
	
	
	// 20080707 김현우 수정분입니다.

	//클랜
	"CLAN_LIST_N",				EVENT_CLAN_LIST_N,						// 리스트 요청
	"CLAN_REQUEST_LIST_N",		EVENT_CLAN_REQUEST_LIST_N,				// 가입신청서 목록보기
	"CLAN_DETAIL_INFO_N",		EVENT_CLAN_DETAIL_INFO_N,				// 상세 정보 요청
	"CLAN_MEMBER_LIST_N",		EVENT_CLAN_MEMBER_LIST_N,				// 멤버 목록 보기 
	"CLAN_REQUEST_INFO_N",		EVENT_CLAN_REQUEST_INFO_N,				// 가입신청서 상세정보 보기
	"BATTLE_REMAINTIME_N",		EVENT_BATTLE_REMAINTIME_N,				// 배틀 스테이지 내에서 현재 라운드 및 배틀 남은시간을 서버에 전송
	"CLAN_INVITE_ACCEPT_N",		EVENT_CLAN_INVITE_ACCEPT_N,				// 클랜 초대 수락
	

	"MATCH_TEAM_LIST_N",		EVENT_MATCH_TEAM_LIST_N,				// 클랜전 팀목록 받기
	"MATCH_TEAM_ALL_LIST_N",	EVENT_MATCH_TEAM_ALL_LIST_N,			// 클랜전 모든 팀 목록 받기
	"MATCH_TEAM_CHANGE_PER_RESULT_N", EVENT_MATCH_TEAM_CHANGE_PER_RESULT_N,	// 클랜전 인원수 변경 결과
	"MATCH_TEAM_INFO_N",		EVENT_MATCH_TEAM_INFO_N,				// 클랜전 팀 정보	

	"HEART_BIT_N",				EVENT_HEART_BIT_N,
	"N_NORMAL_END",				EVENT_N_NORMAL_END,						//정상 종료
	"N_ABNORMAL_END",			EVENT_N_ABNORMAL_END,					//비정상 종료 
	"N_QUEST_END_NOTICE",		EVENT_N_QUEST_END_NOTICE,				//이벤트 종료 노티스


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Context -> Game	
	"M",						EVENT_M,							// 서버에서만 오는 메시지 
	"M_KICK_USER",				EVENT_M_KICK_USER,					// 강퇴 
	"M_END_BATTLE",				EVENT_M_END_BATTLE,					// 게임 끝 
	"M_DRIVE_AWAY",				EVENT_M_DRIVE_AWAY,					// 동일 계정 접속자 요청에 의한 강제종료
	"M_ERROR",					EVENT_M_ERROR,						// 서버에서 접속을 끊어버리는 에러들
	"M_DISCONNECTION",			EVENT_M_DISCONNECTION,				// DISCONNECTIONSUCCESS_ACK에 대한 처리
	"M_GAMEGUARD_ERROR",		EVENT_M_GAMEGUARD_ERROR,			// 게임가드 에러 
	"M_TIMEOUTCN",				EVENT_M_TIMEOUTCN,					// TimeOut
	"M_TIMEOUTCS",				EVENT_M_TIMEOUTCS,					// TimeOut 방장
	"M_PRE_ROUNDSTART_BATTLE",	EVENT_M_PRE_ROUNDSTART_BATTLE,		// 라운드 시작 준비
	"M_ROUNDSTART_BATTLE",		EVENT_M_ROUNDSTART_BATTLE,			// 라운드 시작
	"M_ROUNDEND_BATTLE",		EVENT_M_ROUNDEND_BATTLE,			// 라운드 끝
	"M_HOSTLOST_BATTLE",		EVENT_M_HOSTLOST_BATTLE,			// 방장변경
	"M_INVALID_TIMESYNC",		EVENT_M_INVALID_TIMESYNC,			// 서버에서 게임중 로컬타임과 서버타임이 맞지않는다라고 판단
	"M_DESTROYED_NETWORK",		EVENT_M_DESTROYED_NETWORK,			// 네트워크 연결이 끊기다.

	"M_CLAN_MEMBER_INFO",		EVENT_M_CLAN_MEMBER_INFO,			// 커뮤니티클랜맴버 받기
	"M_MATCH_FIGHT_RESULT",		EVENT_M_MATCH_FIGHT_RESULT,			// 클랜전 전투신청 응답
	"M_MATCH_FIGHT_ACCECT_RESULT", EVENT_M_MATCH_FIGHT_ACCECT_RESULT,	// 클랜전 전투신청 응답
	"M_MATCH_FIGHT_CHANGE_INFO_RESULT", EVENT_M_MATCH_FIGHT_CHANGE_INFO_RESULT,	// 클랜전 전투신청 응답

	"M_MATCH_ROOM_CREATE",		EVENT_MATCH_ROOM_CREATE,			// 클랜전 전투 방 생성
	"M_MATCH_ROOM_JOIN",		EVENT_MATCH_ROOM_JOIN,			// 클랜전 전투 방 입장

	"INVITED_CLAN_MESSAGE",		EVENT_INVITED_CLAN_MESSAGE,			// 클랜 초대받기

	"RUN_PARAM_CHECK",			EVENT_RUN_PARAM_CHECK,				// 실행 인자 체크하기.

	// GM 명령
	"KICK_USER",				EVENT_KICK_USER,					// 유저 강퇴
	"EXIT_USER",				EVENT_EXIT_USER,					// 방 유저 접속종료
	"DESTROY_ROOM",				EVENT_DESTROY_ROOM,					// 방 파괴
	"LOBBY_EXIT_USER",			EVENT_LOBBY_EXIT_USER,				// 로비 유저 접속종료
	0);

	if( bReturn == false)
	{
		I3FATAL("Add Enum to Lua failed");
		return false;
	}

	return true;
}
#endif	// #if defined( USE_LUA_UI)
