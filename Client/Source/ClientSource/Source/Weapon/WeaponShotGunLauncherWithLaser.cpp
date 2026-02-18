#include "pch.h"
#include "WeaponShotGunLauncherWithLaser.h"
#include "WeaponShotGunLauncher.h"
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
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( WeaponShotGunLauncherWithLaser);//, WeaponShotGunLauncher);

namespace 
{
	bool Is1PVObserver(const CGameCharaBase* pOwner)
	{
		if (!pOwner) return false;

		return pOwner->isCharaStateMask(CHARA_STATE_OBSERVER) && pOwner->is1PV() ? true : false;
	}
}

#define LAZER_SIGHT_THIKNESS	0.5f

WeaponShotGunLauncherWithLaser::WeaponShotGunLauncherWithLaser(void) : m_fElapsedTime(0.f)
	, m_fLocalTime(10.0f), m_bReloaded(false), m_fRandX(0.0f), m_fRandY(0.0f), m_bCalcFirst(false)
	, m_fCalcTime(0.0f)
{
	m_pDotLazer = CDotLazer::new_object();
	I3ASSERT( m_pDotLazer != nullptr);

	i3mem::FillZero( m_vecRandPos, sizeof( m_vecRandPos));
	i3mem::FillZero( m_vecRandTangent, sizeof( m_vecRandTangent));
	i3mem::FillZero( m_vecRandSub, sizeof( m_vecRandSub));
}

WeaponShotGunLauncherWithLaser::~WeaponShotGunLauncherWithLaser(void)
{
	I3_SAFE_RELEASE( m_pDotLazer);
}


void WeaponShotGunLauncherWithLaser::_CreateLazerDot(void)
{
	if( m_pOwner != nullptr)
	{
		if (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			m_pDotLazer->CreateLazerDot();
	}	
}

void WeaponShotGunLauncherWithLaser::_UpdateLazerDot(void)
{
	if( m_pOwner != nullptr)
	{		
		if( (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			&& m_pOwner->GetCurrentCharaWeapon() == this)
		{
			if( GetCurrentExtensionType() == WEAPON::EXT_ZOOM && 
				(isWeaponStateMask(WSM_ZOOM) || m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM)))
			{	//	줌 상태일 경우
				m_pDotLazer->DotStampDisable();
			}
			else
			{
				// 시점이 향하고 있는 목표 지점을 구한다.
				GetShotTargetPos( m_TargetPos);

				m_pDotLazer->UpdateDotStamp();
			}
		}		
		else
		{
			m_pDotLazer->DotStampDisable();
		}
	}
}

void WeaponShotGunLauncherWithLaser::DotStampDisable()
{
	if( m_pOwner != nullptr)
	{
		if( (m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this) ||
			Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->DotStampDisable();
		}
	}	
}

void WeaponShotGunLauncherWithLaser::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponShotGunLauncher::Create( pOwner, Itemid, bCreateInWeapon);

	m_pDotLazer->CreateLazerSight(getSceneObject());

	if( m_pOwner != nullptr)
	{
		if (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponShotGunLauncherWithLaser::Reset( void)
{
	WeaponShotGunLauncher::Reset();

	m_fElapsedTime = 0.0f;
	m_fLocalTime = 10.0f;

	m_fRandX = 0.0f;
	m_fRandY = 0.0f;
	m_bCalcFirst = false;
	m_fCalcTime = 0.0f;	

	if( m_pOwner != nullptr)
	{
		if ((m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			&& m_pOwner->GetCurrentCharaWeapon() == this)
		{
			m_pDotLazer->DotStampDisable();
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponShotGunLauncherWithLaser::OnBindResource( void)
{
	WeaponShotGunLauncher::OnBindResource();
}

void WeaponShotGunLauncherWithLaser::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGunLauncher::OnIdle( pCtx, tm);
}

void WeaponShotGunLauncherWithLaser::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGunLauncher::OnFire( pCtx, tm);
}

bool WeaponShotGunLauncherWithLaser::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bRet = WeaponShotGunLauncher::OnLoadMag( pCtx, tm);

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

void WeaponShotGunLauncherWithLaser::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGunLauncher::OnEndLoadMag( pCtx, tm);

	m_bReloaded = false;
}

void WeaponShotGunLauncherWithLaser::OnUpdate( REAL32 rDeltaSeconds)
{	
	m_fElapsedTime = rDeltaSeconds;

	WeaponShotGunLauncher::OnUpdate( rDeltaSeconds);	
	
	if( m_pOwner != nullptr)
	{
		bool bEnableLaserByOption = g_pEnvSet->m_bEnableLaserSight;

		if( m_pOwner->isLocal())
		{
			if( m_pOwner->GetCurrentCharaWeapon() == this)
			{
				// 강제로 레이저 사이트 off 불가능한 무기는 항상 켜둔다. (예. 커스텀 M4)
				if( isPossibleTurnOffWithDotLaser() == false)	
					bEnableLaserByOption = true;

				if (MyRoomInfoContext::i()->getStage()->IsHudOff())
				{
					// 레이저 사이트 표시하기
					if (isWeaponStateMask(WSM_ZOOM))
					{
						m_pDotLazer->SetEnableDotLaser(true);
						_UpdateLazerDot();

					}
					else
					{
						m_pDotLazer->SetEnableDotLaser(false);
					}
				}
				else
				{
					// 레이저 사이트 표시하기
					if (bEnableLaserByOption || isWeaponStateMask(WSM_ZOOM) ||
						(GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate()))
					{
						m_pDotLazer->SetEnableDotLaser(true);
						_UpdateLazerDot();
					}
					else
					{
						m_pDotLazer->SetEnableDotLaser(false);
					}
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
						if (isWeaponStateMask(WSM_ZOOM) ||
							(GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate()))
						{
							m_pDotLazer->SetEnableDotLaser(true);
							_UpdateLazerDot();
						}
						else
						{
							m_pDotLazer->SetEnableDotLaser(false);
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

void WeaponShotGunLauncherWithLaser::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponShotGunLauncher::AttachNode( pSceneNode, pOwner);
	_CreateLazerDot();
}

void WeaponShotGunLauncherWithLaser::DetachNode( void )
{
	WeaponShotGunLauncher::DetachNode();

	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponShotGunLauncherWithLaser::OnDrop( void)
{
	WeaponShotGunLauncher::OnDrop();
	
	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponShotGunLauncherWithLaser::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
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

void WeaponShotGunLauncherWithLaser::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( m_pOwner == nullptr)
		return;

	if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
	{
		WeaponShotGunLauncher::FireCollision( pStartPos, pTargetPos);
	}
	else
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
}

bool WeaponShotGunLauncherWithLaser::isBeing(VEC2D* pVec)
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

bool WeaponShotGunLauncherWithLaser::isBeing(REAL32 fVal1, REAL32 fVal2)
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

void WeaponShotGunLauncherWithLaser::OnProcessExtension( bool bVal)
{
	// LSS의 경우 독립된 무기 정보가 존재하므로, LSS로 Extension 활성화시 기존에 사용되는 m_pWeaponInfo에 적용하기 위해 m_pExtWeaponInfo 정보로 대체된다.	
	if( m_pExtWeaponInfo != nullptr && (GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER || GetCurrentExtensionType() == WEAPON::EXT_ZOOM))
	{
		if( bVal)
			m_pWeaponInfo = m_pExtWeaponInfo;		//	Extension 무기가 주무기 정보가 된다.
		else
			m_pWeaponInfo = m_pAbsoluteWeaponInfo;	//	메인 주무기 정보로 다시 전환
	}
}


bool WeaponShotGunLauncherWithLaser::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	m_pDotLazer->OnLostDevice(bLostDevice);
	return true;
}

bool WeaponShotGunLauncherWithLaser::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	m_pDotLazer->OnRevive(pCtx);
	return true;
}

void WeaponShotGunLauncherWithLaser::ReturnInstance( void)
{
	//	노드 제거 (렌더링과 게임노드에서 제거된다.)
	WeaponShotGunLauncher::ReturnInstance();

	m_pDotLazer->ReturnInstance();
}
