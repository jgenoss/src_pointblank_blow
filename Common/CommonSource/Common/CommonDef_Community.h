#ifndef __COMMONDEF_COMMUNITY_H__
#define __COMMONDEF_COMMUNITY_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define
#define FRIEND_MAX_COUNT					50			//친구 생성 맥스 카운트 
#define BLOCK_MAX_COUNT						50			// 차단 생성 맥스 카운트
#define BLOCK_COMMENT_MAX					101			// 차단 사유 최대 값 ( 영문 기준 )

// 쪽지 설정
#define MAX_NORMAL_NOTE_COUNT			100						// 최대 일반 쪽지 - 100개를 기준으로 1개당 1%씩 취급한다.
#define MAX_NOTE_LENGTH					240						// 한글 120자
#define MAX_NOTE_COUNT_PER_PAGE			25						// 전송 페이지당 최대 쪽지 갯수
#define NET_NOTE_SIZE					(MAX_NOTE_LENGTH + 1)	// 널문자 포함
#define MAX_SEND_NOTE_LIMIT				100						// 하루 전송 제한
#define REMAIN_DAY_FOR_READED_NOTE		7						// 읽은 편지 삭제 기간 7일
#define REMAIN_DAY_FOR_UNREAD_NOTE		15						// 안읽은 편지 삭제 기간 15일

//#define CLEAR_FRIEND_MATCH(state)						(state & 0xFFFFFF00FFFFFFFF)

// 커뮤니티
#define MAKE_FRIEND_STATE(state,server,channel,room)				((((state) & 0x0000000F) << 28) | (((server) & 0x000000FF) << 20) | (((channel) & 0x000000FF) << 12) | ((room) & 0x00000FFF))
#define MAKE_FRIEND_MATCH_STATE(match,state,server,channel,room)	(((UINT64)(match) & 0x000000000000FFFF) << 32) | ((((state) & 0x000000000F) << 28) | (((server) & 0x00000000FF) << 20) | (((channel) & 0x00000000FF) << 12) | ((room) & 0x0000000FFF))

#define GET_FRIEND_MATCH(state)							(((state) >> 32) & 0x000000000000FFFF)
#define GET_FRIEND_STATE(state)							(((state) >> 28) & 0x0000000F)
#define GET_FRIEND_SERVER(state)						(((state) >> 20) & 0x000000FF)
#define GET_FRIEND_CHANNEL(state)						(((state) >> 12) & 0x000000FF)
#define GET_FRIEND_ROOMIDX(state)						((state) & 0x00000FFF)

#define SET_FRIEND_MATCH(state, match)					( (state) | (((UINT64)(match) & 0x000000000000FFFF ) << 32))
#define CLEAR_FRIEND_MATCH(state)						(state & 0xFFFF0000FFFFFFFF)

 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum
enum	FIND_USER_TYPE
{
	FIND_UID					= 0,
	FIND_ROOM_USER_SLOT_IDX,			// Room User Slot IDX
	FIND_USER_SESSEION_IDX,				// User SessionIDX
	FIND_NICK,
};

// 쪽지 종류
enum NoteType
{
	NT_NORMAL = 0,		// 일반 쪽지
//	NT_NORMAL_ASK,		// 답장
	NT_NOTICE,			// 알림 쪽지
	NT_GIFT,			// 선물 쪽지
	NT_WEB,				// 웹구매 쪽지
	NT_CLAN,			// 클랜 쪽지
	NT_CLAN_ASK			// 클랜 권유 쪽지
};

// 쪽지 상태
enum NoteState
{
	NS_READED = 0,		// 읽은 쪽지
	NS_UNREAD,			// 안읽은 쪽지
	NS_NEW,				// 새 쪽지 - 로그인 후 전달된 쪽지
};

enum FRIEND_STATE
{
	FRIEND_STATE_REQUEST = 1,
	FRIEND_STATE_HOLD,
	FRIEND_STATE_OFFLINE,		//이하단은 친구 
	FRIEND_STATE_ONLINE,
	FRIEND_STATE_LOBBY,
	FRIEND_STATE_ROOM,	
	FRIEND_STATE_CANT_INVITE,	// 초대불가능

	FRIEND_STATE_MAX
};

enum FRIEND_CHANGE_STATE
{
	FRIEND_CHANGE_STATE_INSERT = 1,
	FRIEND_CHANGE_STATE_ACCEPT, 
	FRIEND_CHANGE_STATE_DELETE, 
	FRIEND_CHANGE_STATE_CHANGE,
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
inline	UINT32	FRIEND_STATE_64_TO_32(UINT64 State)
{
	return static_cast<UINT32>(MAKE_FRIEND_STATE(GET_FRIEND_STATE(State),GET_FRIEND_SERVER(State),
		GET_FRIEND_CHANNEL(State),GET_FRIEND_ROOMIDX(State)));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)

struct	NEW_FIND_USER_INFO
{	// v1.0, v1.1 UI 전용
	UINT8							m_ui8Rank = 0;			// 랭크
	INT32							m_i32State = 0;			// 유저 상태. MAKE_FRIEND_STATE 이용해서 정보 추출.
	TTCHAR							m_strClanName[NET_CLAN_NAME_SIZE] = { 0 };	// 클랜 명
	
	USER_INFO_RECORD				m_Record;	// 전적정보
	USER_INFO_EQUIPMENT_WEAPON		m_Weapon;	// 무기정보
	USER_INFO_EQUIPMENT_CHARA		m_Chara;	// 캐릭터정보

	T_ItemID						m_CharaItemID[TEAM_COUNT] = { 0 };
	// vv3 - job 
	// 공용파츠 장비 정보 필요??
};

struct FRIEND_INFO
{
	T_UID	_fuid = 0;		// 친구 uid
	UINT32	_state = 0;		// 상태 0xF 00 F F FFF - bit 28-31:loginState, 20-27:reserved, 16-19:serverIdx, 12-15:channelIdx, 0-11:roomIdx
	UINT8	_rank = 0;		// 계급 (0,51]
	UINT8	_nickcolor = 0;
};

struct BLOCK_INFO
{
	INT32	_Idx = 0;		// DB Index : 차단 유저 삭제 요청시에 사용
	UINT8	_rank = 0;		// 계급 (0,51]
};

struct NOTE_HEADER
{
	INT32 _id = 0;								// 고유번호
	T_UID _suid = 0;							// Sender Idx
	UINT8 _type = 0;							// NoteType
	UINT8 _state = 0;							// NoteState
	UINT8 _remainDay = 0;						// Day
	INT32 _appendingData = 0;					// 첨부 정보
};

// 쪽지 몸체 구조체
struct NOTE_BODY
{
	UINT8 _senderSize = 0;						// 보낸사람 문자열의 널포함 길이
	UINT8 _messageSize = 0;						// 내용 문자열의 널포함 길이
	TTCHAR _sender[NET_NICK_NAME_SIZE] = { 0 };		// 보낸사람	
	TTCHAR _message[NET_NOTE_SIZE] = { 0 };			// 내용
	void * _user = nullptr;							// 유저 데이터 (현재 Gift에서 사용함)
};

// 쪽지 몸체 구조체(UID 이용시)
struct NOTE_BODY_BY_UID
{
	T_UID	_receiverUid = 0;					// 받는 사람의 UID
	UINT8	_messageSize = 0;					// 내용 문자열의 널포함 길이
	TTCHAR	_sender[NET_NICK_NAME_SIZE] = { 0 };	// 보낸사람					
	TTCHAR	_message[NET_NOTE_SIZE] = { 0 };		// 내용
	void	*_user = nullptr;							// 유저 데이터 (현재 Gift에서 사용함)
};

// 쪽지 구조체
struct NOTE_NODE
{
	NOTE_HEADER _head;						// 쪽지 머리
	NOTE_BODY _body;						// 쪽지 몸체
};

#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif