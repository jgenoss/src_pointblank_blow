#include "pch.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaWeaponContext.h"
#include "Character/GameDinoAnimContext.h"
#include "GameCharaBase.h"
#include "WeaponDualHandGun.h"

#include "./Character/GameCharaActionControl1PV.h"
#include "./Character/GameCharaActionControl3PV.h"


// #define ENABLE_STATE_OUTPUT

I3_CLASS_INSTANCE( CGameCharaActionContext);

CGameCharaActionContext::~CGameCharaActionContext()
{	
	OnDestroy();
}

void CGameCharaActionContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	// Before call
	m_pCurrentActionControl = nullptr;
	m_p1PVActionControl = CGameCharaActionControl1PV::new_object();
	m_p1PVActionControl->BindOwner( pOwner);
	m_p3PVActionControl = CGameCharaActionControl3PV::new_object();
	m_p3PVActionControl->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaActionContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	m_pCurrentActionControl = nullptr;
	I3_SAFE_RELEASE( m_p1PVActionControl );
	I3_SAFE_RELEASE( m_p3PVActionControl );
}

void CGameCharaActionContext::OnReset()
{
	m_ActionBody			= (CHARA_ACTION_BODYLOWER) -1;
	m_ActionBody2			= (CHARA_ACTION_BODYUPPER) -1;
	m_ActionLower			= (CHARA_ACTION_LOWER) -1;
	m_ActionUpper			= (CHARA_ACTION_UPPER) -1;
	m_ActionUpperLeft		= (CHARA_ACTION_UPPER) -1;
	m_ActionCommon			= (CHARA_ACTION_COMMON) -1;
}

void CGameCharaActionContext::OnChange1PV()
{
	m_pCurrentActionControl = m_p1PVActionControl;
}

void CGameCharaActionContext::OnChange3PV()
{
	CGameCharaContextBase::OnChange3PV();

	m_pCurrentActionControl = m_p3PVActionControl;
}

void CGameCharaActionContext::OnChangeLocal()
{
	CGameCharaContextBase::OnChangeLocal();

	if( m_pOwner->is1PV() )		m_pCurrentActionControl = m_p1PVActionControl;
	else						m_pCurrentActionControl = m_p3PVActionControl;
}

void CGameCharaActionContext::OnChangeNetwork()
{
	CGameCharaContextBase::OnChangeNetwork();

	if( m_pOwner->is1PV() )		m_pCurrentActionControl = m_p1PVActionControl;
	else						m_pCurrentActionControl = m_p3PVActionControl;
}

void CGameCharaActionContext::OnChangeNPC()
{
	CGameCharaContextBase::OnChangeNPC();
}

void CGameCharaActionContext::OnChangeAI()
{
	CGameCharaContextBase::OnChangeAI();

	if( m_pOwner->is1PV() )		m_pCurrentActionControl = m_p1PVActionControl;
	else						m_pCurrentActionControl = m_p3PVActionControl;
}

void CGameCharaActionContext::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnUpdate( rDeltatime);
}

void CGameCharaActionContext::ApplyActionBody( CHARA_ACTION_BODYLOWER state)
{
	if( isBody( state))	return;

	setBodyLower( state);

	getAnimContext()->ChangeAnimSet( m_pOwner->getViewType(), true);
}

void CGameCharaActionContext::ApplyActionBody2( CHARA_ACTION_BODYUPPER state)
{
	if( isBody2( state)) return;

	CGameCharaAnimContext * pAnimCtx = getAnimContext();

	setBodyUpper( state);

	pAnimCtx->ChangeAnimationContext();

	if( isUpper( CHARA_UPPER_IDLE) || isUpper( CHARA_UPPER_MOVE) || 
		isUpper( CHARA_UPPER_JUMP) || isUpper( CHARA_UPPER_FIRE) ||
		isUpper( CHARA_UPPER_SECONDARY_FIRE) || isUpper(CHARA_UPPER_DAMAGE) )
	{
		//	애니메이션을 계속 합니다.
		REAL32 fSyncTime = 0.0f;
		
		if( pAnimCtx->isAILower())		fSyncTime = getAnimContext()->GetLowerSyncTime();		//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
		else							fSyncTime = getAnimContext()->GetUpperSyncTime();		//	상체에 타임 싱크하기 위해

		//	애니메이션 타임 싱크해야 합니다.
		if (m_pOwner->IsDino())
		{
			((CGameDinoAnimContext*)pAnimCtx)->_Play_AnimDino_Lower( ((CGameDinoAnimContext*)pAnimCtx)->getOldDinoLowerID(), fSyncTime);
		}
		else
		{
			pAnimCtx->_Play_Anim_Lower( pAnimCtx->getOldLowerID(), fSyncTime);
		}

		// KEYWORD : WEAPON_DUALHANDGUN
		if( m_pOwner->IsDino()) return;

		WeaponBase * pWeapon = getUsedWeapon();
		if( pWeapon == nullptr) return;

		if( pWeapon->isDualGun() )
		{
			pAnimCtx->_Play_Anim_DualUpper( pAnimCtx->getOldUpperDualLeftID(), fSyncTime);
			pAnimCtx->_Play_Anim_DualUpper( pAnimCtx->getOldUpperDualRightID(), fSyncTime);
		}
		else
		{
			pAnimCtx->_Play_Anim_Upper( pAnimCtx->getOldUpperID(), fSyncTime);
		}
	}
}

void CGameCharaActionContext::_All_Stop()
{
	m_pCurrentActionControl->OnActionReset();
}

void CGameCharaActionContext::_Play_Anim_DualUpper( ID_UPPER_DUAL_AI aiID, REAL32 startTime /*= 0.f*/, REAL32 scale /*= 1.f*/)
{
	getAnimContext()->_Play_Anim_DualUpper( aiID, startTime, scale);
}

void CGameCharaActionContext::_Play_Anim_Lower( ID_LOWER_AI aiID, REAL32 startTime /*= 0.f*/, REAL32 scale /*= 1.f*/)
{
	getAnimContext()->_Play_Anim_Lower( aiID, startTime, scale);
}


void CGameCharaActionContext::_Play_Anim_Upper( ID_UPPER_AI aiID, REAL32 startTime /*= 0.f*/, REAL32 scale /*= 1.f*/)
{
	getAnimContext()->_Play_Anim_Upper( aiID, startTime, scale);
}

void CGameCharaActionContext::_Play_Body_Sync( void)
{
	getAnimContext()->_Play_Body_Sync();
}

bool CGameCharaActionContext::_Play_Ceremony( void)
{
	setCommon( CHARA_ACTION_COMMON_CEREMONY);
	return getAnimContext()->_Play_Anim_Common( ID_COMMON_CEREMONY);
}

void CGameCharaActionContext::_Play_Crouch( void)
{
	m_pCurrentActionControl->OnActionCrouch();
}

void CGameCharaActionContext::_Play_Damage( CHARA_HIT_PART part /*= CHARA_HIT_UNKNOWN*/, VEC3D * pDir /*= nullptr*/, bool bFalling /*= false*/)
{
	if( bFalling)
		m_pCurrentActionControl->OnActionDamage( CHARA_HIT_ROOT, i3Matrix::GetAt( m_pOwner->GetMatrix()));
	else
		m_pCurrentActionControl->OnActionDamage( part, pDir);
}

void CGameCharaActionContext::_Play_Death( INT32 iKillIdx, CHARA_DEATH_TYPE type, VEC3D * pDir)
{
	m_pCurrentActionControl->OnActionDeath( iKillIdx, type, pDir);
}

void CGameCharaActionContext::_Play_Dual_Idle_Left()
{
	m_pCurrentActionControl->ActionUpperLeftReturn();
}

void CGameCharaActionContext::_Play_Dual_Idle_Right()
{
	m_pCurrentActionControl->ActionUpperRightReturn();
}

void CGameCharaActionContext::_Play_Dual_JumpEnd_Left(void)
{
	m_pCurrentActionControl->OnActionJumpUpperLeftFinish();
}

void CGameCharaActionContext::_Play_Dual_Run_Left()
{
	m_pCurrentActionControl->ActionRunUpperLeft();
}

void CGameCharaActionContext::_Play_Dual_Run_Right()
{
	m_pCurrentActionControl->ActionRunUpperRight();
}

bool CGameCharaActionContext::_Play_Extension( bool bEnable)
{
	return m_pCurrentActionControl->OnActionExtensionWeapon( bEnable) == true;
}

bool CGameCharaActionContext::_Play_JumpStart( void)
{
	return m_pCurrentActionControl->OnActionJumpStart();
}

void CGameCharaActionContext::_Play_JumpEnd( void)
{
	m_pCurrentActionControl->OnActionJumpLowerFinish();
}

void CGameCharaActionContext::_Play_ForceJump(VEC3D *pDir, REAL32 rSpeed, REAL32 rJumpPower)
{
	m_pCurrentActionControl->OnActionForceJumpStart(pDir, rSpeed, rJumpPower);
}

void CGameCharaActionContext::_Play_Lower_Idle()
{
	if( !isLower( CHARA_LOWER_IDLE ))
		m_pCurrentActionControl->OnActionSteadyLower();
}

void CGameCharaActionContext::_Play_DinoLower_Idle()
{
	if( !isLower( CHARA_LOWER_IDLE ))
		m_pCurrentActionControl->OnActionSteadyDinoLower();
}

void CGameCharaActionContext::_Play_Lower_Return( void)
{
	m_pCurrentActionControl->ActionLowerReturn();
}

void CGameCharaActionContext::_Play_Lower_Run( VEC3D * pDir)
{
	m_pCurrentActionControl->OnActionRunLower( pDir);
}

void CGameCharaActionContext::_Play_Dino_Lower_Run( VEC3D * pDir)
{
	m_pCurrentActionControl->OnActionDinoRunLower( pDir);
}

void CGameCharaActionContext::_Play_Lower_Walk( VEC3D * pDir)
{
	m_pCurrentActionControl->OnActionWalkLower( pDir);
}

void CGameCharaActionContext::_Play_Profile_Idle_A()
{
	setCommon( CHARA_ACTION_COMMON_NONE);
	m_pCurrentActionControl->OnActionProfileIdleA();
}

void CGameCharaActionContext::_Play_Profile_Idle_B()
{
	setCommon( CHARA_ACTION_COMMON_NONE);
	m_pCurrentActionControl->OnActionProfileIdleB();
}

void CGameCharaActionContext::_Play_Profile_Idle_Change( bool bIsSameWeapon)
{
	setCommon( CHARA_ACTION_COMMON_NONE);
	m_pCurrentActionControl->OnActionProfileIdleChange(bIsSameWeapon);
}

void CGameCharaActionContext::_Play_Profile_Idle_Stand( void)
{
	setCommon( CHARA_ACTION_COMMON_NONE);
	m_pCurrentActionControl->OnActionProfileIdleStand();
}

void CGameCharaActionContext::_Play_StandUp()
{
	m_pCurrentActionControl->OnActionStandup();
}

void CGameCharaActionContext::_Play_Upper_ChangeWeapon()
{
	m_pCurrentActionControl->OnActionSwapWeapon();
}

bool CGameCharaActionContext::_Play_Upper_Fire( INT32 iSlot /*= -1*/, WEAPON::FIRE_ORDER order /*= WEAPON::PRIMARY_FIRE*/)
{
	if( order == WEAPON::READY_FIRE)
		return m_pCurrentActionControl->OnActionReadyToFire() == true;
	else
		return m_pCurrentActionControl->OnActionFire( iSlot, order) == true;
}

bool CGameCharaActionContext::_Play_Upper_Idle( void)
{
	return m_pCurrentActionControl->OnActionSteadyUpper() == true;
}

void CGameCharaActionContext::_Play_Upper_JumpEnd( void)
{
	m_pCurrentActionControl->OnActionJumpUpperFinish();
}

bool CGameCharaActionContext::_Play_Upper_Reload( ACTION_LOAD_MAG_TYPE type /*= ACTION_LOAD_MAG*/)
{	
	switch( type)
	{
	case ACTION_LOAD_MAG :			return m_pCurrentActionControl->OnActionLoadMagazine() == true;
	case ACTION_LOAD_MAG_QUICK :	return m_pCurrentActionControl->OnActionLoadMagazineDual() == true;
	case ACTION_LOAD_MAG_B :		return m_pCurrentActionControl->OnActionLoad_B_Magazine() == true;
	case ACTION_LOAD_MAG_BULLET :	return m_pCurrentActionControl->OnActionLoadBullet() == true;
	case ACTION_LOAD_REMOVE_SHELL :	return m_pCurrentActionControl->OnActionRemoveShell() == true;
	case ACTION_LOAD_READY_MAG :	return m_pCurrentActionControl->OnActionLoadMag_Ready() == true;
	case ACTION_LOAD_FIRE_BULLET :	return m_pCurrentActionControl->OnActionFireBullet() == true;
	default :	return false;
	}
}
bool CGameCharaActionContext::_Play_Upper_Return( void)
{
	return m_pCurrentActionControl->ActionUpperReturn() == true;
}

bool CGameCharaActionContext::_Play_Upper_Run( void)
{
	return m_pCurrentActionControl->OnActionRunUpper() == true;
}

bool CGameCharaActionContext::_Play_Upper_Walk( void)
{
	return (m_pCurrentActionControl->OnActionWalkDinoUpper()?true:false);
}

bool CGameCharaActionContext::_Play_Zoom( void)
{
	return m_pCurrentActionControl->OnActionActiveZoom() == true;
}

bool CGameCharaActionContext::_Play_ZoomCancel( void)
{
	return m_pCurrentActionControl->OnActionCancelZoom() == true;
}

void CGameCharaActionContext::_Toggle_BodyState( void)
{
	I3ASSERT( m_pCurrentActionControl != nullptr);
	m_pCurrentActionControl->ActionToggleIdle();
}

void CGameCharaActionContext::_Stop_Anim( void)
{
	getAnimContext()->_Stop_Anim();
}





