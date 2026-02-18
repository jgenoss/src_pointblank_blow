#ifndef __WEAPON_INFO_H__
#define __WEAPON_INFO_H__

#define DS_MAX_WEAPON_EXTENSION	4	// weapon 1개당 4개 가능하고 depth 2까지 16개까지 가능하지만 동시에 사용 가능한 것은 4개로 제한.
#define DS_MAX_WEAPON_REGISTER	5	// 등록 무기의 갯수[ 원본 무기(1) + 확장 무기(4) ] 

//////////////////////////////////////////////////////////////////////////////////
//
// A weapon damage information
//
//////////////////////////////////////////////////////////////////////////////////
enum WEAPON_ABILITY
{
	WEAPON_ABILITY_THROW			= 0x1,
};

struct WeaponExtInfo
{
	T_ItemID			m_TItemID;
	WEAPON::EXT_TYPE	m_aExtType;

	bool IsSameItemID(T_ItemID ItemID)
	{
		return m_TItemID == ItemID;
	}

	bool IsSameExtType( WEAPON::EXT_TYPE extType)
	{
		return m_aExtType == extType;
	}

	void SetWeaponExtInfo(T_ItemID TItemID, WEAPON::EXT_TYPE extType = WEAPON::EXT_NONE)
	{
		m_TItemID = TItemID;
		m_aExtType = extType;
	}

	void Copy(WeaponExtInfo * pData)
	{
		m_TItemID = pData->m_TItemID;
		m_aExtType = pData->m_aExtType;
	}
};

struct WeaponInfo
{
	// Weapon Unique Key Information
	T_ItemID				m_TItemID;
	UINT8					m_ui8Usage;

	// Weapon Damage Value
	REAL32					m_r32Damage[ WEAPON::ATTACK_TYPE_MAX ];
	REAL32					m_r32MaxDamage;

	// Fire Delay
	REAL32					m_r32FireDelay;
	REAL32					m_r32AttackDelay[ WEAPON::ATTACK_TYPE_MAX ];

	// Explosion
	REAL32					m_r32ExplosionRange;
	REAL32					m_r32EffectiveSlope;

	// Bullet Info
	UINT16					m_ui16LoadBullet;
	UINT16					m_ui16MaxBullet;
	UINT8					m_ui8IncreaseCount;	// 투척무기(폭파/특수) 슬롯 증가 아이템을 사용할 경우 증가되는 수
	REAL32					m_r32Range;
	REAL32					m_r32DPDRate;

	// Recoil Parameter
	REAL32					m_r32RecoilHorzAngle;	//수평반동
	REAL32					m_r32RecoilHorzMax;
	REAL32					m_r32RecoilVertAngle;	//수직반동
	REAL32					m_r32RecoilVertMax;	
	REAL32					m_r32Deviation;			// 편차

	// Fire Bullet Count : 산탄총 등
	UINT8					m_ui8FireBulletCount;
	UINT8					m_ui8FireDecBulletCount;


	REAL32					m_r32CriticalDamageRate;
	UINT8					m_ui8CriticalChanceRange;

	// 무기별 (상태별) 속도 중 최대값
	REAL32					m_r32MoveSpeed;

	BOOL					m_bLauncher;	// IsThrowWeapon
	BOOL					m_bIndicator;

	// Extention Info.
	WeaponExtInfo			m_ExtInfo[ DS_MAX_WEAPON_EXTENSION ];
	
	// Register Weapon Info
	INT32					m_i32ExtCount;
	WeaponExtInfo			m_RegWeaponInfo[ DS_MAX_WEAPON_REGISTER ];

	// 
	WEAPON::BULLET_SHOT		m_eBulletType[ WEAPON::EXTENSION_MAX ][ WEAPON::HAND_TYPE_MAX ];		// 총알이 날아가는 형태
	WeaponInfo*				m_pExtInfo[ WEAPON::EXTENSION_MAX ][ WEAPON::HAND_TYPE_MAX ];

public:
	WeaponInfo();
	~WeaponInfo();

	void					Clear();

	void					Copy( WeaponInfo* pInfo );
	void					BuildSpec();

	bool					IsValid()
	{
		if( m_TItemID == 0 )																					return false;
		if( m_r32Range < I3_EPS )																				return false;
		if( GET_ITEM_SUBTYPE(m_TItemID) == WEAPON_CLASS_THROWING_GRENADE && m_r32ExplosionRange < I3_EPS )		return false;

		return true;
	}

	// EXT_TYPE을 통한 ItemID 반환.
	T_ItemID FindWeaponID( WEAPON::EXT_TYPE extType)
	{
		if(extType == WEAPON::EXT_NONE)
			return m_TItemID;

		for( INT32 i = 0 ; i < DS_MAX_WEAPON_REGISTER ; ++i)
		{
			if(m_RegWeaponInfo[i].IsSameExtType(extType))	
				return m_RegWeaponInfo[i].m_TItemID;
		}

		return 0;
	}

	INT32 FindExtStateIdx( WEAPON::EXT_TYPE extType)
	{
		if(extType == WEAPON::EXT_NONE )
			return 0;

		for( INT32 idx = 0 ; idx < DS_MAX_WEAPON_REGISTER; ++idx)
		{
			if(m_RegWeaponInfo[idx].IsSameExtType(extType))
				return idx;
		}

		return -1;
	}

	void					SetRegisterWeaponInfo(WeaponInfo * pInfo);

	bool					UseRandomDamage(void)
	{
		ITEM_TYPE itemType = (ITEM_TYPE)GET_ITEM_TYPE(m_TItemID);
		WEAPON_CLASS_TYPE subType = (WEAPON_CLASS_TYPE)GET_ITEM_SUBTYPE(m_TItemID);

		// 주무기 (RocketLauncher 제외), 보조무기만 랜덤 데미지 사용
		return ( itemType == ITEM_TYPE_PRIMARY || itemType == ITEM_TYPE_SECONDARY) &&
				( subType != WEAPON_CLASS_ROCKET_LAUNCHER );
	}

	INT32					GetExtType( BOOL bExtType )								{	return bExtType ? WEAPON::EXTENSION_ON : WEAPON::EXTENSION_OFF;	}
	INT32					GetHandType( WEAPON::ATTACK_TYPE eAttackType )
	{
		switch( eAttackType )
		{
		case WEAPON::ATTACK_TYPE_L1:		case WEAPON::ATTACK_TYPE_L2:		case WEAPON::ATTACK_TYPE_L3:	default:
			return WEAPON::HAND_TYPE_LEFT;
			break;
		case WEAPON::ATTACK_TYPE_R1:		case WEAPON::ATTACK_TYPE_R2:		case WEAPON::ATTACK_TYPE_R3:	case WEAPON::ATTACK_TYPE_R4:
			return WEAPON::HAND_TYPE_RIGHT;
			break;
		}
	}
	WeaponInfo*				GetExtInfo( BOOL bExtType, WEAPON::ATTACK_TYPE ui8AttackType );
	WeaponInfo*				GetExtInfo( BOOL bExtType, WEAPON::HAND_TYPE eHandType );
	WEAPON_SLOT_TYPE		GetWeaponUsage()		{ return static_cast<WEAPON_SLOT_TYPE>(WEAPON::ItemID2Slot(m_TItemID)); }
	WEAPON_CLASS_TYPE		GetWeaponClass()		{ return static_cast<WEAPON_CLASS_TYPE>( GET_ITEM_SUBTYPE(m_TItemID) ); }

	REAL32					GetWeaponRange( BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType );
	INT32					GetWeaponDamage( UINT8 ui8Critical, BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType, REAL32 r32DamagePlus, UINT8 ui8PBVer );
	REAL32					GetDPDRate( BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType );

	WEAPON::BULLET_SHOT		GetBulletType( BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType )
	{
		return m_eBulletType[ GetExtType(bExtType) ][ GetHandType(eAttackType) ];
	}

	T_ItemID				GetItemID( BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType );
	T_ItemID				GetAttackItemID( BOOL bExtType );
};

inline REAL32 WeaponInfo::GetWeaponRange( BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType )
{
	switch( Weapon::GetUsageFromItemID(m_TItemID) )
	{
	case WEAPON_SLOT_THROWING1:
	case WEAPON_SLOT_THROWING2:
		return m_r32ExplosionRange;
	}

	WeaponInfo* pInfo = GetExtInfo( bExtType, eAttackType );
	if( NULL == pInfo )	return 0.f;
		
	return pInfo->m_r32Range;
}

inline INT32 WeaponInfo::GetWeaponDamage( UINT8 ui8Critical, BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType, REAL32 r32DamagePlus, UINT8 ui8PBVer)
{
	if( WEAPON::ATTACK_TYPE_NONE >= eAttackType )	return 0;
	if( WEAPON::ATTACK_TYPE_MAX <= eAttackType )	return 0;

	WeaponInfo* pInfo = GetExtInfo( bExtType, eAttackType );
	if( NULL == pInfo )	return 0;

	INT32 i32Damage;	
		
	switch (ui8PBVer)
	{
	case PB_VER_V10:
	{
		// random damage = (DamageMax - Damage) * [Random Value]
		// random value : 0.0 ~ 1.0
		REAL32 r32RandomDamage = static_cast<INT32>((pInfo->m_r32MaxDamage - pInfo->m_r32Damage[eAttackType] ) * i3Math::Randf() + 0.5f);

		REAL32 r32Damage = pInfo->m_r32Damage[eAttackType];

		if(r32RandomDamage > I3_EPS)
			r32Damage += r32RandomDamage;

		if (pInfo->m_r32MaxDamage > I3_EPS && r32Damage > pInfo->m_r32MaxDamage)
			r32Damage = pInfo->m_r32MaxDamage;

		r32Damage += r32DamagePlus;

		i32Damage = static_cast<INT32>(ui8Critical > 0 ? r32Damage * pInfo->m_r32CriticalDamageRate : r32Damage);
	}
		break;
	case PB_VER_V15://1.5 무기대미지 계산공식(Critical계수가 기본 대미지에만 적용)
	default:
	{
		i32Damage = static_cast<INT32>(ui8Critical > 0 ? pInfo->m_r32Damage[eAttackType] * pInfo->m_r32CriticalDamageRate : pInfo->m_r32Damage[eAttackType]);

		if (pInfo->m_r32MaxDamage < I3_EPS)
			return i32Damage;
		
		// random damage = (DamageMax - Damage) * [Random Value]
		// random value : 0.0 ~ 1.0
		INT32 i32RandomDamage = static_cast<INT32>((pInfo->m_r32MaxDamage - i32Damage) * i3Math::Randf() + 0.5f);

		i32Damage += i32RandomDamage;
		i32Damage += static_cast<INT32>(r32DamagePlus);
	}
		break;
	}
	
	return i32Damage;
}

inline 
REAL32 WeaponInfo::GetDPDRate( BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType )
{
	WeaponInfo* pInfo = GetExtInfo( bExtType, eAttackType );
	if( NULL == pInfo )	return 0.f;

	return pInfo->m_r32DPDRate;
}

#endif