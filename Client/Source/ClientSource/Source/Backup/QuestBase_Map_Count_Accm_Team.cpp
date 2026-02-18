#include "pch.h"
#include "QuestBase_Map_Count_Accm_Team.h"
#include "QPI.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CQuestBase_Map_Count_Accm_Team);//, CQuestBase);

CQuestBase_Map_Count_Accm_Team::CQuestBase_Map_Count_Accm_Team(void)
{
	m_nValidKillCount = 0;
}

CQuestBase_Map_Count_Accm_Team::~CQuestBase_Map_Count_Accm_Team(void)
{
}

bool		CQuestBase_Map_Count_Accm_Team::_checkCondition(void)
{
	if( _getStage() != STAGE_ID_NA)
	{
		STAGE_ID id1, id2;

		id1 = _getStage();
		id2 = QPI_GetStage();

		if (getStageOrd(id1) != getStageOrd(id2))
			return false;
	}

	if( _getTeam() != 0)
	{
		if( g_pGameContext->getMyTeam() != _getTeam())
			return false;
	}

	return true;
}

// 주인공으로 인해, 다른 캐릭터가 죽었을 때 호출된다.
// Kill Chara.에 대한 구체적인 정보는 KILL_INFO Structure에 저장되어 있다.
bool		CQuestBase_Map_Count_Accm_Team::OnKill( KILL_INFO * pKillInfo)
{
	switch( _getAction())
	{
		case ACTION_ID_KILL :
			{
				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;
			}break;
		case ACTION_ID_SUICIDE :
			{
				if( pKillInfo->m_CharaShotIdx != g_pGameContext->getMySlotIdx())
					return false;
			}
			break;
		case ACTION_ID_DOUBLE_KILL:
			{
				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;
			}
			break;
		case ACTION_ID_TRIPLE_KILL:
			{
				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;
			}
			break;

		default :
			return false;
	}

	if( _checkCondition() == false)
		return false;

	//action에 누적 카운트가 따로 필요한 경우
	UINT8 validKillCnt = _getValidKillCount();

	_setValidKillCount( validKillCnt + 1);

	bool valid = false;
	switch( _getAction())
	{
		case ACTION_ID_DOUBLE_KILL:
			{
				if(( validKillCnt < 2) && (_getValidKillCount() >= 2))
				{
					valid = true;
				}
			}
			break;
		case ACTION_ID_TRIPLE_KILL:
			{
				if(( validKillCnt < 3) && (_getValidKillCount() >= 3))
				{
					valid = true;
				}
			}
			break;
		default:
			{
			valid = true;
			break;
			}
	}

	if( !valid )	return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	I3TRACE( "OnKill : %p : %d\n", this, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// 다른 캐릭터에 의해 주인공 캐릭터가 죽임을 당했을 때에 호출된다.
// Kill Chara에 대한 구체적인 정보는 KILL_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Accm_Team::OnDeath( KILL_INFO * pDeathInfo)
{
	if( _getAction() != ACTION_ID_DEATH)
		return false;

	if( _checkCondition() == false)
		return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// 케릭터가 공격을 당했을 때 호출된다.
// 공격 내용에 대한 구체적인 정보는 DAMAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Accm_Team::OnDamage( DAMAGE_INFO * pDamageInfo)
{
	switch( _getAction())
	{
		case ACTION_ID_HELMET_PROTECTION :
			if( pDamageInfo->m_HitPart != HIT_CHARA_PART_HELMET)
				return false;

			break;

		default :
			return false;
	}

	if( _checkCondition() == false)
		return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// 새롭게 Stage를 시작했을 때 호출된다.
// Deathmatch, Mission 등의 종류를 막론하고 각 Stage마다 한번만 호출된다.
// 새로 시작하는 Stage에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Accm_Team::OnStageStart( STAGE_INFO * pStage)
{
	if( _checkCondition() == false)
		return false;

	switch( _getAction())
	{
		case ACTION_ID_STAGE_ENTER :
			break;

		case ACTION_ID_STAGE_INTERCEPT :
			if( QPI_IsIntercept() == FALSE)
				return false;

			break;

		default :
			return false;
	}

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// Stage를 종료할 때 호출된다.
// Deathmatch, Mission 등의 종류를 막론하고 각 Stage마다 한번만 호출된다.
// 새로 시작하는 Stage에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Accm_Team::OnStageEnd( STAGE_INFO * pStageInfo)
{
	bool bRet = true;	
	//연속카운팅 퀘스트의 경우는 stageend에서 퀘스트미완료판단시 연속카운트를 0으로 만들어주어야한다. 
	//그것을 위한 판단용 -raja

	if( _checkCondition() == false)
		bRet = false;

	switch( _getAction())
	{
		//각 모드에 따른 승리판별법이 틀린관계로, 게임시스템에서 넘겨주는 승리정보를 그대로 사용 - raja
		case ACTION_ID_WIN :
			if( g_pGameContext->getMyTeam() != pStageInfo->m_WinnerTeam)
			{
				bRet = false;
			}
			break;

		case ACTION_ID_DEFEAT :
			if( g_pGameContext->getMyTeam() == pStageInfo->m_WinnerTeam)
			{
				bRet = false;
			}
			break;

		case ACTION_ID_HIT_HEAD_20 :
		case ACTION_ID_HIT_HEAD_30 :
		case ACTION_ID_HIT_CHEST_20 :
		case ACTION_ID_HIT_CHEST_30 :
		case ACTION_ID_HIT_ARM_20 :
		case ACTION_ID_HIT_ARM_30 :
		case ACTION_ID_HIT_LEG_20 :
		case ACTION_ID_HIT_LEG_30 :
			break;

		case ACTION_ID_ENFORCE_EXIT :
			break;
			
		case ACTION_ID_RANK_1:							// 최종 결과 순위 1
		case ACTION_ID_RANK_2:							// 최종 결과 순위 2
		case ACTION_ID_RANK_3:							// 최종 결과 순위 3
		case ACTION_ID_RANK_4:							// 최종 결과 순위 4
		case ACTION_ID_RANK_5:							// 최종 결과 순위 5
		case ACTION_ID_RANK_6:							// 최종 결과 순위 6
		case ACTION_ID_RANK_7:							// 최종 결과 순위 7
		case ACTION_ID_RANK_8:							// 최종 결과 순위 8
			if( (_getAction() - ACTION_ID_RANK_1) != QPI_GetPlayerStageRank())
			{ 
				bRet = false;
			}
			break;
		default :
			bRet = false;
			break;
	}

	if( bRet)
	{
		UINT8 cnt = getArg( 5);

		cnt++;
		setArg( 5, cnt);

		if( cnt >= _getCount())
			return true;
	}

	if( !_isAccmQuest())
		setArg( 5, 0);

	return false;
}

// 새롭게 Round를 시작했을 때 호출된다.
// Deathmatch 등과 같이 Round제 Mission이 아닌경우에는 호출되지 않을 수 있다.
// 새로 시작하는 Round에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Accm_Team::OnRoundStart( STAGE_INFO * pStageInfo)
{
	return false;
}

// Round를 종료할 때 호출된다.
// Deathmatch 등과 같이 Round제 Mission이 아닌경우에는 호출되지 않을 수 있다.
// 새로 시작하는 Round에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Accm_Team::OnRoundEnd( STAGE_INFO * pStageInfo)
{
	return false;
}

// 주인공 캐릭터가 특정 동작을 취했을 때 호출된다.
// 동작에 대한 구체적인 정보는 ACTION_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Accm_Team::OnAction( ACTION_INFO * pAction)
{
	if( _getAction() != pAction->m_Action)
		return false;

	if( pAction->m_CharaSlotIdx != g_pGameContext->getMySlotIdx())
		return false;

	if( _checkCondition() == false)
		return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	I3TRACE( "OnAction : %p : %d\n", this, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// 화면에 출력할 Mission에 대한 설명문을 요구할 때 호출된다.
// CQuestBase Class에서 파생된 개별 Mission Class들은
// 반드시 이 함수를 Override하여 적절한 형식의 문자열을 제공해야 한다.
void		CQuestBase_Map_Count_Accm_Team::OnQueryDesc( char * pszBuff, INT32 maxLen)
{
	char conv[256], temp[256];

	*pszBuff = 0;

	// 위치
	switch( _getStage())
	{
		case STAGE_ID_NA :				i3::safe_string_copy( temp, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC4"), 256 );/*조건 없음*/	break;
		default :
		{
			char szTemp2[256];
			getStageDisplayName(szTemp2, _getStage());
			i3::safe_string_copy(temp, szTemp2, 256 );	
		}
		break;
	}
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC5"), temp);/*위치 : %s\n \n*/
	strcat( pszBuff, conv);

	/*
	// 모드
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC8"));//모드      : 조건 없음\n \n
	strcat( pszBuff, conv);

	// 진영
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC3"));//진영      : 조건 없음\n \n
	strcat( pszBuff, conv);

	// 사용 무기
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC6"));//사용 무기 : 조건 없음\n \n
	strcat( pszBuff, conv);
	*/

	// 특수 조건
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC2"));/*특수 조건\n*/

	if( _isAccmQuest())
		i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC15"), 256 );/*   - 횟수 누적 수행\n \n*/
	else
		i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC14"), 256 );/*   - 횟수 연속 수행\n \n*/
	strcat( pszBuff, conv);

	// 목적
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC7"));/*목적\n*/
	if( _getAction() != ACTION_ID_NA)
		sprintf_s( conv, "   - %s\n \n", getActionName( _getAction()));
	else
		i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC16"), 256 );/*   - 없음\n \n*/
	strcat( pszBuff, conv);

	// 횟수
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC1"));/*횟수\n*/

	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC17"), getArg(5), _getCount());/*   - %d/%d회*/
	strcat( pszBuff, conv);
}

void		CQuestBase_Map_Count_Accm_Team::OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed)
{
	char conv[256];

	*pszBuff = 0;

	// 위치
	if( _getStage() != STAGE_ID_NA)
	{
		char szTemp[256];
		getStageDisplayName(szTemp, _getStage());

		//	각 나라 어법상 알맞게 수정.
		if( GetCodePage() == I3_LANG_CODE_KOREAN)
		{
			

			if( _getAction() == ACTION_ID_STAGE_ENTER || _getAction() == ACTION_ID_STAGE_INTERCEPT)
			{
				

				sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC13"), szTemp);/*%s에 */
			}
			else
			{
				sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC12"), szTemp);/*%s에서 */
			}
		}
		else
		{
			i3::safe_string_copy( conv, szTemp, 256 );
		}

		strcat( pszBuff, conv);
	}

	// 목적
	sprintf_s( conv, "%s", getActionName( _getAction()));
	strcat( pszBuff, conv);

	if( bLineFeed)
	{
		strcat( pszBuff, "\n");
	}

	// 특수 조건
	if( _isAccmQuest())
		strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC11"));/*(누적) */
	else
		strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC10"));/*(연속) */

	// 횟수
	REAL32 rate;
	rate = (REAL32) getArg(5) / _getCount();

	COLOR col;

	getProgressColor( getArg(5), _getCount(), &col);
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC9"), col.r, col.g, col.b, col.a, getArg(5), _getCount());/*{col:%d,%d,%d,%d} %d/%d회{/col}*/
	strcat( pszBuff, conv);
}

// Quest의 진행 상태를 반환한다.
QUEST_STATE CQuestBase_Map_Count_Accm_Team::OnQueryState(void)
{
	if( getArg( 5) == 0)
	{
		return QST_NOT_STARTED;
	}
	else if( getArg( 5) == _getCount())
	{
		return QST_COMPLETED;
	}

	return QST_DOING;
}

bool	CQuestBase_Map_Count_Accm_Team::ForceComplete(void)
{
	setArg( 5, _getCount());

	return true;
}


