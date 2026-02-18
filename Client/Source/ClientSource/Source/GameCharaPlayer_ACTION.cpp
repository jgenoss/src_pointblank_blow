#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaBase.h"
#include "StageBattle.h"
#include "Weapon.h"

//#define NO_SCALE_ANIMATION

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ACTION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//현재 Character가 특정 컬리전 그룹과 충돌중인지 아닌지의 여부를 테스트 하는 함수입니다.
/*
bool CGameCharaPlayer::_IsOverlappedBy( UINT32 nGroupIdx)
{
	if( isNetwork() )
		return TRUE;

	VEC3D vec;
	i3Vector::Copy( &vec, GetPos() );
	i3Vector::Add( &vec, 0.f, RADIUS+0.05f, 0.f );
	NxVec3 v1( getX(&vec), getY(&vec), getZ(&vec) );
	i3Vector::Add( &vec, 0.f, TALL-0.05f, 0.f );
	NxVec3 v2( getX(&vec), getY(&vec), getZ(&vec) );
	NxSegment	seg( v1, v2 );
	NxCapsule Capsule( seg, RADIUS );
	
	NxScene * pScene = g_pPhysixCtx->getScene();
 	NxU32	activeGrp = g_pPhysixCtx->getGroupMask( nGroupIdx );

	activeGrp &= ~( 1 << CGRP_TRIGGER );
	//activeGrp = 0;
	
	return pScene->checkOverlapCapsule( Capsule, NX_ALL_SHAPES, activeGrp, NULL);
}

BOOL CGameCharaPlayer::ACTION_Stand( void)
{
	if( getActionContext()->isBody( CHARA_BODY_CROUCH) )
	{
		if( !isNetwork() )
		{	
			if( _IsOverlappedBy( CGRP_MYCHARA_BOUND) )
			{	
				return FALSE;
			}

			//m_pMoveContext->ChangeControllerHeight( TALL );
		}

		_ANIM_playBodyLowerSync( CHARA_BODY_STAND, !is1PV());
		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_Crouch( void)
{
	if( getActionContext()->isBody( CHARA_BODY_STAND))
	{
//		if( !isNetwork() )
//			m_pMoveContext->ChangeControllerHeight( HALFTALL );

		_ANIM_playBodyLowerSync( CHARA_BODY_CROUCH, !is1PV());
		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_ToggleIdle( void)
{	
	if( getActionContext()->isBody2( CHARA_BODY_ATTACKIDLE))
	{
		_ANIM_playBody2( CHARA_BODY_IDLE, !is1PV());
	}
	else if( getActionContext()->isBody2( CHARA_BODY_IDLE))
	{
		_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV());
	}

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_Respawn( VEC3D * pStartPos)
{
	I3ASSERT( pStartPos != NULL);

	ResetChara();	//	캐릭터 초기화
	
	//	처음 리스폰시 0번 무기로 교체하는 동작한다.
	{
		getWeaponContext()->getControl()->OnSelectWeapon( 2);	// 무기제한에 포함되지 않는 근접무기로 초기 설정
		//ACTION_apply( !is1PV());
		getAnimContext()->getControl()->AnimSetApply();

		SyncWeapon();
		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->ReturnToReady();	//	무기 교체시 이전 무기는 Ready 상태로 변경
		}

		//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.
		if( isNetwork() )
		{
			getWeaponContext()->getControl()->OnAttachPurchasePrimWeapon( 0);
			getWeaponContext()->getControl()->OnAttachPurchaseSideWeapon( 1);
		}

		CommandParameter * pParam = getCommandContext()->getParameter();
		pParam->_nSlotIdx = 0;
		
		//if (FALSE == ACTION_SwapWeapon( 0))
		if( !getCommandContext()->Command( CHARA_CMD_SWAPWEAPON))
		{
			// 주무기 제한시 보조무기로 변경
			pParam->_nSlotIdx = 1;
			getCommandContext()->Command( CHARA_CMD_SWAPWEAPON);
			//ACTION_SwapWeapon(1);
		}

		SyncWeapon();

		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->WeaponChange();		//	무기 교체시 이후 무기는 Change 상태로
		}

		//	교체가 끝나고 애니메이션을 한다.
		ANIM_playUpper( CHARA_UPPER_SWAPWEAPON, ID_UPPER_SWAP_WEAPON);

		addEnableStateMask( CHARA_STATE_DISABLE_SHOT );//| CHARA_STATE_DISABLE_CROUCH);
		removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL | CHARA_STATE_DISABLE_MOVE );

		//removeStateMask( CHARA_STATE_DEFAULT);
	}

	if( !isNetwork() && (g_pCameraManager != NULL) && g_pCamera != NULL )
	{	//	Default 카메라로
		g_pCameraManager->setTargetCharacter(this);
		g_pCameraManager->ChangeCamera( "FPS", 0.0f);
		
		//I3PRINTF("FPS idx = %d, Nick = %s\n", getCharaInfo()->_NetIdx, g_pGameContext->GetNickForSlot(getCharaInfo()->_NetIdx));

		//_CAMERA_SetIdle();		//	일반 카메라 상태
		m_pCameraContext->SetIdleCamera();

		// 1인칭 모드에 대한 뷰 처리
		ChangeFPSView( FALSE);
	}

	// 앉아서 죽은 경우 앉은 상태의 컨트롤러가 세팅되어 있습니다.
	// 리스폰할시에 강제로 서 있는 상태로 세팅해줍니다.
	m_pMoveContext->ChangeControllerHeight( TALL );

	// 이팩트용 컨택트 박스를 Kinematic으로 변경
	getCollisionContext()->SetContactShapeSetEnable( FALSE);
	
	forceSetPos( pStartPos);		//	리스폰 위치 설정 (월드의 로컬 좌표)

	{
		//m_rElapsedTakeOffTime	= 0.0f;
		//m_rCurJumpHeight		= m_pDBInfo->m_rJumpHeight;
		//m_rCurJumpTime			= m_pDBInfo->m_rJumpTime * 0.5f;

		//i3Vector::Copy( &m_TakeOffPos, pStartPos);
	}
#if defined( I3_DEBUG )
	MATRIX mat;
	VEC3D	vDir;
	i3Matrix::Identity( &mat);
	i3Matrix::SetRotateY( &mat, m_pBoneContext->getTheta() );

	i3Vector::Copy( &m_vDebugOriginalPos, GetPos() );
	i3Vector::TransformCoord( &vDir, m_pMoveContext->getMoveDir(), &mat );
	i3Vector::Add( &m_vDebugTargetPos, GetPos(), &vDir );
	m_rDebugElp = 1.f;
#endif

	ApplyCrosshair();

//	m_DropCount = 0;
	
	//I3TRACE("StartPos: %f, %f, %f\n", getX( pStartPos), getY( pStartPos), getZ( pStartPos));

	//I3TRACE( "Chara Respawn : %d ( %.4f, %.4f, %.4f)\n", getCharaNetIndex(), getX( pStartPos), getY( pStartPos), getZ( pStartPos));

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_Invincible( BOOL bVal )
{
	if( bVal )
	{
		m_rInvincibleElapseTime = g_pCharaInfoDataBase->getGlobal()->m_rInvincibleTime;
		getMaterialContext()->SetBlendEnable( TRUE);
	}
	else
	{
		m_rInvincibleElapseTime = 0.f;
		getMaterialContext()->SetBlendEnable( FALSE );
	}

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_NoneBattleIdle( void)
{
	REAL32 fSyncTime = 0.f;
	if( m_pAnimContext->isAIUpper() && getActionContext()->isUpper( CHARA_UPPER_NONEBATTLEIDLE))
	{	
		fSyncTime = getAnimContext()->getUpperAniTimer();

		//if( getWeaponContext()->getCurrent() != NULL)
		{
		//	CWeaponInfo * pWeaponInfo = getWeaponContext()->getCurrent()->getWeaponInfo();
		//	INT32 nAnimSet = (INT32) ANIM_getAnimSet();
		//	INT32 nWeaponType = (INT32) pWeaponInfo->m_TypeClass;
		//	INT32 nWeaponNum = (INT32) pWeaponInfo->m_nNumber;
		//	INT32 nBody2 = (INT32) STATE_getBody2();
			
		//	i3AIState * pNewAIState = m_pAnimContext->getAIUpper()->getAIState( m_nAIUpperId[ nAnimSet ][ nWeaponType ][ nWeaponNum][ nBody2][ ID_UPPER_NONEBATTLE_IDLE] );
		//	if( pNewAIState != NULL)
			{
		//		if( fSyncTime > pNewAIState->getAnimDuration() )
		//			fSyncTime -= pNewAIState->getAnimDuration();
			}
		}
	}

	ANIM_playUpper( CHARA_UPPER_NONEBATTLEIDLE, ID_UPPER_NONEBATTLE_IDLE, fSyncTime);

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_NoneBattleWeaponAction( void)
{
	REAL32 fSyncTime = 0.f;
	if( m_pAnimContext->isAIUpper() && getActionContext()->isUpper( CHARA_UPPER_NONEBATTLEWEAPONSHOW))
	{
		//if( getWeaponContext()->getCurrent() != NULL)
		{
		//	CWeaponInfo * pWeaponInfo = getWeaponContext()->getCurrent()->getWeaponInfo();
		//	INT32 nWeaponNum = (INT32) pWeaponInfo->m_nNumber;
		//	INT32 nID = m_pAnimContext->getAIUpperID( getAnimSet(), pWeaponInfo->m_TypeClass, nWeaponNum, getActionContext()->getBody2(), ID_UPPER_NONEBATTLE_IDLE);
		//	i3AIState * pNewAIState = m_pAnimContext->getAIUpper()->getAIState(	nID );
		//	if( pNewAIState != NULL)
			{
		//		if( fSyncTime > pNewAIState->getAnimDuration() )
		//			fSyncTime -= pNewAIState->getAnimDuration();
			}
		}
		

		fSyncTime = getAnimContext()->getUpperAniTimer();
	}
	else
		getAnimContext()->setUpperAniTimer( 0.f);

	ANIM_playUpper( CHARA_UPPER_NONEBATTLEWEAPONSHOW, ID_UPPER_NONEBATTLE_SHOWWEAPON, fSyncTime );
	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_MoveLower( VEC3D * pMoveDir)
{	
	if( pMoveDir == NULL )
		pMoveDir	= m_pMoveContext->getMoveDir();

	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_AI nId = _ANIM_getMoveId( pMoveDir);
	CHARA_ACTION_LOWER state = CHARA_LOWER_MOVE;
	
	if( i3Vector::IsSame( pMoveDir, &I3_ZEROVECTOR) && !getActionContext()->isLower( CHARA_LOWER_IDLE) )
		return ACTION_IdleLower();

	if( nId != ID_LOWER_IDLE )
	{
#if !defined( NO_SCALE_ANIMATION )
		REAL32 fVel = i3Vector::Length( m_pMoveContext->getVelocity() );

		if( fVel > I3_EPS )
		{	
			if( g_pViewer->getDeltaTime() > I3_EPS )
			{
				fVel /= g_pViewer->getDeltaTime();
				
				if( getActionContext()->isBody( CHARA_BODY_STAND ) )
				{
					switch( nId )
					{
					case ID_LOWER_MOVE_FRONT :
					case ID_LOWER_MOVE_RIGHT :
					case ID_LOWER_MOVE_FRONT_LEFT :
						fScaleTime = (fVel * 0.2222222222222222222222222222222f);
						break;

					case ID_LOWER_MOVE_LEFT :
					case ID_LOWER_MOVE_BACK :
						fScaleTime = (fVel * 0.3030303030303030303030303030303f);
						break;
						
					case ID_LOWER_MOVE_FRONT_RIGHT :
						fScaleTime = (fVel * 0.25641025641025641025641025641026f);
						break;

					case ID_LOWER_MOVE_BACK_LEFT :
					case ID_LOWER_MOVE_BACK_RIGHT :
						fScaleTime = (fVel * 0.33333333333333333333333333333333f);
						break;
					}
				}
				else
				{
					switch( nId )
					{
					case ID_LOWER_MOVE_FRONT :
					case ID_LOWER_MOVE_BACK :
					case ID_LOWER_MOVE_BACK_RIGHT :
						fScaleTime = (fVel * 0.88888888888888888888888888888889f);
						break;
					
					case ID_LOWER_MOVE_FRONT_LEFT :
						fScaleTime = fVel;
						break;

					case ID_LOWER_MOVE_LEFT :
					case ID_LOWER_MOVE_RIGHT :
					case ID_LOWER_MOVE_BACK_LEFT :
						fScaleTime = (fVel * 0.8f);
						break;
						
					case ID_LOWER_MOVE_FRONT_RIGHT :
						fScaleTime = (fVel * 0.72727272727272727272727272727273f);
						break;
					}
				}
			}

			if( fScaleTime < 0.05f )
			{
				fScaleTime = 0.05f;
			}
		}
#endif

		if( m_pAnimContext->isAILower())
		{		
			//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
			if( getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK ))
			{
				fSyncTime = m_pAnimContext->getAILower()->getLocalTime();
				if( m_pAnimContext->getOldLowerID() != nId &&
					((m_pAnimContext->getOldLowerID() > ID_LOWER_IDLE) &&
					(m_pAnimContext->getOldLowerID() <= ID_LOWER_WALK_BACK_RIGHT)) )
				{	
					INT32 nID = m_pAnimContext->getAILowerID( getActionContext()->getBody(), (ID_LOWER_AI)m_pAnimContext->getOldLowerID() );
					i3AIState * pOldAIState = m_pAnimContext->getAILower()->getAIState( nID );
					nID = m_pAnimContext->getAILowerID( getActionContext()->getBody(), nId );
					i3AIState * pNewAIState = m_pAnimContext->getAILower()->getAIState( nID );

					if( pOldAIState->getAnimDuration() > I3_EPS )
						fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
				}

				//I3TRACE( "SyncTime:%.4f,ScaleTime:%.4f\n", fSyncTime, fScaleTime );
			}
		}
	}

	if( (m_pAnimContext->getOldLowerID() != nId) )
	{
		ANIM_playLower( state, nId, fSyncTime, fScaleTime );
	}
	else if( m_pAnimContext->getAILower() != NULL)
	{
		REAL32 rRet = m_pMoveContext->getSpeed() - fScaleTime;
		if( rRet < -0.05f || rRet > 0.05f )
		{
			SetLowerTimeScale( fScaleTime );
			m_pMoveContext->setSpeed( fScaleTime);
		}
	}

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_MoveUpper( VEC3D * pMoveDir)
{	
	if( pMoveDir == NULL )
		pMoveDir	= m_pMoveContext->getMoveDir();

	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_UPPER_AI	nId = ID_UPPER_MOVE;	//	상체는 방향 관계없이 "Move"만
	CHARA_ACTION_UPPER state = CHARA_UPPER_MOVE;

	ACTION_SyncWeapon();
	
	if( i3Vector::IsSame( pMoveDir, &I3_ZEROVECTOR))
	{
		nId = ID_UPPER_IDLE;
		state = CHARA_UPPER_IDLE;
	}

	if( nId != ID_UPPER_IDLE )
	{
		if( m_pAnimContext->isAIUpper())
		{		
			//	현재 이동 중이다. ( 이동 애니메이션 동작시간과 맞춰 플레이)
			fSyncTime = m_pAnimContext->getAIUpper()->getLocalTime();
		}

		switch( getActionContext()->getLower() )
		{
		case CHARA_LOWER_MOVE :
			fScaleTime = 1.f;
			break;

		case CHARA_LOWER_JUMP :
		case CHARA_LOWER_DEATH :
			break;

		case CHARA_LOWER_WALK :
			fScaleTime = m_pDBInfo->m_rWalkFrontMove / m_pDBInfo->m_rFrontMoveSpeed;
			break;
		}

		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		if( pWeapon != NULL )
		{	
			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();*/

			/*
			fScaleTime *= pWeaponInfo->m_rMoveNormalStand;
			

			if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_PRIMARY )
			{
				
			}
			else if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_SECONDARY )
			{
				fScaleTime *= 0.8f;
			}
			else if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_MELEE )
			{
			}
			else if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_THROWING || pWeaponInfo->m_TypeUsage == WEAPON_USAGE_ITEM )
			{
				fScaleTime *= 0.95f;
			}
			else if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_MISSION )
			{
				fScaleTime *= 0.75f;
			}*/

			//CWeaponInfo * pDefaultInfo = g_pWeaponInfoDataBase->getWeaponInfo( pWeaponInfo->m_TypeClass, 1 );

			/*REAL32	rScaleRate = 0.0f;
			if( getActionContext()->isBody( CHARA_BODY_STAND))			//	기본 서있는 상태에서
				//fScaleTime *= (1.f - ((pDefaultInfo->m_rMoveNormalStand - pWeaponInfo->m_rMoveNormalStand) * 0.01f) );
				rScaleRate =  pWeaponInfo->m_rMoveNormalStand * 0.01f;
			else if( getActionContext()->isBody( CHARA_BODY_CROUCH))		//	기본 앉아있는 상태에서
				//fScaleTime *= (0.6f - ((pDefaultInfo->m_rMoveNormalSit - pWeaponInfo->m_rMoveNormalSit)  * 0.01f));
				rScaleRate = pWeaponInfo->m_rMoveNormalSit * 0.01f;

			if( isStateMask( CHARA_STATE_ZOOM))
			{
				if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) &&
					!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))	//	줌인 중이지만 장전시에는 적용안되게...
					rScaleRate *= pWeaponInfo->m_rMoveNormalZoom * 0.01f;
			}

			if( getActionContext()->isUpper( CHARA_UPPER_FIRE) ||
				getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE))			//	발사 상태일때
			{
				rScaleRate *= pWeaponInfo->m_rMoveNormalFire * 0.01f;
			}

			fScaleTime *= rScaleRate * 1.25f;
		}
	}

	if( (m_pAnimContext->getOldUpperID() != nId) )
	{
		ANIM_playUpper( state, nId, fSyncTime, fScaleTime );
	}
	else if( m_pAnimContext->getAIUpper() != NULL)
	{
		i3AIState * pAI = m_pAnimContext->getAIUpper()->getCurrentAIState();
		if( pAI != NULL)
		{
			pAI->setTimeScale( fScaleTime);
		}

		SetUpperTimeScale( fScaleTime );
	}

//	ANIM_playUpper( state, nId, fSyncTime, fScaleTime );

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_WalkLower( VEC3D * pMoveDir)
{	
	if( pMoveDir == NULL )
		pMoveDir	= m_pMoveContext->getMoveDir();

	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_AI nId = _ANIM_getWalkId( pMoveDir);
	CHARA_ACTION_LOWER state = CHARA_LOWER_WALK;	//	걷기 상태
	
	if( i3Vector::IsSame( pMoveDir, &I3_ZEROVECTOR) && !getActionContext()->isLower( CHARA_LOWER_IDLE) )
		CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);
		//return ACTION_IdleLower();
	
	if( nId != ID_LOWER_IDLE )
	{
#if !defined( NO_SCALE_ANIMATION )
		REAL32 fVel = i3Vector::Length( m_pMoveContext->getVelocity() );

		if( fVel > 0.f )
		{
			if( g_pViewer->getDeltaTime() > I3_EPS )
			{
				fVel /= g_pViewer->getDeltaTime();
				
				if( getActionContext()->isBody( CHARA_BODY_STAND ) )
				{
					switch( nId )
					{
					case ID_LOWER_WALK_FRONT :
					case ID_LOWER_WALK_LEFT :
					case ID_LOWER_WALK_BACK_LEFT:
					case ID_LOWER_WALK_BACK_RIGHT :
					case ID_LOWER_WALK_FRONT_LEFT :
						fScaleTime = fVel * 0.59259259259259259259259259259259f;
						break;
					case ID_LOWER_WALK_RIGHT :
						fScaleTime = fVel * 0.44444444444444444444444444444444f;
						break;
					case ID_LOWER_WALK_BACK :
						fScaleTime = fVel * 0.66666666666666666666666666666667f;
						break;
					case ID_LOWER_WALK_FRONT_RIGHT :
						fScaleTime = fVel * 0.53333333333333333333333333333333f;
						break;
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
					case ID_LOWER_WALK_BACK_RIGHT :
						fScaleTime = fVel * 3.5555555555555555555555555555556f;
						break;
					case ID_LOWER_WALK_FRONT_LEFT :
						fScaleTime = fVel * 4.f;
						break;
					case ID_LOWER_WALK_RIGHT :
						fScaleTime = fVel * 3.2f;
						break;
					case ID_LOWER_WALK_FRONT_RIGHT :
						fScaleTime = fVel * 2.9090909090909090909090909090909f;
						break;
					}
				}
			}

			if( fScaleTime < 0.01f )
			{
				fScaleTime = 0.01f;
			}
		}
#endif

		if( m_pAnimContext->isAILower())
		{		
			//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
			if( getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK ))
			{
				fSyncTime = m_pAnimContext->getAILower()->getLocalTime();
				
				if( m_pAnimContext->getOldLowerID() != nId && (m_pAnimContext->getOldLowerID() > ID_LOWER_IDLE && m_pAnimContext->getOldLowerID() <= ID_LOWER_WALK_BACK_RIGHT) )
				{	
					INT32 nID = m_pAnimContext->getAILowerID( getActionContext()->getBody(), nId );
					i3AIState * pNewAIState = m_pAnimContext->getAILower()->getAIState( nID );
					nID = m_pAnimContext->getAILowerID( getActionContext()->getBody(), (ID_LOWER_AI) m_pAnimContext->getOldLowerID());
					i3AIState * pOldAIState = m_pAnimContext->getAILower()->getAIState( nID );
					if( pOldAIState->getAnimDuration() > I3_EPS )
						fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
				}

				//I3TRACE( "SyncTime:%.4f,ScaleTime:%.4f\n", fSyncTime, fScaleTime );
			}
		}
	}

	if( m_pAnimContext->getOldLowerID() != nId )
		ANIM_playLower( state, nId, fSyncTime, fScaleTime );
	else
	{
		REAL32 rRet = m_pMoveContext->getSpeed() - fScaleTime;
		if( rRet < -0.01f || rRet > 0.01f )
		{
			SetLowerTimeScale( fScaleTime );
			m_pMoveContext->setSpeed( fScaleTime);
		}
	}

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_IdleLower( void)
{
	ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_IDLE);
	
	m_fRunElapseTime = 0.0f;
	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_IdleUpper( void)
{
	REAL32 fSyncTime = 0.f;

	if( getActionContext()->isUpper( CHARA_UPPER_IDLE ))
		return FALSE;

	if( m_pAnimContext->isAILower())
	{
		//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
		fSyncTime = GetSyncTimeLower();
	}

	ACTION_SyncWeapon();

	ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_IDLE, fSyncTime);

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_Jump( void)
{
	//setClimbingLadderEnable( false);
	if( !isNetwork() )
	{
		// 캐릭터의 바운드 체크
		//	연속 점프를 막기 위해 딜레이 시간을 준다.
		if( m_fJumpDelayElapsedTime <= m_pDBInfo->m_rJumpDelay || _IsOverlappedBy( CGRP_MYCHARA_BOUND) )
			return FALSE;
	}

	addStateMask( CHARA_STATE_JUMP);

	m_pMoveContext->setElapsedTakeOffTime( 0.f);

	I3ASSERT( m_pDBInfo->m_rJumpPenaltyTime > I3_EPS );
	REAL32	rRate = m_fJumpDelayElapsedTime / m_pDBInfo->m_rJumpPenaltyTime;
	REAL32  rValidRate = 1.0f;

	if( rRate < 0.3f)
	{
		rValidRate = MINMAX( 0.1f, 1.0f - ( m_pDBInfo->m_rJumpPenaltyRate * 3.0f), 1.0f);
	}
	else if( (rRate >= 0.3f) && ( rRate < 0.6f))
	{
		rValidRate = MINMAX( 0.1f, 1.0f - ( m_pDBInfo->m_rJumpPenaltyRate * 2.0f), 1.0f);
	}
	else if(( rRate > 1.0f) && ( rRate >= 0.6f))
	{
		rValidRate = MINMAX( 0.1f, 1.0f - ( m_pDBInfo->m_rJumpPenaltyRate * 1.0f), 1.0f);
	}

	m_pMoveContext->setCurJumpHeight( m_pDBInfo->m_rJumpHeight * rValidRate);
	m_pMoveContext->setCurJumpTime( m_pDBInfo->m_rJumpTime * rValidRate * 0.5f);

	if( getActionContext()->isBody( CHARA_BODY_CROUCH))	//	앉아 있으면
	{
		_ANIM_playBody( CHARA_BODY_STAND, !is1PV());
	}

	ANIM_playLower( CHARA_LOWER_JUMP, ID_LOWER_JUMP_START);

	if( getActionContext()->isUpper( CHARA_UPPER_IDLE) ||
		getActionContext()->isUpper( CHARA_UPPER_MOVE) )// || (STATE_getUpper() == CHARA_UPPER_JUMP))
	{
		ACTION_SyncWeapon();
		ANIM_playUpper( CHARA_UPPER_JUMP, ID_UPPER_JUMP_START);
	}

	// 이동과 앉기를 막는다.
	addEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_START, GetPos(), is1PV());

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_JumpEndUpper( void )
{
	ACTION_SyncWeapon();
	ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_JUMP_END);

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_JumpEndLower( void )
{
	ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_JUMP_END);

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_Damage( CHARA_HIT_PART part, VEC3D * pvHitDir)
{
	if( isInvincible() )
		return FALSE;

	if( !isNetwork() )
	{
		BOOL bCheck = FALSE;
		CHARA_ACTION_UPPER state = getActionContext()->getUpper();

		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		if( (pWeapon!=NULL && pWeapon->getWeaponInfo()->m_TypeClass==WEAPON_CLASS_MISSION) &&
			state == CHARA_UPPER_FIRE)
			bCheck = TRUE;

		if( !bCheck && (state != CHARA_UPPER_IDLE) && (state != CHARA_UPPER_MOVE) && (state != CHARA_UPPER_JUMP ) )
		{
			return FALSE;	// 1인칭이고 캐릭터가 발사 상태이면 스킵
		}
	}

	ACTION_SyncWeapon();

	INT32 nType = i3Math::Rand() % 2;

	VEC3D * pAt = i3Matrix::GetAt( GetMatrix());

	//	앞방향 구분
	BOOL bFront = (i3Vector::Dot( pAt, pvHitDir) < 0.0f);

	ID_UPPER_AI id;
		
	switch( part)
	{	
	case CHARA_HIT_HEAD		:
	case CHARA_HIT_NECK		:
	case CHARA_HIT_BODY1	:
	case CHARA_HIT_BODY2	:
	case CHARA_HIT_BODY3	:		//	몸통을 맞았다.
		if( bFront)
		{	
			if( nType == 0)
			{
				if( i3Math::Rand() % 2 == 0)
					id = ID_UPPER_DAMAGE_FRONT_LEFT_A;
				else
					id = ID_UPPER_DAMAGE_FRONT_LEFT_B;
			}
			else
			{
				if( i3Math::Rand() % 2 == 0)
					id = ID_UPPER_DAMAGE_FRONT_RIGHT_A;
				else
					id = ID_UPPER_DAMAGE_FRONT_RIGHT_B;
			}
		}
		else
		{
			if( (i3Math::Rand() % 2) == 0)
				id = ID_UPPER_DAMAGE_BACK_A;
			else
				id = ID_UPPER_DAMAGE_BACK_B;
		}
		break;	

	//	왼쪽 데미지
	case CHARA_HIT_LUARM	:		//	왼쪽 팔을 맞았다.
	case CHARA_HIT_LDARM	:
	case CHARA_HIT_LHAND	:
	case CHARA_HIT_LCLAVICLE	:
		if( bFront)
		{
			if( nType == 0)
			{
				id = ID_UPPER_DAMAGE_FRONT_LEFT_A;
			}
			else
			{
				id = ID_UPPER_DAMAGE_FRONT_LEFT_B;
			}
		}
		else
		{
			if( nType == 0)
				id = ID_UPPER_DAMAGE_BACK_A;
			else
				id = ID_UPPER_DAMAGE_BACK_B;
		}
		break;

	//	오른쪽 데미지
	case CHARA_HIT_RUARM	:
	case CHARA_HIT_RDARM	:
	case CHARA_HIT_RHAND	:
	case CHARA_HIT_RCLAVICLE	:
		if( bFront)
		{
			if( nType == 0)
			{
				id = ID_UPPER_DAMAGE_FRONT_RIGHT_A;
			}
			else
			{
				id = ID_UPPER_DAMAGE_FRONT_RIGHT_B;
			}
		}
		else
		{
			if( nType == 0)
				id = ID_UPPER_DAMAGE_BACK_A;
			else
				id = ID_UPPER_DAMAGE_BACK_B;
		}
		break;
	//	다리
	case CHARA_HIT_ROOT		:
	case CHARA_HIT_LULEG	:
	case CHARA_HIT_LDLEG	:
	case CHARA_HIT_RULEG	:
	case CHARA_HIT_RDLEG	:
	case CHARA_HIT_LFOOT	:
	case CHARA_HIT_RFOOT	:
		if( nType == 0)
		{
			id = ID_UPPER_DAMAGE_BACK_B;
		}
		else
			id = ID_UPPER_DAMAGE_BACK_A;
		break;

	default :	I3TRACE( "[%s] : WARNING! Hit part(%d) is Invalid!\n", __FUNCTION__, part ); 
		ANIM_playUpper( CHARA_UPPER_DAMAGE, ID_UPPER_DAMAGE_BACK_B);
		return FALSE;
	}
	
	BOOL	bCheckAction = FALSE;

	if( m_pAnimContext->getOldUpperID() != id )
	{
		if( getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
		{
			if( m_pAnimContext->getAIUpper() != NULL && m_pAnimContext->getAIUpper()->getElapsedTime() > _ANIM_getUpperDuration() * 0.2f )
				bCheckAction = TRUE;
		}
		else
			bCheckAction = TRUE;
	}
	else if( m_pAnimContext->getAIUpper() != NULL && m_pAnimContext->getAIUpper()->getCurrentAIState() != NULL)
	{
		if( m_pAnimContext->getAIUpper()->getElapsedTime() > _ANIM_getUpperDuration() * 0.5f)
		{
			bCheckAction = TRUE;
		}
	}

	if( bCheckAction )
	{
		ANIM_playUpper( CHARA_UPPER_DAMAGE, id );
	}

	//	데미지 신음소리 플레이~	외부에서 호출해줍니다.
	//PlayCharaDamageSound( part, TRUE);


	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_Death( VEC3D * pHitDir, CHARA_DEATH_TYPE type, INT32 nKillerIdx)
{	
	//ACTION_IdleUpper();
	CALL_ACTION_COMMAND(ACTION_CMD_STEADYUPPER, NULL);

	m_pAnimContext->getAIUpper()->SetAIStateImmediately( g_pViewer->getDeltaTime());

	//	Death 동작이므로 NONE 상태로 만들어 준다.
	getActionContext()->setBody2( CHARA_BODY_NONE);
	getActionContext()->setUpper( CHARA_UPPER_NONE);

	if( is1PV())
	{
		//_CAMERA_SetIdle();			//	일반 카메라 상태 (캐릭터 Visible On, ScopeHUD off, Cam FOV default)
		m_pCameraContext->SetIdleCamera();
	}	

	ApplyCrosshair();
	
	m_pMoveContext->setVelocity( 0.0f, 0.0f, 0.0f);		// 이동값을 0으로
	m_pMoveContext->setAccel( 0.0f, 0.0f, 0.0f);

	ID_LOWER_AI nId = ID_LOWER_DEATH_BACK_A;

	if( is1PV())		//	자신일 경우 Kill Camera로 변경
	{
		g_pCameraManager->ChangeCamera( "Kill", 0.0f);
		g_pCameraManager->setTargetCharacter(this);
		// 킬 카메라 시작시 1인칭 캐릭터는 안보이도록 합니다.
		SetVisible( FALSE );	
		
		if( g_pCamera->IsExactTypeOf( CGameCamera_Kill::GetClassMeta()))
		{
			CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
			pKillCam->SetEnemyIdx( nKillerIdx);
		}

		//	Screen Effect 초기화
		//{
		//	CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		//	I3ASSERT( pStage != NULL);

		//	i3RasterizeChain_Blur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
		//	I3ASSERT( pRasterizeBlur != NULL);

		//	CHud * pHud = pStage->getHud();
		//	if( pHud)		
		//	{
		//		pHud->SetScreenEffectEnable( FALSE);
				//pRasterizeBlur->SetEnable( FALSE);	//	<- 죽었을 경우도 계속 모션 블러 적용하려면 주석 처리 할것!
				
				//	죽었을 경우도 계속 모션 블러 적용하려면 활설화 할것!
		//		if( m_rFlashElapseTime > 0.0f)		//	섬광 효과가 진행중이면 모션 블러 이펙트를 끈다.
		//		{
		//			pRasterizeBlur->SetEnable( FALSE);

		//			pHud->SetScreenTex2DEnable( FALSE);
		//			m_rDamageElapseTime = 0.0f;	
		//		}

		//		m_rFlashElapseTime = m_rFlashEffectiveTime = 0.0f;
				//m_rDamageElapseTime = 0.0f;			//	<- 죽었을 경우도 계속 모션 블러 적용하려면 주석 처리 할것!
		//	}
		}
	}
	else				//	자기외 캐릭터
	{
		ChangeAnimSet( CHARA_ANIMSET_3PV, FALSE, !is1PV());
	}

	if( EFFECT_IsAvailable())
	{
		EFFECT_Clear();
	}

	_ANIM_stop();

	m_pAnimContext->getAIUpper()->SetEnable( false, true);
	m_pAnimContext->setAIUpper( NULL);

	ANIM_playLower( CHARA_LOWER_DEATH, nId);

	// 모든 기능을 끕니다.
	addEnableStateMask( CHARA_STATE_DISABLE_CONTROL | CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE |
		CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE );

	addStateMask( CHARA_STATE_USED_COMBINE_ANIM);

	//EnableCollisionGroup( FALSE );

	//	무기도 모든 동작을 종료하고 Ready 상태로
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon != NULL)
	{
		pWeapon->ReturnToReady();
		pWeapon->SetVisible( TRUE);
	}

	//I3TRACE( "Death!!  - %d\n", getCharaNetIndex() );

	return TRUE;
}

void CGameCharaPlayer::ACTION_GhostFire( WEAPON_FIRE_ORDER type)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);

	CHARA_ACTION_UPPER	state = CHARA_UPPER_NONE;
	ID_UPPER_AI nId = ID_UPPER_UNKNOWN;

	//	주 공격
	if( type == WEAPON_PRIMARY_FIRE)
	{
		state = CHARA_UPPER_FIRE;

		switch( pWeaponInfo->m_TypeClass)
		{			
		case WEAPON_CLASS_KNIFE:
			{	//	나이프일 경우 공격 처리
				I3ASSERT( pWeapon->IsExactTypeOf( WeaponKnife::GetClassMeta()));
				WeaponKnife * pWeaponKnife = (WeaponKnife *) pWeapon;

				pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				nId = ID_UPPER_ATTACK;
			}
			break;
		case WEAPON_CLASS_HANDGUN:
		case WEAPON_CLASS_ASSAULT:
		case WEAPON_CLASS_SMG:
		case WEAPON_CLASS_MG:
		case WEAPON_CLASS_SNIPER:
		case WEAPON_CLASS_SHOTGUN:	
		case WEAPON_CLASS_THROWING:
		case WEAPON_CLASS_MISSION:
		case WEAPON_CLASS_ITEM:
			{
				nId = ID_UPPER_ATTACK;
			}
			break;
		default:	I3ASSERT_0;
		}
	}
	//	보조 공격
	else if( type == WEAPON_SECONDARY_FIRE)
	{
		state = CHARA_UPPER_SECONDARY_FIRE;
		nId = ID_UPPER_SECONDARY_ATTACK;

		if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_KNIFE )
		{
			I3ASSERT( pWeapon->IsExactTypeOf( WeaponKnife::GetClassMeta()));
			WeaponKnife * pWeaponKnife = (WeaponKnife *) pWeapon;

			pWeaponKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
		}
	}

	I3ASSERT( state != CHARA_UPPER_NONE && nId != ID_UPPER_UNKNOWN);

	// 발사!!		
	ANIM_playUpper( state, nId);
}

BOOL CGameCharaPlayer::ACTION_FireReady( void)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);
	
	pWeapon->SetVisible( TRUE);

	// (임시) 발사 준비. 현재 투척 무기만 가능한 동작....변경시 애니메이션 추가 필요함.
	if( pWeaponInfo->m_TypeClass != WEAPON_CLASS_THROWING)		return FALSE;
	
	pWeapon->FireReady();

	ANIM_playUpper( CHARA_UPPER_FIRE_READY, ID_UPPER_ATTACK_READY);	

	addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	발사 준비중에 발사하지 못하도록. 애니메이션이 끝나면 발사 가능

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_Fire( VEC3D * pTargetPos, WEAPON_FIRE_ORDER type)
{
	ACTION_SyncWeapon();
	bool bApplyExt = true;

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

	if( !isNetwork() )
	{
		ACTION_GhostFire( type);
		pWeapon->Fire( type);

		// 이건	버그이나 기획실의 요청으로 사용하지 않습니다. 발사중에 스왑안되는 기능입니다.
		//addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
		// 수류탄 계열은 버그로 던지지 못하고 없어질 수 있습니다.
		if( pWeapon->getWeaponInfo()->m_TypeClass == WEAPON_CLASS_THROWING )
		{
			addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
		}

		ApplyCrosshair();
	}

	if( pWeapon->getExtensionType() & WEAPON_EXT_MASK_NEED_TO_ACTIVATE)
	{
		if( pWeapon->isExtensionActivate() == false)
		{
			bApplyExt = false;
		}
	}

	if( !(pWeapon->getExtensionType() & WEAPON_EXT_MASK_DIRECT_USE))
	{
		if( !isStateMask( CHARA_STATE_ZOOM))
			bApplyExt = false;
	}

	// Extension이 활성화 되어 있을 때의 처리..

	// 수직 반동
	CHARAPLAYER_RECOIL * pRecoilData = m_pBoneContext->getRecoilData();

	{
		REAL32 factor = 1.0f;

		if( bApplyExt)		factor = pWeaponInfo->m_ExtRecoilVertFactor;

		pRecoilData->m_RecoilAngle +=	pWeapon->getRecoil() * factor;
		if( pRecoilData->m_RecoilAngle > pWeapon->getRecoilMax())
			pRecoilData->m_RecoilAngle = pWeapon->getRecoilMax();

		pRecoilData->m_RecoilTargetAngle = pRecoilData->m_RecoilAngle;
		pRecoilData->m_RecoilTargetTime = log10f( pRecoilData->m_RecoilAngle);
		pRecoilData->m_RecoilTime = 0.0f;
	}

	// 수평 반동
	{
		INT32 limitCount;

		if( bApplyExt)
			limitCount = pWeaponInfo->m_ExtRecoilHorzCount;
		else
			limitCount = pWeapon->getRecoilHorzCount();

		// 지정 개수 이하의 연사에서는 수평 반동을 적용하지 않는다.
		if( limitCount < pWeapon->getAutoFireCount())
		{
			REAL32 factor = 1.0f;

			if( bApplyExt)	factor = pWeaponInfo->m_ExtRecoilHorzFactor;

			pRecoilData->m_RecoilHorzAngle = i3Math::Randf2() * pWeapon->getRecoilHorz() * factor;

			// 만약 변경된 Theta의 값이 지정된 범위를 벗어난다면
			// 반대로 반향으로 적용되도록 한다.
			{
				REAL32 diff = i3Math::abs( (I3_RAD2DEG( m_pBoneContext->getTheta()) + pRecoilData->m_RecoilHorzAngle) - I3_RAD2DEG(pRecoilData->m_HorzStartAngle));

				if( diff > pWeapon->getRecoilHorzMax())
				{
					//I3TRACE( "Turn Angle : %f, %f, %f\n", I3_RAD2DEG(m_Theta), pRecoilData->m_RecoilHorzAngle, I3_RAD2DEG( pRecoilData->m_HorzStartAngle));

					pRecoilData->m_RecoilHorzAngle = -pRecoilData->m_RecoilHorzAngle;		// 방향 뒤집기
				}
			}

			pRecoilData->m_RecoilHorzTime = 0.0f;
			pRecoilData->m_RecoilHorzTimeTarget = MIN( pWeapon->getWeaponInfo()->m_rFireDelay, 0.5f);

			if( pRecoilData->m_RecoilHorzTimeTarget > I3_EPS )
				pRecoilData->m_rRecoilHorzTimeTarget = 1.0f / pRecoilData->m_RecoilHorzTimeTarget;
			else
				pRecoilData->m_rRecoilHorzTimeTarget = 0.0f;
		}
	}

	// 편차
	CHARAPLAYER_DEVIATION * pDeviationData = m_pBoneContext->getDeviationData();
	{
		REAL32 dev, factor = 1.0f;

		if( bApplyExt)	factor = pWeaponInfo->m_ExtDeviationFactor;

		dev = pWeapon->getDeviation() * factor;

		pDeviationData->m_FireDeviation = pDeviationData->m_DeviationAngle;
		pDeviationData->m_DeviationAngle = pDeviationData->m_DeviationTargetAngle = pDeviationData->m_DeviationAngle + dev;
		pDeviationData->m_DeviationStartAngle = pDeviationData->m_DeviationAngle;

		if( pDeviationData->m_DeviationTargetAngle > pWeaponInfo->m_rCrossHairMax)
			pDeviationData->m_DeviationTargetAngle = pWeaponInfo->m_rCrossHairMax;

		pDeviationData->m_DeviationTargetTime = pWeapon->getCrosshairSpeedFactor() * pWeaponInfo->m_CrossHairContractTime;
		pDeviationData->m_DeviationTime = 0.0f;
		pDeviationData->m_DeviationState = DEVIATION_CHANGE;
	}

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_ToggleExtensionWeapon( void)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	BOOL bActivate = pWeapon->isExtensionActivate();
	
	return ACTION_ExtensionWeapon( !bActivate);
}

BOOL CGameCharaPlayer::ACTION_ExtensionWeapon( BOOL bActivate)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	//	무기 Extension 동작
	if( pWeapon->ExtensionActivate( bActivate) == FALSE)
		return FALSE;

	pWeapon->SetVisible( TRUE);

	//	무기 Extension 기능 활성화
	if( bActivate)
	{	
		ANIM_playUpper( CHARA_UPPER_EXTENSION_ACTIVATE, ID_UPPER_EXTENSION_ACTIVATE);

#if defined( I3_DEBUG )
		if( !isNetwork() )
			g_pFramework->getChatBox()->PutSystemChatting( "Extension On");
#endif
	}
	else
	{	
		ANIM_playUpper( CHARA_UPPER_EXTENSION_DEACTIVATE, ID_UPPER_EXTENSION_DEACTIVATE);

#if defined( I3_DEBUG )
		if( !isNetwork() )
			g_pFramework->getChatBox()->PutSystemChatting( "Extension Off");
#endif
	}

	if( !isNetwork() )
		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	발사 준비중에 발사하지 못하도록. 애니메이션이 끝나면 발사 가능

	return TRUE;
}*/

//BOOL CGameCharaPlayer::ACTION_Hit( WeaponBase * pShooterWeapon)
/*
BOOL CGameCharaPlayer::CharaHit( WeaponBase * pShooterWeapon)
{
	I3ASSERT( pShooterWeapon != NULL);

	CWeaponInfo * pWeaponInfo = pShooterWeapon->getWeaponInfo();

	CHARA_DAMAGEINFO DamageInfo;							
	CGameCharaPlayer * pKiller = pShooterWeapon->getOwner();
	if( pKiller == NULL )
		return FALSE;

	DamageInfo._WeaponType		= pWeaponInfo->m_TypeClass;
	DamageInfo._DamageType		= pWeaponInfo->m_TypeDamage;
	DamageInfo._rEffectiveTime	= pWeaponInfo->m_rEffectiveTime;
	DamageInfo._rExplosionRange = pWeaponInfo->m_rExplosion_Range;
	DamageInfo._rExplosionSlope = pWeaponInfo->m_rExplosion_EffectiveSlope;
	DamageInfo._rDamage			= pShooterWeapon->getAP();	
	DamageInfo._IsOwner			= !pKiller->isNetwork();
	DamageInfo._nKillerIdx		= pKiller->getCharaNetIndex();
	DamageInfo._nHarmerIdx		= pWeaponInfo->m_TypeUsage;
	DamageInfo._pHitResult		= NULL;
	DamageInfo._DeathType		= CHARA_DEATH_UN;	
	DamageInfo._pVecDir			= NULL;
	DamageInfo._pWallHitResult	= NULL;
	DamageInfo._nWeaponNumber	= pWeaponInfo->m_nNumber;

	{
		VEC3D diff;

		i3Vector::Sub( &diff, pKiller->GetPos(), GetPos());
		DamageInfo._rDistance	= i3Vector::Length( &diff);
	}
	
	if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING || pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION)
		DamageInfo._pVecPos		= pShooterWeapon->GetPos();
	else
		DamageInfo._pVecPos		= pKiller->GetPos();

	//I3ASSERT( DamageInfo.isValid() );

	//return ACTION_Hit( &DamageInfo);
	return CharaHit( &DamageInfo);
}
*/
//BOOL CGameCharaPlayer::ACTION_Hit( CHARA_DAMAGEINFO * pDamageInfo)
/*
BOOL CGameCharaPlayer::CharaHit( CHARA_DAMAGEINFO * pDamageInfo)
{
	I3_PHYSIX_HIT_RESULT * pResult;
	I3ASSERT( pDamageInfo != NULL);

	INT32 effectCount = 1;

	// Last Attacker에 대한 갱신
	{		
		m_pLastAttacker = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx);
	}
	
	switch( pDamageInfo->_HitPart )
	{
	case CHARA_HIT_HEAD :	effectCount = 6;	break;
	case CHARA_HIT_NECK :
	case CHARA_HIT_LCLAVICLE :
	case CHARA_HIT_RCLAVICLE :
	case CHARA_HIT_BODY1 :
	case CHARA_HIT_BODY2 :
	case CHARA_HIT_BODY3 :
	case CHARA_HIT_ROOT :
		effectCount = 3;	break;
	}
	
	{
		//	1인칭 자신이 피격 됬을때 화면 및 이펙트 처리합니다.
		if( is1PV()) 
		{
			//	피가 나오는 방향을 앞방향 대각선 위로 수정하고 시작지점은 자신의 앞으로 한다.
			VEC3D vPos;
			MATRIX Temp;
			i3Matrix::Identity( &Temp);
			i3Matrix::SetRotateAxisDeg( &Temp, i3Matrix::GetRight( g_pCamera->getCamMatrix()), -45.0f); 		
			i3Vector::Scale( &vPos, i3Matrix::GetAt( g_pCamera->getCamMatrix()), 0.5f);
			vPos.x = -vPos.x;	//	카메라의 At이 거꾸로?
			vPos.y = -vPos.y;
			vPos.z = -vPos.z;
			i3Vector::Add( &vPos, i3Matrix::GetPos( m_pBoneContext->getIKMatrix()), &vPos);

			//	Hit Effect
			g_pEffectManager->AddCharaHitEffect( WEAPON_CLASS_UNKNOWN, &vPos, i3Matrix::GetUp( &Temp), pDamageInfo->_rDistance);
		    
			//	Hit Sound
			//g_pEffectManager->PlayCharaHitSound( i3Matrix::GetPos( getHeadMatrix()), pDamageInfo->_WeaponType, TRUE);
			if( pDamageInfo->_WeaponType == WEAPON_CLASS_KNIFE)
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNIFE_HUMAN, 0, &vPos, TRUE);
			}
			else
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HUMAN, 2, &vPos, TRUE);
			}
		}
		//	3인칭 경우
		else
		{
			if( pDamageInfo->_pHitResult != NULL)
			{	
				MATRIX mat;
				VEC3D vPos, vRand;
				VEC3D * pPos, * pNormal;
				REAL32 rRandFactor = 0.05f;

				pPos = &pDamageInfo->_pHitResult->m_Intersect;
				pNormal = &pDamageInfo->_pHitResult->m_Normal;
				i3Vector::Zero( &vRand);

				if( pDamageInfo->_WeaponType == WEAPON_CLASS_THROWING )
				{	// 수류탄 예외 처리
					for( INT32 i = 0; i < m_PhysixShapeSetList.GetCount(); i++ )
					{
						i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_PhysixShapeSetList.GetItem( i);
						if( getCollisionContext()->GetHitPart( pShapeSet) == CHARA_HIT_NECK)
						{
							pShapeSet->getShapeGlobalPose( &mat);

							pPos = i3Matrix::GetPos( &mat);
							break;
						}
					}

					//pNormal = &vNormal;
					//i3Vector::Set( &vNormal, 0.f, 1.f, 0.f);
					effectCount = 3;
					rRandFactor = 0.5f;
				}

				for( INT32 i = 0; i < effectCount; i++ )
				{
					i3Vector::Add( &vPos, pPos, &vRand);
					g_pEffectManager->AddCharaHitEffect( pDamageInfo->_WeaponType, &vPos, 
														&pDamageInfo->_pHitResult->m_Normal, pDamageInfo->_rDistance);
					i3Vector::Set( &vRand, i3Math::Randf2() * rRandFactor, i3Math::Randf2() * rRandFactor, i3Math::Randf2() * rRandFactor);
				}

				BOOL bListener = (pDamageInfo->_nKillerIdx == g_pGameContext->getMySlotIdx()) ? TRUE : FALSE;

				if( bListener)
				{
					if( pDamageInfo->_WeaponType == WEAPON_CLASS_KNIFE)
					{
						g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNIFE_HUMAN, 0, &pDamageInfo->_pHitResult->m_Intersect, TRUE);
					}
					else
					{
						g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HUMAN, i3Math::Rand()%2, &pDamageInfo->_pHitResult->m_Intersect, TRUE);
					}
				}
				else
				{
					if( pDamageInfo->_WeaponType == WEAPON_CLASS_KNIFE)
					{
						g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_KNIFE_HUMAN, 0, &pDamageInfo->_pHitResult->m_Intersect, FALSE);
					}
					else
					{
						g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_HUMAN, i3Math::Rand()%2, &pDamageInfo->_pHitResult->m_Intersect, FALSE);
					}
				}
			}
		}
	}

	// Wall effect
	if( g_pEnvSet->m_bEnableBlood )
	{
		if( pDamageInfo->_rDamage < I3_EPS )
			pDamageInfo->_rDamage = I3_EPS;

		REAL32	rHeight = 1.f /(REAL32)pDamageInfo->_rDamage;
		BLOOD_EFFECT_TYPE effectType = BLOOD_EFFECT_ASSAULT;

		switch( pDamageInfo->_WeaponType )
		{
		case WEAPON_CLASS_ASSAULT :		effectType = BLOOD_EFFECT_ASSAULT;	rHeight *= 21.f;	break;
		case WEAPON_CLASS_SMG :			effectType = BLOOD_EFFECT_SMG;		rHeight *= 25.f;	break;
		case WEAPON_CLASS_HANDGUN :		effectType = BLOOD_EFFECT_HANDGUN;	rHeight *= 20.f;	break;
		case WEAPON_CLASS_SHOTGUN :		effectType = BLOOD_EFFECT_SHOTGUN;	rHeight *= 15.f;	break;
		case WEAPON_CLASS_KNIFE :		effectType = BLOOD_EFFECT_KNIFE;	rHeight *= 80.f;	break;
		case WEAPON_CLASS_THROWING :	effectType = BLOOD_EFFECT_GRENADE;	rHeight *= 70.f;	break;
		case WEAPON_CLASS_SNIPER :		effectType = BLOOD_EFFECT_RIFLE;	rHeight *= 50.f;	break;
		case WEAPON_CLASS_MG :			effectType = BLOOD_EFFECT_ASSAULT;	rHeight *= 20.f;	break;
		}

		// 벽면에 피를 붙입니다.
		if( pDamageInfo->_pWallHitResult != NULL )
		{
			pResult = pDamageInfo->_pWallHitResult;
			if( pResult->m_pTerrain != NULL )
			{
				i3Object * pObj = NULL;
				if( pResult->m_pDestShapeSet->getEventReceiver()->IsExactTypeOf( i3Object::GetClassMeta()) )
				{
					pObj = (i3Object*) pResult->m_pDestShapeSet->getEventReceiver();
				}

				if( effectType == BLOOD_EFFECT_KNIFE )
					g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_FLOOR, effectType, &pResult->m_Intersect, &pResult->m_Normal, pDamageInfo->_pVecDir, 0.f, pObj );
				else
					g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_WALL, effectType, &pResult->m_Intersect, &pResult->m_Normal, pDamageInfo->_pVecDir, 0.f, pObj );
			}
		}
		else
		{
			// 바닥에 피를 붙입니다.
			VEC3D	vDir, vRand;
			i3CollideeLine Line;

			if( pDamageInfo->_WeaponType == WEAPON_CLASS_KNIFE )
			{
				i3Vector::Set( &vDir, -getX( pDamageInfo->_pVecDir ), -getY( pDamageInfo->_pVecDir ) - rHeight, -getZ( pDamageInfo->_pVecDir ) );
			}
			else
			{
				i3Vector::Set( &vDir, getX( pDamageInfo->_pVecDir ), -getY( pDamageInfo->_pVecDir ) - rHeight, getZ( pDamageInfo->_pVecDir ) );
			}

			if( pDamageInfo->_pHitResult != NULL )
				Line.SetStart( &pDamageInfo->_pHitResult->m_Intersect );
			else
			{
				Line.SetStart( i3Matrix::GetPos( m_pBoneContext->getHeadMatrix() ) );
				//i3Vector::Set( &vDir, -getX( &vDir), -getY(&vDir), -getZ(&vDir) );
			}

			i3Vector::Set( &vRand, (REAL32)((i3Math::Rand() % 100)-50)*0.001f, (REAL32)((i3Math::Rand() % 100)-50)*0.001f, (REAL32)((i3Math::Rand() % 100)-50)*0.001f );
			i3Vector::Add( &vDir, &vDir, &vRand );
			Line.SetDir( &vDir );

			UINT32 nResultCount = g_pFramework->raycast( &Line, CGRP_ATTACK );
			if( nResultCount > 0 )
			{
				pResult = g_pFramework->getRaycastResult();
				for( UINT32 i = 0; i < nResultCount; i++ )
				{
					i3Object * pObj = NULL;
					if( pResult[ i ].m_pTerrain != NULL && pResult[ i ].m_T < 5.f )
					{
						I3ASSERT( pDamageInfo->_nKillerIdx < SLOT_MAX_COUNT );

						CGameCharaPlayer * pChara = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx );
						REAL32	rRotate = 0.f;
						if( pChara != NULL )
						{
							rRotate = -pChara->getBoneContext()->getTheta();
						}

						if( pResult[ i ].m_pDestShapeSet->getEventReceiver()->IsExactTypeOf( i3Object::GetClassMeta() ) )
						{
							pObj = (i3Object*) pResult[ i ].m_pDestShapeSet->getEventReceiver();
						}

						VEC3D vDir;
						i3Vector::Sub( &vDir, &pResult[ i ].m_Intersect, Line.GetStart() );
						i3Vector::Normalize( &vDir, &vDir );
						 
						g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_FLOOR, effectType, &pResult[ i ].m_Intersect, &pResult[ i ].m_Normal, &vDir, rRotate, pObj );
					}	
				}
			}
		}
	}

	//	피격 스크린 이펙트를 한다. (Onwer가 아닌경우 해야 한다.)
	{		
		CWeaponGlobal * pWeaponGlobal = g_pWeaponInfoDataBase->getGlobal();
		I3ASSERT( pWeaponGlobal != NULL);

		EFFECT_Damage( pDamageInfo->_rEffectiveTime, 
						&pWeaponGlobal->m_DamageEffectColor, 
						pWeaponGlobal->m_DamageEffectBlurStart,
						pWeaponGlobal->m_DamageEffectBlurEnd);
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_ActiveZoom( void)
{
	BOOL bResult = FALSE;

	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL );

	WEAPON_ZOOM_TYPE type = pWeaponInfo->m_TypeZoom;

	//	견착 기능이 있는 무기
	switch( type)
	{
		case  WEAPON_ZOOM_ZOOM:
			if( ! isStateMask( CHARA_STATE_ZOOM))
			{
				_ANIM_playBody2( CHARA_BODY_ZOOM, !is1PV());

				//_CAMERA_SetZoom();		//	견착 카메라 상태
				m_pCameraContext->SetZoom();
				
				bResult = TRUE;
			}
			break;

		case WEAPON_ZOOM_SCOPE :
			//	스코프 기능이 있는 무기
			{
				INT32 nLvl = m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel + 1;

				if( ! isStateMask( CHARA_STATE_ZOOM))
					_ANIM_playBody2( CHARA_BODY_ZOOM, !is1PV());

				//bResult = _CAMERA_SetScope( nLvl);		//	스나이퍼 카메라 상태	
				bResult = m_pCameraContext->SetScope( nLvl);
			}
			break;

		case WEAPON_ZOOM_STEADY_AIM :
			// 다른 어떤 Animation 및 Crosshair의 변경없이 FOV만을 조절
			if( ! isStateMask( CHARA_STATE_ZOOM))
			{
				//_CAMERA_SetZoom();		//	견착 카메라 상태
				m_pCameraContext->SetZoom();

				bResult = TRUE;
			}
			break;
	}

	addStateMask( CHARA_STATE_ZOOM);

	ApplyCrosshair();

	return bResult;
}

BOOL CGameCharaPlayer::ACTION_CancelZoom( void)
{	//	Zoom 취소

	BOOL bResult = FALSE;

	if( isStateMask( CHARA_STATE_ZOOM))
	{
		_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV());

		//_CAMERA_SetIdle();			//	일반 카메라 상태
		m_pCameraContext->SetIdleCamera();

		bResult = TRUE;
	}

	removeStateMask( CHARA_STATE_ZOOM);

	ApplyCrosshair();

	return bResult;
}

BOOL CGameCharaPlayer::ACTION_ToggleZoom( void)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL );

	WEAPON_ZOOM_TYPE type = pWeaponInfo->m_TypeZoom;

	//	견착 기능이 있는 무기
	switch( type)
	{
		case WEAPON_ZOOM_ZOOM :
			if( isStateMask( CHARA_STATE_ZOOM))
				return ACTION_CancelZoom();		//	현재 견착일땐 취소
			else
				return ACTION_ActiveZoom();		//	현재 보통일땐 견착
			break;

		case WEAPON_ZOOM_SCOPE :
			//	스코프 기능이 있는 무기
			if( isStateMask( CHARA_STATE_ZOOM))
			{
				if( m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel < pWeaponInfo->m_nMaxScopeLevel )
					return ACTION_ActiveZoom();
				else
					return ACTION_CancelZoom();		//	현재 견착일땐 취소
			}
			else
				return ACTION_ActiveZoom();		//	현재 보통일땐 견착
			break;

		case WEAPON_ZOOM_STEADY_AIM :
			if( ! isStateMask( CHARA_STATE_ZOOM))
				ACTION_ActiveZoom();
			else
				ACTION_CancelZoom();
			break;

	}

	return FALSE;
}

void CGameCharaPlayer::ACTION_SetObserveZoom(void)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();

	if( pWeapon == NULL )
		return;

	I3ASSERT( pWeapon->getWeaponInfo() != NULL);

	WEAPON_ZOOM_TYPE type = pWeapon->getWeaponInfo()->m_TypeZoom;
	BOOL	bCheckZoom = FALSE;

	if( isAlive() )
	{
		if( type == WEAPON_ZOOM_ZOOM || type == WEAPON_ZOOM_SCOPE )
		{
			if( getActionContext()->isBody2( CHARA_BODY_ZOOM ))
			{
				if( type == WEAPON_ZOOM_ZOOM)
				{
					//_CAMERA_SetZoom();
					m_pCameraContext->SetZoom();
					bCheckZoom = TRUE;
				}
				else if( type == WEAPON_ZOOM_SCOPE)
				{
					//_CAMERA_SetScope( m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel);
					m_pCameraContext->SetScope( m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel);
					bCheckZoom = TRUE;
				}
			}
		}
	}

	if( !bCheckZoom )
	{
		//_CAMERA_SetIdle();
		m_pCameraContext->SetIdleCamera();
	}
}*/

//void CGameCharaPlayer::ACTION_SyncWeapon( void)
/*
void CGameCharaPlayer::SyncWeapon( void)
{
	//	무기 교체시(SwapWeapon)은 다음 프레임에 적용되기 때문에 현재 프레임에서 교체 무기를 적용할 필요가 있을 경우
	//	호출해야 합니다. (네트워크 같은 ...)
	//	
	//	예.)
	//	Frame			Command			Current Weapon
	//=====================================================================
	//	(1) :	SwapWeapon (No.1)	CurWeapon(No.0) <= current (before swap)
	//			Fire (No.1)					:
	//	(2) :		:				CurWeapon(No.1)	<= Apply swap 

	//if( m_nSwapWeaponSlotNum != m_nCurWeaponSlotNum)
	INT32 nSwapSlot = getWeaponContext()->getSwapSlotNum();
	INT32 nCurrentSlot = getWeaponContext()->getCurrentSlotNum();
	INT32 nOldSlot = getWeaponContext()->getOldSlotNum();

	if( nSwapSlot != nCurrentSlot )
	{
		//	무기 노드 변경
		getWeaponContext()->getControl()->OnSelectWeapon( nSwapSlot);
		if( isNetwork() )	// invalid using
		{
			getWeaponContext()->getControl()->OnAttachPurchasePrimWeapon( nOldSlot);
			getWeaponContext()->getControl()->OnAttachPurchaseSideWeapon( nOldSlot);
		}
		
		//ACTION_apply( !is1PV());
		getAnimContext()->getControl()->AnimSetApply();

		ApplyCrosshair();
	}
	/*
	if( getWeaponContext()->getWeaponSwapSlotNum() != getWeaponContext()->getCurrentSlotNum() )
	{
		//	무기 노드 변경
		WEAPON_Select( m_nSwapWeaponSlotNum);
		if( isNetwork() )	// invalid using
		{
			WEAPON_AttachToBackpack( m_nOldWeaponSlotNum);
			WEAPON_AttachToSidepack( m_nOldWeaponSlotNum);
		}
		
		ACTION_apply( !is1PV());

		ApplyCrosshair();
	}
	*/
}
/*
BOOL CGameCharaPlayer::ACTION_SwapWeapon( INT32 slot)
{
	// 무기 제한 검사
	if (TRUE == g_pGameContext->IsLockedWeaponSlot(slot))
	{
		return FALSE;
	}

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( slot == 5)
	{
		if( (pMissionMng->getStageType() == STAGE_TYPE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
			return FALSE;	// 폭탄인 경우 설치전이 아니라면 바꿀 수 없습니다.
	}

	WeaponBase * pSwapWeapon = getWeaponContext()->getWeapon( slot);
	INT32	nCurSlot = getWeaponContext()->getCurrentSlotNum();
	
	if( pSwapWeapon != NULL && nCurSlot != slot)
	{
		CWeaponInfo * pWeaponInfo = pSwapWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);

		if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING ||
			pWeaponInfo->m_TypeClass == WEAPON_CLASS_ITEM || 
			pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION)
		{
			//	소모성 무기인 Throwing 또는 Item은 지닌 갯수가 0이면 교체될 수 없다.
			if( pSwapWeapon->getLoadedBulletCount() <= 0)
				return FALSE;
		}

		//	Weapon Change
		getWeaponContext()->getControl()->OnSwapWeapon( slot);*/

		//--> WeaponControl
		/*
		WeaponBase * pOldWeapon = getWeaponContext()->getCurrent();
		if( pOldWeapon != NULL)
		{	
			//	캐릭터 애니메이션을 변경하면 다음 프레임부터 적용되기 때문에 이번 프레임에 무기를 바꾸지 않고 다름 프레임에 
			//	무기를 바꾸도록 한다.
			m_nOldWeaponSlotNum = m_nCurWeaponSlotNum;
			m_nSwapWeaponSlotNum = slot;		//	다음 프레임에 바뀔 무기 번호 
		}
		//	현재 무기가 NULL이면 당장 바꾼다.
		else
		{			
			//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.
			WEAPON_Select( slot);			

			WeaponBase * pChangeWeapon = getWeaponContext()->getCurrent();
			if( pChangeWeapon != NULL )
			{
				if( isNetwork() )
					pChangeWeapon->NET_WeaponChange();
				else
					pChangeWeapon->WeaponChange();		//	무기 교체시 이후 무기는 Change 상태로
			}
		}

		//	무기는 기본 오른손에 부착	
		ChangeWeaponAttachHand( CHARA_WEAPON_ATTACH_RIGHT_HAND);

		pSwapWeapon->SetVisible( TRUE);

		*/

		/*getActionContext()->setBody2( CHARA_BODY_ATTACKIDLE);

		if( isStateMask( CHARA_STATE_ZOOM))
		{	//	견착 상태에서는 일반 카메라로 돌아갑니다
			//_CAMERA_SetIdle();
			m_pCameraContext->SetIdleCamera();
		}

		if( m_pAnimContext->getAIUpper() != NULL)
		{
			m_pAnimContext->getAIUpper()->StopAI( g_pViewer->getDeltaTime());
		}

		//ACTION_apply( !is1PV());	//	이미 캐릭터는 다음 무기의 동작으로 설정한다.
		getAnimContext()->getControl()->AnimSetApply();

		//	교체가 끝나고 애니메이션을 한다.
		ANIM_playUpper( CHARA_UPPER_SWAPWEAPON, ID_UPPER_SWAP_WEAPON);
		
		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		//addStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE);
		
		// 네트워크로 보내줍니다.
		//if( !isNetwork() )
		//	g_pGameContext->WriteMyWeaponChange( m_nCurWeaponSlotNum );		//-->WeaponControlLocal

		//I3TRACE("Changing! Weapon (%d)\n", slot);

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_SwapNextWeapon( void)
{
	INT32 nIdx = getWeaponContext()->getCurrentSlotNum() + 1;

	if( nIdx > MAX_ITEMSLOT_COUNT - 1)		nIdx = 0;

	if( nIdx == 5)
	{
		CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
		if( (pMissionMng->getStageType() == STAGE_TYPE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
			nIdx = 0;	// 폭탄인 경우 설치전이 아니라면 바꿀 수 없습니다.
	}

	//	다음 번호의 무기로 교체
	while( !ACTION_SwapWeapon( nIdx))
	{
		if( nIdx < MAX_ITEMSLOT_COUNT - 1)
			nIdx++;
		else	
			nIdx = 0;

		if( nIdx == getWeaponContext()->getCurrentSlotNum())	//	한바퀴 돌면( 바꿀 무기가 없다면)
			return FALSE;
	}

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_SwapPrevWeapon( void)
{
	INT32 nIdx = getWeaponContext()->getCurrentSlotNum() - 1;

	if( nIdx < 0)		nIdx = MAX_ITEMSLOT_COUNT - 1;

	if( nIdx == 5)
	{
		CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
		if( (pMissionMng->getStageType() == STAGE_TYPE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
			nIdx = 4;	// 폭탄인 경우 설치전이 아니라면 바꿀 수 없습니다.
	}

	//	다음 번호의 무기로 교체
	while( !ACTION_SwapWeapon( nIdx))
	{
		if( nIdx > 0)
			nIdx--;
		else
			nIdx = MAX_ITEMSLOT_COUNT - 1;

		if( nIdx == getWeaponContext()->getCurrentSlotNum())	//	한바퀴 돌면( 바꿀 무기가 없다면)
			return FALSE;
	}

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_SwapOldWeapon( void)
{
	if( getWeaponContext()->getCurrentSlotNum() != getWeaponContext()->getOldSlotNum())
	{
		//	바로 이전 무기로 교체
		return ACTION_SwapWeapon( getWeaponContext()->getOldSlotNum());
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_LoadMagazine( void)
{
	REAL32 rTimeScale = 1.f;
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	if( pWeaponInfo == NULL )
		return FALSE;

	if( pWeapon->IsLoadMagazineLeftGrip() )
	{
		ChangeWeaponAttachHand( CHARA_WEAPON_ATTACH_LEFT_HAND);
		pWeapon->ChangeGunDummy( WEAPON_ATTACH_LEFT_GRIP);
	}
	
	{	
		INT32 nID = m_pAnimContext->getAIUpperID( getAnimSet(), pWeaponInfo->m_TypeClass, pWeaponInfo->m_nNumber, getActionContext()->getBody2(), ID_UPPER_LOAD_MAGAZINE);
		i3AIState * pNewAIState = m_pAnimContext->getAIUpper()->getAIState( nID );

		if( pNewAIState != NULL && pWeaponInfo->m_rReloadTime > I3_EPS )
			rTimeScale = pNewAIState->getAnimDuration() / pWeaponInfo->m_rReloadTime;
	}
	
	pWeapon->SetVisible( TRUE);

	//	총기 계열만 가능
	if( pWeaponInfo->m_TypeFire == WEAPON_FIRE_RANGE)
	{
		if( isNetwork() )
		{
			ANIM_playUpper( CHARA_UPPER_LOADMAGAZINE, ID_UPPER_LOAD_MAGAZINE, 0.f, rTimeScale );
			pWeapon->NET_LoadMagazine();

			// 관전모드 1인칭 시점일때 스코프 해제
			if( g_pCamera && g_pCamera->IsExactTypeOf( CGameCamera_FPS::GetClassMeta()) )
			{	
				if( isStateMask( CHARA_STATE_ZOOM))
				{
					//_CAMERA_SetIdle();
					m_pCameraContext->SetIdleCamera();
				}
			}
		}
		else
		{
			//	소유 총알이 부족할 경우 장전을 할 수 없다.
			if( pWeapon->getTotalBulletCount() <= 0)	
				return FALSE;

			if( (pWeapon->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) && pWeapon->getLoadedBulletCount() == pWeapon->getWeaponInfo()->m_nLoadBullet )
				return FALSE;

			if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON ))	// 무기 스왑중에는 리로드 금지
				return FALSE;

			ANIM_playUpper( CHARA_UPPER_LOADMAGAZINE, ID_UPPER_LOAD_MAGAZINE, 0.f, rTimeScale);

			pWeapon->LoadMagazine();

			//	견착 또는 스나이퍼 카메라 상태에서는 장전시에 일반 시점 상태로 돌아옵니다.
			if( isStateMask( CHARA_STATE_ZOOM))
			{
				//_CAMERA_SetIdle();
				m_pCameraContext->SetIdleCamera();
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_LoadMagazineDual( void )
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	pWeapon->SetVisible( TRUE);

	if( pWeapon->getWeaponInfo()->m_TypeFire == WEAPON_FIRE_RANGE)
	{
		//	소유 총알이 부족할 경우 장전을 할 수 없다.
		if( pWeapon->getTotalBulletCount() <= 0)	
			return FALSE;
		//	무기 Extension 동작 
		pWeapon->ExtensionActivate( TRUE);

		//	무기 Extension 기능 활성화
		ANIM_playUpper( CHARA_UPPER_EXTENSION_ACTIVATE, ID_UPPER_EXTENSION_ACTIVATE);

		g_pFramework->getChatBox()->PutSystemChatting( "Extension On");

		addStateMask( CHARA_STATE_DISABLE_SHOT);	//	발사 준비중에 발사하지 못하도록. 애니메이션이 끝나면 발사 가능

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_LoadBullet( void)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	// MG는 탄약 장전시에만 왼손 파지
	if( pWeapon->IsLoadBulletLeftGrip() )
	{
		ChangeWeaponAttachHand( CHARA_WEAPON_ATTACH_LEFT_HAND);
		pWeapon->ChangeGunDummy( WEAPON_ATTACH_LEFT_GRIP);
	}

	pWeapon->SetVisible( TRUE);

	//	총기 계열만 가능
	if( pWeapon->getWeaponInfo()->m_TypeFire == WEAPON_FIRE_RANGE)
	{
		if( isNetwork() )
		{	// 2006. 9. 23 by 정순구
			// desc : 네트워크 부분 추가
			ANIM_playUpper( CHARA_UPPER_LOADBULLET, ID_UPPER_LOAD_BULLET);
			pWeapon->NET_LoadBullet();
			
			// 관전모드 1인칭 시점일때 스코프 해제
			if( g_pCamera && g_pCamera->IsExactTypeOf( CGameCamera_FPS::GetClassMeta()) )
			{
				if( isStateMask( CHARA_STATE_ZOOM))
				{
					//_CAMERA_SetIdle();
					m_pCameraContext->SetIdleCamera();
				}
			}
		}
		else
		{
			if( pWeapon->isLoadBulletEnabled())
			{
				ANIM_playUpper( CHARA_UPPER_LOADBULLET, ID_UPPER_LOAD_BULLET);

				pWeapon->LoadBullet();

				//	견착 또는 스나이퍼 카메라 상태에서는 장전시에 일반 시점 상태로 돌아옵니다.
				if( isStateMask( CHARA_STATE_ZOOM))
				{
					//_CAMERA_SetIdle();
					m_pCameraContext->SetIdleCamera();
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::ACTION_FallDamage( void)
{
	//	상체가 데미지 동작이 가능한 상태이면
	ACTION_Damage( CHARA_HIT_ROOT, i3Matrix::GetAt( GetMatrix()));
	//	데미지 동작을 안할때도 비명 소리는 나야 한다.
	m_pSoundContext->PlayCharaDamageSound( CHARA_HIT_BODY1, TRUE);

	//	Post Effect
	if( is1PV() )
	{
		CWeaponGlobal * pWeaponGlobal = g_pWeaponInfoDataBase->getGlobal();

		//	Screen Effect 
		EFFECT_Damage( 1.0f, 
					   &pWeaponGlobal->m_DamageEffectColor, 
					   pWeaponGlobal->m_DamageEffectBlurStart,
					   pWeaponGlobal->m_DamageEffectBlurEnd);
	}

	//	Damage Direction
	if( !isNetwork() )
	{
		ShowDamageHUD( this->GetPos(), this->GetPos(), I3_PI);
	}
}

BOOL CGameCharaPlayer::ACTION_DropEquip( EQUIP_CLASS_TYPE type, i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos )
{	
	//	캐릭터 장비를 떼어낸다
	CGameCharaEquip * pCharaEquip = m_pEquipContext->getControl()->OnDetach( type); 

	//	장착된 장비가 있다면
	if( pCharaEquip != NULL)
	{	
		pCharaEquip->SetVisible( TRUE);

		//	새로운 부모에 붙는다.
		pCharaEquip->Attach( pNewSceneNode, pNewOwner, FALSE);
		
		//	탈착 위치 지정
		if( pPos != NULL )
		{
			pCharaEquip->SetPos( pPos );

			if( is1PV() )
			{
				MATRIX mat;
				i3Skeleton *pSkel = pCharaEquip->GetSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
				for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
				{
					i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
					if( pShapeSet == NULL ) continue;

					pShapeSet->getShapeGlobalPose( &mat );
					i3Matrix::SetPos( &mat, pPos );
					pShapeSet->setShapeGlobalPose( &mat );
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_ReturnToReady( BOOL bWithCurrentWeapon)
{	
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	BOOL ret = FALSE;

	//	Idle 상태로 돌아간다.
	if( bWithCurrentWeapon)
		pWeapon->ReturnToReady();

	//	상체만 이전 상태로 돌아간다.	
	ret = ANIM_playUpperReturn( !is1PV() );

	return ret;
}

BOOL CGameCharaPlayer::ACTION_DropWeapon( INT32 nSlot, VEC3D * pDropPos)
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);
	if( pWeapon != NULL)
	{
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);

		//	무기을 월드에 떨군다.
		getWeaponContext()->getControl()->OnWeaponDropToWorld( nSlot, pDropPos);

		//	예외 처리
		switch( pWeaponInfo->m_TypeClass)
		{
		case WEAPON_CLASS_MISSION:	
			{
				switch( pWeaponInfo->m_nNumber)
				{
					case WEAPON_NUM_MISSION_C4:
						{
							removeEnableStateMask( CHARA_STATE_DISABLE_MOVE);
							removeEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

							((WeaponC4*)pWeapon)->m_bEnableActive = TRUE;

							//	주무기로 바로 교체
							CommandParameter * pParam = getCommandContext()->getParameter();
							pParam->_nSlotIdx = 0;
							//if( !ACTION_SwapWeapon( 0) )
							if( !getCommandContext()->Command( CHARA_CMD_SWAPWEAPON))
							{
								//ACTION_SwapOldWeapon();

								if( getWeaponContext()->getCurrentSlotNum() != getWeaponContext()->getOldSlotNum())
								{
									//	바로 이전 무기로 교체
									pParam->_nSlotIdx = getWeaponContext()->getOldSlotNum();
									getCommandContext()->Command( CHARA_CMD_SWAPWEAPON);
								}
							}
						}
						break;
				}
			}
			break;
		default:
			{
				//	다음 무기로 바로 교체
				//ACTION_SwapNextWeapon();	

				CommandParameter * pParam = getCommandContext()->getParameter();
				pParam->_nSlotIdx = getWeaponContext()->getNextUsableWeaponSlot();

				if(pParam->_nSlotIdx != -1)
				{
					getCommandContext()->Command( CHARA_CMD_SWAPWEAPON);
				}
			}
			break;
		}			
		
		return TRUE;
	}

	return FALSE;
}


void CGameCharaPlayer::ACTION_TurnBack( void)
{
	REAL32 rTheta = m_pBoneContext->getTheta();
	rTheta		+= I3_PI; 
	m_pBoneContext->setTheta( i3Math::NormalizeAngle( rTheta));

	m_pMoveContext->setVelocity( 0.0f, 0.0f, 0.0f);
	m_pMoveContext->setAccel( 0.0f, 0.0f, 0.0f);

	m_pMoveContext->setAccelElapsed( 0.f);

	MATRIX * pCharaMat = GetSceneObject()->GetMatrix();
	i3Matrix::PreRotateAxisDeg( pCharaMat, &I3_YAXIS, 180.0f);
}

BOOL CGameCharaPlayer::ACTION_ActiveMission( WeaponBase * pWeapon)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

#if defined( I3_DEBUG)
	if( g_pConfig->m_bNetworking )
	{
#endif
	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// 폭파모드가 아닌경우
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// 설치전이 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// 공격팀이 아닌경우
		return FALSE;
#if defined( I3_DEBUG)
	}
#endif

	I3ASSERT( pWeapon != NULL);
	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);
	
	CGameMissionManager * pMissionManager = g_pGameContext->getMissionManager();
	
	if( (pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION) && (pWeaponInfo->m_nNumber == WEAPON_NUM_MISSION_C4 ) )
	{		
		if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
			(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
		{	
			switch( pMissionManager->getMissionState() )
			{
			case GAME_MISSION_STATE_INBATTLE :					//	미션에 폭탄이 설치되어 있지 않다면
				if( pMissionManager->IsEnableMy())
				{//	폭탄 설치가 가능하면
					//	이동 불가. 
					i3Vector::Zero( m_pMoveContext->getMoveDir());

					if( !getActionContext()->isLower( CHARA_LOWER_IDLE))
						CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);
						//ACTION_IdleLower();

					//ACTION_Crouch();	// 설치시 강제로 앉는다.
					CALL_ACTION_COMMAND(ACTION_CMD_CROUCH, NULL);		// 설치시 강제로 앉는다.
					getCommandContext()->Command(CHARA_CMD_PRIMATTACK);
					//ACTION_Fire();		// 폭탄 설치!!

					addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
					addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

					addStateMask( CHARA_STATE_ARMINGBOMB);

					g_pGameContext->WriteMyInstallingMission( TRUE );

					return TRUE;
				}
				else
				{
					CStageBattle * pBattle = (CStageBattle*)g_pFramework->GetCurrentStage();
					I3ASSERT( pBattle != NULL);
					pBattle->getHud()->SetMissionNotice( STBL_IDX_BOMB_MISSION_NONAREA);			// 지점에서만 설치할수 있심.
				}
				break;

			case GAME_MISSION_STATE_INSTALLED :					//	이미 설치 됐다면
				{
					removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				}
				
			default :
				{
					//CStageBattle * pBattle = (CStageBattle*)g_pFramework->GetCurrentStage();
					//I3ASSERT( pBattle != NULL);
					//pBattle->getHud()->SetMissionNotice( STBL_IDX_BOMB_MISSION_NONAREA);			// 지점에서만 설치할수 있심.
				}
				
				break;
			}
		}
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_ActiveDisarm( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// 폭파모드가 아닌경우
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// 설치 후가 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// 방어팀이 아닌경우
	{
		return FALSE;
	}
	
	//	폭탄 해체	
	if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
		(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
	{	
		switch( pMissionMng->getMissionState() )
		{
		case GAME_MISSION_STATE_INSTALLED :					//	미션에 폭탄의 설치되어 있다면
			if( pMissionMng->IsEnableMy() && pMissionMng->IsEnableCrossHit() )
			{
				WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4);

				if( pWeapon != NULL)
				{
					I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
					WeaponC4 * pBomb = (WeaponC4 *) pWeapon;

					addStateMask( CHARA_STATE_DISARMINGBOMB);

					pBomb->ActiveDisarm( TRUE);
				}
				else
				{
					removeStateMask( CHARA_STATE_DISARMINGBOMB);
				}
			}
			else
			{
				removeStateMask( CHARA_STATE_DISARMINGBOMB);
			}
			break;
		}
	}

	if( isStateMask( CHARA_STATE_DISARMINGBOMB) )
	{	// 해체중
		//	이동 불가. 
		i3Vector::Zero( m_pMoveContext->getMoveDir());

		if( !getActionContext()->isLower( CHARA_LOWER_IDLE))
			CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);
			//ACTION_IdleLower();

		if( !getActionContext()->isUpper( CHARA_UPPER_IDLE))
			CALL_ACTION_COMMAND(ACTION_CMD_STEADYUPPER, NULL);
			//ACTION_IdleUpper();

		//ACTION_Crouch();	// 해체시 강제로 앉는다.
		CALL_ACTION_COMMAND(ACTION_CMD_CROUCH, NULL);	// 해체시 강제로 앉는다.

		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
		addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->WriteMyUninstallingMission( TRUE );		//	폭탄 해체

		return TRUE;
	}
	else
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->WriteMyUninstallingMission( FALSE );

		//I3TRACE( "-- 폭탄 해체취소...\n");

		return FALSE;
	}
}

BOOL CGameCharaPlayer::ACTION_CancelMission( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// 폭파모드가 아닌경우
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// 설치 전이 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// 공격팀이 아닌경우
		return FALSE;
	
	if( pMissionMng->IsInstalling( g_pGameContext->getMySlotIdx() ) )
	{	//	자신이 폭탄 설치 중이면 취소
		g_pGameContext->WriteMyInstallingMission( FALSE );	//	폭탄 설치 취소
		
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		//if( getActionContext()->isUpper( CHARA_UPPER_FIRE) || getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE))
			//ACTION_ReturnToReady( TRUE);
			getCommandContext()->CmdReturnToReady( TRUE);

		//I3TRACE("-- 폭탄 설치 취소!!\n");

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_CancelDisarm( void)
{	//	폭탄 해체 중이면
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// 폭파모드가 아닌경우
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// 설치 후가 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// 방어팀이 아닌경우
		return FALSE;

	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();

	if( isStateMask( CHARA_STATE_DISARMINGBOMB) )
	{
		//	자신이 폭탄 해제 중이면 취소
		//I3TRACE("isUnInstalling() : %d, WhoUnInstall() : %d\n", pMissionData->IsUninstalling( g_pGameContext->getMySlotIdx() ), pMissionData->WhoUninstall( nMyNetIdx));
		if( pMissionMng->IsUninstalling( mySlotIdx ))
		{			
			g_pGameContext->WriteMyUninstallingMission( FALSE );		//	폭탄 해체 취소

			removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

			removeStateMask( CHARA_STATE_DISARMINGBOMB);

			WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4);

			if( pWeapon != NULL)
			{
				I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
				WeaponC4 * pBomb = ( WeaponC4*)pWeapon;

				pBomb->ActiveDisarm( FALSE);
			}

			//I3TRACE("-- 폭탄 해체 취소!!\n");

			return TRUE;
		}
	}

	return FALSE;
}
*/
