#include "pch.h"
#include "WeaponInfo.h"

WeaponInfo::WeaponInfo()
{
	Clear();
}

WeaponInfo::~WeaponInfo()
{
}

void WeaponInfo::Clear() 
{ 
	m_TItemID = 0;
	m_ui8Usage = 0;

	i3mem::FillZero(m_r32Damage, sizeof(m_r32Damage));
	m_r32MaxDamage = 0.0f;

	m_r32FireDelay = 0.0f;
	i3mem::FillZero(m_r32AttackDelay, sizeof(REAL32)*WEAPON::ATTACK_TYPE_MAX);

	m_r32ExplosionRange = m_r32EffectiveSlope = 0.0f;

	m_ui16LoadBullet = 0xFFFF;							// 무한탄창 예외처리입니다.
	m_ui16MaxBullet = 0;
	m_ui8IncreaseCount = 0;

	m_r32Range = 0.0f;
	m_r32DPDRate = 0.0f;

	m_r32RecoilHorzAngle = m_r32RecoilHorzMax = m_r32RecoilVertAngle = m_r32RecoilVertMax = 0.0f;
	m_r32Deviation = 0.0f;

	m_ui8FireBulletCount = 0;
	m_ui8FireDecBulletCount = 1;						// 기본적으로 한발씩 발사 합니다.

	m_r32CriticalDamageRate = 0.0f;
	m_ui8CriticalChanceRange = 0;

	m_r32MoveSpeed = 0.0f;

	m_bLauncher			= FALSE;
	m_bIndicator		= FALSE;

	i3mem::FillZero( m_ExtInfo, sizeof(m_ExtInfo) );

	m_i32ExtCount = 0;
	i3mem::FillZero( m_RegWeaponInfo, sizeof(m_RegWeaponInfo) );

	i3mem::FillZero( m_eBulletType, sizeof(WEAPON::BULLET_SHOT)*(WEAPON::EXTENSION_MAX*WEAPON::HAND_TYPE_MAX) );
	i3mem::FillZero( m_pExtInfo,	sizeof(WeaponInfo*)*(WEAPON::EXTENSION_MAX*WEAPON::HAND_TYPE_MAX) );

	
}

WeaponInfo*	WeaponInfo::GetExtInfo( BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType )
{
	WEAPON::EXTENSION_TYPE	eExtType	= (WEAPON::EXTENSION_TYPE)GetExtType( bExtType );
	WEAPON::HAND_TYPE		eHandType	= (WEAPON::HAND_TYPE)GetHandType( eAttackType );

	WeaponInfo* pInfo = m_pExtInfo[ eExtType ][ eHandType ];
	if( NULL == pInfo )																return NULL;

	return pInfo;
}

WeaponInfo*	WeaponInfo::GetExtInfo( BOOL bExtType, WEAPON::HAND_TYPE eHandType )
{
	WEAPON::EXTENSION_TYPE	eExtType	= (WEAPON::EXTENSION_TYPE)GetExtType( bExtType );

	WeaponInfo* pInfo = m_pExtInfo[ eExtType ][ eHandType ];
	if( NULL == pInfo )																return NULL;

	return pInfo;
}

T_ItemID WeaponInfo::GetItemID( BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType )
{
	WeaponInfo*	pInfo = GetExtInfo( bExtType, eAttackType );
	if( NULL == pInfo )																return m_TItemID;

	return pInfo->m_TItemID;
}

T_ItemID WeaponInfo::GetAttackItemID( BOOL bExtType )
{
	WeaponInfo*	pInfo = GetExtInfo( bExtType, WEAPON::ATTACK_TYPE_L1 );
	if( NULL == pInfo )																return m_TItemID;

	return pInfo->m_TItemID;
}

void WeaponInfo::Copy( WeaponInfo* pInfo )
{
	memcpy( this, pInfo, sizeof(WeaponInfo) );
}

void WeaponInfo::BuildSpec()
{
	// PEF 에서 지원하지 않는 부분을 여기 하드코딩 합니다 ㅜㅠ
	switch( GET_ITEM_TYPE( m_TItemID ) )
	{
	case ITEM_TYPE_MELEE:
		{
			if( !m_bLauncher )	// Throwing knife가 아니면 무제한 공격
				m_ui16MaxBullet = 0xFFFF;
		}
		break;
	}

	WEAPON::BULLET_SHOT eBulletType;
	switch( GET_ITEM_SUBTYPE( m_TItemID ) )
	{
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_GRENADESHELL:
	case WEAPON_CLASS_DINO:
	case WEAPON_CLASS_ROCKET_LAUNCHER:
	case WEAPON_CLASS_THROWING_HEAL:
	case WEAPON_CLASS_BOW:
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_BOMBTRIGGER:
		{
			eBulletType = WEAPON::BULLET_SHOT_DROPPING;
		}
		break;
	default:
		{
			eBulletType = WEAPON::BULLET_SHOT_DIRECT;
		}
		break;
	}

	if( m_bLauncher )
	{
		eBulletType = WEAPON::BULLET_SHOT_DROPPING;
	}
	// 여기까지 -- PEF 에서 지원하지 않는 부분을 여기 하드코딩 합니다 ㅜㅠ

	for( INT32 i = 0 ; i < WEAPON::HAND_TYPE_MAX ; i++ )
	{
		if( WEAPON::BULLET_SHOT_NONE == m_eBulletType[ WEAPON::EXTENSION_OFF ][ i ] )	continue;
				
		m_eBulletType[ WEAPON::EXTENSION_OFF ][ i ]		= eBulletType;
		m_pExtInfo[ WEAPON::EXTENSION_OFF ][ i ]		= this;
	}

	WeaponExtInfo* pExtInfo;
	for( INT32 i = 0 ; i < DS_MAX_WEAPON_EXTENSION ; i++ )
	{
		pExtInfo = &m_ExtInfo[ i ];

		switch( pExtInfo->m_aExtType )
		{
		case WEAPON::EXT_BAYONET:
			{
				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DIRECT;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= this;

				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_RIGHT ]	= WEAPON::BULLET_SHOT_DIRECT;

				// 착검류 총기( ex : 103057. VZ.52 )는 Extention Weapon 정보가 pef에 없어서 예외처리 합니다.
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_RIGHT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );;
			}
			break;
		case WEAPON::EXT_GLAUNCHER:
			{
				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DROPPING;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );
			}
			break;
		case WEAPON::EXT_SHOTGUN_LAUNCHER:
			{
				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DIRECT;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );
			}
			break;
		case WEAPON::EXT_DINO:
			{
				m_eBulletType[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]	= WEAPON::BULLET_SHOT_DROPPING;
				m_pExtInfo[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );
			}
			break;
		case WEAPON::EXT_THROW:
			{
				switch( GET_ITEM_SUBTYPE( m_TItemID ) )
				{
				case WEAPON_CLASS_BOW:
				case WEAPON_SUBCLASS_BOW:
					{
						m_eBulletType[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_LEFT ]	= WEAPON::BULLET_SHOT_DROPPING;
						m_pExtInfo[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_LEFT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );
					}
					break;
				default:
					{
						m_eBulletType[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]	= WEAPON::BULLET_SHOT_DROPPING;
						m_pExtInfo[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );
					}
					break;
				}
			}
			break;
		case WEAPON::EXT_DUAL:
			{
				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DIRECT;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );

				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_RIGHT ]	= WEAPON::BULLET_SHOT_DIRECT;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_RIGHT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );
			}
			break;
		case WEAPON::EXT_DUAL_MAG:
			{
				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DIRECT;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= this;
			}
			break;
		case WEAPON::EXT_SILENCER:
			{
				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DIRECT;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= this;
			}
			break;
		case WEAPON::EXT_DUAL_LEFT:
			{
				m_eBulletType[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]	= WEAPON::BULLET_SHOT_DIRECT;

				switch( GET_ITEM_SUBTYPE( m_TItemID ) )
				{
				case WEAPON_CLASS_DUALKNIFE:
					m_pExtInfo[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]	= this;
					break;
				default:
					m_pExtInfo[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]	= Table::GetWeaponInfo( pExtInfo->m_TItemID );
					break;
				}
			}
			break;
		case WEAPON::EXT_THROW_SHELL:
			{
				m_eBulletType[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_LEFT ]	= WEAPON::BULLET_SHOT_DROPPING;
				m_pExtInfo[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_LEFT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );

				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DROPPING;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );
			}
			break;
		case WEAPON::EXT_CROSSFIRE:
			{
				m_eBulletType[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DIRECT;
				m_pExtInfo[ WEAPON::EXTENSION_ON ][ WEAPON::HAND_TYPE_LEFT ]		= this;
			}
			break;
		case WEAPON::EXT_DUAL_LEFT_DIF:
			{
				m_eBulletType[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]	= WEAPON::BULLET_SHOT_DIRECT;
				m_pExtInfo[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]		= Table::GetWeaponInfo( pExtInfo->m_TItemID );
			}
			break;
		}
	}

	if( m_bIndicator )
	{
		m_pExtInfo[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ] = this;
	}

	if( 0.f != m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L3 ] )
	{
		m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L3 ] -= m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L2 ];
	}
	if( 0.f != m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L2 ] )
	{
		m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L2 ] -= m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L1 ];
	}
	
	if( 0.f != m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R4] )
	{
		m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R4 ] -= m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R3 ];
	}
	if( 0.f != m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R3 ] )
	{
		m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R3 ] -= m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R2 ];
	}
	if( 0.f != m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R2 ] )
	{
		m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R2 ] -= m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R1 ];
	}
}

void WeaponInfo::SetRegisterWeaponInfo(WeaponInfo * pInfo)
{
	WeaponExtInfo * pExtInfo = NULL;

	if(pInfo == NULL)
	{
		m_RegWeaponInfo[m_i32ExtCount].SetWeaponExtInfo(m_TItemID);
		m_i32ExtCount++;
		pExtInfo = m_ExtInfo;
	}
	else
	{
		pExtInfo = pInfo->m_ExtInfo;
	}

	WeaponInfo * pRegWeaponInfo	= NULL;

	for(INT32 idx = 0 ; idx < DS_MAX_WEAPON_EXTENSION ; ++idx)
	{
		T_ItemID ExtItemID = pExtInfo[idx].m_TItemID;
		if(ExtItemID == 0)	continue;
		// Base ItemID와 Ext ItemID가 같은 경우, Extension 등록은 한번만 하기위한 예외처리.
		if(pInfo != NULL && pInfo->m_TItemID == ExtItemID ) continue;

		pRegWeaponInfo = Table::GetWeaponInfo(ExtItemID);

		if( NULL == pRegWeaponInfo || FALSE == pRegWeaponInfo->IsValid() )
		{
			DSERROR("WeaponInfo Not Found:%d (ExtItemID[%d]), BaseID=%d\n", pExtInfo[idx].m_TItemID, idx, m_TItemID );
			return;
		}

		I3ASSERT( m_i32ExtCount < DS_MAX_WEAPON_REGISTER );
		m_RegWeaponInfo[m_i32ExtCount++].Copy(&pExtInfo[idx]);

		// 재귀를 돌며, 확장 무기가 가진, 확장 타입까지 등록합니다.
		SetRegisterWeaponInfo(pRegWeaponInfo);
	}
}
