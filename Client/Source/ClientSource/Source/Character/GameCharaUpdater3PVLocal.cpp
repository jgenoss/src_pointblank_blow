#include "pch.h"
#include "GameCharaUpdater3PVLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater3PVLocal, CGameCharaUpdater3PV);

CGameCharaUpdater3PVLocal::CGameCharaUpdater3PVLocal()
{
}

CGameCharaUpdater3PVLocal::~CGameCharaUpdater3PVLocal()
{
}

void CGameCharaUpdater3PVLocal::OnFirstUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnFirstUpdate( rDeltatime);

	if( g_pFramework->IsBattleStage())
	{	
		// Local용 패킷 처리
		ProcessLocalFromRecvPacket();

		getMoveContext()->Move( rDeltatime);
		getMoveContext()->UpdateSpeedReduction( rDeltatime);
	}

	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate()  )	return;
	
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
		getBoneContext()->CalcTheta( rDeltatime);				// Position

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
		getBoneContext()->CalcRho3PV( rDeltatime);
	}

	getBoneContext()->TransformWorldSpace( rDeltatime);
}

void CGameCharaUpdater3PVLocal::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnUpdate( rDeltatime);
	//	Add code here
	if( m_pOwner == NULL)						return;
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate()  )	return;
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		ProcessDeathState( rDeltatime);
	}

	getMaterialContext()->DriveLightVolume( rDeltatime);
	getCameraContext()->OnUpdate( rDeltatime);

	//	캐릭터 모델링 알파 처리
	getMaterialContext()->ProcessMaterial( rDeltatime);

	//	사용자의 HUD 화면 이펙트 
	m_pOwner->ProcessScreenEffect( rDeltatime);

	if( g_pCharaManager->getCharaTask( m_pOwner->getCharaNetIndex()) == CHARA_TASK_RESPAWN)	//	조작 가능한 상태이면
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( ((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_BATTLE)
			OnProcessInvincible( rDeltatime);
		else
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEFAULT ) )
			{	
				m_pOwner->Cmd_Reset();

				m_pOwner->addCharaStateMask( CHARA_STATE_DEFAULT);
				m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
			}
		}
	}
	else
	{	//	캐릭터가 죽었으면
		ProcessRespawn( rDeltatime);
	}

	// RPG7 접근 사운드 플레이
	CheckGrazeSoundPlay();
}

void CGameCharaUpdater3PVLocal::OnLastUpdate( REAL32 rDeltatime)
{
	// Local의 데이터를 네트워크로 보냅니다.
	m_pOwner->WriteData_DefaultInfo();
}

void CGameCharaUpdater3PVLocal::OnProcessInvincible( REAL32 rDeltatime)
{
	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( g_pFramework->IsBattleStage() == false)
		return;

	//	리스폰 직후 무적시간을 계산해 준다.
	if( rInvincible > 0.0f)
	{
		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);
		
		//	리스폰이 된후 일정시간 지나면 키입력을 풀어준다. (무기 교체 GUI에서 키입력을 막았으므로)
		REAL32 InvTime = g_pGameContext->getMissionManager()->GetInvincibleTime( g_pGameContext->getMyRoomInfo()->_StageID);

		////-----------------	무적시간 +1sec
		//	사용자가 사용중이면 무적시간 증가. (자신 사용)
		if( m_pOwner && g_pGameContext->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_INVINCIBLE_TIME))
		{
			//	무적시간 증가
			InvTime += (REAL32) VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME;
		}

		if( rInvincible < InvTime - 0.5f)
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		getMaterialContext()->SetBlendEnable( true);
	}		
	else if( rInvincible < 0.0f)		
	{	
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		m_pOwner->setInvincibleTime( 0.f);

		getMaterialContext()->SetBlendEnable( false);
	}
}


