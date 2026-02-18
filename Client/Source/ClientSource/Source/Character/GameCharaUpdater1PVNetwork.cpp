#include "pch.h"
#include "GameCharaUpdater1PVNetwork.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "./Character/GameCharaActionControlDefine.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaMaterialContext.h"

I3_CLASS_INSTANCE( CGameCharaUpdater1PVNetwork, CGameCharaUpdater1PV);

CGameCharaUpdater1PVNetwork::CGameCharaUpdater1PVNetwork()
{
}

CGameCharaUpdater1PVNetwork::~CGameCharaUpdater1PVNetwork()
{
}

void CGameCharaUpdater1PVNetwork::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater1PV::OnUpdate( rDeltatime);

	if( m_pOwner == NULL)						return;
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate()  )	return;

	//	1인칭일 경우에 하체 aistate의경우는 호출되지 않으므로 필요한경우 강제 호출해주도록 하는 함수
	OnProcess1PVOnly( rDeltatime);

	getBoneContext()->UpdateAnim( rDeltatime);					// Animation의 Key 값을 적용 (각 Bone의 Cache Matrix에 저장)

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		// 총기 발사에 대한 Theta 계산은 네트워크에서는 하지 않아야 합니다.
		getBoneContext()->CalcThetaNetwork( rDeltatime);

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// 이하에서는 getSceneObject()의 Matrix는 변경되어선 안된다.
		// 만약 변경된다면, Bone 계산에서 변경된 만큼의 오차가 날 수 있다.
		//

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 들의 World-Space Matrix를 계산
		// 

		getBoneContext()->_CalcSpineBones();

		//	Rho 
		getBoneContext()->CalcRecoil( rDeltatime);					// 독립적
		getBoneContext()->CalcRho( rDeltatime);						// IK Bone ( IK Cache Matrix)
	}
	else
		ProcessDeathState( rDeltatime);

	getBoneContext()->TransformWorldSpace( rDeltatime);
	getMaterialContext()->DriveLightVolume( rDeltatime);
	getCameraContext()->OnUpdate( rDeltatime);				// Camera Update

	//	캐릭터 모델링 알파 처리
	getMaterialContext()->ProcessMaterial( rDeltatime);

	//	사용자의 HUD 화면 이펙트 
	//m_pOwner->ProcessScreenEffect( rDeltatime);

	if( g_pCharaManager->getCharaTask( m_pOwner->getCharaNetIndex()) == CHARA_TASK_RESPAWN)	//	조작 가능한 상태이면
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( ((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_BATTLE )
			OnProcessInvincible( rDeltatime);
	}
	else
	{	//	캐릭터가 죽었으면
		ProcessRespawn( rDeltatime);
	}
}


void CGameCharaUpdater1PVNetwork::OnLastUpdate( REAL32 rDeltatime)
{
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Send Network info
	if( g_pGameContext->IsP2PHost() )
		m_pOwner->WriteData_HP();
}

void CGameCharaUpdater1PVNetwork::OnProcessInvincible( REAL32 rDeltatime)
{
	//	리스폰 직후 무적시간을 계산해 준다.
	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( rInvincible > 0.f )
	{
		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);

		getMaterialContext()->SetBlendEnable( true);
	}		
	else if( rInvincible < 0.f )
	{	
		m_pOwner->setInvincibleTime( 0.f);

		getMaterialContext()->SetBlendEnable( false);
	}
}

void CGameCharaUpdater1PVNetwork::OnProcess1PVOnly( REAL32 rDeltatime)
{
	if( getActionContext()->isLower( CHARA_LOWER_JUMP) )
	{
		getAnimContext()->CallbackAnim_JumpStartRun( NULL, rDeltatime);
	}
	else if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
	{
		getAnimContext()->CallbackAnim_MoveRun( NULL, rDeltatime);
	}
}


