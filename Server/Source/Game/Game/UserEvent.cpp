#include "pch.h"
#include "UserEvent.h"

CUserEventInfo::CUserEventInfo()
{
}

CUserEventInfo::~CUserEventInfo()
{
}

void CUserEventInfo::Reset()
{
	i3mem::FillZero( &m_stNormalBonus,	sizeof(TYPE_ROOM_BONUS) );
	i3mem::FillZero( m_stPcCafeBonus,	sizeof(TYPE_ROOM_BONUS) * PC_MAX_COUNT );
	i3mem::FillZero( m_stChannelBonus,	sizeof(TYPE_ROOM_BONUS) * CHANNEL_TYPE_MAX );
	i3mem::FillZero( m_stNationBonus,	sizeof(TYPE_ROOM_BONUS) * GEOIP2_MAX_COUNTRY_COUNT );
	i3mem::FillZero( m_stRankBonus,		sizeof(TYPE_ROOM_BONUS) * RANK_MAX );
	i3mem::FillZero( m_stRuleBonus,		sizeof(TYPE_STAGE_BONUS) * BOOST_EVENT_COUNT);
	i3mem::FillZero( m_stStageBonus,	sizeof(TYPE_STAGE_BONUS) * BOOST_EVENT_COUNT);
	i3mem::FillZero( m_stMatchingBonus, sizeof(TYPE_STAGE_BONUS) * BOOST_EVENT_COUNT);
	i3mem::FillZero( m_stChannelBonusList, sizeof(TYPE_CHANNEL_BONUS) * CHANNEL_TYPE_MAX );
	i3mem::FillZero(&m_stRandomMapBonus, sizeof(TYPE_ROOM_BONUS));

	m_ui16RuleBonusCount		= 0;
	m_ui16StageBonusCount		= 0;
	m_ui16MatchingBonusCount	= 0;
	m_i32ChannelBonusCount		= 0;

}

void CUserEventInfo::MakeChannelBonus()
{
	TYPE_ROOM_BONUS*	pRoomBouns;
	TYPE_CHANNEL_BONUS*	pChannelBonus;
	for( INT32 i = 0; i < CHANNEL_TYPE_MAX; ++i )
	{
		pRoomBouns					= &m_stChannelBonus[ i ];

		if( FALSE == pRoomBouns->_bActive )	continue;

		pChannelBonus				= &m_stChannelBonusList[ m_i32ChannelBonusCount ];

		pChannelBonus->_ui16Type	= (UINT16)i;
		pChannelBonus->_ui32Exp		= (UINT32)pRoomBouns->_r32Exp;
		pChannelBonus->_ui32Point	= (UINT32)pRoomBouns->_r32Point;

		m_i32ChannelBonusCount++;
	}
}

BOOL CUserEventInfo::SetBoostValue( EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, INT32 i32Exp, INT32 i32Point )
{
	TYPE_ROOM_BONUS* pBonus = NULL;

	switch (eEvent)
	{
	case EVENT_BONUS_NORMAL:
		{
			pBonus = &m_stNormalBonus;
		}
		break;
	case EVENT_BONUS_PC_CAFE:
		{
			if (PC_MAX_COUNT <= ui32SubType)	return FALSE;
			pBonus = &m_stPcCafeBonus[ui32SubType];
		}
		break;
	case EVENT_BONUS_CHANNEL:
		{
			if (CHANNEL_TYPE_MAX <= ui32SubType)	return FALSE;
			pBonus = &m_stChannelBonus[ui32SubType];
		}
		break;
	case EVENT_BONUS_NATION:
		{
			if (GEOIP2_MAX_COUNTRY_COUNT <= ui32SubType)	return FALSE;
			pBonus = &m_stNationBonus[ui32SubType];
		}
		break;
	case EVENT_BONUS_RANK:
		{
			if (RANK_MAX <= ui32SubType)	return FALSE;
			pBonus = &m_stRankBonus[ui32SubType];
		}
		break;
	case EVENT_BONUS_RULE:
		{
			if (BOOST_EVENT_COUNT <= m_ui16RuleBonusCount)	return FALSE;
			m_stRuleBonus[m_ui16RuleBonusCount]._ui32StageID = ui32SubType;
			pBonus = &m_stRuleBonus[m_ui16RuleBonusCount];
			++m_ui16RuleBonusCount;
		}
		break;
	case EVENT_BONUS_STAGE:
		{
			if (BOOST_EVENT_COUNT <= m_ui16StageBonusCount)	return FALSE;
			m_stStageBonus[m_ui16StageBonusCount]._ui32StageID = ui32SubType;
			pBonus = &m_stStageBonus[m_ui16StageBonusCount];
			++m_ui16StageBonusCount;
		}
		break;
	case EVENT_BONUS_MATCHING:
		{
			if (BOOST_EVENT_COUNT <= m_ui16MatchingBonusCount)	return FALSE;
			m_stMatchingBonus[m_ui16MatchingBonusCount]._ui32StageID = ui32SubType;
			pBonus = &m_stMatchingBonus[m_ui16MatchingBonusCount];
			++m_ui16MatchingBonusCount;
		}
		break;
	case EVENT_BONUS_RANDOM_MAP:
		{
			pBonus = &m_stRandomMapBonus;
		}
		break;
		default:										return FALSE;
	}
	
	pBonus->_bActive	= TRUE;
	pBonus->_r32Exp		+= ((0.01f*i32Exp) + 0.00001f);
	pBonus->_r32Point	+= ((0.01f*i32Point) + 0.00001f);

	return TRUE;
}

BOOL CUserEventInfo::GetChannelBonusInfo( UINT8* pi8ChannelBonusCount, TYPE_CHANNEL_BONUS* pstChannelBonus )
{
	*pi8ChannelBonusCount	= (UINT8)m_i32ChannelBonusCount;
	i3mem::Copy( pstChannelBonus, m_stChannelBonus, sizeof(TYPE_CHANNEL_BONUS)*m_i32ChannelBonusCount );

	return TRUE;
}

BOOL CUserEventInfo::GetRoomBonus(EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, REAL32* pr32Exp, REAL32* pr32Point)
{
	TYPE_ROOM_BONUS* pBonus = nullptr;
	TYPE_STAGE_BONUS* pStageBonus = nullptr;
	UINT16 ui16Count = 0;
	
	switch( eEvent )
	{
	case EVENT_BONUS_NORMAL:
		{
			pBonus = &m_stNormalBonus;
		}
		break;
	case EVENT_BONUS_PC_CAFE:
		{
			if (PC_MAX_COUNT <= ui32SubType)	return FALSE;
			pBonus = &m_stPcCafeBonus[ui32SubType];
		}
		break;
	case EVENT_BONUS_CHANNEL:
		{
			if (CHANNEL_TYPE_MAX <= ui32SubType)	return FALSE;
			pBonus = &m_stChannelBonus[ui32SubType];
		}
		break;
	case EVENT_BONUS_NATION:
		{
			if (GEOIP2_MAX_COUNTRY_COUNT <= ui32SubType)	return FALSE;
			pBonus = &m_stNationBonus[ui32SubType];
		}
		break;
	case EVENT_BONUS_RANK:
		{
			if (RANK_MAX <= ui32SubType)	return FALSE;
			pBonus = &m_stRankBonus[ui32SubType];
		}
		break;
	case EVENT_BONUS_RULE:
		{
			ui16Count	= m_ui16RuleBonusCount;
			pStageBonus	= m_stRuleBonus;
		}
		break;
	case EVENT_BONUS_STAGE:
		{	
			ui16Count	= m_ui16StageBonusCount;
			pStageBonus	= m_stStageBonus;
		}
		break;
	case EVENT_BONUS_MATCHING:
		{
			ui16Count	= m_ui16MatchingBonusCount;
			pStageBonus	= m_stMatchingBonus;			
		}
		break;
	case EVENT_BONUS_RANDOM_MAP:
		{
			pBonus = &m_stRandomMapBonus;
		}
		break;
	default:										return FALSE;
	}

	//Type이 EVENT_BONUS_RULE, EVENT_BONUS_STAGE, EVENT_BONUS_MATCHING 인경우
	//해당 리스트에서 이벤트 데이터를 찾습니다.
	if ( nullptr == pBonus )
	{ 
		for ( UINT16 i = 0 ; i < ui16Count ; ++i )
		{
			if ( pStageBonus[i]._ui32StageID == ui32SubType )
			{
				pBonus = &pStageBonus[i];
				break;
			}
		}
	}
	
	if( nullptr == pBonus )						return FALSE;
	if( FALSE == pBonus->_bActive )				return FALSE;

	*pr32Exp	+= pBonus->_r32Exp;
	*pr32Point	+= pBonus->_r32Point;

	return TRUE;
}

CUserEventLocale::CUserEventLocale()
{
}

CUserEventLocale::~CUserEventLocale()
{
}

void CUserEventLocale::Reset()
{
	for( INT32 i = 0 ; i < USER_EVENT_COUNT ; i++ )
	{
		m_stEventLocale[i].Reset();
	}
}

BOOL CUserEventLocale::SetBoostValue( UINT8 ui8LocaleCode, EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, INT32 i32Exp, INT32 i32Point )
{
	if( USER_EVENT_COUNT <= ui8LocaleCode )	return FALSE;

	return m_stEventLocale[ ui8LocaleCode ].SetBoostValue( eEvent, ui32SubType, i32Exp, i32Point );
}

void CUserEventLocale::MakeChannelBonus()
{
	for( INT32 i = 0 ; i < USER_EVENT_COUNT ; i++ )
	{
		m_stEventLocale[ i ].MakeChannelBonus();
	}
}

CUserEventManager::CUserEventManager()
{
	m_i32UseBufferIdx	= 0;
	m_i32BackBufferIdx	= 1;
}

CUserEventManager::~CUserEventManager()
{
}

void CUserEventManager::SwapBuffer()
{
	INT32 i32Temp		= m_i32UseBufferIdx;
	m_i32UseBufferIdx	= m_i32BackBufferIdx;
	m_i32BackBufferIdx	= i32Temp;
}

void CUserEventManager::Reset()
{
	m_stUserEvent[ m_i32BackBufferIdx ].Reset();
}

void CUserEventManager::MakeChannelBonus()
{
	m_stUserEvent[ m_i32BackBufferIdx ].MakeChannelBonus();
}

BOOL CUserEventManager::SetBoostValue( UINT8 ui8LocaleCode, EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, INT32 i32Exp, INT32 i32Point )
{
	return m_stUserEvent[ m_i32BackBufferIdx ].SetBoostValue( ui8LocaleCode, eEvent, ui32SubType, i32Exp, i32Point );
}
