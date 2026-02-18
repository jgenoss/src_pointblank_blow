#include "pch.h"
#include "WeaponDotLaser.h"
#include "GlobalVariables.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "DotLazer.h"
#include "../MyRoomInfoContext.h"
#include "../UI/UIUtil.h"
#include "../UI/UIHudManager.h"
#include "../UI/UIHudMultiWeapon.h"
#include "../UtilBattle.h"
#include "../BattleSlotContext.h"
#include "MyRoomInfoContext.h"
#include "UserInfoContext.h"
#include "CrosshairMgr.h"
#include "Crosshair.h"

I3_CLASS_INSTANCE( WeaponDotLaser);//, WeaponBase);

namespace
{
	bool Is1PVObserver(const CGameCharaBase* pOwner)
	{
		if (!pOwner) return false;

		return pOwner->isCharaStateMask(CHARA_STATE_OBSERVER) && pOwner->is1PV() ? true : false;
	}
}

#define LAZER_SIGHT_THIKNESS	0.5f

WeaponDotLaser::WeaponDotLaser(void)
{
	m_fElapsedTime = 0.0f;
	m_fLocalTime = 10.0f;

	m_bReloaded = false;

	m_fRandX = 0.0f;
	m_fRandY = 0.0f;
	m_bCalcFirst = false;
	m_fCalcTime = 0.0f;

	m_pDotLazer = CDotLazer::new_object();

	i3mem::FillZero( m_vecRandPos, sizeof( m_vecRandPos));
	i3mem::FillZero( m_vecRandSub, sizeof( m_vecRandSub));
	i3mem::FillZero( m_vecRandTangent, sizeof( m_vecRandTangent));
}

WeaponDotLaser::~WeaponDotLaser(void)
{
	I3_SAFE_RELEASE(m_pDotLazer);
}

void WeaponDotLaser::_CreateLazerDot(void)
{
	if( m_pOwner != nullptr)
	{
		if (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->CreateLazerDot();
		}
	}	
}

void WeaponDotLaser::_UpdateLazerDot( bool bEnableByOption)
{
	if( m_pOwner != nullptr)
	{	
		const SUB_SCOPE_INFO* subscopeinfo = g_pWeaponInfoDataBase->GetSubScopeInfo(getWeaponInfo()->GetScopeSubType());
		bool bOnDotLaser = true;
		if (subscopeinfo != nullptr)
		{
			bOnDotLaser = !subscopeinfo->m_bOffDotLaser;
		}

		if( (!bOnDotLaser || GetCurrentExtensionType() == WEAPON::EXT_ZOOM) &&
			(isWeaponStateMask( WSM_ZOOM) || m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM)))
		{
			m_pDotLazer->DotStampDisable();		// 저격 스코프에서 줌 상태일때는 DotStamp 안보이게
		}
		else if ((m_pOwner->isLocal() || Is1PVObserver(m_pOwner)) && i3::kind_of<WeaponDotLaser*>(m_pOwner->GetCurrentCharaWeapon()) &&
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

void WeaponDotLaser::DotStampDisable()
{
	if( m_pOwner != nullptr)
	{
		if( m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this)
		{
			m_pDotLazer->DotStampDisable();
		}
	}	
}

void WeaponDotLaser::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create( pOwner, Itemid, bCreateInWeapon);
	
	m_pDotLazer->CreateLazerSight(getSceneObject());

	if( m_pOwner != nullptr)
	{
		if (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			m_pDotLazer->CreateLazerDot();
		
	}

}

void WeaponDotLaser::Reset( void)
{
	WeaponBase::Reset();

	m_fElapsedTime = 0.0f;
	m_fLocalTime = 10.0f;

	m_fRandX = 0.0f;
	m_fRandY = 0.0f;
	m_bCalcFirst = false;
	m_fCalcTime = 0.0f;	

	if( m_pOwner != nullptr)
	{
		if( (m_pOwner->isLocal() || Is1PVObserver(m_pOwner)) && m_pOwner->GetCurrentCharaWeapon() == this)
		{
			m_pDotLazer->DotStampDisable();
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponDotLaser::OnBindResource( void)
{
	WeaponBase::OnBindResource();
}

void WeaponDotLaser::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnIdle( pCtx, tm);
}

void WeaponDotLaser::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnFire( pCtx, tm);
}

bool WeaponDotLaser::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bRet = WeaponBase::OnLoadMag( pCtx, tm);

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

void WeaponDotLaser::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndLoadMag( pCtx, tm);

	m_bReloaded = false;
}

namespace
{
	bool IsObserveState()
	{
		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION))
		{
			if (ub::is_live_character(BattleSlotContext::i()->getMySlotIdx()) == false)
				return true;
		}

		return false;
	}
}

void WeaponDotLaser::OnUpdate( REAL32 rDeltaSeconds)
{	
	m_fElapsedTime = rDeltaSeconds;

	WeaponBase::OnUpdate( rDeltaSeconds);	
	
	if( m_pOwner != nullptr)
	{
		bool bEnableLaserByOption = (g_pEnvSet->m_bEnableLaserSight == TRUE);

		if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH)) return;

		if( m_pOwner->isLocal())
		{
			if( i3::kind_of<WeaponDotLaser*>(m_pOwner->GetCurrentCharaWeapon()))
			{
				BOOL bMultiWeapon = TRUE;

				if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				{
					UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

					if ( pHud->IsVisible() && !pHud->GetAutoView())
					{
						bMultiWeapon = FALSE;
					}
				}

				// 키처리
				if( g_pFramework->getKeyStroke() & GAME_KEY_ACTION_WEAPON_EXTENSION && isPossibleTurnOffWithDotLaser() && bMultiWeapon)
				{
					g_pEnvSet->m_bEnableLaserSight = !g_pEnvSet->m_bEnableLaserSight;	
					m_pDotLazer->SetEnableDotLaser( g_pEnvSet->m_bEnableLaserSight?true:false);

					m_pOwner->Cmd_ApplyCrosshair();

					
					if (IsObserveState() == false)
					{
						if( g_pEnvSet->m_bEnableLaserSight)
						{
							g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STBL_IDX_DOTLAZER_ACTIVATE"));//{col:255,120,0,255}레이져사이트가 활성화 되었습니다.{/col}
						}
						else
						{
							g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STBL_IDX_DOTLAZER_DEACTIVATE"));//{col:255,120,0,255}레이져사이트가 비활성화 되었습니다.{/col}
						}

						g_pFramework->getChatBox()->ResetBackupBuffer();
					}
				}
							
				if ( MyRoomInfoContext::i()->getStage()->IsHudOff() == true)
				{
					// 레이저 사이트 표시하기
					if (isWeaponStateMask(WSM_ZOOM))
					{
						m_pDotLazer->SetEnableDotLaser(true);
						_UpdateLazerDot(bEnableLaserByOption);

					}
					else
					{
						m_pDotLazer->SetEnableDotLaser(false);
					}
				}
				else
				{
					// 강제로 레이저 사이트 off 불가능한 무기는 항상 켜둔다. (예. 커스텀 M4)
					if (isPossibleTurnOffWithDotLaser() == false)
						bEnableLaserByOption = true;

					// 레이저 사이트 표시하기
					if (bEnableLaserByOption || isWeaponStateMask(WSM_ZOOM))
					{
						m_pDotLazer->SetEnableDotLaser(true);
						_UpdateLazerDot(bEnableLaserByOption);

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
						// 레이저 사이트 표시하기
						m_pDotLazer->SetEnableDotLaser(true);
						_UpdateLazerDot(true);
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

void WeaponDotLaser::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponBase::AttachNode( pSceneNode, pOwner);

	_CreateLazerDot();
}

void WeaponDotLaser::DetachNode( void )
{
	WeaponBase::DetachNode();

	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponDotLaser::OnDrop( void)
{
	WeaponBase::OnDrop();

	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponDotLaser::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
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

void WeaponDotLaser::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
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
		
		autoFireCount ++;
		setAutoFireCount( autoFireCount);
	}
	else
	{
		WeaponBase::FireCollision( pStartPos, pTargetPos);
	}
}

bool WeaponDotLaser::isBeing(VEC2D* pVec)
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

bool WeaponDotLaser::isBeing(REAL32 fVal1, REAL32 fVal2)
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

bool WeaponDotLaser::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	m_pDotLazer->OnLostDevice(bLostDevice);
	return true;
}

bool WeaponDotLaser::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	m_pDotLazer->OnRevive(pCtx);
	return true;
}

void WeaponDotLaser::ReturnInstance( void)
{
	//	노드 제거 (렌더링과 게임노드에서 제거된다.)
	WeaponBase::ReturnInstance();

	m_pDotLazer->ReturnInstance();
}
