#include "pch.h"
#include "CreateRoom_MainCD.h"

#include "MyRoomInfoContext.h"
#include "ClanGameContext.h"
#include "ChannelContext.h"
#include "../StageBattle/AIModeContext.h"

#include "UIUtil.h"
#include "ClientStageInfoUtil.h"

#include "TextSet.h"

//MAIN_CD======================================================================================================

CreateRoom_MainCD::CreateRoom_MainCD(i3UIScene * pBase)
{
	CreateRoom_Base::CreateRoom_Base(pBase);

	m_SubType = 0;
	m_bAiMode = false;
	m_bisCreateRoom = false;
}

CreateRoom_MainCD::~CreateRoom_MainCD()
{
	CreateRoom_Base::~CreateRoom_Base();
}

void		CreateRoom_MainCD::InitData(bool isCreateRoom)
{
	m_bisCreateRoom = isCreateRoom;

	m_GameWinList.clear();
	m_GameTimeList.clear();

	for (int idx = 0; idx < CD_E_MAX; idx++)
	{
		m_Conditons[idx].m_ncuridx = 0;
		m_Conditons[idx].m_wsconditon.clear();
	}
}

void		CreateRoom_MainCD::UpdateforStage(CSI_STAGE * stage, bool firstupdate)
{
	m_pStage = stage;

	if (m_bisCreateRoom)
	{
		if (stage->m_i32RuleIdx == MyRoomInfoContext::i()->getStage()->m_i32RuleIdx)
		{
			ROOM_INFO_ADD	roomAddInfo;
			MyRoomInfoContext::i()->getMyRoomInfoAdd(&roomAddInfo);
			m_SubType = roomAddInfo._SubType;
		}
		else
		{
			m_SubType = CStageInfoUtil::getMissionRoundType(*m_pStage);
		}
	}
	else
	{
		m_SubType = CStageInfoUtil::getMissionRoundType(*m_pStage);
	}

	SetWin();
	SetTime();
	SetPeople();

	if (m_pStage->IsAiMode())	SetAIModeCondition();
	else						m_bAiMode = false;

	MainCDUIUpdate();
}

bool		CreateRoom_MainCD::ConfimRoomInfo(ROOM_INFO_ALL & info)
{

	if (g_pFramework->GetClanContext()->InBattleTeam()
		&& CStageInfoUtil::GetSlotMode(*m_pStage) == STAGE_SLOT_MODE_4VS4
		&& ClanGameContext::i()->getMyMatchSquadInfo()->getMaxPerson() > 4)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MATCH_IMPOSIBLE_4VS4"));
		return false;
	}

	if (!m_bisCreateRoom)
		info._Basic._PersonPlayerMax = getMaxPeople();
	else if (info._Basic._PersonPlayerMax <= SLOT_MAX_COUNT)
	{
		// 레디룸 상태에서 변경할 경우 서버에 최대 인원수 정보를 무조건 보내줘야함 - 혜빈님 요청
		switch ( CStageInfoUtil::GetSlotMode(*m_pStage))
		{
		case STAGE_SLOT_MODE_4VS4:
			info._Basic._PersonPlayerMax = 8;
			break;
		case STAGE_SLOT_MODE_5VS5:
			info._Basic._PersonPlayerMax = 10;
			break;
		default:
			info._Basic._PersonPlayerMax = 16;
			break;
		}
	}
	else
		info._Basic._PersonPlayerMax = SLOT_MAX_COUNT;

	info._Add._SubType = m_SubType;

	if (m_bAiMode)
	{
		info._Basic._WeaponFlag = 0x0F;

		info._Add._NowRoundCount = 0;
		info._Add._NowIngTime = 0;
		info._Add._LockInterEnter = 0x00;	// 기본 가능함
		info._Add._LockTeamBalance = 2;	// 기본 밸런스 (2번째)

		AIModeContext::i()->StartLevel	  = m_Conditons[CD_E_AI_LEVEL].m_ncuridx + 1;
		AIModeContext::i()->NewEnemyCount = m_Conditons[CD_E_AI_CNT].m_ncuridx + 1;

		if (m_pStage->IsAiModeforAiType(AI_USE_CHALLENGE))
			info._Add._AIState = AI_MODE_STATE_ORIGINAL;
		else if (m_pStage->IsAiModeforAiType(AI_USE_DIEHARD))
			info._Add._AIState = AI_MODE_STATE_DIEHARD;
		else if (m_pStage->IsAiModeforAiType(AI_USE_MADNESS))
			info._Add._AIState = AI_MODE_STATE_MADNESS;

		if (m_pStage->IsAiModeforAiType(AI_USE_MADNESS)) AIModeContext::i()->NewEnemyCount = 8;
	}

	return true;
}

void		CreateRoom_MainCD::ChangeWinConditon(INT32 cur)
{
	UINT8 gameTime = HINIBBLE(m_SubType);
	UINT8 gameType = LONIBBLE(m_GameWinList[cur]);

	m_SubType = MAKEBYTE(gameTime, gameType);
}

void		CreateRoom_MainCD::ChangeTimeLimit(INT32 cur)
{
	UINT8 gameTime = LONIBBLE(m_GameTimeList[cur]);
	UINT8 gameType = LONIBBLE(m_SubType);

	m_SubType = MAKEBYTE(gameTime, gameType);
}

void		CreateRoom_MainCD::ChangeAiCnt(INT32 cur)
{
	m_Conditons[CD_E_AI_CNT].m_ncuridx = cur;
}

void		CreateRoom_MainCD::ChangeAiLevel(INT32 cur)
{
	m_Conditons[CD_E_AI_LEVEL].m_ncuridx = cur;
}

void		CreateRoom_MainCD::MainCDUIUpdate()
{
	LuaState* L = _CallLuaFunction("SetRoomState");

	if (nullptr != L)
	{
		i3Lua::PushBoolean(L, m_bisCreateRoom);
		_EndLuaFunction(L, 1);
	}

	if (!m_bAiMode)
	{

		L = _CallLuaFunction("SetConditions");
		if (L != nullptr)
		{
			i3Lua::PushStringUTF16To8(L, m_Conditons[CD_E_WIN].m_wsconditon);
			i3Lua::PushInteger(L, m_Conditons[CD_E_WIN].m_ncuridx);
			i3Lua::PushStringUTF16To8(L, m_Conditons[CD_E_TIME].m_wsconditon);
			i3Lua::PushInteger(L, m_Conditons[CD_E_TIME].m_ncuridx);
			_EndLuaFunction(L, 4);
		}

		L = _CallLuaFunction("SetPeopleComboInfo");
		if (L != nullptr)
		{
			i3Lua::PushStringUTF16To8(L, m_Conditons[CD_E_PEOPLE].m_wsconditon);
			i3Lua::PushInteger(L, m_Conditons[CD_E_PEOPLE].m_ncuridx);
			_EndLuaFunction(L, 2);
		}

		L = _CallLuaFunction("SetDefaultWindow");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, !m_bisCreateRoom);
			_EndLuaFunction(L, 1);
		}

	}
	else
	{
		L = _CallLuaFunction("SetAIConditions");
		i3Lua::PushStringUTF16To8(L, m_Conditons[CD_E_AI_CNT].m_wsconditon);
		i3Lua::PushInteger(L, m_Conditons[CD_E_AI_CNT].m_ncuridx);
		i3Lua::PushStringUTF16To8(L, m_Conditons[CD_E_TIME].m_wsconditon);
		i3Lua::PushInteger(L, m_Conditons[CD_E_TIME].m_ncuridx);
		i3Lua::PushStringUTF16To8(L, m_Conditons[CD_E_AI_LEVEL].m_wsconditon);
		i3Lua::PushInteger(L, m_Conditons[CD_E_AI_LEVEL].m_ncuridx);
		_EndLuaFunction(L, 6);

		L = _CallLuaFunction("SetAIModeWindow");
		_EndLuaFunction(L, 0);
	}
}

void		CreateRoom_MainCD::SetWin()
{
	// 시작/끝 인덱스 설정
	INT32 i;
	INT32 idxMin, idxMax, curCondition;
	i3::stack_wstring wstrCondition;

	UINT32 gameType = m_SubType & 0x0F;

	//win
	m_GameWinList.clear();
	{	// 라운드 또는 킬카운트 조건
		if (CStageInfoUtil::IsModeRoundType(*m_pStage))
		{
			if (CStageInfoUtil::IsFixedRoundTypeMode(*m_pStage))
			{
				i3::sprintf(wstrCondition, L"%s;", GAME_RCSTRING("STBL_IDX_MODE_TYPE1"));
				idxMax = idxMin = gameType;
			}
			else
			{
				idxMin = BATTLE_ROUND_TYPE_3;
				idxMax = BATTLE_ROUND_TYPE_9;		// 라운드전

				for (i = idxMin; i <= idxMax; i++)
				{
					m_GameWinList.push_back(UINT8(i));
					i3::generic_string_cat(wstrCondition, TextSet::Round(i));
					i3::generic_string_cat(wstrCondition, L";");
				}
			}
		}
		else
		{
			idxMin = BATTLE_KILL_TYPE_60;
			idxMax = BATTLE_KILL_TYPE_160;		// Kill전

			for (i = idxMin; i <= idxMax; i++)
			{
				m_GameWinList.push_back(UINT8(i));
				i3::generic_string_cat(wstrCondition, TextSet::KillCount(i));
				i3::generic_string_cat(wstrCondition, L";");
			}
		}
		curCondition = gameType - idxMin;
	}

	m_Conditons[CD_E_WIN].m_wsconditon = wstrCondition;
	m_Conditons[CD_E_WIN].m_ncuridx = curCondition;
}

void		CreateRoom_MainCD::SetTime()
{
	INT32 idxMin, idxMax, curTime;
	i3::stack_wstring wstrTimes;

	UINT32 gameTime = m_SubType & 0xF0;

	//time
	m_GameTimeList.clear();
	{
		getTimeMinMax(&idxMin, &idxMax);

		for (int i = idxMin; i <= idxMax; i++)
		{
			m_GameTimeList.push_back(UINT8(i));
			i3::generic_string_cat(wstrTimes, TextSet::GameTime(MAKEBYTE(i, 0)));
			i3::generic_string_cat(wstrTimes, L";");
		}

		curTime = HINIBBLE(gameTime) - idxMin;
		if (curTime < 0 ) curTime = 0;
	}

	m_Conditons[CD_E_TIME].m_wsconditon = wstrTimes;
	m_Conditons[CD_E_TIME].m_ncuridx = curTime;
}

void		CreateRoom_MainCD::SetPeople()
{
	m_Conditons[CD_E_PEOPLE].m_wsconditon.clear();
	INT32 nMaxPeople = 8;

	bool b4VS4Stage = false;
	bool b5VS5Stage = false;
	switch (CStageInfoUtil::GetSlotMode(*(m_pStage)))
	{
	case STAGE_SLOT_MODE_4VS4:
		b4VS4Stage = true;
		break;
	case STAGE_SLOT_MODE_5VS5:
		b5VS5Stage = true;
		break;
	default:
		break;
	}

	// 4 vs 4 맵, AI전의 경우는 4 vs 4 부터 설정이 가능하게 한다.
	if (b4VS4Stage || m_pStage->IsAiMode())
		nMaxPeople = 4;
	else if (b5VS5Stage)
		nMaxPeople = 5;

	i3::stack_wstring wstrTemp;
	for (INT32 nCount = nMaxPeople; nCount > 0; nCount--)
	{
		if (ChannelContext::i()->GetBattleChannel() && nCount < 4)
		{
			// 4VS4 스테이지, AI전이 아니면, Default Select = 5 VS 5로 설정
			if (b4VS4Stage || m_pStage->IsAiMode() || b5VS5Stage)
				m_Conditons[CD_E_PEOPLE].m_ncuridx = 0;
			else
				m_Conditons[CD_E_PEOPLE].m_ncuridx = 3;

			break;
		}

		i3::sprintf(wstrTemp, L"%d VS %d;", nCount, nCount);

		m_Conditons[CD_E_PEOPLE].m_wsconditon += wstrTemp;
	}
}

void		CreateRoom_MainCD::SetAIModeCondition()
{
	m_bAiMode = true;

	m_Conditons[CD_E_AI_LEVEL].m_wsconditon.clear();
	m_Conditons[CD_E_AI_CNT].m_wsconditon.clear();

	if (m_pStage->IsAiModeforAiType(AI_USE_MADNESS))
	{
		i3::generic_string_cat(m_Conditons[CD_E_AI_LEVEL].m_wsconditon, TextSet::AIModeLevel(1));
		i3::generic_string_cat(m_Conditons[CD_E_AI_LEVEL].m_wsconditon, L";");
		m_Conditons[CD_E_AI_LEVEL].m_ncuridx = 0;

		m_Conditons[CD_E_AI_CNT].m_wsconditon = L"8";
		m_Conditons[CD_E_AI_CNT].m_ncuridx = 0;
	}
	else
	{
		for (INT32 idx = 0; idx < CHALLENGE_DIFFICULTY_LEVEL_HIGH; idx++)
		{
			i3::generic_string_cat(m_Conditons[CD_E_AI_LEVEL].m_wsconditon, TextSet::AIModeLevel(idx + 1));
			i3::generic_string_cat(m_Conditons[CD_E_AI_LEVEL].m_wsconditon, L";");
			m_Conditons[CD_E_AI_LEVEL].m_ncuridx = 0;
		}

		m_Conditons[CD_E_AI_CNT].m_wsconditon = L"1;2;3;4;5;6;7;8";
		m_Conditons[CD_E_AI_CNT].m_ncuridx = 7;
	}
}

void		CreateRoom_MainCD::getTimeMinMax(INT32 * idxMin, INT32 * idxMax)
{
	switch (m_pStage->GetStageMode())
	{
	case STAGE_MODE_ANNIHILATION:
	case STAGE_MODE_BOMB:
	case STAGE_MODE_DESTROY:
	case STAGE_MODE_RUN_AWAY:
		*idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
		*idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_7);
		break;
	case STAGE_MODE_CONVOY:
		if (m_pStage->IsEventforEventType(STAGE_EVENT_SEASON))
		{
			*idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			*idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
		}
		else
		{
			*idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			*idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_7);
		}
		break;
	case STAGE_MODE_DEFENCE:
		*idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
		*idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
		break;

	case STAGE_MODE_ESCAPE:
		*idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_5);
		*idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_5);
		break;
	case STAGE_MODE_CROSSCOUNT:
		*idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
		*idxMax = HINIBBLE(BATTLE_TIME_TYPE_15);
		break;
	case STAGE_MODE_DEATHMATCH:
	default:
		// AI전 = 5~10분 , 기타 = 5~30분 설정
		if (m_pStage->IsAiModeforAiType(AI_USE_CHALLENGE) ||
			m_pStage->IsAiModeforAiType(AI_USE_DIEHARD))
		{
			*idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
			*idxMax = HINIBBLE(BATTLE_TIME_TYPE_10);
		}
		else if (m_pStage->IsAiModeforAiType(AI_USE_MADNESS))
		{
			*idxMin = HINIBBLE(BATTLE_TIME_TYPE_10);
			*idxMax = HINIBBLE(BATTLE_TIME_TYPE_10);
		}
		else
		{
			*idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
			*idxMax = HINIBBLE(BATTLE_TIME_TYPE_30);
		}
		break;
	}

	if ((m_pStage->IsHeadShotOnly())/*헤드헌터*/ ||
		(m_pStage->IsHudOff() && m_pStage->IsOneShotOneKill())/*카오스모드*/)
	{
		*idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
		*idxMax = HINIBBLE(BATTLE_TIME_TYPE_30);
	}
}

UINT8		CreateRoom_MainCD::getMaxPeople()
{
	INT8 nMaxPeople = 0;

	bool b4VS4Stage = false;
	bool b5VS5Stage = false;
	switch (CStageInfoUtil::GetSlotMode(*m_pStage))
	{
	case STAGE_SLOT_MODE_4VS4:
		b4VS4Stage = true;
		break;
	case STAGE_SLOT_MODE_5VS5:
		b5VS5Stage = true;
		break;
	default:
		break;
	}

	// 인원수
	LuaState*	L = _CallLuaFunction("getPeople");
	INT32 cur = UILua::ReturnLuaInteger(L, 0);

	switch (cur)
	{	// 8vs8;7vs7;6vs6;5vs5;4vs4;3vs32vs2;1vs1
	case 0: nMaxPeople = 16;	break;	// 8vs8
	case 1: nMaxPeople = 14;	break;	// 7vs7
	case 2: nMaxPeople = 12;	break;	// 6vs6
	case 3: nMaxPeople = 10;	break;	// 5vs5
	case 4: nMaxPeople = 8;		break;	// 4vs4
	case 5: nMaxPeople = 6;		break;	// 3vs3
	case 6: nMaxPeople = 4;		break;	// 2vs2
	case 7: nMaxPeople = 2;		break;	// 1vs1
	default: nMaxPeople = 16;	break;	// 8vs8
	}

	// 4 vs 4 맵, AI전의 경우는 최대 인원수가 8이기 때문에, -8 을 해준다
	// 단, 클랜 채널의 4 vs 4 맵은 최소/최대인원이 8이기 떄문에 강제로 8명으로 맞춤
	if (b4VS4Stage || m_pStage->IsAiMode() || b5VS5Stage)
	{
		INT8 subtractNumOfPeople = 0;

		if (b4VS4Stage || m_pStage->IsAiMode())
			subtractNumOfPeople = 8;
		else if (b5VS5Stage)
			subtractNumOfPeople = 6;

		nMaxPeople -= subtractNumOfPeople;
	}

	// 최대 인원수는 적어도 2명은 되어야 한다.
	return nMaxPeople >= 2 ? nMaxPeople : 2;
}