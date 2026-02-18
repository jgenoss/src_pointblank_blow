#include "pch.h"
#include "WeaponShotGun_13PV.h"
#include "GameCharaBase.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"

#include "WeaponFunction.h"


I3_CLASS_INSTANCE( WeaponShotGun13PV);//, WeaponBase);

WeaponShotGun13PV::WeaponShotGun13PV(void)
{
}

WeaponShotGun13PV::~WeaponShotGun13PV(void)
{
}

void WeaponShotGun13PV::_OnInitVairable( void)
{
	WeaponBase::_OnInitVairable();

	m_aiiLoad_Mag1PV		= -1;
	m_aiiLoad_Mag3PV		= -1;
	m_aiiLoad_Bullet1PV		= -1;
	m_aiiLoad_Bullet3PV		= -1;
}

void WeaponShotGun13PV::OnBindResource( void)
{
	WeaponBase::OnBindResource();

	m_aiiLoad_Mag1PV	= (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV");
	m_aiiLoad_Mag3PV	= (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV");
	m_aiiLoad_Bullet1PV	= (INT8) m_pAICtx->FindAIStateByName( "LoadBullet1PV");
	m_aiiLoad_Bullet3PV	= (INT8) m_pAICtx->FindAIStateByName( "LoadBullet3PV");
}

void WeaponShotGun13PV::Reset( void)
{
	WeaponBase::Reset();
}

void WeaponShotGun13PV::_OnPlayWeaponFireAnim()
{
	I3ASSERT( getAIIndex( WEAPON::AI_FIRE) != -1);
	i3AIState * pAIState = m_pAICtx->getAIState( getAIIndex( WEAPON::AI_FIRE) );
	I3ASSERT( pAIState != nullptr);
	REAL32 rDelay = GetFireDelay();
	I3ASSERT( rDelay != 0.f);
	REAL32 rTimeScale = pAIState->getDuration() / rDelay;
	WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE), rTimeScale );
}

REAL32 WeaponShotGun13PV::GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai )
{
	REAL32 rTimeScale = 1.f;
	if( m_pOwner != nullptr)
	{
		CGameCharaAnimContext * pAnimCtx = m_pOwner->getAnimContext();

		i3AIContext * pAIUpper = pAnimCtx->getAIUpper();

		#if defined( AI_MOVE_TO_WEAPON)
			INT32 nID = getCharaAIIndex( m_pOwner->getAnimSet(), m_pOwner->getActionContext()->getBodyUpper(), ai);
		#else
			INT32 nID = pAnimCtx->getAIUpperID( m_pWeaponInfo->GetTypeClass(), m_pWeaponInfo->GetNumber(), ai);
		#endif

		if( pAIUpper != nullptr )
		{
			if( nID == -1)
			{
				I3PRINTLOG(I3LOG_NOTICE, "nID == -1");
				return 1.f;
			}
			I3ASSERT( nID < pAIUpper->getAIStateCount());
			i3AIState * pNewAIState = pAIUpper->getAIState( nID );
			if( pNewAIState != nullptr )
			{
				CGameFramework::LoadAIStateAnimation( pNewAIState);
				rTimeScale = pNewAIState->getAnimDuration() / GetFireDelay();
			}
		}
	}

	return rTimeScale;
}

void WeaponShotGun13PV::OnPlayLoadBulletAnimation( void)
{
	INT8 aiIndex = -1;

	if( m_pOwner != nullptr )
	{
		if( m_pOwner->is1PV() )
			aiIndex = getAIIndex( WEAPON::AI_LOADBULLET1PV);
		else
			aiIndex = getAIIndex( WEAPON::AI_LOADBULLET3PV);
	}
	
	if( _PlayWeaponAnimWithApplyTimeScale( aiIndex ) == false)
	{
		_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET));
	}
}

void WeaponShotGun13PV::OnPlayLoadMagAnimation( void)
{
	INT8 aiIndex = -1;

	if( m_pOwner != nullptr )
	{
		if( m_pOwner->is1PV() )
			aiIndex = getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV);
		else
			aiIndex = getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV);
	}
	
	if( _PlayWeaponAnimWithApplyTimeScale( aiIndex) == false)
	{
		_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE) );
	}

	setWeaponState( WEAPON::STATE_LOADMAG);
}