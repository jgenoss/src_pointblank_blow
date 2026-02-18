#include "pch.h"
#include "GameCharaUpdater1PVLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater1PVLocal, CGameCharaUpdater1PV);

CGameCharaUpdater1PVLocal::CGameCharaUpdater1PVLocal()
{
}

CGameCharaUpdater1PVLocal::~CGameCharaUpdater1PVLocal()
{
}

void CGameCharaUpdater1PVLocal::OnFirstUpdate( REAL32 rDeltatime)
{	
	// Local용 패킷 처리
	ProcessLocalFromRecvPacket();
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		return;

	// 이동 처리
	getMoveContext()->Move( rDeltatime);
	getMoveContext()->UpdateSpeedReduction( rDeltatime);

	getBoneContext()->UpdateAnim( rDeltatime);		// Animation의 Key 값을 적용 (각 Bone의 Cache Matrix에 저장)

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		getBoneContext()->CalcTheta( rDeltatime);
		
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// 이하에서는 getSceneObject()의 Matrix는 변경되어선 안된다.
		// 만약 변경된다면, Bone 계산에서 변경된 만큼의 오차가 날 수 있다.
		//
		//i3Matrix::Mul( m_pOwner->GetMatrix(), m_pOwner->getLocalMatrix(), m_pOwner->getIdentityMatrix());

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 들의 World-Space Matrix를 계산
		// 
		getBoneContext()->_CalcSpineBones();

		//	Rho 
		getBoneContext()->CalcRecoil( rDeltatime);		// 독립적
		getBoneContext()->CalcRho( rDeltatime);			// IK Bone ( IK Cache Matrix)
	}

	getBoneContext()->TransformWorldSpace( rDeltatime);
}

void CGameCharaUpdater1PVLocal::OnUpdate( REAL32 rDeltatime)
{	
	//	Add code here
	//	1인칭일 경우에 하체 aistate의경우는 호출되지 않으므로 필요한경우 강제 호출해주도록 하는 함수
	OnProcess1PVOnly( rDeltatime);
	
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

void CGameCharaUpdater1PVLocal::OnLastUpdate( REAL32 rDeltatime)
{	
	// Local의 데이터를 네트워크로 보냅니다.
	m_pOwner->WriteData_DefaultInfo();
}

void CGameCharaUpdater1PVLocal::OnProcessInvincible( REAL32 rDeltatime)
{
	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( g_pFramework->IsBattleStage() == false)
		return;
	
#if !defined( DEF_OBSERVER_MODE)
	if(g_pGameContext->IsGM_Observer()) 
			g_pPlayer->Suicide();
#endif			

	//	리스폰 직후 무적시간을 계산해 준다.
	if( rInvincible > 0.0f)
	{
		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);
		
		REAL32 InvTime = g_pGameContext->getMissionManager()->GetInvincibleTime( g_pGameContext->getMyRoomInfo()->_StageID);

		////-----------------	무적시간 +1sec
		//	사용자가 사용중이면 무적시간 증가. (자신 사용)
		if( m_pOwner && g_pGameContext->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_INVINCIBLE_TIME))
		{
			//	무적시간 증가
			InvTime += (REAL32) VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME;
		}

		//	리스폰이 된후 일정시간 지나면 키입력을 풀어준다. (무기 교체 GUI에서 키입력을 막았으므로)
		if( rInvincible < InvTime - 0.5f)
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		getMaterialContext()->SetBlendEnable( true);
	}		
	else if( rInvincible < 0.0f)		
	{	
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);

		getMaterialContext()->SetBlendEnable( false);
		
	

		if(rInvincible < -0.5f) {
			
			m_pOwner->setInvincibleTime( -0.5f);
		}
	}
}

void CGameCharaUpdater1PVLocal::OnProcess1PVOnly( REAL32 rDeltatime)
{
	if( getActionContext()->isLower( CHARA_LOWER_JUMP) ||
		!m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) || m_pOwner->isCharaStateMask( CHARA_STATE_JUMP))
	{
		getAnimContext()->CallbackAnim_JumpStartRun( NULL, rDeltatime);
	}
	else if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))
	{
		if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
		{
			getAnimContext()->CallbackAnim_MoveRun( NULL, rDeltatime);
		}
	}
}





