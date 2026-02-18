#ifndef __USER_EVENT__
#define __USER_EVENT__

class CUserEventInfo
{
	// Boost Event
	TYPE_ROOM_BONUS			m_stNormalBonus;
	TYPE_ROOM_BONUS			m_stPcCafeBonus[PC_MAX_COUNT];
	TYPE_ROOM_BONUS			m_stChannelBonus[CHANNEL_TYPE_MAX];
	TYPE_ROOM_BONUS			m_stNationBonus[GEOIP2_MAX_COUNTRY_COUNT];
	TYPE_ROOM_BONUS			m_stRankBonus[RANK_MAX];
	TYPE_ROOM_BONUS			m_stRandomMapBonus;

	UINT16					m_ui16RuleBonusCount;
	TYPE_STAGE_BONUS		m_stRuleBonus[BOOST_EVENT_COUNT];

	UINT16					m_ui16StageBonusCount;
	TYPE_STAGE_BONUS		m_stStageBonus[BOOST_EVENT_COUNT];

	UINT16					m_ui16MatchingBonusCount;
	TYPE_STAGE_BONUS		m_stMatchingBonus[BOOST_EVENT_COUNT];

	// Client 전달용
	INT32					m_i32ChannelBonusCount;
	TYPE_CHANNEL_BONUS		m_stChannelBonusList[ CHANNEL_TYPE_MAX ];

public:
	CUserEventInfo();
	~CUserEventInfo();

	void					Reset();
	void					MakeChannelBonus();

	BOOL					SetBoostValue( EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, INT32 i32Exp, INT32 i32Point );

	BOOL					GetChannelBonusInfo( UINT8* pi8ChannelBonusCount, TYPE_CHANNEL_BONUS* pstChannelBonus );
	BOOL					GetRoomBonus( EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, REAL32* pr32Exp, REAL32* pr32Point );
};

class CUserEventLocale
{
	CUserEventInfo			m_stEventLocale[ USER_EVENT_COUNT ];

public:
	CUserEventLocale();
	~CUserEventLocale();


	void					Reset();
	void					MakeChannelBonus();

	BOOL					SetBoostValue( UINT8 ui8LocaleCode, EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, INT32 i32Exp, INT32 i32Point );

	BOOL					GetChannelBonusInfo( UINT8 ui8LocaleCode, UINT8* pi8ChannelBonusCount, TYPE_CHANNEL_BONUS* pstChannelBonus )
	{
		if( USER_EVENT_COUNT <= ui8LocaleCode )	return FALSE;

		return m_stEventLocale[ ui8LocaleCode ].GetChannelBonusInfo( pi8ChannelBonusCount, pstChannelBonus );
	}
	BOOL					GetRoomBonus( UINT8 ui8LocaleCode, EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, REAL32* pr32Exp, REAL32* pr32Point )
	{
		if( USER_EVENT_COUNT <= ui8LocaleCode )	return FALSE;

		return m_stEventLocale[ ui8LocaleCode ].GetRoomBonus( eEvent, ui32SubType, pr32Exp, pr32Point );
	}
};

class CUserEventManager
{
	INT32					m_i32UseBufferIdx;
	INT32					m_i32BackBufferIdx;					// Next Buffer Idx 체크가 많아 변수를 따로 둠

	CUserEventLocale		m_stUserEvent[ 2 ];					// Double Buffer 를 위한 2개 배열

public:
	CUserEventManager();
	~CUserEventManager();

	void					SwapBuffer();

	void					Reset();
	void					MakeChannelBonus();

	BOOL					SetBoostValue( UINT8 ui8LocaleCode, EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, INT32 i32Exp, INT32 i32Point );
	
	BOOL					GetChannelBonusInfo( UINT8 ui8LocaleCode, UINT8* pi8ChannelBonusCount, TYPE_CHANNEL_BONUS* pstChannelBonus )
	{
		if( FALSE == m_stUserEvent[ m_i32UseBufferIdx ].GetChannelBonusInfo( ui8LocaleCode, pi8ChannelBonusCount, pstChannelBonus ) )
		{
			*pi8ChannelBonusCount = 0;
			return FALSE;
		}
		return TRUE;
	}
	BOOL					GetRoomBonus( UINT8 ui8LocaleCode, EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, REAL32* pr32Exp, REAL32* pr32Point )
	{
		return m_stUserEvent[ m_i32UseBufferIdx ].GetRoomBonus( ui8LocaleCode, eEvent, ui32SubType, pr32Exp, pr32Point );
	}
};

#endif