#ifndef __COMMONDEF_SYSTEM_H__
#define __COMMONDEF_SYSTEM_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define
#define ENTRANCE_NAME_SIZE				32					// Entrance 이름 크기
#define ENTRANCE_CHANNEL_SET_COUNT		4					// Channel 입장조건 개수

#define ENTRANCE_GROUP_COUNT			20					// Group Type Count
#define ENTRANCE_SERVER_COUNT			20					// Server Type Count
#define ENTRANCE_CHANNEL_COUNT			20					// Channel Type Count

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum
/*	서버 추가
	1.	SChannelType 요기에 추가한다.
	2.	LPCTSTR TabList[S_CHANNEL_TYPE_MAX] 에 ConfigEx.pef 파일에서 읽을 스트링을 적는다.
	3.	GetServerTabTypeText( SChannelType TabType ), GetServerTypeText( SChannelType ServerType )
		Excel 스트링을 등록한다.
*/
enum SChannelType
{
	S_CHANNEL_TYPE_UNDEFINED	= -1,

	S_CHANNEL_TYPE_ALL		= 0,	//전체
	S_CHANNEL_TYPE_NORMAL	= 1,	//일반 
	S_CHANNEL_TYPE_NORMAL2,			//일반 2
	S_CHANNEL_TYPE_BEGIN,			//초보 
	S_CHANNEL_TYPE_CLAN,			//클랜
	S_CHANNEL_TYPE_EXPERT,			//고수 
	S_CHANNEL_TYPE_CONBINATION,		//조합	
	S_CHANNEL_TYPE_CHAMPIONSHIP,	//대회
	S_CHANNEL_TYPE_NATIONAL,		//중남미 국가
	S_CHANNEL_TYPE_PCCAFE_SILVER,	//인니 피씨방 일반
	S_CHANNEL_TYPE_PCCAFE_GOLD,		//인니 피씨방 골드 
	S_CHANNEL_TYPE_PCCAFE_CLAN,		//인니 피씨방 클랜 서버
	S_CHANNEL_TYPE_AZERBAIJAN,
	
	S_CHANNEL_TYPE_MAX,
};


/*	채널 추가
	1.	ChannelType 요기에 추가한다.
	2.	SERVERSELECT_MAINTYPE 요기에 추가한다.
	3.	SCript/String/korea/ServerSelect.txt 파일에 채널 스트링을 등록한다.
	4.2번에서 등록 된 내용을 _LoadServerTextFromFile() 읽을 수 있도록 수정
	5. GetCurrentSeverInfo	:	로비방 채널 제목
		_SetServerEnv	:	채널 안 슬롯
		GetCategoryText	:	채널 제목
*/

//#define USE_NEW_CHANNEL_SYS

enum ChannelType
{
#ifdef USE_NEW_CHANNEL_SYS
	CHANNEL_TYPE_NONE = 0,
	CHANNEL_TYPE_NORMAL,			// 일반 채널
	CHANNEL_TYPE_RANK,				// 랭킹 제한 ( Min, Max )
	CHANNEL_TYPE_KILL_RATE,			// 킬 Rate ( Min, Max )
	CHANNEL_TYPE_CLAN,				// 클랜
	CHANNEL_TYPE_NATION,			// 국가만 입장 가능 ( Min )
	CHANNEL_TYPE_NATION_NOT,		// 국가만 입장 불가능 ( Min )
	CHANNEL_TYPE_PCCAFE,			// PC 방 ( Min, Max )
	CHANNEL_TYPE_BATTLE_TIME,		// 배틀 시간 ( Min, Max )
	CHANNEL_TYPE_CHAMPIONSHIP,		// 대회 채널
#else
	CHANNEL_TYPE_NONE = 0,
	CHANNEL_TYPE_NORMAL,			//일반 채널
	CHANNEL_TYPE_BEGIN1,			//초보 채널 Kill/Death로 제한
	CHANNEL_TYPE_BEGIN2,			//초보 채널 계급으로 제한
	CHANNEL_TYPE_CLAN,			//클랜 채널
	CHANNEL_TYPE_EXPERT,			//고수 채널 병장 이상 
	CHANNEL_TYPE_EXPERT2,			//고수 채널 상사 이상
	CHANNEL_TYPE_CHAMPIONSHIP,	//대회 채널
	CHANNEL_TYPE_NORMAL2,			//일반 채널 2
	CHANNEL_TYPE_NATIONAL,		//중남미 국가 채널
	CHANNEL_TYPE_MIDDLE,			//중간 채널
	CHANNEL_TYPE_PCCAFE_SILVER,	//인니 피씨방 일반
	CHANNEL_TYPE_PCCAFE_GOLD,		//인니 피씨방 골드 
	CHANNEL_TYPE_PCCAFE_CLAN,		//인니 피씨방 클랜 채널
	CHANNEL_TYPE_AZERBAIJAN,		//아제르바이잔 전용 채널
#endif	

	CHANNEL_TYPE_MAX,
};

/*	GM 제재 종류
	1. GM이 유저를 GMChat 창에서 제재하는 종류를 추가한다.
*/
enum GMChatPenaltyType
{
	GMCHAT_PENALTY_CHAT = 1,
	GMCHAT_PENALTY_BLOCK
};

enum GMChatIndexType
{
	GMCHAT_INDEX_LOBBY = 1,
	GMCHAT_INDEX_ROOMSLOT,
	GMCHAT_INDEX_ROOM
	
};

enum GMBlockType
{
	GMCHAT_BLOCK_TYPE_BLOCKON = 0,
	GMCHAT_BLOCK_TYPE_BLOCKOFF
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)

//방 정보 
struct LOBBY_USER_INFO_LIST
{
	INT32	_Session = 0;							// DB ID
	UINT32	_ClanMark = CLAN_MARK_DEFAULT;							// Clan Mark
	TTCHAR	_ClanName[NET_CLAN_NAME_SIZE] = { 0 };	// 클랜 이름
	UINT8	_Rank = 0;								// 랭킹
	UINT8	_eSportID = 0;							// E-Sport ID
	TTCHAR	_Nick[NET_NICK_NAME_SIZE] = { 0 };		// 닉네임
	UINT8	_NickColor = 0;
	UINT8	_ui8NationalCode = 0;


	void	Reset() {
		//_Idx		= 0;
		_Session	= 0;
		_ClanMark	= CLAN_MARK_DEFAULT;
		_ClanName[0] = '\0';
		_Rank		= 0;
		_eSportID	= 0;
		_Nick[ 0 ]	= '\0';
		_NickColor	= 0;
		_ui8NationalCode = 0;
	}
};

struct COMMON_SERVER_INFO
{
	UINT8	_Type = 0;
	UINT32	_IP = 0;
};

struct GAME_SERVER_INFO
{
	INT32	_IsActive = 0;
	UINT32	_OutIP = 0;
	UINT16	_OutPort = 0;
	UINT8	_AllType = 0;
	UINT16	_ui16MaxUserCount = 0;
	INT32	_Arg0 = 0;
	UINT8	m_aui8ChannelType[GAME_CHANNEL_COUNT] = { 0 };
};

struct GAME_SERVER_INFO_ALL
{
	GAME_SERVER_INFO		m_GameServerInfo;
	
	UINT8					m_pChannelType[GAME_CHANNEL_COUNT] = { 0 };
	UINT8					m_ui8HaveChannel[CHANNEL_TYPE_MAX] = { 0 };
	INT32					m_i32UserCount[GAME_CHANNEL_COUNT] = { 0 };
};

struct ENTRANCE_GROUP_TYPE
{
	UINT8					m_ui8Type = 0;
	wchar_t					m_wstrName[ENTRANCE_NAME_SIZE] = { 0 };
};

struct ENTRANCE_SERVER_TYPE
{
	UINT8					m_ui8Type = 0;
	UINT8					m_ui8Color = 0;
	wchar_t					m_wstrName[ENTRANCE_NAME_SIZE] = { 0 };
};

struct ENTRANCE_CHANNEL_TYPE
{
	UINT8					m_ui8Type = 0;
	UINT8					m_ui8Color = 0;
	UINT8					m_ui8Min = 0;
	UINT8					m_ui8Max = 0;
	wchar_t					m_wstrName[ENTRANCE_NAME_SIZE] = { 0 };
};

struct ENTRANCE_SERVER_LIST
{
	UINT8					m_ui8GroupIdx = 0;
	UINT8					m_ui8ServerIdx = 0;
	UINT8					m_ui8SetCount = 0;
	UINT8					m_ui8ChannelIdx[GAME_CHANNEL_COUNT][ENTRANCE_CHANNEL_SET_COUNT] = { 0 };
};

struct ENTRANCE_LIST
{
	UINT8					m_ui8GroupTypeCount = 0;
	ENTRANCE_GROUP_TYPE		m_GroupType[ ENTRANCE_GROUP_COUNT ];

	UINT8					m_ui8ServerTypeCount = 0;
	ENTRANCE_SERVER_TYPE	m_ServerType[ ENTRANCE_SERVER_COUNT ];

	UINT8					m_ui8ChannelTypeCount = 0;
	ENTRANCE_CHANNEL_TYPE	m_ChannelType[ ENTRANCE_CHANNEL_COUNT ];

	UINT8					m_ui8ServerCount = 0;
	ENTRANCE_SERVER_LIST	m_ServerList[ MAX_GAME_SERVER_COUNT ];
};

#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif