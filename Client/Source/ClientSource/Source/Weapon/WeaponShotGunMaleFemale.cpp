#include "pch.h"
#include "WeaponShotGunMaleFemale.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"
#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponShotGunMaleFemale);//, WeaponShotGun);

WeaponShotGunMaleFemale::WeaponShotGunMaleFemale()
{
	m_bFirstLoadMag =0;
	m_iLoadableBulletCount = -1;
	m_iLoadedBulletCount = -1;

	m_aii_LoadMag_Ready_Male 	= -1;
	m_aii_LoadMag_Ready_Female	= -1;

	m_iLoadedBulletCount = -1;
	m_iLoadableBulletCount= -1;

	m_aii_LoadMag_Ready_Male= -1; 
	m_aii_LoadMag_Ready_Female= -1;

	m_aii_LoadMag_Male= -1;		
	m_aii_LoadMag_Female= -1;	

	m_aii_LoadMag2_Male= -1;		
	m_aii_LoadMag2_Female= -1;	

	m_aii_LoadBullet_Male= -1;	
	m_aii_LoadBullet_Female= -1;	

	m_LoadMagChg = false;

}

WeaponShotGunMaleFemale::~WeaponShotGunMaleFemale()
{
}


void WeaponShotGunMaleFemale::_OnInitVairable()
{
	INT32 i, j;

	WeaponShotGun::_OnInitVairable();

	m_aii_LoadMag_Ready_Male 	= -1;
	m_aii_LoadMag_Ready_Female	= -1;

	m_iLoadedBulletCount = -1;
	m_iLoadableBulletCount= -1;

	m_aii_LoadMag_Ready_Male= -1; 
	m_aii_LoadMag_Ready_Female= -1;

	m_aii_LoadMag_Male= -1;		
	m_aii_LoadMag_Female= -1;	

	m_aii_LoadMag2_Male= -1;		
	m_aii_LoadMag2_Female= -1;	

	m_aii_LoadBullet_Male= -1;	
	m_aii_LoadBullet_Female= -1;	

	m_LoadMagChg = false;

	for( i = 0; i < BULLETCOUNT; i++)
	{
		for( j = 0; j < 3; j++)
			m_pBullet[ i][ j] = nullptr;
	}

	m_iLoadableBulletCount	= 0;
	m_bFirstLoadMag			= false;
}

void WeaponShotGunMaleFemale::_OnProcessFireMiddleAnim( REAL32 rDelay)
{
	I3ASSERT( m_pOwner != nullptr);

	if( !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && (m_idxBulletEffect != -1) && (m_idxFXBullet != -1) )
	{
		if( rDelay > 0.6f )		
			addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		if( isWeaponStateMask( WSM_EMPTY_CARTRIDGE) )
		{
			MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);

			_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
			I3ASSERT( m_pOwner->getMoveContext() != nullptr);
			_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
		}
	}
}

void WeaponShotGunMaleFemale::OnBindResource()
{
	WeaponBase::OnBindResource();
	setAIIndex( WEAPON::AI_FIREMALE, (INT8) m_pAICtx->FindAIStateByName( "Fire_Male"));
	setAIIndex( WEAPON::AI_FIREFEMALE, (INT8) m_pAICtx->FindAIStateByName( "Fire_Female"));
	
	setAIIndex( WEAPON::AI_LOADBULLET_LEFT_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMagB_Male"));
	setAIIndex( WEAPON::AI_LOADBULLET_RIGHT_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMagC_Male"));

	setAIIndex( WEAPON::AI_LOADBULLET_LEFT_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMagB_Female"));
	setAIIndex( WEAPON::AI_LOADBULLET_RIGHT_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMagC_Female"));

	m_aii_LoadMag_Ready_Male = (INT8) m_pAICtx->FindAIStateByName( "LoadMag_Ready_Male");
	m_aii_LoadMag_Ready_Female = (INT8) m_pAICtx->FindAIStateByName( "LoadMag_Ready_Female");

	m_aii_LoadMag_Male		= (INT8) m_pAICtx->FindAIStateByName( "LoadMagB_Male");
	m_aii_LoadMag_Female	= (INT8) m_pAICtx->FindAIStateByName( "LoadMagB_Female");

	m_aii_LoadMag2_Male		= (INT8) m_pAICtx->FindAIStateByName( "LoadMagC_Male");
	m_aii_LoadMag2_Female	= (INT8) m_pAICtx->FindAIStateByName( "LoadMagC_Female");

	m_aii_LoadBullet_Male	= (INT8) m_pAICtx->FindAIStateByName( "LoadMagD_Male");
	m_aii_LoadBullet_Female	= (INT8) m_pAICtx->FindAIStateByName( "LoadMagD_Female");

	INT32 i, j;
	char szTemp[ MAX_STRING_COUNT];
	for( i = 0; i < BULLETCOUNT; i++)
	{
		if( i < 1)	sprintf_s( szTemp, "VolAnimationDummy");
		else sprintf_s( szTemp, "VolAnimationDummy_0%d", i );

		for( j = 0; j < 3; j++)
		{
			if( getSceneObject()->GetBody()->getLOD( j) != nullptr)
			{
				m_pBullet[ i][ j] = i3Scene::FindNodeByName( getSceneObject()->GetBody()->getLOD( j)->getShapeNode(), szTemp);
			}
		}
	}
}

void WeaponShotGunMaleFemale::_VisibleBullet( bool bEnable)
{
	INT32 i, j;

	if( bEnable)
	{
		for( i = 0; i < BULLETCOUNT; i++)
		{
			for( j = 0; j < 3; j++)
			{
				if( m_pBullet[ i][ j] != nullptr)
				{
					m_pBullet[ i][ j]->RemoveFlag( I3_NODEFLAG_INVISIBLE);
				}
			}
		}
	}
	else
	{
		for( i = 0; i < BULLETCOUNT; i++)
		{
			for( j = 0; j < 3; j++)
			{
				if( m_pBullet[ i][ j] != nullptr)
				{
					m_pBullet[ i][ j]->AddFlag( I3_NODEFLAG_INVISIBLE);
				}
			}
		}
	}
}

void WeaponShotGunMaleFemale::OnSetVisibleExtension( bool bVal)
{
	_VisibleBullet( bVal);
}

bool WeaponShotGunMaleFemale::NotMasterLoadMagAnim(INT32 idx)
{
	if( idx == m_aii_LoadMag_Male || idx == m_aii_LoadMag_Female ||
		idx == m_aii_LoadMag2_Male || idx == m_aii_LoadMag2_Female )
	{
		return true;
	}

	return false;
}
void WeaponShotGunMaleFemale::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun::OnIdle( pCtx, tm);

	m_bFirstLoadMag = true;
	removeWeaponStateMask( WSM_READY_TO_LOADMAG);
	m_FireOrder = WEAPON::PRIMARY_FIRE;
}

void WeaponShotGunMaleFemale::OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnLoadMag_Ready( pCtx, tm);

	I3ASSERT( pCtx != nullptr);
	REAL32 localTime = pCtx->getLocalTime();

	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( localTime + tm > pAIState->getDuration() )
	{
		setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
		addWeaponStateMask( WSM_READY_TO_LOADMAG);
	}
}

void WeaponShotGunMaleFemale::LoadMagazine_Ready( void)
{
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()))
	{
		if( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			addWeaponStateMask( WSM_FIRE_DISABLE);

		addWeaponStateMask( WSM_LOADMAG_DISABLE );
	}

	OnPlayLoadMagReadyAnimation();
}

void WeaponShotGunMaleFemale::OnPlayLoadMagReadyAnimation( void)
{
	INT8 idx = m_aii_LoadMag_Ready_Male;

	if( m_pOwner->isFemale() == false)	idx = m_aii_LoadMag_Ready_Male;
	else								idx = m_aii_LoadMag_Ready_Female;

	_PlayWeaponAnimWithApplyTimeScale( idx);
}

void WeaponShotGunMaleFemale::OnPlayLoadMagAnimation( void)
{
	INT8 idx; // = m_aii_LoadBullet_Male;

	if(getOwner()->isLoadMagChg() == true)
	{
		if( m_pOwner->isFemale() == false)	idx = m_aii_LoadMag_Male;
		else								idx = m_aii_LoadMag_Female;
	}
	else
	{
		if( m_pOwner->isFemale() == false)	idx = m_aii_LoadMag2_Male;
		else								idx = m_aii_LoadMag2_Female;
	}

	m_LoadMagChg = !m_LoadMagChg;


	if( _PlayWeaponAnimWithApplyTimeScale( idx) == false)
	{
		_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE) );
	}

	setWeaponState( WEAPON::STATE_LOADMAG);
}

bool WeaponShotGunMaleFemale::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bLoaded = false;

	bLoaded = WeaponShotGun::OnLoadMag( pCtx, tm);

	if( pCtx->getLocalTime() == 0.f && m_bFirstLoadMag)
	{
		m_iLoadedBulletCount = -1;

		m_iLoadableBulletCount = GetLoadBulletWithCashItem() - getLoadedBulletCount();
		if( m_iLoadableBulletCount > BULLETCOUNT)
			m_iLoadableBulletCount = BULLETCOUNT;

		_VisibleBullet( true);
		m_bFirstLoadMag = false;
	}
	else if( pCtx->getLocalTime() > 0.15f)
	{
		if( m_iLoadedBulletCount == -1)
		{
			m_iLoadedBulletCount = 0;
		}
	}

	if( bLoaded && m_iLoadedBulletCount != -1)
	{
		m_iLoadedBulletCount++;
	}

	return bLoaded;
}

void WeaponShotGunMaleFemale::OnPlayLoadBulletAnimation( void)
{
	INT8 idx = m_aii_LoadBullet_Male;

	if( m_pOwner->isFemale() == false)	idx = m_aii_LoadBullet_Male;
	else								idx = m_aii_LoadBullet_Female;

	_PlayWeaponAnimWithApplyTimeScale( idx);
}

REAL32 WeaponShotGunMaleFemale::GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type)
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetMoveSpeed( type);

	if( type == WEAPON_MOVE_SPEED_FIRE)
		type = WEAPON_MOVE_SPEED_NORMAL;

	return WeaponBase::GetMoveSpeed( type);
}

REAL32 WeaponShotGunMaleFemale::GetRange()
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetRange();
	
	return  m_pWeaponInfo->GetExtRange();
}

