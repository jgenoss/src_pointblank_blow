#include "pch.h"
#include "WeaponShotGun_Fire13PV.h"
#include "GameCharaBase.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"

#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponShotGunFire13PV);//, WeaponBase);

WeaponShotGunFire13PV::WeaponShotGunFire13PV()
{
}

WeaponShotGunFire13PV::~WeaponShotGunFire13PV()
{

}

void WeaponShotGunFire13PV::_OnInitVairable()
{
	WeaponShotGun::_OnInitVairable();

	m_aiFire_1PV		= -1;
	m_aiFire_3PV		= -1;
	//m_aiLoadMag1PV		= -1;
	//m_aiLoadMag3PV		= -1;
	//m_aiLoadBullet1PV	= -1;
	//m_aiLoadBullet3PV	= -1;
}

void WeaponShotGunFire13PV::Reset()
{
	WeaponShotGun::Reset();
}

void WeaponShotGunFire13PV::OnBindResource()
{
	WeaponShotGun::OnBindResource();

	m_aiFire_1PV		= (INT8) m_pAICtx->FindAIStateByName("Fire1PV");
	m_aiFire_3PV		= (INT8) m_pAICtx->FindAIStateByName("Fire3PV");
	//m_aiLoadMag1PV		= (INT8) m_pAICtx->FindAIStateByName("LoadMag_1PV");
	//m_aiLoadMag3PV		= (INT8) m_pAICtx->FindAIStateByName("LoadMag_3PV");
	setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_1PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_3PV"));
	setAIIndex( WEAPON::AI_LOADBULLET1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet_1PV"));
	setAIIndex( WEAPON::AI_LOADBULLET3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet_3PV"));
	//m_aiLoadBullet1PV	= (INT8) m_pAICtx->FindAIStateByName("LoadBullet_1PV");
	//m_aiLoadBullet3PV	= (INT8) m_pAICtx->FindAIStateByName("LoadBullet_3PV");

	i3::stack_string strTemp;
	INT32 i, j;
	for( i = 0; i < BULLETCOUNT_REMINGTON_ETA; i++)
	{
		i3::sprintf(strTemp, "Model_VOL0%d", i + 1);
			
		for( j = 0; j < 3; j++)
		{
			if( getSceneObject()->GetBody()->getLOD( j) != nullptr)
			{
				m_pBulletForRemington[ i][ j] = i3Scene::FindNodeByName( getSceneObject()->GetBody()->getLOD( j)->getShapeNode(), strTemp.c_str());

				if (m_pBulletForRemington[i][j] == nullptr)
				{
					i3::stack_string strTemp2;
					i3::sprintf(strTemp2, "Model_VOL%d", i + 1);
					m_pBulletForRemington[i][j] = i3Scene::FindNodeByName(getSceneObject()->GetBody()->getLOD(j)->getShapeNode(), strTemp2.c_str());
				}
			}
		}
	}
}

void WeaponShotGunFire13PV::_OnPlayWeaponFireAnim()
{
	INT8 aiFire = -1;

	if((m_aiFire_1PV != -1) && (m_aiFire_3PV != -1))
	{
		if(m_pOwner != nullptr && m_pOwner->is1PV()) aiFire = m_aiFire_1PV;
		else	aiFire = m_aiFire_3PV;
	}

	I3ASSERT( aiFire != -1);
	i3AIState * pAIState = m_pAICtx->getAIState( aiFire );
	I3ASSERT( pAIState != nullptr);
	REAL32 rDelay = GetFireDelay();
	I3ASSERT( rDelay != 0.f);
	REAL32 rTimeScale = pAIState->getDuration() / rDelay;
	WF::PlayWeaponAnim(this, aiFire, rTimeScale );
}

void WeaponShotGunFire13PV::OnPlayLoadBulletAnimation()
{
	INT8 idx = -1;

	if( m_pOwner->is1PV())	idx = getAIIndex(WEAPON::AI_LOADBULLET1PV);
	else					idx = getAIIndex(WEAPON::AI_LOADBULLET3PV);

	_PlayWeaponAnimWithApplyTimeScale( idx);
}

void WeaponShotGunFire13PV::OnPlayLoadMagAnimation()
{
	INT8 idx; 
	if( m_pOwner->is1PV())	
		idx = getAIIndex(WEAPON::AI_LOADMAGAZINE_1PV);
	else					
		idx = getAIIndex(WEAPON::AI_LOADMAGAZINE_3PV);

	_PlayWeaponAnimWithApplyTimeScale( idx);
}

REAL32 WeaponShotGunFire13PV::GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai )
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

void WeaponShotGunFire13PV::OnLoadBullet( i3AIContext * pCtx, REAL32 tm )
{
	WeaponShotGun::OnLoadBullet( pCtx, tm);
}

bool WeaponShotGunFire13PV::OnLoadMag( i3AIContext * pCtx, REAL32 tm )
{
	bool bLoaded = false;

	bLoaded = WeaponShotGun::OnLoadMag(pCtx, tm);

	_VisibleBullet( true);

	return bLoaded;
}

void WeaponShotGunFire13PV::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/ )
{
	if( g_pFramework->IsBattleStage() )
	{
		m_pOwner->Cmd_Only_ChangeWeaponAction();
		WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGE), rTimeScale );
	}
}

REAL32 WeaponShotGunFire13PV::GetAnimLoadBulletScaleTime( CHARA_VIEW_TYPE eCAT )
{
	REAL32 rTimeScale = 1.f;
	INT32 idx = getAIIndex(WEAPON::AI_LOADBULLET1PV);

	if( eCAT == CHARA_VIEW_1PV)	idx = getAIIndex(WEAPON::AI_LOADBULLET1PV);
	else						idx = getAIIndex(WEAPON::AI_LOADBULLET3PV);

	if( idx != -1)
	{
		i3AIState * pAIState = m_pAICtx->getAIState( idx);
		if( pAIState != nullptr)
		{
			CGameFramework::LoadAIStateAnimation( pAIState);

			rTimeScale = pAIState->getAnimDuration() / m_pWeaponInfo->GetLoadBulletTime();
		}
	}

	return rTimeScale;
}

void WeaponShotGunFire13PV::OnSetVisibleExtension( bool bVal )
{
	_VisibleBullet(bVal);
}

void WeaponShotGunFire13PV::_VisibleBullet( bool bEnable )
{
	INT32 i, j;

	if( bEnable)
	{
		for( i = 0; i < BULLETCOUNT_REMINGTON_ETA; i++)
		{
			for( j = 0; j < 3; j++)
			{
				if( m_pBulletForRemington[ i][ j] != nullptr)
				{
					m_pBulletForRemington[ i][ j]->RemoveFlag( I3_NODEFLAG_INVISIBLE);
				}
			}
		}
	}
	else
	{
		for( i = 0; i < BULLETCOUNT_REMINGTON_ETA; i++)
		{
			for( j = 0; j < 3; j++)
			{
				if( m_pBulletForRemington[ i][ j] != nullptr)
				{
					m_pBulletForRemington[ i][ j]->AddFlag( I3_NODEFLAG_INVISIBLE);
				}
			}
		}
	}
}