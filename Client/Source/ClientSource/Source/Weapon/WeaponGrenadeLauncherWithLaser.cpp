#include "pch.h"
#include "WeaponGrenadeLauncherWithLaser.h"
#include "WeaponGrenadeLauncher.h"
#include "GlobalVariables.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "DotLazer.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( WeaponGrenadeLauncherWithLaser);//, WeaponGrenadeLauncher);

namespace
{
	bool Is1PVObserver(const CGameCharaBase* pOwner)
	{
		if (!pOwner) return false;

		return pOwner->isCharaStateMask(CHARA_STATE_OBSERVER) && pOwner->is1PV() ? true : false;
	}

}

#define LAZER_SIGHT_THIKNESS	0.5f

WeaponGrenadeLauncherWithLaser::WeaponGrenadeLauncherWithLaser(void)
{
	m_fElapsedTime = 0.0f;
	m_fLocalTime = 10.0f;
	
	m_bReloaded = false;

	m_fRandX = 0.0f;
	m_fRandY = 0.0f;
	m_bCalcFirst = false;
	m_fCalcTime = 0.0f;

	m_pDotLazer = CDotLazer::new_object();

	INT32 i;

	for( i = 0; i < 3; i++)
		i3Vector::Zero( &m_vecRandPos[ i]);

	for( i = 0; i < 2; i++)
		i3Vector::Zero( &m_vecRandSub[ i]);

	for( i = 0; i < 2; i++)
		i3Vector::Zero( &m_vecRandTangent[ i]);
}

WeaponGrenadeLauncherWithLaser::~WeaponGrenadeLauncherWithLaser(void)
{
	I3_SAFE_RELEASE(m_pDotLazer);
}

void WeaponGrenadeLauncherWithLaser::_CreateLazerDot(void)
{
	if( m_pOwner != nullptr)
	{
		if (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			m_pDotLazer->CreateLazerDot();
	}	
}

void WeaponGrenadeLauncherWithLaser::_UpdateLazerDot( bool bEnableByOption)
{
	if( m_pOwner != nullptr)
	{
		if( GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER && isExtensionActivate())
		{	//	유탄발사 모드이고 발사기가 활성화 되어 있으면 끈다.
			m_pDotLazer->DotStampDisable();
		}
		else if ((m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			&& m_pOwner->GetCurrentCharaWeapon() == this &&
			(bEnableByOption || isWeaponStateMask(WSM_ZOOM)))
		{
			// 시점이 향하고 있는 목표 지점을 구한다.
			GetShotTargetPos( m_TargetPos);

			m_pDotLazer->UpdateDotStamp();
		}		
		else
		{
			m_pDotLazer->DotStampDisable();
		}
	}
}

void WeaponGrenadeLauncherWithLaser::DotStampDisable()
{
	if( m_pOwner != nullptr)
	{
		if( m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this)
		{
			m_pDotLazer->DotStampDisable();
		}
	}	
}

void WeaponGrenadeLauncherWithLaser::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponGrenadeLauncher::Create( pOwner, Itemid, bCreateInWeapon);

	m_pDotLazer->CreateLazerSight(getSceneObject());

	if( m_pOwner != nullptr)
	{
		if( m_pOwner->isLocal()|| Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponGrenadeLauncherWithLaser::Reset( void)
{
	WeaponGrenadeLauncher::Reset();

	m_fElapsedTime = 0.0f;
	m_fLocalTime = 10.0f;

	m_fRandX = 0.0f;
	m_fRandY = 0.0f;
	m_bCalcFirst = false;
	m_fCalcTime = 0.0f;	

	if( m_pOwner != nullptr)
	{
		if( (m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this)
			|| Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->DotStampDisable();
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponGrenadeLauncherWithLaser::OnBindResource( void)
{
	WeaponGrenadeLauncher::OnBindResource();
}

void WeaponGrenadeLauncherWithLaser::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenadeLauncher::OnIdle( pCtx, tm);
}

void WeaponGrenadeLauncherWithLaser::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenadeLauncher::OnFire( pCtx, tm);
}

bool WeaponGrenadeLauncherWithLaser::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bRet = WeaponGrenadeLauncher::OnLoadMag( pCtx, tm);

	if( pCtx->getLocalTime() == 0.0f)
	{
		m_bReloaded = true;
	}
	else if( bRet)
	{
		m_bReloaded = false;
	}

	return true;
}

void WeaponGrenadeLauncherWithLaser::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenadeLauncher::OnEndLoadMag( pCtx, tm);

	m_bReloaded = false;
}

void WeaponGrenadeLauncherWithLaser::OnUpdate( REAL32 rDeltaSeconds)
{	
	m_fElapsedTime = rDeltaSeconds;

	WeaponGrenadeLauncher::OnUpdate( rDeltaSeconds);	
	
	if( m_pOwner != nullptr)
	{
		static const bool	bEnableLaserByOption = true;		// 확실하게 해두기 위해 static const로 박아둡니다..16.08.01.수빈

		if( m_pOwner->isLocal())
		{
			if( m_pOwner->GetCurrentCharaWeapon() == this)
			{
				// bool bEnableLaserByOption = (g_pEnvSet->m_bEnableLaserSight==true);
				// 강제로 레이저 사이트 off 불가능한 무기는 항상 켜둔다. (예. 커스텀 M4)
				//if( isPossibleTurnOffWithDotLaser() == false)	// 주석처리. WeaponGrenadeLauncherWithLaser 종류는 항상 레이저가 켜져 있어야 합니다. 140507 Gyungju.kim
				//	bEnableLaserByOption = true;

				if (MyRoomInfoContext::i()->getStage()->IsHudOff())
				{
					// 레이저 사이트 표시하기
					if (isWeaponStateMask(WSM_ZOOM))
					{
						if (_isInvisibleLazerScopeType() == true)
						{
							m_pDotLazer->SetEnableDotLaser(false);
						}
						else
						{
							m_pDotLazer->SetEnableDotLaser(true);
							_UpdateLazerDot(bEnableLaserByOption);
						}

					}
					else
					{
						m_pDotLazer->SetEnableDotLaser(false);
					}
				}
				else
				{
					// bEnableLaserByOption에 대한 커버리티딴지에 의해 관련 조건분기는 모조리 주석처리합니다.  (16.08.01.수빈)
					// 강제로 레이저 사이트 off 불가능한 무기는 항상 켜둔다. (예. 커스텀 M4)
//					if (isPossibleTurnOffWithDotLaser() == false)
//						bEnableLaserByOption = true;

					// 레이저 사이트 표시하기
//					if (bEnableLaserByOption || isWeaponStateMask(WSM_ZOOM))
					{
						if (_isInvisibleLazerScopeType() == true)
						{
							m_pDotLazer->SetEnableDotLaser(false);
						}
						else
						{
							m_pDotLazer->SetEnableDotLaser(true);
							_UpdateLazerDot(bEnableLaserByOption);
						}
					}
//					else
//					{
//						m_pDotLazer->SetEnableDotLaser(false);
//					}
				}

			}
			else
			{
				m_pDotLazer->SetEnableDotLaser(false);
			}
		}
		else
		{
			if( m_pOwner->GetCurrentCharaWeapon() == this)
			{
				m_pDotLazer->SetEnableLaserBar(true);
			}	
			if (CGameCameraManager::i())
			{
				CGameCharaBase* pChara = CGameCameraManager::i()->getTargetCharacter();
				if (pChara && pChara == m_pOwner)
				{
					//옵져버 1인칭상태에서 발동!!
					if (Is1PVObserver(m_pOwner))
					{
						if (_isInvisibleLazerScopeType() == true)
						{
							m_pDotLazer->SetEnableDotLaser(false);
						}
						else
						{
							m_pDotLazer->SetEnableDotLaser(true);
							_UpdateLazerDot(true);
						}
					}

					if (UserInfoContext::i()->IsGM_Observer() && m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM))
					{
						m_pDotLazer->DotStampDisable();
					}
				}
			}
		}
	}
	else
	{
		// 표시하지 않기
		m_pDotLazer->SetEnableDotLaser(false);
	}	
}

void WeaponGrenadeLauncherWithLaser::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponGrenadeLauncher::AttachNode( pSceneNode, pOwner);

	_CreateLazerDot();
}

void WeaponGrenadeLauncherWithLaser::DetachNode( void )
{
	WeaponGrenadeLauncher::DetachNode();

	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponGrenadeLauncherWithLaser::OnDrop( void)
{
	WeaponGrenadeLauncher::OnDrop();

	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponGrenadeLauncherWithLaser::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
{
	if( m_pOwner == nullptr)
		return;

	if( (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
		&& m_pOwner->GetCurrentCharaWeapon() == this)
	{
		REAL32	fRange = 400.f;

		if( m_pWeaponInfo->GetTypeFire() == WEAPON::FIRE_MELEE )
			fRange = 400.f;
		else
			fRange = GetRange();

		m_pDotLazer->GetShotTargetPos(pDest, pStart, m_bReloaded, fRange, m_pOwner, getFXMatrix());
		if(isWeaponStateMask( WSM_ZOOM))
		{
			//m_pDotLazer->DotStampDisable();				
		}
	}
	else
	{
		WeaponBase::GetShotTargetPos( pDest, pStart);			
	}
}

void WeaponGrenadeLauncherWithLaser::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( m_pOwner == nullptr)
		return;

	if( m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this)
	{
		VEC3D target, start;
		INT32 i, collGroup;

		i3Vector::Copy( &target, pTargetPos);

		if( pStartPos == nullptr )
		{
			// 총알의 발사 시작점을 (1인칭의 경우에는) Camera의 Position으로,
			// (3인칭의 경우에는) 총구 지점으로 한다.
			if( m_pOwner->is1PV())
			{
				// 현재는 Camera의 지점
				m_pOwner->getCameraContext()->getVirtualCamPosition( &start );
			}
			else
			{
				i3Vector::Copy( &start, i3Matrix::GetPos( getFXMatrix()));
			}
		}
		else
		{
			i3Vector::Copy( &start, pStartPos );
		}

		collGroup = CGRP_MASKGROUP_ATTACK;
		if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)
			collGroup = CGRP_MASKGROUP_CHARA_BLUE;
		else
			collGroup = CGRP_MASKGROUP_CHARA_RED;

		// 네트워크로 보내줍니다.
		NET_FireSync( &target );

		for( i = 0; i < m_pWeaponInfo->GetFireBulletCount(); i++)
		{
			DoLineCollision( &start, pTargetPos, collGroup);
		}

		INT32 autoFireCount = getAutoFireCount();

		autoFireCount++;
		setAutoFireCount( autoFireCount);
	}
	else
	{
		WeaponGrenadeLauncher::FireCollision( pStartPos, pTargetPos);
	}
}

bool WeaponGrenadeLauncherWithLaser::isBeing(VEC2D* pVec)
{
	bool bRet = false;
	INT32 i = 0;

	for( i=0; i<3; i++)
	{
		if( (i3Vector::GetX( &m_vecRandPos[i]) == i3Vector::GetX( pVec)) &&	(i3Vector::GetY( &m_vecRandPos[i]) == i3Vector::GetY( pVec)) )
		{
			bRet = true;
		}
	}

	return bRet;
}

bool WeaponGrenadeLauncherWithLaser::isBeing(REAL32 fVal1, REAL32 fVal2)
{
	bool bRet = false;
	INT32 i = 0;

	for( i=0; i<3; i++)
	{
		if( (i3Vector::GetX( &m_vecRandPos[i]) == fVal1) &&	(i3Vector::GetY( &m_vecRandPos[i]) == fVal2) )
		{
			bRet = true;
		}
	}

	return bRet;
}

bool WeaponGrenadeLauncherWithLaser::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	m_pDotLazer->OnLostDevice(bLostDevice);
	return true;
}

bool WeaponGrenadeLauncherWithLaser::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	m_pDotLazer->OnRevive(pCtx);
	return true;
}

void WeaponGrenadeLauncherWithLaser::ReturnInstance( void)
{
	//	노드 제거 (렌더링과 게임노드에서 제거된다.)
	WeaponGrenadeLauncher::ReturnInstance();

	m_pDotLazer->ReturnInstance();
}

// scope 중에 lazer sight 가 꺼져야 하는 것들 추가 필요합니다.
bool WeaponGrenadeLauncherWithLaser::_isInvisibleLazerScopeType()
{
	if (isWeaponStateMask(WSM_ZOOM) == false) return false;

	CWeaponInfo * pWeaponInfo = GetCurrentWeaponInfo();
	I3ASSERT(pWeaponInfo != nullptr);

	const SUB_SCOPE_INFO* subscopeinfo = g_pWeaponInfoDataBase->GetSubScopeInfo(pWeaponInfo->GetScopeSubType());
	bool invisible = false;
	if (subscopeinfo != nullptr)
	{
		invisible = subscopeinfo->m_bOffDotLaser;
	}

	return invisible;
}