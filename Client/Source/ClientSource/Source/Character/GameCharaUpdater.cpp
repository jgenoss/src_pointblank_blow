#include "pch.h"
#include "GameCharaUpdater.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"
#include "StageBattle.h"
#include "GameStateMgr.h"

#include "Fullbody/GameCharaFullbody.h"

#include "./StageBattle/UserContext.h"

#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

#include "MainWeapon_RLShell.h"
//리스폰시 UIIngameChat활성화 되어있으면 캐릭터이동막기  16.11.01 김민제
#include "UI/UIHUDIngameChat.h"		
#include "UI/UIBattleFrame.h"		


#define PHYSIX_CHARA_SLEEP_TIME		4.f
#define CAMERA_CHARA_SLEEP_TIME		0.6f
#define PHYSIX_CHARA_SHAPE_EPS_POS	0.1f

//#define PHYSIX_CHARA_RECALC_POS

I3_CLASS_INSTANCE( CGameCharaUpdater);

// UDP 데이터를 처리합니다.
void CGameCharaUpdater::ProcessLocalFromRecvPacket()
{	
	if( BattleServerContext::i()->IsP2PHost() )		return;

	INT32	idx = BattleSlotContext::i()->getMySlotIdx();
	if( idx == -1 ) return;

	GAMEINFO_CHARACTER * pCharaInfo	= BattleSlotContext::i()->setGameInfo_Chara( idx );

	if( !pCharaInfo->_bRecvFlag )			return;

	GAMEINFO_SUFFER * pSufferInfo	= &pCharaInfo->_tNetData._tSufferData;

	UINT32 iInfoFlag = pCharaInfo->_tNetData._iInfoFlag;

	UINT16 uNetHP = pCharaInfo->_tNetData._tHP.getHP();

	if( iInfoFlag & P2P_INFO_FLAG_CS_DEATH)
	{
		if( (uNetHP == 0) &&
			!m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) &&
			(pSufferInfo->getDeathType() != CHARA_DEATH_UN) &&
			!m_pOwner->isInvincible() )
			//!UserContext::i()->Death[idx] )
		{	// 자신이 죽은 경우
			if( pSufferInfo->getDeathType() == CHARA_DEATH_B || pSufferInfo->getDeathType() == CHARA_DEATH_O )
			{
				VEC3D vTemp;
				i3Vector::Scale( &vTemp, pSufferInfo->getHitDirection(), -1.f );
				pSufferInfo->setHitDirection( &vTemp);
			}

			g_pCharaManager->DeathChara( m_pOwner, pSufferInfo->getShooterIndex(),
				const_cast<VEC3D*>(pSufferInfo->getHitDirection()),
				(CHARA_DEATH_TYPE) pSufferInfo->getDeathType(), (CHARA_HIT_PART) pSufferInfo->getHitPart() );
			
			UserContext::i()->Death[idx] = true;
			pSufferInfo->Reset();
		}
		pCharaInfo->_tNetData._iInfoFlag &= ~P2P_INFO_FLAG_CS_DEATH;
	}

	if( iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
	{
		// 여기는 수류탄 및 오브젝트 파괴에 의한 데미지만 들어옵니다.
		if( (pSufferInfo->getHitPart() != CHARA_HIT_UNKNOWN) &&
			m_pOwner->getCharaInfo()->GetHP() != uNetHP )  // hp를 부동소수점으로 바꾸면서 이 부분이 문제가 될 소지가 있음 - 박명진
		{	// 자신이 맞은 경우
			m_pOwner->Cmd_Only_DamageAction( (CHARA_HIT_PART) pSufferInfo->getHitPart(),
				const_cast<VEC3D*>(pSufferInfo->getHitDirection()) );
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_THROW, 0, m_pOwner->GetPos(), true);
			pSufferInfo->Reset();
		}

		pCharaInfo->_tNetData._iInfoFlag &= ~P2P_INFO_FLAG_CS_SUFFERING;
	}

	
	if( !m_pOwner->isInvincible() )
		m_pOwner->setCharaInfo()->SetHP((REAL32)uNetHP);
	pCharaInfo->_bRecvFlag				= false;
}

void CGameCharaUpdater::ProcessRespawn( REAL32 rDeltatime)
{
	// Owner 캐릭터는 죽은 후 KillCam 모델 안보이게
	if( m_pOwner->is1PV() && i3::same_of<CGameCamera_Kill*>(g_pCamera))
	{
		CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
		
		if (pKillCam->isFPS())
		{
			m_pOwner->Cmd_SetColor( nullptr, nullptr, 0.f);
		}
	}

	//	 리스폰 타이머 체크	
	REAL32 rRespawnElapseTime = m_pOwner->getRespawnElapseTime();

	if( rRespawnElapseTime > 0.0f)
	{	
		getMaterialContext()->SetBlendEnable( true);

		rRespawnElapseTime -= rDeltatime;
		m_pOwner->setRespawnElapseTime( rRespawnElapseTime);

		//	리스폰이 된다.
		if( rRespawnElapseTime <= 0.0f)
		{
			getMaterialContext()->SetBlendEnable( false);
			m_pOwner->setRespawnElapseTime( 0.f);
			
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			if( g_pConfig != nullptr && !g_pConfig->m_bNetworking)
			{	
				const NET_CHARA_INFO * pMyCharaInfo = BattleSlotContext::i()->getNetCharaInfo( BattleSlotContext::i()->getMySlotIdx() );
				NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( m_pOwner->getCharaNetIndex());

				//	현재 주인공이 가진 무기를 그대로 장비시킨다.
				pNetCharaInfo->SetWeaponList( pMyCharaInfo->getWeaponList( WEAPON_SLOT_PRIMARY));
			
				if( m_pOwner->getCharaInfo()->GetAIToClient())
				{
					//util_battle::AddAiChara( m_pOwner->getCharaNetIndex());
					//g_pCharaManager->RespawnCharaForAI( m_pOwner->getCharaNetIndex() );
				}
				else
				{
					g_pCharaManager->RespawnChara( m_pOwner->getCharaNetIndex() );
				}

				i3Vector::Set( getMoveContext()->getMoveDir(), (REAL32)(i3Math::Rand() % 2), 0.f, (REAL32)(i3Math::Rand() % 2) );

				MATRIX mat;
				VEC3D	vDir;
				
				i3Matrix::SetRotateY( &mat, getBoneContext()->getTheta() );
				i3Vector::Copy( &m_pOwner->m_vDebugOriginalPos, m_pOwner->GetPos() );
				i3Vector::TransformCoord( &vDir, getMoveContext()->getMoveDir(), &mat );
				i3Vector::Add( &m_pOwner->m_vDebugTargetPos, m_pOwner->GetPos(), &vDir );
				m_pOwner->m_rDebugElp = 1.f;

				if( i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR) )
					m_pOwner->Cmd_Idle_Lower();
				else
					m_pOwner->Cmd_Run( getMoveContext()->getMoveDir());
			}
			else
#endif
			{	// 리스폰 타임 전에 리스폰 위치로 옮기고 캐릭터를 끕니다
				g_pCharaManager->DeathPosition( m_pOwner->getCharaNetIndex() );
			}
		}
	}
}

void CGameCharaUpdater::DoFirstUpdate( REAL32 rDeltatime)
{
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !BattleServerContext::i()->IsP2PActivate() )		return;

	if (i3::kind_of<CGameCharaFullbody*>((CGameCharaBase*)(m_pOwner)))
		OnFirstUpdateFullbody(rDeltatime);
	else
		OnFirstUpdate(rDeltatime);
}

void CGameCharaUpdater::OnFirstUpdate( REAL32 rDeltatime)
{
	if( m_pOwner->isLocal() || m_pOwner->isAI() )
	{
		// Local용 패킷 처리
		ProcessLocalFromRecvPacket();
	
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		{
			// 이동 처리
			getMoveContext()->Move( rDeltatime);
			getMoveContext()->UpdateSpeedReduction( rDeltatime);
		}
	}

	if( m_pOwner->is3PV())
	{
		// Animation과 World-Space Transform 사이에 LOD 계산이 달라질 수 있으며,
		// 그것은 Visual 상에 여러 문제가 될 수 있기 때문에
		// 수동으로 LOD를 계산시킨다.
		//	Zoom시 FOV 비율 계산
		if(g_pCamera)
		{
			REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();

			//	모델링 LOD 계산
			m_pOwner->calcLOD( m_pOwner->GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);
		}
	}

	getBoneContext()->UpdateAnim( rDeltatime);		// Animation의 Key 값을 적용 (각 Bone의 Cache Matrix에 저장)

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		if( m_pOwner->isNetwork())
			getBoneContext()->_CalcThetaNetwork( rDeltatime);
		else
			getBoneContext()->_CalcTheta( rDeltatime);

		if( m_pOwner->is3PV())
		{
			// 상, 하체 다 돌린다.
			getBoneContext()->_CalcAimBone( rDeltatime);

			// 머리에 대한 IK
			if( !m_pOwner->IsDino())
				getBoneContext()->_CalcHeadBone( rDeltatime);
		}
		
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// 이하에서는 getSceneObject()의 Matrix는 변경되어선 안된다.
		// 만약 변경된다면, Bone 계산에서 변경된 만큼의 오차가 날 수 있다.

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 들의 World-Space Matrix를 계산
		// 
		getBoneContext()->_CalcSpineBones();

		//	Rho 
		getBoneContext()->CalcRecoil( rDeltatime);		// 독립적

		if( m_pOwner->is1PV())
		{
			getBoneContext()->_CalcRho( rDeltatime);			// IK Bone ( IK Cache Matrix)
		}
		else
		{
			// 상체만 돌린다.
			getBoneContext()->_CalcRho3PV( rDeltatime);
		}
	}

	getBoneContext()->TransformWorldSpace( rDeltatime);
}

void CGameCharaUpdater::OnFirstUpdateFullbody(REAL32 tm)
{
	REAL32 ratio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();
	m_pOwner->calcLOD(m_pOwner->GetPos(), i3Matrix::GetPos(g_pCamera->getCamMatrix()), ratio);

	getBoneContext()->UpdateAnim(tm);
	if (!m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))
	{
		if( m_pOwner->isNetwork())
			getBoneContext()->_CalcThetaNetwork(tm);
		else
			getBoneContext()->_CalcTheta(tm);
	}
	getBoneContext()->TransformWorldSpace(tm);
}


void CGameCharaUpdater::DoUpdate( REAL32 rDeltatime)
{
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !BattleServerContext::i()->IsP2PActivate() )		return;

	OnProcessJump( rDeltatime);
	OnUpdate( rDeltatime);
}

void CGameCharaUpdater::OnUpdate( REAL32 rDeltatime)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		ProcessDeathState( rDeltatime);

	getMaterialContext()->DriveLightVolume( rDeltatime);
	getCameraContext()->OnUpdate( rDeltatime);

	//	캐릭터 모델링 알파 처리
	getMaterialContext()->ProcessMaterial( rDeltatime);

	CHARA_TASK task = CHARA_TASK_NONE;
	task = g_pCharaManager->getCharaTask( m_pOwner->getCharaNetIndex());

	if( task == CHARA_TASK_RESPAWN)	//	조작 가능한 상태이면
	{
		if( gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE)
			OnProcessInvincible( rDeltatime);
		else
		{
			if( !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_CONTROL ) )
			{	
				m_pOwner->Cmd_Reset();
				m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
			}
		}
	}
	else
	{	//	캐릭터가 죽었으면
		ProcessRespawn( rDeltatime);
	}
}

void CGameCharaUpdater::DoLastUpdate( REAL32 rDeltatime)
{
	if( !BattleServerContext::i()->IsP2PActivate()  )	return;

	OnLastUpdate( rDeltatime);
}

void CGameCharaUpdater::OnLastUpdate( REAL32 rDeltatime)
{
	
}

void CGameCharaUpdater::OnProcessInvincible( REAL32 rDeltatime)
{
	if( m_pOwner == nullptr )	return;

	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( g_pFramework->IsBattleStage() == false)
		return;

	if( !m_pOwner->isNetwork())
	{
#if !defined( DEF_OBSERVER_MODE)
		if(UserInfoContext::i()->IsGM_Observer())
		{
			if( m_bObserverStart )
			{
				CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
				if( pMyChara != nullptr && pMyChara->getCurHP() > 0)
				{
					pMyChara->Cmd_DamageSelf( 255);
				}
			}
			else					m_bObserverStart	= true;
		}
#endif			
		//	리스폰 직후 무적시간을 계산해 준다.
		if( rInvincible > 0.0f)
		{
			rInvincible -= rDeltatime;
			m_pOwner->setInvincibleTime( rInvincible);
			
			REAL32 limitTime = m_pOwner->getAbility( CHARA::eABILITY_INVINCIBLE_TIME);

			//	리스폰이 된 후 일정시간 지나면 키입력을 풀어준다. (무기 교체 GUI에서 키입력을 막았으므로)
			if (rInvincible < limitTime - 0.5f)
			{
				//리스폰이 된 후(무적인 상태) 인게임 채팅창이 나타나 있다면 캐릭터 컨트롤 막기
				UIHUDIngameChat* chat = (UIHUDIngameChat*)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
				if ( nullptr != chat && chat->GetEnableChat() )
				{
					if (false == m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_CONTROL))
					{
						m_pOwner->addEnableStateMask(CHARA_STATE_DISABLE_CONTROL);
					}
				}
				else
				{
					m_pOwner->removeEnableStateMask(CHARA_STATE_DISABLE_CONTROL);
				}
			}

			getMaterialContext()->SetBlendEnable( true);
		}		
		else if( rInvincible < 0.0f)		
		{	
			//리스폰이 된 후(무적해제) 인게임 채팅창이 나타나 있다면 캐릭터 컨트롤 막기
			UIHUDIngameChat* chat = (UIHUDIngameChat*)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
			if ( nullptr != chat && chat->GetEnableChat() )
			{
				if ( false == m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_CONTROL) )
				{
					m_pOwner->addEnableStateMask(CHARA_STATE_DISABLE_CONTROL);
				}
			}
			else
			{
				m_pOwner->removeEnableStateMask(CHARA_STATE_DISABLE_CONTROL);
			}

			rInvincible -= rDeltatime;
			m_pOwner->setInvincibleTime( rInvincible);

			CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

			if( (pMyChara != nullptr) && pMyChara->IsDino())
			{
				getMaterialContext()->setFaceCull(false);
			}
			else
			{
				getMaterialContext()->SetBlendEnable( false);
			}
			if(rInvincible < -0.5f) 
			{
				m_pOwner->setInvincibleTime( -0.5f);
			}

		}
	}
	else
	{
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
}

void CGameCharaUpdater::OnProcessJump( REAL32 rDeltatime)
{
	/*
	//	점프 딜레이 체크
	if( m_fJumpDelayElapsedTime > 0.0f)
	{
		m_fJumpDelayElapsedTime -= tm;
	}
	else if( m_fJumpDelayElapsedTime < 0.0f)		m_fJumpDelayElapsedTime = 0.0f;
	*/

	m_pOwner->setJumpDelayElapsedTime( m_pOwner->getJumpDelayElapsedTime() + rDeltatime);
}

void CGameCharaUpdater::ProcessPhysixSleep( REAL32 rDeltatime)
{
#if defined( PHYSIX_CHARA_RECALC_POS)
	ProcessRecalcPhysixPos( rDeltatime);
#endif

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXCONTACT) )
		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXFREEZE))
		return;

	REAL32 rContactTime = m_pOwner->getContactTime();

	rContactTime += rDeltatime;
	m_pOwner->setContactTime( rContactTime);

	if( rContactTime >= CAMERA_CHARA_SLEEP_TIME)
		m_pOwner->addCharaStateMask( CHARA_STATE_CAMFREEZE);

	if( rContactTime > PHYSIX_CHARA_SLEEP_TIME )
	{	
		for( INT32 i = 0; i < CHARA_HIT_MAXCOUNT; i++)
		{
			PHYSIX_SHAPESET_INFO * info = m_pOwner->setPhysixShapeSetInfo( (CHARA_HIT_PART) i);
			if( info->_pShapeSet != nullptr)
				info->_pShapeSet->setDynamicState(  I3_PHYSIX_STATE_DISABLE);
		}

		m_pOwner->addCharaStateMask( CHARA_STATE_PHYSIXFREEZE);
	}
}

void CGameCharaUpdater::ProcessPhysixSleepForOnObject( REAL32 rDeltatime)
{
#if defined( PHYSIX_CHARA_RECALC_POS)
	ProcessRecalcPhysixPos( rDeltatime);
#endif

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXCONTACT) )
		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXFREEZE ) )
		return;

	REAL32 rContactTime = m_pOwner->getContactTime();

	rContactTime += rDeltatime;
	m_pOwner->setContactTime( rContactTime);

	if( rContactTime >= CAMERA_CHARA_SLEEP_TIME)
		m_pOwner->addCharaStateMask( CHARA_STATE_CAMFREEZE);

	if( rContactTime > PHYSIX_CHARA_SLEEP_TIME )
	{
		REAL32 rHideTime = getUpdateContext()->getHideTimer();

		rHideTime -= rDeltatime;
		getUpdateContext()->setHideTimer( rHideTime);
		
		if( rHideTime > 0.f)
		{
			I3ASSERT( getUpdateContext()->getToHideTime() > I3_EPS);
			REAL32 fAlpha = rHideTime / getUpdateContext()->getToHideTime();

			m_pOwner->Cmd_SetColor( nullptr, nullptr, fAlpha);
		}
		else
		{	
			getUpdateContext()->setToHideTime( 0.f);
			getUpdateContext()->setHideTimer( 0.f);

			m_pOwner->Cmd_SetColor( nullptr, nullptr, 1.f);

			m_pOwner->getSceneObject()->AddFlag( I3_NODEFLAG_INVISIBLE);

			for( INT32 i = 0; i < CHARA_HIT_MAXCOUNT; i++)
			{
				PHYSIX_SHAPESET_INFO * info = m_pOwner->setPhysixShapeSetInfo( (CHARA_HIT_PART) i);
				if( info->_pShapeSet != nullptr)
					info->_pShapeSet->setDynamicState(  I3_PHYSIX_STATE_DISABLE);
			}

			m_pOwner->addCharaStateMask( CHARA_STATE_PHYSIXFREEZE);
		}
	}
}

void CGameCharaUpdater::DebugProcess( REAL32 rDeltatime)
{	
#if defined( I3_DEBUG) | defined( BUILD_RELEASE_QA_VERSION)
	if( g_pConfig != nullptr && !g_pConfig->m_bNetworking)
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
			return;

		m_pOwner->m_rDebugMoveTime += ( rDeltatime * m_pOwner->m_rDebugElp * 3.f);
		if( m_pOwner->m_rDebugMoveTime > 3.f || m_pOwner->m_rDebugMoveTime < -3.f)
		{
			m_pOwner->m_rDebugElp *= -1.f;
			if( m_pOwner->m_rDebugMoveTime > 3.f )
				m_pOwner->m_rDebugMoveTime = 3.f;
			else if( m_pOwner->m_rDebugMoveTime < -3.f)
				m_pOwner->m_rDebugMoveTime = -3.f;
			i3Vector::Scale( getMoveContext()->getMoveDir(), getMoveContext()->getMoveDir(), -1.f );
		}

		i3Vector::Lerp( &m_pOwner->m_vDebugPos, &m_pOwner->m_vDebugOriginalPos, &m_pOwner->m_vDebugTargetPos, m_pOwner->m_rDebugMoveTime );
		i3Vector::Sub( getMoveContext()->getOldVelocity(), &m_pOwner->m_vDebugPos, m_pOwner->GetPos() );
		getMoveContext()->setSpeed( i3Vector::Length( getMoveContext()->getOldVelocity() ) );
		
		m_pOwner->Cmd_Run( getMoveContext()->getMoveDir());

		m_pOwner->forceSetPos( &m_pOwner->m_vDebugPos );
	}
#endif
}

void CGameCharaUpdater::ProcessRecalcPhysixPos( REAL32 rDeltatime)
{
	VEC3D vDist;
	MATRIX Mtx, destMtx;
	PHYSIX_SHAPESET_INFO * pCurInfo = nullptr;
	
	for( INT32 i = 1; i < CHARA_HIT_MAXCOUNT; i++)
	{
		pCurInfo = m_pOwner->setPhysixShapeSetInfo( (CHARA_HIT_PART)i);
		if( pCurInfo->_pShapeSet == nullptr)
			continue;
		
		if( (pCurInfo->_pShapeSet->getNxJoint() != nullptr) && (pCurInfo->_iParentShapeIdx != (INT32)CHARA_HIT_UNKNOWN) )
		{
			pCurInfo->_pShapeSet->getShapeGlobalPose( &Mtx);

			const PHYSIX_SHAPESET_INFO * pLinkInfo = m_pOwner->getPhysixShapeSetInfo( pCurInfo->_iParentShapeIdx);
			I3ASSERT( pLinkInfo != nullptr);
			
			if( pLinkInfo->_pShapeSet != nullptr)
			{
				pLinkInfo->_pShapeSet->getShapeGlobalPose( &destMtx);
				
				VEC3D * pPos = i3Matrix::GetPos( &Mtx);
				i3Vector::Sub( &vDist, i3Matrix::GetPos( &destMtx), pPos);
				REAL32 rLen = i3Vector::Normalize( &vDist, &vDist);

				if( pCurInfo->_rDistanceToParentBone + PHYSIX_CHARA_SHAPE_EPS_POS < rLen )
				{
					//I3TRACE( "[%d] %s : %.4f, %s : %.4f\n", i, pCurInfo->_pShapeSet->GetName(), pCurInfo->_rDistanceToParentBone, pLinkInfo->_pShapeSet->GetName(), rLen );
					i3Vector::Scale( &vDist, &vDist, (rLen - pCurInfo->_rDistanceToParentBone) * 0.5f );
					i3Vector::Add( pPos, pPos, &vDist);
					pCurInfo->_pShapeSet->setShapeGlobalPose( &Mtx);
				}
			}
		}
	}
}

void CGameCharaUpdater::ProcessDeathState( REAL32 rDeltatime)
{
	if( g_pEnvSet->m_bEnablePhysX == false || m_pOwner->IsRex() || m_pOwner->IsDomiMutantRex())
	{
		REAL32 rOriginalHeight = getUpdateContext()->getDeathOriginalHeight();
		REAL32 rBlendTime = getUpdateContext()->getDeathBlendTime();
		rBlendTime += rDeltatime;
		if( rBlendTime > 0.5f)
			rBlendTime = 0.5f;

		getUpdateContext()->setDeathBlendTime( rBlendTime);

		setY( m_pOwner->GetPos(), rOriginalHeight + (getUpdateContext()->getDeathBlendHeight() - rOriginalHeight) * rBlendTime * 2.f);
	}

	getCollisionContext()->ProcessCollisionEffect();

	if( g_pFramework->IsBattleStage())
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_ONOBJECT))
			ProcessPhysixSleepForOnObject( rDeltatime);
		else
			ProcessPhysixSleep( rDeltatime);

		getUpdateContext()->UpdateDeathTime( rDeltatime );
	}
}

void CGameCharaUpdater::CheckGrazeSoundPlay(void)
{
	const i3::vector<WeaponRocketLauncherShell*>&	List = g_pWeaponManager->getRPG7ShellList();
	GrazeSoundPlay(List);

	GrazeSoundPlay(List);		// 무슨 일인지 몰라도 2번호출됨..(의도적인듯?) 2012.05.30.수빈.
}

void CGameCharaUpdater::GrazeSoundPlay(const i3::vector<WeaponRocketLauncherShell*>& List)
{
	for( size_t i = 0; i < List.size(); i++)
	{
		WeaponRocketLauncherShell * pShell = List[i];
		if( pShell->isEnable() && pShell->getOwner() != m_pOwner)
		{		
			MainWeapon_RLShell* mainWeapon_Shell = static_cast<MainWeapon_RLShell*>(pShell->GetMainWeapon());

			if( mainWeapon_Shell->isBombState( GRENADESHELL_STATE_LAUNCH) && !(mainWeapon_Shell->isPlayedGrazeSound()) )
			{
				// 거리 체크
				VEC3D vDir;
				i3Vector::Sub( &vDir, m_pOwner->GetPos(), pShell->GetPos());
				REAL32 rLen = i3Vector::Length( &vDir);
				if( rLen < 10.f)
				{
					 mainWeapon_Shell->PlayGrazeSound();
				}
			}
		}
	}
}