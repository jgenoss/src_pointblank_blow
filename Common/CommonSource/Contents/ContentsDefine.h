#pragma once


//////////////////////////////////////////////////////////////////////////
// Contents List
enum CONTENTS
{
	CONTENTS_CALCULATE_POINT	= 0,		// 포인트 보상
	CONTENTS_REPARE_DURABILITY	,			// 내구도 수리				// 작업 완료
	CONTENTS_DEFAULT_WEAPON		,			// 주무기 지급 여부			// 작업 완료
	CONTENTS_MULTI_SLOT			,			// 멀티슬롯
	CONTENTS_IP_BLOCK			,			// IP Block					// 작업 완료
	CONTENTS_CASH_RECHARGE		,			// 캐시 충전				// 작업 완료
	CONTENTS_RANKING			,			// 랭킹						// 작업 완료
	CONTENTS_GIFT				,			// 선물						// 작업 완료

	CONTENTS_TS_EVENT			,			// TS 이벤트
	CONTENTS_ATTENDANCE			,			// 출석 이벤트	
	CONTENTS_MISSION_CARD		,			// 미션 카드
	CONTENTS_MEDAL				,			// 메달

	CONTENTS_CLAN				,			// 클랜
	CONTENTS_SHOP				,			// 상점
	CONTENTS_GACHA				,			// 가챠
	CONTENTS_SHUTDOWN			,			// 셧다운

	CONTENTS_CLAN_MATCH_NEW		,
	CONTENTS_ACCOUNT_TRANS		,			// 계정 이동
	CONTENTS_TEAM_CHANGE		,			// 게임중 팀 변경

	CONTENTS_RESTRICT_CHARA_BUY	,			// 캐릭터 구매 제한
	CONTENTS_RESTRICT_SKILL		,			// 스킬 사용 제한 조건
	CONTENTS_ULSAN_GUARD		,			// ULSAN GUARD

	CONTENTS_DORMANT			,			// 휴면 계정 출석 이벤트
	CONTENTS_TIER_RANK			,			// 랭크 확장
	CONTENTS_GIFTSHOP			,			// 선물샵
	CONTENTS_RANDOMMAP			,			// 랜덤맵 시스템

	CONTENTS_COUNT				,
};



//////////////////////////////////////////////////////////////////////////
// Structures
#pragma pack(push, 1)

struct ContentsInfo
{
	INT32				m_i32Version;
	bool				m_bEnable;
};

struct ContentsEnableFlag
{
public:
	void				SetContentsEnabled(CONTENTS eContents)		{ m_ui32ContentsFlag |= (1 << eContents); }
	bool				IsContentsEnabled(CONTENTS eContents)		{ return (m_ui32ContentsFlag & (1 << eContents)) > 0;  }
	void				Reset()										{ m_ui32ContentsFlag = 0; }

public:
	ContentsEnableFlag() : m_ui32ContentsFlag(0)	{}
	~ContentsEnableFlag()							{}

private:
	UINT32				m_ui32ContentsFlag;
};

#pragma pack(pop)
