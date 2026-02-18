#include "pch.h"
#include "MeleeWeaponInfo.h"
#include "MyRoomInfoContext.h"

I3_CLASS_INSTANCE( CMeleeWeaponInfo);//, CWeaponInfo);

void CMeleeWeaponInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	INT32 nTemp = 0;

	CWeaponInfo::ReadWeaponInfo(pKey);

	FIND_REG_DATA( pKey, LS_STR("Explosion_Time"),				&m_rExplosion_Time);				// 폭발 지연시간( Grenade류 등등)

	for(INT32 i=0; i<ATTACK_NUM_MAX; i++)
	{
		char temp[256] = "";

		i3::snprintf(temp, 256, LS_STR("AttackTimeA%d"), i+1);
		GlobalFunc::VAR_FIND_REG_DATA( pKey, temp,			m_rAttackTimeA[i]);

		i3::snprintf(temp, 256, LS_STR("AttackTimeB%d"), i+1);
		GlobalFunc::VAR_FIND_REG_DATA( pKey, temp,			m_rAttackTimeB[i]);

		i3::snprintf(temp, 256, LS_STR("AttackDamageA%d"), i+1);
		FIND_REG_DATA( pKey, temp,			&m_rAttackDamageA[i]);

		i3::snprintf(temp, 256, LS_STR("AttackDamageB%d"), i+1);
		FIND_REG_DATA( pKey, temp,			&m_rAttackDamageB[i]);

		m_rAttackDamageA_ForDino[i] = m_rAttackDamageA[i];
		m_rAttackDamageB_ForDino[i] = m_rAttackDamageB[i];

		i3::snprintf(temp, 256, LS_STR("AttackDamageA%dForDino"), i+1);
		FIND_REG_DATA( pKey, temp,			&m_rAttackDamageA_ForDino[i]);

		i3::snprintf(temp, 256, LS_STR("AttackDamageB%dForDino"), i+1);
		FIND_REG_DATA( pKey, temp,			&m_rAttackDamageB_ForDino[i]);
	}

	FIND_REG_DATA( pKey, LS_STR("Throw_Speed"),					&m_rThrowSpeed);

	FIND_REG_DATA( pKey, "InstanceCount",				&m_iInstanceCount);

	FIND_REG_DATA( pKey, "HideTime",					&m_rHideTime);

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("AutoFireCount"),				&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_nAutoFireCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("AutoFireCount2"),				&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_nAutoFireCount2 = (UINT8) nTemp;

	FIND_REG_DATA( pKey, "CollisionBoxExtent",			&m_v3CollisionBoxExtent);

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("LoadBullet"),					&nTemp);					// 탄창당 최대 탄수
	I3_BOUNDCHK( nTemp, 65535);
	m_nLoadBullet = (UINT16) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("MaxBullet"),					&nTemp);						// 탄창 수
	I3_BOUNDCHK( nTemp, 65535);
	m_nMaxBullet = (UINT16) nTemp;

	FIND_REG_DATA(pKey, "WallBetweenDistance",			&m_WallBetweenDistance);
	FIND_REG_DATA(pKey, "RotateFront",					&m_RotateFront);

	_FindAttributeKey( pKey, "CIC");
	_FindAttributeKey( pKey, "EnableBounce");
	_FindAttributeKey( pKey, "DelayedFireSound");
	_FindAttributeKey( pKey, "NoMoreSecSound");
	_FindAttributeKey( pKey, "DoNotThrowLast");

	FIND_REG_DATA( pKey, LS_STR("ThrowDelayTime"),				&m_rThrowDelayTime);

	_FindAttributeKey( pKey, "UseHitedAnimation");
	_FindAttributeKey( pKey, LS_STR("SpraySecAttack"));
}

/*virtual*/ void CMeleeWeaponInfo::OnEncriptInfo( UINT8 u8Key)
{
	INT32 i;

	CWeaponInfo::OnEncriptInfo( u8Key);

#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue( &m_rExplosion_Time, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rThrowSpeed, m_u8EncryptKey[VALUETYPE_REAL]);
#endif

	GlobalFunc::EncryptValue( &m_nLoadBullet, m_u8EncryptKey[VALUETYPE_INT]);
	GlobalFunc::EncryptValue( &m_nMaxBullet, m_u8EncryptKey[VALUETYPE_INT]);
	GlobalFunc::EncryptValue( &m_v3CollisionBoxExtent, sizeof(VEC3D), m_u8EncryptKey[VALUETYPE_VEC]);

	GlobalFunc::EncryptValue(&m_RotateFront, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue(&m_WallBetweenDistance, m_u8EncryptKey[VALUETYPE_REAL]);

	for( i = 0; i < ATTACK_NUM_MAX; i++)
	{	
		GlobalFunc::EncryptValue( &m_rAttackDamageA[i], m_u8EncryptKey[VALUETYPE_REAL]);
		GlobalFunc::EncryptValue( &m_rAttackDamageB[i], m_u8EncryptKey[VALUETYPE_REAL]);

		GlobalFunc::EncryptValue( &m_rAttackDamageA_ForDino[i], m_u8EncryptKey[VALUETYPE_REAL]);
		GlobalFunc::EncryptValue( &m_rAttackDamageB_ForDino[i], m_u8EncryptKey[VALUETYPE_REAL]);
	}
}
REAL32 CMeleeWeaponInfo::GetRotateFront()
{
	return GlobalFunc::DecryptValue( &m_RotateFront, m_u8EncryptKey[VALUETYPE_REAL]);
}

REAL32 CMeleeWeaponInfo::GetWallBetweenDistance()
{
	return GlobalFunc::DecryptValue( &m_WallBetweenDistance, m_u8EncryptKey[VALUETYPE_REAL]);
}

/*virtual*/ void CMeleeWeaponInfo::GetCollisionBoxExtent(VEC3D* pVec) const
{
	return GlobalFunc::DecryptValue( pVec, &m_v3CollisionBoxExtent, m_u8EncryptKey[VALUETYPE_VEC]);
}

/*virtual*/ REAL32 CMeleeWeaponInfo::GetExplosion_Time() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rExplosion_Time, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rExplosion_Time;
#endif
}

// 왼쪽 클릭 공격시 공격 타이밍. 인자는 공격 횟수째 인덱스 (예: 첫번째 공격은 인덱스 0)
/*virtual*/ REAL32 CMeleeWeaponInfo::GetAttackTimeA( INT32 num) const
{
	REAL32 time = m_rAttackTimeA[ num];

	return ( (num < ATTACK_NUM_MAX) ? time : 0.f);
}

// 오른쪽 클릭 공격시 공격 타이밍.
/*virtual*/ REAL32 CMeleeWeaponInfo::GetAttackTimeB( INT32 num) const
{
	REAL32 time = m_rAttackTimeB[ num];

	return ( (num < ATTACK_NUM_MAX) ? time : 0.f);
}

//	무기 공격력
/*virtual*/ INT32 CMeleeWeaponInfo::GetDamage() const
{
	REAL32 rDamage = 0.f;

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{
		rDamage = GlobalFunc::DecryptValue( &m_rAttackDamageA_ForDino[0], m_u8EncryptKey[VALUETYPE_REAL]);
	}
	else
	{
		rDamage = GlobalFunc::DecryptValue( &m_rAttackDamageA[0], m_u8EncryptKey[VALUETYPE_REAL]);
	}

	return static_cast<INT32>(rDamage);
}

// 장전 가능 탄알 수
/*virtual*/ INT32 CMeleeWeaponInfo::GetLoadBullet() const
{
	return GlobalFunc::DecryptValue( &m_nLoadBullet, m_u8EncryptKey[VALUETYPE_INT]);
}

// 왼쪽 클릭 공격 데미지
/*virtual*/ REAL32 CMeleeWeaponInfo::GetDamageA( INT32 num) const
{
	if(num >= ATTACK_NUM_MAX)
	{
		return 0.f;
	}

	REAL32 dm = 0.f;
	
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{
		dm = GlobalFunc::DecryptValue( &m_rAttackDamageA_ForDino[ num], m_u8EncryptKey[VALUETYPE_REAL]);
	}
	else
	{
		dm = GlobalFunc::DecryptValue( &m_rAttackDamageA[ num], m_u8EncryptKey[VALUETYPE_REAL]);
	}

	return dm;
}

// 오른쪽 클릭 공격 데미지
/*virtual*/ REAL32 CMeleeWeaponInfo::GetDamageB( INT32 num) const
{
	if(num >= ATTACK_NUM_MAX)
	{
		return 0.f;
	}

	REAL32 dm = 0.f;
	
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{
		dm = GlobalFunc::DecryptValue( &m_rAttackDamageB_ForDino[ num], m_u8EncryptKey[VALUETYPE_REAL]);
	}
	else
	{
		dm = GlobalFunc::DecryptValue( &m_rAttackDamageB[ num], m_u8EncryptKey[VALUETYPE_REAL]);
	}

	return dm;
}

// 던지는 속도
/*virtual*/ REAL32 CMeleeWeaponInfo::GetThrowSpeed() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rThrowSpeed, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rThrowSpeed;
#endif
}

/*virtual*/ INT32 CMeleeWeaponInfo::GetAutoFireCount() const
{
	return m_nAutoFireCount;
}

/*virtual*/ INT32 CMeleeWeaponInfo::GetAutoFireCount2() const
{
	return m_nAutoFireCount2;
}


// 최대 소유 탄알 수
/*virtual*/ INT32 CMeleeWeaponInfo::GetMaxBullet() const
{
	return GlobalFunc::DecryptValue( &m_nMaxBullet, m_u8EncryptKey[VALUETYPE_INT]);
}

/*virtual*/WEAPON::ATTACK_TYPE CMeleeWeaponInfo::GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount) const
{ 	
	INT32 atkType = WEAPON::ATTACK_TYPE_L1;

	if(fireOrder == WEAPON::PRIMARY_FIRE)		 atkType = WEAPON::ATTACK_TYPE_L1 + i32FireCount;
	else if(fireOrder == WEAPON::SECONDARY_FIRE) atkType = WEAPON::ATTACK_TYPE_R1 + i32FireCount;

	return (WEAPON::ATTACK_TYPE)atkType;
}
