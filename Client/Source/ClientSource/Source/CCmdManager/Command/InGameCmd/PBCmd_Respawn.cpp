#include "pch.h"
#include "PBCmd_Respawn.h"

#include "../TBoundChk.h"
#include "../../../Cheatkey_LimitPred.h"
#include "../../../Cheatkey_WarpHelpers.h"
#include "../../../Cheatkey_Misc.h"
#include "../../../GameMissionManager.h"
#include "../../../BattleSlotContext.h"
#include "../../../MyRoomInfoContext.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Cheatkey
{
	//--------------------------------------------------
	// 리스폰 시간 제어.
	//	ex) "/spawntime t[초], n[횟수]"
	//--------------------------------------------------
	RespawnWaitTimeCtrl::RespawnWaitTimeCtrl(ILimitPred* p) : ICommand(p)
	{
		m_waittime = m_numOfAvail = std::numeric_limits<REAL32>::quiet_NaN();
	} 

	bool RespawnWaitTimeCtrl::_ParseParams(const i3::vector<i3::string>& params)
	{
		for (size_t i=0; i<params.size(); i++)
		{
			i3::vector<i3::string> tokens;

			Tokenize(params[i], tokens, "[]<>()");
			if (tokens.size() < 2) return false; // "t[10]"일 경우 key는 t 값은 10으로 두 개의 토큰이 추출되어야 한다.

			const i3::string& key = tokens[0];
			if (::strcmp(key.c_str(), "T") == 0)
			{
				m_waittime = static_cast<REAL32>(::atof(tokens[1].c_str()));
			}
			else if (::strcmp(key.c_str(), "N") == 0)
			{
				m_numOfAvail = static_cast<REAL32>(::atof(tokens[1].c_str()));
			}
		}

		if (m_waittime == std::numeric_limits<REAL32>::quiet_NaN())
		{
			CGameMissionManager::i()->GetRespawnTime(MyRoomInfoContext::i()->getStage(), BattleSlotContext::i()->getMySlotIdx()); // 기존 리스폰 타임 적용.
		}
		
		if (m_numOfAvail== std::numeric_limits<REAL32>::quiet_NaN())
			m_numOfAvail = std::numeric_limits<REAL32>::max(); // 무한대 적용.

		if (!TBoundChk(m_waittime, 0.0f, std::numeric_limits<REAL32>::max())) return false;
		if (!TBoundChk(m_numOfAvail, 0.0f, std::numeric_limits<REAL32>::max())) return false;
		
		return true;
	}

	bool RespawnWaitTimeCtrl::_Execute()
	{
		CGameMissionManager::i()->setRespawnTime(m_waittime);
		CGameMissionManager::i()->setRespawnTime4vs4(m_waittime);

		return true;
	}

	ICommand* RespawnWaitTimeCtrl::Creator()
	{
		return new RespawnWaitTimeCtrl(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// 리스폰 위치 제어.
	//	ex) "/spawnpos x y z"
	//		"/spawnpos pos_name"
	//--------------------------------------------------
	bool RespawnPosCtrl::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() == 1) // 저장된 위치명 이용.
		{
			WarpHelpers::WarpKey k;
			k.stageID = g_pFramework->GetCurrentStageIndex();
			k.locationName = params[0].c_str();

			if (!WarpHelpers::WarpStorageInst().GetLocation(k, m_pos)) return false;
		}
		else if (params.size() == 3) // 좌표 x, y, z 입력
		{
			m_pos.x = static_cast<REAL32>(::atof(params[0].c_str()));
			m_pos.y = static_cast<REAL32>(::atof(params[1].c_str()));
			m_pos.z = static_cast<REAL32>(::atof(params[2].c_str()));
		}

		if (!i3Vector::isValid(&m_pos)) return false;

		return true;
	}

	bool RespawnPosCtrl::_Execute()
	{
		// 기존 리스폰 구조를 변경하는 게 어렵고 위험하단 생각이 들어 워프를 활용하는 방향으로.
		CGameCharaBase* localPlayer = 
			g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

		if (localPlayer)
		{
			char msg[64] = {0,};
			sprintf_s(msg, 64, "/warp slot[%d] pos[%f %f %f]", BattleSlotContext::i()->getMySlotIdx(), m_pos.x, m_pos.y, m_pos.z);
			g_pCharaManager->SetRespawnCheatkey(msg);
		}

		return true;
	}

	ICommand* RespawnPosCtrl::Creator()
	{
		return new RespawnPosCtrl(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}
}