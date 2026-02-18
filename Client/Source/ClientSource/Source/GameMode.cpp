#include "pch.h"
#include "GameMode.h"
#include "GameMap.h"
#include "BattleHUD.h"
#include "StageBattle.h"
#include "TutorialMode.h"
#include "StageObject/GameControledObject.h"
#include "Weapon/WeaponItemID.h"

#define DEFENCE_MISSION_HURRYUP_TIME		30.f
#include "Mode/Domination/GameMode_Domination.h"

#include "./StageBattle/ScoreContext.h"
#include "./StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "TutorialContext.h"
#include "GameMissionManager.h"

#include "GameStateMgr.h"
#include "UI/UIHudManager.h"

namespace
{

template<class B, class D>
struct static_creator 
{
	static 	B*		call()
	{
		B* p = new D;
		MEMDUMP_NEW(p, sizeof(D));
		return p;
	}	
};

class gmode_factory
{
public:
	gmode_factory()
	{
		m_afun[STAGE_MODE_NA] = &static_creator<gmode, gmode_na>::call;
		m_afun[STAGE_MODE_DEATHMATCH] = &static_creator<gmode, gmode_deathmatch>::call;
		m_afun[STAGE_MODE_BOMB] = &static_creator<gmode, gmode_bomb>::call;
		m_afun[STAGE_MODE_DESTROY] = &static_creator<gmode, gmode_destroy>::call;
		m_afun[STAGE_MODE_ANNIHILATION] = &static_creator<gmode, gmode_annihilation>::call;
		m_afun[STAGE_MODE_DEFENCE] = &static_creator<gmode, gmode_defense>::call;
		m_afun[STAGE_MODE_ESCAPE] = &static_creator<gmode, gmode_escape>::call;
		m_afun[STAGE_MODE_TUTORIAL] = &static_creator<gmode, gmode_tutorial>::call;
		m_afun[STAGE_MODE_CROSSCOUNT] = &static_creator<gmode, gmode_cc>::call;
		m_afun[STAGE_MODE_CONVOY] = &static_creator<gmode, gmode_annihilation>::call;
		m_afun[STAGE_MODE_RUN_AWAY] = &static_creator<gmode, gmode_annihilation>::call;
	}

	typedef gmode* (*function_pointer)();

	function_pointer	get_fun(STAGE_MODE type) const { return m_afun[type]; }
private:

	function_pointer	m_afun[STAGE_MODE_MAX];
} g_factory;
	
}

void		create_gmode(STAGE_MODE t)
{
	tinst<gmode>() = (*g_factory.get_fun(t))();
}

void		destroy_gmode()
{
	I3_SAFE_DELETE(tinst<gmode>());
}

gmode::gmode() : m_pStage( nullptr)
{
	// 미니맵의 층구별 값을 초기화 해야 합니다. 2008.09.11
	// 스톰튜브에서 미니맵 폭판 마크가 안나오는 문제 수정
	//C4Context::i()->setBombFloorID( BOMB_AREA_A, 0);
	//C4Context::i()->setBombFloorID( BOMB_AREA_B, 0);
}

void gmode::ResetObjectStatus(void)
{
	
	StageObjectListPtr	pObjectList = m_pStage->GetStageObjectList();

	if( pObjectList == nullptr )
		return;

	I3TRACE( "Objects are Initialized...\n");
	for( size_t i = 0; i < pObjectList->size(); i++ )
	{
		i3Object * pObj	= pObjectList->at(i);

		// 이벤트를 초기 상태로 돌립니다.
		if( CGameMissionManager::i()->IsResetobj())
			pObj->OnChangeTime( 0.0f);

		if( i3::kind_of<CGameControledObject*>(pObj))
		{
			((CGameControledObject*)pObj)->ResetObject();
		}

		GAMEINFO_OBJECT * pInfo = BattleObjContext::i()->setGameInfo_Object( pObj->getNetIndex() );

		i3Vector::Copy( &pInfo->_tAssistData._vPostPos, pObj->GetPos() );
		I3_BOUNDCHK( pObj->getMaxHP(), 65536);
		pInfo->_tNetData.setHP( (UINT16) pObj->getMaxHP());
		pInfo->_tNetData.setPosition( pObj->GetPos() );
	}

	if( (MyRoomInfoContext::i()->getStage()->IsAiMode()) == true && 
		 MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_MIDNIGHT_ZONE )
	{
		for( INT32 i = 0; i < OBJ_TOTAL_COUNT; ++i)
		{
			GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object(i);	// 모든 오브젝트 데이터

			i3Object * pObj = pObjectInfo->_pObjectRef;
			if( pObj != nullptr)
				pObj->Damage( pObj->getMaxHP(), nullptr, nullptr);
		}
	}

	CGameMissionManager::i()->SetResetObjFlag( true);
}


gmode_include_bomb::gmode_include_bomb()
{
	//i3mem::FillZero(m_BombInfos, sizeof(m_BombInfos));
}

void	gmode_include_bomb::ProcessLoadProperties(i3RegKey* stage_key)
{
	// 폭파 스테이지 관련
	
	// 폭탄위치 [0->A지점 폭파위치], [1->B지점 폭파위치]
	//FIND_REG_DATA(stage_key, "BombPosition_A", &m_BombInfos[BOMB_AREA_A].Pos);
	//FIND_REG_DATA(stage_key, "BombPosition_B", &m_BombInfos[BOMB_AREA_B].Pos);
	
	// 미니맵에 표시되는 폭탄 위치의 층 (기획팀에서 툴에서 설정한 ID)
	//FIND_REG_DATA(stage_key, "BombFloorOnMiniMap_A", &m_BombInfos[BOMB_AREA_A].FloorID);
	//FIND_REG_DATA(stage_key, "BombFloorOnMiniMap_B", &m_BombInfos[BOMB_AREA_B].FloorID);
	
	//C4Context::i()->setBombAreaPos( BOMB_AREA_A, getBombPosition( BOMB_AREA_A) );	//	A 폭탄 위치 설정
	//C4Context::i()->setBombFloorID( BOMB_AREA_A, getBombFloorId( BOMB_AREA_A) );	//	미니맵에 표시되는 A 폭탄 위치의 층 설정

	//C4Context::i()->setBombAreaPos( BOMB_AREA_B, getBombPosition( BOMB_AREA_B) );	//	B 폭탄 위치 설정
	//C4Context::i()->setBombFloorID( BOMB_AREA_B, getBombFloorId( BOMB_AREA_B) );	//	미니맵에 표시되는 B 폭탄 위치의 층
	
	C4Context::i()->ClearAreaProps();

	BombInstallAreaProp propA;
	FIND_REG_DATA(stage_key, LS_STR("BombPosition_A"), &propA.pos);
	FIND_REG_DATA(stage_key, LS_STR("BombFloorOnMiniMap_A"), &propA.floor);
	if (propA.IsValid())
		C4Context::i()->InsertAreaProp(BOMB_AREA_A, propA);

	BombInstallAreaProp propB;
	FIND_REG_DATA(stage_key, LS_STR("BombPosition_B"), &propB.pos);
	FIND_REG_DATA(stage_key, LS_STR("BombFloorOnMiniMap_B"), &propB.floor);
	if (propB.IsValid())
		C4Context::i()->InsertAreaProp(BOMB_AREA_B, propB);
}


gmode_na::gmode_na()
{
	
}

void gmode_na::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);
}

/////////////////////////
gmode_deathmatch::gmode_deathmatch()
{

}

void gmode_deathmatch::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);

	m_nMadnessState = 0;
	m_fMadnessAmbientTime = 0.f;
	m_nMadnessAmbientPlayCount = 1;
	m_nMadness_Ambient_Seed = 0;
	for( int i = 0; i < MADNESS_AMBIENT_COUNT ; i++)
		m_bMadnessAmientTable[i] = false;
}

void gmode_deathmatch::ProcessUpdate(REAL32 fDeltaTime)
{
	//매드니스 모드는 데스매치에 ai전이므로 따로 스테이지 타입을 따지 않음. 따라 이런식으로 처리. 
	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
	{
		if( ( gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE))
		{
			bool send_message = false;
			if( m_nMadnessState == 0 && UserContext::i()->fRemainingBattleTime <= 490.f )
			{
				m_nMadnessState =1;
				send_message = true;
			}
			else if( m_nMadnessState == 1 && UserContext::i()->fRemainingBattleTime <= 310.f)
			{
				m_nMadnessState =2;
				send_message = true;
			}
			else if( m_nMadnessState == 2 && UserContext::i()->fRemainingBattleTime <= 190.f )
			{
				m_nMadnessState =3;
				send_message = true;
			}

			if( send_message )
			{
				HUD::instance()->SetParameter(HUD::MADNESSNOTICE, (INT32)HUD::MN_START);
				if (BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx())
					GameEventSender::i()->SetEvent(EVENT_CHANGE_DIFFICULTY_LEVEL);
				g_pSndMng->StartAnnounceMessage(GTA_TRAINING_LEVELUP);
			}
		}
		ProcessMadnessAmbient(fDeltaTime);
	}
}

void gmode_deathmatch::ProcessEventRoundStart(void)
{
	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
	{
		GSND_TYPE_ANNOUNCE announce = (GSND_TYPE_ANNOUNCE)0;
		announce = GTA_MADNESS_GIRLSCREAM;
		g_pSndMng->StartAnnounceMessage( announce );
	}
}

void gmode_deathmatch::ProcessMadnessAmbient(REAL32 fDeltaTime)
{
	m_nMadness_Ambient_Seed = i3Math::Rand()%MADNESS_AMBIENT_COUNT;
	m_fMadnessAmbientTime += fDeltaTime;

	if( (INT)(m_fMadnessAmbientTime > (15.0f * m_nMadnessAmbientPlayCount)) )
	{
		UINT32 Seed = (++m_nMadness_Ambient_Seed%(MADNESS_AMBIENT_COUNT));
		UINT32 announce = 0;

		announce = GTA_MADNESS_DROPWATER + Seed;
		g_pSndMng->StartAnnounceMessage( announce );
		m_bMadnessAmientTable[Seed] = true;
		m_nMadnessAmbientPlayCount++;
	}

	bool not_all_play_check = false;
	for( int i = 0; i< MADNESS_AMBIENT_COUNT; i++)
	{
		if(m_bMadnessAmientTable[i] == false)
		{
			not_all_play_check = true;
		}
	}
	if(!not_all_play_check && m_fMadnessAmbientTime >= 60.f)
	{
		for( int i = 0; i< MADNESS_AMBIENT_COUNT; i++)
			m_bMadnessAmientTable[i] = false;

		m_nMadnessAmbientPlayCount = 1;
		m_fMadnessAmbientTime = 0.0f;
		m_nMadness_Ambient_Seed = i3Math::Rand()%MADNESS_AMBIENT_COUNT;
	}
}
/////////////////////////

gmode_bomb::gmode_bomb()
{

}

void gmode_bomb::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);

	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_1LIFE);
}

void gmode_bomb::Process_Event_RoundStart_MissionNoticeString()
{
	if(CHud::i() != nullptr)
	{
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		if( myPlayer->getCharaInfo()->GetTeam() & CHARACTER_TEAM_BLUE)
			CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_HOLDON_OBJECT"));
		else
			CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_OBJECT"));
	}
}

void gmode_bomb::Process_Event_RoundEnd_MissionEndBombFire(bool& boutNormalEnd)
{
	if(CHud::i() != nullptr)
		CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_BOMB_MISSION_EXPLOSION"));

	WeaponBase * pWeapon = nullptr;
#if defined( USE_EVENT_SHUTTLEX)
	pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SHUTTLEX));
#else
	pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4) );
#endif

	if( pWeapon != nullptr)
	{
		I3ASSERT( i3::same_of<WeaponC4*>(pWeapon));
		WeaponC4 * pBomb = (WeaponC4 *) pWeapon;

		pBomb->Explosion();			//	꽝!!
	}
	else
	{
		// 난입한 사람은 폭탄이 설치되었는지 알 수 없습니다.
		I3PRINTF(GAME_STRING("STR_TBL_INGAME_HUD_I3PRINTF_C4_WARNING2"), __FILE__, __LINE__);/*난입으로 인하여 C4 설치 여부를 확인할 수 없습니다. - 담당자 백성민, %s(%d)\n*/
	}

	// 방어팀을 모두 죽입니다..잔인하군요..;
	VEC3D vDir;	i3Vector::Set( &vDir, 0.f, 0.f, 1.f );

	for(INT32 j = 0; j < g_pCharaManager->getCharaCount(); j++ )
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( j );
		I3ASSERT( pChara != nullptr );

		if( pChara->getCharaInfo()->GetTeam() & CHARACTER_TEAM_BLUE )
		{
			if( pChara->getCharaNetIndex() == -1 )
				continue;

			if( UserContext::i()->IsPlayingSlot( pChara->getCharaNetIndex()))
			{
				g_pCharaManager->DeathChara( pChara, pChara->getCharaNetIndex(), &vDir, CHARA_DEATH_F );
				UserContext::i()->Death[pChara->getCharaNetIndex()] = true;
			}
		}
	}

	g_pSndMng->StartAnnounceMessage( GTA_BOMB_EXPLODED);
	boutNormalEnd = false;		
}

void gmode_bomb::Process_Event_RoundEnd_IfNoBombFire_ResetBomb(MISSION_END_TYPE met)
{
	if (met != MISSION_END_BOMBFIRE)
	{
		WeaponBase * pWeapon = nullptr;
#if defined( USE_EVENT_SHUTTLEX)
		pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SHUTTLEX));
		if( pWeapon != nullptr)
		{
			pWeapon->Reset();
			g_pWeaponManager->PushDropWeaponToChara( pWeapon);
		}
#else
		pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
		if( pWeapon != nullptr)
		{
			I3ASSERT( i3::same_of<WeaponC4*>(pWeapon));
			WeaponC4 * pBomb = (WeaponC4 *) pWeapon;

			pBomb->ReturnToReady();//뤼셋
			g_pWeaponManager->PushDropWeaponToChara( pWeapon);

		}
#endif
	}
}


//////////////////////////////////////////////////////////////////////////



/////////////////////////

gmode_destroy::gmode_destroy()
{

}

void gmode_destroy::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);

	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);
}

void gmode_destroy::Process_Event_RoundStart_MissionNoticeString()
{
	// 파괴미션의 경우 KillCount를 초기화
	ScoreContext::i()->InitTeamKillCount();

	gmap::i()->Process_Event_RoundStart_MissionNoticeString_From_GameMode();

}

void gmode_destroy::Process_Event_RoundEnd_MissionEnd_ObjectExplosion(INT32 i32Winner)
{

	CGameMissionManager * pMissionManager = CGameMissionManager::i();

	if(pMissionManager  == nullptr)
		return;
	
	//Red Wim
	if (i32Winner == RS_RED_WIN)
	{
		pMissionManager->getDestructionData()->getBlueObject()->setCurrentHP(0.f);
		pMissionManager->getDestructionData()->getBlueObject()->setCurState(2);
	}
	//Blue Win
	else if (i32Winner == RS_BLUE_WIN)
	{
		pMissionManager->getDestructionData()->getRedObject()->setCurrentHP(0.f);
		pMissionManager->getDestructionData()->getRedObject()->setCurState(2);

	}

}


/////////////////////////
gmode_annihilation::gmode_annihilation()
{
	
}

void gmode_annihilation::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_1LIFE);

}

void gmode_annihilation::Process_Event_RoundStart_MissionNoticeString()
{
	if(CHud::i() != nullptr)
		CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STR_TBL_INGAME_HUD_MISSION_OBJECT_TEXT1"));/*모든 적을 섬멸하라!*/
}

/////////////////////////
gmode_defense::gmode_defense() : m_rRemainTimeForBGM(0.f)
{

}

void gmode_defense::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	m_rRemainTimeForBGM = 0.f;

	TutorialContext::i()->SetWasTutorial(false);
	CGameMissionManager * pMissionManager = CGameMissionManager::i(); 
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);
}

void gmode_defense::Process_Event_RoundStart_MissionNoticeString()
{
	// KillCount 초기화
	ScoreContext::i()->InitTeamKillCount();
	
	gmap::i()->Process_Event_RoundStart_MissionNoticeString_From_GameMode();
}

void gmode_defense::Process_Event_RoundEnd_MissionEnd_ObjectExplosion(INT32 i32Winner)
{

	//디펜스모드의 경우 해당 함수 호출시 팀에 관계없이 방어 오브젝트 모두 폭파
	CGameMissionManager * pMissionManager = CGameMissionManager::i();

	if(pMissionManager  == nullptr)
		return;

	pMissionManager->getDefenceObj(0)->setCurrentHP(0);
	pMissionManager->getDefenceObj(0)->setCurState(2);
	pMissionManager->getDefenceObj(1)->setCurrentHP(0);
	pMissionManager->getDefenceObj(1)->setCurState(2);

}

void gmode_defense::ProcessUpdate(REAL32 fDeltaTime)
{
	I3ASSERT( g_pSndMng!= nullptr);

	// Defence Mission에서 bgm on

	if( m_rRemainTimeForBGM > DEFENCE_MISSION_HURRYUP_TIME &&
		UserContext::i()->fRemainingBattleTime < DEFENCE_MISSION_HURRYUP_TIME)
	{
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		if( myPlayer != nullptr && (CHud::i()!=nullptr))
		{
			if( myPlayer->getCharaTeam() & CHARACTER_TEAM_BLUE)
			{	// 방어팀
				CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DEFENCE_LASTTIME"));
			}
			else
			{	// 공격팀
				CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DEFENCE_HURRYUPTIME"));
			}
		}
		g_pSndMng->StartBGM( "Sound/BackGround/Breakdown.ogg", true);
	}

	m_rRemainTimeForBGM = UserContext::i()->fRemainingBattleTime;
}

/////////////////////////
gmode_sudden_death::gmode_sudden_death()
{
	
}

void gmode_sudden_death::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);
	CGameMissionManager * pMissionManager = CGameMissionManager::i(); 
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_1LIFE);
}

void gmode_sudden_death::Process_Event_RoundStart_MissionNoticeString()
{
	if(CHud::i()!=nullptr)
		CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STR_TBL_INGAME_HUD_MISSION_OBJECT_TEXT1"));/*모든 적을 섬멸하라!*/
}

/////////////////////////
gmode_escape::gmode_escape()
{

}

void gmode_escape::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);
	CGameMissionManager * pMissionManager = CGameMissionManager::i(); 
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);

}

/////////////////////////
gmode_head_hunter::gmode_head_hunter()
{

}

void gmode_head_hunter::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);
	CGameMissionManager * pMissionManager = CGameMissionManager::i(); 
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);

}



/////////////////////////
gmode_head_killer::gmode_head_killer()
{
	
}

void gmode_head_killer::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	TutorialContext::i()->SetWasTutorial(false);
	CGameMissionManager * pMissionManager = CGameMissionManager::i(); 
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_1LIFE);
}

void gmode_head_killer::Process_Event_RoundStart_MissionNoticeString()
{
	if(CHud::i() != nullptr)
		CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STR_TBL_INGAME_HUD_MISSION_OBJECT_TEXT1"));/*모든 적을 섬멸하라!*/
}

/////////////////////////
gmode_tutorial::gmode_tutorial() : m_pTutorialMode(nullptr)
{
	
}


void gmode_tutorial::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	m_pTutorialMode = CTutorialMode::new_object();

	m_pTutorialMode->OnCreate(s);

	CGameMissionManager * pMissionManager = CGameMissionManager::i(); 
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);

	/*if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		C4Context::i()->setBombAreaPos( BOMB_AREA_A, getBombPosition( BOMB_AREA_A) );
		C4Context::i()->setBombFloorID( BOMB_AREA_A, getBombFloorId( BOMB_AREA_A) );
	}*/
}

void gmode_tutorial::ProcessOnFinish()
{
	m_pTutorialMode->OnFinish();
	I3_SAFE_RELEASE(m_pTutorialMode);
}

//
//  cc모드 추가.. (그외 맵,Ord 추가 된것 있으면 반영하는게 좋다..)
//

void gmode_cc::ProcessCreateStage(CStageBattle* s)
{
	gmode::ProcessCreateStage( s);

	CGameMissionManager * pMissionManager = CGameMissionManager::i(); 
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);
}