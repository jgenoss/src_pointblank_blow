#include "pch.h"
#include "CommonDef_Skill.h"

MAIN_SKILL_INFO		g_MainSkillInfo[MAX_MAIN_SKILL][ MAIN_SKILL_MAX_LEVEL ];
ASSIST_SKILL_INFO	g_AssistSkillInfo[MAX_ASSIST_SKILL][ ASSIST_SKILL_MAX_LEVEL ];
COMMON_SKILL_INFO	g_CommonSkillInfo[MAX_COMMON_SKILL][ COMMON_SKILL_MAX_LEVEL ];

BOOL	CheckGetMainSkill( CHARA_CLASS ePrimaryClass, MAIN_SKILL_TYPE eSkill, UINT8 ui8Level, UINT32 ui32BattleTime, bool bIsEnable )
{	
	if( ui8Level > MAIN_SKILL_MAX_LEVEL )
		return FALSE;

	// Level 이 0 이면 스킬 해제로 판단 ( 빈 스킬 등록 )
	if( ui8Level == 0 )
		return TRUE;

	// 자신의 주특기 병과에 맞는 스킬 장착을 시도했는지 검사.
	switch( ePrimaryClass )
	{
	case CHARA_CLASS_ASSAULT:
		{
			if( eSkill < MAIN_ASSAULT_FIREDELAY || MAIN_ASSAULT_RANGE < eSkill )
				return FALSE;
		}
		break;
	case CHARA_CLASS_SMG:
		{
			if( eSkill < MAIN_SMG_FIREDELAY || MAIN_SMG_RELOAD < eSkill )
				return FALSE;
		}
		break;
	case CHARA_CLASS_SNIPER:
		{
			if( eSkill < MAIN_SNIPER_PENETRATE || MAIN_SNIPER_CHANGE < eSkill )
				return FALSE;
		}
		break;
	case CHARA_CLASS_MACHINE:
		{
			if( eSkill < MAIN_MG_PENETRATE || MAIN_MG_RELOAD < eSkill )
				return FALSE;
		}
		break;
	case CHARA_CLASS_SHOTGUN:
		{
			if( eSkill < MAIN_SHOTGUN_FIREDELAY || MAIN_SHOTGUN_CHANGE < eSkill )
				return FALSE;
		}
		break;
	default:
		return FALSE;
	}

	//스킬 착용제한이 On 되어 있을때, 캐릭터 배틀 타임이 충분한지 확인한다.
	if( bIsEnable && g_MainSkillInfo[ eSkill ][ ui8Level - 1 ].ui32BattleTime > ui32BattleTime )	return FALSE;

	return TRUE;
}

BOOL	CheckGetAssistSkill( ASSIST_SKILL_TYPE eSkill, UINT8 ui8Level, UINT8 ui8Rank, bool bIsEnable )
{
	// 스킬 레벨 설정이 0 이면 빈 스킬 장착으로 판단 ( 스킬 해제 )
	if( ui8Level > ASSIST_SKILL_MAX_LEVEL )								return FALSE;

	if( ui8Level == 0 )													return TRUE;

	if( eSkill <= ASSIST_SKILL_NONE || MAX_ASSIST_SKILL <= eSkill )		return FALSE;

	//스킬 착용제한이 On 되어 있을때, 계급이 기준 이상인지 확인한다.
	if( bIsEnable && g_AssistSkillInfo[ eSkill ][ ui8Level - 1 ].ui8Rank > ui8Rank )	return FALSE;

	return TRUE;
}

BOOL	CheckGetCommonSkill( COMMON_SKILL_TYPE eSkill, UINT8 ui8Level, UINT8 ui8Rank, bool bIsEnable )
{
	// 스킬 레벨이 0이면 장착 해제로 판단 ( 빈 스킬 장착 )
	if( ui8Level > COMMON_SKILL_MAX_LEVEL )								return FALSE;

	if( ui8Level == 0 )													return TRUE;

	if( eSkill <= COMMON_SKILL_NONE || MAX_COMMON_SKILL <= eSkill )		return FALSE;

	//스킬 착용제한이 On 되어 있을때, 계급이 기준 이상인지 확인한다.
	if( bIsEnable && g_CommonSkillInfo[ eSkill ][ ui8Level - 1 ].ui8Rank > ui8Rank )	return FALSE;

	return TRUE;
}

void	SetMainSkillInfo()
{
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][0].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][1].ui32BattleTime = 28800;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][2].ui32BattleTime = 90000;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][3].ui32BattleTime = 234000;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][4].ui32BattleTime = 468000;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][5].ui32BattleTime = 828000;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][6].ui32BattleTime = 882000;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][7].ui32BattleTime = 1224000;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][8].ui32BattleTime = 1368000;
	g_MainSkillInfo[MAIN_ASSAULT_FIREDELAY][9].ui32BattleTime = 1512000;

	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][0].ui32BattleTime = 18000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][1].ui32BattleTime = 36000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][2].ui32BattleTime = 108000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][3].ui32BattleTime = 252000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][4].ui32BattleTime = 504000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][5].ui32BattleTime = 900000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][6].ui32BattleTime = 1080000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][7].ui32BattleTime = 1260000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][8].ui32BattleTime = 1440000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][9].ui32BattleTime = 1620000;

	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][0].ui32BattleTime = 10800;
	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][1].ui32BattleTime = 25200;
	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][2].ui32BattleTime = 50400;
	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][3].ui32BattleTime = 216000;
	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][4].ui32BattleTime = 432000;
	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][5].ui32BattleTime = 648000;
	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][6].ui32BattleTime = 792000;
	g_MainSkillInfo[MAIN_ASSAULT_DAMAGE][7].ui32BattleTime =  936000;
	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][8].ui32BattleTime = 1062000;
	g_MainSkillInfo[MAIN_ASSAULT_CONTROL][9].ui32BattleTime = 1260000;

	g_MainSkillInfo[MAIN_ASSAULT_RANGE][0].ui32BattleTime = 3600;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][1].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][2].ui32BattleTime = 43200;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][3].ui32BattleTime = 162000;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][4].ui32BattleTime = 324000;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][5].ui32BattleTime = 540000;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][6].ui32BattleTime = 684000;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][7].ui32BattleTime = 864000;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][8].ui32BattleTime = 972000;
	g_MainSkillInfo[MAIN_ASSAULT_RANGE][9].ui32BattleTime = 1080000;

	g_MainSkillInfo[MAIN_SMG_FIREDELAY][0].ui32BattleTime = 18000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][1].ui32BattleTime = 36000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][2].ui32BattleTime = 108000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][3].ui32BattleTime = 252000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][4].ui32BattleTime = 504000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][5].ui32BattleTime = 900000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][6].ui32BattleTime = 1080000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][7].ui32BattleTime = 1260000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][8].ui32BattleTime = 1440000;
	g_MainSkillInfo[MAIN_SMG_FIREDELAY][9].ui32BattleTime = 1620000;

	g_MainSkillInfo[MAIN_SMG_PENETRATE][0].ui32BattleTime = 3600;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][1].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][2].ui32BattleTime = 43200;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][3].ui32BattleTime = 162000;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][4].ui32BattleTime = 324000;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][5].ui32BattleTime = 540000;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][6].ui32BattleTime = 684000;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][7].ui32BattleTime = 864000;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][8].ui32BattleTime = 972000;
	g_MainSkillInfo[MAIN_SMG_PENETRATE][9].ui32BattleTime = 1080000;

	g_MainSkillInfo[MAIN_SMG_CONTROL][0].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_SMG_CONTROL][1].ui32BattleTime = 28800;
	g_MainSkillInfo[MAIN_SMG_CONTROL][2].ui32BattleTime = 90000;
	g_MainSkillInfo[MAIN_SMG_CONTROL][3].ui32BattleTime = 234000;
	g_MainSkillInfo[MAIN_SMG_CONTROL][4].ui32BattleTime = 468000;
	g_MainSkillInfo[MAIN_SMG_CONTROL][5].ui32BattleTime = 828000;
	g_MainSkillInfo[MAIN_SMG_CONTROL][6].ui32BattleTime = 882000;
	g_MainSkillInfo[MAIN_SMG_CONTROL][7].ui32BattleTime = 1224000;
	g_MainSkillInfo[MAIN_SMG_CONTROL][8].ui32BattleTime = 1368000;
	g_MainSkillInfo[MAIN_SMG_CONTROL][9].ui32BattleTime = 1512000;

	g_MainSkillInfo[MAIN_SMG_RELOAD][0].ui32BattleTime = 10800;
	g_MainSkillInfo[MAIN_SMG_RELOAD][1].ui32BattleTime = 25200;
	g_MainSkillInfo[MAIN_SMG_RELOAD][2].ui32BattleTime = 50400;
	g_MainSkillInfo[MAIN_SMG_RELOAD][3].ui32BattleTime = 216000;
	g_MainSkillInfo[MAIN_SMG_RELOAD][4].ui32BattleTime = 432000;
	g_MainSkillInfo[MAIN_SMG_RELOAD][5].ui32BattleTime = 648000;
	g_MainSkillInfo[MAIN_SMG_RELOAD][6].ui32BattleTime = 792000;
	g_MainSkillInfo[MAIN_SMG_RELOAD][7].ui32BattleTime = 936000;
	g_MainSkillInfo[MAIN_SMG_RELOAD][8].ui32BattleTime = 1062000;
	g_MainSkillInfo[MAIN_SMG_RELOAD][9].ui32BattleTime = 1260000;

	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][0].ui32BattleTime = 3600;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][1].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][2].ui32BattleTime = 43200;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][3].ui32BattleTime = 162000;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][4].ui32BattleTime = 324000;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][5].ui32BattleTime = 540000;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][6].ui32BattleTime = 684000;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][7].ui32BattleTime = 864000;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][8].ui32BattleTime = 972000;
	g_MainSkillInfo[MAIN_SNIPER_PENETRATE][9].ui32BattleTime = 1080000;

	g_MainSkillInfo[MAIN_SNIPER_CHANGE][0].ui32BattleTime = 18000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][1].ui32BattleTime = 36000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][2].ui32BattleTime = 108000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][3].ui32BattleTime = 252000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][4].ui32BattleTime = 504000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][5].ui32BattleTime = 900000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][6].ui32BattleTime = 1080000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][7].ui32BattleTime = 1260000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][8].ui32BattleTime = 1440000;
	g_MainSkillInfo[MAIN_SNIPER_CHANGE][9].ui32BattleTime = 1620000;

	g_MainSkillInfo[MAIN_SNIPER_CONTROL][0].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][1].ui32BattleTime = 28800;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][2].ui32BattleTime = 90000;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][3].ui32BattleTime = 234000;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][4].ui32BattleTime = 468000;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][5].ui32BattleTime = 828000;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][6].ui32BattleTime = 882000;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][7].ui32BattleTime = 1224000;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][8].ui32BattleTime = 1368000;
	g_MainSkillInfo[MAIN_SNIPER_CONTROL][9].ui32BattleTime = 1512000;

	g_MainSkillInfo[MAIN_SNIPER_RANGE][0].ui32BattleTime = 10800;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][1].ui32BattleTime = 25200;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][2].ui32BattleTime = 50400;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][3].ui32BattleTime = 216000;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][4].ui32BattleTime = 432000;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][5].ui32BattleTime = 648000;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][6].ui32BattleTime = 792000;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][7].ui32BattleTime = 936000;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][8].ui32BattleTime = 1062000;
	g_MainSkillInfo[MAIN_SNIPER_RANGE][9].ui32BattleTime = 1260000;
	
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][0].ui32BattleTime = 3600;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][1].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][2].ui32BattleTime = 43200;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][3].ui32BattleTime = 162000;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][4].ui32BattleTime = 324000;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][5].ui32BattleTime = 540000;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][6].ui32BattleTime = 684000;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][7].ui32BattleTime = 864000;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][8].ui32BattleTime = 972000;
	g_MainSkillInfo[MAIN_SHOTGUN_FIREDELAY][9].ui32BattleTime = 1080000;

	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][0].ui32BattleTime = 18000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][1].ui32BattleTime = 36000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][2].ui32BattleTime = 108000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][3].ui32BattleTime = 252000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][4].ui32BattleTime = 504000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][5].ui32BattleTime = 900000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][6].ui32BattleTime = 1080000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][7].ui32BattleTime = 1260000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][8].ui32BattleTime = 1440000;
	g_MainSkillInfo[MAIN_SHOTGUN_CHANGE][9].ui32BattleTime = 1620000;

	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][0].ui32BattleTime = 10800;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][1].ui32BattleTime = 25200;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][2].ui32BattleTime = 50400;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][3].ui32BattleTime = 216000;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][4].ui32BattleTime = 432000;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][5].ui32BattleTime = 648000;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][6].ui32BattleTime = 792000;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][7].ui32BattleTime = 936000;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][8].ui32BattleTime = 1062000;
	g_MainSkillInfo[MAIN_SHOTGUN_CONTROL][9].ui32BattleTime = 1260000;

	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][0].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][1].ui32BattleTime = 28800;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][2].ui32BattleTime = 90000;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][3].ui32BattleTime = 234000;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][4].ui32BattleTime = 468000;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][5].ui32BattleTime = 828000;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][6].ui32BattleTime = 882000;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][7].ui32BattleTime = 1224000;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][8].ui32BattleTime = 1368000;
	g_MainSkillInfo[MAIN_SHOTGUN_RELOAD][9].ui32BattleTime = 1512000;

	g_MainSkillInfo[MAIN_MG_PENETRATE][0].ui32BattleTime = 3600;
	g_MainSkillInfo[MAIN_MG_PENETRATE][1].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_MG_PENETRATE][2].ui32BattleTime = 43200;
	g_MainSkillInfo[MAIN_MG_PENETRATE][3].ui32BattleTime = 162000;
	g_MainSkillInfo[MAIN_MG_PENETRATE][4].ui32BattleTime = 324000;
	g_MainSkillInfo[MAIN_MG_PENETRATE][5].ui32BattleTime = 540000;
	g_MainSkillInfo[MAIN_MG_PENETRATE][6].ui32BattleTime = 684000;
	g_MainSkillInfo[MAIN_MG_PENETRATE][7].ui32BattleTime = 864000;
	g_MainSkillInfo[MAIN_MG_PENETRATE][8].ui32BattleTime = 972000;
	g_MainSkillInfo[MAIN_MG_PENETRATE][9].ui32BattleTime = 1080000;

	g_MainSkillInfo[MAIN_MG_DAMAGE][0].ui32BattleTime = 18000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][1].ui32BattleTime = 36000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][2].ui32BattleTime = 108000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][3].ui32BattleTime = 252000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][4].ui32BattleTime = 504000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][5].ui32BattleTime = 900000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][6].ui32BattleTime = 1080000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][7].ui32BattleTime = 1260000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][8].ui32BattleTime = 1440000;
	g_MainSkillInfo[MAIN_MG_DAMAGE][9].ui32BattleTime = 1620000;

	g_MainSkillInfo[MAIN_MG_CONTROL][0].ui32BattleTime = 10800;
	g_MainSkillInfo[MAIN_MG_CONTROL][1].ui32BattleTime = 25200;
	g_MainSkillInfo[MAIN_MG_CONTROL][2].ui32BattleTime = 50400;
	g_MainSkillInfo[MAIN_MG_CONTROL][3].ui32BattleTime = 216000;
	g_MainSkillInfo[MAIN_MG_CONTROL][4].ui32BattleTime = 432000;
	g_MainSkillInfo[MAIN_MG_CONTROL][5].ui32BattleTime = 648000;
	g_MainSkillInfo[MAIN_MG_CONTROL][6].ui32BattleTime = 792000;
	g_MainSkillInfo[MAIN_MG_CONTROL][7].ui32BattleTime = 936000;
	g_MainSkillInfo[MAIN_MG_CONTROL][8].ui32BattleTime = 1062000;
	g_MainSkillInfo[MAIN_MG_CONTROL][9].ui32BattleTime = 1260000;

	g_MainSkillInfo[MAIN_MG_RELOAD][0].ui32BattleTime = 14400;
	g_MainSkillInfo[MAIN_MG_RELOAD][1].ui32BattleTime = 28800;
	g_MainSkillInfo[MAIN_MG_RELOAD][2].ui32BattleTime = 90000;
	g_MainSkillInfo[MAIN_MG_RELOAD][3].ui32BattleTime = 234000;
	g_MainSkillInfo[MAIN_MG_RELOAD][4].ui32BattleTime = 468000;
	g_MainSkillInfo[MAIN_MG_RELOAD][5].ui32BattleTime = 828000;
	g_MainSkillInfo[MAIN_MG_RELOAD][6].ui32BattleTime = 882000;
	g_MainSkillInfo[MAIN_MG_RELOAD][7].ui32BattleTime = 1224000;
	g_MainSkillInfo[MAIN_MG_RELOAD][8].ui32BattleTime = 1368000;
	g_MainSkillInfo[MAIN_MG_RELOAD][9].ui32BattleTime = 1512000;
}

void	SetAssistSkillInfo()
{
	g_AssistSkillInfo[ASSIST_HANDGUN_FIREDELAY][0].ui8Rank = RANK_06;
	g_AssistSkillInfo[ASSIST_HANDGUN_FIREDELAY][1].ui8Rank = RANK_10;
	g_AssistSkillInfo[ASSIST_HANDGUN_FIREDELAY][2].ui8Rank = RANK_16;
	g_AssistSkillInfo[ASSIST_HANDGUN_FIREDELAY][3].ui8Rank = RANK_22;
	g_AssistSkillInfo[ASSIST_HANDGUN_FIREDELAY][4].ui8Rank = RANK_32;
	
	g_AssistSkillInfo[ASSIST_HANDGUN_DAMAGE][0].ui8Rank = RANK_09;
	g_AssistSkillInfo[ASSIST_HANDGUN_DAMAGE][1].ui8Rank = RANK_16;
	g_AssistSkillInfo[ASSIST_HANDGUN_DAMAGE][2].ui8Rank = RANK_24;
	g_AssistSkillInfo[ASSIST_HANDGUN_DAMAGE][3].ui8Rank = RANK_28;
	g_AssistSkillInfo[ASSIST_HANDGUN_DAMAGE][4].ui8Rank = RANK_34;
	
	g_AssistSkillInfo[ASSIST_HANDGUN_CONTROL][0].ui8Rank = RANK_07;
	g_AssistSkillInfo[ASSIST_HANDGUN_CONTROL][1].ui8Rank = RANK_14;
	g_AssistSkillInfo[ASSIST_HANDGUN_CONTROL][2].ui8Rank = RANK_20;
	g_AssistSkillInfo[ASSIST_HANDGUN_CONTROL][3].ui8Rank = RANK_22;
	g_AssistSkillInfo[ASSIST_HANDGUN_CONTROL][4].ui8Rank = RANK_25;
	
	g_AssistSkillInfo[ASSIST_HANDGUN_RELOAD][0].ui8Rank = RANK_10;
	g_AssistSkillInfo[ASSIST_HANDGUN_RELOAD][1].ui8Rank = RANK_16;
	g_AssistSkillInfo[ASSIST_HANDGUN_RELOAD][2].ui8Rank = RANK_22;
	g_AssistSkillInfo[ASSIST_HANDGUN_RELOAD][3].ui8Rank = RANK_26;
	g_AssistSkillInfo[ASSIST_HANDGUN_RELOAD][4].ui8Rank = RANK_29;
	
	g_AssistSkillInfo[ASSIST_MELEE_DAMAGE][0].ui8Rank = RANK_13;
	g_AssistSkillInfo[ASSIST_MELEE_DAMAGE][1].ui8Rank = RANK_22;
	g_AssistSkillInfo[ASSIST_MELEE_DAMAGE][2].ui8Rank = RANK_26;
	g_AssistSkillInfo[ASSIST_MELEE_DAMAGE][3].ui8Rank = RANK_32;
	g_AssistSkillInfo[ASSIST_MELEE_DAMAGE][4].ui8Rank = RANK_34;
	
	g_AssistSkillInfo[ASSIST_MELEE_RANGE][0].ui8Rank = RANK_16;
	g_AssistSkillInfo[ASSIST_MELEE_RANGE][1].ui8Rank = RANK_18;
	g_AssistSkillInfo[ASSIST_MELEE_RANGE][2].ui8Rank = RANK_21;
	g_AssistSkillInfo[ASSIST_MELEE_RANGE][3].ui8Rank = RANK_28;
	g_AssistSkillInfo[ASSIST_MELEE_RANGE][4].ui8Rank = RANK_33;
	
	g_AssistSkillInfo[ASSIST_MELEE_CHANGE][0].ui8Rank = RANK_10;
	g_AssistSkillInfo[ASSIST_MELEE_CHANGE][1].ui8Rank = RANK_18;
	g_AssistSkillInfo[ASSIST_MELEE_CHANGE][2].ui8Rank = RANK_21;
	g_AssistSkillInfo[ASSIST_MELEE_CHANGE][3].ui8Rank = RANK_23;
	g_AssistSkillInfo[ASSIST_MELEE_CHANGE][4].ui8Rank = RANK_26;
	
	g_AssistSkillInfo[ASSIST_THROW_DAMAGE][0].ui8Rank = RANK_13;
	g_AssistSkillInfo[ASSIST_THROW_DAMAGE][1].ui8Rank = RANK_19;
	g_AssistSkillInfo[ASSIST_THROW_DAMAGE][2].ui8Rank = RANK_27;
	g_AssistSkillInfo[ASSIST_THROW_DAMAGE][3].ui8Rank = RANK_33;
	g_AssistSkillInfo[ASSIST_THROW_DAMAGE][4].ui8Rank = RANK_36;

	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_RANGE][0].ui8Rank = RANK_13;
	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_RANGE][1].ui8Rank = RANK_18;
	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_RANGE][2].ui8Rank = RANK_25;
	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_RANGE][3].ui8Rank = RANK_29;
	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_RANGE][4].ui8Rank = RANK_34;
	
	g_AssistSkillInfo[ASSIST_THROW_THROW_RANGE][0].ui8Rank = RANK_02;
	g_AssistSkillInfo[ASSIST_THROW_THROW_RANGE][1].ui8Rank = RANK_11;
	g_AssistSkillInfo[ASSIST_THROW_THROW_RANGE][2].ui8Rank = RANK_16;
	g_AssistSkillInfo[ASSIST_THROW_THROW_RANGE][3].ui8Rank = RANK_26;
	g_AssistSkillInfo[ASSIST_THROW_THROW_RANGE][4].ui8Rank = RANK_32;

	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_TIME][0].ui8Rank = RANK_19;
	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_TIME][1].ui8Rank = RANK_23;
	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_TIME][2].ui8Rank = RANK_28;
	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_TIME][3].ui8Rank = RANK_33;
	g_AssistSkillInfo[ASSIST_THROW_EXPLOSION_TIME][4].ui8Rank = RANK_35;
	
	g_AssistSkillInfo[ASSIST_EXP][0].ui8Rank = RANK_13;
	g_AssistSkillInfo[ASSIST_EXP][1].ui8Rank = RANK_17;
	g_AssistSkillInfo[ASSIST_EXP][2].ui8Rank = RANK_21;
	g_AssistSkillInfo[ASSIST_EXP][3].ui8Rank = RANK_MAX;
	g_AssistSkillInfo[ASSIST_EXP][4].ui8Rank = RANK_MAX;
	
	g_AssistSkillInfo[ASSIST_POINT][0].ui8Rank = RANK_14;
	g_AssistSkillInfo[ASSIST_POINT][1].ui8Rank = RANK_18;
	g_AssistSkillInfo[ASSIST_POINT][2].ui8Rank = RANK_21;
	g_AssistSkillInfo[ASSIST_POINT][3].ui8Rank = RANK_MAX;
	g_AssistSkillInfo[ASSIST_POINT][4].ui8Rank = RANK_MAX;
}

void	SetCommonSkillInfo()
{
	g_CommonSkillInfo[COMMON_RUN][0].ui8Rank = RANK_01;
	g_CommonSkillInfo[COMMON_RUN][1].ui8Rank = RANK_09;
	g_CommonSkillInfo[COMMON_RUN][2].ui8Rank = RANK_12;
	g_CommonSkillInfo[COMMON_RUN][3].ui8Rank = RANK_15;
	g_CommonSkillInfo[COMMON_RUN][4].ui8Rank = RANK_18;
	g_CommonSkillInfo[COMMON_RUN][5].ui8Rank = RANK_21;
	g_CommonSkillInfo[COMMON_RUN][6].ui8Rank = RANK_25;
	g_CommonSkillInfo[COMMON_RUN][7].ui8Rank = RANK_29;
	g_CommonSkillInfo[COMMON_RUN][8].ui8Rank = RANK_33;
	g_CommonSkillInfo[COMMON_RUN][9].ui8Rank = RANK_36;
	
	g_CommonSkillInfo[COMMON_WALK][0].ui8Rank = RANK_02;
	g_CommonSkillInfo[COMMON_WALK][1].ui8Rank = RANK_07;
	g_CommonSkillInfo[COMMON_WALK][2].ui8Rank = RANK_10;
	g_CommonSkillInfo[COMMON_WALK][3].ui8Rank = RANK_12;
	g_CommonSkillInfo[COMMON_WALK][4].ui8Rank = RANK_13;
	g_CommonSkillInfo[COMMON_WALK][5].ui8Rank = RANK_15;
	g_CommonSkillInfo[COMMON_WALK][6].ui8Rank = RANK_17;
	g_CommonSkillInfo[COMMON_WALK][7].ui8Rank = RANK_20;
	g_CommonSkillInfo[COMMON_WALK][8].ui8Rank = RANK_22;
	g_CommonSkillInfo[COMMON_WALK][9].ui8Rank = RANK_26;
	
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][0].ui8Rank = RANK_01;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][1].ui8Rank = RANK_08;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][2].ui8Rank = RANK_11;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][3].ui8Rank = RANK_14;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][4].ui8Rank = RANK_17;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][5].ui8Rank = RANK_20;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][6].ui8Rank = RANK_24;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][7].ui8Rank = RANK_28;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][8].ui8Rank = RANK_32;
	g_CommonSkillInfo[COMMON_DAMAGE_RATE][9].ui8Rank = RANK_34;
	
	g_CommonSkillInfo[COMMON_HELMET_DEF][0].ui8Rank = RANK_03;
	g_CommonSkillInfo[COMMON_HELMET_DEF][1].ui8Rank = RANK_07;
	g_CommonSkillInfo[COMMON_HELMET_DEF][2].ui8Rank = RANK_10;
	g_CommonSkillInfo[COMMON_HELMET_DEF][3].ui8Rank = RANK_12;
	g_CommonSkillInfo[COMMON_HELMET_DEF][4].ui8Rank = RANK_13;
	g_CommonSkillInfo[COMMON_HELMET_DEF][5].ui8Rank = RANK_15;
	g_CommonSkillInfo[COMMON_HELMET_DEF][6].ui8Rank = RANK_17;
	g_CommonSkillInfo[COMMON_HELMET_DEF][7].ui8Rank = RANK_19;
	g_CommonSkillInfo[COMMON_HELMET_DEF][8].ui8Rank = RANK_21;
	g_CommonSkillInfo[COMMON_HELMET_DEF][9].ui8Rank = RANK_22;
	
	g_CommonSkillInfo[COMMON_JUMP][0].ui8Rank = RANK_04;
	g_CommonSkillInfo[COMMON_JUMP][1].ui8Rank = RANK_08;
	g_CommonSkillInfo[COMMON_JUMP][2].ui8Rank = RANK_11;
	g_CommonSkillInfo[COMMON_JUMP][3].ui8Rank = RANK_14;
	g_CommonSkillInfo[COMMON_JUMP][4].ui8Rank = RANK_17;
	g_CommonSkillInfo[COMMON_JUMP][5].ui8Rank = RANK_19;
	g_CommonSkillInfo[COMMON_JUMP][6].ui8Rank = RANK_21;
	g_CommonSkillInfo[COMMON_JUMP][7].ui8Rank = RANK_23;
	g_CommonSkillInfo[COMMON_JUMP][8].ui8Rank = RANK_27;
	g_CommonSkillInfo[COMMON_JUMP][9].ui8Rank = RANK_30;
}

void GetMainSkillBit( UINT8& ui8SkillType, UINT8& ui8SkillLv, INT32 i32SkillData )
{
	ui8SkillType	=	(UINT8)(MAIN_SKILL_TYPE)	(i32SkillData >> SKILL_INFO_BIT);
	ui8SkillLv	=	(i32SkillData & 0x00FF);
}

void GetAssistSkillBit( UINT8& ui8SkillType, UINT8& ui8SkillLv, INT32 i32SkillData )
{
	ui8SkillType	=	(UINT8)(ASSIST_SKILL_TYPE)	(i32SkillData >> SKILL_INFO_BIT);
	ui8SkillLv	=	(i32SkillData & 0x00FF);
}

void GetCommonSkillBit( UINT8& ui8SkillType, UINT8& ui8SkillLv, INT32 i32SkillData )
{
	ui8SkillType	=	(UINT8)(COMMON_SKILL_TYPE)	(i32SkillData >> SKILL_INFO_BIT);
	ui8SkillLv	=	(i32SkillData & 0x00FF);
}

void SetMainSkillBit( UINT8 ui8SkillType, UINT8 ui8SkillLv, INT32& i32SkillData )
{	
	i32SkillData   = ui8SkillType << SKILL_INFO_BIT;
	i32SkillData   |= ui8SkillLv;
}

void SetAssistSkillBit( UINT8 ui8SkillType, UINT8 ui8SkillLv, INT32& i32SkillData )
{
	i32SkillData   = ui8SkillType << SKILL_INFO_BIT;
	i32SkillData   |= ui8SkillLv;
}

void SetCommonSkillBit( UINT8 ui8SkillType, UINT8 ui8SkillLv, INT32& i32SkillData )
{
	i32SkillData   = ui8SkillType << SKILL_INFO_BIT;
	i32SkillData   |= ui8SkillLv;
}