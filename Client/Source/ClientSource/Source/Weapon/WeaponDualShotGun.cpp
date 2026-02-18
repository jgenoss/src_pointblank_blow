#include "pch.h"
#include "WeaponDualShotGun.h"
#include "WeaponFunction.h"

#include "MainWeapon_DualShotGun.h"
#include "GameCharaInputContext.h"

namespace
{
	i3::string s_strAIName[AI_COUNT] =
	{
		"LoadMag_Ready",
		"LoadMag",
		"LoadBullet",
		"Fire",
		"Fire_Ext",
		"Idle",
		"Idle_Ext",
		"ExtensionActivate",
		"ExtensionDeactivate",
		"Change",
		"Change_Ext",
		"Change_Ext_Dual_Right",
		"Change_Ext_Dual_Left"
	};

	i3::string s_strGenderType[GENDER_COUNT] =
	{
		"Male",
		"Female"
	};
};


I3_CLASS_INSTANCE( WeaponDualShotGun);//, WeaponShotGun);

MainWeapon*		WeaponDualShotGun::CreateMainWeapon()
{
	MainWeapon_DualShotGun * p = new MainWeapon_DualShotGun(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualShotGun));
	return p;
}

WeaponDualShotGun::WeaponDualShotGun()
{

}

WeaponDualShotGun::~WeaponDualShotGun()
{
}


/*virtual*/ void WeaponDualShotGun::_OnInitVairable()
{
	WeaponShotGun::_OnInitVairable();

	i3mem::FillZero(m_pBullet, sizeof(m_pBullet) );
	i3mem::Fill(m_i8AIIndex, -1, sizeof(m_i8AIIndex) );
}


/*virtual*/ void WeaponDualShotGun::_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo)
{
	I3ASSERT( pInfo->_pPos != nullptr);
	I3ASSERT( pInfo->_pNormal != nullptr);
	I3ASSERT( pInfo->_pDir != nullptr);

	g_pEffectManager->AddTerrainEffect( pInfo->_terrainType, m_pWeaponInfo->GetTypeClass(),
		static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType()),
		pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj );
}

/*virtual*/ void WeaponDualShotGun::_PlayFireSound( WEAPON::FIRE_SOUND_TYPE type)
{
	// 듀얼 샷건의 Fire Sound는 WEAPON::FIRE_SOUND_NORMAL로 강제합니다.
	WeaponBase::_PlayFireSound(WEAPON::FIRE_SOUND_NORMAL);
}

void WeaponDualShotGun::_VisibleBullet( bool bEnable)
{
	for( INT32 type = 0 ; type < LOD_COUNT ; ++type )
	{
		for ( INT32 idx = 0 ; idx < BULLET_COUNT ; ++idx)
		{
			if( m_pBullet[type][idx] != nullptr)
			{
				if( bEnable )
					m_pBullet[type][idx]->RemoveFlag(I3_NODEFLAG_INVISIBLE);
				else
					m_pBullet[type][idx]->AddFlag(I3_NODEFLAG_INVISIBLE);
			}
		}
	}
}

void WeaponDualShotGun::_BindAnimationIndex(CGameCharaBase * pOwner)
{
	if( pOwner == nullptr )
		return;

	INT32 type = pOwner->isFemale() ? 1 : 0 ;	// 남 : 0 , 여 : 1;

	setAIIndex( WEAPON::AI_LOADMAGAZINE,			m_i8AIIndex[type][1]	);
	setAIIndex( WEAPON::AI_LOADBULLET,				m_i8AIIndex[type][2]	);
	setAIIndex( WEAPON::AI_FIRE,					m_i8AIIndex[type][4]	);
	setAIIndex( WEAPON::AI_SECFIRE,					m_i8AIIndex[type][4]	);
	setAIIndex( WEAPON::AI_IDLE,					m_i8AIIndex[type][6]	);
	setAIIndex( WEAPON::AI_EXTENSION_ACTIVATE,		m_i8AIIndex[type][7]	);
	setAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE,	m_i8AIIndex[type][8]	);
	setAIIndex( WEAPON::AI_CHANGE,					m_i8AIIndex[type][10]	);
	setAIIndex( WEAPON::AI_CHANGEDUAL_RIGHT,		m_i8AIIndex[type][11]	);
	setAIIndex( WEAPON::AI_CHANGEDUAL_LEFT,			m_i8AIIndex[type][12]	);
}

/*virtual*/ void WeaponDualShotGun::OnBindResource(void)
{
	WeaponBase::OnBindResource();

	// LOD별 Bullet Dummy 설정
	for( INT32 type = 0 ; type < LOD_COUNT ; ++type)
	{
		i3LOD * pLOD = getSceneObject()->GetBody()->getLOD(type);
		I3ASSERT( pLOD != nullptr );

		for( INT32 idx = 0; idx < BULLET_COUNT ; ++idx )
		{
			i3::string str_tmp = i3::format_string( "VolanimationDummy%02d", idx + 1 );
			m_pBullet[type][idx] = i3Scene::FindNodeByName( pLOD->getShapeNode(), str_tmp.c_str() );
		}
	}

	// Animation 설정
	for( INT32 type = 0 ; type < GENDER_COUNT ; ++type )
	{
		for( INT32 idx = 0 ; idx < AI_COUNT ; ++idx )
		{
			i3::string str_tmp = i3::format_string( "%s_%s", s_strAIName[idx], s_strGenderType[type] );
			m_i8AIIndex[type][idx] = static_cast<INT8>( m_pAICtx->FindAIStateByName( str_tmp.c_str() ) );
		}	
	}
}

/*virtual*/ void WeaponDualShotGun::SetOwner( CGameCharaBase * pOwner)
{
	_BindAnimationIndex(pOwner);
	WeaponBase::SetOwner( pOwner);
}

/****************************************************************************/
/*							Weapon Order Functions							*/
/****************************************************************************/

/*virtual*/bool WeaponDualShotGun::Fire( WEAPON::FIRE_ORDER type)
{
	MainWeapon_DualShotGun* pMainWeapon = static_cast<MainWeapon_DualShotGun*>( GetMainWeapon() );
	return pMainWeapon->Fire_DualShotGun(type);
}

/*virtual*/void WeaponDualShotGun::FireEnd( void)
{
	WeaponBase::FireEnd();

	if( m_pOwner && m_pOwner->isLocal() )
	{
		UINT64 key_press = m_pOwner->getInputContext()->getKeyPressInput();

		if( key_press & GAME_KEY_ACTION_ATTACK || key_press & GAME_KEY_ACTION_SUBFUNC )
			return;
	}

	MainWeapon_DualShotGun* pMainWeapon = static_cast<MainWeapon_DualShotGun*>(GetMainWeapon());
	pMainWeapon->FireEnd_DualShotGun();
}

/****************************************************************************/
/*							Animation Play Functions						*/
/****************************************************************************/

/*virtual*/ void WeaponDualShotGun::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/ )
{
	if( g_pFramework->IsBattleStage() )
	{
		// LEFT, RIGHT Change Animation이 별도로 동작하는 경우에만 해당 프로세스 호출.
		if( (getAIIndex(WEAPON::AI_CHANGEDUAL_RIGHT) != -1) && (getAIIndex(WEAPON::AI_CHANGEDUAL_LEFT) != -1) )
		{
			MainWeapon_DualShotGun* pMainWeapon = static_cast<MainWeapon_DualShotGun*>(GetMainWeapon());
			pMainWeapon->PlayAnim_Change_DualShotGun(rTimeScale);
		}
		else
		{
			// _BindAnimationIndex에 다른 교체 애니메이션 등록 후 이용해주세요..
			WeaponBase::OnPlayChangeAniImpl_Default(rTimeScale);
		}
	}
	else
	{
		WeaponBase::OnPlayChangeAniImpl_Default(rTimeScale);
	}
}

/****************************************************************************/
/*								Network Functions							*/
/****************************************************************************/

/*virtual*/void WeaponDualShotGun::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	MainWeapon_DualShotGun* mainWeapon = static_cast<MainWeapon_DualShotGun*>(GetMainWeapon());
	mainWeapon->NET_Fire_DualShotGun(pStartPos, pTargetPos, type);	
}

/****************************************************************************/
/*						Animation Callback function							*/
/****************************************************************************/

/*virtual*/ void WeaponDualShotGun::OnEndFire(i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFire(pCtx, tm);

	MainWeapon_DualShotGun* pMainWeapon = static_cast<MainWeapon_DualShotGun*>(GetMainWeapon());
	pMainWeapon->setFireState(MainWeapon_DualShotGun::STATE_END);

	WeaponBase * pWeapon = i3::same_cast<WeaponDualShotGun*>( getPairWeapon() );
	if( pWeapon != nullptr )
		pWeapon->OnEndFire(pCtx, tm);
}

//////////////////////////////////////////////////////////////////////////////

/*virtual*/ REAL32 WeaponDualShotGun::GetFireDelay(void)
{
	// Left Weapon인 경우에는 Right 무기의 FireDelay를 따라갑니다.
	WeaponBase * pWeapon = i3::same_cast<WeaponDualShotGun*>( getPairWeapon() );
	if( pWeapon != nullptr )
		return pWeapon->GetFireDelay();

	REAL32 rVal = m_pWeaponInfo->GetFireDelay(); 
	MainWeapon_DualShotGun* pMainWeapon = static_cast<MainWeapon_DualShotGun*>(GetMainWeapon());

	if( pMainWeapon->isDualFire() )
		rVal = _CalcFireDelayToVersion(rVal);
	else
		rVal = GetFireDelay2();

	return rVal;
}

INT32 WeaponDualShotGun::_OnProcessFireBulletCount(void)
{
	MainWeapon_DualShotGun* pMainWeapon = static_cast<MainWeapon_DualShotGun*>( GetMainWeapon() );
	WeaponBase* pairweapon = getPairWeapon();

	INT32 bullet_count = getLoadedBulletCount();
	INT32 dec_bullet_count = getWeaponInfo()->GetFireDecBulletCount();

	bullet_count -= dec_bullet_count;

	if (bullet_count <= 0)
		bullet_count = 0;

	setLoadedBulletCount(bullet_count);

	if (pairweapon != nullptr && pMainWeapon->getLeftWeapon() == nullptr)
	{
		INT32 pair_bullet_count = pairweapon->getLoadedBulletCount();
		INT32 pair_dec_bullet_count = pairweapon->getWeaponInfo()->GetFireDecBulletCount();
		pair_bullet_count -= pair_dec_bullet_count;

		pairweapon->setLoadedBulletCount(pair_bullet_count);
	}

	return bullet_count;
}
