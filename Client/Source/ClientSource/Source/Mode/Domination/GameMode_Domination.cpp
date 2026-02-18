#include "pch.h"

#ifdef	DOMI_CORE

#include "GameMode_Domination.h"
#include "UI/HUD/UIHUD_Domination_Manager.h"

#include "StageBattle.h"
#include "StageObject/GameControledObject.h"

#include "../GameStateMgr.h"
#include "../BattleGui.h"
#include "../../UI/UIBattleFrame.h"

#include "GameDropItemManager.h"
#include "DominationSkillSystem.h"
#include "../../ui/hud/DomiRevivalController.h"
#include "../ObserverBattle.h"

#include "./UI/UIBattlePopupBase.h"

#include "DomiUserSkillMgr.h"
#include "DomiStageProperties.h"
#include "DomiSkillProperties.h"
#include "DomiSkillObjBulletinBoard.h"
#include "../../Weapon/WeaponItemID.h"

#include "../../UI/HUD/DomiRevivalController.h"

#include "../../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "TutorialContext.h"
#include "GameMissionManager.h"

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
		case NF_INTERMISSION: return "";
		case NF_DEFENSE: return "";
		case NF_SURVIVAL: return "";
		case NF_DESTROY: return "";
		case NF_COLLECTION: return "";
		case NF_TOWERDEFENSE: return "";
		case NF_BOSS: return "";
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
	CNaviMesh::CNaviMesh( const STAGE_ID id) : m_pMesh(nullptr), m_nVertexCount(0)
	{
		char stage_name[MAX_STRING_COUNT] = {0,};

		char file_path[MAX_STRING_COUNT] = {0,};
		_snprintf( file_path, MAX_STRING_COUNT, "./World/%s/%s.sam", stage_name, stage_name);

		m_pMesh = new CDomiStageCli();
		MEMDUMP_NEW( m_pMesh, sizeof(CDomiStageCli));

		m_pMesh->LoadSAM( file_path );

		m_nVertexCount = m_pMesh->GetNeviMeshCount();
		if( m_nVertexCount > 0 )
		{
			i3mem::FillZero( m_Vertex, sizeof(CUSTOM_VERTEX) * MAX_NAVIMESH_COUNT);

			for( INT32 i = 0; i < m_nVertexCount; i++)
			{
				ST_NM_NODE_SAM*	pNode	= m_pMesh->GetNeviMeshNode(i);

				if( pNode == nullptr )
					continue;

				m_Vertex[i].Pos0	= pNode->m_v3Pos[0];
				m_Vertex[i].Pos1	= pNode->m_v3Pos[1];
				m_Vertex[i].Pos2	= pNode->m_v3Pos[2];

				float	r	= 0.5f / ((static_cast<INT32>(pNode->m_v3Pos[0].x * 5) % 20) + 1) + 0.5f;
				float	g	= 0.5f / ((static_cast<INT32>(pNode->m_v3Pos[1].z * 5) % 20) + 1) + 0.5f;

				m_Vertex[i].SetColor( D3DXCOLOR(r, g, 0.0f, 1.0f) );
			}
		}
	}

	void CNaviMesh::SetNaviMeshAICollision( PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK* pData )
	{
		m_nVertexCount = pData->_i16Count;
		{
			INT32 i32Tmp = 0;
			for( INT32 i = pData->_i16BeginIdx; i <= pData->_i16EndIdx; i++)
			{
				i3Vector::Copy( &m_Vertex[i].Pos0, (VEC3D*)pData->_v3Polygon[i32Tmp][0] );
				i3Vector::Copy( &m_Vertex[i].Pos1, (VEC3D*)pData->_v3Polygon[i32Tmp][1] );
				i3Vector::Copy( &m_Vertex[i].Pos2, (VEC3D*)pData->_v3Polygon[i32Tmp][2] );
				i32Tmp++;

				float	r	= 0.5f / ((static_cast<INT32>(rand() * 5) % 20) + 1) + 0.5f;
				float	g	= 0.5f / ((static_cast<INT32>(rand() * 5) % 20) + 1) + 0.5f;

				m_Vertex[i].SetColor( D3DXCOLOR(r, g, 0.0f, 1.0f) );
			}
		}
	}

	CNaviMesh::~CNaviMesh()
	{
		if( m_pMesh) { m_pMesh->Destroy(); I3_SAFE_DELETE( m_pMesh);}
	}
// 	INT16 CNaviMesh::GetNaviMeshIdx( VEC3D& v)
// 	{
// 		if( m_pMesh == nullptr )
// 			return INVALID_IDX;
// 
// 		return m_pMesh->GetNMIdx( &v);
// 	}

	void RenderNaviMesh(i3Viewer* pViewer, void* pData)
	{
		i3RenderContext* pRender = pViewer->GetRenderContext();

		if( pRender == nullptr )
			return;

		if( gmode::i() == nullptr) return;

		du::CNaviMesh* pNaviMesh = gmode::i()->GetNaviMesh();
		if( pNaviMesh == nullptr) return;

		INT32			nCount	= pNaviMesh->GetVertexCount();
		CUSTOM_VERTEX*	pVertex	= pNaviMesh->GetVertexList();

		if( nCount <= 0 || pVertex == nullptr )
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
namespace inner
{
	UISecCount sec_count;		// 시간재는 유틸..
}

gmode_domination::gmode_domination(void) : m_pNaviMesh(nullptr), m_pHud( nullptr)
{
//	UI::insert_cb("WindowMinimized", this, &my_type::cb_window_minimized);			// 제압모드 안쓸테니..그냥 주석처리 합니다..
//	UI::insert_cb("WindowRestored", this, &my_type::cb_window_restored);			// 제압모드 안쓸테니..그냥 주석처리 합니다..

	inner::sec_count.SetReverse(true);
	m_SecCountCB.SetObj(this);
	inner::sec_count.SetCallback(&m_SecCountCB);
}

gmode_domination::~gmode_domination(void)
{	
//	UI::remove_cb(this);

	I3_SAFE_DELETE( m_pNaviMesh);
	I3_SAFE_RELEASE(tinst<CGameDropItemManager>());
	I3_SAFE_DELETE(tinst<CDominationSkillSystem>());
	I3_SAFE_RELEASE(tinst<UIHUDDominationMgr>());
	I3_SAFE_DELETE(tinst<DomiSkillObjBulletinBoard>());
}


//=========================================
// 생성 및 초기화 함수들
//=========================================
void gmode_domination::ProcessCreateStage( CStageBattle* pStage)
{
	gmode::ProcessCreateStage( pStage);

	TutorialContext::i()->SetWasTutorial(false);

	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	pMissionManager->setRespawnMode( GAME_RESPAWN_MODE_NOLIMIT);			// 디폴트가 NOLIMIT으로 되어있음..

	// Domination Hud
	tinst<UIHUDDominationMgr>()	= UIHUDDominationMgr::new_object();
	UIHUDDominationMgr::i()->Create( pStage);
	m_pHud	= UIHUDDominationMgr::i();

	// Domination Skill
	tinst<CDominationSkillSystem>() = new CDominationSkillSystem();
	MEMDUMP_NEW( tinst<CDominationSkillSystem>(), sizeof(CDominationSkillSystem));

	CDominationSkillSystem::i()->Initialize();

	// 스킬 오브젝트 사용 알림판
	tinst<DomiSkillObjBulletinBoard>() = new DomiSkillObjBulletinBoard();
	MEMDUMP_NEW( tinst<DomiSkillObjBulletinBoard>(), sizeof(DomiSkillObjBulletinBoard));

	// DropItem Manager
	tinst<CGameDropItemManager>() = CGameDropItemManager::new_object();
	CGameDropItemManager::i()->OnCreate();

	ROOM_INFO_BASIC room_info; 
	MyRoomInfoContext::i()->getMyRoomInfo( &room_info);
	m_pNaviMesh = new du::CNaviMesh( static_cast<STAGE_ID>(room_info._StageID));
	MEMDUMP_NEW( m_pNaviMesh, sizeof(du::CNaviMesh));
}

//#ifdef DOMI_UDP_OBJECT
namespace
{
	void InstallSupplyHP()
	{
		T_ItemID itemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SUPPLY_HP));
		INT32 beginIdx = 20;

		const domi::Locations* hpsupplyLocsPtr = 
			domi::StageProp::i()->GetLocations(DOMI_TYPE_HPSUPPLY);

		for (size_t i=0; i<hpsupplyLocsPtr->size(); i++)
		{
			const domi::LOCATION_TYPE& location =  hpsupplyLocsPtr->GetLocation(i);
			INT32 idx = beginIdx + i;

			bool success = CDominationSkillSystem::i()->Install(itemID, DOMI_TYPE_HPSUPPLY, idx, &(location.position),
				g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx()), location.rotation);

			if (success)
			{
				dss::CIdentity* supplyHP = CDominationSkillSystem::i()->Find(DOMI_TYPE_HPSUPPLY, idx);
				static_cast<dss::CServiceable*>(supplyHP)->SetState(DominationSkill::HP_RECOVERABLE);
			}
		}
	}

	void InstallSupplyAmmo()
	{
		T_ItemID itemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SUPPLY_AMMO));
		INT8 beginIdx = 25;

		const domi::Locations* ammosupplyLocsPtr = 
			domi::StageProp::i()->GetLocations(DOMI_TYPE_AMMOSUPPLY);

		for (size_t i=0; i<ammosupplyLocsPtr->size(); i++)
		{
			const domi::LOCATION_TYPE& location =  ammosupplyLocsPtr->GetLocation(i);
			INT8 idx = beginIdx + static_cast<INT8>(i);

			bool success = CDominationSkillSystem::i()->Install(itemID, DOMI_TYPE_AMMOSUPPLY, idx, &(location.position),
				g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx()), location.rotation);

			if (success)
			{
				dss::CIdentity* supplyAmmo = CDominationSkillSystem::i()->Find(DOMI_TYPE_AMMOSUPPLY, idx);
				static_cast<dss::CServiceable*>(supplyAmmo)->SetState(DominationSkill::AMMO_RECOVERABLE);
			}
		}
	}

	void InstallSkillShop()
	{
		T_ItemID itemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SKILLSHOP));
		INT8 beginIdx = 30;

		const domi::Locations* skillshopLocsPtr = 
			domi::StageProp::i()->GetLocations(DOMI_TYPE_SKILLSHOP);

		for (size_t i=0; i<skillshopLocsPtr->size(); i++)
		{
			const domi::LOCATION_TYPE& location =  skillshopLocsPtr->GetLocation(i);
			INT8 idx = beginIdx + static_cast<INT8>(i);

			bool success = CDominationSkillSystem::i()->Install(itemID, DOMI_TYPE_SKILLSHOP, idx, &(location.position),
				g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx()), location.rotation);

			if (success)
			{
				CDominationSkillSystem::i()->CreateSkillShopHud(idx);
				bool is_enable_skill_shop = UIBattleFrame::i()->IsOpenPopup(UBP_SKILL_SHOP);
				CDominationSkillSystem::i()->SetEnableSkillShopHud( true, is_enable_skill_shop);

				dss::CIdentity* skillshop = CDominationSkillSystem::i()->Find(DOMI_TYPE_SKILLSHOP, idx);
				static_cast<dss::CServiceable*>(skillshop)->SetState(DominationSkill::SHOP_OPEN);
			}
		}
	}
}
//#endif

void gmode_domination::InitialIzeStage(void)
{
	// gmode::InitialIzeStage() 에서 오브젝트를 Reset하기 때문에
	// 미리 레이어를 초기화해야 한다.
	InitLayer();

	gmode::InitialIzeStage();

	// 구버전 HUD를 사용하는 경우가 있기때문에 여기서 초기화해줘야 한다.
	m_pHud->Initialize();

	InstallSupplyHP();
	InstallSupplyAmmo();
	InstallSkillShop();
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
	CGameDropItemManager::i()->Process_PreStartRound();

#ifdef DOMI_NEW_FLOW
	UIBattleFrame* bf =	UIBattleFrame::i();
	bf->EnableHUD(UIHUD_MANAGER, true);
	bf->EnableHUD(UIHUD_INGAMECHATING, true);

	GameEventSender::i()->SetEvent( EVENT_STAGE_JUMP_ROUND_START);
#endif
}

// 게임 시작
// PreRoundStart->Battle
void gmode_domination::ProcessEventRoundStart(void)
{	
	gstatemgr::i()->setStageState( GAMESTAGE_STATE_BATTLE);
}

// 게임 끝
// Battle->GameEnd
void gmode_domination::ProcessEventRoundEnd(void)
{
	g_pSndMng->StopBGM();

	CDominationSkillSystem::i()->DeleteSkillShopHud();
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_CLEAR_INDICATIONS, tmp_uu::NONE);
}

void gmode_domination::ProcessEventRespawn( INT32 idx)
{
	if( idx == BattleSlotContext::i()->getMySlotIdx())
	{
		//sp UI 켜기.
		CUIHudDomi_Skill* ui_skill = (CUIHudDomi_Skill*)UIHUDDominationMgr::i()->GetHUD( HUD_DOMI_SKILL);
		ui_skill->VisibleGauge( true);
	}
	else
	{
		//전멸상태에서 누군가 리스폰 했을 경우
		/*if( observer_battle::i()->is_team_all_death())
			observer_battle::i()->init_observer( 0);
		else
			observer_battle::i()->RefreshObserver();*/

	}

}	

void gmode_domination::ProcessEventGiveUpBattle( INT32 idx)
{
	if( UserContext::i()->IsPlayingSlot( idx))
	{
		domi_state_info* info = domi_state_info::i();
		info->grade_state = DOMI_REWARD_GRADE_STATE_USER_LEAVE;
		info->grade_user_nick = BattleSlotContext::i()->getNickForSlot( idx);
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_GRADE);
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_USER_STATE, dus::USER_STATE_E_SYNC);
	}
}

//=========================================
// 업데이트 함수들
//=========================================
static const REAL32		AIRemoveTime	= 5.0f;

void gmode_domination::ProcessUpdate( REAL32 rDeltaSec)
{
	inner::sec_count.Update();

	m_pHud->OnUpdate( rDeltaSec);

	CDominationSkillSystem::i()->Update( rDeltaSec);
	CGameDropItemManager::i()->OnUpdate( rDeltaSec);
}

void gmode_domination::OnUpdateEndGame( REAL32 rDeltaSec)
{
}

void gmode_domination::ProcessLoadProperties(i3RegKey* stage_key)
{
#ifdef DOMI_STAGE_OBJ_DATA
	domi::StageProp::i()->LoadFromFile();
#endif
}

void gmode_domination::cb_window_minimized(void* arg1, void* arg2)
{
	enum { TIMER = 300, };
	inner::sec_count.SetBaseSec(TIMER);
	inner::sec_count.Start();
}

void gmode_domination::cb_window_restored(void* arg1, void* arg2)
{
	inner::sec_count.Stop();
}

void	gmode_domination::OnSecCount_Done(INT32 result_time)
{
	inner::sec_count.Stop();

	INT32 nArg = 0;
	GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &nArg);

	// 홀펀칭 관련하여 패킷이 안 올 수 있기 때문에 자신의 전투포기는 패킷을 안받아도 방으로 돌아간다.
	CGameFramework::JumpToReady();
}

#endif