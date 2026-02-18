#include "pch.h"
#include "GameMap.h"
#include "GameObject.h"
#include "StageBattle.h"
#include "ObjectManager.h"
#include "BattleHud.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"

#include <unordered_map>

void				ChangeMyRoomInfoStageID_ForNonNetworkDebug()
{
	/* MODE_NEW_CHEAK 없앨지 살려줄지 생각해보자 추후에.
	ISNETWORKRETURN(true)

	i3StageInfo * pStageInfo = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex() );

	STAGE_ID nStageID = STAGE_ID_TD_PORTAKABA;

	// 현재 Stage를 검색한다.
	for(INT32 i=STAGE_ORD_NA+1; i<STAGE_ORD_MAX; i++)
	{
		char szTemp[256];

		getStageCodeName(szTemp, i);
		if( i3::generic_is_iequal(szTemp, pStageInfo->GetName()) )
		{
			nStageID = getStageId((STAGE_ORDINAL_ID)i);

			// 네이밍이 동일한  데스매치 맵은 원래 폭파미션 또는 파괴미션으로 예외처리
			switch( nStageID)
			{
			case STAGE_ID_TD_DOWNTOWN:			nStageID = STAGE_ID_BB_DOWNTOWN;		break;
			case STAGE_ID_TD_LUXVILLE:			nStageID = STAGE_ID_BB_LUXVILLE;		break;
			case STAGE_ID_TD_BLOWCITY:			nStageID = STAGE_ID_BB_BLOWCITY;		break;
			case STAGE_ID_TD_STORMTUBE:			nStageID = STAGE_ID_BB_STORMTUBE;		break;
			case STAGE_ID_TD_GIRAN2:			nStageID = STAGE_ID_DS_GIRAN2;			break;
			case STAGE_ID_TD_BREAKDOWN:			nStageID = STAGE_ID_DS_BREAKDOWN;		break;
			case STAGE_ID_TD_HOLIDAY:			nStageID = STAGE_ID_EH_HOLIDAY;			break;
			case STAGE_ID_EH_MERRYWHITEVILLAGE:	nStageID = STAGE_ID_EH_MERRYWHITEVILLAGE;			break;
			}

			break;
		}
	}

	ROOM_INFO_BASIC	roomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);
	roomInfo._StageID = nStageID;
	MyRoomInfoContext::i()->setMyRoomInfo(&roomInfo);
	*/
}



namespace
{
	template<class B, class D>
	struct static_creator {
		static 	B*		call()
		{
			B* p = new D;
			MEMDUMP_NEW( p, sizeof(D));
			return p;
		}
	};

	class gmap_factory
	{
	public:

		typedef gmap* (*function_pointer)();
		typedef i3::unordered_map<STAGE_UID, function_pointer>		map_type;

		gmap_factory()
		{
			m_map.insert(map_type::value_type(STAGE_UID_BREAKDOWN, &static_creator<gmap, gmap_breakdown>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_HELISPOT, &static_creator<gmap, gmap_helispot>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_GIRAN, &static_creator<gmap, gmap_giran>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_GIRAN2, &static_creator<gmap, gmap_giran2>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_BLACKPANTHER, &static_creator<gmap, gmap_blackpanther>::call ));
//
			m_map.insert(map_type::value_type(STAGE_UID_PORT_AKABA, &static_creator<gmap, gmap_port_akaba>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_RED_ROCK, &static_creator<gmap, gmap_red_rock>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_LIBRARY, &static_creator<gmap, gmap_library>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_MSTATION, &static_creator<gmap, gmap_mstation>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_BURNINGHALL, &static_creator<gmap, gmap_burninghall>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_DOWNTOWN, &static_creator<gmap, gmap_downtown>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_LUXVILLE, &static_creator<gmap, gmap_luxville>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_OUTPOST, &static_creator<gmap, gmap_outpost>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_BLOWCITY, &static_creator<gmap, gmap_blowcity>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_STORMTUBE, &static_creator<gmap, gmap_stormtube>::call ));

			m_map.insert(map_type::value_type(STAGE_UID_SENTRYBASE, &static_creator<gmap, gmap_sentrybase>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_HOSPITAL, &static_creator<gmap, gmap_hospital>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_DOWNTOWN2, &static_creator<gmap, gmap_downtown2>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_SHOPPINGCENTER, &static_creator<gmap, gmap_shoppingcenter>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_SANDSTORM, &static_creator<gmap, gmap_sandstorm>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_BREEDING_NEST, &static_creator<gmap, gmap_breeding_nest>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_DUPTOWN, &static_creator<gmap, gmap_d_uptown>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_DINOBREAKDOWN, &static_creator<gmap, gmap_dino_breakdown>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_MERRYWHITEVILLAGE, &static_creator<gmap, gmap_runaway>::call ));
		}

		function_pointer	get_fun(STAGE_UID type) const 
		{ 
			map_type::const_iterator it = m_map.find(type);
			return (it != m_map.end()) ? it->second : &static_creator<gmap, gmap>::call;
		}

	private:
		map_type	m_map;		// MS의 unordered_map은 해쉬함수선정문제가 있고, 메모리할당 속도가 느린 것으로 보고되었지만..
		// 일단 진행하고 나중에 컨테이너의 네임스페이스를 교체할 예정..
	} g_factory;
	
}

void		create_gmap(STAGE_UID t)
{
	tinst<gmap>() = (*g_factory.get_fun(t))();
}

void		destroy_gmap()
{
	I3_SAFE_DELETE( tinst<gmap>());
}

void gmap::Process_InitObject(i3Object * pObj)
{
	STAGE_MODE StageType = MyRoomInfoContext::i()->getStage()->GetStageMode();

	if( ( StageType == STAGE_MODE_DESTROY) || (StageType == STAGE_MODE_DEFENCE) )
	{
		I3PRINTLOG(I3LOG_WARN,  "HUD no work.");
	}
}

void gmap_breakdown::Process_InitObject(i3Object * pObj)
{
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DESTROY))
	{
		CGameMissionManager * pMissionManager = CGameMissionManager::i();
		if (i3::same_of<CGameObjectHelicopter*>(pObj))
			pMissionManager->setRedObject(pObj);
		else if (i3::same_of<CGameObjectPowerGenerator*>(pObj))
			pMissionManager->setBlueObject(pObj);
	}
}

void	gmap_breakdown::Process_Event_RoundStart_MissionNoticeString_From_GameMode()
{
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DESTROY))
	{
		if (CHud::i() != nullptr)
		{
			if (BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE)
				CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_HELICOPTER"));
			else
				CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_GENERATOR"));
		}

		// 파괴 미션의 헬기 이팩트를 뿌려줍니다.
		CGameObjectManager::i()->EnableObject();
	}
}


void gmap_helispot::Process_InitObject(i3Object * pObj)
{
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
	{
		if( ((CGameObjectRespawnHelicopter*)pObj)->getTeamIdx() == 0)	
			pMissionManager->setRedObject( pObj);
		else															
			pMissionManager->setBlueObject( pObj);
	}
}

void	gmap_helispot::Process_Event_RoundStart_MissionNoticeString_From_GameMode()
{
	if(CHud::i() != nullptr)
	{
		if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE)
			CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_HELISPOT_RED"));
		else
			CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_HELISPOT_BLUE"));
	}
}

void gmap_giran_base::Process_InitObject(i3Object * pObj)
{
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	if( i3::same_of<CGameObjectWildHogCannon*>(pObj))
		pMissionManager->setRedObject( pObj);
	else if( i3::same_of<CGameObjectSiegeGolem*>(pObj))
		pMissionManager->setBlueObject( pObj);
}

void	gmap_giran_base::Process_Event_RoundStart_MissionNoticeString_From_GameMode()
{
	if(CHud::i() != nullptr)
	{
		if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE)
			CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_INGAME_HUD_MISSION_OBJECT_DESTROY_HOG_CANNON"));/*RED 진영의 와일드 호그 캐논을 파괴 하라!*/
		else
			CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_INGAME_HUD_MISSION_OBJECT_DESTROY_GOLEM"));/*BLUE 진영의 시즈 골렘을 파괴 하라!*/
	}

	// 각 오브젝트의 애니메이션 플레이
	CGameObjectManager::i()->EnableObject();
}

void gmap_blackpanther::Process_InitObject(i3Object * pObj)
{
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	if( i3::generic_is_iequal( pObj->GetName(), "normal_Tank") )			pMissionManager->setDefenceObj( 0, pObj);
	else if( i3::generic_is_iequal( pObj->GetName(), "armerd_Tank") )		pMissionManager->setDefenceObj( 1, pObj);
}

void	gmap_blackpanther::Process_Event_RoundStart_MissionNoticeString_From_GameMode()
{
	// Mission 목적을 표시합니다.
	if(CHud::i() != nullptr)
	{
		if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE)
			CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DEFENCE_HOLDON"));
		else
			CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DEFENCE_DESTROY"));
	}
}

void gmap_runaway::Process_InitObject(i3Object * pObj)
{
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	if( i3::same_of<CGameObjectEvent*>(pObj))
	{
		CGameObjectEvent* pEvent = (CGameObjectEvent*)pObj;
		if( pEvent->getEventType() == OBJ_EVENT_MISSION_TRIGGER)
			pMissionManager->setRunAwayObject( pObj);
	}
}

void	gmap_runaway::Process_Event_RoundStart_MissionNoticeString_From_GameMode()
{

}

void gmap_stormtube::Process_Event_ArmedBomb_Battle()
{
	//	InstallC4_InStormTube_At_B_Area() 구현을 옮김..
	// B 지역 이라면
	if(CGameMissionManager::i()->IsInstalledArea(1))
	{
		gmap_stormtube::Process_MoveRepairTrain();
	}
}

void	gmap_stormtube::Process_MoveRepairTrain()
{
	CStageBattle* s = g_pFramework->GetStageBattle();

	size_t iObjCount = s->GetStageObjectList()->size();

	for( size_t j = 0 ; j <iObjCount; j++)
	{
		i3Object * pObj = (*s->GetStageObjectList())[j];
		if( i3::same_of<CGameObjectStormTubeRepairTrain*>(pObj))
		{
			((CGameObjectStormTubeRepairTrain*)pObj)->SetMoveAnimSTRT();
		}
	}
}

void gmap_stormtube::ResetRepairTrain()
{
	CStageBattle* s = g_pFramework->GetStageBattle();
	size_t iObjCount = s->GetStageObjectList()->size();

	for( size_t j = 0 ; j <iObjCount; j++)
	{
		i3Object * pObj = (*s->GetStageObjectList())[j];

		if( i3::same_of<CGameObjectStormTubeRepairTrain*>(pObj))
		{
			((CGameObjectStormTubeRepairTrain*)pObj)->ResetObject();
		}
	}
}

void	gmap_outpost::Process_SetToOnObjectCamera()
{
	g_pFramework->SetVisionMaskingType( VE_NIGHT);
	g_pFramework->SetNightVisionEnable( true);
}

void 	gmap_destroy_base::Process_Event_RoundStart_MissionNoticeString_From_GameMode()
{
	I3PRINTLOG(I3LOG_WARN,  "HUD no work.");
}

void	gmap_defense_base::Process_Event_RoundStart_MissionNoticeString_From_GameMode()
{
	I3PRINTLOG(I3LOG_WARN,  "HUD no work.");
}
