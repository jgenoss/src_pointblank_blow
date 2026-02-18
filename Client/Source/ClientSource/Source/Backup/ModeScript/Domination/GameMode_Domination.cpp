#include "pch.h"

#ifdef	DOMI_CORE

#include "GameMode_Domination.h"
#include "Domination_Intermission.h"
#include "Domi_Flow_Boss.h"
#include "Domi_Flow_Collection.h"
#include "Domi_Flow_Defense.h"
#include "Domi_Flow_Destroy.h"
#include "Domi_Flow_Survival.h"
#include "Domi_Flow_TowerDefense.h"

#include "UI/HUD/UIHUD_Domination_Manager.h"

#include "StageBattle.h"
#include "StageObject/GameControledObject.h"

#include "../GameStateMgr.h"
#include "../BattleGui.h"
#include "../../UI/UIBattleFrame.h"

#include "GameDropItemManager.h"
#include "DominationSkillSystem.h"

#include "./UI/UIBattlePopupBase.h"

#ifdef DOMI_FLOW
//=========================================
// ModeScript 데이터
//=========================================
namespace ModeScript
{
	const char* GetFlowName( INT32 val)
	{
		NEW_FLOW_TYPE e = static_cast<NEW_FLOW_TYPE>(val);
		switch(e)
		{
		case NF_INTERMISSION: ""; break;
		case NF_DEFENSE: ""; break;
		case NF_SURVIVAL: ""; break;
		case NF_DESTROY: ""; break;
		case NF_COLLECTION: ""; break;
		case NF_TOWERDEFENSE: ""; break;
		case NF_BOSS: ""; break;
		}

		return "";
	}

	const char*	GetFlowMessage( INT32 val)
	{
		return "";
	}
}


namespace Domination_Util
{
	CNaviMesh::CNaviMesh( const STAGE_ID id) : m_pMesh(NULL), m_pVertex(NULL), m_nVertexCount(0)
	{
		char stage_name[MAX_STRING_COUNT] = {0,};
		getStageCodeName( stage_name, id);

		char file_path[MAX_STRING_COUNT] = {0,};
		_snprintf( file_path, MAX_STRING_COUNT, "./World/%s/%s.ASE", stage_name, stage_name);

		m_pMesh = new CDomiStage();
		m_pMesh->LoadASEFile( file_path, static_cast<INT32>(getStageOrd(id)) );

		m_nVertexCount = m_pMesh->GetNeviMeshCount();
		if( m_nVertexCount > 0 )
		{
			m_pVertex	= new CUSTOM_VERTEX[m_nVertexCount];
			i3mem::FillZero( m_pVertex, sizeof(CUSTOM_VERTEX) * m_nVertexCount);

			for( INT32 i = 0; i < m_nVertexCount; i++)
			{
				stNMNode*	pNode	= m_pMesh->GetNeviMeshNode(i);

				if( pNode == NULL )
					continue;

				m_pVertex[i].Pos0	= pNode->m_v3Pos[0];
				m_pVertex[i].Pos1	= pNode->m_v3Pos[1];
				m_pVertex[i].Pos2	= pNode->m_v3Pos[2];

				float	r	= 0.5f / ((static_cast<INT32>(pNode->m_v3Center.x * 5) % 20) + 1) + 0.5f;
				float	g	= 0.5f / ((static_cast<INT32>(pNode->m_v3Center.z * 5) % 20) + 1) + 0.5f;

				m_pVertex[i].SetColor( D3DXCOLOR(r, g, 0.0f, 1.0f) );
			}
		}
	}
	CNaviMesh::~CNaviMesh()
	{
		if( m_pVertex) { delete[] m_pVertex; m_pVertex = NULL;}
		if( m_pMesh) { m_pMesh->Destroy_CDomiStage(); delete m_pMesh;}
	}
	INT16 CNaviMesh::GetNaviMeshIdx( VEC3D& v)
	{
		if( m_pMesh == NULL )
			return INVALID_IDX;

		if( m_pMesh->m_bAllocated == false )
			return INVALID_IDX;

		return m_pMesh->GetNMIdx( &v);
	}

	void RenderNaviMesh(i3Viewer* pViewer, void* pData)
	{
		i3RenderContext* pRender = pViewer->GetRenderContext();

		if( pRender == NULL )
			return;

		if( gmode::i() == NULL) return;

		du::CNaviMesh* pNaviMesh = gmode::i()->GetNaviMesh();
		if( pNaviMesh == NULL) return;

		INT32			nCount	= pNaviMesh->GetVertexCount();
		CUSTOM_VERTEX*	pVertex	= pNaviMesh->GetVertexList();

		if( nCount <= 0 || pVertex == NULL )
			return;

		I3G_FACE_CULL	mode	= pRender->GetFaceCullMode();

		pRender->SetRenderMode((I3G_RENDER_MODE)(INT32)pData);
		pRender->SetFaceCullMode(I3G_FACE_CULL_NONE);
		pRender->DrawPrimUP(I3G_PRIM_TRILIST, nCount, pVertex, sizeof(CUSTOM_VERTEX)/3);
		pRender->SetFaceCullMode( mode );
		pRender->SetRenderMode(I3G_RENDER_POLYGON);
	}
};

#endif


//=========================================
//=========================================
// gmode_domination Class
//=========================================

REAL32 gmode_domination::s_totalPlayTime[SLOT_MAX_COUNT] = {0.0f,};

gmode_domination::gmode_domination(void) : m_pNaviMesh(NULL), m_pHud( NULL),

#ifdef DOMI_ANIM_CAM
										   m_animCamRegisterDomi(NULL), 
#endif
										   m_firstBossBgmPlayed(FALSE)
{

}

gmode_domination::~gmode_domination(void)
{	
	if( m_pNaviMesh) { delete m_pNaviMesh; m_pNaviMesh = NULL;}

	I3_SAFE_RELEASE(tinst<CGameDropItemManager>());
	I3_SAFE_DELETE(tinst<CDominationSkillSystem>());

#ifdef DOMI_HUD
	I3_SAFE_RELEASE(tinst<UIHUDDominationMgr>());
#endif

#ifdef DOMI_ANIM_CAM
	// 애니메이션 카메라 이벤트 삭제
	delete m_animCamRegisterDomi; m_animCamRegisterDomi = 0;
#endif
}


//=========================================
// 생성 및 초기화 함수들
//=========================================
void gmode_domination::ProcessCreateStage( CStageBattle* pStage)
{
	gmode::ProcessCreateStage( pStage);

	g_pGameContext->SetWasTutorial(false);

	CGameMissionManager * pMissionManager = g_pGameContext->getMissionManager();
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);			// 디폴트가 NOLIMIT으로 되어있음..

	// Domination Hud
	tinst<UIHUDDominationMgr>()	= UIHUDDominationMgr::new_object();
	UIHUDDominationMgr::i()->Create( pStage);
	m_pHud	= UIHUDDominationMgr::i();

	// Domination Skill
	tinst<CDominationSkillSystem>() = new CDominationSkillSystem();
	tinst<CDominationSkillSystem>()->Initialize();

	// DropItem Manager
	tinst<CGameDropItemManager>() = CGameDropItemManager::new_object();
	CGameDropItemManager::i()->OnCreate();


	ROOM_INFO_BASIC room_info; 
	g_pGameContext->getMyRoomInfo( &room_info);
	m_pNaviMesh = new du::CNaviMesh( static_cast<STAGE_ID>(room_info._CurrentStageID));
}

void gmode_domination::InitialIzeStage(void)
{
	// gmode::InitialIzeStage() 에서 오브젝트를 Reset하기 때문에
	// 미리 레이어를 초기화해야 한다.
	InitLayer();

	gmode::InitialIzeStage();

	// 구버전 HUD를 사용하는 경우가 있기때문에 여기서 초기화해줘야 한다.
	m_pHud->Initialize();
}




//=========================================
// 레이어 및 오브젝트 관련 함수
//=========================================
// Layer 별로 Object들을 리스트에 담음
void gmode_domination::InitLayer(void)
{
}


//=========================================
// 이벤트 처리 함수들
//=========================================

// 게임 준비 - 리스폰, 오브젝트, 캐릭터등 초기화
// Loading End ->PreRoundStart
void gmode_domination::ProcessEventPreStartRound(void)
{
#ifdef DOMI_ANIM_CAM
	// 애니메이션 이벤트 카메라 등록
	if (!m_animCamRegisterDomi)
	{
		m_animCamRegisterDomi = new AnimCamRegisterDomi;
		m_animCamRegisterDomi->Register();
	}
#endif

	CGameDropItemManager::i()->Process_PreStartRound();
}

// 게임 시작
// PreRoundStart->Battle
void gmode_domination::ProcessEventRoundStart(void)
{	
	gstatemgr::i()->setStageState( GAMESTAGE_STATE_BATTLE);

	g_pSndMng->StartBGM("Sound/Domination/Domi_BGM_DeadTown.ogg", true);
}

// 게임 끝
// Battle->GameEnd
void gmode_domination::ProcessEventRoundEnd(void)
{
	g_pSndMng->StopBGM();

	tinst<CDominationSkillSystem>()->DeleteSkillShopHud();
	tinst<UIHUDDominationMgr>()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_CLEAR_INDICATIONS, -1);
}


void gmode_domination::ProcessEventDefault( INT32 nEvent, INT32 Arg)
{
	switch( nEvent)
	{
	case	EVENT_EVENT_ALARM :
		{
#ifdef DOMI_ANIM_CAM
			if (Arg >= 500)
				ProcessEventAnimationCamera(Arg);
#endif
		}
		
		break;

	case	EVENT_DEATH_BATTLE :	// 죽었을때

		_ProcessEventDeath( Arg);

		break;
		
	case	EVENT_RESPAWN_BATTLE :
		{//자신이 리스폰 됐을 경우에만 호출
			tinst<UIHUDDominationMgr>()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::revival);
			tinst<UIHUDDominationMgr>()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::annihilation);
		}
		break;
	}
}

#ifdef DOMI_ANIM_CAM
void gmode_domination::ProcessEventAnimationCamera(INT32 eventIdx)
{
	_LOG_("animation camera event index : %d\n", eventIdx);

	if (AnimCamRegisterDomi::ANIM_END <= eventIdx && eventIdx <= AnimCamRegisterDomi::ANIM_SCENEB_MUTANT_DEATH)
	{
		CGameCameraAnimated* aniCam = CGameCameraManager::i()->GetAnimationCamera();

		if (eventIdx == AnimCamRegisterDomi::ANIM_END)
		{
			aniCam->End();
			g_pSndMng->StartBGM("Sound/Domination/Domi_BGM_Boss_oneplay.ogg", false, false);
			m_firstBossBgmPlayed = TRUE;
		}
		else
		{
			aniCam->Start(m_animCamRegisterDomi->GetAnimKey(eventIdx));
			g_pSndMng->StopBGM();
		}
	}
}
#endif


//=========================================
// 업데이트 함수들
//=========================================
static const REAL32		AIRemoveTime	= 5.0f;

void gmode_domination::ProcessUpdate( RT_REAL32 rDeltaSec)
{
#ifdef DOMI_HUD
	m_pHud->OnUpdate( rDeltaSec);
#endif

	tinst<CDominationSkillSystem>()->Update( rDeltaSec);
	CGameDropItemManager::i()->OnUpdate( rDeltaSec);

	UpdateTotalPlayTime(rDeltaSec);

#ifdef DOMI_FLOW
	// 보스 등장할 때 ProcessEventAnimationCamera()에서 Domi_BGM_Boss_oneplay.ogg를 한 번만 플레이한 후
	// Domi_BGM_Boss.ogg를 반복 플레이한다. 기획 요구사항으로 산탄코드가 되었다.
	if (m_firstBossBgmPlayed)
	{
		static REAL32 bossSndTime = 0.0f;
		bossSndTime += rDeltaSec;

		REAL32 firstBossBgmPlayTime = 1.0f;
		if (bossSndTime > firstBossBgmPlayTime)
		{
			g_pSndMng->StopBGM();
			g_pSndMng->StartBGM("Sound/Domination/Domi_BGM_Boss.ogg", true);
			m_firstBossBgmPlayed = FALSE;
			bossSndTime = 0.0f;
		}
	}
#endif
}

void gmode_domination::OnUpdateEndGame( REAL32 rDeltaSec)
{
}

void gmode_domination::UpdateTotalPlayTime(REAL32 deltaSeconds)
{
	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if (g_pGameContext->IsPlayingSlot(i))
		{
			s_totalPlayTime[i] += deltaSeconds;
		}
	}
}

void gmode_domination::_ProcessEventDeath( INT32 Arg)
{
	INT32 nKillMessage  = g_pGameContext->getMessage(); 
	nKillMessage >>= 12;

	DEATH_ADD_INFO infoAdd;
	if (FALSE == g_pGameContext->PopDeathAddInfo(&infoAdd))
	{
		I3TRACE("  -- PopDeathAddInfo Buffer Empty In Domination..\n");
		return;
	}

	WEAPON_CLASS_TYPE eClassType	= (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_TYPE(infoAdd._KillWeaponFlag);
	UINT32 iWeaponNum				= GET_ITEM_FLAG_NUMBER(infoAdd._KillWeaponFlag);

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( eClassType, iWeaponNum);
	if( pWeaponInfo == NULL )
	{
		I3TRACE( GAME_STRING("STR_TBL_INGAME_HUD_ERROR_I3TRACE_WRONG_WEAPON_INFO") );/*Event_Death_Battle. 잘못된 무기 정보입니다.\n*/
	}

	DEATH_INFO_CHARA* pInfo = NULL;
	INT32 nDeathSupprot	= 0;
	INT32 nDeathType		= 0;
	INT32 nDeathIdx		= 0;

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();

	for( UINT32 cnt = 0; cnt < infoAdd._nDeathCount; ++cnt)
	{
		pInfo = &infoAdd._DeathCharaInfo[cnt];

		nDeathSupprot	= GET_DEATHINFO_ENEMY_TYPE( pInfo->_DeathInfo );
		nDeathType		= GET_DEATHINFO_DEATH_TYPE( pInfo->_DeathInfo );
		nDeathIdx		= GET_DEATHINFO_IDX_TYPE( pInfo->_DeathInfo );

		//Reserve Death 필요 ( idx로는 AI인지 User 인지알 수 없다.. )
		//GAMEINFO_CHARACTER * pCharaInfo = g_pGameContext->getGameInfo_Chara( nDeathIdx);
		//g_pCharaManager->ReserveDeath( nDeathIdx, nKillIdx,&pCharaInfo->_tNetData._tSufferData._vHitDir, (CHARA_DEATH_TYPE ) nDeathType);

		if( nDeathIdx == g_pGameContext->getMySlotIdx())
		{
			if( (pMissionMng->getRespawnMode() == GAME_RESPAWN_MODE_NOLIMIT) || (pMissionMng->getTotalRespawnCount() > pMissionMng->getRespawnCount( nDeathIdx)))
			{

#if defined( DEF_OBSERVER_MODE)		
				if( g_pGameContext->isObserverMe() == false)
#else
				if(g_pGameContext->IsGM_Observer() == false)
#endif
					gstatemgr::i()->setRespawnEnable( true);

				CBattleGui::i()->SetEnableGui(FALSE, GAME_GUI_RESPAWN);  

				UIBattleFrame* battle_frame = UIBattleFrame::i();
				if (battle_frame && g_pGameContext->IsTutorialMode() == false )
					battle_frame->SetDeath();
			}
			else
			{
				UIBattleFrame* battle_frame = UIBattleFrame::i();
				if (battle_frame)
					battle_frame->SetDeathDelay();
			}
		}
	}
}

#endif