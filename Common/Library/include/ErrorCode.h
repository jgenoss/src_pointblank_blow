#ifndef __S2_MO_ERROR_H__
#define __S2_MO_ERROR_H__

//////////////////////////////////////////////////////////////////////////
// Error Code
//////////////////////////////////////////////////////////////////////////
#define EVENT_ERROR_MASK_FAIL											0x80FFFFFF	// ERC Fail Bit-mask
//	하위 20 bits는 bitflag 형식으로 전달됩니다. - praptor
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK								0x000FFFFF	// 하위 20 bits Mask

// ERC - Common define
#define EV_SUCCESSED(arg)												((arg) >= 0)
#define EV_FAILED(arg)													((arg) < 0)
#define EV_IS_ERROR(arg,err)											(((arg) & (EVENT_ERROR_MASK_FAIL)) == (err))

enum EVENT_ERROR
{
	EVENT_ERROR_SUCCESS													= 0x00000000,	// 기본 성공
	EVENT_ERROR_SUCCESS_1												= 0x00000001,	// 기본 성공 1	

	EVENT_ERROR_FAIL													= 0x80000000,	// 기본 실패
	
	EVENT_ERROR_WRONG_POSITION_CONNECT									= 0x80000010,
	EVENT_ERROR_WRONG_POSITION_LOGIN									= 0x80000011,
	EVENT_ERROR_WRONG_POSITION_INFO										= 0x80000012,
	EVENT_ERROR_WRONG_POSITION_CHANNEL									= 0x80000013,
	EVENT_ERROR_WRONG_POSITION_ROOM										= 0x80000014,
	EVENT_ERROR_WRONG_POSITION_BATTLE									= 0x80000015,
	EVENT_ERROR_WRONG_POSITION_ROOM_GM_PAUSE							= 0x80000016,
	EVENT_ERROR_WRONG_POSITION_NON_BATTLE								= 0x80000017,
	EVENT_ERROR_WRONG_POSITION_QUICK_JOIN								= 0x80000018,
	EVENT_ERROR_WRONG_POSITION_LOBBY									= 0x80000019,

	// ERC - Game scope
	EVENT_ERROR_GAME_WRONG_POSITION					 					= 0x80000001,	// 명령을 실행할 수 없는 포지션에 있습니다. 이메시를 받으면 다시 시작위치로 이동합니다. 서버에서도 리셋합니다.	-2147483647
	EVENT_ERROR_GAME_WRONG_SAME_REQ										= 0x80000002,	// 채널이 선택되있지않음, CServerStage구현 후 redefine 예정 - praptor
	EVENT_ERROR_GAME_WRONG_REQUEST_LOGIN								= 0x80000003,	// 로그인 하지 않은 유저가 로그인 이후의 동작을 요구한 경우 : 해킹 입니다.
	EVENT_ERROR_GAME_WRONG_REQUEST_CHANNEL								= 0x80000004,	// 채널선택을 하지않은 유저가 채널선택 이후의 동작을 요구한 경우 : 해킹 입니다.									-2147483644
	EVENT_ERROR_GAME_WRONG_REQUEST_ROOM									= 0x80000005,	// 방에 없는 유저가 방안에 있는 동작을 요구한 경우 : 해킹입니다. 
	EVENT_ERROR_GAME_WRONG_DATA											= 0x80000006,	// 보낼수 없는 값을 요청한 경우 : 해킹이나..깨진 데이터 입니다. 
	EVENT_ERROR_GAME_WRONG_IMPOSSIBLE_BEGINROUND						= 0x80000007,	// 시작라운드로 설정불가능한 값을 보내온 경우 : 해킹으로 예상됩니다.

	// Login
	EVENT_ERROR_LOGIN													= 0x80000100,
	EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN								= 0x80000101,	// 동일 아이디로 접속된 사용자가 있음
	EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT							= 0x80000102,	// 아이디나 패스워드가 틀림
	EVENT_ERROR_LOGIN_BREAK_SESSION										= 0x80000103,	// Session이 깨졌음
	EVENT_ERROR_EVENT_LOGOUTING											= 0x80000104,	// 아직 로그아웃 중입니다.
	EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1									= 0x80000105,	// 로그인-서버에 문제가 있습니다.
	EVENT_ERROR_EVENT_LOG_IN_TIME_OUT2									= 0x80000106,	// 로그인-서버에 문제가 있습니다.
	EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT								= 0x80000107,	// 계정제재
	EVENT_ERROR_EVENT_LOG_IN_UNKNOWN									= 0x80000108,	// 처리 외 이유로 로그인 실패
	//인도 네시아 로그인으로 추가됨 
	EVENT_ERROR_EVENT_LOG_IN_PACKET										= 0x80000109,	// 패킷 깨짐 
	EVENT_ERROR_EVENT_LOG_IN_MD5										= 0x8000010A,	// MD5 
	EVENT_ERROR_EVENT_LOG_IN_HTTP										= 0x8000010B,	// HTTP ERROR
	EVENT_ERROR_EVENT_LOG_IN_SYS										= 0x8000010C,	// HTTP ERROR
	EVENT_ERROR_EVENT_LOG_IN_NOTAVAILABLE								= 0x8000010D,	// HTTP ERROR
	EVENT_ERROR_EVENT_LOG_IN_MAXUSER									= 0x8000010E,	// 
	// 중남미 로그인으로 추가됨
	EVENT_ERROR_EVENT_LOG_IN_SERVER_INSPECT_TIME						= 0x80000115,	// 서버 점검중 입니다.
	EVENT_ERROR_EVENT_LOG_IN_NO_SERVICE_GAME							= 0x80000116,	// 서비스 되는 게임이 아닙니다.
	// 이탈리아 로그인으로 추가됨
	EVENT_ERROR_EVENT_LOG_IN_DB											= 0x80000122,	// Login DB 오류
	// 북미 로그인으로 추가됨
	EVENT_ERROR_EVENT_LOG_IN_BLOCK_COUNTRY								= 0x80000123,	// Block 된 국가
	EVENT_ERROR_EVENT_LOG_IN_CUSTOM_MESSAGE								= 0x80000124,	// 북미에서 보내준 에러 메세지를 클라이언트에 전송하여 띄우게 합니다.
	// 대만 로그인으로 추가됨 
	EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB								= 0x80000125,	// DB 서버 접속 실패.
	EVENT_ERROR_EVENT_LOG_IN_BUSY_SERVER								= 0x80000126,	// 서버가 바쁘다?.
	EVENT_ERROR_EVENT_LOG_IN_WRONG_INPUT_DATA							= 0x80000127,	// Input Data ( TCP 패킷) 포맷이 잘못된다.
	EVENT_ERROR_EVENT_LOG_IN_SEND_PACKET								= 0x80000128,	// 로그인 패킷 전송 실패.
	EVENT_ERROR_EVENT_LOG_IN_TIME_OUT_TAIWAN							= 0x80000129,	// 지정된 시간안에 대만 로그인 서버로부터 ACK 패킷을 받지 못했다.



	EVENT_ERROR_EVENT_SIA_USERITEM_SUCCESS								= 0x8000010F,	// SIA 를 사용하지 않음
	EVENT_ERROR_EVENT_BUY_GOODS_REPAIR									= 0x80000110,	// Repair 실패
	EVENT_ERROR_EVENT_BUFFER_FULL										= 0x80000111,	// 버퍼가 가득 찼다.
	EVENT_ERROR_DATABASE												= 0x80000112,	// DATABASE ERROR
	EVENT_ERROR_EVENT_DUPLICATE_NICK									= 0x80000113,	// 닉네임 중복
	EVENT_ERROR_EVENT_LOG_IN_DB_BUFFER_FULL								= 0x80000114,	// DB Buffer 가득 참
	
	// 터키 로그인으로 추가됨
	EVENT_ERROR_EVENT_LOG_IN_ID_FAIL									= 0x80000117,	// ID 오류
	EVENT_ERROR_EVENT_LOG_IN_PW_FAIL									= 0x80000118,	// PW 오류
	EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT								= 0x80000119,	// 삭제된 계정
	EVENT_ERROR_EVENT_LOG_IN_EMAIL_AUTH_ERROR							= 0x80000120,	// email 인증 실패
	EVENT_ERROR_EVENT_LOG_IN_BLOCK_IP									= 0x80000121,	// ip 블럭
	EVENT_ERROR_EVENT_LOG_IN_AUTHKEY_FAIL								= 0x80000122,	// AuthKey Fail
	EVENT_ERROR_EVENT_LOG_IN_AUTHKEY_TIMEOUT							= 0x80000123,	// Authkey Session Time out

	// 한국 로그인으로 추가
	EVENT_ERROR_EVENT_LOG_IN_AAS										= 0x80000124,	// 중독방지시스템(피로도)

	// 인도네시아 PC방 연동으로 추가됨 - 인니 PC방 웹서버가 보내는 값으로 Block PC방만 접속 실패처리고 나머지는 오류코드는 사용하되 접속은 성공시킴 ( 음수가 아니라 양수 )
	EVENT_ERROR_EVENT_BLOCK_PCBANG										= 0x80000130,	// 접속이 차단되있는 PC방
	EVENT_ERROR_EVENT_UNKNOWN_KEY										= 0x00000131,	// 인증키 정보가 정확하지 않음 - 음수가 아닙니다.
	EVENT_ERROR_EVENT_EXPIRATION_KEY									= 0x00000132,	// 인증키 정보가 만료됨 - 음수가 아닙니다.
	EVENT_ERROR_EVENT_UNKOWN_SYSTEM_ERROR								= 0x00000133,	// 알수없는 시스템 오류 - 음수가 아닙니다.

	// 북미-Steam 연동으로 추가됨
	EVENT_ERROR_EVENT_LOG_IN_STEAM_HTTP									= 0x80000140,	// 스팀 인증페이지에 인증 요청 전송이 실패하였다.
	EVENT_ERROR_EVENT_LOG_IN_STEAM_AUTH_FAIL							= 0x80000141,	// 스팀 인증결과가 실패로 반환되었다.
	EVENT_ERROR_EVENT_LOG_IN_STEAM_PARAMETER							= 0x80000142,	// 파라미터값이 정해진 포멧과 다릅니다.
	EVENT_ERROR_EVENT_LOG_IN_STEAM_KEY_DECRYPT							= 0x80000143,	// Encryted App Ticket 복호화에 실패하였습니다.
	EVENT_ERROR_EVENT_LOG_IN_STEAM_NOT_PB_KEY							= 0x80000144,	// PB용 Encryted App Ticket가 아닙니다.(다른게임 인증티켓값인 경우)

	// ERC - Stage scope Server
	EVENT_ERROR_SERVER													= 0x80000200,
	EVENT_ERROR_SERVER_USER_FULL										= 0x80000201,	// 채널에 사람이 꽉 찼음, CServerStage구현 후 redefine 예정 - praptor
	EVENT_ERROR_CHANNEL_ENTER_CONDITION									= 0x80000202,	// 채널 입장 실패
	EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME						= 0x80000203,	// 채널 입장 실패(배틀시간이 10시간 미만이라 초보2채널외엔 들어갈수 없음)
	EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET							= 0x80000204,	// 채널 입장 실패(와루넷 전용 채널인데 와루넷 유저가 아닌 경우)
	EVENT_ERROR_CHANNEL_ENTER_CONDITION_AZERBAIJAN						= 0x80000205,	

	// ERC - Stage scope Lobby
	EVENT_ERROR_LOBBY													= 0x80000300,

	// ERC - Stage scope ReadyRoom
	EVENT_ERROR_ROOM													= 0x80000400,
	EVENT_ERROR_ROOM_NO_AUTHORITY										= 0x80000401,	// 권한없는 작업

	EVENT_ERROR_EVENT_NOT_USE_NICK										= 0x80000402,	// 사용할수 없는 닉네임

	// ERC - Event scope
	EVENT_ERROR_EVENT													= 0x80001000,
	EVENT_ERROR_EVENT_TRANS												= 0x80001001,	// Trans 버퍼가 가득차 더이상 Send 할수 없음
	EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL								= 0x80001003,	// 방안에 유저가 다 차있음
	EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM							= 0x80001004,	// 방이 이미 사라졌음
	EVENT_ERROR_EVENT_JOIN_ROOM_WRONG_PASSWORD							= 0x80001005,	// 방 비밀번호가 다름
	EVENT_ERROR_EVENT_BATTLE_TIMEOUT_CN									= 0x80001006,	// 타임아웃 에러이다.. 로비로 강제 이동합니다.
	EVENT_ERROR_EVENT_BATTLE_TIMEOUT_CS									= 0x80001007,	// 타임아웃 에러
	EVENT_ERROR_NO_REAL_IP												= 0x80001008,	// RealIP가 없다
	EVENT_ERROR_NO_READY_TEAM											= 0x80001009,	// 2팀이 되지 않았다
	EVENT_ERROR_FIRST_MAINLOAD											= 0x8000100A,	// 로딩시간으로 아웃(방장)
	EVENT_ERROR_FIRST_HOLE												= 0x8000100B,	// 홀펀칭으로 아웃
	EVENT_ERROR_ROOM_JOIN_VOTE											= 0x8000100C,	// 방안에 입장할 수 없습니다.
	EVENT_ERROR_WAIT_BATTLE_CLIMAX										= 0x8000100F,	// ! praptor - 사용하지 않습니다.

	EVENT_ERROR_NO_START_FOR_UNDER_NAT									= 0x80001012,	// 게임을 시작할 수 없는 NAT 아래에 있음
	EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER							= 0x80001013,	// 난입이 불가능한 방 
	EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_LEVEL								= 0x80001014,	// 난입이 불가능한 방 
	EVENT_ERROR_PRESTART_BATTLE_ALEADY_END								= 0x80001015,	// 난입 시 이미 게임이 끝났다
	EVENT_ERROR_START_BATTLE_ALEADY_END									= 0x80001016,	// 난입 시 이미 게임이 끝났다

	EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS							= 0x80001017,	// 목록에 없는 상품 - idx overflow
	EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_USER							= 0x80001018,	// 유저를 찾을 수 없음
	EVENT_ERROR_EVENT_BUY_GOODS_NETWORK									= 0x80001019,	// IBGame error
	EVENT_ERROR_EVENT_BUY_GOODS_OVERLOAD								= 0x8000101A,	// 빌링요청 폭주로 당분간 요청을 받을 수없음
	EVENT_ERROR_EVENT_GET_USER_ITEM_PARAMETER_USER						= 0x8000101B,	// 유저를 찾을 수 없음
	EVENT_ERROR_EVENT_GET_USER_ITEM_NETWORK								= 0x8000101C,	// IBGame error
	EVENT_ERROR_EVENT_GET_USER_ITEM_OVERLOAD							= 0x8000101D,	// 빌링요청 폭주로 당분간 요청을 받을 수없음
	EVENT_ERROR_EVENT_GET_USER_ITEM_FAIL								= 0x8000101E,	// Auth fail - 반환 목록 만들기 실패
	EVENT_ERROR_EVENT_INSERT_PERMANENT_PARAMETER_USER					= 0x8000101F,	// ! 사용하지 않는다.
	EVENT_ERROR_EVENT_INSERT_PERMANENT_NETWORK							= 0x80001020,	// ! 사용하지 않는다.
	EVENT_ERROR_EVENT_INSERT_PERMANENT_OVERLOAD							= 0x80001021,	// ! 사용하지 않는다.
	EVENT_ERROR_EVENT_INSERT_PERMANENT_FAIL								= 0x80001022,	// ! 사용하지 않는다.
	EVENT_ERROR_EVENT_AUTH_START_PARAMETER_USER							= 0x80001023,	// 유저를 찾을 수 없음
	EVENT_ERROR_EVENT_AUTH_START_NETWORK								= 0x80001024,	// IBGame error
	EVENT_ERROR_EVENT_AUTH_START_OVERLOAD								= 0x80001025,	// 빌링요청 폭주로 당분간 요청을 받을 수없음
	EVENT_ERROR_EVENT_AUTH_START_FAIL									= 0x80001026,	// IBGame fail
	EVENT_ERROR_EVENT_AUTH_START_PRICE									= 0x80001027,	// 돈이없음 
	EVENT_ERROR_EVENT_AUTH_START_ENDTIME_FAIL							= 0x80001028,	// ! 사용하지 않는다.
	EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL							= 0x80001029,	// 아이템을 넣을수 있는 인벤토리가 없습니다. 	
	EVENT_ERROR_EVENT_BUY_GOODS_ENDTIME_FAIL							= 0x80001030,	// ! 사용하지 않는다.
	EVENT_ERROR_EVENT_AUTH_START_ALREADY_AUTH							= 0x80001031,	// 이미 인증되었음.
	EVENT_ERROR_EVENT_AUTH_START_NONSEC									= 0x80001032,	// 기간인증 아이템 아님
	EVENT_ERROR_EVENT_AUTH_FAIL											= 0x80001033,	// 인증실패
	EVENT_ERROR_EVENT_LOG_IN_USER_ITEM_FAIL								= 0x80001034,	// 로그인 중 창고 받아오기 실패
	ERROR_EVENT_FRIEND_INFO_PARAMETER_USER								= 0x80001035,	// 유저정보 찾지 못함
	ERROR_EVENT_FRIEND_INFO_QUERY										= 0x80001036,	// DB 작업 실패
	EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER						= 0x80001037,	// 유저정보 찾지 못함
	EVENT_ERROR_EVENT_FRIEND_INSERT_FULL								= 0x80001038,	// 허용 친구수 다참
	EVENT_ERROR_EVENT_FRIEND_INSERT_QUERY								= 0x80001039,	// DB 작업 실패
	EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_USER						= 0x8000103A,	// 유저정보 찾지 못함
	EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_IDX						= 0x8000103B,	// 친구 idx 범위 초과
	EVENT_ERROR_EVENT_FRIEND_ACCEPT_QUERY								= 0x8000103C,	// DB 작업 실패
	EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_USER						= 0x8000103D,	// 유저정보 찾지 못함
	EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_IDX						= 0x8000103E,	// 친구 idx 범위 초과
	EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_CONNECTED						= 0x8000103F,	// 접속중인 친구 정보 찾지 못함
	EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_MATCHED						= 0x80001040,	// 상대방에 친구등록정보가 없다
	EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND						= 0x80001041,	// 이미 등록된 친구
	EVENT_ERROR_EVENT_FRIEND_INSERT_NOT_FIND_NICK						= 0x80001042,	// 닉네임을 찾을 수 없음
	EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID							= 0x80001043,	// 잘못된 아이템ID
	EVENT_ERROR_EVENT_INSERT_ITEM_OVERLOAD								= 0x80001044,	// 요청 폭주로 당분간 요청을 받을 수 없음
	EVENT_ERROR_EVENT_INSERT_ITEM_NETWORK								= 0x80001045,	// IBGame error
	EVENT_ERROR_EVENT_INSERT_ITEM_PARAMETER_USER						= 0x80001046,	// 유저를 찾을 수 없음
	EVENT_ERROR_EVENT_INSERT_ITEM_FAIL									= 0x80001047,	// IBGame fail
	EVENT_ERROR_CLAN_CREATE_FAIL										= 0x80001048,	// DB에 쓰기 실패
	EVENT_ERROR_CLAN_CREATE_DUPLICATE									= 0x80001049,	// 중복된 클랜(클랜명||azitURL)
	EVENT_ERROR_CLAN_CREATE_ALEADY_MEMBER								= 0x8000104A,	// 클랜에 가입된 유저
	EVENT_ERROR_EVENT_INSERT_INFO_FAIL									= 0x8000104B,	// 추가성공하였으나 종료일을 받아내지 못했음
	EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL							= 0x8000104C,	// 구입할 카드를 찾을 수 없다
	EVENT_ERROR_EVENT_BUY_CARD_PRICE_FAIL								= 0x8000104D,	// 돈이 부족하다
	EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL								= 0x8000104E,	// 그외 이유로 실패
	EVENT_ERROR_EVENT_DELETE_CARD_PARAMETER_FAIL						= 0x8000104F,	// 삭제할 카드를 찾을 수 없다
	EVENT_ERROR_EVENT_DELETE_CARD_UNKOWN_FAIL							= 0x80001050,	// 그외 이유로 실패
	EVENT_ERROR_EVENT_AUTH_COUNT_PARAMETER_USER							= 0x80001051,	// 유저정보 찾지 못함
	EVENT_ERROR_EVENT_AUTH_COUNT_NETWORK								= 0x80001052,	// IBGame error
	EVENT_ERROR_EVENT_AUTH_COUNT_OVERLOAD								= 0x80001053,	// 요청 폭주로 당분간 요청을 받을 수 없음
	EVENT_ERROR_EVENT_AUTH_COUNT_FAIL									= 0x80001054,	// 개수인증 실패
	EVENT_ERROR_CLAN_MAXCLAN											= 0x80001055,	// 클랜의 맴버임
	EVENT_ERROR_CLAN_MAXMEMBER											= 0x80001056,	// 클랜의 멤버가 아님
	EVENT_ERROR_CLAN_MAXREQUEST											= 0x80001057,	// 클랜에 가입되 있지 않음
	EVENT_ERROR_CLAN_MEMBER												= 0x80001058,	// 클랜의 맴버임
	EVENT_ERROR_CLAN_NOMEMBER											= 0x80001059,	// 클랜의 멤버가 아님
	EVENT_ERROR_CLAN_SEARCH_CLANIDX										= 0x8000105A,	// 클랜에 가입되 있지 않음
	EVENT_ERROR_CLAN_NOSEARCH_CLANIDX									= 0x8000105B,	// 클랜을 찾을수 없다.
	EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX								= 0x8000105C,	// 클랜원이 존재한다.
	EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX								= 0x8000105D,	// 클랜원을 찾을수 없다.
	EVENT_ERROR_CLAN_LEVEL_MASTER										= 0x8000105E,	// 클랜 레벨이 클랜장이다.
	EVENT_ERROR_CLAN_LEVEL_NOMASTER										= 0x8000105F,	// 클랜 레벨이 클랜장이 아니다.
	EVENT_ERROR_CLAN_LEVEL_STAFF										= 0x80001060,	// 클랜 레벨이 스탭이다.
	EVENT_ERROR_CLAN_LEVEL_NOSTAFF										= 0x80001061,	// 클랜 레벨이 스탭이 아니다.
	EVENT_ERROR_CLAN_LEVEL_REGULAR										= 0x80001062,	// 클랜 레벨이 일반 회원이다.
	EVENT_ERROR_CLAN_LEVEL_NOREGULAR									= 0x80001063,	// 클랜 레벨이 일반 회원이 아니다.
	EVENT_ERROR_CLAN_SEARCH_SECTION										= 0x80001064,	// 색션을 찾았음
	EVENT_ERROR_CLAN_NOSEARCH_SECTION									= 0x80001065,	// 색션을 찾을수 없음
	EVENT_ERROR_CLAN_SEARCH_REQUESTIDX									= 0x80001066,	// 가입신청서에 있다
	EVENT_ERROR_CLAN_NOSEARCH_REQUESTIDX								= 0x80001067,	// 가입신청서에 없다
	EVENT_ERROR_CLAN_REQUEST_FAIL										= 0x80001068,	// 가입신청 실패
	EVENT_ERROR_CLAN_SECESSION_FAIL										= 0x80001069,	// 클랜탈퇴 실패
	EVENT_ERROR_CLAN_SECESSION_POSITION_FAIL							= 0x80010000,	// 클랜탈퇴 불가능한 위치에 있다(클랜서버에 있다)
	EVENT_ERROR_CLAN_CLOSECLAN_FAIL										= 0x8000106A,	// 클랜 해체 실패
	EVENT_ERROR_CLAN_CLOSECLAN_MEMBER_FAIL								= 0x8000106B,	// 클랜에 가입인원이 남아있어 해체 실패
	EVENT_ERROR_CLAN_COMMISSION_FAIL									= 0x8000106C,	// 권한변경 실패
	EVENT_ERROR_CLAN_CANCEL_REQUEST_FAIL								= 0x8000106D,	// 클랜신청서 취소에 실패
	EVENT_ERROR_CLAN_DEPORTATION_FAIL									= 0x8000106E,	// 클랜원 제명 실패
	EVENT_ERROR_CLAN_DEPORTATION_POSITION_FAIL							= 0x80010000,	// 클랜원이 제명 안되는 위치에 있다(클랜서버에 있다)
	EVENT_ERROR_FIND_USER_DB											= 0x8000106F,	// 유저찾기 DB 실패
	EVENT_ERROR_FIND_USER_NOT_FOUND										= 0x80001070,	// 유저찾기 없는 유저
	EVENT_ERROR_NO_CLAN_TEAM											= 0x80001071,	// 팀이 이상하다
	EVENT_ERROR_NO_CLAN_TEAM_NOTFULL									= 0x80001072,	// 한팀에 4명 이상이 되지 않았다
	EVENT_ERROR_CLAN_NOTICE_FAIL										= 0x80001073,	// 공지사항 변경 실패
	EVENT_ERROR_CLAN_INTRO_FAIL											= 0x80001074,	// 클랜 소개 변경 실패
	EVENT_ERROR_CLAN_MARKCHANGE_FAIL									= 0x80001075,	// 클랜 마크 변경 실패
	EVENT_ERROR_CLAN_NAMECHANGE_FAIL									= 0x80001076,	// 클랜 이름 변경 실패
	EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_FAIL							= 0x80001077,	// 배틀 클랜팀 만들기 실패
	EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_FAIL								= 0x80001078,	// 배틀 클랜팀 입장 실패
	EVENT_ERROR_CLAN_BATTLE_TEAM_CLOSE									= 0x80001079,	// 배틀 클랜팀 해체
	EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL									= 0x8000107A,	// 가입 권한(나이)이 맞지 않는다.
	EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL								= 0x8000107B,	// 가입 권한(계급)이 맞지 않는다.
	EVENT_ERROR_EVENT_JOIN_ROOM_TRAINING								= 0x8000107C,	// 사격장은 난입할 수 없다.
	EVENT_ERROR_EVENT_CREATE_TRAINING_WRONG_CHANNEL						= 0x8000107D,	// 사격장을 만들 수 있는 채널이 아니다.
	EVENT_ERROR_NOTE_SEND_UKNOWN_NICK									= 0x8000107E,	// 해당 닉네임의 사용자 없다.
	EVENT_ERROR_NOTE_SEND_BOX_FULL										= 0x8000107F,	// 쪽지함이 꽉 찼다.
	EVENT_ERROR_NOTE_SEND_SELF											= 0x80001080,	// 스스로에게 쪽지를 보낼 수 없다.
	EVENT_ERROR_NOTE_SEND_DAILY_LIMIT									= 0x80001081,	// 하루 쪽지 발송 제한에 걸려 보낼 수 없다.
	EVENT_ERROR_CLAN_RECORDRESET_FAIL									= 0x80001082,	// 클랜 전적을 초기화 할수 없다.
	EVENT_ERROR_USER_TITLE_REQUIREMENT_FAIL								= 0x80001083,	// 호칭 교환 조건을 만족하지 않는다.
	EVENT_ERROR_USER_TITLE_ALREADY_GAIN_FAIL							= 0x80001084,	// 이미 호칭을 보유중이다.
	EVENT_ERROR_EVENT_BUY_GOODS_USER_TITLE_NOT_ALLOWED					= 0x80001085,	// 허용된 호칭이 없어 구입할 수 없다.
	EVENT_ERROR_EVENT_EQUIP_USER_TITLE_NOT_ALLOWED						= 0x80001086,	// 허용된 호칭이 없어 장비할 수 없다.
	EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL								= 0x80001087,	// 배틀 클랜팀 실패_이미 팀을 가지고 있다.
	EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_FULL_FAIL						= 0x80001088,	// 배틀 클랜팀 만들기 실패_모든 팀이 가득차서 더이상 생성불가
	EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_PER_FAIL						= 0x80001089,	// 배틀 클랜팀 만들기 실패_제한인원수가 이상하다
	EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_NOTEMPTY_FAIL					= 0x8000108A,	// 배틀 클랜팀 만들기 실패_비어있는 클랜이 아니다.
	EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_NOSEARCH_FAIL						= 0x8000108B,	// 배틀 클랜팀 입장 실패_찾을수 없는 팀
	EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_EMPTY_FAIL						= 0x8000108C,	// 배틀 클랜팀 입장 실패_없는 방
	EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL						= 0x8000108D,	// 배틀 클랜팀 입장 실패_인원이 가득차 입장할 수 없음
	EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_EMPTY_FAIL						= 0x8000108E,	// 배틀 클랜팀 퇴장 실패_찾을수 없는 팀
	EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_NOUSER_FAIL						= 0x8000108F,	// 배틀 클랜팀 퇴장 실패_없는 유저
	EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL						= 0x80001090,	// 배틀 클랜전 신청 실패_이미 시작된 팀
	EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_SELF_FAIL						= 0x80001091,	// 배틀 클랜전 신청 실패_자기자신에게 신청
	EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_NOPER_FAIL						= 0x80001092,	// 배틀 클랜전 신청 실패_인원이 맞지 않습니다.
	EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_REFUSAL_FAIL					= 0x80001093,	// 배틀 클랜전 신청 거절
	EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL							= 0x80001094,	// 존재하지 않는 팀
	EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_FAIL							= 0x80001095,	// 배틀 클랜전 인원변경 실패_변경할 인원이 이상함
	EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_PERMAX_FAIL						= 0x80001096,	// 배틀 클랜전 인원변경 실패_인원이 많아서 실패
	EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_NOMASTER_FAIL					= 0x80001097,	// 배틀 클랜전 인원변경 실패_방장이 아닌데 전달 되었음
	EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY								= 0x80001098,	// 클랜전 실패, 모두 레디가 아니다.
	EVENT_ERROR_CLAN_INVITED_REFUSAL									= 0x80001099,	// 클랜 가입 신청 거절
	EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_GOODS						= 0x8000109A,	// 선물하기 실패 - 잘못된 상품아이디
	EVENT_ERROR_CLAN_INCREASE_MAXPERSON_FAIL							= 0x8000109B,	// 클랜 최대인원 증가 실패
	EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_USER							= 0x8000109C,	// 선물하기 실패 - 잘못된 유저
	EVENT_ERROR_EVENT_GIFT_GOODS_NOT_ENOUGH_POINT						= 0x8000109D,	// 선물하기 실패 - 포인트 부족
	EVENT_ERROR_EVENT_GIFT_GOODS_WRONG_RECEIVE_USER						= 0x8000109E,	// 선물하기 실패 - 잘못된 상대 유저
	EVENT_ERROR_EVENT_CHEKC_GIFT_PARAMETER_GOODS						= 0x8000109F,	// 선물받기 실패 - 없는 상품
	EVENT_ERROR_EVENT_CHECK_GIFT_OVERLOAD								= 0x800010A0,	// 선물받기 실패 - 서버과부화
	EVENT_ERROR_EVENT_CHECK_GIFT_NETWORK								= 0x800010A1,	// 선물받기 실패 - 네트워크 문제 - IBGameError
	EVENT_ERROR_EVENT_DELETE_INFO_FAIL									= 0x800010A2,	// 아이템 삭제 실패
	EVENT_ERROR_EVENT_DELETE_ITEM_VALID_ITEM_ID							= 0x800010A3,	// 잘못된 아이템ID
	EVENT_ERROR_EVENT_DELETE_ITEM_OVERLOAD								= 0x800010A4,	// 요청 폭주로 당분간 요청을 받을 수 없음
	EVENT_ERROR_EVENT_DELETE_ITEM_NETWORK								= 0x800010A5,	// IBGame error
	EVENT_ERROR_EVENT_DELETE_ITEM_PARAMETER_USER						= 0x800010A6,	// 유저를 찾을 수 없음
	EVENT_ERROR_EVENT_DELETE_ITEM_FAIL									= 0x800010A7,	// IBGame fail
	EVENT_ERROR_CLAN_USE_ITEM_FAIL										= 0x800010A8,	// 클랜 아이템이 있어서 실패
	EVENT_ERROR_EVENT_BLOCK_INSERT_ALREADY_BLOCK						= 0x800010A9,	// 이미 차단된 목록
	EVENT_ERROR_EVENT_BLOCK_INSERT_ALREADY_CLAN							= 0x800010AA,	// 같은 클랜
	EVENT_ERROR_EVENT_READY_WEAPON_EQUIP								= 0x800010AB,	// 
	EVENT_ERROR_CLAN_MEMORY_FULL										= 0x800010AC,	// 메모리가 부족하여 더이상 클랜 생성에 실패(클랜 서버 메모리 부족)
	EVENT_ERROR_HACKING_EXIT_USER										= 0x800010AD,	// 유저가 해킹을 하여 팅겨냅니다.
	EVENT_ERROR_GAMEGUARD_ERROR											= 0x800010AE,	// 게임가드에 문제가 있습니다.
	EVENT_ERROR_LOGIN_ERROR												= 0x800010AF,	// 로그인에 문제가 있습니다.
	EVENT_ERROR_CHANNEL_TIMEOUT											= 0x800010B0,	// 로그인에 문제가 있습니다.
	EVENT_ERROR_ABUSING_CHECK_ERROR										= 0x800010B1,	// 어뷰징 체크 에러
	EVENT_ERROR_EVENT_FRIEND_INSERT_BLOCK_USER							= 0x800010B2,	// 차단되어 친구 추가 요청 실패.
	EVENT_ERROR_NOTE_SEND_BLOCK_USER									= 0x800010B3,	// 차단되어 쪽지 전송 실패.
	EVENT_ERROR_GRAVITY_X_CHECK											= 0x800010B4,	// 중력핵 X범위 벗어남.
	EVENT_ERROR_GRAVITY_Y_CHECK											= 0x800010B5,	// 중력핵 Y범위 벗어남.
	EVENT_ERROR_GRAVITY_Z_CHECK											= 0x800010B6,	// 중력핵 Z범위 벗어남.
	EVENT_ERROR_CLAN_MATCH_ALREADY_REQUESTED							= 0x800010B7,	// 이미 클랜전 신청을 하였습니다.
	EVENT_ERROR_NOT_ENOUGH_KILL_DEATH_WEIGHT							= 0x800010B8,	// Kill/Death 비율이 낮아 입장할 수 없습니다.
	EVENT_ERROR_CANT_INVITE_MERCENARY									= 0x800010B9,	// 용병을 초대할 수 없습니다.
	EVENT_ERROR_FAIL_INVITE_MERCENARY									= 0x800010BA,	// 용병 초대에 실패하였습니다.
	EVENT_ERROR_FAIL_REGIST_MERCENARY									= 0x800010BB,	// 용병 등록에 실패하였습니다.
	EVENT_ERROR_EXCEED_MERCENARY_LIMIT									= 0x800010BC,	// 참여 가능한 용병 인원 수가 초과했습니다.
	EVENT_ERROR_FAIL_JOIN_TEAM											= 0x800010BD,	// 클랜전 팀 참가에 실패했습니다.
	EVENT_ERROR_CANT_REQUEST_MATCHMAKING								= 0x800010BE,	// 클랜전을 신청할 수 없는 상태입니다.
	EVENT_ERROR_NOT_MATCHMAKING											= 0x800010BC,	// 매치메이킹 중이 아닙니다.
	EVENT_ERROR_NO_CLAN_DUPLE_CLAN_TEAM									= 0x800010BD,	// 비매치메이킹 시작 조건에 맞지 않습니다. (같은 클랜원이 각기 다른 팀에 존재합니다)

	EVENT_ERROR_GM_KICK_USER											= 0x800010BF,	// GM에 의해 Kick되었음.

	// 상점 구매
	EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED							= 0x800010C0,	// 장바구니 구입 실패. - 아이템이 여러가지라 이유도 여러개가 될 수 있겠지만 다 알려줄수 없으니 그냥 실패만
	EVENT_ERROR_EVENT_BUY_GOODS_INVALID_DATA							= 0x800010C1,	// 구입 실패. - 허용하지 않는 값으로 구매를 요청하였다. 
	EVENT_ERROR_EVENT_BUY_GOODS_GOODSCOUNT								= 0x800010C2,	// 구입 실패 - 굿즈 갯수 이상함.
	EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL_SHOP						= 0x800010C3,	// 구입 실패 - 상점에서 아이템 구매시 인벤토리가 가득차서 구입할 수 없다.
	
	// 임무카드 구매
	EVENT_ERROR_EVENT_BUY_CARD_NO_GOODS									= 0x800010D0,	// 임무 카드가 상품으로 판매되지 않고 있다.
	EVENT_ERROR_EVENT_BUY_CARD_TRANS_ERROR								= 0x800010D1,	// 트랜스 서버 연결 오류로 임무 카드 구입 실패.
	EVENT_ERROR_EVENT_BUY_CARD_NETWORK									= 0x800010D2,	// 네트워크 오류로 임무 카드 구입 실패.
	EVENT_ERROR_EVENT_BUY_CARD_SLOT_ERROR								= 0x800010D3,	// 임무카드 슬롯 오류로 임무 카드 구입 실패.

	EVENT_ERROR_EVENT_PACKET_REPEATING									= 0x800010D4,	// 패킷을 복사해서 다시 보냈습니다.

	EVENT_ERROR_EVENT_BUY_CARD_DISQUALIFIED_FAIL						= 0x800010D5,	// 임무카드를 구입할 자격이 없다 (튜토리얼 임무를 완료 해야 자격 부여)

	EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED								= 0x800010D6,	// 상품 구매할수 있는 상태가 아니다.
	EVENT_ERROR_EVENT_USE_ITEM_NOT_ALLOWED								= 0x800010D7,	// 상품 사용할수 있는 상태가 아니다.
	EVENT_ERROR_EVENT_INGAMEITEM_PROCESSING								= 0x800010D8,	// 야전 상점 효과 적용중입니다. 결과는 나중에 돌아와야 하므로 일단 보류처리 해야함

	// 강제 탈퇴 투표 거절 이유
	EVENT_ERROR_KICKVOTE_ALREADY_EXIST_OTHER_VOTE						= 0x800010E0,	// 이미 다른 투표가 진행 중입니다.
	EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_ONE_MINUTE						= 0x800010E1,	// 1분내에 같은 신고인 투표가 진행된 적이 있습니다.
	EVENT_ERROR_KICKVOTE_SMALL_USER_IN_ROOM								= 0x800010E2,	// Room 내에 투표 인원이 2인 이하라 진행할 수 없습니다.
	EVENT_ERROR_KICKVOTE_NOT_ENOUGH_VOTE_TIME							= 0x800010E3,	// 게임 잔여 시간이 부족합니다.
	EVENT_ERROR_KICKVOTE_MAX_VOTE_COUNT_OVER							= 0x800010E4,	// 최대 신고 횟수를 초과 했습니다.
	EVENT_ERROR_KICKVOTE_UNDER_RANK_LIMIT								= 0x800010E5,	// 최소 Rank조건에 만족하지 않습니다.
	EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_GUARD_TIME						= 0x800010E6,	// 보호 시간내에 유저가 피신고자가 되었습니다.

	// 강제 탈퇴 투표의 표 불인정 이유
	EVENT_ERROR_KICKVOTE_NOTAPPLY_NONE_VOTE								= 0x800010F0,	// 진행중인 투표가 없습니다. ( 투표가 종료되면, 서버에서 투표종료 패킷을 보내서 클라이언트가 투표 패킷을 보낼수 없게 막기 때문에 발생 가능성 낮습니다. )
	EVENT_ERROR_KICKVOTE_NOTAPPLY_ALREADY_VOTED							= 0x800010F1,	// 이미 투표를 했습니다. ( 투표를 처음하면, 클라이언트 자체내에서 더 이상 투표 패킷을 보낼수 없게 되기 때문에 발생 가능성 극히 낮습니다. 이 에러가 자주 발생한다면, 버그나 해킹 의심 )
	EVENT_ERROR_KICKVOTE_NOTAPPLY_NO_RIGHT_TO_VOTE						= 0x800010F2,	// 투표권이 없습니다. ( 투표권이 없는 신고자, 피신고자, 난입자는 클라이언트 자체내에서 투표 패킷을 보낼수 없기 때문에 발생 가능성 극리 낮습니다. 이 에러가 자주 발생한다면, 버그나 해킹 의심 )

	// 강제 탈퇴 투표 부결 이유
	EVENT_ERROR_KICKVOTE_REJECTED_SHORTAGES								= 0x80001100,	// 과반수에 도달하지 못했습니다.
	EVENT_ERROR_KICKVOTE_REJECTED_NO_USER_IN_ALLY						= 0x80001101,	// 아군에서 찬성표가 한 표도 없습니다.
	EVENT_ERROR_KICKVOTE_REJECTED_NO_USER_IN_ENEMY						= 0x80001102,	// 적군에서 찬성표가 한 표도 없습니다.

	// 차단 리스트 - 에러 코드
	EVENT_ERROR_EVENT_BLOCK_PARAMETER_USER								= 0x80001300,	// 유저정보 찾지 못함
	EVENT_ERROR_EVENT_BLOCK_FULL										= 0x80001301,	// 허용 차단유저수 다참
	EVENT_ERROR_EVENT_BLOCK_QUERY										= 0x80001302,	// DB 작업 실패
	EVENT_ERROR_EVENT_BLOCK_ALREADY_BLOCK								= 0x80001303,	// 이미 등록된 차단유저
	EVENT_ERROR_EVENT_BLOCK_NOT_FIND_NICK								= 0x80001304,	// 닉네임을 찾을 수 없음
	EVENT_ERROR_EVENT_BLOCK_MY_NICKNAME									= 0x80001305,	// 자신의 닉네임으로 차단 추가/삭제 요청.
	EVENT_ERROR_EVENT_BLOCK_PARAMETER_IDX								= 0x80001306,	// 차단 유저 목록 범위 초과.
	EVENT_ERROR_EVENT_BLOCK_INSERT_FAIL_CLAN							= 0x80001307,	// 동일 클랜원이라서 차단 추가 실패.
	EVENT_ERROR_EVENT_BLOCK_INSERT_FAIL_FRIEND							= 0x80001308,	// 친구사이라서 차단 유저 추가 실패.
	EVENT_ERROR_EVENT_BLOCK_BUFFER_OVERFLOW								= 0x80001309,	// 버퍼 오버 플로우.
	EVENT_ERROR_EVENT_BLOCK_UNKOWN										= 0x80001310,	// 알수없는 이유.


	// Coupon Award Item
	EVENT_FAIL_COUPON_AWARD_ITEM_OF										= 0x84000100,	// 
	EVENT_FAIL_COUPON_AWARD_ITEM_OF_NOT_COUPON_NUMBER					= 0x84000101,	// 쿠폰 번호가 유효하지 않음
	EVENT_FAIL_COUPON_AWARD_ITEM_OF_IN_BUFFER_FULL						= 0x84000102,	// 쿠폰 보상 쿠폰 요청 버퍼가 꽉 찼습니다. (메시지 : 네트워크 오류 발생.)
	EVENT_FAIL_COUPON_AWARD_ITEM_OF_DB_BUFFER_FULL						= 0x84000103,	// 디비 버퍼가 꽉 찼습니다.(메시지 : 네트워크 오류 발생.)
	EVENT_FAIL_COUPON_AWARD_ITEM_OF_BILLING_PROBLEM						= 0x84000104,	// 쿠폰 인증에 문제가 있습니다.(외부 쿠폰인증 연동 - ex. Payletter)
	EVENT_ERROR_COUPON_AWARD_ITEM_COUPON_SIZE							= 0x84000105,	// 쿠폰번호를 이용한 보상시 쿠폰번호의 사이즈가 잘못된 경우
	EVENT_ERROR_COUPON_AWARD_ITEM_COUPON_NULL							= 0x84000106,	// 쿠폰번호를 이용한 보상시 쿠폰번호의 파라미터가 NULL인 경우
	EVENT_ERROR_COUPON_AWARD_ITEM_PARAMETER_USER						= 0x84000107,	// 유저를 찾을 수 없음


	// 출석체크 이벤트 - 서동권 2011.07.07
	EVENT_ERROR_ATTENDANCE_PARAMETER_USER								= 0x80001500,	// 유저를 찾을 수 없습니다.
	EVENT_ERROR_ATTENDANCE_NOT_ENOUGH									= 0x80001501,	// 현재 상태가 완료 조건보다 부족합니다.
	EVENT_ERROR_ATTENDANCE_ALREADY_COMPLETED							= 0x80001502,	// 이미 보상 처리를 완료 하였습니다.
	EVENT_ERROR_ATTENDANCE_WRONG_VERSION								= 0x80001503,	// 출석체크 이벤트 버전이 다릅니다.
	EVENT_ERROR_ATTENDANCE_SUCCESS										= 0x80001504,	// 보상 지급, 출석이 성공 하였습니다.
	EVENT_ERROR_ATTENDANCE_UNKWON										= 0x80001505,	// 알수없는 이유로 실패하였습니다.
	EVENT_ERROR_ATTENDANCE_WRONG_TYPE									= 0x80001506,	// 잘못된 이벤트 타입입니다.
	EVENT_ERROR_ATTENDANCE_PERIOD										= 0x80001507,	// 이벤트 기간이 아닙니다.
	EVENT_ERROR_ATTENDANCE_TARGET										= 0x80001508,	// 이벤트 대상이 아닙니다.
	EVENT_ERROR_ATTENDANCE_REWARD_RANGE									= 0x80001509,	// 보상 범위를 초과했습니다.
	EVENT_ERROR_ATTENDANCE_REWARD_LIST									= 0x8000150A,	// 보상 목록에 없는 아이템 입니다.

	// 할인쿠폰 - 서동권 2011.11.11
	EVENT_ERROR_INVALID_COUPON											= 0x80001600,	// 존재하지 않는 쿠폰 입니다.
	EVENT_ERROR_EXPIRY_DATE_COUPON										= 0x80001601,	// 유효기간이 지난 쿠폰 입니다.
	EVENT_ERROR_COUPON_DATA												= 0x80001602,	// 잘못된 쿠폰 데이터이거나 쿠폰 사용 조건에 맞지 않습니다.

	// 뽑기 상점 - 클라 - 서버 공통 에러코드
	EVENT_ERROR_RS_LOAD_FAIL_RSINFO										= 0x80001700,	// 뽑기상점 정보를 가져오지 못했습니다.
	EVENT_ERROR_RS_START_FAIL_CASH_BALANCE								= 0x80001701,	// 캐쉬 잔액이 부족하여 룰렛 시작이 실패 했습니다.
	EVENT_ERROR_RS_START_FAIL_WRONG_ROULETTE							= 0x80001702,	// 룰렛구성이 올바르지 못해 룰렛 시작이 실패 했습니다.
	EVENT_ERROR_RS_FAIL_NETWORK											= 0x80001703,	// 네트워크 오류 발생.
	EVENT_ERROR_RS_START_FAIL_NOT_ENOUGH_ITEM							= 0x80001704,	// 선택한 아이템의 갯수가 부족하여 룰렛 시작이 실패 했습니다.
	EVENT_ERROR_RS_START_FAIL_UNKNOWN									= 0x80001705,	// 알수없는 이유로 룰렛 시작이 실패 했습니다.
	EVENT_ERROR_RS_START_FAIL_ALREADY_PLAY								= 0x80001706,	// 이전 뽑기가 진행중입니다.

	// 뽑기 상점 - 서버 테스트 용 에러코드
	EVENT_ERROR_RS_START_FAIL_RS_ST_NULL								= 0x80001707,	// RS_ROULETTE_INFO가 NULL 이다.
	EVENT_ERROR_RS_START_FAIL_RS_INSERT_FALSE							= 0x80001708,	// Insert() 결과값이 FALSE이다.
	EVENT_ERROR_RS_START_FAIL_WRONG_PRIZE_GOODSID						= 0x80001709,	// 뽑힌 GoodsID 가 잘못 지정될경우
	EVENT_ERROR_RS_START_FAIL_WRONG_GOODSID								= 0x8000170A,	// GoodsID 오류
	EVENT_ERROR_RS_START_FAIL_NO_GOODS									= 0x8000170B,	// 존재하지 않는 Goods.
	EVENT_ERROR_RS_START_FAIL_DISABLE_GOODS								= 0x8000170C,	// 사용할 수 없는 Goods.
	EVENT_ERROR_RS_START_FAIL_NO_CASHITEM								= 0x8000170D,	// 구매하려는 Goods가 Cash아이템이 아니다.



	// NEW 유저 찾기  에러코드
	EVENT_ERROR_NEW_USER_FIND											= 0x80001800,	
	EVENT_ERROR_NEW_USER_FIND_MS_BUFFER_FULL							= 0x80001801,	// Messenger 서버 버퍼가 풀이다.
	EVENT_ERROR_NEW_USER_FIND_RUSER_WRONG								= 0x80001802,	// 요청한 유저가 없다.
	EVENT_ERROR_NEW_USER_FIND_FUSER_NOT_FIND_DB							= 0x80001803,	// 찾고자 하는 유저를 DB에서 찾지못했다.
	EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE								= 0x80001804,	// 찾고자 하는 유저가 오프라인이다.
	EVENT_ERROR_NEW_USER_FIND_NETWORK									= 0x80001805,	// 네트워크 오류로 찾기 실패

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1.5 에러코드
	EVENT_ERROR_PBV15													= 0x80010000,	// 1.5 기본 실패
	EVENT_ERROR_PBV15_PROTOCOL_DUPLICATE								= 0x80010001,	// 패킷 중복 전송
	EVENT_ERROR_LOGIN_INVALID_VERSION									= 0x80010003,	// 버전 오류(업데이트 받으세요)
	EVENT_ERROR_GAME_GUARD_INVALID_VERSION								= 0x80010004,	// 버전 오류(업데이트 받으세요)

	// 유저 정보 로드
	EVENT_ERROR_PBV15_USER_LOAD_FAIL									= 0x80010050,	// 유저정보 로드 실패

	// 캐릭터 생성
	EVENT_ERROR_PBV15_NEW_CHARA_CREATE_INVALD							= 0x80010100,	// 캐릭터 생성 조건 위배.
	EVENT_ERROR_PBV15_CHARA_BUY_INVALD									= 0x80010101,	// 캐릭터 구매 조건 불가.
	EVENT_ERROR_PBV15_CHARA_CHECK_INVALD								= 0x80010102,	// 캐릭터 검사조건 오류
	EVENT_ERROR_CHARA_FULL_SLOT											= 0x80010103,	// 캐릭터를 더이상 만들 수 없음
	EVENT_ERROR_CHARA_NOT_ITEM											= 0x80010104,	// 상점에 없는 케릭터를 이용해 생성
	EVENT_ERROR_CHARA_RANK												= 0x80010105,	// 랭크가 맞지 않아 생성 실패
	EVENT_ERROR_CHARA_BUY_DEFAULT										= 0x80010106,	// 기본 캐릭터 구매로 인한 실패
	EVENT_ERROR_CHARA_SLOT_STATE										= 0x80010107,	// 캐릭터 슬롯 상태 오류

	// 1.5 주특기 에러코드
	EVENT_ERROR_PBV15_SPECIALITY										= 0x80010200,	// 주특기 기본 실패
	EVENT_ERROR_PBV15_SPECIALITY_SET_USER								= 0x80010201,	// 주특기 셋팅 실패 - 유저가 없다.
	EVENT_ERROR_PBV15_SPECIALITY_SET_CHARA_SLOT							= 0x80010202,	// 주특기 셋팅 실패 - 잘못된 캐릭터 슬롯 번호 
	EVENT_ERROR_PBV15_SPECIALITY_SET_TYPE_CLASS							= 0x80010203,	// 주특기 셋팅 실패 - 셋팅 요청한 주특기 타입 또는 클래스가 잘못 되었다.
	EVENT_ERROR_PBV15_SPECIALITY_SET_DIFF_TYPE_CLASS					= 0x80010204,	// 주특기 셋팅 실패 - 클라/서버 주특기 정보(타입,클래스)가 다르다.
	EVENT_ERROR_PBV15_SPECIALITY_SET_SKILL_TREE							= 0x80010205,	// 주특기 셋팅 실패 - 잘못된 스킬트리 정보를 보냈다. 
	EVENT_ERROR_PBV15_SPECIALITY_SET_SEARCH_RANK_POINT					= 0x80010206,	// 주특기 셋팅 실패 - 스킬 구입시 체크하는 랭크 및 차감 플레이 타임 포인트 검색 실패.
	EVENT_ERROR_PBV15_SPECIALITY_SET_RANK								= 0x80010207,	// 주특기 셋팅 실패 - 스킬 구입시 자신의 랭크가 제한 랭크 보다 작다.
	EVENT_ERROR_PBV15_SPECIALITY_SET_POINT								= 0x80010208,	// 주특기 셋팅 실패 - 스킬 구입시 자신의 플레이 타임 포인트가 차감 플레이 타임 포인트 보다 적다.
	EVENT_ERROR_PBV15_SPECIALITY_ALEADY_SET_CLASS						= 0x80010209,	// 주특기 클래스 셋팅 실패 - 이미 클래스가 셋팅되었다.
	EVENT_ERROR_PBV15_SPECIALITY_NETWORK								= 0x8001020A,	// 주특기 클래스 셋팅 실패 - 네트워크 오류.
	EVENT_ERROR_PBV15_SPECIALITY_DB_BUFFER_FULL							= 0x8001020B,	// 주특기 클래스 셋팅 실패 - DB Buffer Full
	EVENT_ERROR_PBV15_SPECIALITY_NOT_FIND_USER							= 0x8001020C,	// 주특기 클래스 셋팅 실패 - 요청한 유저를 찾을수 없다.
	EVENT_ERROR_PBV15_SPECIALITY_HACK_PACKET							= 0x8001020D,	// 주특기 핵 패킷 - 해킹 패킷으로 인한 Disconnect.

	// 1.5 퀘스트 에러코드
	EVENT_ERROR_PBV15_QUEST												= 0x80010300,	// 퀘스트 기본 실패.
	EVENT_ERROR_PBV15_QUEST_MISSION_SET_RANGE							= 0x80010301,	// 퀘스트 미션 셋팅 실패. 셋팅 요청 가능 범위 초과.
	EVENT_ERROR_PBV15_QUEST_MISSION_SET_NO_QUEST						= 0x80010302,	// 퀘스트 미션 셋팅 실패. 셋팅 요청한 퀘스트가 존재 하지 않는다.
	EVENT_ERROR_PBV15_QUEST_GET_CLASS_QUEST_RANK						= 0x80010303,	// 계급 퀘스트 지급 실패. 지급할 계급퀘스트의 랭크가 잘못되었다.
	EVENT_ERROR_PBV15_QUEST_GET_CLASS_QUEST_INDEX						= 0x80010304,	// 계급 퀘스트 지급 실패. 지급할 계급퀘스트의 고유인덱스가 잘못되었다.
	EVENT_ERROR_PBV15_QUEST_GET_CLASS_QUEST_NO_SLOT						= 0x80010305,	// 계급 퀘스트 지급 실패. 지급할 계급퀘스트 슬롯 부족. 에러는 아니다.

	// v1.5 Medal ErrorCode
	EVENT_ERROR_PBV15_MEDAL												= 0x80001900,	// 메달 기본 실패.
	EVENT_ERROR_PBV15_MEDAL_DB_LOADING									= 0x80001901,	// 메달 DB 로드중
	EVENT_ERROR_PBV15_MEDAL_NETWORK										= 0x80001902,	// 메달 네트워크 오류. 
	EVENT_ERROR_PBV15_MEDAL_LOAD_STATE									= 0x80001903,	// 메달 로드 상태 오류.
	EVENT_ERROR_PBV15_MEDAL_UNKNOWN_USER								= 0x80001904,	// 메달 유저 정보 없음
	EVENT_ERROR_PBV15_MEDAL_DB_SAVE_BUFFER_FULL							= 0x80001905,	// 메달 DB Save Buffer Full
	EVENT_ERROR_PBV15_MEDAL_DB_LOAD_BUFFER_FULL							= 0x80001906,	// 메달 DB Load Buffer Full
	EVENT_ERROR_PBV15_MEDAL_DB_SAVE_FAIL								= 0x80001907,	// 메달 DB Save Fail
	EVENT_ERROR_PBV15_MEDAL_DB_LOAD_FAIL								= 0x80001908,	// 메달 DB Load Fail
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_BASIC_NORESULT			= 0x80001909,	// 메달 Save Sp Fail. Medal Basic - 결과값없음
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_BASIC_NOUSER				= 0x8000190A,	// 메달 Save Sp Fail. Medal Basic - 유저 없음
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_BASIC_MER_FAIL			= 0x8000190B,	// 메달 Save Sp Fail. Medal Basic	- 머지 실패
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_BASIC_UNRESULT			= 0x8000190C,	// 메달 Save Sp Fail. Medal Basic	- 알수없는 결과값
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_NORESULT					= 0x8000190D,	// 메달 Save Sp Fail. Medal - 결과값없음
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_MER_FAIL					= 0x8000190E,	// 메달 Save Sp Fail. Medal - 머지 실패
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_UNRESULT					= 0x8000190F,	// 메달 Save Sp Fail. Medal - 알수없는 결과값
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_NORESULT			= 0x80001911,	// 메달 Save Sp Fail. Cur Medal Set  - 결과값없음
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_ALCOM			= 0x80001912,	// 메달 Save Sp Fail. Cur Medal Set  - 이미 완료된 메달 셋트.
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_MER_FAIL			= 0x80001913,	// 메달 Save Sp Fail. Cur Medal Set  - 머지 실패
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_UNRESULT			= 0x80001914,	// 메달 Save Sp Fail. Cur Medal Set  - 알수없는 결과값
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_NOUSER							= 0x80001915,	// 메달 Save Sp Fail. 유저 없음.


	EVENT_ERROR_PBV15_MEDAL_FAIL_MEDAL_FILE_OPEN						= 0x80001916,	// 메달 mqfex 파일 Open 실패.
	EVENT_ERROR_PBV15_MEDAL_FAIL_MEDAL_FILE_HEADER						= 0x80001917,	// 메달 mqfex 파일 헤더 오류.


	EVENT_ERROR_PBV15_MEDAL_NOTIFY_MEDAL_HACK							= 0x80001918,	// 메달 알리미 등록 요청 값 해킹.
	EVENT_ERROR_PBV15_MEDAL_NOTIFY_MEDAL_NO								= 0x80001919,	// 메달 알리미 등록 요청 값에 해당하는 메달이 없다.

	EVENT_ERROR_PBV15_MEDAL_REWARD_HACK									= 0x8000191A,	// 메달 보상 요청 값 해킹.
	EVENT_ERROR_PBV15_MEDAL_REWARD_FIND_FAIL_MEDAL						= 0x8000191B,	// 메달 정보 조회 실패.

	EVENT_ERROR_PBV15_MEDAL_REWARD_FIND_FAIL_ITEM						= 0x8000191C,	// 보상 - 아이템 정보가 없다.
	EVENT_ERROR_PBV15_MEDAL_REWARD_NETWORK								= 0x8000191D,	// 보상 - 아이템 네트워크 오류

	EVENT_ERROR_PBV15_MEDAL_REWARD_NO_USER								= 0x8000191E,	// 보상 - 유저 정보 없음.

	EVENT_ERROR_PBV15_MEDAL_REWARD_DB_SAVE_BUFFER_FULL					= 0x8000191F,	// 메달 Reward DB Save Buffer Full
	EVENT_ERROR_PBV15_MEDAL_REWARD_SAVE_SP_FAIL_NOUSER					= 0x80001920,	// 메달 Reward Save Sp Fail. 유저 없음.
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_REWARD_UPD_FAIL			= 0x80001921,	// 메달 Save Sp Fail. Medal Reward	- UPDATE 실패
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_REWARD_UNRESULT			= 0x80001922,	// 메달 Save Sp Fail. Medal Reward	- 알수없는 결과값
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_REWARD_NORESULT			= 0x80001923,	// 메달 Save Sp Fail. Medal Reward - 결과값없음

	EVENT_ERROR_PBV15_MEDAL_FAIL_MEDAL_FILE_ARGUMENT					= 0x80001924,	// 메달 mqfex 파일 매개변수 값 잘못됨

	EVENT_ERROR_PBV15_MEDAL_CUR_MEDAL_SET_COM_DB_SAVE_BUFFER_FULL		= 0x80001925,	// 진행중인 메달 셋트 완료 DB Save Buffer Full
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_NOUSER			= 0x80001926,	// 메달 Save Sp Fail. Cur Medal Set  - 유저 없음

	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_COM_FAIL			= 0x80001927,	// 메달 Save Sp Fail. Cur Medal Set Com - SP 실패. 이전에 완료됬엇던 메달 셋트를 또 완료시킨 경우.
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_COM_NOUSER		= 0x80001928,	// 메달 Save Sp Fail. Cur Medal Set Com - 유저 없음
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_COM_UNRESULT		= 0x80001929,	// 메달 Save Sp Fail. Cur Medal Set Com - 알수없는 결과값.
	EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_COM_NORESULT		= 0x8000192A,	// 메달 Save Sp Fail. Cur Medal Set Com - 결과값없음.
	EVENT_ERROR_PBV15_MEDAL_CUR_MEDAL_SET_COMPLETE_DB_SAVE_BUFFER_FULL	= 0x8000192B,	// CurMedalSet Complete DB Save Buffer Full
	EVENT_ERROR_PBV15_MEDAL_NOTIFY_SETTING_FAIL							= 0x8000192C,	// 메달 알리미 등록 실패
	EVENT_ERROR_PBV15_MEDAL_REWARD_NO_REWARD_INFO						= 0x8000192D,	// 메달 보상 정보가 없다. 
	EVENT_ERROR_PBV15_MEDAL_REWARD_EXPIRE								= 0x8000192E,	// 메달 보상 - 기간이 만료되어 보상 지급 실패.

	EVENT_ERROR_PBV15_MEDAL_SYSTEM_DISABLE								= 0x8000192F,	// 메달 시스템 OFF.

	EVENT_ERROR_PBV15_MEDAL_GET_REWARD_FAIL								= 0x80001930,	// 메달 보상 아이템 지급 실패.


	// command
	EVENT_ERROR_PBV15_MEDAL_COMMAND_NOT_FIND_NORMAL_MEDAL				= 0x80001950,	// 커맨드 - 일반 메달 찾기 실패
	EVENT_ERROR_PBV15_MEDAL_COMMAND_NOT_FIND_SET_MEDAL					= 0x80001951,	// 커맨드 - 셋트 메달 찾기 실패
	EVENT_ERROR_PBV15_MEDAL_COMMAND_NOT_FIND_USER_SET_MEDAL				= 0x80001952,	// 커맨드 - 유저 메달 셋트 찾기 실패
	EVENT_ERROR_PBV15_MEDAL_COMMAND_NOT_FIND_USER_SET_MEDAL_ARR_IDX		= 0x80001953,	// 커맨드 - 유저 메달 셋트 인덱스 찾기 실패

	// 1.5 주특기 에러코드
	EVENT_ERROR_PBV15_SKILL												= 0x80002000,	// 주특기 기본 실패
	EVENT_ERROR_PBV15_SKILL_SET_USER									= 0x80002001,	// 주특기 셋팅 실패 - 유저가 없다.
	EVENT_ERROR_PBV15_SKILL_SET_CHARA_SLOT								= 0x80002002,	// 주특기 셋팅 실패 - 잘못된 캐릭터 슬롯 번호 
	EVENT_ERROR_PBV15_SKILL_SET_TYPE_CLASS								= 0x80002003,	// 주특기 셋팅 실패 - 셋팅 요청한 주특기 타입 또는 클래스가 잘못 되었다.
	EVENT_ERROR_PBV15_SKILL_SET_DIFF_TYPE_CLASS							= 0x80002004,	// 주특기 셋팅 실패 - 클라/서버 주특기 정보(타입,클래스)가 다르다.
	EVENT_ERROR_PBV15_SKILL_SET_SKILL_TREE								= 0x80002005,	// 주특기 셋팅 실패 - 잘못된 스킬트리 정보를 보냈다. 
	EVENT_ERROR_PBV15_SKILL_SET_SEARCH_RANK_POINT						= 0x80002006,	// 주특기 셋팅 실패 - 스킬 구입시 체크하는 랭크 및 차감 플레이 타임 포인트 검색 실패.
	EVENT_ERROR_PBV15_SKILL_SET_RANK									= 0x80002007,	// 주특기 셋팅 실패 - 스킬 구입시 자신의 랭크가 제한 랭크 보다 작다.
	EVENT_ERROR_PBV15_SKILL_SET_POINT									= 0x80002008,	// 주특기 셋팅 실패 - 스킬 구입시 자신의 플레이 타임 포인트가 차감 플레이 타임 포인트 보다 적다.
	EVENT_ERROR_PBV15_SKILL_ALEADY_SET_CLASS							= 0x80002009,	// 주특기 클래스 셋팅 실패 - 이미 클래스가 셋팅되었다.
	EVENT_ERROR_PBV15_SKILL_NETWORK										= 0x8000200A,	// 주특기 클래스 셋팅 실패 - 네트워크 오류.
	EVENT_ERROR_PBV15_SKILL_DB_BUFFER_FULL								= 0x8000200B,	// 주특기 클래스 셋팅 실패 - DB Buffer Full
	EVENT_ERROR_PBV15_SKILL_NOT_FIND_USER								= 0x8000200C,	// 주특기 클래스 셋팅 실패 - 요청한 유저를 찾을수 없다.
	EVENT_ERROR_PBV15_SKILL_HACK_PACKET									= 0x8000200D,	// 주특기 핵 패킷 - 해킹 패킷으로 인한 Disconnect.

	// 장비 장착 체크.
	EVENT_ERROR_INVENTORY_LEAVE_MAIN_SLOT								= 0x80050001,	
	EVENT_ERROR_INVENTORY_LEAVE_EQUIPMENT								= 0x80050002,	
	EVENT_ERROR_EQUIPMENT_DUPLICATE_PARTS								= 0x80050003,	// 2개의 캐릭터가 동일한(DBIdx가 같은) 파츠 장착.
	EVENT_ERROR_EQUIPMENT_WRONG_DB_IDX									= 0x80050004,	// 유효하지 않은 DBIdx.
	EVENT_ERROR_EQUIPMENT_WRONG_ITEM_ID									= 0x80050005,	// 유효하지 않은 ItemID
	EVENT_ERROR_EQUIPMENT_NOT_EXIST_INVENTORY							= 0x80050006,	// 인벤토리에 존재하지 않음
	EVENT_ERROR_EQUIPMENT_SLOT_TO_SLOT									= 0x80050007,	// 슬롯(파츠)간 장착 여부 불가
	EVENT_ERROR_EQUIPMENT_CHARA_COUNT									= 0x80050008,	// 캐릭터 갯수 오류
	EVENT_ERROR_EQUIPMENT_COMMON										= 0x80050009,	// 공용 장비 오류
	EVENT_ERROR_EQUIPMENT_CHARA_SLOT_IDX								= 0x8005000A,	// 캐릭터 Slot Idx 오류

	// 클랜
	EVENT_ERROR_PBV15_CLAN_MATCH_RESULT_EMOTION_DUPLE					= 0x80020000,	// 클랜 매치 기록 감정표현을 중복해서 했다.
	EVENT_ERROR_PBV15_CLAN_HONOR_MATCH_NOT_LEADER						= 0x80020001,	// 클랜 명예전 신청자가 팀리더가 아니다
	EVENT_ERROR_PBV15_CLAN_HONOR_MATCH_FAIL								= 0x80020002,	// 클랜 명예전 신청이 실패햇다
	EVENT_ERROR_PBV15_CLAN_HONOR_MATCH_SKIP								= 0x80020003,	// 클랜 명예전 신청이 무시되었다(이미 명예전 신청을 받은 상태)
	EVENT_ERROR_PBV15_CLAN_HONOR_MATCH_NOT_FIND_LEADER					= 0x80020004,	// 클랜 명예전 신청을 받을 리더를 찾지 못했다.
	EVENT_ERROR_PBV15_CLAN_OPERATION_FAIL								= 0x80020005,	// 클랜 작전명 전달이 실패했다

	EVENT_ERROR_PBV15_CLAN_MATCH_RESULT_ADD_FAIL						= 0x80020006,	// 클랜 매치 기록을 추가하지 못했다.(메모리문제)
	EVENT_ERROR_PBV15_CLAN_MATCH_RESULT_DEL_FAIL						= 0x80020007,	// 클랜 매치 기록을 삭제하지 못했다.(메모리문제)

	EVENT_ERROR_PBV15_CLAN_COMMISION_FAIL_RANK							= 0x80020008,	// 랭크제한으로 인해 클랜 마스터 위임 실패


	// Guide
	EVENT_ERROR_PBV15_GUIDE_FAIL										= 0x80030000,	// 가이드 기본 실패.
	EVENT_ERROR_PBV15_GUIDE_EVENT_IDX_RANGE_OVER						= 0x80030001,	// 가이드 이벤트 인덱스 범위 초과 
	EVENT_ERROR_PBV15_GUIDE_ALEADY_COMPLETE								= 0x80030002,	// 이미 완료된 가이드.

	// 보상 팝업
	EVENT_ERROR_PBV15_REWARD_POPUP_FAIL									= 0x80030100,	// 보상 에러 팝업

	// 제압모드
	EVENT_ERROR_PBV15_DOMI_LAST_WAVE_NANIB_FAIL							= 0x80030200,	// 마지막웨이브를 진행중에는 난입을 할수가 없다.

	// Shop 관련
	EVENT_ERROR_PBV15_SHOP_REPAIR_HACK_PACKET							= 0x80030300,	// 수리 요청 패킷 해킹.
	EVENT_ERROR_PBV15_SHOP_VERSION										= 0x80030301,	// 상점 버전 오류.

	//////////////////////////////////////////////////////////////////////////
	
	EVENT_ERROR_EVENT_LOG_IN_BLOCK_HEADER_MASK							= 0x80F00000,
	EVENT_ERROR_EVENT_LOG_IN_BLOCK_INNER								= 0x80100000,	// InnerBlock in GameBlock 
	EVENT_ERROR_EVENT_LOG_IN_BLOCK_OUTER								= 0x80200000,	// OuterBlock in GameBlock 
	EVENT_ERROR_EVENT_LOG_IN_BLOCK_RESERVED								= 0x80400000,	// 예약.
	EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME									= 0x80800000,	// GameBlock in PortalBlock


	EVENT_ERROR_EVENT_UNEXPECTED_VALUE									= 0x80002001,


	// [zion] 추가 메시지
	EVENT_RECEIVE_CLAN_NOTE												= 0x80003001,	// 클랜 전체 쪽지 도착
	EVENT_ERROR_EVENT_FAIL_INVITED_USER									= 0x80003002,	// 배틀룸에 초대된 유저가 초대 가능한 상태가 아님
	EVENT_ERROR_EVENT_FAIL_INVITED_USER_IN_CLAN_MATCH					= 0x80003003,	// 배틀룸에 초대된 유저가 클랜 서버에서 매치 매이킹 중이면 초대 가능한 상태가 아님
	EVENT_ERROR_EVENT_DUPLICATE_CHARA									= 0x80003004,	// 캐릭터 중복
	EVENT_ERROR_EVENT_EXIST_CHARA										= 0x80003005,	// 캐릭터 존재함
	EVENT_ERROR_EVENT_NOT_EXIST_CHARA									= 0x80003006,	// 캐릭터 존재하지 않음


	// 중국 빌링
	EVENT_ERROR_EVENT_LOAD_CASH_TIMEOUT									= 0x80000125,	// 중국 잔액조회 콜백 타임아웃
	EVENT_ERROR_EVENT_LOAD_CASH_RSP_SLOW								= 0x80000126,	// 중국 잔액조회 콜백 느려짐
	EVENT_ERROR_EVENT_LOAD_CASH_INVALID_BUFFER							= 0x80000127,	// 중국 잔액조회 콜백 버퍼 깨짐
	EVENT_ERROR_EVENT_LOAD_CASH_ADD_FAIL								= 0x80000128,	// 중국 잔액조회 콜백 버퍼 입력된 데이터 추가 실패
	EVENT_ERROR_EVENT_LOAD_CASH_REMOVE_FAIL								= 0x80000129,	// 중국 잔액조회 콜백 버퍼 완료된 데이터 삭제 실패
	EVENT_ERROR_EVENT_LOAD_CASH_INVALID_CASH_TYPE						= 0x80000130,	// 중국 잔액조회 콜백 받은 캐쉬의 화폐단위가 올바르지 않다.

	EVENT_ERROR_EVENT_CURRENCY_FREEZE_TIMEOUT							= 0x80000131,	// 중국 구매요청 콜백 타임아웃
	EVENT_ERROR_EVENT_CURRENCY_FREEZE_RSP_SLOW							= 0x80000132,	// 중국 구매요청 콜백 느려짐
	EVENT_ERROR_EVENT_CURRENCY_FREEZE_INVALID_BUFFER					= 0x80000133,	// 중국 구매요청 콜백 버퍼 깨짐
	EVENT_ERROR_EVENT_CURRENCY_FREEZE_ADD_FAIL							= 0x80000134,	// 중국 구매요청 콜백 버퍼 입력된 데이터 추가 실패
	EVENT_ERROR_EVENT_CURRENCY_FREEZE_REMOVE_FAIL						= 0x80000135,	// 중국 구매요청 콜백 버퍼 완료된 데이터 삭제 실패
	EVENT_ERROR_EVENT_CURRENCY_FREEZE_INVALID_CASH_TYPE					= 0x80000136,	// 중국 구매요청 콜백 받은 캐쉬의 화폐단위가 올바르지 않다.

	EVENT_ERROR_EVENT_CURRENCY_THAW_TIMEOUT								= 0x80000137,	// 중국 구매완료 콜백 타임아웃
	EVENT_ERROR_EVENT_CURRENCY_THAW_RSP_SLOW							= 0x80000138,	// 중국 구매완료 콜백 느려짐
	EVENT_ERROR_EVENT_CURRENCY_THAW_INVALID_BUFFER						= 0x80000139,	// 중국 구매완료 콜백 버퍼 깨짐
	EVENT_ERROR_EVENT_CURRENCY_THAW_ADD_FAIL							= 0x80000140,	// 중국 구매완료 콜백 버퍼 입력된 데이터 추가 실패
	EVENT_ERROR_EVENT_CURRENCY_THAW_REMOVE_FAIL							= 0x80000141,	// 중국 구매완료 콜백 버퍼 완료된 데이터 삭제 실패
	EVENT_ERROR_EVENT_CURRENCY_THAW_INVALID_CASH_TYPE					= 0x80000142,	// 중국 구매완료 콜백 받은 캐쉬의 화폐단위가 올바르지 않다.

	// Gacha
	EVENT_ERROR_GACHA_NOT_ENOUGH_CHARGE									= 0x80040000,	// 캐쉬, 이용권 부족
	EVENT_ERROR_GACHA_INVALID_REQUEST									= 0x80040001,	// 잘못된 요청
	EVENT_ERROR_GACHA_NO_RANDOM_ITEM									= 0x80040002,	// 랜덤 아이템이 설정되어 있지 않음
	EVENT_ERROR_GACHA_NOT_ENOUGH_AMOUNT									= 0x80040003,	// 해당 아이템의 남은 수량이 부족
	EVENT_ERROR_GACHA_SERVER_ERROR										= 0x80040004,	// 서버 내부 에러
	EVENT_ERROR_GACHA_INVALID_ITEMCOUNT									= 0x80040005,	// 인벤 내 가챠 티켓이 2개 이상으로 나뉘어져 있는 경우
	EVENT_ERROR_GACHA_FAIL_PAYMENT										= 0x80040006,	// 결제 실패

	// 방입장
	EVENT_ERROR_ROOM_JOIN_POS_CHARA_IDX									= 0x80040FFF,	// 방입장시 대표 캐릭 인덱스 오류.


	// QuickJoin
	EVENT_ERROR_QUICK_JOIN												= 0x80041000,	// 빠른입장 - 기본실패.
	EVENT_ERROR_QUICK_JOIN_UNKNOWN										= 0x80041001,	// 빠른입장 - 알 수 없는 이유.
	EVENT_ERROR_QUICK_JOIN_2ND_BEST										= 0x80041002,	// 빠른입장 - 차선책 권유.
	EVENT_ERROR_QUICK_JOIN_WRONG_INFO									= 0x80041003,	// 빠른입장 - 잘못된 설정 정보. 해킹.
	EVENT_ERROR_QUICK_JOIN_FAIL_SEARCH_ROOM								= 0x80041004,	// 빠른입장 - 방 검색 실패
	EVENT_ERROR_QUICK_JOIN_CHANNEL_FULL									= 0x80041005,	// 빠른입장 - 채널 이동 도중 채널 제한인원이 초과한 경우. 채널 선택 화면으로 이동 시킵니다.
	EVENT_ERROR_QUICK_JOIN_FAIL_ENTER_ROOM								= 0x80041006,	// 빠른입장 - 방 입장 실패
	EVENT_ERROR_QUICK_JOIN_WRONG_POSITION								= 0x80041007,	// 빠른입장 - 방에서 빠른 입장 요청을 한 경우.
	EVENT_ERROR_QUICK_JOIN_WRONG_INFO_2ND_BEST							= 0x80041008,	// 빠른입장 - 차선책 방입장시 잘못된 정보. 해킹. 

	// Shop
	EVENT_ERROR_SHOP_BUY_FAIL_ALEADY_EXIST_CHARA						= 0x80042000,	// 상점 - 이미 존재하는 캐릭터 슬롯을 구매하려고 해서 실패. 
	EVENT_ERROR_SHOP_BUY_FAIL_BASCKET_CHARA_BUY							= 0x80042001,	// 상점 - 장바구니 캐릭터 구매로 인한 실패.
	EVENT_ERROR_SHOP_CHARA_DEL_FAIL_SLOT_IDX							= 0x80042002,	// 캐릭터 삭제 실패 - Chara Slot Idx 오류.
	EVENT_ERROR_SHOP_CHARA_DEL_FAIL_SLOT_COUNT							= 0x80042003,	// 캐릭터 삭제 실패 - 캐릭터 슬롯 1개 이하.
	EVENT_ERROR_SHOP_CHARA_DEL_FAIL_NOT_ACTIVE							= 0x80042004,	// 캐릭터 삭제 실패 - 활성화된 캐릭터가 아니다.
	EVENT_ERROR_SHOP_CHARA_DEL_FAIL_NETWORK								= 0x80042005,	// 캐릭터 삭제 실패 - 네트워크 오류.
	EVENT_ERROR_SHOP_REPAIR_FAIL_NETWORK								= 0x80042006,	// 전체 수리 실패 - 네트워크 오류.
	EVENT_ERROR_SHOP_REPAIR_HALF_SUCCESS								= 0x80042007,	// 전체 수리 일부성공 일부실패. 성공한 품목은 수리완료. 실패한 품목은 수리비 차감X. 
	EVENT_ERROR_SHOP_EXTEND_WRONG_GOODS_TYPE							= 0x80042008,	// 아이템 기간 연장 실패 - 잘못된 굿즈 타입.
	EVENT_ERROR_SHOP_EXTEND_HACKING										= 0x80042009,	// 아이템 기간 연장 실패 - TWareDBIdx, Goods ID 해킹.
	EVENT_ERROR_SHOP_ITEM_TYPE_ERROR									= 0x8004200A,	// 아이템 타입 오류
	EVENT_ERROR_SHOP_NOT_FIND_ITEM										= 0x8004200B,	// 아이템 찾기 실패.
	EVENT_ERROR_SHOP_EXTEND_NO_HAVE_ITEM								= 0x8004200C,	// 아이템 기간 연장 실패 - 없는 아이템.
	EVENT_ERROR_SHOP_BUY_FAIL_LINK										= 0x8004200D,	// 링크 결제 실패
	EVENT_ERROR_SHOP_GET_CASH_FAIL_LINK									= 0x8004200E,	// 링크 잔액조회 실패
	EVENT_ERROR_SHOP_GIFT_AUTH_FAIL_HACK								= 0x8004200F,	// 선물 지급 실패 - 해킹	
	EVENT_ERROR_SHOP_BUY_FAIL											= 0x80042010,	// 구매 기본 실패
	EVENT_ERROR_SHOP_BUY_FAIL_RESTRICTION_MEDAL							= 0x80042011,	// 상점 - 구매제한걸린 메달 미소지로 인한 구매 실패. v1.5
	EVENT_ERROR_SHOP_BUY_FAIL_RESTRICTION_TITLE							= 0x80042012,	// 상점 - 구매제한걸린 호칭 미소지로 인한 구매 실패. v1.0
	EVENT_ERROR_SHOP_BUY_FAIL_NO_GOODS_PRICE							= 0x80042013,	// 상점 - 상품 가격 정보 없음
	EVENT_ERROR_SHOP_CHARA_DEL_FAIL_DEFAULT								= 0x80042014,	// 캐릭터 삭제 실패 - 기본 캐릭터
	

	// DataBase
	//EVENT_ERROR_DATABASE												= 0x80043000,	// 데이터베이스 - 기본실패
	EVENT_ERROR_DATABASE_EXECUTE_SP										= 0x80043001,	// 데이터베이스 - SP 실행 실패.
	EVENT_ERROR_DATABASE_SP_RESULT										= 0x80043002,	// 데이터베이스 - SP 실행 결과값 오류. Row Count, Rv.

	// 
	EVENT_ERROR_USER_WRONG_CHANNEL_IDX									= 0x80044001,	// 유저 채널 인덱스 에러.

	// 클라이언트 해킹 패킷. 
	EVENT_ERROR_USER_HACK_PACKET										= 0x80045001,	 
	EVENT_ERROR_HACKING_TOOL											= 0x80045002,	// **주의 : 울산 가드에 의한 해킹 탐지. 다른 용도로 사용 하지 마세요.

	// GM PAUSE
	EVENT_ERROR_GM_PAUSE_FAIL_REASON_C4_INSTALL							= 0x80046000,	// C4 설치중이거나 C4 해제중이므로 정지 불가능
	EVENT_ERROR_GM_PAUSE_FAIL_REASON_ALREADY_PAUSE						= 0x80046001,	// 이미 정지중이므로 정지 불가능
	EVENT_ERROR_GM_PAUSE_FAIL_REASON_U_R_NOT_GM							= 0x80046002,	// GM이 아니므로 정지 불가능
	EVENT_ERROR_GM_PAUSE_FAIL_REASON_TIME_LIMIT							= 0x80046003,	// 한번 정지후 재정지 할수 있는 시간까지 아직 되지않았으므로 정지 불가능
	EVENT_ERROR_GM_RESUME_FAIL_REASON_NOT_PAUSE							= 0x80046004,	// 정지상태가 아니므로 정지해제 불가능
	EVENT_ERROR_GM_RESUME_FAIL_REASON_U_R_NOT_LOCKER					= 0x80046005,	// 정지를 한 유저가 아니므로 정지해제 불가능

	// Gacha Error
	ERROR_GACHA_DIABLED_GACHA_SHOP										= 0x80047001,	// 사용할 수 없는 기능입니다.
	ERROR_GACHA_INVALID_ENTER_POSION									= 0x80047002,	// 현재 위치에서 입장할 수 없습니다.

	// 중독방지 시스템
	EVENT_ERROR_AAS_READY												= 0x80047000,	// AAS시간이 되어서 레디/시작 할 수 없음
	EVENT_ERROR_AAS_CREATE_ROOM											= 0x80047001,	// AAS시간이 되어서 방생성을 할 수 없음
	EVENT_ERROR_AAS_JOIN_ROOM											= 0x80047002,	// AAS시간이 되어서 방조인을 할 수 없음
	EVENT_ERROR_AAS_ENTER_SHOP											= 0x80047003,	// AAS시간이 되어서 상점에 들어갈 수 없음
	EVENT_ERROR_AAS_ENTER_INVEN											= 0x80047004,	// AAS시간이 되어서 인벤토리에 들어갈 수 없음
	EVENT_ERROR_AAS_FORCE_LOGOUT										= 0x80047005,	// AAS시간이 되어서 강제로 로그아웃 되었음

	EVENT_ERROR_AAS_ACCOUNT_APPOINTED_READY								= 0x80047010,	// 자녀관리 셧다운시간이 되어서 레디/시작 할 수 없음
	EVENT_ERROR_AAS_ACCOUNT_APPOINTED_CREATE_ROOM						= 0x80047011,	// 자녀관리 셧다운시간이 되어서 방생성을 할 수 없음
	EVENT_ERROR_AAS_ACCOUNT_APPOINTED_JOIN_ROOM							= 0x80047012,	// 자녀관리 셧다운시간이 되어서 방조인을 할 수 없음
	EVENT_ERROR_AAS_ACCOUNT_APPOINTED_ENTER_SHOP						= 0x80047013,	// 자녀관리 셧다운시간이 되어서 상점에 들어갈 수 없음
	EVENT_ERROR_AAS_ACCOUNT_APPOINTED_ENTER_INVEN						= 0x80047014,	// 자녀관리 셧다운시간이 되어서 인벤토리에 들어갈 수 없음
	EVENT_ERROR_AAS_ACCOUNT_APPOINTED_FORCE_LOGOUT						= 0x80047015,	// 자녀관리 셧다운시간이 되어서 강제로 로그아웃 되었음

	EVENT_ERROR_GMCHAT_USER_NOT_INSERVER								= 0x80048000,	// 유저가 현재 게임서버에 없습니다.
	EVENT_ERROR_GMCHAT_OTHER_GM_CONTACT									= 0x80048001,	// 다른 GM이 이미 대화중입니다.
	EVENT_ERROR_GMCHAT_USER_LOGOUT										= 0x80048002,	// 1:1대화중 유저의 접속이 끊어졌습니다.	
	EVENT_ERROR_GMCHAT_GM_LOGOUT										= 0x80048003,	// 1:1대화중 GM의 접속이 아닙니다.
	EVENT_ERROR_GMCHAT_RECIVER_NOTFOUND									= 0x80048004,	// 대화중인 GM이 없습니다.
	EVENT_ERROR_GMCHAT_USER_BLOCK										= 0x80048005,	// GM-USER 1:1 채팅에서 GM이 블럭하였습니다.

	// 빌링 연동
	EVENT_ERROR_BILLING_UNKNOWN											= 0x80048050,	// 알 수 없는 이유.
	EVENT_ERROR_BILLING_NOT_ENOUGH_PRCIE								= 0x80048051,	// 잔액부족.
	EVENT_ERROR_BILLING_NOT_FIND_USER									= 0x80048052,	// 유저를 찾을수 없음.
	EVENT_ERROR_BILLING_GET_CASH										= 0x80048053,	// 잔액조회 실패

	// 페이레터 쿠폰 시스템
	EVENT_ERROR_COUPON_WRONG_NUMBER										= 0x80049000,	// 쿠폰번호가 올바르지 않습니다.
	EVENT_ERROR_COUPON_ALREADY_USED										= 0x80049001,	// 해당 쿠폰은 이미 사용된 쿠폰입니다.
	EVENT_ERROR_COUPON_EXPIRED											= 0x80049002,	// 사용기간이 만료된 쿠폰입니다.
	EVENT_ERROR_COUPON_WRONG_USE_PCCAFE_COUPON							= 0x80049003,	// PC방 쿠폰을 가맹PC방이 아닌곳에서 사용을 시도함
	EVENT_ERROR_COUPON_WRONG_USE_MOBILE_COUPON							= 0x80049004,	// 모바일전용 쿠폰을 입력한경우
	EVENT_ERROR_COUPON_UNKNOWN											= 0x80049005,	// 알 수 없는 쿠폰 에러.

	// 확성기
	EVENT_ERROR_MEGAPHONE_DELAY_TIME									= 0x80050000,	// 확성기 사용 딜레이로 인해 사용할 수 없습니다.

	EVENT_ERROR_DISGUISE_RANK_RANGE_OUT									= 0x80050300,	// 위장계급에서 사용가능한 계급범위를 벗어났습니다.
	EVENT_ERROR_DISGUISE_RANK_NOT_APRIL_FOOL							= 0x80050301,	// 만우절이 아닌 날에 만우절 위장계급을 사용했습니다.
	
	// ClanWar
	EVENT_ERROR_CLAN_WAR_SET_SLOT										= 0x80050100,	// 슬롯 할당 실패.
	EVENT_ERROR_CLAN_WAR_ALLOC_TEAM_MEMBER								= 0x80050101,	// 팀 멤버 할당 실패.
	EVENT_ERROR_CLAN_WAR_NOT_LEADER										= 0x80050102,	// 팀 리더가 아님.
	EVENT_ERROR_CLAN_WAR_LEAVE_ALL_MATCH_ROOM_FOR_LEADER				= 0x80050103,	// 팀 리더의 매치룸 퇴장으로 인해 팀원 모두 매치룸 퇴장.
	EVENT_ERROR_CLAN_WAR_NOT_FIND_MERCENARY								= 0x80050104,	// 용병 검색 실패.
	EVENT_ERROR_CLAN_WAR_WRONG_STATE									= 0x80050105,	// STATE 오류.
	EVENT_ERROR_CLAN_WAR_INVITE_TEAM									= 0x80050106,	// 용병 초대한 팀이 아니다.
	EVENT_ERROR_CLAN_WAR_PER_COUNT										= 0x80050107,	// 인원수 오류
	EVENT_ERROR_CLAN_WAR_MERCENAY										= 0x80050108,	// 용병이라서 실패.
	EVENT_ERROR_CLAN_WAR_TIME_OUT										= 0x80050109,	// 타임아웃
	EVENT_ERROR_CLAN_WAR_HAVE_MATCH_TEAM								= 0x8005010A,	// 매치팀이 존재합니다.
	EVENT_ERROR_CLAN_WAR_MERCENAY_DIS_TIME								= 0x8005010B,	// 용병 이탈 시간제한으로 실패.
	EVENT_ERROR_CLAN_WAR_CANCEL_MATCHMAKING								= 0x8005010C,	// 매치 취소( 테스트용 )

	// S2MO Dat
	EVENT_ERROR_S2MODATFILE_DELETE_FILE									= 0x80050200,	// 파일 삭제  실패
	EVENT_ERROR_S2MODATFILE_NAME_OVER									= 0x80050201,	// 파일명 길이 초과
	EVENT_ERROR_S2MODATFILE_DUPLICATE_NAME								= 0x80050202,	// 파일명 중복
	EVENT_ERROR_S2MODATFILE_CREATE_FILE									= 0x80050203,	// 파일 생성 실패
	EVENT_ERROR_S2MODATFILE_FIND_FILE									= 0x80050204,	// 파일 검색 실패
	EVENT_ERROR_S2MODATFILE_USING_FILE									= 0x80050205,	// 사용중인 파일
	
	EVENT_ERROR_GENERAL_RANKUP_OUT_BUFFER_FULL							= 0x80050300,	// 장성계급 결과전달용 버퍼가 꽉참
	EVENT_ERROR_GENERAL_RANKUP_DB_ERROR									= 0x80050301,	// 장성계급 DB Update시 발생

	// etc
	EVENT_ERROR_DISABLE_CONTENTS										= 0x80099100,	// 해당 컨텐츠가 OFF 되어 있습니다.
	EVENT_ERROR_NETWORK													= 0x80099101,	// 네트워크오류
	EVENT_ERROR_NOT_FIND_USER											= 0x80099102,	// 유저 찾기 실패
	EVENT_ERROR_WRONG_SLOT_IDX											= 0x80099103,	// 슬롯 인덱스 오류
	EVENT_ERROR_CHATTING_LIMIT											= 0x80099104,	// 중복 채팅 제한
	EVENT_ERROR_MATCH_MAKING_ROOM										= 0x80099105,	// 매치메이킹 룸.



	EVENT_ERROR_HACKING_USER											= 0x80010000,	// 유저가 해킹을 하여 팅겨냅니다. ( 뒷 2바이트는 핵 코드가 들어갈 예정입니다. )

	// 호칭 관련

	EVENT_ERROR_TITLE_EQUIP_DUPLICATE_CATEGORY							= 0x80002400,  // 같은 계열 호칭을 이미 착용하고 있습니다.(1.0/1.1머징)
	
	//선물샵
	EVENT_ERROR_GIFT_SHOP_ENTER_RANK_FAIL								= 0x80002500,  // 선물샵 상점 랭크 제한으로 입장 실패

	EVENT_ERROR_TITLE_EQUIP_SLOTIDX										= 0x80002600,  // 호칭 슬롯 제한
};

#endif