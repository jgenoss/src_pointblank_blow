#include "pch.h"
#include "GameCharaUpdater3PVNetwork.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater3PVNetwork, CGameCharaUpdater3PV);

CGameCharaUpdater3PVNetwork::CGameCharaUpdater3PVNetwork()
{
}

CGameCharaUpdater3PVNetwork::~CGameCharaUpdater3PVNetwork()
{
}

void CGameCharaUpdater3PVNetwork::OnFirstUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnFirstUpdate( rDeltatime);

	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate() )		return;

	// Animation과 World-Space Transform 사이에 LOD 계산이 달라질 수 있으며,
	// 그것은 Visual 상에 여러 문제가 될 수 있기 때문에
	// 수동으로 LOD를 계산시킨다.
	//	Zoom시 FOV 비율 계산
	REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();

	//	모델링 LOD 계산
	m_pOwner->calcLOD( m_pOwner->GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);

	getBoneContext()->UpdateAnim( rDeltatime);				// Animation의 Key 값을 적용 (각 Bone의 Cache Matrix에 저장)

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		// 총기 발사에 대한 Theta 계산은 네트워크에서는 하지 않아야 합니다.
		getBoneContext()->CalcThetaNetwork( rDeltatime);
		
		// 상, 하체 다 돌린다.
		getBoneContext()->CalcAimBone( rDeltatime);

		// 머리에 대한 IK
		
		getBoneContext()->CalcHeadBone( rDeltatime);
		
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// 이하에서는 getSceneObject()의 Matrix는 변경되어선 안된다.
		// 만약 변경된다면, Bone 계산에서 변경된 만큼의 오차가 날 수 있다.
		//

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 들의 World-Space Matrix를 계산
		// 

		getBoneContext()->_CalcSpineBones();

		//	Rho 
		getBoneContext()->CalcRecoil( rDeltatime);				// 독립적
		// 상체만 돌린다.
		getBoneContext()->CalcRho3PV( rDeltatime);
	}

	getBoneContext()->TransformWorldSpace( rDeltatime);
}

void CGameCharaUpdater3PVNetwork::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnUpdate( rDeltatime);
	//	Add code here
	if( m_pOwner == NULL)						return;
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate() )		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		ProcessDeathState( rDeltatime);

	getMaterialContext()->DriveLightVolume( rDeltatime);
	getCameraContext()->OnUpdate( rDeltatime);				// Camera update

	//	캐릭터 모델링 알파 처리
	getMaterialContext()->ProcessMaterial( rDeltatime);

	//	사용자의 HUD 화면 이펙트 
	//m_pOwner->ProcessScreenEffect( rDeltatime);

	if( g_pCharaManager->getCharaTask( m_pOwner->getCharaNetIndex()) == CHARA_TASK_RESPAWN)	//	조작 가능한 상태이면
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( ((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_BATTLE)
			OnProcessInvincible( rDeltatime);
	}
	else
	{	//	캐릭터가 죽었으면
		ProcessRespawn( rDeltatime);
	}
}

void CGameCharaUpdater3PVNetwork::OnLastUpdate( REAL32 rDeltatime)
{
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Send Network info
	if( g_pGameContext->IsP2PHost() )
		m_pOwner->WriteData_HP();

#if defined( I3_DEBUG)	//DebugProcess
	 DebugProcess( rDeltatime);
#endif
}

void CGameCharaUpdater3PVNetwork::OnProcessInvincible( REAL32 rDeltatime)
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



