#include "pch.h"
#include "MCardQuest.h"

#include "MCardQuestInfo.h"

#include "MCardUtil.h"

#include "ClientStageInfoUtil.h"

MCardQuest::MCardQuest(const MCard* pCard, INT32 myIndex) : 
m_pParent(pCard), m_myIndex(myIndex), m_currCompleteCount_OffBattle(0), m_currCompleteCount_OnBattle(0), m_pCardQuestInfo(nullptr)
{

}

MCardQuest::~MCardQuest()
{
	//
}

void		MCardQuest::Activate(const MCardQuestInfo* pCardQuestInfo)
{
	m_pCardQuestInfo = pCardQuestInfo;
	m_currCompleteCount_OffBattle = 0;	
	m_currCompleteCount_OnBattle = 0;
}

void		MCardQuest::Deactivate()
{
	MCardQuest::Activate(nullptr);
}

void		MCardQuest::SetCompleteCount_OffBattle(INT32 completeCount)
{
	m_currCompleteCount_OffBattle = completeCount;
	m_currCompleteCount_OnBattle = completeCount;
}

void		MCardQuest::SetCompleteCount_OnBattle(INT32 completeCount) 
{ 
	m_currCompleteCount_OnBattle = completeCount;  
}

void	MCardQuest::CancelCompletion()
{
	m_currCompleteCount_OnBattle = m_currCompleteCount_OffBattle;
}

void	MCardQuest::ConfirmCompletion()
{
	m_currCompleteCount_OffBattle = m_currCompleteCount_OnBattle;
}

namespace
{
	enum QuestDispatchType
	{
		QuestDispatchType_Team,
		QuestDispatchType_Weapon_Accum,
		QuestDispatchType_Weapon_Cont,
	};

}

// 기존 v1.0 코드는 퀘스트 파생클래스 3개가 분기가 되있는데...분기가 훨씬 더 지저분하고 반복이 심해서..그냥 하나로 합친다.

i3::rc_wstring	MCardQuest::GetCurrHUDDesc(bool bLineFeed) const
{
	if (m_pCardQuestInfo == nullptr)
		return i3::rc_wstring();

	bool bTeamCond = ( m_pCardQuestInfo->m_Team == TEAM_BLUE || m_pCardQuestInfo->m_Team == TEAM_RED );
	bool bAccumCond = m_pCardQuestInfo->m_bAccum;
	bool bEnterIntercept = ( m_pCardQuestInfo->m_ActionID == ACTION_ID_STAGE_ENTER || m_pCardQuestInfo->m_ActionID == ACTION_ID_STAGE_INTERCEPT);
	bool bKillCond = ( m_pCardQuestInfo->m_ActionID == ACTION_ID_KILL_WEAPONCLASS ||
		m_pCardQuestInfo->m_ActionID == 	ACTION_ID_DOUBLE_KILL_WEAPONCLASS ||
		m_pCardQuestInfo->m_ActionID == 	ACTION_ID_TRIPLE_KILL_WEAPONCLASS );

	i3::stack_wstring wstrDesc;
	i3::stack_wstring wstrTemp;

	// 다음은 목적표시에서 무기조건, Accum조건, 킬 조건으로 된 중국버전 예외처리이다..
#if defined(LOCALE_CHINA)
	if (bTeamCond == false && bAccumCond == true && bKillCond == true)
	{
		wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13");/*(누적) */
		wstrDesc += GAME_RCSTRING("STR_HEADER_DO_TRIPLE_KILL_ENEMY_CN"); 

		i3::sprintf( wstrTemp, L"(%s)", g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType ) );
		wstrDesc += wstrTemp;
		wstrDesc += GAME_RCSTRING("STR_HEADER_DO_ENEMY_CN");

		i3::rc_wstring wstrActionName =	MCardUtil::FindMCardActionName(m_pCardQuestInfo->m_ActionID);

		wstrDesc += wstrActionName;
	}		
#else	// 비 중국 로케일..
	if (g_pEnvSet->IsBaseOnUIVersion_1_0())		// 2이면 1.0 / 1이면 1.1 / 0은 1.5
	{
		// 특수 조건
		if ( bTeamCond )
		{
			if (bAccumCond)
				wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC11");/*(누적) */
			else
				wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC10");/*(연속) */
		}
		else	// 팀 아니면 무기임..
		{
			if (bAccumCond)
				wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13");	/*(누적) */
			else
				wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC13");	/*(연속) */
		}
	}

	// 위치	
	if ( m_pCardQuestInfo->m_StageID != STAGE_ID_NA)		// 특정 맵..
	{
		CSI_MAP* map = CStageInfoUtil::GetMapData(m_pCardQuestInfo->m_StageID);
		if (map == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN, "CSI_MAP nullptr StageID =  %d", m_pCardQuestInfo->m_StageID);
		}
		else
		{
			i3::rc_wstring szDisplayMapName = map->getDisplayName();

			if (bTeamCond)
			{
				if (GetCodePage() == I3_LANG_CODE_KOREAN)		// 이 쪽 부분은 기존이 개판인데..현재 수정 여유가 없음.
				{
					if (bEnterIntercept)
						i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC13"), szDisplayMapName); /*%s에 */
					else
						i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC12"), szDisplayMapName); /*%s에서 */
				}
				else
				{
					wstrTemp = szDisplayMapName;
				}
			}
			else
				if (bAccumCond)		// 무기 누적..
				{
					if (GetCodePage() == I3_LANG_CODE_KOREAN)		// 이 쪽 부분은 기존이 개판인데..현재 수정 여유가 없음.
					{
						if (bEnterIntercept)
							i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC15"), szDisplayMapName); /*%s에 */
						else
							i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC14"), szDisplayMapName); /*%s에서 */
					}
					else
					{
						wstrTemp = szDisplayMapName;
					}
				}
				else	// 무기 연속..
				{
					if (bEnterIntercept)
						i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC15"), szDisplayMapName);/*%s에 */
					else
						i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC14"), szDisplayMapName);/*%s에서 */
				}

			wstrDesc += wstrTemp;
		}
	}

	// 무기의 경우 무기 조건 표시
	if ( bTeamCond == false && m_pCardQuestInfo->m_WeaponClassType != WEAPON_CLASS_UNKNOWN)
	{
		if ( m_pCardQuestInfo->m_WeaponNumber == 0 )		// 무기 넘버 표시 안하면 무기종 출력으로 대체 
		{
			if( GetCodePage() == I3_LANG_CODE_KOREAN)
			{
				if (bKillCond)
					i3::sprintf(wstrTemp, L"%s%s ", g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType), getWeaponClassPostfix_Do( m_pCardQuestInfo->m_WeaponClassType));
				else
					i3::sprintf(wstrTemp, L"%s%s ", g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType), getWeaponClassPostfix_Use( m_pCardQuestInfo->m_WeaponClassType));
			}
			else
			{

#if defined(LOCALE_CHINA)
				i3::sprintf( wstrTemp, L"%s (%s) ", GAME_RCSTRING("STR_HEADER_DO_USE"), g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType ));
#else
				i3::sprintf( wstrTemp, L"(%s) ", g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType));
#endif
			}
		}
		else	//  m_pCardQuest->m_WeaponNumber != 0
		{
			CWeaponInfo* pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( m_pCardQuestInfo->m_WeaponClassType, m_pCardQuestInfo->m_WeaponNumber);
			if (pWeaponInfo != nullptr)
			{
				i3::rc_wstring wstrTemp1;
				pWeaponInfo->GetWeaponName( wstrTemp1);

				//	각 나라 어법상 알맞게 수정.
				if( GetCodePage() == I3_LANG_CODE_KOREAN)
				{
					i3::rc_wstring wstrTemp2;
					if (m_pCardQuestInfo->m_ActionID == ACTION_ID_USE_WEAPON)
						pWeaponInfo->GetQuestPostfix_Do( wstrTemp2 );
					else
						pWeaponInfo->GetQuestPostfix_Use( wstrTemp2);

					i3::sprintf( wstrTemp, L"%s%s ", wstrTemp1, wstrTemp2);
				}
				else
				{
					i3::sprintf( wstrTemp, L"(%s) ", wstrTemp1);
				}
			}
			else	// pWeaponInfo == nullptr
			{
				wstrTemp = L"(Unknown) ";
			}

		}

		wstrDesc += wstrTemp;
	}

	// 목적 표시..
	if (bTeamCond)	// 팀 조건의 경우 간단함..
	{
		i3::rc_wstring wstrActionName = MCardUtil::FindMCardActionName( m_pCardQuestInfo->m_ActionID );
		wstrDesc += wstrActionName;
	}
	else // !bTeamCond	// 무기 조건의 경우 다소 코드가 지저분한 상태..
	{
#if defined ( LOCALE_THAILAND)		//태국
		i3::rc_wstring wstrActionName = MCardUtil::FindMCardActionName( m_pCardQuestInfo->m_ActionID );
		wstrDesc += wstrActionName;

		if ( bKillCond )
		{
			wstrDesc += L' ';
			if (bAccumCond)
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC6");		/*사용중인 */
			else
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC6");		/*사용중인 */
		}	

#else	// 일반 ( 그냥 태국과 반대로 찍는 것 같다.)
		if (bKillCond)
		{
			if (bAccumCond)
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC6");	/*사용중인 */
			else
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC6");	/*사용중인 */
		}
#if !defined (LOCALE_RUSSIA)
		wstrDesc += L' '; // 한칸띄기
#endif
		i3::rc_wstring wstrActionName = MCardUtil::FindMCardActionName( m_pCardQuestInfo->m_ActionID );
		wstrDesc += wstrActionName;
#endif
	}

#endif	// 비 중국 로케일..끝..

	if( bLineFeed)
		wstrDesc += L'\n'; // 한줄띄기

	// 1.1 UI 특수 조건.. (1.0쪽은 좀더 상단에 표시한다..)
	if (g_pEnvSet->m_nUIVersion == UFV_1_1)		// 2이면 1.0 / 1이면 1.1 / 0은 1.5
	{
		if (bTeamCond)
		{
			if (bAccumCond)
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC11");	/*(누적) */
			else
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC10");	/*(연속) */
		}
		else	// 무기 조건..
		{
			if (bAccumCond)
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13"); /*(누적) */
			else
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC13"); /*(연속) */
		}
	}
	//
	// 횟수 (전버전 공통)
	struct getProgressColor
	{
		void operator()( INT32 cur, INT32 maxCount, COLOR* out )
		{
			static UINT8 s_ColorTable[3][4] =
			{
				{	206, 206, 206, 255 },		// 시작도 안했을 때
				{	252, 255, 100, 255 },		// 진행 중일 때
				{	159, 252, 0, 255 }			// 완료했을 때
			};

			if( cur <= 0.0f)
			{
				out->r = s_ColorTable[0][0];
				out->g = s_ColorTable[0][1];
				out->b = s_ColorTable[0][2];
				out->a = s_ColorTable[0][3];
			}
			else if( cur == maxCount )
			{
				out->r = s_ColorTable[2][0];
				out->g = s_ColorTable[2][1];
				out->b = s_ColorTable[2][2];
				out->a = s_ColorTable[2][3];
			}
			else
			{
				out->r = s_ColorTable[1][0];
				out->g = s_ColorTable[1][1];
				out->b = s_ColorTable[1][2];
				out->a = s_ColorTable[1][3];
			}
		}
	};

	// 현재 완료 카운트 출력..
	{
		COLOR col;
		getProgressColor()( m_currCompleteCount_OnBattle, m_pCardQuestInfo->m_TargetCompleteCount, &col);

		i3::rc_wstring wstrFmtRate;
		if (bTeamCond)
			wstrFmtRate = GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC9");
		else
			if (bAccumCond)
				wstrFmtRate = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC12");
			else
				wstrFmtRate = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC12");

		i3::sprintf(wstrTemp, wstrFmtRate, col.r, col.g, col.b, col.a, m_currCompleteCount_OnBattle, m_pCardQuestInfo->m_TargetCompleteCount);

		wstrDesc += wstrTemp;
	}

	return i3::rc_wstring(wstrDesc);
}
