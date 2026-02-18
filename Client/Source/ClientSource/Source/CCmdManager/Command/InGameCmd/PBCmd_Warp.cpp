#include "pch.h"
#include "PBCmd_Warp.h"
#include "../../../Cheatkey_LimitPred.h"
#include "../../../Cheatkey_WarpHelpers.h"
#include "../../../GameEventHandler.h"
#include "../../../BattleHUD.h"

namespace Cheatkey
{
	//------------------------------------------------------------------------------------------
	// 워프 : A캐릭터(또는 그 이상의 캐릭터)를 B지점으로 이동
	//
	// ex) "/wp slot[0] nick[test01]" : 0번 캐릭터를 'test01' 캐릭터 위치로 이동
	//     "/wp slot[0] pos[x,y,z]"   : 0번 캐릭터를 (x,y,z) 위치로 이동
	//     "/wp slot[0] set[bomb-a]"  : 0번 캐릭터를 파일에 저장된 bomb-a 위치으로 이동
	//     "/wp nick[test02] nick[test01]" : 'test02' 캐릭터를 'test01' 캐릭터 위치로 이동
	//     "/wp nick[test02] pos[x,y,z]"   : 'test02' 캐릭터를 (x,y,z) 위치로 이동
	//     "/wp nick[test02] set[bomb-a]"  : 'test02' 캐릭터를 파일에 저장된 bomb-a 위치으로 이동
	//     "/wp slot[0,1,2] set[bomb-a]"  : 1, 2, 3 캐릭터를 파일에 저장된 bomb-a 위치으로 이동
	//-------------------------------------------------------------------------------------------
	bool Warp::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() < 2) return false;

		WarpHelpers::Parser_SearchMovementPlayer psmp;
		i3::string param_players(params[0]);
		if (psmp.operator()(param_players, m_players) == false) return false;

		WarpHelpers::Parser_SearchMovementDestination psmd;
		i3::string param_destination(params[1]);
		if (psmd.operator()(param_destination, m_destination) == false) return false;

		if (!i3Vector::isValid(&m_destination)) return false;

		return true;
	}

	bool Warp::_Execute()
	{
		QA_COMMAND_TELEPORT_DATA info;

		for (size_t i=0; i<m_players.size(); i++)
		{
			info._i32SlotIdx = m_players[i];

			info._ar32Pos[0] = m_destination.x;
			info._ar32Pos[1] = m_destination.y;
			info._ar32Pos[2] = m_destination.z;

			GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_TELEPORT_SOMEONE, &info);
		}

		return true;
	}

	ICommand* Warp::Creator()
	{
		return new Warp(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//-----------------------------------------------------------------------------------
	// 이동 위치 저장 : 좌표 입력 받아서
	//	ex) "/wpsave bomb-a, 1.0, 2.0, 3.0" : bomb-a 이름으로 (1.0, 2.0, 3.0) 좌표 저장
	//		"/wpsave bomb-a" : bomb-a 이름으로 현재 카메라 위치 저장
	//-----------------------------------------------------------------------------------
	bool Warp_SaveLocation::_ParseParams(const i3::vector<i3::string>& params)
	{
		m_locationName = params[0];

		if (params.size() == 1) // 카메라 위치
		{
			i3Vector::Copy(&m_pos, i3Matrix::GetPos(g_pCamera->getCamMatrix()));
		}
		else if (params.size() == 4) // 좌표 위치
		{
			m_pos.x = static_cast<REAL32>(::atof(params[1].c_str()));
			m_pos.y = static_cast<REAL32>(::atof(params[2].c_str()));
			m_pos.z = static_cast<REAL32>(::atof(params[3].c_str()));
		}

		if (!i3Vector::isValid(&m_pos)) return false;

		return true;
	}

	bool Warp_SaveLocation::_Execute()
	{
		WarpHelpers::WarpKey k;
		k.stageID = g_pFramework->GetCurrentStageIndex();
		k.locationName = m_locationName.c_str(); i3::to_upper(k.locationName);

		// 저장소 소멸 시 자동 저장되기 때문에 메모리에 저장만 시킴.
		WarpHelpers::WarpStorageInst().AddLocation(k, m_pos);

		if (CHud::i()->isEnableCheatkeyWarpList())	// 출력 갱신.
		{
			i3::shared_ptr<ICommand> cmdHide(Warp_HideLocations::Creator());
			(*cmdHide)(i3::vector<i3::string>());

			i3::shared_ptr<ICommand> cmdShow(Warp_ShowLocations::Creator());
			(*cmdShow)(i3::vector<i3::string>());
		}

		return true;	
	}

	ICommand* Warp_SaveLocation::Creator()
	{
		return new Warp_SaveLocation(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// 이동 위치 삭제
	// ex) "/wpdel bomb-a" : 현재 맵의 bomb-a 좌표 삭제
	//--------------------------------------------------
	bool Warp_DelLocation::_ParseParams(const i3::vector<i3::string>& params)
	{
		m_locationName = params[0];
		return true;
	}

	bool Warp_DelLocation::_Execute()
	{
		WarpHelpers::WarpKey k;
		k.stageID = g_pFramework->GetCurrentStageIndex();
		k.locationName = m_locationName.c_str();

		// 저장소 소멸 시 자동 저장되기 때문에 메모리에 저장만 시킴.
		WarpHelpers::WarpStorageInst().DelLocation(k);

		if (CHud::i()->isEnableCheatkeyWarpList())	// 출력 갱신.
		{
			i3::shared_ptr<ICommand> cmdHide(Warp_HideLocations::Creator());
			(*cmdHide)(i3::vector<i3::string>());

			i3::shared_ptr<ICommand> cmdShow(Warp_ShowLocations::Creator());
			(*cmdShow)(i3::vector<i3::string>());
		}

		return true;
	}

	ICommand* Warp_DelLocation::Creator()
	{
		return new Warp_DelLocation(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// 모든 캐릭터 워프
	// ex) "/warpall"
	//--------------------------------------------------
	bool WarpAll::_ParseParams(const i3::vector<i3::string>& params)
	{
		WarpHelpers::Parser_SearchMovementDestination psmd;
		i3::string param_destination(params[0]);
		if (psmd.operator()(param_destination, m_destination) == false) return false;

		if (!i3Vector::isValid(&m_destination)) return false;

		return true;
	}

	bool WarpAll::_Execute()
	{
		QA_COMMAND_TELEPORT_DATA info;

		for (size_t i=0; i<SLOT_MAX_COUNT; i++)
		{
			if (g_pCharaManager->getCharaByNetIdx(i) == nullptr) continue;

			info._i32SlotIdx = i;

			info._ar32Pos[0] = m_destination.x;
			info._ar32Pos[1] = m_destination.y;
			info._ar32Pos[2] = m_destination.z;

			GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_TELEPORT_SOMEONE, &info);
		}

		return true;
	}

	ICommand* WarpAll::Creator()
	{
		return new WarpAll(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// 현재 맵의 워프 가능한 위치 보여주기
	// ex) "/wpshow"
	//--------------------------------------------------
	bool Warp_ShowLocations::_Execute()
	{
		i3::vector<WarpHelpers::WarpKey> keys;
		WarpHelpers::WarpStorageInst().FillupAllKeysTo(keys);

		i3::string text("Cheatkey Warp Locations :\n\n");
		char tmp[256] = {0,};

		for (size_t i=0; i<keys.size(); i++)
		{
			VEC3D pos;
			WarpHelpers::WarpStorageInst().GetLocation(keys[i], pos);

			text += "["; text += keys[i].locationName; text += "]  ";

			sprintf_s(tmp, "%.1f   %.1f   %.1f \n", pos.x, pos.y, pos.z);

			text += tmp;

			memset(tmp, 0, 256);
		}

		i3::wstring wtext(text.begin(), text.end());
		CHud::i()->ShowCheatkeyWarpList(wtext);

		return true;
	}

	ICommand* Warp_ShowLocations::Creator()
	{
		return new Warp_ShowLocations(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// 현재 맵의 워프 가능한 위치 그만 보여주기
	// ex) "/wphide"
	//--------------------------------------------------
	bool Warp_HideLocations::_Execute()
	{
		CHud::i()->HideCheatkeyWarpList();

		return true;
	}

	ICommand* Warp_HideLocations::Creator()
	{
		return new Warp_HideLocations(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}
}