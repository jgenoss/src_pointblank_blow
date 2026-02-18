#include "pch.h"
#include "DomiScript.h"


DomiScript::DomiScript(i3Stage* pStage) : IModeScript(pStage),
	m_nIdxDay( 0), m_nIdxWave( 0), m_nIdxFlowType( 0), m_nIdxSubFlowType( 0),	
	m_nIdxScoreStart( 0), m_nIdxScoreEnd( 0),
	m_nIdxSPStart( 0),	m_nIdxSPEnd( 0),
	m_nIdxLifeStart( 0), m_nIdxLifeEnd( 0),
	m_nIdxRemainDino( 0), 
	m_nIdxSTTimeStart( 0), m_nIdxSTTimeEnd( 0),
	m_nIdxWaveSetTime( 0),
	m_rSPMax( 0.0f), m_rSPChargeSpd( 0.0f),
	m_pnSkillTable( NULL), m_nSkillCount( 0), m_nSkillValueSize( 0),
	m_nSkillMaxLevel( 0), m_nSkillValueCount( 0),
	m_nIdxUserGameOverFlag(0)
{
	for (INT32 i=0; i<4; i++)
		m_nSentrygunMaxHP[i] = 0;
}

DomiScript::~DomiScript()
{
	I3_SAFE_DELETE_ARRAY( m_pnSkillTable);
}

void DomiScript::OnOpen()
{
	// 모드 스크립트용 인덱스
	m_nIdxDay		= find_modedata_index( "DATA_IDX_DAY_NUMBER" );
	m_nIdxWave		= find_modedata_index( "DATA_IDX_WAVE_NUMBER" );
	m_nIdxFlowType	= find_modedata_index( "DATA_IDX_WAVE_GOAL" );
	m_nIdxSubFlowType = find_modedata_index( "DATA_IDX_USER_SUB_GOAL");

	m_nIdxScoreStart	= find_modedata_index( "DATA_IDX_USER_SCORE_START" );
	m_nIdxScoreEnd		= find_modedata_index( "DATA_IDX_USER_SCORE_END" );
	m_nIdxSPStart		= find_modedata_index( "DATA_IDX_USER_SP_START" );
	m_nIdxSPEnd			= find_modedata_index( "DATA_IDX_USER_SP_END" );
	m_nIdxLifeStart		= find_modedata_index( "DATA_IDX_RESPAWN_REMAIN_START" );
	m_nIdxLifeEnd		= find_modedata_index( "DATA_IDX_RESPAWN_REMAIN_END" );

	m_nIdxRemainDino	= find_modedata_index( "DATA_IDX_REMAIN_DINO_COUNT" );

	// DATA_IDX_USER_SP_IS_ABS - HUD 스킬에서 콜백등록

	m_nIdxSTTimeStart	= find_modedata_index( "DATA_IDX_USER_START_TIME_START" );
	m_nIdxSTTimeEnd		= find_modedata_index( "DATA_IDX_USER_START_TIME_END" );

	m_nIdxWaveSetTime	= find_modedata_index( "DATA_IDX_WAVE_SET_TIME" );


	m_rSPMax		= get_global_real( "SPMAXVALUE" );
	m_rSPChargeSpd	= get_global_real( "SPSUpplyAmountPerSec");

	m_nIdxUserGameOverFlag = find_modedata_index( "DATA_IDX_USER_GAMEOVER" );	

	_SetSkillSPTable();

	_SetSentrygunMaxHP();
}

void DomiScript::_SetSentrygunMaxHP()
{
	m_nSentrygunMaxHP[0] = get_global_int("SENTRYGUN_MAX_HP_LV1");
	m_nSentrygunMaxHP[1] = get_global_int("SENTRYGUN_MAX_HP_LV2");
	m_nSentrygunMaxHP[2] = get_global_int("SENTRYGUN_MAX_HP_LV3");
	m_nSentrygunMaxHP[3] = get_global_int("SENTRYGUN_MAX_HP_LV4");
}

BOOL DomiScript::_SetSkillSPTable(void)
{
	m_nSkillCount		= get_table_enum_int( "SKILL", "COUNT");
	m_nSkillMaxLevel	= get_global_int( "SKILL_MAX_LEVEL" );
	m_nSkillValueCount	= get_global_int( "SKILL_VALUE_COUNT" );

	m_nSkillValueSize	= m_nSkillMaxLevel * m_nSkillValueCount;


	string	strTName	= "SKILL_SET";

	access_global_table( strTName.c_str() );

	if (!IModeScript::convertible_table() )	
	{
		I3TRACE("Return type 'table' expected for variable '%s'", strTName.c_str() );
		return FALSE;
	}

	m_pnSkillTable		= new INT32[ m_nSkillCount * m_nSkillValueSize ];
	i3mem::FillZero( m_pnSkillTable, sizeof(INT32) *  m_nSkillCount * m_nSkillValueSize);

	for( INT32 i = 0; i < m_nSkillCount; i++)
	{
		IModeScript::access_table_idx(i);

		for( INT32 j = 0; j < m_nSkillMaxLevel; j++)
		{
			IModeScript::access_table_idx(j);

			for( INT32 k = 0; k < m_nSkillValueCount; k++ )
			{
				IModeScript::access_table_idx(k);

				INT32	nIdx	= i * m_nSkillValueSize + j * m_nSkillValueCount + k;
				string	strData	= IModeScript::get_stack_string();

				m_pnSkillTable[ nIdx]	= i3String::ToInt( strData.c_str() );

				pop_access( 1);
			}
			pop_access( 1);
		}
		pop_access( 1);
	}

	pop_access( 1);

	return TRUE;
}

INT32 DomiScript::_GetSkillLevelIndex( INT32 nType, INT32 nLv)
{
	nLv--;	// 레벨은 1부터 게산하지만 인덱스는 0 부터이므로

	if( nType >= m_nSkillCount || nType < 0
		|| nLv >= m_nSkillMaxLevel || nLv < 0 )
	{
		I3TRACE( "Invalid Skill Level Index - Skill(Count:%d, Cur:%d), Level(Max:%d, Cur:%d)\n", 
			m_nSkillCount, nType, m_nSkillMaxLevel, nLv );
		return -1;
	}

	return nType * m_nSkillValueSize + nLv * m_nSkillValueCount;
}

INT32 DomiScript::GetSkillCool(INT32 nType, INT32 nLv)
{
	INT32	nIdx	= _GetSkillLevelIndex( nType, nLv);

	if( nIdx < 0 )	return 0;

	return m_pnSkillTable[ nIdx + 0];
}
REAL32 DomiScript::GetSkillOpen(INT32 nType, INT32 nLv)
{
	INT32	nIdx	= _GetSkillLevelIndex( nType, nLv);

	if( nIdx < 0 )	return 9999999999.0f;	// 잘못된 인덱스면 못써야 하므로

	return (REAL32)m_pnSkillTable[ nIdx + 1];
}
REAL32 DomiScript::GetSkillCostSP(INT32 nType, INT32 nLv)
{
	INT32	nIdx	= _GetSkillLevelIndex( nType, nLv);

	if( nIdx < 0 )	return 0.0f;

	return (REAL32)m_pnSkillTable[ nIdx + 2];
}

UINT32 DomiScript::GetSkillItem(INT32 nType, INT32 nLv)
{
	INT32	nIdx	= _GetSkillLevelIndex( nType, nLv);

	if( nIdx < 0 )	return 0;

	return (UINT32)m_pnSkillTable[ nIdx + 3];
}

INT32 DomiScript::GetSentrygunMaxHP(INT32 level)
{
	return m_nSentrygunMaxHP[level];
}