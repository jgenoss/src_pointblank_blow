#include "pch.h"
#include "WeaponMachineGun.h"
#include "GlobalVariables.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaMoveContext.h"
#include "./Character/GameCharaControlBase.h"
#include "WeaponFunction.h"

#define MACHINGUN_BULLETBONE_COUNT	12

I3_CLASS_INSTANCE( WeaponMachineGun);//, WeaponBase);

WeaponMachineGun::WeaponMachineGun()
{
	m_aiiFire1 = -1;
	m_aiiFire2 = -1;
	m_aiiFire3 = -1;
	m_aiiFire4 = -1;
	
	m_bMachineGunChangeBullet = false;
}

WeaponMachineGun::~WeaponMachineGun()
{
}

void WeaponMachineGun::_OnInitVairable()
{
	WeaponBase::_OnInitVairable();

	m_aiiFire1 = -1;
	m_aiiFire2 = -1;
	m_aiiFire3 = -1;
	m_aiiFire4 = -1;
	
	m_bMachineGunChangeBullet = false;
}

void WeaponMachineGun::_OnPlayWeaponFireAnim( void)
{
	INT32 nRand = i3Math::Rand() % 5;

	switch( nRand)
	{
	case 1:
		if( m_aiiFire1 != -1)
		{
			WF::PlayWeaponAnim(this, m_aiiFire1 );
			return;
		}
		break;
	case 2:
		if( m_aiiFire2 != -1)
		{
			WF::PlayWeaponAnim(this, m_aiiFire2 );
			return;
		}
		break;
	case 3:
		if( m_aiiFire3 != -1)
		{
			WF::PlayWeaponAnim(this, m_aiiFire3 );
			return;
		}
		break;
	case 4:
		if( m_aiiFire4 != -1)
		{
			WF::PlayWeaponAnim(this, m_aiiFire4 );
			return;
		}
		break;
	}
		
	WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
}

void WeaponMachineGun::Reset( void)
{
	WeaponBase::Reset();

	m_bMachineGunChangeBullet = false;

	for( INT32 i = 1; i < MACHINGUN_BULLETBONE_COUNT; i++)
	{
		char szBoneName[ 8];
		if( i < 10)
		{
			sprintf_s( szBoneName, "Bone0%d", i);
		}
		else
		{
			sprintf_s( szBoneName, "Bone%d", i);
		}
		i3Node * pNode = i3Scene::FindNodeByName( getSceneObject(), szBoneName);
		if( pNode != nullptr)
		{
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
		}
	}
}

void WeaponMachineGun::OnBindResource( void)
{
	WeaponBase::OnBindResource();

	m_aiiFire1 = (INT8) m_pAICtx->FindAIStateByName( "Fire1");
	m_aiiFire2 = (INT8) m_pAICtx->FindAIStateByName( "Fire2");
	m_aiiFire3 = (INT8) m_pAICtx->FindAIStateByName( "Fire3");
	m_aiiFire4 = (INT8) m_pAICtx->FindAIStateByName( "Fire4");
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Animation callback
void WeaponMachineGun::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnIdle( pCtx, tm);

	if( m_pOwner == nullptr )	return;

	CGameCharaActionContext * pActionCtx = m_pOwner->getActionContext();

	if( pActionCtx->isUpper( CHARA_UPPER_MOVE))
	{
		if( getAIIndex( WEAPON::AI_MOVE) != -1 &&
			getAIIndex( WEAPON::AI_CURRENT) != getAIIndex( WEAPON::AI_MOVE) )
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_MOVE));
	}
	else if( pActionCtx->isUpper( CHARA_UPPER_IDLE))
	{
		if( getAIIndex( WEAPON::AI_IDLE) != -1 &&
			getAIIndex( WEAPON::AI_CURRENT) == getAIIndex( WEAPON::AI_MOVE) )
			_OnPlayWeaponIdleAnim();
	}
}

INT32 WeaponMachineGun::_OnProcessFireBulletCount( void)
{
	INT32 iBullet = WeaponBase::_OnProcessFireBulletCount();

	if( ( iBullet < MACHINGUN_BULLETBONE_COUNT + 1) && ( iBullet > 0) )
	{
		char szBoneName[ 8];
		if( ( iBullet < MACHINGUN_BULLETBONE_COUNT-1))
		{
			sprintf_s( szBoneName, "Bone0%d", iBullet - 1);
		}
		else
		{
			sprintf_s( szBoneName, "Bone%d", iBullet - 1);
		}

		i3Node * pNode = i3Scene::FindNodeByName( getSceneObject(), szBoneName);
		if( pNode != nullptr)
		{
			pNode->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}

	return iBullet;
}

bool WeaponMachineGun::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bRv = WeaponBase::OnLoadMag( pCtx, tm);

	if( !m_bMachineGunChangeBullet)
	{	// 탄티가 안보이게 설정되어 잇는 경우 보이도록 세팅해 줘야 합니다.
		if( pCtx->getLocalTime() > m_pWeaponInfo->GetReloadTime() * 0.5f)
		{
			for( INT32 i = 1; i < 12; i++)
			{
				char szBoneName[ 8];
				if( i < 10)
				{
					sprintf_s( szBoneName, "Bone0%d", i);
				}
				else
				{
					sprintf_s( szBoneName, "Bone%d", i);
				}

				i3Node * pNode = i3Scene::FindNodeByName( getSceneObject(), szBoneName);
				if( pNode != nullptr)
				{
					pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
				}
			}
			m_bMachineGunChangeBullet = true;
		}
	}

	if( bRv )
	{
		setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
		removeWeaponStateMask( WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE);
		addWeaponStateMask( WSM_WAIT_FOR_LOADMAG);
		m_bMachineGunChangeBullet = false;
	}

	return bRv;
}

void WeaponMachineGun::_OnProcessFireMiddleAnim( REAL32 rDelay)
{
	if( !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && (m_idxBulletEffect != -1) && (m_idxFXBullet != -1) )
	{
		_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);
		_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
	}
}

void WeaponMachineGun::_OnProcessFireEndAnim( REAL32 rDelay)
{
	if( isWeaponStateMask( WSM_FIRING ) ||
		rDelay >= m_pOwner->getAnimContext()->getUpperDuration() )
	{
		_OnPlayWeaponIdleAnim();

		if( m_pWeaponInfo->GetAutoFireCount() == 0)
		{	// 연사, 또는 점사
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}
	}
	else
	{
		if( getLoadedBulletCount() <= 0)
			_OnPlayWeaponIdleAnim();
		else
		{
			removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
			setWeaponState( WEAPON::STATE_FIRING);
		}
	}
}


