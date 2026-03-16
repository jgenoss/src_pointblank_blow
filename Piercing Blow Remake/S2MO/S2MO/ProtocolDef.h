#ifndef __PROTOCOLDEF_H__
#define __PROTOCOLDEF_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Protocol
#define	MAX_PROTOCOL_BASE_VALUE		64

enum PROTOCOL_BASE_VALUE
{	// *** 반드시 0x0100 단위로 증가시켜주세요***
	PROTOCOL_VALUE_LOGIN = 0x0100,
	PROTOCOL_VALUE_BASE = 0x0200,
	PROTOCOL_VALUE_AUTH = 0x0300,
	PROTOCOL_VALUE_SHOP = 0x0400,
	PROTOCOL_VALUE_TEST = 0x0500,
	PROTOCOL_VALUE_ASC = 0x0600,
	PROTOCOL_VALUE_CLAN = 0x0700,
	PROTOCOL_VALUE_CLAN_MATCH = 0x0800,	// 1.0 클랜 매치메이킹
	PROTOCOL_VALUE_BATTLE_SERVER = 0x0900,
	PROTOCOL_VALUE_MESSAGE = 0x0A00,
	PROTOCOL_VALUE_COMMUNITY = 0x0B00,
	PROTOCOL_VALUE_LOBBY = 0x0C00,
	PROTOCOL_VALUE_INVENTORY = 0x0D00,
	PROTOCOL_VALUE_RS_IGS = 0x0E00,	// 1.0 뽑기상점, 야전상점
	PROTOCOL_VALUE_ROOM = 0x0F00,
	PROTOCOL_VALUE_BATTLE = 0x1000,
	PROTOCOL_VALUE_NEW = 0x1100,
	PROTOCOL_VALUE_MEDAL = 0x1200,
	PROTOCOL_VALUE_CHEAT = 0x1300,
	PROTOCOL_VALUE_GACHA = 0x1400,
	PROTOCOL_VALUE_QUICKJOIN = 0x1500,
	PROTOCOL_VALUE_VALUE_ZLOG = 0x1600,
	PROTOCOL_VALUE_SKILL = 0x1700,
	PROTOCOL_VALUE_CHAR = 0x1800,
	PROTOCOL_VALUE_MYINFO = 0x1900,
	PROTOCOL_VALUE_GMCHAT = 0x1A00,
	PROTOCOL_VALUE_CLAN_WAR = 0x1B00,	// 1.5 클랜 매치, 시즌전, 용병시스템
	PROTOCOL_VALUE_KNOCK = 0x1C00,
};

enum Protocol_Login
{
	PROTOCOL_LOGIN = PROTOCOL_VALUE_LOGIN,

	PROTOCOL_LOGIN_REQ,								// 로그인
	PROTOCOL_LOGIN_WEBKEY_REQ,						// 웹 로그인

	PROTOCOL_LOGIN_ACK,
};

enum Protocol_Base
{
	PROTOCOL_BASE = PROTOCOL_VALUE_BASE,

	PROTOCOL_BASE_CONNECT_REQ,
	PROTOCOL_BASE_CONNECT_ACK,

	PROTOCOL_BASE_LOGOUT_REQ,						// 로그아웃 요청 
	PROTOCOL_BASE_LOGOUT_ACK,						// 로그아웃 응답

	PROTOCOL_BASE_HEART_BIT_REQ,					// Heart Bit
	PROTOCOL_BASE_HEART_BIT_ACK,

	PROTOCOL_BASE_GAMEGUARD_REQ,					//게임 가드 패킷 : 서버가 보냄 
	PROTOCOL_BASE_GAMEGUARD_ACK,					//게임 가드 패킷 : 클라이언트가 보냄 

	PROTOCOL_BASE_LOGIN_WAIT_ACK,					// 로그인 딜레이 패킷

	PROTOCOL_BASE_GET_SYSTEM_INFO_REQ,				// 시스템 정보 받기
	PROTOCOL_BASE_GET_SYSTEM_INFO_ACK,

	PROTOCOL_BASE_GET_USER_INFO_REQ,				// 자신의 정보 받기
	PROTOCOL_BASE_GET_USER_INFO_ACK,

	PROTOCOL_BASE_GET_INVEN_INFO_REQ,				// 자신의 인벤토리 받기.
	PROTOCOL_BASE_GET_INVEN_INFO_ACK,

	PROTOCOL_BASE_GET_OPTION_REQ,					// 게임 옵션 받기 -- 로그인 후 한번만 받습니다.
	PROTOCOL_BASE_GET_OPTION_ACK,

	PROTOCOL_BASE_OPTION_SAVE_REQ,					//키 조작하는것을 세이브합니다. 
	PROTOCOL_BASE_OPTION_SAVE_ACK,

	PROTOCOL_BASE_CHECK_NICK_NAME_REQ,				// 닉네임 확인(중복확인)
	PROTOCOL_BASE_CHECK_NICK_NAME_ACK,

	PROTOCOL_BASE_CREATE_NICK_REQ,					// 닉네임 설정
	PROTOCOL_BASE_CREATE_NICK_ACK,

	PROTOCOL_BASE_USER_LEAVE_REQ,					//Change Server *서버에서 *서버로 갈때.
	PROTOCOL_BASE_USER_LEAVE_ACK,

	PROTOCOL_BASE_USER_ENTER_REQ,					//Change Server *서버에서 *서버로 갈때.
	PROTOCOL_BASE_USER_ENTER_ACK,

	PROTOCOL_BASE_GET_CHANNELLIST_REQ,				// 채널 리스트를 얻습니다.
	PROTOCOL_BASE_GET_CHANNELLIST_ACK,

	PROTOCOL_BASE_SELECT_CHANNEL_REQ,				// 채널 선택	
	PROTOCOL_BASE_SELECT_CHANNEL_ACK,

	// 추후 통합 예정임..
	// 출석체크 이벤트 - 서동권 2011.07.06
	PROTOCOL_BASE_ATTENDANCE_REQ,					// 출석체크 이벤트 출석 요청
	PROTOCOL_BASE_ATTENDANCE_ACK,					// 출석체크 이벤트 출석 결과
	PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ,		// 출석체크 이벤트 완료 보상 아이템 요청
	PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK,		// 출석체크 이벤트 완료 보상 아이템 지급 결과

	// Guide 시스템은 추후에 프로토콜 변경 부탁~
	// Guide
	PROTOCOL_BASE_GUIDE_COMPLETE_REQ,			// 가이드 완료 요청.
	PROTOCOL_BASE_GUIDE_COMPLETE_ACK,			// 가이드 완료 응답.

	PROTOCOL_BASE_RANK_UP_REQ,						// ! 사용안함
	PROTOCOL_BASE_RANK_UP_ACK,						// 진급 포인트 지급

	////////////////////////////////////////////////////////////////////////////////
	// 여기까지 맞춤
	////////////////////////////////////////////////////////////////////////////////





	PROTOCOL_BASE_GET_EQUIPMENT_INFO_REQ,			//사용하지 않음 //추후 사용될 가능성 있음 
	PROTOCOL_BASE_GET_EQUIPMENT_INFO_ACK,
	PROTOCOL_BASE_GET_MEDALSYSTEM_REQ,				//메달시스템 정보 받기 
	PROTOCOL_BASE_GET_MEDALSYSTEM_ACK,
	PROTOCOL_BASE_REFRESH_MEDALSYSTEM_REQ,			//메달 시스템 갱신 하기 NEW표시를 삭제함
	PROTOCOL_BASE_REFRESH_MEDALSYSTEM_ACK,

	PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ,				//레코드 정보 요청하기
	PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK,				//레코드 정보 

	//Quest 
	PROTOCOL_BASE_QUEST_GET_REQ,					//퀘스트 내용 보내기
	PROTOCOL_BASE_QUEST_GET_ACK,
	PROTOCOL_BASE_QUEST_GET_INFO_REQ,				//퀘스트 정보 보내기 
	PROTOCOL_BASE_QUEST_GET_INFO_ACK,
	PROTOCOL_BASE_QUEST_GET_ITEM_REQ,				//퀘스트 보상 보내기 
	PROTOCOL_BASE_QUEST_GET_ITEM_ACK,
	PROTOCOL_BASE_QUEST_CHANGE_REQ,					//변경
	PROTOCOL_BASE_QUEST_CHANGE_ACK,
	PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_REQ,
	PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK,

	//For Medal
	PROTOCOL_BASE_MEDAL_GET_INFO_REQ,
	PROTOCOL_BASE_MEDAL_GET_INFO_ACK,

	// Buy/Delete (Quest) Card
	PROTOCOL_BASE_QUEST_BUY_CARD_SET_REQ,			// 카드셋 구입
	PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK,
	PROTOCOL_BASE_QUEST_DELETE_CARD_SET_REQ,		// 카드셋 삭제
	PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK,

	PROTOCOL_BASE_GET_MYINFO_RECORD_REQ,			//										사용하지 않음
	PROTOCOL_BASE_GET_MYINFO_RECORD_ACK,			// 자신의 레코드 정보를 얻음

	PROTOCOL_BASE_GET_MYINFO_BASIC_REQ,				// 사용하지 않음
	PROTOCOL_BASE_GET_MYINFO_BASIC_ACK,				// 자신의 기본 정보를 얻음



	PROTOCOL_BASE_INITIAL_RANK_UP_REQ,				// ! 사용안함
	PROTOCOL_BASE_INITIAL_RANK_UP_ACK,				// 하사이상 초도 진급 포인트

	PROTOCOL_BASE_REGIST_TRAINING_SCORE_REQ,		// 사격장 점수 등록하기
	PROTOCOL_BASE_REGIST_TRAINING_SCORE_ACK,		// ! 사용안함

	// 호칭 시스템
	PROTOCOL_BASE_USER_TITLE_CHANGE_REQ,			// 호칭 교환 요청	- 획득 LogD
	PROTOCOL_BASE_USER_TITLE_CHANGE_ACK,			// 호칭 교환 응답

	PROTOCOL_BASE_USER_TITLE_EQUIP_REQ,				// 호칭 장비 요청	- 장착 LogD
	PROTOCOL_BASE_USER_TITLE_EQUIP_ACK,				// 호칭 장비 응답

	PROTOCOL_BASE_USER_TITLE_RELEASE_REQ,			// 호칭 해제 요청	- 해제 LogD
	PROTOCOL_BASE_USER_TITLE_RELEASE_ACK,			// 호칭 해제 응답

	PROTOCOL_BASE_USER_TITLE_INFO_REQ,				// ! 사용안함
	PROTOCOL_BASE_USER_TITLE_INFO_ACK,				// 호칭 정보 받기

	PROTOCOL_BASE_CHATTING_REQ,						//채팅 
	PROTOCOL_BASE_CHATTING_ACK,

	// 랑데뷰 서버
	PROTOCOL_BASE_RENDEZVOUS_NAT_STATE_REQ,			// NAT 상태 요청
	PROTOCOL_BASE_RENDEZVOUS_NAT_STATE_ACK,			// NAT 상태 응답

	PROTOCOL_BASE_RENDEZVOUS_HOLEPUNCH_REQ,			// 랑데뷰 홀펀칭 요청
	PROTOCOL_BASE_RENDEZVOUS_HOLEPUNCH_ACK,			// 랑데뷰 홀펀칭 응답

	PROTOCOL_BASE_GET_MYINFO_ALL_REQ,				// ! 사용안함
	PROTOCOL_BASE_GET_MYINFO_ALL_ACK,				// BASIC + RECORD

	PROTOCOL_BASE_QUEST_FINISH_NOTICE_REQ,			// 퀘스트 완료 notice
	PROTOCOL_BASE_QUEST_FINISH_NOTICE_ACK,			// 퀘스트 완료 notice					사용하지 않음  

	PROTOCOL_BASE_INV_ITEM_DATA_REQ,				// 인벤 아이템 요청			// 미사용
	PROTOCOL_BASE_INV_ITEM_DATA_ACK,				// 인벤 아이템 응답

	PROTOCOL_BASE_GET_RECORD_INFO_SESSION_REQ,		//레코드 정보 요청하기
	PROTOCOL_BASE_GET_RECORD_INFO_SESSION_ACK,		//레코드 정보

	PROTOCOL_BASE_NOT_USE_UPDATE_SERVER,			//임시 사용.. 성백

	PROTOCOL_BASE_GAME_SERVER_STATE_REQ,			// 서버 상태 요청
	PROTOCOL_BASE_GAME_SERVER_STATE_ACK,			// 서버 상태 응답

	PROTOCOL_BASE_ENTER_PASS_REQ,					// 서버 입장 비번 물어보기 요청
	PROTOCOL_BASE_ENTER_PASS_ACK,					// 서버 입장 비번 물어보기 응답

	PROTOCOL_BASE_GET_RECORD_INFO_SEASON_REQ,		//레코드 정보 요청하기
	PROTOCOL_BASE_GET_RECORD_INFO_SEASON_ACK,		//레코드 정보

	PROTOCOL_BASE_GET_USERINFO_ALL_REQ,				// 유저 상세 정보 보기 요청(point 차감)
	PROTOCOL_BASE_GET_USERINFO_ALL_ACK,				// 유저 상세 정보 보기 응답

	PROTOCOL_BASE_GET_USERINFO_ALL_DB_REQ,			// 유저 상세 정보 보기 Trans에서 가져오기(Game->Trans)
	PROTOCOL_BASE_GET_USERINFO_ALL_DB_ACK,			// 유저 상세 정보 보기 온라인 상태에서만 전송(Trans->Game) 	

	//PROTOCOL_BASE_GET_MYCLAN_REQ,					// 자신의 클랜 정보 받기 -- 로그인 후 한번만 받습니다.
	//PROTOCOL_BASE_GET_MYCLAN_ACK,

	PROTOCOL_BASE_SUPPLY_BOX_ANNOUNCE_ACK,			// 보급상자 수량, 초기화 시간 등
	PROTOCOL_BASE_SUPPLY_BOX_PRESENT_MESSAGE_ACK,	// 보급상자 지급 알림 메시지
	PROTOCOL_BASE_SUPPLY_BOX_PRESENT_ACK,			// 보급상자 지급자에게 팝업

	PROTOCOL_BASE_HACKING_CHECK_KICK_REQ,			// 클라이언트에서 해킹 체크시 서버에 로그를 남기기위해 요청

	// 브라질 XTrap 정상작동 테스트를 위하여 추가 - 서버에서 핵실드 확인을 위해 보내온 ACK 에 REQ 를 보내기 위해 핵실드 함수를 호출하는데 그 함수 처리 시간이 5초이상 걸릴경우 보내옵니다.
	PROTOCOL_BASE_GAMEGUARD_FAIL_NOACK,				// 클라이언트에서 일방적으로 보냅니다. ACK 필요없음

	PROTOCOL_BASE_DAILY_RECORD_REQ,					// 당일 전적정보 요청 Client -> Game (종료 이벤트 후[ex] : 종료하기 버튼, Alt + F4)
	PROTOCOL_BASE_DAILY_RECORD_ACK,					// 당일 전적정보 응답 Game -> Client

	PROTOCOL_BASE_GET_USER_LIST_REQ,				// 유저리스트 요청
	PROTOCOL_BASE_GET_USER_LIST_ACK,				// 유저리스트 요청

	PROTOCOL_BASE_EQUIPMENT_REQ,					// 유저 아이템 장착 패킷
	PROTOCOL_BASE_EQUIPMENT_ACK,					// 유저 아이템 장착 패킷

	PROTOCOL_BASE_LOGIN_THAI_REQ,					// 태국 가레나 로그인

	PROTOCOL_BASE_SERVER_USER_NOT_FIND,				// 이 프로토콜에대한 자세한 문의는 김현우 에게 물어보세요

	//////////////////////////////////////////////////////////////////////////
	// 유저 정보 보기
	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_BASE_GET_USER_INFO_LOBBY_REQ,				// 동일서버 유저 간략 정보 요청
	PROTOCOL_BASE_GET_USER_INFO_ROOM_REQ,				// 대기방   유저 간략 정보 요청
	PROTOCOL_BASE_GET_USER_INFO_UID_REQ,				// UID로	 유저 간략 정보 요청
	PROTOCOL_BASE_GET_USER_INFO_NICK_REQ,				// 닉네임으로 유저 간략 정보 요청
	PROTOCOL_BASE_GET_USER_BASIC_INFO_ACK,

	//////////////////////////////////////////////////////////////////////////
	// 왼쪽 화면의 화살표로 대표 캐릭 설정
	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_REQ,			// (사용안함)대표캐릭설정 요청
	PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_ACK,			// (사용안함)대표캐릭설정 응답

	//////////////////////////////////////////////////////////////////////////
	// 보상처리 팝업
	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_BASE_NEW_REWARD_POPUP_ACK,				// 보상 팝업

	PROTOCOL_BASE_SERVER_INFO_REQ,					// 서버 정보 얻어오기
	PROTOCOL_BASE_SERVER_INFO_ACK,

	PROTOCOL_BASE_BATTLE_LOG,

	// 아래 프로토콜에 대한 문의는 DU 팀으로 주시기 바랍니다.- 서동권
	//////////////////////////////////////////////////////////////
	//    D  U - 수정 시 DU 팀에 반드시 먼저 문의 후 작업
	//////////////////////////////////////////////////////////////
	PROTOCOL_BASE_HEART_BIT_2,
	PROTOCOL_BASE_READY_HEART_BIT_REQ,
	PROTOCOL_BASE_READY_HEART_BIT_ACK,
	//////////////////////////////////////////////////////////////

	PROTOCOL_BASE_FIND_UID_GET_USER_DETAIL_INFO_REQ,		// 사용하지 않음
	PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK,					// 유저 상세 정보 응답

	PROTOCOL_BASE_CHANGE_CLAN_MARK_REQ,						// 클랜 마크 변경. Game -> Trans. ACK 필요없음.
	PROTOCOL_BASE_CHANGE_CLAN_NAME_REQ,						// 클랜 마크 변경. Game -> Trans. ACK 필요없음.

	PROTOCOL_BASE_GET_UID_LOBBY_REQ,						// GM 이 유저를 이용해서 UID 를 얻어 옵니다.
	PROTOCOL_BASE_GET_UID_LOBBY_ACK,						// GM 이 유저를 이용해서 UID 를 얻어 옵니다.

	PROTOCOL_BASE_GET_UID_ROOM_REQ,							// GM 이 유저를 이용해서 UID 를 얻어 옵니다.
	PROTOCOL_BASE_GET_UID_ROOM_ACK,							// GM 이 유저를 이용해서 UID 를 얻어 옵니다.

	PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_REQ,
	PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_ACK,					// 유저에게 변경된 PC방 혜택값을 전달합니다.

	PROTOCOL_BASE_GET_USER_SUBTASK_REQ,						// 유저 현재 위치 조회 요청
	PROTOCOL_BASE_GET_USER_SUBTASK_ACK,						// 유저 현재 위치 조회 응답

	PROTOCOL_BASE_MEGAPHONE_REQ,							// 확성기 사용요청 - 차후 기능확장시 사용예정. 현재는 기존 PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ 프로토콜 사용
	PROTOCOL_BASE_MEGAPHONE_ACK,							// 확성기 내용 전달

	// 장성계급 실시간 반영 통지
	PROTOCOL_BASE_GENERALRANK_CHANGE_ACK,

	PROTOCOL_BASE_GET_CHARA_INFO_REQ,						// 자신의 캐릭터 받기
	PROTOCOL_BASE_GET_CHARA_INFO_ACK,

	PROTOCOL_BASE_CLIENT_END_TYPE_REQ,						// 클라이언트 종료 사유.
	PROTOCOL_BASE_NOTICE_ACK,								// 공지사항 전달
	PROTOCOL_BASE_EVENT_ITEM_ACK,							// 선물함 기능이 꺼져 있을 인벤에 들어갔다는 것을 알려줍니다.};

	PROTOCOL_BASE_MAP_VERSION_REQ,							// 맵 리스트 버전 요청
	PROTOCOL_BASE_MAP_VERSION_ACK,
	PROTOCOL_BASE_MAP_LIST_REQ,								// 맵 리스트 전송 요청
	PROTOCOL_BASE_MAP_LIST_ACK,
	PROTOCOL_BASE_MAP_RULELIST_REQ,
	PROTOCOL_BASE_MAP_RULELIST_ACK,							// RuleList 응답
	PROTOCOL_BASE_MAP_MATCHINGLIST_REQ,
	PROTOCOL_BASE_MAP_MATCHINGLIST_ACK,						// Rule-Stage 매칭리스트 응답
	PROTOCOL_BASE_URL_LIST_REQ,
	PROTOCOL_BASE_URL_LIST_ACK,								// ULR List 전달
	PROTOCOL_BASE_MAP_RANDOM_LIST_ACK,						// 랜덤맵 데이터 전달.
	PROTOCOL_BASE_MAP_RANDOM_INDEX_ACK,						// 랜덤맵 인덱스 데이터 전달.
	PROTOCOL_BASE_BOOSTEVENT_INFO_ACK,						// 부스트이벤트 경험치, 포인트 전달.
};

enum Protocol_Auth
{
	PROTOCOL_AUTH = PROTOCOL_VALUE_AUTH,

	PROTOCOL_AUTH_LOGIN_REQ,				// 로그인
	PROTOCOL_AUTH_LOGIN_WEBKEY_REQ,			//
	PROTOCOL_AUTH_LOGIN_ACK,				//

	PROTOCOL_AUTH_LOGOUT_REQ,				// 로그아웃
	PROTOCOL_AUTH_LOGOUT_ACK,

	PROTOCOL_AUTH_GET_MYINFO_REQ,			// 개인 정보 요청
	PROTOCOL_AUTH_GET_MYINFO_ACK,

	PROTOCOL_AUTH_USER_LEAVE_REQ,			// 유저의 서버 이동 시작
	PROTOCOL_AUTH_USER_LEAVE_ACK,			// 유저의 서버 이동 응답

	PROTOCOL_AUTH_USER_ENTER_REQ,			// 유저의 서버 이동 완료
	PROTOCOL_AUTH_USER_ENTER_ACK,			// 유저의 서버 이동 응답

	PROTOCOL_AUTH_ANNOUNCE,					//AdminGateWay공지 
	PROTOCOL_AUTH_ANNOUNCE_ROOM,			//ROOM에 있는 유저만 공지 
	PROTOCOL_AUTH_ROOM_DESTROY,				//AdminGateWay Room 파괴

	PROTOCOL_AUTH_USER_NOW_INFO_REQ,		//유저상태를 요구하기 	
	PROTOCOL_AUTH_USER_NOW_INFO_ACK,

	PROTOCOL_AUTH_FRIEND_INFO_REQ,				// 친구정보 요청하기 
	PROTOCOL_AUTH_FRIEND_INFO_ACK,				// 요청할수도 있고, 로그인후 자동으로 받을수도 있음

	PROTOCOL_AUTH_FRIEND_INVITED_REQ,			// 배틀룸에 친구초대 요청
	PROTOCOL_AUTH_FRIEND_INVITED_ACK,			// 배틀룸에 친구초대 실패시 발신자에게 전달
	PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK,	// 배틀룸에 친구초대 성공시 수신자에게 전달

	////////////////////////////////////////////////////////////////////////////
	//						GSM Bridge Listen Port							  //
	PROTOCOL_GSM_ACCOUNT_KICK_REQ,			// GSM에서 연결을 끊었습니다.

	PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK,	//상태 변경시 전달됨

	PROTOCOL_AUTH_FRIEND_ACCEPT_REQ,		//친구 등록, 삭제도 여기에 포함됨
	PROTOCOL_AUTH_FRIEND_ACCEPT_ACK,		//
	PROTOCOL_AUTH_FRIEND_INSERT_REQ,		//친구 넣기  
	PROTOCOL_AUTH_FRIEND_INSERT_ACK,		//
	PROTOCOL_AUTH_FRIEND_DELETE_REQ,		//친구 삭제(친구 거절)
	PROTOCOL_AUTH_FRIEND_DELETE_ACK,		//

	PROTOCOL_AUTH_FRIEND_ROOM_ENTER_REQ,	//방에 들어가기 
	PROTOCOL_AUTH_FRIEND_ROOM_ENTER_ACK,

	PROTOCOL_AUTH_FRIEND_ROOM_LEAVE_REQ,	//방에서 나가기 
	PROTOCOL_AUTH_FRIEND_ROOM_LEAVE_ACK,

	PROTOCOL_AUTH_SEND_WHISPER_REQ,			// 귓속말 보내기 요청
	PROTOCOL_AUTH_SEND_WHISPER_ACK,			// 귓속말 보내기 응답 - 실패

	PROTOCOL_AUTH_SEND_WHISPER_FIND_UID_REQ,	// Receiver UID로 귓속말 보내기 요청
	//PROTOCOL_AUTH_SEND_WHISPER_ACK,			// 귓속말 보내기 응답 - 실패

	PROTOCOL_AUTH_RECV_WHISPER_REQ,			// ! 사용안함
	PROTOCOL_AUTH_RECV_WHISPER_ACK,			// 귓속말 받기

	//PROTOCOL_AUTH_DELETE_CONNECT_INFO_ACK 중계서버가 확인을 위해서 게임서버에 보냄 게임서버에서 유저가 있으면 아무런 일을 하지 않고, 없다면 PROTOCOL_AUTH_DELETE_CONNECT_INFO_REQ을 보냄
	PROTOCOL_AUTH_DELETE_CONNECT_INFO_REQ,	// 접속상태 제거 요청
	PROTOCOL_AUTH_DELETE_CONNECT_INFO_ACK,	// 중계서버가 보냄 

	PROTOCOL_AUTH_FIND_USER_REQ,			// 유저찾기 요청
	PROTOCOL_AUTH_FIND_USER_ACK,			// 유저찾기 응답

	PROTOCOL_AUTH_CHANGE_NICKNAME_REQ,		// 닉네임 변경 통보
	PROTOCOL_AUTH_CHANGE_NICKNAME_ACK,		// ! 사용안함

	PROTOCOL_AUTH_RESET_FRIEND_INFO_REQ,	// 친구목록 초기화 요청
	PROTOCOL_AUTH_RESET_FRIEND_INFO_ACK,	// ! 사용안함

	PROTOCOL_AUTH_GET_REMAIN_MONEY_REQ,		// 보유 포인트, 톨 받기 요청 - 사용안함
	PROTOCOL_AUTH_GET_REMAIN_MONEY_ACK,		// 보유 포인트, 톨 받기 응답 - 사용안함

	PROTOCOL_AUTH_CHANGE_RANK_REQ,			// 랭크 변경 통보
	PROTOCOL_AUTH_CHANGE_RANK_ACK,			// ! 사용안함

	PROTOCOL_AUTH_CHANGE_COLOR_NICK_REQ,	// 칼라 닉네임
	PROTOCOL_AUTH_CHANGE_COLOR_NICK_ACK,	// ! 사용안함

	PROTOCOL_AUTH_BLOCK_INSERT_REQ,			// 차단 추가 요청
	PROTOCOL_AUTH_BLOCK_INSERT_ACK,			// 차단 추가 응답

	PROTOCOL_AUTH_BLOCK_DELETE_REQ,			// 차단 삭제 요청
	PROTOCOL_AUTH_BLOCK_DELETE_ACK,			// 차단 삭제 요청

	PROTOCOL_AUTH_UPDATE_BASICINFO_REQ,
	PROTOCOL_AUTH_UPDATE_BASICINFO_ACK,		//사용하지 않음

	PROTOCOL_AUTH_UPDATE_RECORD_REQ,
	PROTOCOL_AUTH_UPDATE_RECORD_ACK,		//사용하지 않음

	PROTOCOL_AUTH_UPDATE_EQUIP_REQ,
	PROTOCOL_AUTH_UPDATE_EQUIP_ACK,			//사용하지 않음			

	PROTOCOL_AUTH_UPDATE_MEDAL_REQ,
	PROTOCOL_AUTH_UPDATE_MEDAL_ACK,			//사용하지 않음

	PROTOCOL_GSM_ACCOUNT_KICK_ACK,			// GSM에서 연결을 끊었습니다.

	PROTOCOL_AUTH_FCM_INFO_REQ,				// 중국 실명 인증 정보 체크(클라이언트 실명인증 완료시 전송)
	PROTOCOL_AUTH_FCM_INFO_ACK,				// 응답(성공시 피로도 적용)(실패시 클라이언트단에 전달.클라이언트단은 실명인증 브라우저 실행)

	//PROTOCOL_AUTH_FCM_LOGIN_REQ,			// 중국 피로도 로그인 요청(사용안함)
	//PROTOCOL_AUTH_FCM_LOGIN_ACK,			// 응답(서버에서만 사용)

	PROTOCOL_AUTH_FCM_LOGOUT_REQ,			// 중국 피로도 로그아웃 요청(게임서버에서 트랜스로 전송)
	//PROTOCOL_AUTH_FCM_LOGOUT_ACK,			// 사용안함(이미 클라이언트 종료)

	PROTOCOL_AUTH_FCM_STATE_REQ,			// 중국 피로도 상태 체크(게임서버에서 트랜스로 전송)(사용안함)
	PROTOCOL_AUTH_FCM_STATE_ACK,			// 응답

	//// 메신저 서버 프로토콜 시작
	PROTOCOL_MESSENGER_SERVER = PROTOCOL_AUTH + 0x00A0,

	PROTOCOL_MESSENGER_NOTE_SEND_REQ,			// Nick으로 쪽지 보내기 요청			
	PROTOCOL_MESSENGER_NOTE_SEND_ACK,			// 쪽지 보내기 응답 - 성공 or 실패	

	PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ,	// UID로 쪽지 보내기 요청(기존 프로토콜은 유지)
	//PROTOCOL_MESSENGER_NOTE_SEND_ACK,			// 쪽지 보내기 응답 - 성공 or 실패	

	PROTOCOL_MESSENGER_NOTE_LIST_REQ,			// ! 사용안함
	PROTOCOL_MESSENGER_NOTE_LIST_ACK,			// 쪽지 목록 받기

	PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ,	// 쪽지 읽은 표시 하기 요청
	PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK,	// 쪽지 읽은 표시 하기 응답

	PROTOCOL_MESSENGER_NOTE_DELETE_REQ,			// 쪽지 삭제 요청
	PROTOCOL_MESSENGER_NOTE_DELETE_ACK,			// 쪽지 삭제 응답

	PROTOCOL_MESSENGER_NOTE_RECEIVE_REQ,		// ! 사용안함	
	PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK,		// 새 쪽지 도착

	PROTOCOL_MESSENGER_LOGIN_REQ,				// 메신저 서버 로그인 요청
	PROTOCOL_MESSENGER_LOGIN_ACK,				// 메신저 서버 로그인 응답

	PROTOCOL_MESSENGER_LOGOUT_REQ,				// 메신저 서버 로그아웃 요청
	PROTOCOL_MESSENGER_LOGOUT_ACK,				// 메신저 서버 로그아웃 응답

	PROTOCOL_MESSENGER_ENTER_USER_REQ,			// 게임서버 입장 통보
	PROTOCOL_MESSENGER_ENTER_USER_ACK,			// ! 사용안함

	PROTOCOL_MESSENGER_LEAVE_USER_REQ,			// 게임서버 퇴장 통보
	PROTOCOL_MESSENGER_LEAVE_USER_ACK,			// ! 사용안함

	PROTOCOL_MESSENGER_CHANGE_NICKNAME_REQ,		// 닉네임 변경 - 초기 설정 시에도 사용
	PROTOCOL_MESSENGER_CHANGE_NICKNAME_ACK,		// ! 사용안함

	PROTOCOL_MESSENGER_CHANGE_RANK_REQ,			// 계급 변경 - 초기 설정 시에도 사용
	PROTOCOL_MESSENGER_CHANGE_RANK_ACK,			// ! 사용안함

	PROTOCOL_MESSENGER_CLAN_NOTE_SEND_REQ,		// 클랜 쪽지 보내기 요청 
	PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK,		// 클랜 쪽지 보내기 응답 - 성공 or 실패

	PROTOCOL_MESSENGER_CLAN_GROUP_NOTE_SEND_REQ,	// 클랜 한번에 여러 사람에게 쪽지 보내기 요청(가입 승인, 거부, 강제 제명, 스텝 임명, 강등, 클랜 전체 쪽지)
	PROTOCOL_MESSENGER_CLAN_GROUP_NOTE_SEND_ACK,	// 클랜 한번에 여러 사람에게 쪽지 보내기 응답 - 성공 or 실패

	PROTOCOL_MESSENGER_CHANGE_COLOR_NICK_REQ,	// 컬러 클랜 변경 - 초기 설정 시에도 사용
	PROTOCOL_MESSENGER_CHANGE_COLOR_NICK_ACK,	// ! 사용안함

	PROTOCOL_MESSENGER_FRIEND_ACCEPT_REQ,			// 친구 등록, 삭제도 여기에 포함됨
	PROTOCOL_MESSENGER_FRIEND_ACCEPT_ACK,			// 친구 승인 에러 보고
	PROTOCOL_MESSENGER_FRIEND_INSERT_REQ,			// 친구 추가 요청
	PROTOCOL_MESSENGER_FRIEND_INSERT_ACK,			// 친구 추가 에러 보고
	PROTOCOL_MESSENGER_FRIEND_INVITED_REQ,			// 배틀룸에 친구초대 요청	
	PROTOCOL_MESSENGER_FRIEND_INVITED_ACK,			// 배틀룸에 친구초대 실패시 발신자에게 전달
	PROTOCOL_MESSENGER_FRIEND_INVITED_REQUEST_ACK,	// 배틀룸에 친구초대 성공시 수신자에게 전달
	PROTOCOL_MESSENGER_FRIEND_DELETE_REQ,			// 친구 삭제 요청
	PROTOCOL_MESSENGER_FRIEND_DELETE_ACK,			// 친구 삭제 에러를 발신자에게 전달
	PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_REQ,		// 방에 들어가기 
	PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_ACK,
	PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_REQ,		// 방에서 나가기 
	PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_ACK,
	PROTOCOL_MESSENGER_RESET_FRIEND_INFO_REQ,		// 친구목록 초기화 요청
	PROTOCOL_MESSENGER_RESET_FRIEND_INFO_ACK,		// ! 사용안함
	PROTOCOL_MESSENGER_KICKUSER_REQ,				// GSM에 의한 Kick
	PROTOCOL_MESSENGER_KICKUSER_ACK,				// ! 사용안함
	PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK,		//상태 변경시 전달됨
	PROTOCOL_MESSENGER_FRIEND_INFO_REQ,				// 친구정보 요청하기 
	PROTOCOL_MESSENGER_FRIEND_INFO_ACK,				// 요청할수도 있고, 로그인후 자동으로 받을수도 있음	
	PROTOCOL_MESSENGER_FIND_USER_REQ,				// 유저찾기 요청
	PROTOCOL_MESSENGER_FIND_USER_ACK,				// 유저찾기 응답
	PROTOCOL_MESSENGER_SEND_WHISPER_REQ,			// 귓속말 보내기 요청
	PROTOCOL_MESSENGER_SEND_WHISPER_ACK,			// 귓속말 보내기 응답 - 실패	
	PROTOCOL_MESSENGER_SEND_WHISPER_BY_UID_REQ,		// 귓속말 ReceiverUID로 보내기 요청
	//PROTOCOL_MESSENGER_SEND_WHISPER_ACK,			// 귓속말 보내기 응답 - 실패

	PROTOCOL_MESSENGER_RECV_WHISPER_REQ,			// ! 사용안함
	PROTOCOL_MESSENGER_RECV_WHISPER_ACK,			// 귓속말 받기

	PROTOCOL_MESSENGER_FRIEND_LOBBY_ENTER_REQ,		// 유저가 채널(로비)에 입장시 채널 번호를 전달함
	PROTOCOL_MESSENGER_FRIEND_LOBBY_LEAVE_REQ,		// 로비에서 나가기

	PROTOCOL_ALIVE_USER_REQ,						// 유저가 살아있으면 패킷을 보냄 
	PROTOCOL_ALIVE_USER_ACK,						// ! 사용안함

	PROTOCOL_AUTH_BLOCK_INFO_ACK,					// 차단목록 통보, 로그인시. Game -> Client

	PROTOCOL_MESSENGER_BLOCK_INSERT_REQ,			// 차단 추가 요청.
	PROTOCOL_MESSENGER_BLOCK_INSERT_ACK,			// 차단 추가 응답.
	PROTOCOL_MESSENGER_BLOCK_DELETE_REQ,			// 차단 삭제 요청.
	PROTOCOL_MESSENGER_BLOCK_DELETE_ACK,			// 차단 삭제 응답.

	PROTOCOL_MESSENGER_BLOCK_INFO_ACK,				// 차단목록 정보 전달. Mess -> Game. 로그인시.

	//// 뽑기 상점
	//PROTOCOL_AUTH_RS_START_REQ,						// 뽑기 상점. 뽑기 시작 요청. Game -> Trans.
	//PROTOCOL_AUTH_RS_RESULT_ACK,					// 뽑기 상점. 뽑기 결과 응답. Trans-> Game.

	//PROTOCOL_AUTH_RS_ENTER_REQ,						// 뽑기 상점. 뽑기 상점 입장 요청. Game -> Trans.
	//PROTOCOL_AUTH_RS_ENTER_ACK,						// 뽑기 상점. 뽑기 상점 입장 응담. Trans-> Game.

	//PROTOCOL_AUTH_RS_ITEM_INFO_REQ,					// 뽑기 상점. 아이템 정보 요청. Game -> Trans.
	//PROTOCOL_AUTH_RS_ITEM_INFO_ACK,					// 뽑기 상점. 아이템 정보 응담. Trans-> Game.

	//PROTOCOL_AUTH_RS_JACKPOT_USER_NICK_INSERT_REQ,	// 뽑기 상점. 대박 당첨 유저 닉네임 기록. Trans->Control.

	// NEW 유저 찾기.
	PROTOCOL_MESSENGER_FUSER_INFO_REQ,				// 찾고자 하는 유저의 정보요청. Mess -> Game ( 찾고자 하는 유저가 접속중인 )
	PROTOCOL_MESSENGER_FUSER_INFO_ACK,				// 찾고자 하는 유저의 정보응답. Game( 찾고자 하는 유저가 접속중인 ) -> Mess

	PROTOCOL_MESSENGER_FUSER_INFO_FIN_ACK,			// 두 유저가 다른 서버에 있을때 찾고자 하는 유저 정보. Mess->Game( 찾기 요청한 유저가 접속한 Game서버 ). 

	PROTOCOL_AUTH_ACCOUNT_KICK_ACK,			// 강퇴

	PROTOCOL_AUTH_SPECIALITY_CLASS_SETTING_REQ,		// v1.5 주특기. 병과(클래스) 셋팅 요청. Game->Trans
	PROTOCOL_AUTH_SPECIALITY_CLASS_SETTING_ACK,		// v1.5 주특기. 병과(클래스) 셋팅 응답. Trans->Game

	PROTOCOL_AUTH_USE_GIFTCOUPON_REQ,				// 쿠폰 사용 요청
	PROTOCOL_AUTH_USE_GIFTCOUPON_ACK,				// 쿠폰 사용 응답

	PROTOCOL_MESSENGER_FRIEND_CHANGE_STATE_REQ,		// 상태 변경

	PROTOCOL_MESSENGER_FRIEND_INVITE_FAIL_REQ,			// 친구초대 실패 전달.

	PROTOCOL_AUTH_CHARA_DB_SAVE_REQ,				// 캐릭터 DB 정보 저장

	PROTOCOL_AUTH_SHOP_EXPIRE_DELETE_ITEM_REQ,		// 기간 만료로 아이템 삭제시 DB에서 삭제.

	PROTOCOL_MESSENGER_NOTE_SEND_GIFT_NOTICE_REQ,		// 인벤토리에 이벤트로 아이템 지급시 쪽지 전달
	PROTOCOL_MESSENGER_NOTE_SEND_USERGIFT_NOTICE_REQ,	// 선물함OFF때 선물 구입시 받는 유저에게 쪽지 전달
};

enum Protocol_Shop
{
	PROTOCOL_SHOP = PROTOCOL_VALUE_SHOP,

	PROTOCOL_SHOP_ENTER_REQ,			//Shop에 들어감
	PROTOCOL_SHOP_ENTER_ACK,

	PROTOCOL_SHOP_LEAVE_REQ,			//Shop에서 나감 
	PROTOCOL_SHOP_LEAVE_ACK,

	PROTOCOL_SHOP_GET_SAILLIST_REQ,		// 판매 품목리스트를 얻습니다.
	PROTOCOL_SHOP_GET_SAILLIST_ACK,

	// IBGame - Shop
	PROTOCOL_AUTH_SHOP_VERSION_REQ,					// 상품 버전 요청
	PROTOCOL_AUTH_SHOP_VERSION_ACK,					// 상품 버전 응답

	PROTOCOL_AUTH_SHOP_LIST_REQ,					// 상점 리스트 요청
	PROTOCOL_AUTH_SHOP_LIST_ACK,					// 상점 리스트 응답

	PROTOCOL_AUTH_SHOP_GOODSLIST_REQ,				// 상품 리스트 요청
	PROTOCOL_AUTH_SHOP_GOODSLIST_ACK,				// 상품 리스트 요청

	PROTOCOL_AUTH_SHOP_ITEMLIST_REQ,				// 아이템 리스트 요청
	PROTOCOL_AUTH_SHOP_ITEMLIST_ACK,				// 아이템 리스트 요청

	PROTOCOL_AUTH_SHOP_MATCHINGLIST_REQ,			// 매칭 리스트 요청
	PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK,			// 매칭 리스트 요청

	PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ,			// 받은 선물 목록 요청
	PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK,			// 받은 선물 목록 응답

	PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ,				// 상품 구입 요청
	PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK,				// 상품 구입 결과 응답

	PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ,				// 선물하기 요청
	PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK,				// 선물하기 응답

	PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ,				// 아이템 인증 요청
	PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK,				// 아이템 인증 응답

	PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ,			// 아이템 인증 데이타 요청
	PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_ACK,			// 아이템 인증 데이타 응답

	PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ,				// 창고에 아이템 추가 요청
	PROTOCOL_AUTH_SHOP_INSERT_ITEM_ACK,				// 창고에 아이템 추가 응답

	PROTOCOL_AUTH_SHOP_AUTH_GIFT_REQ,				// 선물 인증 요청
	PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK,				// 선물 인증 응답

	PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ,				// 창고에 아이템 삭제 요청
	PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK,				// 창고에 아이템 삭제 응답

	PROTOCOL_AUTH_GET_POINT_CASH_REQ,				// Solicita tu propia información de puntos y efectivo
	PROTOCOL_AUTH_GET_POINT_CASH_ACK,				// Responde con tus propios puntos e información en efectivo

	PROTOCOL_AUTH_USED_WEAPON_REQ,					// 사용 무기 정보 요청
	PROTOCOL_AUTH_USED_WEAPON_ACK,					// 사용 무기 정보 응답

	PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_REQ,			// 닉네임 중복 검사 요청
	PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK,			// 닉네임 중복 검사 응답		// 왜 이걸로 통합 됨??

	PROTOCOL_AUTH_SHOP_CAPSULE_REQ,					// 캡슐 결과값 요청
	PROTOCOL_AUTH_SHOP_CAPSULE_ACK,					// 캡슐 결과값 응답

	PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_REQ,				// 선물함 변환 알림 요청 -- 사용하지 않음
	PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_ACK,				// 선물함 변환 알림 응답


	PROTOCOL_AUTH_SHOP_JACKPOT_REQ,					// 캡슐 잭팟 요청
	PROTOCOL_AUTH_SHOP_JACKPOT_ACK,					// 캡슐 잭팟 응답

	PROTOCOL_AUTH_SHOP_REPAIRLIST_REQ,				// 매칭 리스트 요청
	PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK,				// 매칭 리스트 요청

	PROTOCOL_SHOP_PLUS_POINT_REQ,					// 포인트와 얻음 요청 ( 구매, 캡슐 등 )
	PROTOCOL_SHOP_PLUS_POINT_ACK,					// 포인트와 얻음 응답

	// 할인쿠폰
	PROTOCOL_AUTH_SHOP_SALECOUPONLIST_REQ,			// 할인쿠폰 리스트 요청
	PROTOCOL_AUTH_SHOP_SALECOUPONLIST_ACK,			// 할인쿠폰 리스트 응답

	PROTOCOL_AUTH_SHOP_ITEM_EXTEND_REQ,				// 기간 연장 요청. ACK는 PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK 입니다.

	PROTOCOL_SHOP_REPAIR_REQ,						// 단일 수리 or 캐릭터뷰에 보이는 캐릭의 장착물품 전부 수리 요청.
	PROTOCOL_SHOP_REPAIR_ACK,						// 단일 수리 or 캐릭터뷰에 보이는 캐릭의 장착물품 전부 수리 응답.

	PROTOCOL_AUTH_SHOP_SEND_GIFT_REQ,				// 선물 지급 요청. ( 시스템 or GM ). 서버만 사용

	PROTOCOL_AUTH_SHOP_RECV_GIFT_ACK,				// 선물 받음. ( 선물 받은 사람이 접속중일 경우만 )

	PROTOCOL_AUTH_ENTER_GIFTSHOP_REQ,				// 선물 상점 입장 요청.
	PROTOCOL_AUTH_ENTER_GIFTSHOP_ACK,				// 선물 상점 입장 응답.

	PROTOCOL_AUTH_LEAVE_GIFTSHOP_REQ,				// 선물 상점 퇴장 요청.
	PROTOCOL_AUTH_LEAVE_GIFTSHOP_ACK,				// 선물 상점 퇴장 응답.

	PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_REQ,			// 쿠폰 사용 요청
	PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_ACK,			// 쿠폰 사용 응답

	PROTOCOL_AUTH_SHOP_REWARD_ITEM_REQ,				// 보상 아이템 지급 요청.

	PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_REQ,		// 사용중인 아이템 설정 변경 요청
	PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_ACK,		// 사용중인 아이템 설정 변경 응답
	//선물함 대신 쪽지함으로 메시지 넣기위함
	PROTOCOL_AUTH_GIFT_INSERT_ACK,

};

enum Protocol_Asc
{
	PROTOCOL_ASC = PROTOCOL_VALUE_ASC,
	//////////////////////////////////////////////////////////////////////////

	PROTOCOL_ASC_CONFIRMATION_REQ,					//컨넥후 처음 던지는 메시지 자신이 서버 인지, 유저인지를 보냄.
	PROTOCOL_ASC_CONFIRMATION_ACK,

	PROTOCOL_ASC_STATE_REQ,							//상태 보내기 (서버용)
	PROTOCOL_ASC_STATE_ACK,							//상태 요구하기(관리자툴용)	

	PROTOCOL_ASC_ADMIN_SERVER_START_REQ,			//서버 시작  
	PROTOCOL_ASC_ADMIN_SERVER_START_ACK,

	PROTOCOL_ASC_ADMIN_SERVER_STOP_REQ,				//서버 종료 
	PROTOCOL_ASC_ADMIN_SERVER_STOP_ACK,

	PROTOCOL_ASC_ADMIN_SERVER_ALL_START_REQ,		//모든 서버 시작  
	PROTOCOL_ASC_ADMIN_SERVER_ALL_START_ACK,

	PROTOCOL_ASC_ADMIN_SERVER_ALL_STOP_REQ,			//모든 서버 종료 
	PROTOCOL_ASC_ADMIN_SERVER_ALL_STOP_ACK,

	PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_REQ,			//공지 보내기 (관리자가 보냄)
	PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_ACK,			//공지 보내기 (게임서버에서 받음)

	PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_REQ,		//모든 서버 킥하기 (관리자가 보냄)
	PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_ACK,		//서버가 받음

	PROTOCOL_ASC_ADMIN_CONTROL_ANNOUNCE_REQ,		// 모든 관제툴에 보내기
	PROTOCOL_ASC_ADMIN_CONTROL_ANNOUNCE_ACK,		// 관리자툴

	PROTOCOL_ASC_ADMIN_SERVER_KICK_ONEUSER_REQ,		// 한개 서버 킥하기 (관리자가 보냄)
	PROTOCOL_ASC_ADMIN_SERVER_KICK_ONEUSER_ACK,		//서버가 받음	

	PROTOCOL_ASC_ADMIN_GAMESERVER_INFO_REQ,			// 게임서버 정보 받기 
	PROTOCOL_ASC_ADMIN_GAMESERVER_INFO_ACK,

	PROTOCOL_ASC_ADMIN_KICK_TRANS_USER_REQ,
	PROTOCOL_ASC_ADMIN_KICK_TRANS_USER_ACK,

	PROTOCOL_ASC_SET_MD5_KEY_REQ,					// char 32 byte 
	PROTOCOL_ASC_SET_MD5_KEY_ACK,					// INT32 0 이상이면 성공 

	PROTOCOL_ASC_SERVER_WARNING_REQ,				// 서버경고패킷.	 Control Server -> Admin Tool
	PROTOCOL_ASC_SERVER_WARNING_ACK,				// 서버경고패킷.	 Admin Tool   -> Control Server 

	PROTOCOL_ASC_GET_MD5_KEY_REQ,					// char 32 byte 
	PROTOCOL_ASC_GET_MD5_KEY_ACK,					// INT32 0 이상이면 성공 

	PROTOCOL_ASC_CONFIG_RELOAD_REQ,					// Config 리로드 옵션 변경 설정
	PROTOCOL_ASC_CONFIG_RELOAD_ACK,					// Config 리로드 옵션 변경 설정

	PROTOCOL_ASC_CHANGE_HACKCHECKRATIO_NTF,			// Change Hack Check ratio (ControlTool->ControSvr->DediSvr & Other Control tools) 
	PROTOCOL_ASC_NOTALLOW_ROOMCREATION_NTF,			// Not allow Room creation because of server shut down...(ControlTool->ControSvr->DediSvr & Other Control tools) 

	PROTOCOL_ASC_SERVER_LOADING_RESULT,				// 서버 로딩 결과

	PROTOCOL_ASC_BATTLE_ROOM_COUNT_REQ,				// 배틀 서버 방 개수
	PROTOCOL_ASC_BATTLE_ROOM_COUNT_ACK,				// 배틀 서버 방 개수

	// Web 용 프로토콜입니다. Web 용 프로토콜은 항상 맨 뒤에 추가해 주세요.
	PROTOCOL_ASC_WEB = PROTOCOL_ASC + 0x0040,

	PROTOCOL_ASC_WEB_GM_KICK_REQ,					// WEB툴 유저 킥
	PROTOCOL_ASC_WEB_GM_KICK_ACK,

	PROTOCOL_ASC_WEB_USER_INFO_LOCATION_REQ,		// WEB툴 접속여부 및 유저 위치 확인
	PROTOCOL_ASC_WEB_USER_INFO_LOCATION_ACK,

	PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_REQ,			// WEB툴 클랜전 여부
	PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_ACK,

	PROTOCOL_ASC_WEB_NOTICE_REQ,					// 전체공지
	PROTOCOL_ASC_WEB_NOTICE_ACK,
	PROTOCOL_ASC_GAMEGUARD_UPDATE_REQ,				//게임 가드 패킷 : 컨트롤 서버가 전체 게임 서버에 보냄 
	PROTOCOL_ASC_GAMEGUARD_UPDATE_ACK,				//게임 가드 패킷 : 게임가드 업데이트가 완료된 게임서버

	PROTOCOL_ASC_NEW_MD5_REQ,
	PROTOCOL_ASC_NEW_MD5_ACK,

	PROTOCOL_ASC_NORMAL_DISCONNECT_REQ,					// 서버 정상 종료. All Server -> Control Server.

	PROTOCOL_ASC_DOMI_REWARD_INFO_REQ,				// 제압 미션 보상 테이블 정보. Control - > Game.  
	PROTOCOL_ASC_DOMI_REWARD_INFO_ACK,				// 제압 미션 보상 테이블 정보. Control - > Game. 

	PROTOCOL_ASC_CONTENTS_STATE,				// Contents on/off state
	PROTOCOL_ASC_DATABASE_CONFIG_ACK,				// Database Config 정보 
	PROTOCOL_ASC_BILLING_CONFIG_ACK,				// Database Config 정보 
};

enum Protocol_Clan
{
	PROTOCOL_CS = PROTOCOL_VALUE_CLAN,
	//////////////////////////////////////////////////////////////////////////	

	PROTOCOL_CS_CLIENT_ENTER_REQ,					// 클랜 입장 요청
	PROTOCOL_CS_CLIENT_ENTER_ACK,					// 클랜 입장 응답

	PROTOCOL_CS_CLIENT_LEAVE_REQ,					//클랜 나가기 요청
	PROTOCOL_CS_CLIENT_LEAVE_ACK,					//클랜 나가기 응답	

	PROTOCOL_CS_CLIENT_CLAN_LIST_REQ,				//클랜 리스트 요청
	PROTOCOL_CS_CLIENT_CLAN_LIST_ACK,				//클랜 리스트 응답

	PROTOCOL_CS_CLIENT_CLAN_CONTEXT_REQ,			// 클랜 입장 요청
	PROTOCOL_CS_CLIENT_CLAN_CONTEXT_ACK,			// 클랜 입장 응답

	PROTOCOL_CS_DELETE_CONNECT_INFO_REQ,			// 없는 유저			// 사용하지 않음

	PROTOCOL_CS_USER_LOGIN_REQ,						// 클랜 로그인 요청
	PROTOCOL_CS_USER_LOGIN_ACK,						// 미사용

	PROTOCOL_CS_USER_LOGOUT_REQ,					// 클랜 로그아웃 요청
	PROTOCOL_CS_USER_LOGOUT_ACK,					// 미사용

	PROTOCOL_CS_USER_SERVER_ENTER_REQ,				// 서버 입장 요청
	PROTOCOL_CS_USER_SERVER_ENTER_ACK,				// 미사용

	PROTOCOL_CS_USER_SERVER_LEAVE_REQ,				// 서버 퇴장 요청
	PROTOCOL_CS_USER_SERVER_LEAVE_ACK,				// 미사용

	PROTOCOL_CS_USER_CHANEL_ENTER_REQ,				// 채널 입장 요청
	PROTOCOL_CS_USER_CHANEL_ENTER_ACK,				// 미사용

	PROTOCOL_CS_USER_CHANEL_LEAVE_REQ,				// 채널 퇴장 요청
	PROTOCOL_CS_USER_CHANEL_LEAVE_ACK,				// 미사용

	PROTOCOL_CS_USER_ROOM_ENTER_REQ,				// 방 입장 요청
	PROTOCOL_CS_USER_ROOM_ENTER_ACK,				// 미사용

	PROTOCOL_CS_USER_ROOM_LEAVE_REQ,				// 방 퇴장 요청
	PROTOCOL_CS_USER_ROOM_LEAVE_ACK,				// 미사용

	PROTOCOL_CS_CLAN_CONTEXT_REQ,					// 클랜 목록 정보 요청
	PROTOCOL_CS_CLAN_CONTEXT_ACK,					// 클랜 목록 정보 응답

	PROTOCOL_CS_CLAN_LIST_STREAM_REQ,				// 클랜 목록 연속받기 요청
	PROTOCOL_CS_CLAN_LIST_STREAM_ACK,				// 클랜 목록 연속받기 응답

	PROTOCOL_CS_CLAN_LIST_GS_REQ,					// 클랜 개별 목록 갱신 요청
	PROTOCOL_CS_CLAN_LIST_GS_ACK,					// 클랜 개별 목록 갱신 응답

	PROTOCOL_CS_DETAIL_INFO_REQ,					// 클랜 상세정보 요청
	PROTOCOL_CS_DETAIL_INFO_ACK,					// 클랜 상세정보 응답

	PROTOCOL_CS_MEMBER_CONTEXT_REQ,					// 클랜멤버 목록 정보 요청
	PROTOCOL_CS_MEMBER_CONTEXT_ACK,					// 클랜멤버 목록 정보 응답

	PROTOCOL_CS_MEMBER_LIST_REQ,					// 클랜멤버 목록 보기 요청
	PROTOCOL_CS_MEMBER_LIST_ACK,					// 클랜멤버 목록 보기 응답

	PROTOCOL_CS_CREATE_CLAN_REQ,					// 클랜 생성 요청
	PROTOCOL_CS_CREATE_CLAN_ACK,					// 클랜 생성 응답

	PROTOCOL_CS_CLOSE_CLAN_REQ,						// 클랜 해체 요청
	PROTOCOL_CS_CLOSE_CLAN_ACK,						// 클랜 해체 응답

	PROTOCOL_CS_CHECK_JOIN_AUTHORITY_REQ,			// 클랜 가입 권한 체크 요청
	PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK,			// 클랜 가입 권한 체크 응답

	PROTOCOL_CS_JOIN_REQUEST_REQ,					// 클랜 가입신청서 등록 요청
	PROTOCOL_CS_JOIN_REQUEST_ACK,					// 클랜 가입인청서 등록 응답

	PROTOCOL_CS_CANCEL_REQUEST_REQ,					// 클랜 가입신청 취소 요청
	PROTOCOL_CS_CANCEL_REQUEST_ACK,					// 클랜 가입신청 취소 응답

	PROTOCOL_CS_REQUEST_CONTEXT_REQ,				// 클랜 가입 신청서 목록 정보 요청
	PROTOCOL_CS_REQUEST_CONTEXT_ACK,				// 클랜 가입 신청서 목록 정보 응답

	PROTOCOL_CS_REQUEST_LIST_REQ,					// 클랜 가입신청서 목록 보기 요청
	PROTOCOL_CS_REQUEST_LIST_ACK,					// 클랜 가입신청서 목록 보기 응답

	PROTOCOL_CS_REQUEST_INFO_REQ,					// 클랜 가입신청서 상세정보 보기 요청
	PROTOCOL_CS_REQUEST_INFO_ACK,					// 클랜 가입신청서 상세정보 보고 응답

	PROTOCOL_CS_ACCEPT_REQUEST_REQ,					// 클랜 가입신청서 수락 요청
	PROTOCOL_CS_ACCEPT_REQUEST_ACK,					// 클랜 가입신청서 수락 응답

	PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK,			// 클랜 가입 신청 승인 응답

	PROTOCOL_CS_DENIAL_REQUEST_REQ,					// 클랜 가입신청서 거절 요청
	PROTOCOL_CS_DENIAL_REQUEST_ACK,					// 클랜 가입신청서 거절 응답

	PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK,			// 클랜 가입 신청 거절 응답

	PROTOCOL_CS_SECESSION_CLAN_REQ,					// 클랜 탈퇴 요청
	PROTOCOL_CS_SECESSION_CLAN_ACK,					// 클랜 탈퇴 응답

	PROTOCOL_CS_DEPORTATION_REQ,					// 클랜 강제 제명 요청
	PROTOCOL_CS_DEPORTATION_ACK,					// 클랜 강제 제명 응답

	PROTOCOL_CS_DEPORTATION_RESULT_ACK,				// 클랜 강제 제명 승인 응답

	PROTOCOL_CS_COMMISSION_MASTER_REQ,				// 클랜 마스터 위임 요청
	PROTOCOL_CS_COMMISSION_MASTER_ACK,				// 클랜 마스터 위임 응답

	PROTOCOL_CS_COMMISSION_MASTER_RESULT_ACK,		// 클랜장 위임 응답

	PROTOCOL_CS_COMMISSION_STAFF_REQ,				// 클랜 스탭위임 요청
	PROTOCOL_CS_COMMISSION_STAFF_ACK,				// 클랜 스탭위임 응답

	PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK,		// 클랜 스탭 위임 응답

	PROTOCOL_CS_COMMISSION_REGULAR_REQ,				// 클랜 스탭을 일반 회원으로 강등 요청
	PROTOCOL_CS_COMMISSION_REGULAR_ACK,				// 클랜 스탭을 일반 회원으로 강등 응답

	PROTOCOL_CS_COMMISSION_REGULAR_RESULT_ACK,		// 클랜 스탭에게 일반 회원으로 강등 결과 응답

	PROTOCOL_CS_STAFF_AUTHORITY_REQ,				// 클랜 스탭권한 요청
	PROTOCOL_CS_STAFF_AUTHORITY_ACK,				// 클랜 스탭권한 응답

	PROTOCOL_CS_MEMBER_INFO_REQ,					// 클랜 커뮤니티 목록 요청(Only Server)
	PROTOCOL_CS_MEMBER_INFO_ACK,					// 클랜 커뮤니티 목록 응답

	PROTOCOL_CS_MEMBER_INFO_INSERT_REQ,				// 클랜 커뮤니티 유저 가입 요청(Only Server)
	PROTOCOL_CS_MEMBER_INFO_INSERT_ACK,				// 클랜 커뮤니티 유저 가입 응답

	PROTOCOL_CS_MEMBER_INFO_DELETE_REQ,				// 클랜 커뮤니티 유저 탈퇴 요청(Only Server)
	PROTOCOL_CS_MEMBER_INFO_DELETE_ACK,				// 클랜 커뮤니티 유저 탈퇴 응답

	PROTOCOL_CS_MEMBER_INFO_CHANGE_REQ,				// 클랜 커뮤니티 유저 상태 변경 요청(Only Server)
	PROTOCOL_CS_MEMBER_INFO_CHANGE_ACK,				// 클랜 커뮤니티 유저 상태 변경 응답

	PROTOCOL_CS_MEMBER_DETAIL_INFO_REQ,				// 클랜 맴버 상세정보 요청
	PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK,				// 클랜 맴버 상세정보 응답

	PROTOCOL_CS_CHATTING_REQ,						// 클랜 채팅 요청
	PROTOCOL_CS_CHATTING_ACK,						// 클랜 채팅 응답

	PROTOCOL_CS_CHECK_MARK_REQ,						// 클랜 마크 중복검사 요청
	PROTOCOL_CS_CHECK_MARK_ACK,						// 클랜 마크 중복검사 응답

	PROTOCOL_CS_REPLACE_NOTICE_REQ,					// 클랜 공지사항 요청
	PROTOCOL_CS_REPLACE_NOTICE_ACK,					// 클랜 공지사항 응답

	PROTOCOL_CS_REPLACE_INTRO_REQ,					// 클랜 소개 변경 요청
	PROTOCOL_CS_REPLACE_INTRO_ACK,					// 클랜 소개 변경 응답	

	PROTOCOL_CS_REPLACE_NAME_REQ,					// 클랜 이름변경 요청(Only Server)
	PROTOCOL_CS_REPLACE_NAME_ACK,					// 클랜 이름변경 응답

	PROTOCOL_CS_REPLACE_NAME_RESULT_ACK,			// 클랜 이름 변경 전달

	PROTOCOL_CS_REPLACE_MARK_REQ,					// 클랜 마크 변경 요청
	PROTOCOL_CS_REPLACE_MARK_ACK,					// 클랜 마브 변경 응답

	PROTOCOL_CS_REPLACE_MARK_RESULT_ACK,			// 클랜 마크 변경

	PROTOCOL_CS_REPLACE_MANAGEMENT_REQ,				// 클랜 관리 변경 요청
	PROTOCOL_CS_REPLACE_MANAGEMENT_ACK,				// 클랜 관리 변경 응답

	PROTOCOL_CS_RECORD_RESET_REQ,					// 클랜 전적 초기화 요청(Only Server)
	PROTOCOL_CS_RECORD_RESET_ACK,					// 클랜 전적 초기화 응답

	PROTOCOL_CS_REPLACE_PERSONMAX_REQ,				// 클랜 인원 늘리기 요청(Only Server)
	PROTOCOL_CS_REPLACE_PERSONMAX_ACK,				// 클랜 인원 늘리기 응답

	PROTOCOL_CS_REPLACE_MEMBER_NICK_REQ,			// 클랜원 닉네임 변경 요청
	PROTOCOL_CS_REPLACE_MEMBER_NICK_ACK,			// 클랜원 닉네임 변경 응답	

	PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK,	// 클랜원 정보 변경 전달

	PROTOCOL_CS_ROOM_INVITED_REQ,					// 클랜방 초대 요청
	PROTOCOL_CS_ROOM_INVITED_ACK,					// 클랜방 초대 전달

	PROTOCOL_CS_ROOM_INVITED_RESULT_ACK,			// 클랜방 초대 전달

	PROTOCOL_CS_CHANGE_CLAN_EXP_RANK_ACK,			// 클랜 경험치등급 변경
	PROTOCOL_CS_CHANGE_CLAN_PER_RANK_ACK,			// 클랜 인원수등급 변경

	PROTOCOL_CS_USER_PAGE_ENTER_REQ,				// 클랜 페이지 입장 요청
	PROTOCOL_CS_USER_PAGE_ENTER_ACK,				// 미사용

	PROTOCOL_CS_USER_PAGE_LEAVE_REQ,				// 클랜 페이지 퇴장 요청
	PROTOCOL_CS_USER_PAGE_LEAVE_ACK,				// 미사용

	PROTOCOL_CS_PAGE_CHATTING_REQ,					// 클랜 페이지 채팅 요청
	PROTOCOL_CS_PAGE_CHATTING_ACK,					// 클랜 페이지 채팅 응답

	PROTOCOL_CS_INVITE_REQ,							// 클랜 초대 요청(마스터 -> 일반 유저)
	PROTOCOL_CS_INVITE_ACK,							// 클랜 초대 전달(마스터)

	PROTOCOL_CS_INVITE_ACCEPT_REQ,					// 클랜 초대 결과 요청(클랜 초대 쪽지에 대한 수락)
	PROTOCOL_CS_INVITE_ACCEPT_ACK,					// 클랜 초대 결과 전달

	PROTOCOL_CS_NOTE_REQ,							// 클랜 전체 쪽지 요청
	PROTOCOL_CS_NOTE_ACK,							// 클랜 전체 쪽지 전달

	PROTOCOL_CS_POINT_RESET_REQ,					// 클랜 포인트 초기화 요청
	PROTOCOL_CS_POINT_RESET_ACK,					// 클랜 포인트 초기화 응답

	PROTOCOL_CS_EXPUP_REQ,							// 클랜 경험치 증가권 요청
	PROTOCOL_CS_EXPUP_ACK,							// 클랜 경험치 증가권 응답

	PROTOCOL_CS_REPLACE_MEMBER_RANK_REQ,			// 클랜원 랭크변경 요청
	PROTOCOL_CS_REPLACE_MEMBER_RANK_ACK,			// 클랜원 랭크변경 응답

	PROTOCOL_CS_REPLACE_COLOR_NAME_REQ,				// 클랜이름 색변경 요청(Only Server)
	PROTOCOL_CS_REPLACE_COLOR_NAME_ACK,				// 클랜이름 색변경 응답

	PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK,		// 클랜이름 색변경 전달

	PROTOCOL_CS_REPLACE_MEMBER_COLOR_NICK_REQ,		// 클랜원 칼라 닉네임 요청
	PROTOCOL_CS_REPLACE_MEMBER_COLOR_NICK_ACK,		// 클랜원 칼라 닉네임 응답

	PROTOCOL_CS_RECORD_RESET_RESULT_ACK,			// 클랜 전적 초기화 전달
	PROTOCOL_CS_POINT_RESET_RESULT_ACK,				// 클랜 포인트 초기화 전달

	PROTOCOL_CS_REPLACE_MANAGEMENT_RESULT_ACK,		// 클랜원 정보 변경 전달

	PROTOCOL_CS_GS_CREATE_CLAN_REQ,					// 클랜 생성 통보 요청
	PROTOCOL_CS_GS_CREATE_CLAN_ACK,					// 클랜 생성 통보 응답

	PROTOCOL_CS_GS_DESTROY_CLAN_REQ,				// 클랜 해체 통보 요청
	PROTOCOL_CS_GS_DESTROY_CLAN_ACK,				// 클랜 해체 통보 응답

	PROTOCOL_CS_CREATE_CLAN_CONDITION_REQ,			// 클랜 생성 조건 요청
	PROTOCOL_CS_CREATE_CLAN_CONDITION_ACK,			// 클랜 생성 조건 응답

	PROTOCOL_CS_CHECK_DUPLICATE_REQ,				//중복이름 검사 요청
	PROTOCOL_CS_CHECK_DUPLICATE_ACK,				//중복이름 검사 응답

	PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_REQ,		// 아지트 URL 중복 검사 요청
	PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK,		// 아지트 URL 중복 검사 응답

	PROTOCOL_CS_CLIENT_CHECK_DUPLICATE_REQ,			//사용안함
	PROTOCOL_CS_CLIENT_CHECK_DUPLICATE_ACK,			//사용안함

	PROTOCOL_CS_CLAN_INO_RANK_UPDATE_REQ,			// 클랜 목록에 장성인 자신의 랭킹을 업데이트
	//PROTOCOL_CS_CLAN_INO_RANK_UPDATE_ACK,			// 미사용

	PROTOCOL_CS_CLAN_ITEM_DELETE_REQ,				// 클랜전 경험치, 클랜명컬러 아이템 제거

	PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_REQ,			// 클랜 리스트에서 특정 클랜 선택시 해당 클랜의 상세정보 요청
	PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK,			// 클랜 리스트에서 특정 클랜 선택시 해당 클랜의 상세정보 응답

	PROTOCOL_CS_NEW_DETAIL_INFO_REQ,				// 클랜 상세정보 요청
	PROTOCOL_CS_NEW_DETAIL_INFO_ACK,				// 클랜 상세정보 응답

	PROTOCOL_CS_NEW_CREATE_CLAN_REQ,				// 클랜 생성 요청
	PROTOCOL_CS_NEW_CREATE_CLAN_ACK,				// 클랜 생성 응답

	PROTOCOL_CS_NEW_REQUEST_LIST_REQ,					// 클랜 가입신청서 목록 보기 요청
	PROTOCOL_CS_NEW_REQUEST_LIST_ACK,					// 클랜 가입신청서 목록 보기 응답

	PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_REQ,			// 클랜전 매치 정보 보기 목록 정보 요청
	PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_ACK,			// 클랜전 매치 정보 보기 목록 정보 응답

	PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_REQ,				// 클랜전 매치 정보 보기 요청
	PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_ACK,				// 클랜전 매치 정보 보기 응답

	PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ,		// 클랜전 매치 정보 감정표현(좋아요) 요청
	PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_ACK,		// 클랜전 매치 정보 감정표현(좋아요) 응답

	PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ,	// 클랜 가입신청서 (자동,수동) 승인 설정 요청(클랜마스터만 가능)
	PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK,	// 클랜 가입신청서 (자동,수동) 승인 설정 요청

	PROTOCOL_CS_NEW_REFRESH_CLAN_PAGE_ACK,					// 클랜페이지를 켠 상태의 클랜원에게 페이지 새로고침 처리(클랜마스터가 가입신청 승인 or 클랜 강제제명을 하였을때)
	//////////////////////////////////////////////////////////////////////////

	PROTOCOL_GC_DELETE_CONNECT_INFO_REQ,			// 없는 유저			// 사용하지 않음

	PROTOCOL_GC_USER_LOGIN_REQ,						// 클랜 로그인 요청
	PROTOCOL_GC_USER_LOGIN_ACK,						// 클랜 로그인 응답

	PROTOCOL_GC_USER_LOGOUT_REQ,					// 클랜 로그아웃 요청
	//PROTOCOL_GC_USER_LOGOUT_ACK,					// 미사용

	PROTOCOL_GC_USER_SERVER_ENTER_REQ,				// 서버 입장 요청
	//PROTOCOL_GC_USER_SERVER_ENTER_ACK,				// 미사용

	PROTOCOL_GC_USER_SERVER_LEAVE_REQ,				// 서버 퇴장 요청
	//PROTOCOL_GC_USER_SERVER_LEAVE_ACK,				// 미사용

	PROTOCOL_GC_USER_CHANEL_ENTER_REQ,				// 채널 입장 요청
	//PROTOCOL_GC_USER_CHANEL_ENTER_ACK,				// 미사용

	PROTOCOL_GC_USER_CHANEL_LEAVE_REQ,				// 채널 퇴장 요청
	//PROTOCOL_GC_USER_CHANEL_LEAVE_ACK,				// 미사용

	//PROTOCOL_GC_USER_ROOM_ENTER_REQ,				// 방 입장 요청
	//PROTOCOL_GC_USER_ROOM_ENTER_ACK,				// 미사용

	PROTOCOL_GC_USER_ROOM_LEAVE_REQ,				// 방 퇴장 요청
	//PROTOCOL_GC_USER_ROOM_LEAVE_ACK,				// 미사용

	PROTOCOL_GC_CLAN_CONTEXT_REQ,					// 클랜 목록 정보 요청
	PROTOCOL_GC_CLAN_CONTEXT_ACK,					// 클랜 목록 정보 응답

	PROTOCOL_GC_CLAN_LIST_STREAM_REQ,				// 클랜 목록 연속받기 요청
	PROTOCOL_GC_CLAN_LIST_STREAM_ACK,				// 클랜 목록 연속받기 응답

	//PROTOCOL_GC_CLAN_LIST_GS_REQ,					// 클랜 개별 목록 갱신 요청
	//PROTOCOL_GC_CLAN_LIST_GS_ACK,					// 클랜 개별 목록 갱신 응답

	//PROTOCOL_GC_STAFF_AUTHORITY_REQ,				// 클랜 스탭권한 요청
	//PROTOCOL_GC_STAFF_AUTHORITY_ACK,				// 클랜 스탭권한 응답

	PROTOCOL_GC_REPLACE_MEMBER_NICK_REQ,			// 클랜원 닉네임 변경 요청
	PROTOCOL_GC_REPLACE_MEMBER_NICK_ACK,			// 클랜원 닉네임 변경 응답	

	PROTOCOL_GC_USER_PAGE_LEAVE_REQ,				// 클랜 페이지 퇴장 요청
	//PROTOCOL_GC_USER_PAGE_LEAVE_ACK,				// 미사용

	PROTOCOL_GC_POINT_RESET_REQ,					// 클랜 포인트 초기화 요청
	//PROTOCOL_GC_POINT_RESET_ACK,					// (미사용)클랜 포인트 초기화 응답

	PROTOCOL_GC_EXPUP_REQ,							// 클랜 경험치 증가권 요청
	PROTOCOL_GC_EXPUP_ACK,							// 클랜 경험치 증가권 응답

	PROTOCOL_GC_REPLACE_MEMBER_RANK_REQ,			// 클랜원 랭크변경 요청
	PROTOCOL_GC_REPLACE_MEMBER_RANK_ACK,			// 클랜원 랭크변경 응답

	PROTOCOL_GC_REPLACE_MEMBER_COLOR_NICK_REQ,		// 클랜원 칼라 닉네임 요청
	PROTOCOL_GC_REPLACE_MEMBER_COLOR_NICK_ACK,		// 클랜원 칼라 닉네임 응답

	//PROTOCOL_GC_GS_CREATE_CLAN_REQ,				// 클랜 생성 통보 요청
	//PROTOCOL_GC_GS_CREATE_CLAN_ACK,				// 클랜 생성 통보 응답

	//PROTOCOL_GC_GS_DESTROY_CLAN_REQ,				// 클랜 해체 통보 요청
	//PROTOCOL_GC_GS_DESTROY_CLAN_ACK,				// 클랜 해체 통보 응답

	PROTOCOL_GC_CLAN_INO_RANK_UPDATE_REQ,			// 클랜 목록에 장성인 자신의 랭킹을 업데이트	

	PROTOCOL_GC_NEW_USER_ROOM_ENTER_REQ,				// 방 입장 요청

	PROTOCOL_GC_NEW_USER_PAGE_ENTER_REQ,			// 클랜 페이지 입장 요청
	//PROTOCOL_GC_NEW_USER_PAGE_ENTER_ACK,			// 미사용

	PROTOCOL_GC_CLAN_USER_INFO_REQ,				// 유저찾기 기본정보 중 클랜 정보 요청
	PROTOCOL_GC_CLAN_USER_INFO_ACK,				// 유저찾기 기본정보 중 클랜 정보 응답

	PROTOCOL_CS_USER_CHANGE_STATE_REQ,				// 상태변경
};

enum Protocol_ClanMatch
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// MatchMaking
	PROTOCOL_CS_MATCH = PROTOCOL_VALUE_CLAN_MATCH,						// 클랜 배틀 서버

	PROTOCOL_CS_MATCH_BATTLE_RESULT,				// 게임 종료 클랜 결과

	PROTOCOL_CS_MATCH_TEAM_CONTEXT_REQ,				// 동일 클랜에서 만든 방 요청(클랜채널입장시 클랜 매치 클릭시)
	PROTOCOL_CS_MATCH_TEAM_CONTEXT_ACK,				// 동일 클랜에서 만든 방 응답

	PROTOCOL_CS_MATCH_TEAM_LIST_REQ,				// 동일 클랜에서 만든 방 요청
	PROTOCOL_CS_MATCH_TEAM_LIST_ACK,				// 동일 클랜에서 만든 방 응답

	PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_REQ,			// 클랜전 할 모든 팀 리스트 요청(팀등록 클릭시)
	PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_ACK,			// 클랜전 할 모든 팀 리스트 응답

	PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ,			// 클랜전 할 모든 팀 리스트 요청
	PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK,			// 클랜전 할 모든 팀 리스트 응답

	PROTOCOL_CS_MATCH_TEAM_CREATE_REQ,				// 팀 생성 요청
	PROTOCOL_CS_MATCH_TEAM_CREATE_ACK,				// 팀 생성 응답

	PROTOCOL_CS_MATCH_TEAM_JOIN_REQ,				// 팀 입장 요청
	PROTOCOL_CS_MATCH_TEAM_JOIN_ACK,				// 팀 입장 응답

	PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ,				// 팀 나가기 요청
	PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK,				// 팀 나기기 응답

	PROTOCOL_CS_MATCH_TEAM_INFO_ACK,				// 팀 정보 전달 응답

	PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ,			// 전투 신청 요청
	PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK,			// 전투 신청 응답

	PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK,		// 전투 신청 전달 응답

	PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_REQ,		// 빠른 신청 요청
	PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_ACK,		// 빠른 신청 응답

	PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ,				// 전투 신청 응답 요청
	PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK,				// 전투 신청 응답 응답

	PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK,		// 전투 신청 거절 전달

	PROTOCOL_CS_MATCH_ROOM_CREATE_ACK,				// 방생성 응답

	PROTOCOL_CS_MATCH_ROOM_JOIN_ACK,				// 방 입장 응답

	PROTOCOL_CS_MATCH_ROOM_INFO_ACK,				// 방 정보 응답

	PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ,			// 클랜전을 위한 방 만들기 요청 ( 게임서버로의 요청)
	PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK,			// 클랜전을 위한 방 만들기 응답( 게임서버로의 부터 응답)

	PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ,				// 방 입장 요청
	PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK,

	PROTOCOL_CS_MATCH_ROOM_LEAVE_GS_REQ,			// 방 퇴장 요청

	PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ,			// 클랜전 할 상대의 상세 정보 요청
	PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK,			// 클랜전 할 상대의 상세 정보 응답

	PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ,			// 팀 기본 인원 변경 요청
	PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK,			// 팀 기본 인원 변경 응답

	PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_RESULT_ACK,	// 팀 기본 인원 변경 전달 응답

	PROTOCOL_CS_MATCH_FIGHT_INFO_RESULT_ACK,		// 클랜전 할 상대의 상세 정보 

	PROTOCOL_CS_MATCH_FIGHT_CHANGE_INFO_ACK,		// 클랜전 할 상대의 상세 정보 응답

	PROTOCOL_CS_MATCH_CHATING_REQ,					// 클랜전팀 체팅 요청
	PROTOCOL_CS_MATCH_CHATING_ACK,					// 클랜전팀 체팅 응답

	PROTOCOL_CS_MATCH_ROOM_LEAVE_REQ,				// 클랜전팀 룸나가기 요청

	PROTOCOL_CS_MATCH_CHANNEL_REQ,					// 클랜 매치 채널

	PROTOCOL_CS_MATCH_NEW_CLAN_INFO_REQ,				// 클랜 룸에서 상대 클랜 정보 요청
	PROTOCOL_CS_MATCH_NEW_CLAN_INFO_ACK,				// 클랜 룸에서 상대 클랜 정보 응답

	PROTOCOL_CS_MATCH_NEW_HONOR_REQ,					// 클랜 명예전 신청 요청
	PROTOCOL_CS_MATCH_NEW_HONOR_ACK,					// 클랜 명예전 신청 전달 응답(명예전 신청자에게 메시지 전달)	
	PROTOCOL_CS_MATCH_NEW_HONOR_REQUEST_ACK,			// 클랜 명예전 신청 수신(명예전 수신자에게 메시지 전달)	

	PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_REQ,				// 클랜 명예전 신청 답장 전달(수락할 경우, 거부할 경우 모두 전달)
	PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK,				// 클랜 명예전 신청 답장 수신(명예전 참가자 전원에게 메시지 전달)	

	PROTOCOL_CS_MATCH_NEW_FIGHT_QUICK_REQUEST_REQ,		// 빠른 신청 요청(퀵매치)
	PROTOCOL_CS_MATCH_NEW_FIGHT_QUICK_REQUEST_ACK,		// (사용안함)  빠른 신청 응답 대신 PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK 이걸로 받음.

	PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_REQ,		// (사용안함)전투 신청 요청
	PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_ACK,		// 퀵매치 등록의 성공,실패를 응답

	PROTOCOL_CS_MATCH_NEW_OPERATION_REQ,				// 작전명 입력
	PROTOCOL_CS_MATCH_NEW_OPERATION_ACK,				// 작전명 응답 & 분대원에 전송

	PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_REQ,		// 클랜 채널에서 방의 집계 정보를 요청(대기,매치 팀수)
	PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_ACK,		// 클랜 채널에서 방의 집계 정보를 응답

	PROTOCOL_GC_MATCH_NEW_BATTLE_RESULT_REQ,			// 게임 종료 클랜 결과

	PROTOCOL_GC_MATCH_NEW_CLAN_INFO_REQ,				// 매치 클랜 정보 요청
	PROTOCOL_GC_MATCH_NEW_CLAN_INFO_ACK,				// 매치 클랜 정보 응답

};

enum Protocol_Clan_War
{
	PROTOCOL_CLAN_WAR = PROTOCOL_VALUE_CLAN_WAR,

	/////////////////////////////////////////////////////////////////
	// Client <-> Game Server <-> Clan Server
	PROTOCOL_CLAN_WAR_PRESEASON_RANKING_ACK,		// 이전 시즌 클랜 랭킹 정보.		Game -> Client
	PROTOCOL_CLAN_WAR_CHECK_PRESEASON_RANKING_REQ,	// 이전 시즌 클랜 랭킹 확인함.	Client -> Game

	PROTOCOL_CLAN_WAR_MATCH_TEAM_COUNT_ACK,			// 클랜 매치 팀 카운트. Clan -> Game -> Client. 일정주기마다.

	PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_REQ,			// 매치 팀 리스트 요청. 2초마다 게임서버로 보냅니다. 
	PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK,			// 매치 팀 리스트 응답.

	PROTOCOL_CLAN_WAR_CREATE_TEAM_REQ,				// 매치 팀 생성 요청
	PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK,				// 매치 팀 생성 응답

	PROTOCOL_CLAN_WAR_JOIN_TEAM_REQ,				// 매치 팀 입장 요청
	PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK,				// 매치 팀 입장 응답

	PROTOCOL_CLAN_WAR_LEAVE_TEAM_REQ,				// 매치 팀 퇴장 요청
	PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK,				// 매치 팀 퇴장 응답

	PROTOCOL_CLAN_WAR_CHANGE_OPERATION_REQ,			// 매치 팀 임무 설정 요청
	PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK,			// 매치 팀 임무 설정 응답

	PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_REQ,			// 매치 팀 최대 인원수 변경 요청.
	PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK,			// 매치 팀 최대 인원수 변경 응답.

	PROTOCOL_CLAN_WAR_TEAM_CHATTING_REQ,			// 매치팀 채팅 요청.
	PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK,			// 매치팀 채팅 응답.

	PROTOCOL_CLAN_WAR_MATCH_TEAM_INFO_ACK,			// 매칭된 팀의 정보 응답.

	PROTOCOL_CLAN_WAR_CHANGE_MEMBER_INFO_ACK,		// 매치팀원 정보 변경 

	PROTOCOL_CLAN_WAR_MATCHMAKING_REQ,				// 매치메이킹 요청
	PROTOCOL_CLAN_WAR_MATCHMAKING_ACK,				// 매치메이킹 응답

	PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_REQ,		// 매치메이킹 취소 요청
	PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK,		// 매치메이킹 취소 응답

	PROTOCOL_CLAN_WAR_MERCENARY_LIST_REQ,			// 용병 리스트 요청
	PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK,			// 용병 리스트 응답

	PROTOCOL_CLAN_WAR_REGIST_MERCENARY_REQ,			// 용병 등록 요청
	PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK,			// 용병 등록 응답

	PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_REQ,			// 용병 해제 요청
	PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK,			// 용병 해제 응답

	PROTOCOL_CLAN_WAR_INVITE_MERCENARY_REQ,			// 용병 초대 요청 - 리더가 보냄.
	PROTOCOL_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK,	// 용병 초대 응답 - 리더가 받음.
	PROTOCOL_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK,	// 용병 초대 응답 - 용병이 받음.

	PROTOCOL_CLAN_WAR_INVITE_MERCENARY_ACK,			// ClanServer -> GameServer

	PROTOCOL_CLAN_WAR_INVITE_ACCEPT_REQ,			// 용병 초대 수락 - 용병이 보냄.
	PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK,			// 용병 초대 수락 - 용병이 받음.

	PROTOCOL_CLAN_WAR_INVITE_DENIAL_REQ,			// 용병 초대 거절 요청 - 용병이 보냄
	PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK,			// 용병 초대 거절 응답 - 용병, 리더가 받음

	PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ,	// 용병 상세 정보 요청.
	PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK,	// 용병 상세 정보 응답.

	PROTOCOL_CLAN_WAR_MATCH_MAKING_RESULT_ACK,		// 매치메이킹 결과를 클랜원에게 전달.

	/////////////////////////////////////////////////////////////////
	// Game Server <-> Clan Server  
	PROTOCOL_CLAN_WAR_CREATE_ROOM_REQ,				// 매치룸 생성 요청. Clan Server -> Game Server
	PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK,			// 매치룸 생성 응답. Game Server -> Clan Server

	PROTOCOL_CLAN_WAR_JOIN_ROOM_REQ,				// 매치룸 입장 요청.	Clan Server -> Game Server
	PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK,				// 매치룸 입장 응답. Game Server -> Clan Server

	PROTOCOL_CLAN_WAR_LEAVE_ROOM_REQ,				// 매치 룸 퇴장 요청.  Game Server -> Clan Server
	PROTOCOL_CLAN_WAR_LEAVE_ROOM_ACK,				// 매치 룸 퇴장 응답.  Clan Server -> Game Server

	PROTOCOL_CLAN_WAR_LEAVE_TEAM_MERCENARY_ACK,		// 매치팀에 용병만 남아 매치팀 퇴장 .	Clan Server -> Game Server

	PROTOCOL_CLAN_WAR_RELEASE_INVITE_TEAM_REQ,		// 용병 초대 팀 해제. Game Server -> Clan Server

	PROTOCOL_CLAN_WAR_START_GAME_SERVER_REQ,		// GAME Server ON

	////////////////////////////////////////////////////////////////////
	// Etc
	PROTOCOL_CLAN_WAR_FORCE_MATCH_ROOM_LEAVE_ACK,	// 매치 룸 퇴장 - 리더가 퇴장함에 따라 강제적으로.

	PROTOCOL_CLAN_WAR_RESULT_REQ,					// DB에 저장된 매치 결과 요청( 최근 15개 )
	PROTOCOL_CLAN_WAR_RESULT_ACK,					// DB에 저장된 매치 결과 응답( 최근 15개 )

	PROTOCOL_CLAN_WAR_REPLACE_CLAN_NAME_ACK,		// 매치 팀 클랜명 변경 응답.
	PROTOCOL_CLAN_WAR_REPLACE_CLAN_MARK_ACK,		// 매치 팀 클랜 마크 변경 응답.
	PROTOCOL_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK,	// 매치 팀 컬러 클랜명 변경 응답.

	PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_SAVE_REQ,		// 용병 이탈 패널티 시간 저장

	PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_LOAD_REQ,		// 용병 이탈 패널티 시간 로드
	PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_LOAD_ACK,		// 
};

enum Protocol_Server_Message
{//일방적으로 서버가 보내는 메시지 
	PROTOCOL_SERVER_MESSAGE = PROTOCOL_VALUE_MESSAGE,	//	new				
	PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS,		//Connect
	PROTOCOL_SERVER_MESSAGE_DISCONNECTIONSUCCESS,	//DisConnect 	
	PROTOCOL_SERVER_MESSAGE_KICK_PLAYER,			//킥되는 유저 Open, Close 하면서 사라지는 사람..
	PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER,		//게임 중간 퇴출
	PROTOCOL_SERVER_MESSAGE_INVITED,				//초대 받았음//Wirte Nick Name(NET_NICK_NAME_SIZE), RoomIdx, NET_ROOM_PW); 
	PROTOCOL_SERVER_MESSAGE_ERROR,					//Error 메시지 //Client 가 받으면  바로 Disconnect 해야함
	PROTOCOL_SERVER_MESSAGE_ANNOUNCE,				//운영툴 공지
	PROTOCOL_SERVER_MESSAGE_FRIEND_INVITED,			//친구에게 초대를 받음							//사용하지 않음 
	PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY,			//AdminGateWay에서 방 파괴 메시지를 받음 
	PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY,		//인벤토리 내용이 변경됨.
	PROTOCOL_SERVER_MESSAGE_GAMEGUARD_ERROR,		//게임 가드 Recv 에러 
	PROTOCOL_SERVER_MESSAGE_EVENT_COUPON,			// 쿠폰 받음  // 사라질 메시지
	PROTOCOL_SERVER_MESSAGE_NEW_EVENT_CARDSET,		//새 이벤트 카드셋 지급통보 
};

enum Protocol_BattleServer
{
	PROTOCOL_BATTLE_SERVER = PROTOCOL_VALUE_BATTLE_SERVER,

	PROTOCOL_BATTLE_SERVER_CREATE_REQ,
	PROTOCOL_BATTLE_SERVER_CREATE_ACK,

	PROTOCOL_BATTLE_SERVER_DESTROY_REQ,
	PROTOCOL_BATTLE_SERVER_DESTROY_ACK,

	PROTOCOL_BATTLE_SERVER_USER_ENTER_REQ,
	PROTOCOL_BATTLE_SERVER_USER_ENTER_ACK,

	PROTOCOL_BATTLE_SERVER_USER_LEAVE_REQ,
	PROTOCOL_BATTLE_SERVER_USER_LEAVE_ACK,				// 제압모드에서만 사용.(나간 유저의 점수정보 전달을 위해)

	//////////////////////////////////////////////////////////////////////////
	// PB v1.5 서동권
	// 보내는 패킷 데이터 크기가 너무 커져서 여러번 나누어 전달하고
	// Dedication 서버와 클라이언트와 제대로 싱크를 맞추기 위하여 Dedication 에서도 ACK 를 받도록 수정
	//

	PROTOCOL_BATTLE_SERVER_ROUND_START_REQ,
	PROTOCOL_BATTLE_SERVER_ROUND_START_USER_INFO_REQ, //사용안함(2013-03-22)
	PROTOCOL_BATTLE_SERVER_ROUND_START_ACK,

	//////////////////////////////////////////////////////////////////////////

	PROTOCOL_BATTLE_SERVER_KILL_ACK,
	PROTOCOL_BATTLE_SERVER_M_BOMB_INSTALL_ACK,
	PROTOCOL_BATTLE_SERVER_M_BOMB_UNINSTALL_ACK,
	PROTOCOL_BATTLE_SERVER_M_DESTRUCTION_HP_ACK,
	PROTOCOL_BATTLE_SERVER_M_DEFENCE_ACK,

	PROTOCOL_BATTLE_SERVER_M_DINO_INFO_REQ,

	// ----------------------------------------------------------------
	// DEDICATION SERVER - CONTROL SERVER 사이에서만 사용
	// ----------------------------------------------------------------
	PROTOCOL_BATTLE_SERVER_WEAPON_LIST_REQ,				// 무기 리스트 요청//사용안함
	PROTOCOL_BATTLE_SERVER_WEAPON_LIST_ACK,				// 무기 리스트 응답//사용안함

	PROTOCOL_BATTLE_SERVER_HACK_USER_ACK,

	PROTOCOL_BATTLE_SERVER_M_TOUCHDOWN_REQ,				// 터치다운 성공시 데디 서버에 알림

	PROTOCOL_BATTLE_SERVER_SLOT_EQUIPMENT_REQ,			// 배틀 중 무기 장착 요청 (야전 상점)
	PROTOCOL_BATTLE_SERVER_SLOT_EQUIPMENT_ACK,			// 배틀 중 무기 장착 응답 (야전 상점)

	PROTOCOL_BATTLE_SERVER_RESPAWN_REQ,					// 리스폰시 주무기 정보를 데디에 알림
	PROTOCOL_BATTLE_SERVER_RESPAWN_ACK,					// 데디 응답. 아직 사용하지 않으나 클라 리스폰과 데디 주무기 정보간에 지연에 의한 싱크 문제로 핵 오탐 발생시 사용할수도 있음.

	PROTOCOL_BATTLE_SERVER_GM_PAUSE_REQ,
	PROTOCOL_BATTLE_SERVER_GM_PAUSE_ACK,

	PROTOCOL_BATTLE_SERVER_KICK_USER_ACK,			// 데디케이션 서버에서 유저를 방으로 강퇴시킬 때 사용.

	PROTOCOL_BATTLE_SERVER_MISSION_ROUND_START_REQ,// 라운드 시작, 종료를 데디케이션 서버에서 판단
	PROTOCOL_BATTLE_SERVER_MISSION_ROUND_END_REQ,

	PROTOCOL_BATTLE_SERVER_QA_AUTO_COMMAND_REQ,			// QA 자동화용 커맨드 사용 알림
	PROTOCOL_BATTLE_SERVER_QA_AUTO_COMMAND_ACK,

	// ------------------------------------------------------------------
	// Domination PROTOCOL
	// ------------------------------------------------------------------

	// Flow (1/4)
	PROTOCOL_BATTLE_SERVER_WAVE_PRE_START_REQ_notuse = PROTOCOL_BATTLE_SERVER + 0x0080,
	PROTOCOL_BATTLE_SERVER_WAVE_PRE_START_ACK,			// 현재라운드, 현재웨이브, 최대웨이브 통보

	// Flow (2/4)
	PROTOCOL_BATTLE_SERVER_WAVE_READY_INFO_REQ,			// 미사용. READY버튼 누른 유저가 전송
	PROTOCOL_BATTLE_SERVER_WAVE_READY_INFO_ACK,			// 데디에서 보내면 무시됨: 유저 상태 전송

	// Flow (3/4)
	PROTOCOL_BATTLE_SERVER_WAVE_START_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_WAVE_START_ACK,				// 상태싱크만 함: 등장할 공룡 마리수

	// Flow (4/4)
	PROTOCOL_BATTLE_SERVER_WAVE_END_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_WAVE_END_ACK,				// 유저4명의 킬수, 획득점수

	// 인터미션 준비 취소 버튼
	PROTOCOL_BATTLE_SERVER_WAVE_READY_INFO_CANCEL_REQ,	// READY취소 버튼 누른 유저가 전송
	PROTOCOL_BATTLE_SERVER_WAVE_READY_INFO_CANCEL_ACK_notuse,

	// 유저스킬 업그레이드 요청
	PROTOCOL_BATTLE_SERVER_USER_SKILL_UPGRADE_REQ,		// PACKET_DOMINATION_USER_SKILL_UPGRADE_REQ
	PROTOCOL_BATTLE_SERVER_USER_SKILL_UPGRADE_ACK,		// PACKET_DOMINATION_USER_SKILL_UPGRADE_ACK

	// 디버그 메세지 출력 요청
	PROTOCOL_BATTLE_SERVER_PRINT_MESSAGE_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_PRINT_MESSAGE_ACK,			// PACKET_DOMINATION_PRINT_MESSAGE_ACK

	// 애시드아이템 등장알림
	PROTOCOL_BATTLE_SERVER_APPEAR_ACIDITEM_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_APPEAR_ACIDITEM_ACK,			// 애시드아이템공룡의 등장을 알립니다. PACKET_DOMINATION_APPEAR_ACIDITEM_ACK

	// 치트메시지
	PROTOCOL_BATTLE_SERVER_CHEAT_MESSAGE_REQ,			// 치트메세지를 클라가 데디로 보냅니다.
	PROTOCOL_BATTLE_SERVER_CHEAT_MESSAGE_ACK_notuse,

	// 뮤턴트티렉스 하울링 스킬로 인한 유저 느려짐 효과
	PROTOCOL_BATTLE_SERVER_MUTANTTREX_HOWLING_SLOW_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_MUTANTTREX_HOWLING_SLOW_ACK,	// 몇초 느려지는지.

	// 인벤토리에 아이템 지급 ( 데디서버 -> 게임서버 )
	PROTOCOL_BATTLE_SERVER_INSERT_ITEM_USERINVEN_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_INSERT_ITEM_USERINVEN_ACK,	// PACKET_DOMINATION_INSERVER_INSERT_ITEM_USERINVEN_ACK

	// 아이템사용에 대한 트랜스서버의 응답
	PROTOCOL_BATTLE_SERVER_USE_DOMIITEM_RESULT_REQ,		// PACKET_DOMINATION_INSERVER_USE_DOMIITEM_RESULT_REQ
	PROTOCOL_BATTLE_SERVER_USE_DOMIITEM_RESULT_ACK_notuse,

	// 스테이지 점프 알림
	PROTOCOL_BATTLE_SERVER_STAGE_JUMP_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_STAGE_JUMP_ACK,				// PACKET_DOMINATION_INSERVER_STAGE_JUMP_ACK

	// 유저 강퇴
	PROTOCOL_BATTLE_SERVER_KICKUSER_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_KICKUSER_ACK,				// PACKET_DOMINATION_INSERVER_KICKUSER_ACK

	PROTOCOL_BATTLE_SERVER_NAVIMESH_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_NAVIMESH_ACK,				// PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK

	PROTOCOL_BATTLE_SERVER_AI_COLLISION_REQ_notuse,
	PROTOCOL_BATTLE_SERVER_AI_COLLISION_ACK,			// PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK
};



enum Protocol_Community
{
	PROTOCOL_COMMUNITY = PROTOCOL_VALUE_COMMUNITY,

	//////////////////////////////////////////////////////////////////////////
	// 유저 위치 찾기
	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_BASE_FIND_SESSION_USER_POSITION_REQ_notuse,			// 사용안함 - 동일서버   유저 위치 찾기 요청(불법)
	PROTOCOL_BASE_FIND_UID_USER_POSITION_REQ_notuse,				// 사용안함 - UID로	  유저 위치 찾기 요청(불법)
	PROTOCOL_BASE_FIND_NICK_USER_POSITION_REQ_notuse,				// 사용안함
	PROTOCOL_BASE_FIND_USER_POSITION_ACK_notuse,					// 사용안함




	//////////////////////////////////////////////////////////////////////////
	// 유저 초대하기
	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_COMMUNITY_USER_INVITED_REQ,						// 배틀룸에 유저찾기로 검색된 유저 혹은 로비에 있는 초대
	PROTOCOL_COMMUNITY_USER_INVITED_ACK,						// 배틀룸에 유저초대 실패시 발신자에게 전달
	PROTOCOL_COMMUNITY_USER_INVITED_REQUEST_ACK,				// 배틀룸에 유저초대 성공시 수신자에게 전달
};

enum Protocol_Lobby
{
	PROTOCOL_LOBBY = PROTOCOL_VALUE_LOBBY,

	PROTOCOL_LOBBY_ENTER_REQ,					// 로비로 들어가기
	PROTOCOL_LOBBY_ENTER_ACK,

	PROTOCOL_LOBBY_LEAVE_REQ,					// 로비에서 나옴...
	PROTOCOL_LOBBY_LEAVE_ACK,

	PROTOCOL_LOBBY_GET_ROOMLIST_REQ,			// 방 리스트를 얻습니다.(로비대기유저정보 포함)
	PROTOCOL_LOBBY_GET_ROOMLIST_ACK,

	PROTOCOL_LOBBY_GET_ROOMINFO_REQ,			// 방의 상세정보를 얻습니다.
	PROTOCOL_LOBBY_GET_ROOMINFO_ACK,

	PROTOCOL_LOBBY_QUICKJOIN_ROOM_REQ,			// 퀵조인합니다.
	PROTOCOL_LOBBY_QUICKJOIN_ROOM_ACK,


	//PROTOCOL_LOBBY_CREATE_NICK_NAME_REQ,		// 닉네임 설정
	//PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK,
	//PROTOCOL_LOBBY_CHECK_NICK_NAME_REQ,			// 닉네임 확인(중복확인)
	//PROTOCOL_LOBBY_CHECK_NICK_NAME_ACK,

	PROTOCOL_LOBBY_GET_ROOMINFOADD_REQ,			// 룸정보 요청 
	PROTOCOL_LOBBY_GET_ROOMINFOADD_ACK,
	PROTOCOL_LOBBY_CREATE_TRAINING_REQ,			// 사격장 방 만들기 요청
	PROTOCOL_LOBBY_CREATE_TRAINING_ACK,			// 사격장 방 만들기 응답

	PROTOCOL_LOBBY_CHATTING_ACK,				// 로비 채팅입니다. 

#ifdef USE_TRANS_FIND
	PROTOCOL_LOBBY_FIND_NICK_NAME_REQ,			// 유저찾기 요청 (닉네임으로)
	PROTOCOL_LOBBY_FIND_NICK_NAME_ACK,			// 응답

	PROTOCOL_LOBBY_GET_UID_BY_NICK_NAME_REQ,	// 유저 UID 가져오기
	PROTOCOL_LOBBY_GET_UID_BY_NICK_NAME_ACK,	// 응답	
#endif

	PROTOCOL_LOBBY_GM_EXIT_USER_REQ,			// GM이 로비에서 유저 Kick
	PROTOCOL_LOBBY_GM_EXIT_USER_ACK,

	PROTOCOL_LOBBY_ABUSING_POPUP_END_REQ,		// 어뷰징 체크된 유저가 팝업시간을 모두 기다렸을때 미지급 되었던 보상( EPX, POINT 요청 ) 요청
	PROTOCOL_LOBBY_ABUSING_POPUP_END_ACK,		// 어뷰징 체크된 유저가 팝업시간을 모두 기다렸을때 미지급 되었던 보상( EPX, POINT 요청 ) 응답

	PROTOCOL_LOBBY_QUICKJOIN_SETTING_REQ,		// 빠른 입장 설정 요청

	PROTOCOL_LOBBY_VIEW_USER_ITEM_REQ,			// 로비에서 상대방 장비,아이템 보기 요청
	PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK,			// 로비에서 상대방 장비,아이템 보기 응답

	//PROTOCOL_LOBBY_NEW_CREATE_ROOM_REQ,			// 방 만들기
	//PROTOCOL_LOBBY_NEW_CREATE_ROOM_ACK,	 

	//PROTOCOL_LOBBY_NEW_JOIN_ROOM_REQ,			// 선택된 방으로 들어가기
	//PROTOCOL_LOBBY_NEW_JOIN_ROOM_ACK,

	PROTOCOL_LOBBY_SET_BIRTHDAY_REQ,			// 생년월일 설정
	PROTOCOL_LOBBY_SET_BIRTHDAY_ACK,

	PROTOCOL_LOBBY_GM_GET_UID_REQ,
	PROTOCOL_LOBBY_GM_GET_UID_ACK,

	PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_REQ,		// 로비에서 유저 장착 정보 보기 요청(사용안함)

	// 준영 ViewOtherItem
	PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_ACK,		// 로비에서 유저 장착 정보 보기 응답(캐쉬형태)
};

enum Protocol_Inventory
{
	PROTOCOL_INVENTORY = PROTOCOL_VALUE_INVENTORY,
	PROTOCOL_INVENTORY_ENTER_REQ,
	PROTOCOL_INVENTORY_ENTER_ACK,

	PROTOCOL_INVENTORY_LEAVE_REQ,		// 인벤 나가기 요청
	PROTOCOL_INVENTORY_LEAVE_ACK,		// 인벤 나가기 응답

	PROTOCOL_INVENTORY_GET_INFO_REQ,
	PROTOCOL_INVENTORY_GET_INFO_ACK,
};

enum Protocol_MyInfo
{
	PROTOCOL_MYINFO = PROTOCOL_VALUE_MYINFO,

	PROTOCOL_MYINFO_ENTER_REQ,			// 1.0 Only
	PROTOCOL_MYINFO_ENTER_ACK,			// 1.0 Only

	PROTOCOL_MYINFO_LEAVE_REQ,			// 1.0 Only
	PROTOCOL_MYINFO_LEAVE_ACK,			// 1.0 Only
};

enum PROTOCOL_Rs_Igs
{
	PROTOCOL_RS_IGS = PROTOCOL_VALUE_RS_IGS,

	// (구1.0) 뽑기상점 Roulette Shop.
	PROTOCOL_RS_ENTER_REQ,				// 입장요청.
	PROTOCOL_RS_ENTER_ACK,				// 입장응답.

	PROTOCOL_RS_LEAVE_REQ,							// 퇴장요청.
	PROTOCOL_RS_LEAVE_ACK,							// 퇴장응답.

	PROTOCOL_RS_ITEM_INFO_REQ,						//  아이템정보요청.	 ( 개수 + 머리,꼬리 정보 )
	PROTOCOL_RS_ITEM_INFO_ACK,						//  아이템정보응답.  ( 개수 + 머리,꼬리 정보 )

	PROTOCOL_RS_HIDDEN_ITEM_INFO_REQ,				//  아이템정보요청.	 ( 히든 )
	PROTOCOL_RS_HIDDEN_ITEM_INFO_ACK,				//  아이템정보응답.  ( 히든 )

	PROTOCOL_RS_LOSE_ITEM_INFO_REQ,					//  아이템정보요청.	 ( 꽝 )
	PROTOCOL_RS_LOSE_ITEM_INFO_ACK,					//  아이템정보응답.  ( 꽝 )

	PROTOCOL_RS_GRADE1_ITEM_INFO_REQ,				//  아이템정보요청.	 ( 별 1개 )
	PROTOCOL_RS_GRADE1_ITEM_INFO_ACK,				//  아이템정보응답.	 ( 별 1개 )

	PROTOCOL_RS_GRADE2_ITEM_INFO_REQ,				//  아이템정보요청.	 ( 별 2개 )
	PROTOCOL_RS_GRADE2_ITEM_INFO_ACK,				//  아이템정보응답.	 ( 별 2개 )

	PROTOCOL_RS_GRADE3_ITEM_INFO_REQ,				//  아이템정보요청.	 ( 별 3개 )
	PROTOCOL_RS_GRADE3_ITEM_INFO_ACK,				//  아이템정보응답.	 ( 별 3개 )

	PROTOCOL_RS_ROULETTE_START_REQ,					// 뽑기 시작 (단판,연속) 요청. ( 시작버튼)
	PROTOCOL_RS_ROULETTE_RESULT_ACK,				// 뽑기 결과.

	PROTOCOL_RS_JACKPOT_NOTIFY_REQ,					// 잭팟 당청 알림 요청. (잭팟당첨된)Clent -> Game
	PROTOCOL_RS_JACKPOT_NOTIFY_ACK,					// 잭팟 당청 알림 응답. Game -> 모든Client

	PROTOCOL_AUTH_RS_JACKPOT_REQ,					// 뽑기 상점 잭팟 요청. Game - Trans 
	PROTOCOL_AUTH_RS_JACKPOT_ACK,					// 뽑기 상점 잭팟 응답  Trans - Game

	PROTOCOL_AUTH_RS_START_REQ,						// 뽑기 시작 요청. Game -> Trans.
	PROTOCOL_AUTH_RS_RESULT_ACK,					// 뽑기 결과 응답. Trans-> Game.

	PROTOCOL_AUTH_RS_ENTER_REQ,						// 뽑기 상점 입장 요청. Game -> Trans.
	PROTOCOL_AUTH_RS_ENTER_ACK,						// 뽑기 상점 입장 응담. Trans-> Game.

	PROTOCOL_AUTH_RS_ITEM_INFO_REQ,					// 아이템 정보 요청. Game -> Trans.
	PROTOCOL_AUTH_RS_ITEM_INFO_ACK,					// 아이템 정보 응담. Trans-> Game.

	PROTOCOL_AUTH_RS_JACKPOT_USER_NICK_INSERT_REQ,	// 대박 당첨 유저 닉네임 기록. Trans->Control.




	// -----------------------------------------------------------------------------
	// field shop. 야전 상점
	PROTOCOL_FIELDSHOP_GOODSLIST_REQ,	// Request goods list from client
	PROTOCOL_FIELDSHOP_GOODSLIST_ACK,	// Send goods list to client
	PROTOCOL_FIELDSHOP_OPEN_REQ,		// Request field shop open (compare version)
	PROTOCOL_FIELDSHOP_OPEN_ACK,

	PROTOCOL_FIELDSHOP_SEND_NEWVERSION_REQ,	// Request goods list from client
	PROTOCOL_FIELDSHOP_SEND_NEWVERSION_ACK,	// Send Field Shop List (Trans->Game)
};

enum Protocol_Room
{
	PROTOCOL_ROOM = PROTOCOL_VALUE_ROOM,

	PROTOCOL_ROOM_CREATE_REQ,				// 방 만들기
	PROTOCOL_ROOM_CREATE_ACK,

	PROTOCOL_ROOM_JOIN_REQ,						// 선택된 방으로 들어가기
	PROTOCOL_ROOM_JOIN_ACK,

	PROTOCOL_ROOM_GET_SLOTONEINFO_REQ,			//방의 슬롯 정보와 닉네임을 얻습니다.
	PROTOCOL_ROOM_GET_SLOTONEINFO_ACK,

	PROTOCOL_ROOM_GET_SLOTINFO_REQ,				//방의 슬롯 정보를 얻습니다.
	PROTOCOL_ROOM_GET_SLOTINFO_ACK,

	PROTOCOL_ROOM_CHARA_SHIFT_POS_REQ,			// 대표캐릭설정 요청
	PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK,			// 대표캐릭설정 응답
	PROTOCOL_ROOM_CHARA_SHIFT_POS_OTHER_ACK,	// 룸에 있는 유저들과 동기화

	////////////////////////////////////////////////////////////////////////////////
	// 여기까지 맞춤
	////////////////////////////////////////////////////////////////////////////////

	PROTOCOL_ROOM_GET_PLAYERINFO_REQ,			//플레이어 정보 
	PROTOCOL_ROOM_GET_PLAYERINFO_ACK,

	PROTOCOL_ROOM_GET_NICKNAME_REQ,				//닉네임을 보낸다. 
	PROTOCOL_ROOM_GET_NICKNAME_ACK,

	PROTOCOL_ROOM_CHANGE_ROOMINFO_REQ,			// 방정보 변경
	PROTOCOL_ROOM_CHANGE_ROOMINFO_ACK,

	PROTOCOL_ROOM_CHANGE_PASSWD_REQ,			//패스워드 변경 
	PROTOCOL_ROOM_CHANGE_PASSWD_ACK,

	PROTOCOL_ROOM_CHANGE_SLOT_REQ,				//플레이어 강퇴 
	PROTOCOL_ROOM_CHANGE_SLOT_ACK,

	PROTOCOL_ROOM_CHATTING_ACK,

	PROTOCOL_ROOM_LEAVE_ROOM_REQ,				//차후 삭제 
	PROTOCOL_ROOM_LEAVE_ROOM_ACK,

	PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_REQ,		//팀을 변경합니다. 
	PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_ACK,

	PROTOCOL_ROOM_GET_LOBBY_USER_LIST_REQ,		//레디룸에서 로비유저 목록 요청
	PROTOCOL_ROOM_GET_LOBBY_USER_LIST_ACK,		//
	PROTOCOL_ROOM_INVITE_LOBBY_USER_REQ,		//초대하기 Count(4), SessionId(4) * 5을 넘을수 없음.
	PROTOCOL_ROOM_INVITE_LOBBY_USER_ACK,		//초대했다는 메시지. 필요없을듯.
	PROTOCOL_ROOM_FRIEND_INVITE_LOBBY_USER_REQ, //친구 목록에서 초대하기			//사용하지 않음
	PROTOCOL_ROOM_FRIEND_INVITE_LOBBY_USER_ACK,	//초대했다는 메시지 추후 필요 .		//사용하지 않음
	PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_REQ,	// 옵션정보 변경 
	PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_ACK,

	PROTOCOL_ROOM_REQUEST_MAIN_REQ,				//방장 요청하기 
	PROTOCOL_ROOM_REQUEST_MAIN_ACK,

	PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_REQ,		//방장이 방장 변경을 요청하기 (버튼)
	PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK,		// 

	PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_REQ,	//방장이 방장 변경을 요청하기 
	PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK,	// 

	PROTOCOL_ROOM_CHECK_MAIN_REQ,				//방장 체크하기
	PROTOCOL_ROOM_CHECK_MAIN_ACK,

	PROTOCOL_ROOM_MASTER_TEAM_CHANGE_REQ,		//진영 변경
	PROTOCOL_ROOM_MASTER_TEAM_CHANGE_ACK,		//!사용하지 않음.

	PROTOCOL_ROOM_TEAM_BALANCE_REQ,				//!사용하지 않음.
	PROTOCOL_ROOM_TEAM_BALANCE_ACK,				//팀밸런스 적용

	PROTOCOL_ROOM_UNREADY_4VS4_REQ,				//!사용하지 않음.
	PROTOCOL_ROOM_UNREADY_4VS4_ACK,				//4vs4 모드 참여자 제한에 의해 레디상태 해제 통보

	PROTOCOL_ROOM_GET_RANK_REQ,					//계급을 보낸다. 
	PROTOCOL_ROOM_GET_RANK_ACK,

	PROTOCOL_ROOM_GET_COLOR_NICK_REQ,
	PROTOCOL_ROOM_GET_COLOR_NICK_ACK,

	PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_REQ,		// 방정보 변경
	PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_ACK,

	PROTOCOL_ROOM_EQUIPMENT_TEMP_WEAPON_REQ,	// 임시무기 장착
	PROTOCOL_ROOM_EQUIPMENT_TEMP_WEAPON_ACK,	//

	PROTOCOL_ROOM_GM_KICK_USER_REQ,				// GM이 사용하는 플레이어 강퇴 
	PROTOCOL_ROOM_GM_KICK_USER_ACK,

	PROTOCOL_ROOM_GET_USER_ITEM_REQ,			// 유저 장착 정보 보기(사용안함)

	// 준영 ViewOtherItem
	PROTOCOL_ROOM_GET_USER_ITEM_ACK,			// 유저 장착 정보 보기(캐쉬형태)	

	PROTOCOL_ROOM_GM_EXIT_USER_REQ,				// GM이 사용하는 플레이어 접속종료
	PROTOCOL_ROOM_GM_EXIT_USER_ACK,

	PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_REQ,		// 옵저버 슬롯과 일반슬롯의 슬롯위치 이동	// 0 : 일반슬롯으로 이동, 1 : 옵저버슬롯으로 이동
	PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK,

	PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_REQ,		// 히든 슬롯과 일반슬롯의 슬롯위치 이동(GM만 사용가능)	// 0 : 일반슬롯으로 이동, 1 : 히든슬롯으로 이동
	PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK,

	PROTOCOL_ROOM_GM_BLOCK_USER_REQ,			// GM이 사용하는 플레이어 블록
	PROTOCOL_ROOM_GM_BLOCK_USER_ACK,			// 사용안함

	PROTOCOL_ROOM_GM_DESTROY_ROOM_REQ,			// GM이 사용하는 방 파괴
	PROTOCOL_ROOM_GM_DESTROY_ROOM_ACK,

	PROTOCOL_ROOM_LOADING_START_REQ,
	PROTOCOL_ROOM_LOADING_START_ACK,

	PROTOCOL_ROOM_TS_EVENT_REQ,					// TS 이벤트 전송 Game->Client
	PROTOCOL_ROOM_TS_EVENT_ACK,					// TS 이벤트 응답

	PROTOCOL_ROOM_GM_GET_UID_REQ,
	PROTOCOL_ROOM_GM_GET_UID_ACK,


	//PROTOCOL_ROOM_NEW_GET_SLOTINFO_REQ,				// 사용안함
	//PROTOCOL_ROOM_NEW_GET_SLOTINFO_ACK,				// 방의 슬롯 정보 응답

	//	PROTOCOL_BASE에 구현 됨
	//PROTOCOL_ROOM_NEW_GET_PLAYERINFO_REQ,			//플레이어 정보 
	//PROTOCOL_ROOM_NEW_GET_PLAYERINFO_ACK,		

	PROTOCOL_ROOM_NEW_GET_SLOTONEINFO_REQ,			//(사용안함)방의 슬롯 정보와 닉네임을 얻습니다.
	PROTOCOL_ROOM_NEW_GET_SLOTONEINFO_ACK,



	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_ROOM_GET_USER_EQUIPMENT_REQ,		// 유저 장착 정보 보기(룸에서 캐릭터뷰에 표시)
	PROTOCOL_ROOM_GET_USER_EQUIPMENT_ACK,

	//////////////////////////////////////////////////////////////////////////

	PROTOCOL_ROOM_NEW_DOMI_JOIN_US_REQ,				// 사용안함
	PROTOCOL_ROOM_NEW_DOMI_JOIN_US_ACK,				// 유저에게 제압모드에 난입가능하다고 알려줍니다.

	PROTOCOL_ROOM_GET_COLOR_MUZZLE_FLASH_REQ,		// 컬러 총구화염 변경시 전달		
	PROTOCOL_ROOM_GET_COLOR_MUZZLE_FLASH_ACK,

	PROTOCOL_ROOM_INFO_ENTER_REQ,				//정보 보기 			
	PROTOCOL_ROOM_INFO_ENTER_ACK,
	PROTOCOL_ROOM_INFO_LEAVE_REQ,				//정보 보기에서 나가기 
	PROTOCOL_ROOM_INFO_LEAVE_ACK,

	PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_REQ,
	PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_ACK,
};

// --------------------------------------
// | PROTOCOL_BATTLE_READYBATTLE_REQ	|
// --------------------------------------
//					V
// --------------------------------------
// | 일반 유저	: 게임준비상태로 변경	|
// | 방장		: 게임시작				|
// --------------------------------------
//					V
// --------------------------------------
// | PROTOCOL_BATTLE_READYBATTLE_ACK	|
// --------------------------------------
//					V
// --------------------------------------
// | PROTOCOL_BATTLE_PRESTARTBATTLE_REQ |
// --------------------------------------
enum Protocol_Battle
{
	PROTOCOL_BATTLE = PROTOCOL_VALUE_BATTLE,

	PROTOCOL_BATTLE_HOLE_CHECK_REQ,					//홀펀칭 체크 
	PROTOCOL_BATTLE_HOLE_CHECK_ACK,

	PROTOCOL_BATTLE_READYBATTLE_REQ,				// 게임 시작하기 (이 메시지를 보내고  로딩합니다. )
	PROTOCOL_BATTLE_READYBATTLE_ACK,				// 방장 게임시작시 레뒤중인 자신의 슬롯 정보를 전송받는다.(로딩 or 상태변경)	

	PROTOCOL_BATTLE_SLOT_STATE_CHANGE_ACK,			// 슬롯의 스테이트 정보 변경

	//PROTOCOL_BATTLE_START_COUNTDOWN_REQ,
	PROTOCOL_BATTLE_START_COUNTDOWN_ACK,			// 배틀 시작 전 카운트 다운

	//PROTOCOL_BATTLE_START_LOADING_REQ,
	PROTOCOL_BATTLE_START_GAME_ACK,					// 레뒤가 완료 되어 게임을 시작 합니다.

	PROTOCOL_BATTLE_START_GAME_TRANS_ACK,			// 난입을 하여 유저가 접속을 시도 함을 알립니다.

	PROTOCOL_BATTLE_PRESTARTBATTLE_REQ,				// Loading 완료 후 UDP 접속을 시도 합니다.
	PROTOCOL_BATTLE_PRESTARTBATTLE_ACK,

	PROTOCOL_BATTLE_STARTBATTLE_REQ,				// Hole Punching이 끝난후 이메시지를 보내고,
	PROTOCOL_BATTLE_STARTBATTLE_ACK,				// 게임 시간을 받아야 합니다. 	

	PROTOCOL_BATTLE_GIVEUPBATTLE_REQ,				// 중간에 배틀에서 나올때 
	PROTOCOL_BATTLE_GIVEUPBATTLE_ACK,

	PROTOCOL_BATTLE_DEATH_REQ,					// 죽었을때 보내는 메시지 처음 시작할때도 보냅니다. -1 이라는 값으로 보내면 서버에서 그 값을 읽고 무기 리스트를 보내요.
	PROTOCOL_BATTLE_DEATH_ACK,

	PROTOCOL_BATTLE_RESPAWN_REQ,				// 다시 살아날때 보내는 메시지 
	PROTOCOL_BATTLE_RESPAWN_ACK,				//

	PROTOCOL_BATTLE_ENDBATTLE_REQ,				// 배틀이 종료 되었을때(사용안함:서버에서 판단)
	PROTOCOL_BATTLE_ENDBATTLE_ACK,

	PROTOCOL_BATTLE_EQUIPMENT_INFO_REQ,				//사용하지 않음	//추후 사용될 가능성 있음 
	PROTOCOL_BATTLE_EQUIPMENT_INFO_ACK,

	////////////////////////////////////////////////////////////////////////////////////
	// 여기까지 맞춤
	////////////////////////////////////////////////////////////////////////////////////

	PROTOCOL_BATTLE_TIMEOUTCLIENT_REQ,			//방장이 일반유저에게 메시지를 받지 못할때 - 7초 
	PROTOCOL_BATTLE_TIMEOUTCLIENT_ACK,			//이거 받은사람은 무조건 로비로 이동 ( 일반유저용 )
	PROTOCOL_BATTLE_TIMEOUTMAINCLIENT_REQ,		//일반유저가 방장에게 메시지를 받지 못할때  3초// 서버에서 2초 동안 모든 클라이언트 검사 / 모든사람 ? 
	PROTOCOL_BATTLE_SENDPING_REQ,				// CS -> MS		UDP Ping 값을 보냅니다.
	PROTOCOL_BATTLE_SENDPING_ACK,				// MS -> ALL	UDP Ping 값을 받습니다.	
	PROTOCOL_BATTLE_LEAVEP2PSERVER_REQ,			// 방장이 나갔다.. 새로운 방장을 알려줘야한다.
	PROTOCOL_BATTLE_LEAVEP2PSERVER_ACK,

	PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_REQ,	//미션전 라운드 시작 준비  
	PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK,
	PROTOCOL_BATTLE_MISSION_ROUND_START_REQ,		//미션전 라운드 시작 
	PROTOCOL_BATTLE_MISSION_ROUND_START_ACK,
	PROTOCOL_BATTLE_MISSION_ROUND_END_REQ,			//미션전 라운드 끝
	PROTOCOL_BATTLE_MISSION_ROUND_END_ACK,
	PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_REQ,		//폭탄 설치 
	PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_ACK,
	PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_REQ,		//폭탄 해체 
	PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_ACK,

	PROTOCOL_BATTLE_PICKUP_WEAPON_REQ,				//총 줍기 
	PROTOCOL_BATTLE_PICKUP_WEAPON_ACK,

	PROTOCOL_BATTLE_RECORD_REQ,						//아직 사용하지 않음 
	PROTOCOL_BATTLE_RECORD_ACK,						//난입한 사람에게 보내준다. 

	PROTOCOL_BATTLE_MISSION_GENERATOR_DESTROY_REQ,	//발전기 폭파 
	PROTOCOL_BATTLE_MISSION_GENERATOR_DESTROY_ACK,	//

	PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_REQ,		//발전기 데미지.
	PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_ACK,

	PROTOCOL_BATTLE_TIMERSYNC_REQ,					// 게임내 배틀 진행중인 상황에 매 5초마다 누적타임을 서버에 전송( 비정상시에 연결 종료)
	PROTOCOL_BATTLE_TIMERSYNC_ACK,					// 타임싱크에 관한 서버로부터의 응답: invalid sync인 경우에만 받도록 합니다.

	PROTOCOL_BATTLE_PRESTARTBATTLE_DSERVER_REQ,		// UDP 접속을 합니다.( P2P UDP 홀펀칭 시도 ) 네트워크 정보를 받습니다.
	PROTOCOL_BATTLE_PRESTARTBATTLE_DSERVER_ACK,

	PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_REQ,	// 챌린지모드 난이도 변경
	PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK,	// 

	PROTOCOL_BATTLE_RESPAWN_FOR_AI_REQ,				// 사용하지 않습니다.						
	PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK,				// 게임서버가 클라이언트로 AI의 리스폰을 알립니다.		

	PROTOCOL_BATTLE_PRESTARTBATTLE_RELAY_REQ,		// UDP 접속을 합니다.( P2P UDP 홀펀칭 시도 ) 네트워크 정보를 받습니다. 
	PROTOCOL_BATTLE_PRESTARTBATTLE_RELAY_ACK,

	PROTOCOL_BATTLE_MISSION_TOUCHDOWN_REQ,
	PROTOCOL_BATTLE_MISSION_TOUCHDOWN_ACK,

	PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_REQ,		// 방어 오브젝트 데미지.
	PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_ACK,

	PROTOCOL_BATTLE_MISSION_TOUCHDOWN_COUNT_REQ,	// 양팀의 터치다운 횟수
	PROTOCOL_BATTLE_MISSION_TOUCHDOWN_COUNT_ACK,

	PROTOCOL_BATTLE_MISSION_DEATHBLOW_REQ,			// 공룡 임무 (DEATH BLOW)
	PROTOCOL_BATTLE_MISSION_DEATHBLOW_ACK,

	//PROTOCOL_BATTLE_EMPTY_AA,
	//PROTOCOL_BATTLE_EMPTY_BB,

	//PROTOCOL_BATTLE_RESULT_REQ,						// 배틀 종료 결과 전달 요청
	//PROTOCOL_BATTLE_RESULT_ACK,

	PROTOCOL_BATTLE_SUPPLY_BOX_RESULT_REQ,			// 보급 상자 지급 여부 요청
	PROTOCOL_BATTLE_SUPPLY_BOX_RESULT_ACK,

	PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_REQ,	// 튜토리얼 모드 라운드 종료 요청
	PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_ACK,

	// 강제탈퇴투표 관련 14가지( 이중 5개의 REQ는 사용되지 않습니다.) 2011.05.27 안국정

	PROTOCOL_BATTLE_SUGGEST_KICKVOTE_REQ,		// 강제탈퇴투표 요청이 들어옵니다.
	PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK,		// 강제탈퇴투표 가/부 결과를 통보합니다.(신고유저만 받습니다.)

	PROTOCOL_BATTLE_START_KICKVOTE_REQ,			// 사용하지 않습니다.
	PROTOCOL_BATTLE_START_KICKVOTE_ACK,			// 강제탈퇴투표 시작을 통보합니다.(신고유저와 피신고유저를 제외한 전 클라이언트가 받습니다.)

	PROTOCOL_BATTLE_VOTE_KICKVOTE_REQ,			// 투표내용이 들어옵니다.
	PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK,			// 투표내용 반영여부 결과를 통보합니다.( 극히 희박한 확률이나 버그나 해킹을 제외하곤 투표내용 반영됩니다.)

	PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_REQ,	// 사용하지 않습니다.
	PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_ACK, // 투표결과를 전 클라이언트에 통보합니다.(피신고유저만 제외합니다.)

	PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_REQ, // 사용하지 않습니다.
	PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_ACK,	// 피신고자의 ROOM 자진OUT으로 인해 투표가 중지되었음을 통보합니다.

	PROTOCOL_BATTLE_NOTIFY_CURRENT_KICKVOTE_REQ,// 사용하지 않습니다.
	PROTOCOL_BATTLE_NOTIFY_CURRENT_KICKVOTE_ACK,// 표가 집계될때마다 현황을 전 클라이언트에 통보합니다.(피신고유저만 제외합니다.)

	PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_REQ, // 사용하지 않습니다.
	PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK, // 투표가 가결되어 강제퇴장 명령을 받았습니다.

	PROTOCOL_BATTLE_MISSION_TOUCHDOWN_BLOCK_ACK,	// 터치다운을 일정시간 이상 못하게 했음을 전달

	PROTOCOL_BATTLE_GM_PAUSE_REQ,					// GM PAUSE 기능 요청
	PROTOCOL_BATTLE_GM_PAUSE_ACK,					// GM PAUSE 요청 결과 알림. (성공 or 실패) 성공이라면 요청자 슬롯번호, 실패라면 실패사유번호.

	PROTOCOL_BATTLE_NOTIFY_HACK_USER_ACK,			// 배틀중 핵 사용 유저의 SlotIdx를 방에 있는 모든유저에게 보냄.

	// Domination
	PROTOCOL_BATTLE_WAVE_PRE_START_REQ_notuse = PROTOCOL_BATTLE + 0x0070,
	PROTOCOL_BATTLE_WAVE_PRE_START_ACK,

	PROTOCOL_BATTLE_WAVE_READY_INFO_REQ,
	PROTOCOL_BATTLE_WAVE_READY_INFO_ACK,

	PROTOCOL_BATTLE_WAVE_START_REQ_notuse,
	PROTOCOL_BATTLE_WAVE_START_ACK,

	PROTOCOL_BATTLE_WAVE_END_REQ_notuse,
	PROTOCOL_BATTLE_WAVE_END_ACK,

	PROTOCOL_BATTLE_WAVE_INFO_REQ_notuse,
	PROTOCOL_BATTLE_WAVE_INFO_ACK,

	PROTOCOL_BATTLE_WAVE_READY_INFO_CANCEL_REQ,
	PROTOCOL_BATTLE_WAVE_READY_INFO_CANCEL_ACK_notuse,

	PROTOCOL_BATTLE_USER_SKILL_UPGRADE_REQ,
	PROTOCOL_BATTLE_USER_SKILL_UPGRADE_ACK,

	PROTOCOL_BATTLE_PRINT_MESSAGE_REQ_notuse,
	PROTOCOL_BATTLE_PRINT_MESSAGE_ACK,

	PROTOCOL_BATTLE_FINAL_DOMI_SCORE_REQ_notuse,
	PROTOCOL_BATTLE_FINAL_DOMI_SCORE_ACK,

	PROTOCOL_BATTLE_APPEAR_ACIDITEM_REQ_notuse,
	PROTOCOL_BATTLE_APPEAR_ACIDITEM_ACK,

	PROTOCOL_BATTLE_CHEAT_MESSAGE_REQ,
	PROTOCOL_BATTLE_CHEAT_MESSAGE_ACK_notuse,

	PROTOCOL_BATTLE_MUTANTTREX_HOWLING_SLOW_REQ_notuse,
	PROTOCOL_BATTLE_MUTANTTREX_HOWLING_SLOW_ACK,

	PROTOCOL_BATTLE_USE_ITEM_REQ,
	PROTOCOL_BATTLE_USE_ITEM_ACK,

	PROTOCOL_BATTLE_NAVIMESH_REQ_notuse,
	PROTOCOL_BATTLE_NAVIMESH_ACK,				// PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK

	PROTOCOL_BATTLE_AI_COLLISION_REQ_notuse,
	PROTOCOL_BATTLE_AI_COLLISION_ACK,			// PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK

	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_REQ,			// 난입유저 레뒤시 게임스코어 전송 요청
	PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_ACK,

	PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_ACK,		// (사용안함)ReadyBattle시 기존 게임중인 유저들의 장착 정보를 보냄(배틀 최초 로딩을 위함)
	PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_OTHER_ACK,	// (사용안함)ReadyBattle시 레뒤중인 유저들에게 자신의 장착 정보를 전송한다.(배틀 최초 로딩을 위함)

	//PROTOCOL_BATTLE_NEW_EQUIPMENT_INFO_ACK,				// StartBattle시 기존 게임중인 유저들의 장착 정보를 보냄(동기화를 위함)
	PROTOCOL_BATTLE_NEW_EQUIPMENT_INFO_OTHER_ACK,		// (사용안함)StartBattle시 레뒤중인 유저들에게 자신의 슬롯정보를 전송한다.(동기화를 위함)

	PROTOCOL_BATTLE_SLOT_EQUIPMENT_REQ,				// 배틀 중 무기 장착 요청
	PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK,				// 배틀 중 무기 장착 응답

	PROTOCOL_BATTLE_GM_RESUME_REQ,					// GM PAUSE 기능 해제 요청
	PROTOCOL_BATTLE_GM_RESUME_ACK,					//

	PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_ACK,			// ReadyBattle시 기존 게임중인 유저들의 인벤 정보를 보냄(배틀 최초 로딩을 위함)
	PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_OTHER_ACK,		// ReadyBattle시 레뒤중인 유저들에게 자신의 인벤 정보를 전송한다.(배틀 최초 로딩을 위함)

	// 신고하기 요청 - 스크린샷 기능
	PROTOCOL_BATTLE_REPORTSCREENSHOT_REQ,				// 신고하기 요청
	PROTOCOL_BATTLE_REPORTSCREENSHOT_ACK,				// 신고하기 응답

	PROTOCOL_BATTLE_COUNT_DOWN_ACK,	//폭파, 섬멸전 카운트 다운 패킷

	PROTOCOL_BATTLE_USER_SOPETYPE_REQ,			// 배틀 중 스코프 타입 변경 요청.
	PROTOCOL_BATTLE_USER_SOPETYPE_ACK			// 배틀 중 스코프 타입 변경 응답.
};

// 추후 제거.
enum Protocol_New
{
	PROTOCOL_NEW = PROTOCOL_VALUE_NEW,

	//////////////////////////////////////////////////////////////////////
	// 케릭터 세팅
	//PROTOCOL_NEW_CREATE_CHARA_REQ,				// 처음 캐릭 생성시 N개(현재2개)의 캐릭을 슬롯에 입력해주는 코드
	//PROTOCOL_NEW_CREATE_CHARA_ACK,

	PROTOCOL_NEW_SET_CHARA_REQ,					// (사용안함)
	PROTOCOL_NEW_SET_CHARA_ACK,					// PROTOCOL_AUTH_SHOP_GOODS_BUY_CHARA_ACK 성공시 클라에 전송

	PROTOCOL_DELETE_CHARA_REQ,					// 캐릭터 삭제(캐릭슬롯정보초기화)
	PROTOCOL_DELETE_CHARA_ACK,


	PROTOCOL_NEW_SET_CURRENT_CHARA_REQ,			// 대표 캐릭터 3슬롯 세팅(현재 필요없음)
	PROTOCOL_NEW_SET_CURRENT_CHARA_ACK,

	//////////////////////////////////////////////////////////////////////
	// 1.5 주특기 세팅
	//PROTOCOL_SET_SPECIALITY_CLASS_REQ,			// 주특기 클래스 세팅 요청.
	//PROTOCOL_SET_SPECIALITY_CLASS_ACK,			// 주특기 클래스 세팅 응답.
	PROTOCOL_SET_SPECIALITY_REQ,				// 주특기 세팅
	PROTOCOL_SET_SPECIALITY_ACK,

	PROTOCOL_INIT_SPECIALITY_REQ,				// 주특기 초기화
	PROTOCOL_INIT_SPECIALITY_ACK,

	//////////////////////////////////////////////////////////////////////////
	// 서버선택 & 채널선택
	PROTOCOL_NEW_GAME_SERVER_STATE_REQ,			// 서버 상태 요청
	PROTOCOL_NEW_GAME_SERVER_STATE_ACK,			// 서버 상태 응답

	//PROTOCOL_NEW_GET_CHANNELLIST_REQ,			// 선택한 서버의 채널정보 얻기
	//PROTOCOL_NEW_GET_CHANNELLIST_ACK,

	//PROTOCOL_NEW_SELECT_CHANNEL_REQ,			// 채널 선택 (같은서버이면 바로 로비로 입장, 아니면 PROTOCOL_BASE_USER_ENTER_REQ 전송)
	//PROTOCOL_NEW_SELECT_CHANNEL_ACK,			// 성공이면 클라에서 PROTOCOL_LOBBY_ENTER_REQ를 보냄

	//PROTOCOL_NEW_USER_LEAVE_REQ,				//Change Server *서버에서 *서버로 갈때.
	//PROTOCOL_NEW_USER_LEAVE_ACK,
	//PROTOCOL_NEW_USER_ENTER_REQ,				//Change Server *서버에서 *서버로 갈때.
	//PROTOCOL_NEW_USER_ENTER_ACK,

	//////////////////////////////////////////////////////////////////////////
	// 1.5 퀘스트
	PROTOCOL_GET_ALL_QUEST_INFO_REQ,			// 전체 퀘스트 정보 요청.
	PROTOCOL_GET_ALL_QUEST_INFO_ACK,			// 전체 퀘스트 정보 응답.

	PROTOCOL_GET_HONOR_QUEST_INFO_REQ,			// 명예 퀘스트 정보 요청.
	PROTOCOL_GET_HONOT_QUEST_INFO_ACK,			// 명예 퀘스트 정보 응답.

	PROTOCOL_QUEST_SETTING_REQ,					// 퀘스트 셋팅 요청.  사용 X. 추후 프로토콜 정리시 제거.
	PROTOCOL_QUEST_SETTING_ACK,					// 퀘스트 셋팅 응답.  사용 X. 추후 프로토콜 정리시 제거.

	PROTOCOL_HONOR_QUEST_CLEAR_ACK,				// 명예 퀘스트 완료응답.

	//////////////////////////////////////////////////////////////////////////

	// 1.5 주특기 세팅
	PROTOCOL_SET_SPECIALITY_CLASS_REQ,			// 주특기 클래스 세팅 요청.
	PROTOCOL_SET_SPECIALITY_CLASS_ACK,			// 주특기 클래스 세팅 응답.

	//////////////////////////////////////////////////////////////////////

	// 1.5 퀘스트
	PROTOCOL_DAILY_QUEST_CHANGE_ACK,			// 일일 퀘스트 변경시. ACK만 존재.
	PROTOCOL_CLASS_QUEST_CHANGE_ACK,			// 계급 퀘스트 변경시. ACK만 존재.

	PROTOCOL_PERIOD_QUEST_SETTING_REQ,			// 기간(일일 or 이벤트) 퀘스트 셋팅 요청.
	PROTOCOL_PERIOD_QUEST_SETTING_ACK,			// 기간(일일 or 이벤트) 퀘스트 셋팅 응답.

	PROTOCOL_CLASS_QUEST_SETTING_REQ,			// 계급 퀘스트 셋팅 요청,
	PROTOCOL_CLASS_QUEST_SETTING_ACK,			// 계급 퀘스트 셋팅 응답.

	//////////////////////////////////////////////////////////////////////////




	// 서버끼리만 주고받는 프로토콜
	//////////////////////////////////////////////////////////////////////////
	// 유저 정보
	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_NEW_SS_GET_USER_INFO_BASIC_ACK,		// 기본정보
	PROTOCOL_NEW_SS_GET_USER_INFO_INVEN_ACK,		// 인벤정보
	PROTOCOL_NEW_SS_GET_USER_INFO_MEDAL_ACK,		// 메달정보

	//////////////////////////////////////////////////////////////////////////
	// 서버 이동
	//////////////////////////////////////////////////////////////////////////
	PROTOCOL_NEW_SS_USER_ENTER_BASIC_ACK,
	PROTOCOL_NEW_SS_USER_ENTER_INVEN_ACK,
	PROTOCOL_NEW_SS_USER_ENTER_MEDAL_ACK,


	// Trans DB Save
	PROTOCOL_NEW_SS_TRANS_DB_SAVE_REQ,			// Game -> Trans

	// v1.5 주특기 정리된 버전.  2012.06.04 노준영
	PROTOCOL_BASE_NEW_SPECIALITY_SET_REQ,				// 주특기 세팅 요청.	구 : PROTOCOL_SET_SPECIALITY_REQ
	PROTOCOL_BASE_NEW_SPECIALITY_SET_ACK,				// 주특기 세팅 응답.	구 : PROTOCOL_SET_SPECIALITY_ACK

	PROTOCOL_BASE_NEW_SPECIALITY_INIT_REQ,				// 주특기 초기화 요청.	구 : PROTOCOL_INIT_SPECIALITY_REQ
	PROTOCOL_BASE_NEW_SPECIALITY_INIT_ACK,				// 주특기 초기화 응답.	구 : PROTOCOL_INIT_SPECIALITY_ACK

	PROTOCOL_BASE_NEW_SPECIALITY_SET_CLASS_REQ,			// 주특기 클래스 세팅 요청.	구 : PROTOCOL_SET_SPECIALITY_CLASS_REQ
	PROTOCOL_BASE_NEW_SPECIALITY_SET_CLASS_ACK,			// 주특기 클래스 세팅 응답.	구 : PROTOCOL_SET_SPECIALITY_CLASS_ACK
};

enum Protocol_ZLog
{
	PROTOCOL_ZLOG_FILEINFO_SEND = PROTOCOL_VALUE_VALUE_ZLOG,
	PROTOCOL_ZLOG_FILEINFO_RECV,
	PROTOCOL_ZLOG_FILE_SEND,
	PROTOCOL_ZLOG_FILE_RECV,
	PROTOCOL_ZLOG_FILE_FINISH
};

// v1.5 Skill 시스템
enum Protocol_Skill
{
	PROTOCOL_SKILL = PROTOCOL_VALUE_SKILL,
	PROTOCOL_SKILL_SET_REQ,					// 주특기 세팅
	PROTOCOL_SKILL_SET_ACK,
	PROTOCOL_SKILL_RESET_REQ,				// 주특기 초기화
	PROTOCOL_SKILL_RESET_ACK,
	PROTOCOL_SKILL_CLASS_SET_REQ,			// 캐릭터 주특기 병과 선택
	PROTOCOL_SKILL_CLASS_SET_ACK
};

// v1.5 Medal 시스템. 2012.10.25 노준영

// v1.5 Medal. Client <-> Server(Game)
enum Protocol_Medal
{
	PROTOCOL_MEDAL = PROTOCOL_VALUE_MEDAL,

	// 사용 안함. PROTOCOL_BASE_GET_USER_INFO_ACK 에서 받습니다.
	//PROTOCOL_GET_MEDAL_BASIC_INFO_REQ,			// 메달 기본 정보 요청. Client -> Game.
	//PROTOCOL_GET_MEDAL_BASIC_INFO_ACK,			// 메달 기본 정보 응답 헤더. Game -> Client. EVENT_ERROR_PBV15_MEDAL_DB_LOADING일시 로딩중 표시

	// 로그인후 첫 메달 페이지 입장시 클라이언트가 던집니다. 딱 1번만 던집니다.
	PROTOCOL_GET_MEDAL_INFO_REQ,				//  메달 정보 요청. Client -> Game.
	PROTOCOL_GET_MEDAL_INFO_ACK,				//  메달 정보 응답 헤더. Game -> Client.  PROTOCOL_AUTH_SHOP_ITEMLIST_ACK 파싱 참고. 

	// 로그인후 첫 메달 페이지 입장시 클라이언트가 던집니다. 딱 1번만 던집니다.
	PROTOCOL_GET_CUR_MEDAL_SET_INFO_REQ,		// 진행중인 메달 셋트 정보 요청. Client -> Game.
	PROTOCOL_GET_CUR_MEDAL_SET_INFO_ACK,		// 진행중인 메달 셋트 정보 응답. Game -> Client. PROTOCOL_AUTH_SHOP_ITEMLIST_ACK 파싱 참고. 

	// 로그인후 첫 메달 페이지 입장시 클라이언트가 던집니다. 딱 1번만 던집니다.
	PROTOCOL_GET_COM_MEDAL_SET_INFO_REQ,		// 완료된 메달 셋트 정보 요청. Client -> Game.
	PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK,		// 완료된 메달 셋트 정보 응답. Game -> Client. PROTOCOL_AUTH_SHOP_ITEMLIST_ACK 파싱 참고. 


	PROTOCOL_MISSION_COMPLETE_ACK,				// 메달 미션 완료(미션카운트증가) ACK. Game -> Client. v1.0 : PROTOCOL_BASE_QUEST_CHANGE_ACK

	PROTOCOL_GET_MEDAL_REWARD_REQ,				// 메달 완료(1개 레벨)에 대한 보상 요청. Client -> Game.
	PROTOCOL_GET_MEDAL_REWARD_ACK,				// 메달 완료(1개 레벨)에 대한 보상 응답. Game -> Client. 추가로 .메달 갱신 여부 및 갱신된 메달 정보.

	PROTOCOL_SET_NOTIFY_MEDAL_REQ,				// 알리미 메달 설정 요청. Client -> Game. ( 메달 페이지 나갈때만 던집니다. )
	PROTOCOL_SET_NOTIFY_MEDAL_ACK,				// 알리미 메달 설정 응답. Game -> Client. 

	PROTOCOL_SET_REFRESH_MEDAL_SET_ACK,			// 메달 셋트 갱신 여부. Game -> Client. 로그인시, 룸/클랜/상점/뽑기상점 입장시.

};

enum Protocol_Gacha
{
	PROTOCOL_GACHA = PROTOCOL_VALUE_GACHA,

	PROTOCOL_GACHA_ITEM_INFO_REQ,
	PROTOCOL_GACHA_ITEM_INFO_ACK,

	PROTOCOL_GACHA_NOTICE_ACK,
	PROTOCOL_GACHA_WINNING_USER_ACK,

	PROTOCOL_GACHA_SHOP_STATE_REQ,
	PROTOCOL_GACHA_SHOP_STATE_ACK,

	PROTOCOL_GACHA_ENTER_REQ,
	PROTOCOL_GACHA_ENTER_ACK,

	PROTOCOL_GACHA_LEAVE_REQ,
	PROTOCOL_GACHA_LEAVE_ACK,

	PROTOCOL_GACHA_PURCHASE_REQ,
	PROTOCOL_GACHA_PURCHASE_ACK,

	PROTOCOL_GACHA_GET_PURCHASE_COUNT_REQ,
	PROTOCOL_GACHA_GET_PURCHASE_COUNT_ACK,

	//////////////////////////////////////////////////////////////////////////

	PROTOCOL_SS_GACHA = PROTOCOL_GACHA + 0x0050,

	PROTOCOL_SS_GACHA_ENTER_REQ,
	PROTOCOL_SS_GACHA_ENTER_ACK,

	PROTOCOL_SS_GACHA_ITEM_INFO_REQ,
	PROTOCOL_SS_GACHA_ITEM_INFO_ACK,

	PROTOCOL_SS_GACHA_NOTICE_REQ,
	PROTOCOL_SS_GACHA_NOTICE_ACK,

	PROTOCOL_SS_GACHA_WINNING_USER_ACK,

	PROTOCOL_SS_GACHA_PURCHASE_REQ,
	PROTOCOL_SS_GACHA_PURCHASE_ACK,

	PROTOCOL_SS_GACHA_PROVIDE_REQ,
	PROTOCOL_SS_GACHA_PROVIDE_ACK,


};

// 빠른 입장
enum Protocol_QuickJoin
{
	/////////////////////////////////////////////////////////////////////////////////////
	////////////////	GameServer <-> Client		/////////////////////////////////////
	PROTOCOL_QUICKJOIN = PROTOCOL_VALUE_QUICKJOIN,

	PROTOCOL_QUICKJOIN_START_REQ,				// 빠른 입장 시작 요청.
	PROTOCOL_QUICKJOIN_START_ACK,				// 빠른 입장 시작 응답. 방입장 성공시 PROTOCOL_LOBBY_NEW_JOIN_ROOM_ACK을 먼저 받습니다.

	PROTOCOL_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ,		// 차선책 방 입장 요청.
	PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK,		// 차선책 방 입장 응답. 방입장 성공시 PROTOCOL_LOBBY_NEW_JOIN_ROOM_ACK을 먼저 받습니다.
};

// 캐릭터
enum Protocol_Char
{
	PROTOCOL_CHAR = PROTOCOL_VALUE_CHAR,

	PROTOCOL_CHAR_CREATE_CHARA_REQ,					// 케릭터 생성
	PROTOCOL_CHAR_CREATE_CHARA_ACK,

	PROTOCOL_CHAR_CHANGE_CHARA_NICK_REQ,			// 캐릭터 닉네임변경요청(여러 캐릭을 한꺼번에 변경시)(게임서버에서만처리,종료시저장)
	PROTOCOL_CHAR_CHANGE_CHARA_NICK_ACK,

	PROTOCOL_CHAR_CHANGE_EQUIP_REQ,					// 캐릭터 세팅(장착정보 변경 슬롯 전부다 전송), 빠른장비교체,상점에서의장비장착시사용(인벤에서의 장착시는 사용안함)
	PROTOCOL_CHAR_CHANGE_EQUIP_ACK,

	PROTOCOL_CHAR_DELETE_CHARA_REQ,					// 캐릭터 삭제(캐릭슬롯정보초기화
	PROTOCOL_CHAR_DELETE_CHARA_ACK,

	PROTOCOL_CHAR_CHANGE_STATE_ACK,					// 캐릭터 사용 시작.
};

//GM기능강화( 1:1채팅, 인게임 제재 )
enum Protocol_GMCHAT
{
	PROTOCOL_GMCHAT = PROTOCOL_VALUE_GMCHAT,

	PROTOCOL_GMCHAT_START_CHAT_REQ,				//1:1대화시작 요청
	PROTOCOL_GMCHAT_START_CHAT_ACK,

	PROTOCOL_GMCHAT_SEND_CHAT_REQ,				//1:1대화내용 전송
	PROTOCOL_GMCHAT_SEND_CHAT_ACK,

	PROTOCOL_GMCHAT_END_CHAT_REQ,				//1:1대화종료 요청
	PROTOCOL_GMCHAT_END_CHAT_ACK,

	PROTOCOL_GMCHAT_APPLY_PENALTY_REQ,			//유저 제재조치 요청
	PROTOCOL_GMCHAT_APPLY_PENALTY_ACK,

	PROTOCOL_GMCHAT_NOTI_USER_PENALTY_REQ,		//사용안함
	PROTOCOL_GMCHAT_NOTI_USER_PENALTY_ACK,		//제재결과 유저통보
};

// 치트키.
enum Protocol_Cheat
{
	PROTOCOL_CHEAT = PROTOCOL_VALUE_CHEAT,

	PROTOCOL_CHEAT_MEDAL_COMMAND_REQ,
	PROTOCOL_CHEAT_MEDAL_COMMAND_ACK,

	PROTOCOL_CHEAT_INCREASE_KILL_COUNT_REQ,		// 캐릭터 Kill 카운트 증가
	PROTOCOL_CHEAT_INCREASE_KILL_COUNT_ACK,

	PROTOCOL_CHEAT_PLAY_SOLO_REQ,				// 1인으로 인게임 시작 & 진행
	PROTOCOL_CHEAT_PLAY_SOLO_ACK,

	PROTOCOL_CHEAT_REDUCE_TS_EVENT_REQ,			// TS이벤트 시간 단축
	PROTOCOL_CHEAT_REDUCE_TS_EVENT_ACK,

	PROTOCOL_CHEAT_CHARACTER_TELEPORT_REQ,		// 캐릭터 이동
	PROTOCOL_CHEAT_CHARACTER_TELEPORT_ACK,

	PROTOCOL_CHEAT_DAMAGE_GAME_OBJECT_REQ,		// 게임오브젝트에 데미지주기
	PROTOCOL_CHEAT_DAMAGE_GAME_OBJECT_ACK,

	PROTOCOL_CHEAT_REDUCE_ROUND_TIME_REQ,		// 라운드 남은 시간 단축
	PROTOCOL_CHEAT_REDUCE_ROUND_TIME_ACK,

	PROTOCOL_CHEAT_DETECT_HACK_OFF_REQ,			// 핵탐지 OFF
	PROTOCOL_CHEAT_DETECT_HACK_OFF_ACK,

	PROTOCOL_CHEAT_TIMER_GM_PAUSE_REQ,			// 일정시간이후에 GM Pause 걸기
	PROTOCOL_CHEAT_TIMER_GM_PAUSE_ACK,

	PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_REQ,	// 클랜 매치 메이킹 - 매치팀 매칭포인트 조회 요청.
	PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK,	// 클랜 매치 메이킹 - 매치팀 매칭포인트 조회 응답.

	PROTOCOL_CHEAT_CLAN_WAR_RESULT_REQ,			// 클랜 매치 메이킹 - 매치 결과 ( 클랜 포인트, 표준편차 ) 요청.
	PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK,			// 클랜 매치 메이킹 - 매치 결과 ( 클랜 포인트, 표준편차 ) 요청.

	PROTOCOL_CHEAT_ITEM_AUTH_REQ,			// 치트키 커맨드를 이용한 아이템 대량 사용 인증 요청.
	PROTOCOL_CHEAT_ITEM_AUTH_ACK,			// 치트키 커맨드를 이용한 아이템 대량 사용 인증 응답.
};

//Knock Server
enum Protocol_Knock
{
	PROTOCOL_KNOCK = PROTOCOL_VALUE_KNOCK,
	PROTOCOL_KNOCK_CONNECTIONSUCCESS,
	PROTOCOL_KNOCK_CONFIRMATION,
	PROTOCOL_KNOCK_SEND_WARNING_MESSAGE,
	PROTOCOL_KNOCK_NORMALITY_SHUTDOWN,
};

#endif	// PROTOCOLDEF_H
