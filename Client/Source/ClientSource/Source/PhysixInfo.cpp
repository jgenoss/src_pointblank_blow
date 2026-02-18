#include "pch.h"
#include "PhysixInfo.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CPhysixInfo);//, i3NamedElement);
I3_CLASS_INSTANCE( PhysixInfoDataBase);//, i3GameNode);

void CPhysixInfo::ReadInfo( i3RegKey * pKey)
{
	INT32 len;
	char szName[ 260] = {0,};
	I3ASSERT( pKey != nullptr);

	len = i3::generic_string_size( pKey->GetName());

	if( len > 0)
	{
		i3::string_ncopy_nullpad( szName, pKey->GetName(), len);

		GlobalFunc::EncryptValue( szName, len, ENC_SHIFT_PHYSIX);

		SetName( szName);
	}

	//요거 중요한 넘입니다. 지우지 마세요( 케릭터 타입을 문자열이아닌 인덱스로 찾고자하는경우 쓰입니다.
	//내부의 데이터가 정렬이되는경우 Child의 인덱스로 찾기를 시도하는경우 오류가 생길수 있으므로 내부데이터를 하나둡니다.
	FIND_REG_DATA( pKey, LS_STR("Gravity"),			&m_rGravity );
	FIND_REG_DATA( pKey, LS_STR("SleepLinear"),		&m_rSleepLinVelocity );
	FIND_REG_DATA( pKey, LS_STR("SleepAngular"),	&m_rSleepAngVelocity );
	FIND_REG_DATA( pKey, LS_STR("BounceThresHold"),	&m_rBounceThresHold );
	FIND_REG_DATA( pKey, LS_STR("MaxAngVelocity"),	&m_rMaxAngVelocity );
	FIND_REG_DATA( pKey, LS_STR("SimTime"),			&m_rSimTimeSlop);
	FIND_REG_DATA( pKey, LS_STR("StaticFriction"),	&m_rStaticFriction );
	FIND_REG_DATA( pKey, LS_STR("DynamicFriction"), &m_rDynamicFriction );

	for( INT32 i = 0; i < pKey->getChildCount(); i++ )
	{
		i3RegKey * pChildKey = ( i3RegKey*)pKey->getChild( i);

		if( i3::generic_is_iequal( pChildKey->GetName(), "Character_Death" ) )
		{
			FIND_REG_DATA( pChildKey, "FXtoForce",			&m_rFXtoForce );

			FIND_REG_DATA( pChildKey, "Weapon_Assult",		&m_WeaponDeath.m_rWeapon_Assult );
			FIND_REG_DATA( pChildKey, "Weapon_Shotgun",		&m_WeaponDeath.m_rWeapon_Shotgun );
			FIND_REG_DATA( pChildKey, "Weapon_SMG",			&m_WeaponDeath.m_rWeapon_SMG );
			FIND_REG_DATA( pChildKey, "Weapon_Pistol",		&m_WeaponDeath.m_rWeapon_Pistol );
			FIND_REG_DATA( pChildKey, "Weapon_Rifle",		&m_WeaponDeath.m_rWeapon_Rifle );
			FIND_REG_DATA( pChildKey, "Weapon_Knife",		&m_WeaponDeath.m_rWeapon_Knife );
			FIND_REG_DATA( pChildKey, "Weapon_Grenade",		&m_WeaponDeath.m_rWeapon_Grenade );

			FIND_REG_DATA( pChildKey, "LinearForce",		&m_rCharaLinSlop );
			FIND_REG_DATA( pChildKey, "AngularForce",		&m_rCharaAngSlop );

			FIND_REG_DATA( pChildKey, "AngularForceFeed",	&m_vCharaAngForceFeed );
		}
		else if( i3::generic_is_iequal( pChildKey->GetName(), "Character_Default" ) )
		{
			FIND_REG_DATA( pChildKey, "Damping_Root",		&m_Damping.m_rRoot );
			FIND_REG_DATA( pChildKey, "Damping_Head",		&m_Damping.m_rHead );
			FIND_REG_DATA( pChildKey, "Damping_Neck",		&m_Damping.m_rNeck );
			FIND_REG_DATA( pChildKey, "Damping_Clavicle",	&m_Damping.m_rClavicle );
			FIND_REG_DATA( pChildKey, "Damping_Body",		&m_Damping.m_rBody );
			FIND_REG_DATA( pChildKey, "Damping_UpLeg",		&m_Damping.m_rUpperLeg );
			FIND_REG_DATA( pChildKey, "Damping_LwLeg",		&m_Damping.m_rLowLeg );
			FIND_REG_DATA( pChildKey, "Damping_Foot",		&m_Damping.m_rFoot );
			FIND_REG_DATA( pChildKey, "Damping_UpArm",		&m_Damping.m_rUpperArm );
			FIND_REG_DATA( pChildKey, "Damping_LwArm",		&m_Damping.m_rLowArm );
			FIND_REG_DATA( pChildKey, "Damping_Hand",		&m_Damping.m_rHand );
		}
		else if( i3::generic_is_iequal( pChildKey->GetName(), "Character_Equip" ))
		{
			FIND_REG_DATA( pChildKey, "Helmet_Feed",		&m_rHelmet_Feed );
			FIND_REG_DATA( pChildKey, "Helmet_Y",			&m_rHelmet_Y );
		}
		else if( i3::generic_is_iequal( pChildKey->GetName(), "Object" ) )
		{
			FIND_REG_DATA( pChildKey, "ObjectLinSlop",		&m_rObjectLinSlop );
			FIND_REG_DATA( pChildKey, "ObjectAngSlop",		&m_rObjectAngSlop );

			FIND_REG_DATA( pChildKey, "Weapon_Assult",		&m_WeaponObject.m_rWeapon_Assult );
			FIND_REG_DATA( pChildKey, "Weapon_Shotgun",		&m_WeaponObject.m_rWeapon_Shotgun );
			FIND_REG_DATA( pChildKey, "Weapon_SMG",			&m_WeaponObject.m_rWeapon_SMG );
			FIND_REG_DATA( pChildKey, "Weapon_Pistol",		&m_WeaponObject.m_rWeapon_Pistol );
			FIND_REG_DATA( pChildKey, "Weapon_Rifle",		&m_WeaponObject.m_rWeapon_Rifle );
			FIND_REG_DATA( pChildKey, "Weapon_Knife",		&m_WeaponObject.m_rWeapon_Knife );
			FIND_REG_DATA( pChildKey, "Weapon_Grenade",		&m_WeaponObject.m_rWeapon_Grenade );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

PhysixInfoDataBase::~PhysixInfoDataBase( void)
{
	I3_SAFE_RELEASE( m_pGlobalInfo);
}

bool PhysixInfoDataBase::Create( void)
{
	I3ASSERT( m_pGlobalInfo == nullptr);

	i3RegKey * pRoot = Config::_GetRegRoot("Basic/Physix");
	I3ASSERT( pRoot != nullptr);

	m_pGlobalInfo = CPhysixInfo::new_object();
	m_pGlobalInfo->ReadInfo( pRoot);
	
	return true;
}

#if !defined( I3_NO_PROFILE)
void PhysixInfoDataBase::OnReloadProperty()
{
	i3RegKey * pRoot = Config::_GetRegRoot("Basic/Physix");
	I3ASSERT( pRoot != nullptr);

	if( m_pGlobalInfo != nullptr)
	{
		m_pGlobalInfo->ReadInfo( pRoot);
		if( g_pFramework == nullptr )
			return;

		i3PhysixContext * pCtx = g_pFramework->getPhysixContext();
		if( pCtx == nullptr )
			return;

		{
			pCtx->setGravity( m_pGlobalInfo->m_rGravity );
			pCtx->setSimTime( m_pGlobalInfo->m_rSimTimeSlop );
			pCtx->setBoundThresHold( m_pGlobalInfo->m_rBounceThresHold );
			pCtx->setFxToForce( m_pGlobalInfo->m_rFXtoForce );
			pCtx->setSleepLinVelocity( m_pGlobalInfo->m_rSleepLinVelocity );
			pCtx->setSleepAngVelocity( m_pGlobalInfo->m_rSleepAngVelocity );
			pCtx->setMaxAngVelocity( m_pGlobalInfo->m_rMaxAngVelocity );

			pCtx->setStaticFriction( m_pGlobalInfo->m_rStaticFriction );
			pCtx->setDynamicFriction( m_pGlobalInfo->m_rDynamicFriction );
		}
	}
}
#endif
