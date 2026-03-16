#ifndef __COMMONDEF_BASE_H__
#define __COMMONDEF_BASE_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define
#define	DEF_RANK_COUNT			52	// RANK_51
#define	DATASIZE_NO_CHECK		16	// H/W 인증 값 배열 크기
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum
// 권한
enum AUTH_LEVEL
{
	AUTH_LEVEL_NONE = 0,	// 일반 유저

	AUTH_LEVEL_MOD = 50,	// MOD

	AUTH_LEVEL_GM = 100,	// GM
};

// 특수기능
enum SPECIAL_ABILITY
{
	SA_NONE = 0,
	SA_GAME_GUARD,	// Game Guard Check Pass.
	SA_CHEAT,		// 치트키 사용 가능

	SA_MAX,
};
enum RANK_COUNT
{					//	PB 1.0 계급			PB 1.5 계급
	RANK_00 = 0,	//	"훈련병"			"훈련병"
	RANK_01,		//	"이병"				"이병 1호봉"
	RANK_02,		//	"일병"				"이병 2호봉"
	RANK_03,		//	"상병"				"이병 3호봉"
	RANK_04,		//	"병장"				"일병 1호봉"
	RANK_05,		//	"하사 1호봉"		"일병 2호봉"
	RANK_06,		//	"하사 2호봉"		"일병 3호봉"
	RANK_07,		//	"하사 3호봉"		"상병 1호봉"
	RANK_08,		//	"중사 1호봉"		"상병 2호봉"
	RANK_09,		//	"중사 2호봉"		"상병 3호봉"
	RANK_10,		//	"중사 3호봉"		"병장 1호봉"
	RANK_11,		//	"중사 4호봉"		"병장 2호봉"
	RANK_12,		//	"상사 1호봉"		"병장 3호봉"
	RANK_13,		//	"상사 2호봉"		"하사 1호봉"
	RANK_14,		//	"상사 3호봉"		"하사 2호봉"
	RANK_15,		//	"상사 4호봉"		"하사 3호봉"
	RANK_16,		//	"상사 5호봉"		"중사 1호봉"
	RANK_17,		//	"소위 1호봉"		"중사 2호봉"
	RANK_18,		//	"소위 2호봉"		"중사 3호봉"
	RANK_19,		//	"소위 3호봉"		"상사 1호봉"
	RANK_20,		//	"소위 4호봉"		"상사 2호봉"
	RANK_21,		//	"중위 1호봉"		"상사 3호봉"
	RANK_22,		//	"중위 2호봉"		"소위 1호봉"
	RANK_23,		//	"중위 3호봉"		"소위 2호봉"
	RANK_24,		//	"중위 4호봉"		"소위 3호봉"
	RANK_25,		//	"중위 5호봉"		"소위 4호봉"
	RANK_26,		//	"대위 1호봉"		"중위 1호봉"
	RANK_27,		//	"대위 2호봉"		"중위 2호봉"
	RANK_28,		//	"대위 3호봉"		"중위 3호봉"
	RANK_29,		//	"대위 4호봉"		"중위 4호봉"
	RANK_30,		//	"대위 5호봉"		"대위 1호봉"
	RANK_31,		//	"소령 1호봉"		"대위 2호봉"
	RANK_32,		//	"소령 2호봉"		"대위 3호봉"
	RANK_33,		//	"소령 3호봉"		"대위 4호봉"
	RANK_34,		//	"소령 4호봉"		"소령 1호봉"
	RANK_35,		//	"소령 5호봉"		"소령 2호봉"
	RANK_36,		//	"중령 1호봉"		"소령 3호봉"
	RANK_37,		//	"중령 2호봉"		"소령 4호봉"
	RANK_38,		//	"중령 3호봉"		"중령 1호봉"
	RANK_39,		//	"중령 4호봉"		"중령 2호봉"
	RANK_40,		//	"중령 5호봉"		"중령 3호봉"
	RANK_41,		//	"대령 1호봉"		"중령 4호봉"
	RANK_42,		//	"대령 2호봉"		"대령 1호봉"
	RANK_43,		//	"대령 3호봉"		"대령 2호봉"
	RANK_44,		//	"대령 4호봉"		"대령 3호봉"
	RANK_45,		//	"대령 5호봉"		"대령 4호봉"
	RANK_46,		//	"준장"				"준장"
	RANK_47,		//	"소장"				"소장"
	RANK_48,		//	"중장"				"중장"
	RANK_49,		//	"대장"				"대장"
	RANK_50,		//	"원수"				"원수"
	RANK_51,		//	"영웅"				"영웅"
	RANK_NEWBIE,	// 클라만 사용합니다.
	RANK_GAMEMASTER,	// GM
	RANK_MOD,		// MOD
	RANK_NONE,		// 계급 초기화값(위장계급 변수에서만 사용)
	RANK_56,		// 스타플레이어(돌격소총)
	RANK_57,		// 스타플레이어(기관단총)
	RANK_58,		// 스타플레이어(기관총)
	RANK_59,		// 스타플레이어(샷건)
	RANK_60,		// 스타플레이어(저격총)
	RANK_MAX,
};

#define PBRANK_RANGE_DEF(name, minRank, maxRank)  PBRANK_RANGE_##name = minRank | ((UINT8)maxRank << 8 )

// 랭크 구간 확인용 enum
enum PBRANK_RANGE
{
	PBRANK_RANGE_DEF(COLONEL, RANK_42, RANK_45),	// 대령구간
	PBRANK_RANGE_DEF(LIEUTENAT_COLONEL, RANK_38, RANK_41),	// 중령구간
	PBRANK_RANGE_DEF(MAJOR, RANK_34, RANK_37),	// 소령구간
};

// 랭크 구간 확인함수 - 1:구간초과, 0:구간안에있음, -1:구간미만
inline INT32 PBRankRangeCheck(PBRANK_RANGE range, INT32 inRank)
{
	UINT8 minVal = range & 0xff;
	UINT8 maxVal = ((range >> 8) & 0xff);

	if (inRank < minVal)
		return -1;

	if (inRank > maxVal)
		return 1;

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
extern const UINT32	g_ExpValueV1[DEF_RANK_COUNT];
extern const UINT32	g_ExpValueV2[DEF_RANK_COUNT];
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)

struct USER_INFO_BASIC_BASE
{
	TTCHAR					m_strNick[NET_NICK_NAME_SIZE] = { 0 };			// 닉네임	
	UINT32					m_ui32Rank = 0;								// 랭킹
	UINT32					m_ui32KeepRank = 0;							// 한번 올렸던 랭킹
	UINT32					m_ui32Point = 0;								// Point
	UINT32					m_ui32Exp = 0;								// 경험치	
	UINT32					m_ui32AbusingCount = 0;						// 어뷰징 횟수
	UINT8					m_ui8TutorialIng = 0;						// 튜토리얼 완료 여부
	UINT64					m_ui64GuideComplete = 0;					// 가이드 완료 여부. 각 이벤트 별로 1비트씩 사용.
	UINT8					m_ui8AuthLevel = 0;							// AUTH_LEVEL. 하나의 값만 가집니다.
	UINT8					m_ui8SpecialAbility = 0;					// SPECIAL_ABILITY. 플래그 형태로 다수의 값을 가질수 있습니다.
	UINT32					m_ui32Coin = 0;
	UINT32					m_ui32Cash = 0;

};

struct USER_INFO_BASIC : public USER_INFO_BASIC_BASE
{
	// Cash
	UINT32					m_ui32Cash = 0;

	// Clan Index
	UINT32					m_ui32Clanidx = 0;

	// ??
	UINT32					m_ui32ClanState = 0;

	UINT32					m_ui32NCCoin = 0;			//Happy Coin y Ensi Coin para la construcción coreana 
	UINT32					m_ui32HappyCoin = 0;			//
	UINT8					m_ui8PCCafe = PC_NOT_CAFE;
	UINT8					m_ui8ESportID = ESPORT_NOT_ID;
	// clan
	TTCHAR					m_strClanName[NET_CLAN_NAME_SIZE] = { 0 };
	UINT8					m_ui8ClanExpRank = 0;
	UINT8					m_ui8ClanPerRank = 0;
	UINT32					m_ui32ClanMark = CLAN_MARK_DEFAULT;
	UINT8					m_ui8ClanNamecolor = 0;

	void	Reset()
	{
		m_strNick[0] = '\0';
		m_strClanName[0] = '\0';
		m_ui8PCCafe = PC_NOT_CAFE;
		m_ui8ESportID = ESPORT_NOT_ID;
		m_ui32ClanMark = CLAN_MARK_DEFAULT;			// 클랜 마크 초기화는 0xFFFFFFF이기 때문에 세팅해줍니다.
		m_ui32Exp = m_ui32Rank = m_ui32Point = m_ui32Cash = m_ui32Clanidx = m_ui32NCCoin = m_ui32HappyCoin = m_ui32ClanState = 0;
		m_ui8ClanExpRank = m_ui8ClanPerRank = m_ui8ClanNamecolor = 0;
		m_ui32KeepRank = 0;
		m_ui32AbusingCount = 0;
		m_ui8TutorialIng = 0;
	}
};

struct USER_INFO_INVITEM_DATA
{
	// 아이템
	UINT8	_ui8Item = 0;									// 00011111	-- ...컬러닉,채팅,크로스,닉네임,위장

	// 아이템 리스트
	UINT8	_ui8NickColor = 0;								// 컬러닉네임
	UINT32	_wRank = RANK_NONE;										// 위장 계급
	UINT32	_wAprilFoolRank = RANK_NONE;							// 만우절 위장 계급
	TTCHAR	_cNick[NET_NICK_NAME_SIZE] = { 0 };					// 임시 닉네임	
	UINT8	_ui8CrossHairColor = 0;							// 칼라 크로스헤어
	UINT8	_ui8ChattingColor = 0;							// 칼라 채팅
	UINT16	_ui16MuzzleFlashColor = 0;						// 컬러총구화염

	void	Reset()
	{
		_ui8Item = 0;
		_ui8NickColor = 0;
		_wRank = RANK_NONE;
		_wAprilFoolRank = RANK_NONE;
		_cNick[0] = 0;
		_ui8CrossHairColor = 0;
		_ui8ChattingColor = 0;
		_ui16MuzzleFlashColor = 0;
		//i3mem::FillZero( _wCustomCrossHair, sizeof( UINT32 ) * 32 * 32 );
	}
};


struct NET_ENV_SET
{
	UINT32			m_GamePlayState = 0;
	UINT32			m_InterfaceState = 0;
	UINT32			m_GameSoundState = 0;
	UINT32			m_GameSoundVol = 0;
	UINT32			m_GameControlState = 0;
	UINT32			m_GameCommunityState = 0;
	UINT32			m_GameMacroState = 0;

	void Reset(void)
	{
		m_GamePlayState = 0;
		m_InterfaceState = 0;
		m_GameSoundState = 0;
		m_GameSoundVol = 0;
		m_GameControlState = 0;
		m_GameCommunityState = 0;
		m_GameMacroState = 0;
	}
};

struct USER_INFO_SEASON_RECORD
{
	UINT32	_smatch = 0;
	UINT32	_swin = 0;
	UINT32	_slose = 0;
	UINT32	_sdraw = 0;
	UINT32	_skillcount = 0;
	UINT32	_sheadshot = 0;
	UINT32	_sdeath = 0;
	UINT32  _sdmatch = 0;
	UINT32	_sdkillcount = 0;
	UINT32	_sdis = 0;
	UINT32  _sAssistCount = 0;
};

struct USER_INFO_RECORD : public USER_INFO_SEASON_RECORD
{
	UINT32	_match = 0;
	UINT32	_win = 0;
	UINT32	_lose = 0;
	UINT32	_draw = 0;
	UINT32	_killcount = 0;
	UINT32	_headshot = 0;
	UINT32	_death = 0;
	UINT32	_dmatch = 0;
	UINT32	_dkillcount = 0;
	UINT32	_dis = 0;
	UINT32	_AssistCount = 0;

	void	Reset() {
		_match = _win = _lose = _draw = _killcount = _headshot = _death = _smatch = _swin = _slose = _sdraw = _skillcount = _sheadshot = _sdeath = _AssistCount = _sAssistCount = 0;
		_dmatch = _dkillcount = _sdmatch = _sdkillcount = _dis = _sdis = 0;
	}
};

#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif