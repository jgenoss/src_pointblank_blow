#include "pch.h"
#include "QuestBase_Map_Count_Weapon_Cont.h"
#include "QPI.h"
#include "GlobalVariables.h"
#include "i3Base/string/ext/snprintf.h"


I3_CLASS_INSTANCE( CQuestBase_Map_Count_Weapon_Cont);//, CQuestBase);

CQuestBase_Map_Count_Weapon_Cont::CQuestBase_Map_Count_Weapon_Cont(void)
{
	_setValidKillCount( 0);
}

CQuestBase_Map_Count_Weapon_Cont::~CQuestBase_Map_Count_Weapon_Cont(void)
{
}

bool		CQuestBase_Map_Count_Weapon_Cont::_checkCondition(void)
{
	if( _getStage() != STAGE_ID_NA)
	{
		STAGE_ID id1, id2;

		id1 = _getStage();
		id2 = QPI_GetStage();

		if (getStageOrd(id1) != getStageOrd(id2))
			return false;
	}


	if( _getWeaponClass() != WEAPON_CLASS_UNKNOWN)
	{
		if( QPI_GetWeaponClassType() != _getWeaponClass())
			return false;
	}

	if( _getWeaponNumber() != 0)
	{
		if( QPI_GetWeaponType() != _getWeaponNumber())
			return false;
	}

	return true;
}

//OnKill과 OnDeath, OnDamage에서 다른종류의 처리를 위해 필요
bool CQuestBase_Map_Count_Weapon_Cont::_checkCondition( WEAPON_CLASS_TYPE type, INT32 num)
{
	if( _getStage() != STAGE_ID_NA)
	{
		STAGE_ID id1, id2;

		id1 = _getStage();
		id2 = QPI_GetStage();

		if (getStageOrd(id1) != getStageOrd(id2))
			return false;
	}

	if( _getWeaponClass() != WEAPON_CLASS_UNKNOWN)
	{
		if( WEAPON_CLASS_CIC == type || WEAPON_CLASS_DUALHANDGUN == type)
		{
			if( _getWeaponNumber() == 0)
			{
				type = WEAPON_CLASS_HANDGUN;
			}
		}
		else if( WEAPON_CLASS_DUALKNIFE == type)
		{
			if( _getWeaponNumber() == 0)
			{
				type = WEAPON_CLASS_KNIFE;
			}
		}
		else if( WEAPON_CLASS_DUALSMG == type)
		{
			// Dual SMG 일 경우 도전과제에서 일반 SMG로 인식 하도록
			type = WEAPON_CLASS_SMG;
		}
		else
		{
			// 유탄으로 넘어오는 클래스는 발사한 부모 무기 정보로 변환 
			// 유탄으로 사살해도 원래 발사 무기로 사살한것 처럼 도전과제 완료해야 합니다. 
			// 넘오는 인자는 발사 부모 무기가 아닌 유탄 인덱스로 넘어오기 때문에 리턴할때는 원래 발사 무기 인덱스여야 도전과제 체크를 할 수 있다.

			INT32 cnt = g_pWeaponInfoDataBase->GetExtWeaponInfoCount();

			// 해당 익스텐션 무기 정보로 테이블을 검색
			for (INT32 i=0; i<cnt; i++)
			{
				ExtWeaponInfoTable * pTable = g_pWeaponInfoDataBase->GetExtWeaponInfoTable(i);

				if( type == pTable->ExtClass && num == pTable->ExtNumber)
				{
					type = pTable->OwnerClass;	// 원래 발사 부모 무기 인덱스로 전환
					num = pTable->OwnerNumber;	// 원래 발사 부모 무기 인덱스로 전환

					break;
				}
			}
		}

		if( type != _getWeaponClass())
			return false;
	}

	if( _getWeaponNumber() != 0)
	{
		// if( QPI_GetWeapon_DerivedFrom( type, num) != QPI_GetWeapon_DerivedFrom( type, (INT32)_getWeaponNumber()))
		if(num != (INT32)_getWeaponNumber())
			return false;
	}

	return true;
}

// 주인공으로 인해, 다른 캐릭터가 죽었을 때 호출된다.
// Kill Chara.에 대한 구체적인 정보는 KILL_INFO Structure에 저장되어 있다.
bool		CQuestBase_Map_Count_Weapon_Cont::OnKill( KILL_INFO * pKillInfo)
{
	switch( _getAction())
	{
		case ACTION_ID_HEADSHOT:
			{
				if( _checkCondition( pKillInfo->m_WeaponType, pKillInfo->m_Weapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;

				if( pKillInfo->m_HitPart != HIT_CHARA_PART_HEAD)
					return false;
			}break;
		case ACTION_ID_PIERCING:
			{
				if( _checkCondition( pKillInfo->m_WeaponType, pKillInfo->m_Weapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;

				if( pKillInfo->m_bPiercingShot == FALSE)
					return false;
			}break;
		case ACTION_ID_KILL :
		case ACTION_ID_DOUBLE_KILL:
		case ACTION_ID_TRIPLE_KILL:
			{
				if( _checkCondition( pKillInfo->m_WeaponType, pKillInfo->m_Weapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;
			}
			break;
		case ACTION_ID_SUICIDE :
			{
				if( _checkCondition( pKillInfo->m_WeaponType, pKillInfo->m_Weapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx != g_pGameContext->getMySlotIdx())
					return false;
			}
			break;
		case ACTION_ID_KILL_WEAPONCLASS:
		case ACTION_ID_DOUBLE_KILL_WEAPONCLASS:
		case ACTION_ID_TRIPLE_KILL_WEAPONCLASS:
			{
				if( _checkCondition( pKillInfo->m_TargetWeaponType, pKillInfo->m_TargetWeapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
						return false;
			}
			break;
		default :
			return false;
	}

	UINT8 validKillCnt = _getValidKillCount();

	_setValidKillCount( validKillCnt + 1);

	bool valid = false;
	switch( _getAction())
	{
		case ACTION_ID_DOUBLE_KILL:
		case ACTION_ID_DOUBLE_KILL_WEAPONCLASS:
			{
				if(( validKillCnt < 2) && (_getValidKillCount() >= 2))
				{
					valid = true;
				}
			}
			break;
		case ACTION_ID_TRIPLE_KILL:
		case ACTION_ID_TRIPLE_KILL_WEAPONCLASS:
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
			}
			break;
	}

	if( !valid )	return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// 다른 캐릭터에 의해 주인공 캐릭터가 죽임을 당했을 때에 호출된다.
// Kill Chara에 대한 구체적인 정보는 KILL_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Weapon_Cont::OnDeath( KILL_INFO * pDeathInfo)
{
	_setValidKillCount( 0);

	if( _getAction() != ACTION_ID_DEATH)
		return false;

	if( _checkCondition(pDeathInfo->m_WeaponType, pDeathInfo->m_Weapon) == false)
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
bool		CQuestBase_Map_Count_Weapon_Cont::OnDamage( DAMAGE_INFO * pDamageInfo)
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

	if( _checkCondition(pDamageInfo->m_WeaponType, pDamageInfo->m_Weapon) == false)
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
bool		CQuestBase_Map_Count_Weapon_Cont::OnStageStart( STAGE_INFO * pStage)
{
	_setValidKillCount( 0);

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
bool		CQuestBase_Map_Count_Weapon_Cont::OnStageEnd( STAGE_INFO * pStageInfo)
{
	//연속카운팅 퀘스트의 경우는 stageend에서 퀘스트미완료판단시 연속카운트를 0으로 만들어주어야한다. 
	//그것을 위한 판단용 -raja
	bool bRet = true;

	_setValidKillCount( 0);

	if( _checkCondition() == false)
		bRet = false;

	switch( _getAction())
	{
		//각 모드에 따른 승리판별법이 틀린관계로, 게임시스템에서 넘겨주는 승리정보를 그대로 사용- raja
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

	if( bRet )
	{
		UINT8 cnt = getArg( 5);

		cnt++;
		setArg( 5, cnt);

		if( cnt >= _getCount())
			return true;
	}
	
	//누적 수행퀘스트가 아닌 연속수행 퀘스트의 경우엔 완료되지 않은경우 카운트를 0으로 리셋
	setArg( 5, 0);

	return false;
}

// 새롭게 Round를 시작했을 때 호출된다.
// Deathmatch 등과 같이 Round제 Mission이 아닌경우에는 호출되지 않을 수 있다.
// 새로 시작하는 Round에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Weapon_Cont::OnRoundStart( STAGE_INFO * pStageInfo)
{
	_setValidKillCount( 0);

	return false;
}

// Round를 종료할 때 호출된다.
// Deathmatch 등과 같이 Round제 Mission이 아닌경우에는 호출되지 않을 수 있다.
// 새로 시작하는 Round에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Weapon_Cont::OnRoundEnd( STAGE_INFO * pStageInfo)
{
	_setValidKillCount( 0);

	return false;
}

// 주인공 캐릭터가 특정 동작을 취했을 때 호출된다.
// 동작에 대한 구체적인 정보는 ACTION_INFO Structure를 통해 전달된다.
bool		CQuestBase_Map_Count_Weapon_Cont::OnAction( ACTION_INFO * pAction)
{
	if( _getAction() != pAction->m_Action)
		return false;

	if( _getAction() == ACTION_ID_HEADSHOT || _getAction() == ACTION_ID_PIERCING)
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
void		CQuestBase_Map_Count_Weapon_Cont::OnQueryDesc( char * pszBuff, INT32 maxLen)
{
	char conv[256];
	i3::string strTemp;

	*pszBuff = 0;

	// 위치
	switch( _getStage())
	{
		case STAGE_ID_NA :		strTemp = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC4");/*조건 없음*/	break;
		default :						
		{
			char szTemp2[256];

			getStageDisplayName(szTemp2, _getStage());
			strTemp = szTemp2;	
		}
		break;
	}
	i3::snprintf( conv, 256, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC5"), strTemp);/*위치 : %s\n \n*/
	strcat( pszBuff, conv);
/*
	// 모드
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC11"));//모드      : 조건 없음\n \n
	strcat( pszBuff, conv);

	// 진영
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC3"));//진영      : 조건 없음\n \n
	strcat( pszBuff, conv);
*/
	// 사용 무기
	{
		sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC7"));/*사용 무기 : 조건 없음\n \n*/

		if(_getWeaponClass() != WEAPON_CLASS_UNKNOWN)
		{
			sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC9"), g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));/*사용 무기 : %s 계열 */

			if(_getWeaponNumber() != 0)
			{
				CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo( _getWeaponClass(), _getWeaponNumber());

				if( pInfo != NULL)
				{
					pInfo->GetWeaponName( strTemp);
					i3::snprintf( conv, 256, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC8"), strTemp);/*사용 무기 : %s*/
				}
			}

			strcat( conv, "\n \n");
		}

		strcat( pszBuff, conv);
	}

	// 특수 조건
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC2"));/*특수 조건\n*/

	i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC16"), 256 );/*   - 횟수 연속 수행\n \n*/
	strcat( pszBuff, conv);

	// 목적
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC10"));/*목적\n*/
	if( _getAction() != ACTION_ID_NA)
		sprintf_s( conv, "   - %s\n \n", getActionName( _getAction()));
	else
		i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC17"), 256 );/*   - 없음\n \n*/
	strcat( pszBuff, conv);

	// 횟수
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC1"));/*횟수\n*/

	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC18"), getArg(5), _getCount());/*   - %d/%d회*/
	strcat( pszBuff, conv);
}

void		CQuestBase_Map_Count_Weapon_Cont::OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed)
{
	char conv[256], temp2[ MAX_STRING_COUNT];
	i3::string strTemp;

	*pszBuff = 0;

	// 위치
	if( _getStage() != STAGE_ID_NA)
	{
		char szTemp[256];

		getStageDisplayName(szTemp, _getStage());
		if( _getAction() == ACTION_ID_STAGE_ENTER || _getAction() == ACTION_ID_STAGE_INTERCEPT)
		{
			sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC15"), szTemp);/*%s에 */
		}
		else
		{
			sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC14"), szTemp);/*%s에서 */
		}

		strcat( pszBuff, conv);
	}

	// 무기
	if(_getWeaponClass() != WEAPON_CLASS_UNKNOWN)
	{
		if( GetCodePage() == I3_LANG_CODE_KOREAN)
		{
			if( _getAction() == ACTION_ID_KILL_WEAPONCLASS ||
				_getAction() == ACTION_ID_DOUBLE_KILL_WEAPONCLASS ||
				_getAction() == ACTION_ID_TRIPLE_KILL_WEAPONCLASS)
			{
				sprintf_s( conv, "%s%s ", g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()), getWeaponClassPostfix_Do( _getWeaponClass()));
			}
			else
			{
				sprintf_s( conv, "%s%s ", g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()), getWeaponClassPostfix_Use( _getWeaponClass()));
			}
		}
		else
		{
			sprintf_s( conv, "(%s) ", g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));
		}

		if(_getWeaponNumber() != 0)
		{
			CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo( _getWeaponClass(), _getWeaponNumber());

			if( pInfo != NULL)
			{
				//	각 나라 어법상 알맞게 수정.
				if( GetCodePage() == I3_LANG_CODE_KOREAN)
				{
					if( _getAction() == ACTION_ID_USE_WEAPON)
					{
						pInfo->GetWeaponName( strTemp);
						pInfo->GetQuestPostfix_Do( temp2);
						i3::snprintf( conv, 256, "%s%s ", strTemp, temp2);
					}
					else
					{
						pInfo->GetWeaponName( strTemp);
						pInfo->GetQuestPostfix_Use( temp2);
						i3::snprintf( conv, 256, "%s%s ", strTemp, temp2);
					}
				}
				else
				{
					pInfo->GetWeaponName( strTemp);
					i3::snprintf( conv, 256, "(%s) ", strTemp);
				}
			}
		}

		strcat( pszBuff, conv);

		if( _getAction() == ACTION_ID_KILL_WEAPONCLASS ||
			_getAction() == ACTION_ID_DOUBLE_KILL_WEAPONCLASS ||
			_getAction() == ACTION_ID_TRIPLE_KILL_WEAPONCLASS)
		{
			strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC6"));/*사용중인 */
		}
	}

	// 목적
	sprintf_s( conv, "%s", getActionName( _getAction()));
	strcat( pszBuff, conv);

	if( bLineFeed)
	{
		strcat( pszBuff, "\n");
	}

	// 특수 조건
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC13"));/*(연속) */

	// 횟수
	REAL32 rate;
	rate = (REAL32) getArg(5) / _getCount();

	COLOR col;

	getProgressColor( getArg(5), _getCount(), &col);
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_COUNT_ETC12"), col.r, col.g, col.b, col.a, getArg(5), _getCount());/*{col:%d,%d,%d,%d} %d/%d회{/col}*/
	strcat( pszBuff, conv);
}

// Quest의 진행 상태를 반환한다.
QUEST_STATE CQuestBase_Map_Count_Weapon_Cont::OnQueryState(void)
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

bool	CQuestBase_Map_Count_Weapon_Cont::ForceComplete(void)
{
	setArg( 5, _getCount());

	return true;
}
