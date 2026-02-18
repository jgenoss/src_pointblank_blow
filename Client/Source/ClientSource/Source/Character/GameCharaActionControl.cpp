#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaActionControl.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "WeaponShotGun3.h"
#include "WeaponShotGunWithButt.h"
#include "WeaponShotGunMaleFemale.h"
#include "UtilBattle.h"

#include "../ui/Batmanager.h"
#include "../ui/UIUtil.h"
#include "BattleSlotContext.h"

#include "MainWeapon_DualHandGun.h"
#include "MainWeapon_CrossFire.h"
#include "MainWeapon_DualShotGun.h"

#include "StageBattle.h"

#if 0
#define CASHITEM_TRACE	I3TRACE
#else
#define CASHITEM_TRACE	__noop
#endif

I3_CLASS_INSTANCE( CGameCharaActionControl);

void CGameCharaActionControl::__Play_Animation_Jump( void)
{
	CHARA_ACTION_UPPER upperState = getActionContext()->getUpper();

	if(m_pOwner->IsDino())
	{
		// 하체
		setLower( CHARA_LOWER_JUMP);
		if( i3Vector::LengthSq( getMoveContext()->getMoveDir()) > 0.f)
		{
			getAnimContext()->setEnableRunJump( true);
			REAL32 rScale = getAnimContext()->getLowerActualDuration() / getAnimContext()->getLowerDuration();
			getAnimContext()->setRunJumpStepTime( getAnimContext()->getLowerLocalTime() * rScale);
		}

		_Animation_LowerForDino( ID_LOWER_DINO_JUMP_START);
	
		GlobalFunc::PB_TRACE("OnCmdAnimPlayJump %d", upperState);

		if( (upperState == CHARA_UPPER_IDLE) || (upperState == CHARA_UPPER_MOVE) )
		{
			setUpper( CHARA_UPPER_JUMP);
			_Animation_DinoUpper( ID_UPPER_DINO_JUMP_START);
			GlobalFunc::PB_TRACE("_Animation_DinoUpper");
		}
	}
	else
	{
		m_pOwner->Cmd_SyncWeapon();

		// revision 32344
		//CHARA_BODYLOWER_STAND 세우는 코드로 인하여 애니메이션 버그가 많이 발생하여 
		//일단 주석으로 막고 QA테스트 후 주석삭제 또는 코드 삭제 필요함
		//if( isBody( CHARA_BODYLOWER_CROUCH))	//	앉아 있으면
		//{	// 선자세로 세팅
		//	_ApplyActionBody( CHARA_BODYLOWER_STAND);
		//}

		// 하체
		setLower( CHARA_LOWER_JUMP);

		const VEC3D* pMoveDir = getMoveContext()->getMoveDir();
		float sqMoveDirXZ = pMoveDir->x * pMoveDir->x + pMoveDir->z * pMoveDir->z;		// Y쪽은 점프와 사다리 모두 해당되는데, 여기서는 길이 계산 스킵..

		if( sqMoveDirXZ > 0.f )   // i3Vector::LengthSq( getMoveContext()->getMoveDir()) > 0.f
		{
			getAnimContext()->setEnableRunJump( true);
			REAL32 rScale = getAnimContext()->getLowerActualDuration() / getAnimContext()->getLowerDuration();
			getAnimContext()->setRunJumpStepTime( getAnimContext()->getLowerLocalTime() * rScale);
		}

		_Animation_Lower( ID_LOWER_JUMP_START);

		// KEYWORD : WEAPON_DUALHANDGUN
		WeaponBase * pWeapon = getUsedWeapon();
		if( pWeapon == nullptr) return;

		if( pWeapon->isDualGun())
		{
			// 오른쪽
			if( (upperState == CHARA_UPPER_IDLE) || (upperState == CHARA_UPPER_MOVE) )
			{
				setUpper( CHARA_UPPER_JUMP);
				_Animation_DualUpper( ID_UPPER_DUAL_JUMP_START_RIGHT);
			}

			// 왼쪽
			CHARA_ACTION_UPPER leftState = getActionContext()->getUpperLeft();
			if( (leftState == CHARA_UPPER_IDLE) || (leftState == CHARA_UPPER_MOVE))
			{
				setUpperLeft( CHARA_UPPER_JUMP);
				_Animation_DualUpper( ID_UPPER_DUAL_JUMP_START_LEFT);
			}
		}
		else
		{
			if( (upperState == CHARA_UPPER_IDLE) || (upperState == CHARA_UPPER_MOVE) )
			{
				setUpper( CHARA_UPPER_JUMP);
				_Animation_Upper( ID_UPPER_JUMP_START);
			}
		}
	}

}

void CGameCharaActionControl::_Animation_DinoUpper( ID_UPPER_DINO_AI aiID, REAL32 startTime, REAL32 scale)
{
	((CGameDinoAnimContext*)getAnimContext())->_Play_Anim_DinoUpper( aiID, startTime, scale);
}

void CGameCharaActionControl::_Animation_LowerForDino( ID_LOWER_DINO_AI aiID, REAL32 startTime, REAL32 scale)
{
	I3_BOUNDCHK( aiID, ID_LOWER_DINO_AI_MAX);
	((CGameDinoAnimContext*)getAnimContext())->_Play_AnimDino_Lower( aiID, startTime, scale);
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action

bool CGameCharaActionControl::OnActionStandup( void)
{

	if(m_pOwner->IsRaptorBase())
		_Animation_DinoUpper( ID_UPPER_DINO_IDLE); //EscapeMission : 기존에 앉기 동작에 대한 상체 설정이 없는 관계로 "렙터"인 경우 동작을 강제로 지정합니다. 
	
	if( !isBody( CHARA_BODYLOWER_CROUCH) )			return false;

	if( !m_pOwner->isNetwork())
		if( getCollisionContext()->IsOverlappedBy( CGRP_MYCHARA_BOUND) )	return false;


	m_rStartStandUpTime = g_pFramework->GetAccTime();
	getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
	getActionContext()->_Play_Body_Sync();

	return true;
}

bool CGameCharaActionControl::OnActionReset( void)
{
	//	캐릭터 State 정보 리셋
	// Animation을 중지한다.
	getActionContext()->_Stop_Anim();

	getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
	getActionContext()->setBodyUpper( CHARA_BODYUPPER_ATTACKIDLE);
	setLower( CHARA_LOWER_IDLE);
	setUpper( CHARA_UPPER_IDLE);
	setUpperLeft( CHARA_UPPER_IDLE);

	m_rStartStandUpTime = 0.f;

	getAnimContext()->ChangeAnimationContext();

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	
	if(m_pOwner->IsDino() == false)
	{
		if (!getWeaponContext())
		{
			TRACE("CGameCharaActionControl::OnActionReset getWeaponContext() = nullptr\n");
			return false;
		}

		WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();

		if( pWeapon == nullptr)	return false;

		pWeapon = pWeapon->getUsedWeapon();
		I3ASSERT( pWeapon != nullptr);

		if( pWeapon->isDualGun())
		{
			setUpper( CHARA_UPPER_IDLE);
			_Animation_DualUpper( ID_UPPER_DUAL_IDLE_RIGHT);

			setUpperLeft( CHARA_UPPER_IDLE);
			_Animation_DualUpper( ID_UPPER_DUAL_IDLE_LEFT);
			
			//	강제로 적용
			i3AIContext * pAIUpperLeft = getAnimContext()->getAIUpper_Left();
			if( pAIUpperLeft != nullptr)
				pAIUpperLeft->SetAIStateImmediately( g_pViewer->getDeltaTime());
		}
		else
		{
			_Animation_Upper( ID_UPPER_IDLE);
		}
	}
	else
	{	
		_Animation_DinoUpper( ID_UPPER_DINO_IDLE);
	}

	//	강제로 적용
	if( pAIUpper != nullptr )
		pAIUpper->SetAIStateImmediately( g_pViewer->getDeltaTime());

	return true;
}

bool CGameCharaActionControl::OnActionCrouch( void)
{
	// 죽은 상태라면 return 
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	if( !isBody( CHARA_BODYLOWER_STAND))
		return false;

	if( m_pOwner->IsDino())
		return false;

	getActionContext()->setBodyLower( CHARA_BODYLOWER_CROUCH);

/* 렙터 앉기 동작이 다시 사라지는 관계로 주석으로 막아놓습니다.
	if(m_pOwner->IsRaptor())
	_Animation_DinoUpper( ID_UPPER_DINO_SIT_IDLE); //EscapeMission : 기존에 앉기 동작에 대한 상체 설정이 없는 관계로 "렙터"인 경우 동작을 강제로 지정합니다. 
*/	
	
	
	getActionContext()->_Play_Body_Sync();
	return true;
}

bool CGameCharaActionControl::OnActionSteadyDinoUpper( void)
{
	if( m_pOwner->getAttackIdleTime() > 0.f)	return false;
	if( m_pOwner->getActionPendingTime() > 0.f)	return false;

	REAL32 fSyncTime = 0.f;

	if(m_pOwner->is3PV())
	{
		fSyncTime = getAnimContext()->GetLowerSyncTime();
	}

	setUpper( CHARA_UPPER_IDLE);

		//EscapeMission :물기와 할퀴기 통합으로 인해 Bite Idle에 대한 임시로 변경합니다.
	if (isBody(CHARA_BODYLOWER_CROUCH))
	{
		_Animation_DinoUpper( ID_UPPER_DINO_SIT_IDLE, fSyncTime);
	}
	else
		_Animation_DinoUpper( ID_UPPER_DINO_IDLE, fSyncTime);

	return true;
}

bool CGameCharaActionControl::OnActionJumpStart(void)
{
	/// Death 상태에서 Jump packet을 받는다면?? 문제가 생길 수 있다. 2010.08.25 순구
	if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))	return false;

	if (!m_pOwner->isNetwork())
	{
		const CCharaInfo * pDBInfo = m_pOwner->getDBInfo();
		I3ASSERT(pDBInfo != nullptr);
		//탱크와 티라노는 점프 처리를 하지 않습니다.
		if( m_pOwner->IsRex() == true || m_pOwner->IsTank())
			return false;

		// 캐릭터의 바운드 체크
		//	연속 점프를 막기 위해 딜레이 시간을 준다.
		if( m_pOwner->getJumpDelayElapsedTime() < m_pOwner->GetJumpDelay() ||
			getCollisionContext()->IsOverlappedBy( CGRP_MYCHARA_BOUND) )
			return false;
		
		I3ASSERT( pDBInfo->GetJumpPenaltyTime() > I3_EPS );
		REAL32	rRate = m_pOwner->getJumpDelayElapsedTime() / pDBInfo->GetJumpPenaltyTime();
		REAL32  rValidRate = 1.0f;
	
		if( rRate < 0.3f)								rValidRate = MINMAX( 0.1f, 1.0f - ( pDBInfo->GetJumpPenaltyRate() * 3.0f), 1.0f);
		else if( (rRate >= 0.3f) && ( rRate < 0.6f))	rValidRate = MINMAX( 0.1f, 1.0f - ( pDBInfo->GetJumpPenaltyRate() * 2.0f), 1.0f);
		else if(( rRate > 1.0f) && ( rRate >= 0.6f))	rValidRate = MINMAX( 0.1f, 1.0f - ( pDBInfo->GetJumpPenaltyRate() * 1.0f), 1.0f);

		
		if( (g_pFramework->GetAccTime() < m_rStartStandUpTime + pDBInfo->GetClunchJumpCheckTime()) || CHARA_BODYLOWER_CROUCH == getActionContext()->getBodyLower())
		{
			if ( MyRoomInfoContext::i()->getStage()->IsSpace() )
			{
				getMoveContext()->setCurJumpHeight(m_pOwner->getDBInfo()->GetNewTempClunchJumpHeight());
			}
			else
			{
				REAL32 rValue = pDBInfo->GetClunchJumpHeight();
				getMoveContext()->setCurJumpHeight((rValue + m_pOwner->GetJumpHeightOffset() + rValue*m_pOwner->getAbility(CHARA::eABILITY_JUMPHEIGHT)) * rValidRate);
			}
			m_rStartStandUpTime = 0.f;
		}
		else
		{
			if (MyRoomInfoContext::i()->getStage()->IsSpace())
			{
				getMoveContext()->setCurJumpHeight(m_pOwner->getDBInfo()->GetNewTempJumpHeight());
			}
			else
			{
				REAL32  rValue = pDBInfo->GetJumpHeight();
				getMoveContext()->setCurJumpHeight((rValue + m_pOwner->GetJumpHeightOffset() + rValue*m_pOwner->getAbility(CHARA::eABILITY_JUMPHEIGHT)) * rValidRate);
			}
		}

		if( BaTMaN::enable_jump_booster )
		{
			REAL32 jump = getMoveContext()->getCurJumpHeight();
			getMoveContext()->setCurJumpHeight( jump * BaTMaN::jump_booster_factor );
		}

		// getMoveContext()->setCurJumpHeight( pDBInfo->m_rJumpHeight * rValidRate);
		getMoveContext()->setCurJumpTime( pDBInfo->GetJumpTime() * rValidRate * 0.5f);

		// 이동과 앉기를 막는다.
		bool jump_move_enable = false;
		jump_move_enable = MyRoomInfoContext::i()->getStage()->IsSpace();
		
		if( !jump_move_enable )
			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
	}

	m_pOwner->addCharaStateMask( CHARA_STATE_JUMP);
	getMoveContext()->setElapsedTakeOffTime( 0.f);
	__Play_Animation_Jump();

	return true;
}

/*virtual*/ bool CGameCharaActionControl::OnActionSteadyLower( void)
{
	setLower( CHARA_LOWER_IDLE);
	_Animation_Lower( ID_LOWER_IDLE);

	getAnimContext()->setRunElapseTime(0.f);

	return true;
}

/*virtual*/ bool CGameCharaActionControl::OnActionSteadyDinoLower( void)
{
	setLower( CHARA_LOWER_IDLE);
	_Animation_LowerForDino( ID_LOWER_DINO_IDLE);

	getAnimContext()->setRunElapseTime(0.f);

	return true;
}

bool CGameCharaActionControl::OnActionSteadyUpper( void)
{
	if( m_pOwner->IsDino())	return OnActionSteadyDinoUpper();
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;
	if( m_pOwner->getAttackIdleTime() > 0.f)				return false;

	REAL32 fSyncTime = 0.f;

	if(m_pOwner->is3PV())
	{
		fSyncTime = getAnimContext()->GetLowerSyncTime();
	}

	m_pOwner->Cmd_SyncWeapon();
	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == nullptr)	return false;

	// 예외처리
	{
		if( i3::same_of<WeaponKrissSuperV*>(pWeapon))
		{
			if( ((WeaponKrissSuperV*)pWeapon)->isUsedDualWeapon())
				pWeapon = ((WeaponKrissSuperV*)pWeapon)->getDualWeapon();
		}
		else if ( i3::same_of<WeaponShotGunGenderWithDual*>(pWeapon) )
		{
			if( ((WeaponShotGunGenderWithDual*)pWeapon)->isUsedDualWeapon())
				pWeapon = ((WeaponShotGunGenderWithDual*)pWeapon)->getDualWeapon();
		}
	}

	setUpper( CHARA_UPPER_IDLE);

	if( pWeapon->isDualGun())
	{
		_Animation_DualUpper( ID_UPPER_DUAL_IDLE_RIGHT);

		setUpperLeft( CHARA_UPPER_IDLE);
		_Animation_DualUpper( ID_UPPER_DUAL_IDLE_LEFT);
	}
	else
	{
		setUpper( CHARA_UPPER_IDLE);
		_Animation_Upper( ID_UPPER_IDLE, fSyncTime);
	}

	return true;
}

bool CGameCharaActionControl::OnActionRunDinoUpper( void)
{
	////////////////////////////////////////////////////////////////////
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	REAL32 fSyncTime = 0.f;

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();

	//	현재 이동 중이다. ( 이동 애니메이션 동작시간과 맞춰 플레이)
	if( pAIUpper != nullptr)	fSyncTime = pAIUpper->getLocalTime();

	REAL32 fScaleTime = _CalcRunDinoUpperScaleTime();

	if( ((CGameDinoAnimContext*)getAnimContext())->getOldDinoUpperID() != ID_UPPER_DINO_MOVE )
	{
		setUpper( CHARA_UPPER_MOVE);
		_Animation_DinoUpper( ID_UPPER_DINO_MOVE, fSyncTime, fScaleTime);
	}
	else
	{
		getAnimContext()->SetUpperTimeScale( fScaleTime );
	}

	return true;
}

bool CGameCharaActionControl::OnActionWalkDinoUpper( void)
{
	////////////////////////////////////////////////////////////////////
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();

	//	현재 이동 중이다. ( 이동 애니메이션 동작시간과 맞춰 플레이)
	if( pAIUpper != nullptr)	fSyncTime = pAIUpper->getLocalTime();

	fScaleTime = _CalcRunDinoUpperScaleTime();

	setUpper( CHARA_UPPER_MOVE);
	_Animation_DinoUpper( ID_UPPER_DINO_WALK, fSyncTime, fScaleTime);

	return true;
}
/*
bool CGameCharaActionControl::OnActionWalkUpper( void)//EscapeMission : 일반캐릭은 Walk동작을 기존에 쓰던 방식으로 설정
{
	if( m_pOwner->IsDino())	return OnActionWalkDinoUpper();
	else return OnActionRunUpper();
	

}
*/
bool CGameCharaActionControl::OnActionRunUpper( void)
{
	if( m_pOwner->IsDino())	return OnActionRunDinoUpper();
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;
	if( m_pOwner->getAttackIdleTime() > 0.f)				return false;

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;
	ID_UPPER_AI	nId = ID_UPPER_MOVE;	//	상체는 방향 관계없이 "Move"만

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr) return false;

	if( pWeapon->isDualGun())
	{
		if( (getActionContext()->getUpper() == CHARA_UPPER_IDLE) ||
			(getActionContext()->getUpper() == CHARA_UPPER_JUMP) ||
			(getActionContext()->getUpper() == CHARA_UPPER_MOVE))
		{
			ActionRunUpperRight();
		}
		if( (getActionContext()->getUpperLeft() == CHARA_UPPER_IDLE) ||
			(getActionContext()->getUpperLeft() == CHARA_UPPER_JUMP) ||
			(getActionContext()->getUpperLeft() == CHARA_UPPER_MOVE))
		{
			ActionRunUpperLeft();
		}
		return true;
	}

	CWeaponInfo * pWeaponInfo = pWeapon->getRealWeaponInfo();

	I3ASSERT( pWeaponInfo != nullptr);

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();

	//	현재 이동 중이다. ( 이동 애니메이션 동작시간과 맞춰 플레이)
	if( pAIUpper != nullptr)	fSyncTime = pAIUpper->getLocalTime();

	REAL32 fScaleTime = _CalcRunUpperScaleTime();

	if( (getAnimContext()->getOldUpperID() != nId) )
	{
		setUpper( CHARA_UPPER_MOVE);
		_Animation_Upper( nId, fSyncTime, fScaleTime);
	}
	else
		getAnimContext()->SetUpperTimeScale( fScaleTime );

	return true;
}

bool CGameCharaActionControl::OnActionJumpLowerFinish( void)
{
	getAnimContext()->setEnableRunJump( false);
	setLower( CHARA_LOWER_IDLE);

	return true;
}

ID_LOWER_AI CGameCharaActionControl::_FindDeathAnim( CHARA_DEATH_TYPE deathType, VEC3D * pVec)
{
	I3ASSERT( pVec != nullptr);
	LOWER_AI_DIR nFaceDir = LOWER_AI_DIR_FRONT;
	VEC3D vDir;
	i3Vector::Scale( &vDir, pVec, -1.f);

	REAL32 rDot = i3Vector::Dot( i3Matrix::GetAt( m_pOwner->GetMatrix()), &vDir);
	if( rDot > 0.7071f)
	{
		nFaceDir = LOWER_AI_DIR_FRONT;
	}
	else if( rDot < -0.7071f)
	{
		nFaceDir = LOWER_AI_DIR_BACK;
	}
	else
	{
		rDot = i3Vector::Dot( i3Matrix::GetRight( m_pOwner->GetMatrix()), &vDir);
		if( rDot > 0.f)
		{
			nFaceDir = LOWER_AI_DIR_LEFT;
		}
		else
		{
			nFaceDir = LOWER_AI_DIR_RIGHT;
		}
	}

	ID_LOWER_AI nLowerId = ID_LOWER_UNKNOWN;

	switch( deathType)
	{
	case CHARA_DEATH_FAST_OBJECT :		// by Fast Object, Damage Object
	case CHARA_DEATH_A :
		nLowerId = GetLowerAnimID(LOWER_AI_DEATH_A, nFaceDir);
		break;
	case CHARA_DEATH_O :					// by Object Explosion
	case CHARA_DEATH_B :				// Bome! = Granade
		nLowerId = GetLowerAnimID(LOWER_AI_DEATH_B, nFaceDir);
		break;
	case CHARA_DEATH_H :					// Head Shot!!
	case CHARA_DEATH_F :					// Falling death
	case CHARA_DEATH_POISON :				// 독에 의한 사망
	case CHARA_DEATH_TRAMPLED :			// 짓밟힘
	case CHARA_DEATH_HOWL :				// HOWL에 놀라 죽음;;;
		nLowerId = GetLowerAnimID(LOWER_AI_DEATH_H, nFaceDir);
		break;
	}

	return nLowerId;
}

ID_LOWER_DINO_AI CGameCharaActionControl::_FindDeathAnimForDino( CHARA_DEATH_TYPE deathType, VEC3D * pVec)
{
	I3ASSERT( pVec != nullptr);
	LOWER_AI_DIR nFaceDir = LOWER_AI_DIR_FRONT;
	VEC3D vDir;
	i3Vector::Scale( &vDir, pVec, -1.f);

	REAL32 rDot = i3Vector::Dot( i3Matrix::GetAt( m_pOwner->GetMatrix()), &vDir);
	if( rDot > 0.7071f)
	{
		nFaceDir = LOWER_AI_DIR_FRONT;
	}
	else if( rDot < -0.7071f)
	{
		nFaceDir = LOWER_AI_DIR_BACK;
	}
	else
	{
		rDot = i3Vector::Dot( i3Matrix::GetRight( m_pOwner->GetMatrix()), &vDir);
		if( rDot > 0.f)
		{
			nFaceDir = LOWER_AI_DIR_LEFT;
		}
		else
		{
			nFaceDir = LOWER_AI_DIR_RIGHT;
		}
	}

	ID_LOWER_DINO_AI nLowerId = ID_LOWER_DINO_UNKNOWN;

	switch( deathType)
	{
	case CHARA_DEATH_FAST_OBJECT :		// by Fast Object, Damage Object
	case CHARA_DEATH_A :
		nLowerId = GetLowerAnimIDForDino(LOWER_AI_DEATH_A, nFaceDir);
		break;
	case CHARA_DEATH_O :					// by Object Explosion
	case CHARA_DEATH_B :				// Bome! = Granade
		nLowerId = GetLowerAnimIDForDino(LOWER_AI_DEATH_B, nFaceDir);
		break;
	case CHARA_DEATH_H :					// Head Shot!!
	case CHARA_DEATH_F :					// Falling death
	case CHARA_DEATH_POISON :				// 독에 의한 사망
	case CHARA_DEATH_TRAMPLED :			// 짓밟힘
	case CHARA_DEATH_HOWL :				// HOWL에 놀라 죽음;;;
		nLowerId = GetLowerAnimIDForDino(LOWER_AI_DEATH_H, nFaceDir);
		break;
	}

	return nLowerId;
}

bool CGameCharaActionControl::OnActionDeath( INT32 iKillIdx, CHARA_DEATH_TYPE type, VEC3D * pDir)
{
	////////////////////////////////////////////////////////////////////
	m_pOwner->Cmd_Idle_Upper();

	if( getAnimContext()->isAIUpper() )
		getAnimContext()->getAIUpper()->SetAIStateImmediately( g_pViewer->getDeltaTime());

	/*if( g_pEnvSet->m_bEnablePhysX == false || m_pOwner->IsRex())
	{
		m_pOwner->Cmd_ChangeView( CHARA_VIEW_3PV);
	}*/

	//	Death 동작이므로 NONE 상태로 만들어 준다.
	getActionContext()->setBodyUpper( CHARA_BODYUPPER_ATTACKIDLE );
	setUpper( CHARA_UPPER_IDLE );
	setUpperLeft( CHARA_UPPER_IDLE);

	ID_LOWER_AI			nId		= ID_LOWER_UNKNOWN;
	ID_LOWER_DINO_AI	nDinoId	= ID_LOWER_DINO_UNKNOWN;

	if (m_pOwner->IsDino())
	{
		nDinoId =  _FindDeathAnimForDino( type, pDir);

	}
	else
	{
		nId = _FindDeathAnim( type, pDir);

	}

	getActionContext()->_Stop_Anim();

	if( getAnimContext()->isAIUpper() )
	{
		getAnimContext()->getAIUpper()->SetExclusiveEnable( false, true);
		getAnimContext()->setAIUpper( nullptr);
	}

	if( getAnimContext()->getAIUpper_Left() != nullptr)
	{
		getAnimContext()->getAIUpper_Left()->SetExclusiveEnable( false, true);
		getAnimContext()->setAIUpper_Left( nullptr);
	}

	setLower( CHARA_LOWER_DEATH);
	if (m_pOwner->IsDino())
	{
		_Animation_LowerForDino( nDinoId);
	}
	else
	{
		//데스타입이 날라오지 않아 임시로 땜빵처리합니다. 2013.10.29 //수정 필요합니다. 
		if(nId  ==ID_LOWER_UNKNOWN )
			nId = ID_LOWER_DEATH_FRONT_H1;

		_Animation_Lower( nId);
	}

	if( getAnimContext()->getAILower() != nullptr)
		getAnimContext()->getAILower()->SetAIStateImmediately( g_pViewer->getDeltaTime());

	// 모든 기능을 끕니다.
	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_CONTROL | CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE |
		CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE );

	m_pOwner->addCharaStateMask( CHARA_STATE_USED_COMBINE_ANIM);

	//I3TRACE( "Death!!  - %d\n", getCharaNetIndex() );

	return true;
}

bool CGameCharaActionControl::OnActionFire( INT32 iSlot /*= -1*/, WEAPON::FIRE_ORDER order /*= WEAPON::PRIMARY_FIRE*/)
{
	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	if( m_pOwner->IsDino())
	{	
		return OnActionFireDino( iSlot, order);
	}
	else
	{
		return OnActionFireHuman( order);
	}
}

bool CGameCharaActionControl::OnActionFireDino( INT32 iSlot, WEAPON::FIRE_ORDER order)
{
	ID_UPPER_DINO_AI nUpperAiId = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack( order);

	if( nUpperAiId != ID_UPPER_DINO_UNKNOWN)
	{
		INT32 nAIIdx			= ((CGameDinoAnimContext*)getAnimContext())->getDinoAIUpperID( nUpperAiId);
		i3AIContext * pAICtx	= getAnimContext()->getAIUpper();

		if((pAICtx != nullptr) && (nAIIdx != -1))
		{
			i3AIState * pAI = pAICtx->getAIState( nAIIdx);
			if( pAI != nullptr)
			{
				m_pOwner->setAttackIdleTime( pAI->getDuration() );
			}
		}
	}

	return true;
}

void CGameCharaActionControl::_ActionFireDual( REAL32 rStartTime, REAL32 rScale, bool bRight)
{
	if( bRight)
	{
		setUpper( CHARA_UPPER_FIRE);
		_Animation_DualUpper( ID_UPPER_DUAL_ATTACK_RIGHT, rStartTime, rScale);

		if( m_pOwner->is1PV())	{
			if( m_pOwner->isNetwork()) {
				m_pOwner->setAttackIdleTime( 1.f);	// 1.초간 발사 동작을 유지합니다.
			}
		}
		else
			m_pOwner->setAttackIdleTime( 1.f);		// 1.초간 발사 동작을 유지합니다.
	}
	else
	{
		setUpperLeft( CHARA_UPPER_FIRE);
		_Animation_DualUpper( ID_UPPER_DUAL_ATTACK_LEFT, rStartTime, rScale);

		if( m_pOwner->is1PV())	{
			if( m_pOwner->isNetwork()) {
				m_pOwner->setAttackIdleTimeLeft( 1.f);	// 1.초간 발사 동작을 유지합니다.
			}
		}
		else
			m_pOwner->setAttackIdleTimeLeft( 1.f);		// 1.초간 발사 동작을 유지합니다.
	}
}

#define USE_RANDOM_FIRE_STARTTIME

bool CGameCharaActionControl::OnActionFireHuman( WEAPON::FIRE_ORDER order)
{
	if( m_pOwner == nullptr ) return false;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )		return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	REAL32	rStartTime = 0;
	REAL32	rTimeScale = 1.f;

	if( i3::kind_of<WeaponDualHandGun*>(pWeapon))
	{		
	
		if( i3::same_of<WeaponCrossFire*>(pWeapon)
			&& pWeapon->isExtensionActivate())
		{
			MainWeapon_CrossFire* mainWeapon_CrossFire = static_cast<MainWeapon_CrossFire*>(pWeapon->GetMainWeapon());

			if( mainWeapon_CrossFire->isDualFired() )
			{
				_ActionFireDual( rStartTime, rTimeScale, true );
			}

			_ActionFireDual( rStartTime, rTimeScale, false );

			if( m_pOwner->is1PV())	{
				if( m_pOwner->isNetwork()) {
					m_pOwner->setAttackIdleTime( 1.f);		// 1.초간 발사 동작을 유지합니다.
				}
			}
			else
				m_pOwner->setAttackIdleTime( 1.f);		// 1.초간 발사 동작을 유지합니다.
		}
		else
		{
			//WeaponDualHandGun * pDualHandGun = (WeaponDualHandGun*)pWeapon;
			// 각각의 총기에 대해서 발사 상태에 있는 총을 플레이
			MainWeapon_DualHandGun* mainWeapon_DualHandGun = static_cast<MainWeapon_DualHandGun*>(pWeapon->GetMainWeapon());

			if( mainWeapon_DualHandGun->getNextUseFireWeapon() == 1)
			{
				#if defined( USE_RANDOM_FIRE_STARTTIME)
					rTimeScale = 1.f + i3Math::Randf2() * 0.3f;
				#endif
				_ActionFireDual( rStartTime, rTimeScale, true );
			}

			if( mainWeapon_DualHandGun->getNextUseFireWeapon() == 0)
			{
				#if defined( USE_RANDOM_FIRE_STARTTIME)
					rTimeScale = 1.f + i3Math::Randf2() * 0.3f;
				#endif
				_ActionFireDual( rStartTime, rTimeScale, false );
			}
		}
	}
	else if( i3::kind_of<WeaponDualSMG*>(pWeapon))
	{
	#if defined( USE_RANDOM_FIRE_STARTTIME)
		rTimeScale = 1.f + i3Math::Randf2() * 0.3f;
	#endif
		_ActionFireDual( rStartTime, rTimeScale, true );

	#if defined( USE_RANDOM_FIRE_STARTTIME)
		rTimeScale = 1.f + i3Math::Randf2() * 0.3f;
	#endif
		_ActionFireDual( rStartTime, rTimeScale, false );
	}
	else if ( i3::kind_of<WeaponDualShotGun*>(pWeapon) )
	{
		MainWeapon_DualShotGun * pMainWeapon = static_cast<MainWeapon_DualShotGun*>(pWeapon->GetMainWeapon());

		if( pMainWeapon->isDualFire())
		{
			rTimeScale = pMainWeapon->GetDualShotGunTimeScale();
		}
		else
		{
		#if defined( USE_RANDOM_FIRE_STARTTIME)
			rTimeScale = 1.f + i3Math::Randf2() * 0.3f;
		#endif
		}

		if( pMainWeapon->isRightFire() )
			_ActionFireDual( rStartTime, rTimeScale, true);

		if( pMainWeapon->isLeftFire() )
			_ActionFireDual( rStartTime, rTimeScale, false);
	}
	else
	{	
		CHARA_ACTION_UPPER	state = CHARA_UPPER_NONE;
		ID_UPPER_AI nUpperAiId = ID_UPPER_UNKNOWN;

		//	주 공격
		if( order == WEAPON::PRIMARY_FIRE)
		{
			state = CHARA_UPPER_FIRE;
			nUpperAiId = ID_UPPER_ATTACK;
		}
		//	보조 공격
		else if( order == WEAPON::SECONDARY_FIRE)
		{
			state = CHARA_UPPER_SECONDARY_FIRE;
			nUpperAiId = ID_UPPER_SECONDARY_ATTACK;
		}

		I3ASSERT( state != CHARA_UPPER_NONE && nUpperAiId != ID_UPPER_UNKNOWN);

		rTimeScale = pWeapon->GetAnimFireScaleTime( state, nUpperAiId);

		//	미션전에서 C4 폭탄 설치할때만 작용
		if( pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_MISSION)
		{
			////-----------------	C4 Speed Kit
			i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
			I3ASSERT( pAIUpper != nullptr);

		#if defined( AI_MOVE_TO_WEAPON)
			INT32 nAIIdx = pWeapon->getCharaAIIndex( m_pOwner->getAnimSet(), getActionContext()->getBodyUpper(), nUpperAiId);
		#else
			INT32 nAIIdx = getAnimContext()->getAIUpperID( pWeaponInfo->GetTypeClass(), pWeaponInfo->GetNumber(), nUpperAiId);
		#endif

			I3ASSERT( nAIIdx >= 0);

			i3AIState * pAi = pAIUpper->getAIState( nAIIdx);
			if( pAi != nullptr)	// revision 36652 
			{
				//	애니 TimeScale을 계산하기 위해 애니메이션의 duration을 가져와야 한다.
				//REAL32 fDuration = pAi->getDuration();

				REAL32 fRatio = BattleSlotContext::i()->getMissionBomb_SetTime_UsingSpeedKit(m_pOwner->getCharaNetIndex()) / WEAPON_BOMB_SETTIME;
				
				CASHITEM_TRACE("CASHITEMSKILL_C4SPEED = %f\n", fRatio);
				
				//	애니 속도 증가/감소
				rTimeScale = ( (1 / fRatio) );				
			}
		}
		else
		{
			#if defined( USE_RANDOM_FIRE_STARTTIME)
			if( pWeapon->isBulletFireWeapon() &&
				!i3::kind_of<WeaponShotGun*>(pWeapon)
				&&	order == WEAPON::PRIMARY_FIRE &&
				(i3::kind_of<WeaponShotGunLauncher*>(pWeapon)
				&& !pWeapon->isExtensionActivate()) )
			{
				//rStartTime = i3Math::Randf() * 0.05f;
				rTimeScale = 1.f + i3Math::Randf2() * 0.3f;
			}
			#endif
		}

		// 발사!!
		setUpper( state);
		_Animation_Upper( nUpperAiId, rStartTime, rTimeScale);

		// C4는 AttackIdleTime을 계산하지 않습니다.
		// UDP를 받으면 처리하는 방식이기에 자동으로 Idle을 처리하지 않습니다. dfly79 2009.03.10
		if( i3::same_of<WeaponC4*>(pWeapon) == false)
		{
			if( m_pOwner->is1PV())	{
				if( m_pOwner->isNetwork()) {
					m_pOwner->setAttackIdleTime( pWeapon->GetFireDelay() + 0.5f - 2.0f);
				}
			}
			else if(m_pOwner->is3PV()) {
				m_pOwner->setAttackIdleTime( pWeapon->GetFireDelay() + 0.5f - 2.0f);		// 0.5초간 발사 동작을 유지합니다.
			}
		}
	}

	return true;
}

/*virtual*/ bool CGameCharaActionControl::OnActionReadyToFire( void)
{

	WeaponBase * pWeapon = nullptr;
	
	pWeapon = getUsedWeapon();
	if (pWeapon == nullptr) return false;

	m_pOwner->Cmd_SyncWeapon();

	pWeapon->PlayAnimation_AttackReady();

	// 수류탄, 견제형, 공격형 투척 무기 사운드 출력

	switch(pWeapon->getWeaponInfo()->GetTypeClass())
	{
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_THROWING_ATTACK:
		{
			g_pSndMng->StartWeaponStateSnd(GTBWS_ON_READY_GRENADE,0,m_pOwner->GetPos(), m_pOwner->is1PV()?true:false);
		}
		break;
	default:	break;
	}

	setUpper( CHARA_UPPER_FIRE_READY);
	_Animation_Upper( ID_UPPER_ATTACK_READY);

	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	발사 준비중에 발사하지 못하도록. 애니메이션이 끝나면 발사 가능

	return true;
}

bool CGameCharaActionControl::OnActionExtensionWeapon( bool bActivate)
{
	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )		return false;

	//	무기 Extension 기능 활성화
	if( bActivate)
	{
		switch(  pWeapon->GetCurrentExtensionType() )
		{
		case	WEAPON::EXT_GLAUNCHER :
			_ApplyActionBody2( CHARA_BODYUPPER_GRENADE);	
			break;
		case	WEAPON::EXT_SHOTGUN_LAUNCHER :	_ApplyActionBody2( CHARA_BODYUPPER_SHOTGUN);	break;
		// revision 63443 _ApplyActionBody2( CHARA_BODYUPPER_KNIFE); 견착후 익스텐션 사용시 견착한 상태로 익스텐션 AI를 불러 이상한 애니메이션이 실행되는 문제 ( 13.08.26 영정 )
		case	WEAPON::EXT_BAYONET :			_ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);	break;
		case	WEAPON::EXT_CROSSFIRE :			_ApplyActionBody2( CHARA_BODYUPPER_CROSSFIRE);	break;
		}

		setUpper( CHARA_UPPER_EXTENSION_ACTIVATE);
		if( pWeapon->isDualGun()) setUpperLeft( CHARA_UPPER_EXTENSION_ACTIVATE);
		_Animation_Upper( ID_UPPER_EXTENSION_ACTIVATE);
	}
	else
	{
		switch(  pWeapon->GetCurrentExtensionType() )
		{
		case	WEAPON::EXT_GLAUNCHER :
		case	WEAPON::EXT_SHOTGUN_LAUNCHER :
		case	WEAPON::EXT_BAYONET :
		case	WEAPON::EXT_CROSSFIRE :
			_ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);
			break;
		}

		setUpper( CHARA_UPPER_EXTENSION_DEACTIVATE);
		if( pWeapon->isDualGun()) setUpperLeft( CHARA_UPPER_EXTENSION_DEACTIVATE);
		_Animation_Upper( ID_UPPER_EXTENSION_DEACTIVATE);
	}

	//	애니메이션이 끝나면 발사 가능
	if( !m_pOwner->isNetwork())
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	return true;
}

/*virtual*/ bool CGameCharaActionControl::OnActionActiveZoom( void)
{
	bool bResult = false;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )	return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	WEAPON::ZOOM_TYPE type = pWeaponInfo->GetTypeZoom();

	//	견착 기능이 있는 무기
	switch( type)
	{
		case  WEAPON::ZOOM_ZOOM:
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				m_pOwner->addCharaStateMask( CHARA_STATE_ZOOM);
				_ApplyActionBody2( CHARA_BODYUPPER_ZOOM);
				bResult = true;
			}
			break;

		case WEAPON::ZOOM_SCOPE :
			//	스코프 기능이 있는 무기
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				m_pOwner->addCharaStateMask( CHARA_STATE_ZOOM);
				_ApplyActionBody2( CHARA_BODYUPPER_ZOOM);
				bResult = true;
			}
			break;

		case WEAPON::ZOOM_STEADY_AIM :
			// 다른 어떤 Animation 및 Crosshair의 변경없이 FOV만을 조절
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				m_pOwner->addCharaStateMask( CHARA_STATE_ZOOM);
				bResult = true;
			}
			break;
	}

	//신규 점사 방식 작동 중 이라면, 점사 발사 타임 유지 시킨다.
	if (pWeapon->isWeaponStateMask(WSM_POINTSHOTING))
	{
		pWeapon->addWeaponStateMask(WSM_WAIT_AUTOFIREDELAY);

		if (m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM) != false)
			pWeapon->setRemainAutoFireCount(pWeaponInfo->GetAutoFireCount());
		else
			pWeapon->setRemainAutoFireCount(pWeaponInfo->GetExtAutoFireCount());
	}

	m_pOwner->Cmd_ApplyCrosshair();

	return bResult;
}

bool CGameCharaActionControl::OnActionCancelZoom( void)
{
	//	Zoom 취소
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
	{
		_ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);
		return true;
	}

	return false;
}

bool CGameCharaActionControl::OnActionSwapWeapon( void)
{
	if( m_pOwner == nullptr ) return false;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		return false;

	_ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);
	
	//	이미 캐릭터는 다음 무기의 동작으로 설정한다.
	getAnimContext()->ChangeAnimationContext();

	if( getAnimContext()->getAIUpper_Left() != nullptr)
	{
		setUpperLeft( CHARA_UPPER_SWAPWEAPON);
	}

	REAL32 rTimeScale = _CalcSwapTimeScaleToVersion();

	//	교체가 끝나고 애니메이션을 한다.
	setUpper( CHARA_UPPER_SWAPWEAPON);
	_Animation_Upper( ID_UPPER_SWAP_WEAPON, 0.f, rTimeScale);

#if 0 //1.1 소스에 이런 기능이 있지만, 사용 유무를 몰라 막기만 함.
	if (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
	{
		if (rTimeScale > 1.6f && !BattleSlotContext::i()->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_QUICK_CHANGE_WEAPON) )
		{
			HackContext::i()->HackingDetected(CC_QUICK_CHANGE,C_QUICK_CHANGE);
		}
		else if (rTimeScale >1.0f)
		{
			if( BattleSlotContext::i()->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_QUICK_CHANGE_WEAPON) )
			{

			}
			else
			{
				HackContext::i()->HackingDetected(CC_QUICK_CHANGE,C_QUICK_CHANGE);
			}
		}
	}
#endif
	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	return true;
}

bool CGameCharaActionControl::OnActionLoad_B_Magazine( void)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	REAL32 rTimeScale = 1.f;
	ID_UPPER_AI eUpperAI = ID_UPPER_UNKNOWN;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )		return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	CWeaponInfo * pAbsoluteWeaponInfo = pWeapon->getAbsoluteWeaponInfo();		// 주무기 애니메이션 사용을 위해 이용
	I3ASSERT( pWeaponInfo != nullptr);
	I3ASSERT( pAbsoluteWeaponInfo != nullptr);

	//	총기 계열만 가능
	if(!pWeaponInfo->isGun())	return false;

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	i3AIContext * pAIUpper_Left = getAnimContext()->getAIUpper_Left();

	if(pWeapon->EnablePlayMasterAnim())
	{
		if(pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SHOTGUN)
		{
			eUpperAI = ID_UPPER_LOAD_MAGAZINE_SHOTGUN_MASTER_B;
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN,  "OnActionLoad_B_Magazine에서 샷건이 아닌데 호출 되었습니다.");
		}
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN,  "OnActionLoad_B_Magazine에서 마스터가 아닌데 호출 되었습니다.");
	}

	#if defined( AI_MOVE_TO_WEAPON)
		INT32 nID = pWeapon->getCharaAIIndex( m_pOwner->getAnimSet(), getActionContext()->getBodyUpper(), eUpperAI);
	#else
		INT32 nID = getAnimContext()->getAIUpperID( pAbsoluteWeaponInfo->GetTypeClass(), pAbsoluteWeaponInfo->GetNumber(), eUpperAI);
	#endif

	if( pAIUpper != nullptr )
	{
		I3ASSERT( nID >= 0);
		I3ASSERT( nID < pAIUpper->getAIStateCount());
		i3AIState * pNewAIState = pAIUpper->getAIState( nID );
		if( pNewAIState != nullptr )
		{
			if(pWeapon->EnablePlayMasterAnim() && pWeapon->IsFastReloadAnimWeapon() )
			{
				rTimeScale = 1.f;
			}
			else
			{
				rTimeScale = MASTER_RELOAD_SPEED_RATIO;
			}
		}
	}

	if( pAIUpper_Left != nullptr)
	{
		setUpperLeft( CHARA_UPPER_LOADMAGAZINE);
	}

	setUpper( CHARA_UPPER_LOADMAGAZINE);
	_Animation_Upper( eUpperAI, 0.f, rTimeScale);

	if( pWeaponInfo->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT )
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	else
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	return true;
}

bool CGameCharaActionControl::OnActionLoadMag_Ready( void)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	ID_UPPER_AI eUpperAI = ID_UPPER_LOAD_MAGAZINE_READY;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )	return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

	I3ASSERT( pWeaponInfo != nullptr);
	//	총기 계열만 가능
	if(!pWeaponInfo->isGun())
		return false;

	setUpper( CHARA_UPPER_LOADMAG_READY);
	_Animation_Upper( eUpperAI );

	if( pWeaponInfo->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT )
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	else
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	return true;
}

ID_UPPER_AI CGameCharaActionControl::_ValidReloadAnimationID( WeaponBase * pWeapon, CWeaponInfo * pWeaponInfo)
{
	I3ASSERT( pWeapon != nullptr);
	I3ASSERT( pWeaponInfo != nullptr);

	if( pWeapon->EnablePlayMasterAnim())
	{
		if( pWeaponInfo->isShotgunMasterC() )
			return ID_UPPER_LOAD_MAGAZINE_SHOTGUN_MASTER_C;
		else if( pWeapon->IsFastReloadAnimWeapon())
			return ID_UPPER_LOAD_MAGAZINE_MASTER;
	}
	else if(i3::same_of<WeaponShotGunMaleFemale*>(pWeapon))
	{
		
		if(  pWeapon->getOwner()->isLoadMagChg() == true)
		{
			pWeapon->getOwner()->SetLoadMagChg(false);
			return ID_UPPER_LOAD_MAGAZINE;
		}
		else
		{
			pWeapon->getOwner()->SetLoadMagChg(true);
			return ID_UPPER_LOAD_MAGAZINE2;
		}

	}

	return ID_UPPER_LOAD_MAGAZINE;
}

ID_LOWER_AI CGameCharaActionControl::GetLowerAnimID( LOWER_AI_TYPE Type, LOWER_AI_DIR Dir )
{
	ID_LOWER_AI	AI_ID = ID_LOWER_UNKNOWN;

	switch(Type)
	{
	case LOWER_AI_DEATH_A:
		{
			switch(Dir)
			{
			case LOWER_AI_DIR_FRONT:
				AI_ID = static_cast<ID_LOWER_AI>(ID_LOWER_DEATH_FRONT_A1 + (i3Math::Rand() % MAX_DEATH_FRONT_A_MOTION));
				break;

			case LOWER_AI_DIR_LEFT:		AI_ID = ID_LOWER_DEATH_LEFT_A;	break;
			case LOWER_AI_DIR_RIGHT:	AI_ID = ID_LOWER_DEATH_RIGHT_A;	break;
			case LOWER_AI_DIR_BACK:		AI_ID = ID_LOWER_DEATH_BACK_A;	break;
			}
		}
		break;
	case LOWER_AI_DEATH_B:
		{
			switch(Dir)
			{
			case LOWER_AI_DIR_FRONT:	AI_ID = ID_LOWER_DEATH_FRONT_B;	break;
			case LOWER_AI_DIR_LEFT:		AI_ID = ID_LOWER_DEATH_LEFT_B;	break;
			case LOWER_AI_DIR_RIGHT:	AI_ID = ID_LOWER_DEATH_RIGHT_B;	break;
			case LOWER_AI_DIR_BACK:		AI_ID = ID_LOWER_DEATH_BACK_B;	break;
			}
		}
		break;
	case LOWER_AI_DEATH_H:
		{
			switch(Dir)
			{
			case LOWER_AI_DIR_FRONT:
				AI_ID = static_cast<ID_LOWER_AI>(ID_LOWER_DEATH_FRONT_H1 + (i3Math::Rand() % MAX_DEATH_FRONT_H_MOTION));
				break;

			case LOWER_AI_DIR_LEFT:		AI_ID = ID_LOWER_DEATH_LEFT_H;	break;
			case LOWER_AI_DIR_RIGHT:	AI_ID = ID_LOWER_DEATH_RIGHT_H;	break;
			case LOWER_AI_DIR_BACK:		AI_ID = ID_LOWER_DEATH_BACK_H;	break;
			}
		}
		break;
	}

	return AI_ID;
}

ID_LOWER_DINO_AI CGameCharaActionControl::GetLowerAnimIDForDino( LOWER_AI_TYPE Type, LOWER_AI_DIR Dir )
{
	ID_LOWER_DINO_AI	AI_ID = ID_LOWER_DINO_UNKNOWN;

	switch(Type)
	{
	case LOWER_AI_DEATH_A:
		{
			switch(Dir)
			{
			case LOWER_AI_DIR_FRONT:
				AI_ID = static_cast<ID_LOWER_DINO_AI>(ID_LOWER_DINO_DEATH_FRONT_A1 + (i3Math::Rand() % MAX_DEATH_FRONT_A_MOTION));
				break;
			case LOWER_AI_DIR_LEFT:		AI_ID = ID_LOWER_DINO_DEATH_LEFT_A;	break;
			case LOWER_AI_DIR_RIGHT:	AI_ID = ID_LOWER_DINO_DEATH_RIGHT_A;	break;
			case LOWER_AI_DIR_BACK:		AI_ID = ID_LOWER_DINO_DEATH_BACK_A;	break;
			}
		}
		break;
	case LOWER_AI_DEATH_B:
		{
			switch(Dir)
			{
			case LOWER_AI_DIR_FRONT:	AI_ID = ID_LOWER_DINO_DEATH_FRONT_B;	break;
			case LOWER_AI_DIR_LEFT:		AI_ID = ID_LOWER_DINO_DEATH_LEFT_B;	break;
			case LOWER_AI_DIR_RIGHT:	AI_ID = ID_LOWER_DINO_DEATH_RIGHT_B;	break;
			case LOWER_AI_DIR_BACK:		AI_ID = ID_LOWER_DINO_DEATH_BACK_B;	break;
			}
		}
		break;
	case LOWER_AI_DEATH_H:
		{
			switch(Dir)
			{
			case LOWER_AI_DIR_FRONT:
				AI_ID = static_cast<ID_LOWER_DINO_AI>(ID_LOWER_DINO_DEATH_FRONT_H1 + (i3Math::Rand() % MAX_DEATH_FRONT_H_MOTION));
				break;
			case LOWER_AI_DIR_LEFT:		AI_ID = ID_LOWER_DINO_DEATH_LEFT_H;	break;
			case LOWER_AI_DIR_RIGHT:	AI_ID = ID_LOWER_DINO_DEATH_RIGHT_H;	break;
			case LOWER_AI_DIR_BACK:		AI_ID = ID_LOWER_DINO_DEATH_BACK_H;	break;
			}
		}
		break;
	}

	return AI_ID;
}

bool CGameCharaActionControl::OnActionLoadMagazine( void)
{
	if( m_pOwner == nullptr ) return false;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	REAL32 rTimeScale = 1.f;
	ID_UPPER_AI eUpperAI = ID_UPPER_UNKNOWN;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )		return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	CWeaponInfo * pAbsoluteWeaponInfo = pWeapon->getAbsoluteWeaponInfo();		// 주무기 애니메이션 사용을 위해 이용
	I3ASSERT( pWeaponInfo != nullptr);
	I3ASSERT( pAbsoluteWeaponInfo != nullptr);

	//	총기 계열만 가능
	if(!pWeaponInfo->isGun())	return false;

	// 유탄 발사기의 장전동작은 강제로 세팅하도록 합니다.
	// KEYWORD : WEAPON_GRENADELAUNCHER
	if( i3::kind_of<WeaponGrenadeLauncher*>(pWeapon)
		&& pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER
		&& pWeapon->isExtensionActivate())
	{
		getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_GRENADE);
	}
	// 샷건 발사기
	// KEYWORD : WEAPON_SHOTGUNLAUNCHER
	else if( i3::kind_of<WeaponShotGunLauncher*>(pWeapon)
		&& pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER
		&& pWeapon->isExtensionActivate())
	{
		getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_SHOTGUN);
	}

	eUpperAI = _ValidReloadAnimationID( pWeapon, pWeaponInfo);

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	i3AIContext * pAIUpper_Left = getAnimContext()->getAIUpper_Left();

	#if defined( AI_MOVE_TO_WEAPON)
		INT32 nID = pWeapon->getCharaAIIndex( m_pOwner->getAnimSet(), getActionContext()->getBodyUpper(), eUpperAI);
	#else
		INT32 nID = getAnimContext()->getAIUpperID( pAbsoluteWeaponInfo->GetTypeClass(), pAbsoluteWeaponInfo->GetNumber(), eUpperAI);
	#endif

	if( pAIUpper != nullptr )
	{
		I3ASSERT( nID >= 0);
		I3ASSERT( nID < pAIUpper->getAIStateCount());
		i3AIState * pNewAIState = pAIUpper->getAIState( nID );
		if( pNewAIState != nullptr )
		{
			// 무기를 주운경우에는 애니메이션을 아직 설정하지 않은 관계로 여기서 애니메이션을 로딩해야한다.
			CGameFramework::LoadAIStateAnimation(pNewAIState);		

			rTimeScale = _CalcReloadTimeScaleToVersion(pWeapon, pNewAIState->getAnimDuration() );
		}
	}

	if( pAIUpper_Left != nullptr)
	{
		setUpperLeft( CHARA_UPPER_LOADMAGAZINE);
	}

	setUpper( CHARA_UPPER_LOADMAGAZINE);
	_Animation_Upper( eUpperAI, 0.f, rTimeScale);

	if( pWeaponInfo->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT )
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	else
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	return true;
}

bool CGameCharaActionControl::OnActionLoadMagazineDual( void)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)	return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	if( pWeaponInfo->isGun())
	{
		setUpper( CHARA_UPPER_EXTENSION_ACTIVATE);
		_Animation_Upper( ID_UPPER_EXTENSION_ACTIVATE);

		if( pWeaponInfo->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT )
			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		return true;
	}

	return false;
}

bool CGameCharaActionControl::OnActionLoadBullet( void)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr) return false;

	I3ASSERT( pWeapon->getWeaponInfo() != nullptr);
	if( pWeapon->getWeaponInfo()->isGun())
	{
		setUpper( CHARA_UPPER_LOADBULLET);
		_Animation_Upper( ID_UPPER_LOAD_BULLET, 0.f, pWeapon->GetAnimLoadBulletScaleTime( m_pOwner->getViewType()) );
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		return true;
	}

	return false;
}

bool CGameCharaActionControl::OnActionRemoveShell( void)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon != nullptr && pWeapon->isExtensionActivate() )
	{
		setUpper( CHARA_UPPER_LOADBULLET);
		_Animation_Upper( ID_UPPER_LOAD_BULLET);
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		return true;
	}

	return false;
}

/*virtual*/ bool CGameCharaActionControl::OnActionJumpUpperFinish( void)
{
	if(m_pOwner->IsDino())
	{
		setUpper( CHARA_UPPER_IDLE);
		_Animation_DinoUpper( ID_UPPER_DINO_JUMP_END);
	}
	else
	{
		m_pOwner->Cmd_SyncWeapon();
		
		WeaponBase * pWeapon = getUsedWeapon();
		if( pWeapon == nullptr)	return false;

		setUpper( CHARA_UPPER_IDLE);

		if( pWeapon->isDualGun())
		{
			_Animation_DualUpper( ID_UPPER_DUAL_JUMP_END_RIGHT);
		}
		else
		{
			_Animation_Upper( ID_UPPER_JUMP_END);
		}
	}

	return true;
}

/*virtual*/ bool CGameCharaActionControl::OnActionForceJumpStart( VEC3D * pDir, REAL32 rSpeed, REAL32 rJumpPower)
{
	if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))	return false;

	getMoveContext()->ResetJumping();

	if (!m_pOwner->isNetwork())
	{
		VEC3D dir;
		i3Vector::Normalize(&dir, pDir);

		getMoveContext()->setForceJumpDir(&dir);
		getMoveContext()->setForceJumpSpeed(rSpeed);
		getMoveContext()->setCurJumpHeight(rJumpPower);
	}
	
	m_pOwner->addCharaStateMask(CHARA_STATE_FORCEJUMP);
	m_pOwner->addCharaStateMask(CHARA_STATE_JUMP);
	m_pOwner->addCharaStateMask(CHARA_STATE_JUMPING_UP);
	getMoveContext()->setElapsedTakeOffTime(0.f);
	__Play_Animation_Jump();
	
	return true;
}

/*virtual*/ bool CGameCharaActionControl::OnActionJumpUpperLeftFinish( void)
{
	if(m_pOwner->IsDino())		return false;
	else
	{
		m_pOwner->Cmd_SyncWeapon();
		
		WeaponBase * pWeapon = getUsedWeapon();
		if( pWeapon == nullptr)	return false;

		if( pWeapon->isDualGun() == false)	return false;

		setUpperLeft( CHARA_UPPER_IDLE);
		_Animation_DualUpper( ID_UPPER_DUAL_JUMP_END_LEFT);
	}

	return true;
}

bool CGameCharaActionControl::OnActionFireBullet( void)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr) return false;

	I3ASSERT( pWeapon->getWeaponInfo() != nullptr);
	if( pWeapon->getWeaponInfo()->isGun())
	{
		setUpper( CHARA_UPPER_FIREBULLET);
		_Animation_Upper( ID_UPPER_FIRE_BULLET, 0.f, pWeapon->GetAnimFireBulletScaleTime( m_pOwner->getViewType()) );
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaActionControl::ActionToggleIdle( void)
{
	/*if( isBody2( CHARA_BODYUPPER_ATTACKIDLE))
	{
		_ApplyActionBody2(CHARA_BODYUPPER_IDLE);
	}
	else if( isBody2( CHARA_BODYUPPER_IDLE))
	{
		_ApplyActionBody2(CHARA_BODYUPPER_ATTACKIDLE);
	}*/
}

bool CGameCharaActionControl::ActionLowerReturn( void)
{
	TRACE_FUNC;

	//if( isBody2( CHARA_BODYUPPER_IDLE) )
	{
		//_ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);
	}

	setLower( CHARA_LOWER_IDLE);
	if (m_pOwner->IsDino())
	{
		_Animation_LowerForDino( ID_LOWER_DINO_IDLE);
	}
	else
	{
		_Animation_Lower( ID_LOWER_IDLE);
	}

	return true;
}

bool CGameCharaActionControl::ActionUpperReturn( void)
{	//	이전에 동작중인 Upper 애니메이션으로 돌아간다.
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;
	if( m_pOwner->getAttackIdleTime() > 0.f)				return false;

	if(m_pOwner->IsDino())
	{
		//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
		REAL32 fSyncTime = getAnimContext()->GetLowerSyncTime();

		if( isLower( CHARA_LOWER_RUN) )
		{
			// 이전 Move 동작으로
			setUpper( CHARA_UPPER_MOVE);
			_Animation_DinoUpper( ID_UPPER_DINO_MOVE, fSyncTime);
		}
		else
		{
			//	이전 Idle 동작으로
			setUpper( CHARA_UPPER_IDLE);
			_Animation_DinoUpper( ID_UPPER_DINO_IDLE, fSyncTime);
		}
	}
	else
	{
		WeaponBase * pWeapon = getUsedWeapon();
		if( pWeapon == nullptr)	return false;

		//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
		REAL32 fSyncTime = getAnimContext()->GetLowerSyncTime();

		if( pWeapon->isDualGun())
		{
			if( isLower( CHARA_LOWER_RUN))
			{
				setUpper( CHARA_UPPER_MOVE);
				_Animation_DualUpper( ID_UPPER_DUAL_MOVE_RIGHT, fSyncTime);

				setUpperLeft( CHARA_UPPER_MOVE);
				_Animation_DualUpper( ID_UPPER_DUAL_MOVE_LEFT, fSyncTime);
			}
			else
			{
				setUpper( CHARA_UPPER_IDLE);
				_Animation_DualUpper( ID_UPPER_DUAL_IDLE_RIGHT, fSyncTime);

				setUpperLeft( CHARA_UPPER_IDLE);
				_Animation_DualUpper( ID_UPPER_DUAL_IDLE_LEFT, fSyncTime);
			}
		}
		else
		{
			if( isLower( CHARA_LOWER_RUN) )
			{
				// 이전 Move 동작으로
				setUpper( CHARA_UPPER_MOVE);
				_Animation_Upper( ID_UPPER_MOVE, fSyncTime);
			}
			else
			{
				// revision 39192 수류탄 발사 준비 -> 점프완료시 상체 애니메이션 IDLE로 변경되는부분 수정
				// 이전 상체 동작이 발사 대기중이면 처리안합니다.(Grenade)
				// 그 외 상체동작은 IDEL 동작으로 바꿉니다.
				if(isUpper(CHARA_UPPER_FIRE_READY) == false)
				{
					setUpper( CHARA_UPPER_IDLE);
					_Animation_Upper( ID_UPPER_IDLE, fSyncTime);
				}
			}
		}
	}


	return true;
}

bool CGameCharaActionControl::ActionUpperLeftReturn( void)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;
	if( m_pOwner->getAttackIdleTimeLeft() > 0.f)			return false;

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)	return false;

	//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
	REAL32 fSyncTime = getAnimContext()->GetLowerSyncTime();

	if( isLower( CHARA_LOWER_RUN) )
	{
		setUpperLeft( CHARA_UPPER_MOVE);
		_Animation_DualUpper( ID_UPPER_DUAL_MOVE_LEFT, fSyncTime);
	}
	else
	{
		setUpperLeft( CHARA_UPPER_IDLE);
		_Animation_DualUpper( ID_UPPER_DUAL_IDLE_LEFT, fSyncTime);
	}

	return true;
}

bool CGameCharaActionControl::ActionUpperRightReturn( void)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;
	if( m_pOwner->getAttackIdleTime() > 0.f)	return false;

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)	return false;

	//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
	REAL32 fSyncTime = getAnimContext()->GetLowerSyncTime();
	if( isLower( CHARA_LOWER_RUN) )
	{
		setUpper( CHARA_UPPER_MOVE);
		_Animation_DualUpper( ID_UPPER_DUAL_MOVE_RIGHT, fSyncTime);
	}
	else
	{
		setUpper( CHARA_UPPER_IDLE);
		_Animation_DualUpper( ID_UPPER_DUAL_IDLE_RIGHT, fSyncTime);
	}
	

	return true;
}

bool CGameCharaActionControl::ActionRunUpperLeft( void)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;

	m_pOwner->Cmd_SyncWeapon();

	i3AIContext * pAIUpperLeft = getAnimContext()->getAIUpper_Left();

	//	현재 이동 중이다. ( 이동 애니메이션 동작시간과 맞춰 플레이)
	if( pAIUpperLeft != nullptr)	fSyncTime = pAIUpperLeft->getLocalTime();

	REAL32 fScaleTime = _CalcRunUpperScaleTime();

	ID_UPPER_DUAL_AI leftAI = getAnimContext()->getOldUpperDualLeftID();

	if( leftAI != ID_UPPER_DUAL_MOVE_LEFT)
	{
		setUpperLeft( CHARA_UPPER_MOVE);
		_Animation_DualUpper( ID_UPPER_DUAL_MOVE_LEFT, fSyncTime, fScaleTime);
	}
	else
	{
		getAnimContext()->SetUpperLeftTimeScale( fScaleTime);
	}

	return true;
}

bool CGameCharaActionControl::ActionRunUpperRight( void)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;
	//if( m_pOwner->getAttackIdleTime() > 0.f)	return false;

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;

	m_pOwner->Cmd_SyncWeapon();

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();

	//	현재 이동 중이다. ( 이동 애니메이션 동작시간과 맞춰 플레이)
	if( pAIUpper != nullptr)	fSyncTime = pAIUpper->getLocalTime();

	REAL32 fScaleTime = _CalcRunUpperScaleTime();

	ID_UPPER_DUAL_AI rightAI = getAnimContext()->getOldUpperDualRightID();
	if( rightAI != ID_UPPER_DUAL_MOVE_RIGHT)
	{
		setUpper( CHARA_UPPER_MOVE);
		_Animation_DualUpper( ID_UPPER_DUAL_MOVE_RIGHT, fSyncTime, fScaleTime);
	}
	else
		getAnimContext()->SetUpperRightTimeScale( fScaleTime);

	return true;
}

REAL32 CGameCharaActionControl::_CalcRunDinoUpperScaleTime( void)
{
	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaAnimContext * pAnimCtx = getAnimContext();

	bool calced = false;
	REAL32 fScaleTime = 1.f;
	if( pAnimCtx != nullptr)
	{
		i3AIContext * pAICtx = pAnimCtx->getAILower();
		if( pAICtx != nullptr)
		{
			i3AIState * pAI = pAICtx->getCurrentAIState();
			if( pAI != nullptr)
			{
				fScaleTime = pAI->getTimeScale();
				calced = true;
			}
		}
	}

	if( calced == false)
	{
		switch( pActionCtx->getLower() )
		{
		case CHARA_LOWER_WALK :
			fScaleTime = m_pOwner->GetMoveSpeedAddOffset( MST_WALK, MDT_FRONT) / m_pOwner->GetMoveSpeedAddOffset( MST_RUN, MDT_FRONT);
			break;
		}
	}

	return fScaleTime;
}

REAL32 CGameCharaActionControl::_CalcRunUpperScaleTime( void)
{
	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr) return false;

	REAL32 fScaleTime = 1.f;

	switch( getActionContext()->getLower() )
	{
	case CHARA_LOWER_WALK :
		fScaleTime = m_pOwner->GetMoveSpeedAddOffset( MST_WALK, MDT_FRONT) / m_pOwner->GetMoveSpeedAddOffset( MST_RUN, MDT_FRONT);
		break;
	}

	REAL32	rScaleRate = 0.0f;
	if( getActionContext()->isBody( CHARA_BODYLOWER_STAND)) {
		//	기본 서있는 상태에서
		rScaleRate = pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_NORMAL) * 0.01f;
	}
	else if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH)) {
		//	기본 앉아있는 상태에서
		rScaleRate = pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_SIT) * 0.01f;
	}

	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
	{
		//	줌인 중이지만 장전시에는 적용안되게...
		if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) &&
			!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET) &&
			!getActionContext()->isUpper( CHARA_UPPER_FIREBULLET))
			rScaleRate *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_ZOOM) * 0.01f;
	}

	//	발사 상태일때
	if( getActionContext()->isUpper( CHARA_UPPER_FIRE) ||
		getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) ||
		(getActionContext()->getUpperLeft() == CHARA_UPPER_FIRE) )
	{
		rScaleRate *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_FIRE) * 0.01f;
	}
	fScaleTime *= rScaleRate * 1.25f;

	return fScaleTime;
}

REAL32 CGameCharaActionControl::_CalcRunLowerDinoScaleTime( ID_LOWER_DINO_AI nId, REAL32 rSpeed)
{
	REAL32 fScaleTime = 1.f;

	if( m_pOwner->IsRex())
	{
		REAL32 fMoveVelocity	= m_pOwner->GetMoveSpeedAddOffset( MST_RUN, MDT_FRONT);
		REAL32 _Speed			= rSpeed * (1.0f / g_pViewer->getDeltaTime());

		if( (rSpeed > I3_EPS) && (g_pViewer->getDeltaTime() > I3_EPS) )
		{
			REAL32 runSpeed = 12.845f;
			REAL32 runsideSpeed = 5.478f;
			REAL32 walkSpeed = 3.017f;
			REAL32 walkSideSpeed = 1.7f;

			switch( nId )
			{
			case ID_LOWER_DINO_MOVE_FRONT :				fMoveVelocity	= runSpeed;	break;

			case ID_LOWER_DINO_MOVE_FRONT_LEFT :			fMoveVelocity	= runSpeed;	break;
			case ID_LOWER_DINO_MOVE_BACK :				fMoveVelocity	= runSpeed;	break;
			case ID_LOWER_DINO_MOVE_FRONT_RIGHT :		fMoveVelocity	= runSpeed;	break;
			case ID_LOWER_DINO_MOVE_BACK_LEFT :			fMoveVelocity	= runSpeed;	break;
			case ID_LOWER_DINO_MOVE_BACK_RIGHT :			fMoveVelocity	= runSpeed;	break;

			case ID_LOWER_DINO_MOVE_LEFT :				fMoveVelocity	= runsideSpeed; break;
			case ID_LOWER_DINO_MOVE_RIGHT :				fMoveVelocity	= runsideSpeed; break;


			case ID_LOWER_DINO_WALK_FRONT:				fMoveVelocity	= walkSpeed;	break;
			case ID_LOWER_DINO_WALK_FRONT_LEFT: 			fMoveVelocity	= walkSpeed;	break;
			case ID_LOWER_DINO_WALK_FRONT_RIGHT:			fMoveVelocity	= walkSpeed;	break;
			case ID_LOWER_DINO_WALK_BACK:				fMoveVelocity	= walkSpeed;	break;
			case ID_LOWER_DINO_WALK_BACK_LEFT:			fMoveVelocity	= walkSpeed;	break;
			case ID_LOWER_DINO_WALK_BACK_RIGHT:			fMoveVelocity	= walkSpeed;	break;

			case ID_LOWER_DINO_WALK_LEFT:				fMoveVelocity	= walkSideSpeed;	break;
			case ID_LOWER_DINO_WALK_RIGHT:				fMoveVelocity	= walkSideSpeed;	break;
			}

			//I3TRACE("ID: %d , DEF:%f, _Speed: %f, ScaleTime:%f\n", nId, fMoveVelocity, _Speed, _Speed/fMoveVelocity);

			_Speed = _Speed / fMoveVelocity;
		}
		else
		{
			_Speed = 1.0f;
		}

		return _Speed;
	}
	else
	{
		if( (rSpeed > I3_EPS) && (g_pViewer->getDeltaTime() > I3_EPS) )
		{
			rSpeed /= g_pViewer->getDeltaTime();
			//렙터스팅
			if(m_pOwner->IsRaptorBase()) {
				switch( nId )
				{
				case ID_LOWER_DINO_WALK_FRONT :
				case ID_LOWER_DINO_WALK_FRONT_LEFT :
				case ID_LOWER_DINO_WALK_FRONT_RIGHT :
					fScaleTime = rSpeed * 0.125f;
					break;
				case ID_LOWER_DINO_WALK_LEFT :
				case ID_LOWER_DINO_WALK_RIGHT :
					fScaleTime = rSpeed * 0.16667f;
					break;
				case ID_LOWER_DINO_WALK_BACK :
				case ID_LOWER_DINO_WALK_BACK_LEFT:
				case ID_LOWER_DINO_WALK_BACK_RIGHT :
					fScaleTime = rSpeed * 0.125f;
					break;
				}
			}

			if( fScaleTime < 0.05f ) fScaleTime = 0.05f;
		}

		return fScaleTime;
	}
}

REAL32 CGameCharaActionControl::_CalcRunLowerScaleTime( ID_LOWER_AI nId, REAL32 rSpeed)
{
	REAL32 fScaleTime = 1.f;

	I3ASSERT( m_pOwner->IsDino() == false);

	if( (rSpeed > I3_EPS) && (g_pViewer->getDeltaTime() > I3_EPS) )
	{
		rSpeed /= g_pViewer->getDeltaTime();
		
		if( isBody( CHARA_BODYLOWER_STAND ) )
		{
			switch( nId )
			{
			case ID_LOWER_MOVE_FRONT :
			case ID_LOWER_MOVE_RIGHT :
			case ID_LOWER_MOVE_FRONT_LEFT :			fScaleTime = (rSpeed * 0.2222f);		break;
			case ID_LOWER_MOVE_LEFT :
			case ID_LOWER_MOVE_BACK :				fScaleTime = (rSpeed * 0.3030f);		break;
			case ID_LOWER_MOVE_FRONT_RIGHT :		fScaleTime = (rSpeed * 0.2564f);		break;
			case ID_LOWER_MOVE_BACK_LEFT :
			case ID_LOWER_MOVE_BACK_RIGHT :			fScaleTime = (rSpeed * 0.3333f);		break;
			}
		}
		else
		{
			switch( nId )
			{
			case ID_LOWER_MOVE_FRONT :
			case ID_LOWER_MOVE_BACK :
			case ID_LOWER_MOVE_BACK_RIGHT :			fScaleTime = (rSpeed * 0.8888f);		break;
			case ID_LOWER_MOVE_FRONT_LEFT :			fScaleTime = rSpeed;					break;
			case ID_LOWER_MOVE_LEFT :
			case ID_LOWER_MOVE_RIGHT :
			case ID_LOWER_MOVE_BACK_LEFT :			fScaleTime = (rSpeed * 0.8f);			break;
			case ID_LOWER_MOVE_FRONT_RIGHT :		fScaleTime = (rSpeed * 0.7272f);		break;
			}
		}

		if( fScaleTime < 0.05f ) fScaleTime = 0.05f;
	}

	return fScaleTime;
}

REAL32 CGameCharaActionControl::GetScaleTimeByWalkType(REAL32 fVel, VEC3D* pMoveDir)
{
	REAL32 fScaleTime = 1.f;
	CGameCharaActionContext * pActionCtx = getActionContext();

	if( fVel > I3_EPS )
	{
		if( g_pViewer->getDeltaTime() > I3_EPS )
		{
			fVel /= g_pViewer->getDeltaTime();
			//렙터스팅
			if(m_pOwner->IsRaptorBase() )
			{
				I3ASSERT( i3::same_of<CGameDinoAnimContext*>(getAnimContext()) );
				ID_LOWER_DINO_AI nId = ((CGameDinoAnimContext*)getAnimContext())->getWalkIdByDirForDino( pMoveDir);
				switch( nId )
				{
				case ID_LOWER_DINO_WALK_FRONT :
				case ID_LOWER_DINO_WALK_FRONT_LEFT :
				case ID_LOWER_DINO_WALK_FRONT_RIGHT :
					fScaleTime = fVel * 0.2222f;
					break;
				case ID_LOWER_DINO_WALK_LEFT :
				case ID_LOWER_DINO_WALK_RIGHT :
					fScaleTime = fVel * 0.33333f;
					break;
				case ID_LOWER_DINO_WALK_BACK :
				case ID_LOWER_DINO_WALK_BACK_LEFT:
				case ID_LOWER_DINO_WALK_BACK_RIGHT :
					fScaleTime = fVel * 0.33333f;
					break;
				}
				//GlobalFunc::PB_TRACE("nId %d fScaleTime %f fVel %f", nId, fScaleTime, fVel);
			}
			else {
				ID_LOWER_AI nId = getAnimContext()->getWalkIdByDir( pMoveDir);

				if( pActionCtx->isBody( CHARA_BODYLOWER_STAND ) )
				{
					switch( nId )
					{
					case ID_LOWER_WALK_FRONT :
					case ID_LOWER_WALK_LEFT :
					case ID_LOWER_WALK_BACK_LEFT:
					case ID_LOWER_WALK_BACK_RIGHT :
					case ID_LOWER_WALK_FRONT_LEFT :		fScaleTime = fVel * 0.5925f;		break;
					case ID_LOWER_WALK_RIGHT :			fScaleTime = fVel * 0.4444f;		break;
					case ID_LOWER_WALK_BACK :			fScaleTime = fVel * 0.6666f;		break;
					case ID_LOWER_WALK_FRONT_RIGHT :	fScaleTime = fVel * 0.5333f;		break;
					}
				}
				else
				{
					switch( nId )
					{
					case ID_LOWER_WALK_FRONT :
					case ID_LOWER_WALK_BACK :
					case ID_LOWER_WALK_LEFT :
					case ID_LOWER_WALK_BACK_LEFT:
					case ID_LOWER_WALK_BACK_RIGHT :		fScaleTime = fVel * 3.5555f;		break;
					case ID_LOWER_WALK_FRONT_LEFT :		fScaleTime = fVel * 4.f;			break;
					case ID_LOWER_WALK_RIGHT :			fScaleTime = fVel * 3.2f;			break;
					case ID_LOWER_WALK_FRONT_RIGHT :	fScaleTime = fVel * 2.909f;			break;
					}
				}
				//GlobalFunc::PB_TRACE("nId %d fScaleTime %f fVel %f", nId, fScaleTime, fVel);
			}


		}

		if( fScaleTime < 0.01f ) fScaleTime = 0.01f;
	}

	return fScaleTime;
}

/*virtual*/ bool CGameCharaActionControl::OnActionDinoRoar( void)
{
	setLower( CHARA_LOWER_ROAR);


	_Animation_LowerForDino( ID_LOWER_DINO_STATUS_ROAR);

	getAnimContext()->setRunElapseTime(0.f);

	return true;
}

/*virtual*/ bool CGameCharaActionControl::OnActionDinoIdle( void)
{
	setLower( CHARA_LOWER_WAIT);

	_Animation_LowerForDino( ID_LOWER_DINO_STATUS_IDLE);

	getAnimContext()->setRunElapseTime(0.f);

	return true;
}

/*virtual*/ bool CGameCharaActionControl::OnActionDinoStun( void)
{
	setLower( CHARA_LOWER_STUN);

	_Animation_LowerForDino( ID_LOWER_DINO_STATUS_STUN);

	getAnimContext()->setRunElapseTime(0.f);

	return true;
}

// To Version
REAL32 CGameCharaActionControl::_CalcReloadTimeScaleToVersion(WeaponBase * pWeapon, REAL32 rAnimDuration)
{
	REAL32 rTimeScale = 1.f;

	REAL32 rReloadTime = pWeapon->GetReloadTime();

	if (g_pEnvSet->IsV2Version())
	{
		if ( !pWeapon->EnablePlayMasterAnim() )
		{
			rTimeScale = rAnimDuration / rReloadTime;
			I3ASSERT(rTimeScale > I3_EPS);
		}
		else
		{
			if (pWeapon->IsFastReloadAnimWeapon() == false)
			{
				rTimeScale = rAnimDuration / rReloadTime;
				I3ASSERT(rTimeScale > I3_EPS);
			}
		}

		// 마스터 애니메이션 아니더라도 LoopAnimation이면 돌립니다.
		if (pWeapon->isReloadLoopAnimation())
		{
			rTimeScale = 1.f;

			REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
			CASHITEM_TRACE("CASHITEMSKILL_REBULLET_RATE = %f\n", ratio);
			rTimeScale += (rTimeScale * ratio);
		}
	}
	else
	{
		if (pWeapon->EnablePlayMasterAnim())
		{
			if (pWeapon->IsFastReloadAnimWeapon() == false)
				rTimeScale = (rAnimDuration / rReloadTime) * MASTER_RELOAD_SPEED_RATIO;
		}
		else if (pWeapon->isReloadLoopAnimation())
		{
			rTimeScale = 1.f;
		}
		else
		{
			I3ASSERT(rReloadTime > I3_EPS);
			rTimeScale = rAnimDuration / rReloadTime;
		}

		//	빠른 장전 아이템 사용시 시간 감소.
		if (pWeapon->getWeaponInfo()->GetGaugeReload() != 1)
		{
			REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
			CASHITEM_TRACE("CASHITEMSKILL_REBULLET_RATE = %f\n", ratio);
			rTimeScale += (rTimeScale * ratio);
		}
	}

	return rTimeScale;
}

REAL32 CGameCharaActionControl::_CalcSwapTimeScaleToVersion(void)
{
	REAL32 rTimeScale = 1.0f;

	if (g_pEnvSet->IsV2Version())
	{
		rTimeScale = m_pOwner->GetWeaponSwapTimeScale();
	}
	else
	{
		// PBTN Beret 예외처리
		{
			const CEquipInfo * pBeretInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::GetePartToBeret());

			if (pBeretInfo != nullptr && pBeretInfo->GetITEMID() == 2700013)
			{
				REAL32 ratio = 0.5f + I3_EPS;
				rTimeScale += (rTimeScale * ratio);
			}
		}

		//	빠른 무기 변경 아이템 사용시 시간 감소.
		REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_SWAPWEAPON_RATE);
		CASHITEM_TRACE("CASHITEMSKILL_SWAPWEAPON_RATE = %f\n", ratio);
		rTimeScale += (rTimeScale * ratio);
	}

	return rTimeScale;
}
