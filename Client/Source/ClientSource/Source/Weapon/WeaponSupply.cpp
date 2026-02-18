#include "pch.h"

#include "WeaponSupply.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "../UI/HUD/UIHudDomi_Tag.h"
#include "WeaponFunction.h"


I3_CLASS_INSTANCE( WeaponSupply);//, WeaponBase);

WeaponSupply::WeaponSupply() 
	: m_rInstallElapsedTime(0.0f), m_rUninstallElpasedTime(0.0f), m_rExplosionElapsedTime(0.0f)
		, m_rTracerUpdateTime(0.0f), m_bDisarm(false), m_nWarningCount(0), m_bEnableActive(false)
#ifdef DOMI_TAG
		, m_pWeaponTag(nullptr)
#endif
{
	
}

WeaponSupply::~WeaponSupply()
{
#ifdef DOMI_TAG
	I3_SAFE_RELEASE( m_pWeaponTag);
#endif
}

void WeaponSupply::_OnInitVairable()
{	
	WeaponBase::_OnInitVairable();

	m_rInstallElapsedTime	= 0.f;
	m_rUninstallElpasedTime	= 0.f;
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

	m_bDisarm				= false;
	m_bEnableActive			= false;

	i3Matrix::Identity( &m_mtxOld);
}

void WeaponSupply::_InitCollideeGroup( void)
{
	// InitColliderSet( CGRP_OBJECT, CGRP_MASK_TEAM_BLUE | CGRP_MASK_TEAM_RED);
	WF::SetCollideeGroup_Supply(this);
}

void WeaponSupply::Reset()
{
	WeaponBase::Reset();

	SetMatrix( &m_mtxOld);
	m_StateMaskWeapon		= WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE;
	m_rInstallElapsedTime	= 0.f;	
	m_rUninstallElpasedTime	= 0.f;	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;
	m_bDisarm				= false;
	m_bEnableActive			= false;
	m_nWarningCount			= -1;
}

void WeaponSupply::PostCreate( void)
{
	i3Matrix::Identity( &m_mtxOld);
}

void WeaponSupply::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	m_rInstallElapsedTime = 0.f;
}

void WeaponSupply::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFire( pCtx, tm);

	m_rInstallElapsedTime = 0.f;
}

void WeaponSupply::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	m_rInstallElapsedTime = 0.f;
}

void WeaponSupply::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponBase::OnUpdate( rDeltaSeconds);
}

//void WeaponSupply::Fire(void)
//{
//	if( getLoadedBulletCount() <= 0)
//		return;
//
//	NET_Fire( nullptr, nullptr );
//}

void WeaponSupply::FireEnd(void)
{	
	WeaponBase::FireEnd();

	m_rInstallElapsedTime = 0.f;
}

void WeaponSupply::ActiveDisarm( bool bEnable)
{
	if( m_bEnableActive)
	{
		if( bEnable)
		{
			m_bDisarm = true;
		}
		else
		{
			m_bDisarm = false;
			m_rUninstallElpasedTime = 0.f;
		}
	}
}

void WeaponSupply::ReturnToReady( void)
{	
	WeaponBase::ReturnToReady();

	m_rInstallElapsedTime	= 0.f;	
	m_rUninstallElpasedTime	= 0.f;	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;
	m_nWarningCount			= -1;

	
}


void WeaponSupply::Uninstall( void)
{
	m_bEnableActive = false;
	//g_pWeaponManager->PushDropWeaponToChara( this );

}
void WeaponSupply::InstallToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat, REAL32 theta)
{
	VEC3D vWeaponPos = VEC3D( pPos->x, pPos->y, pPos->z);
	
	//if( getOwner() == nullptr )
	//	return;
	
	WEAPON_TAG_TYPE	type = WEP_TAG_NONE;

	i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_YAXIS, -theta);
	i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetUp( GetMatrix()), 0.01f);
	i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetRight( GetMatrix()), 0.02f);	// hight 5cm

	WEAPON::RES_ID num = getWeaponInfo()->GetNumber();
	if (num == GetItemIndex("SUPPLY_HP"))
	{
		type = WEP_TAG_OBJ_HP;
	}
	else if(num == GetItemIndex("SUPPLY_AMMO"))
	{	
		type = WEP_TAG_OBJ_AMMO;
	}	
	else if(num == GetItemIndex("SKILLSHOP"))
	{
		type = WEP_TAG_SKILL_SHOP;
	}
	else
	{
		I3ASSERT_0;
	}

	i3Vector::ScaleAdd( pPos, i3Matrix::GetAt( GetMatrix()), 1.1f);

	I3TRACE( "--- 서플라이베이스 설치 완료 (Pos %.4f %.4f %.4f).\n", pPos->x, pPos->y, pPos->z);

	WeaponBase::Transmission( pNewSceneNode, pNewOwner, &vWeaponPos, true);

	m_rInstallElapsedTime = 0.f;

	if (num < 7)
		WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_FIRE));

	if( SetTagToWeapon( m_pWeaponTag, type ) )	// 표시 아이콘 출력
		getAttachNode()->AddChild( m_pWeaponTag );
}

void WeaponSupply::ReturnInstance( void)
{	
	m_bEnableActive = false;
	Reset();
	WeaponBase::ReturnInstance();
}

void WeaponSupply::_OnCheckCollisionByExplosion(void)
{
}

void WeaponSupply::_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult)
{	
	_OnHit( pResult, pLine, false);
}

