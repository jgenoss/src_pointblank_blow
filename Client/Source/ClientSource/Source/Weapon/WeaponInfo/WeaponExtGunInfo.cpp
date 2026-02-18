#include "pch.h"
#include "WeaponExtGunInfo.h"

I3_CLASS_INSTANCE( CWeaponExtGunInfo); //, CWeaponExtSubInfo);


void CWeaponExtGunInfo::OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType)
{
	CWeaponExtSubInfo::OnReadInfo( pKey, extType);
	
	REAL32 rTemp = 1.0f;
	FIND_REG_DATA( pKey, LS_STR("Ext_RecoilHorzFactor"),	&rTemp);
	m_ExtRecoilHorzFactor = rTemp;
	
	FIND_REG_DATA( pKey, "Ext_AutoPointShotEnable",	&m_ExtAutoPointShotEnable);

	INT32 nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("Ext_RecoilHorzCount"),		&nTemp);
	m_ExtRecoilHorzCount = nTemp;

	rTemp = 1.0f;
	FIND_REG_DATA( pKey, LS_STR("Ext_RecoilVertFactor"),	&rTemp);
	m_ExtRecoilVertFactor = rTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("Ext_RecoilVertCount"),		&nTemp);
	m_ExtRecoilVertCount = nTemp;

	rTemp = 1.0f;
	FIND_REG_DATA( pKey, LS_STR("Ext_DeviationFactor"),		&rTemp);
	m_ExtDeviationFactor = rTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("Ext_AutoFireCount"),		&nTemp);
	m_ExtAutoFireCount = nTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Ext_FireDelay"),			&rTemp);
	m_rExtFireDelay = rTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("Ext_OneHorzEnable"),		&nTemp);
	m_ExtOneHorzEnable = nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("Ext_OneVertEnable"),		&nTemp);
	m_ExtOneVertEnable = nTemp;

	// 한프레임을 0.016초로 맞아떨어지게 계산해주자. 오차를 줄이기 위한 방책
	REAL32 rRemain;
	if( m_rExtFireDelay > 0.f)
	{
		m_rExtFireDelay	= m_rExtFireDelay + 0.00001f;
		rRemain = i3Math::fmod( m_rExtFireDelay, 0.016f);

		if( rRemain > 0.f)
		{
			rRemain = 0.016f - rRemain;
			m_rExtFireDelay = m_rExtFireDelay + rRemain;
		}
	}
}

void CWeaponExtGunInfo::OnEncrypt( UINT8 * pCode)
{
	CWeaponExtSubInfo::OnEncrypt( pCode);
}

REAL32 CWeaponExtGunInfo::GetExtRecoilHorzFactor( UINT8 code) const
{
	return m_ExtRecoilHorzFactor;
}

INT32 CWeaponExtGunInfo::GetExtRecoilHorzCount( UINT8 code) const
{
	return m_ExtRecoilHorzCount;
}

REAL32 CWeaponExtGunInfo::GetExtRecoilVertFactor( UINT8 code) const
{
	return m_ExtRecoilVertFactor;
}

INT32 CWeaponExtGunInfo::GetExtRecoilVertCount(UINT8 code) const
{
	return m_ExtRecoilVertCount;
}

REAL32 CWeaponExtGunInfo::GetExtDeviationFactor( UINT8 code) const
{
	return m_ExtDeviationFactor;
}

INT32 CWeaponExtGunInfo::GetExtAutoFireCount( UINT8 code) const
{
	return m_ExtAutoFireCount;
}

REAL32 CWeaponExtGunInfo::GetExtFireDelay( UINT8 code) const
{
	return m_rExtFireDelay;
}

INT32 CWeaponExtGunInfo::GetExtOneHorzEnable( UINT8 code) const
{
	return m_ExtOneHorzEnable;
}

INT32 CWeaponExtGunInfo::GetExtOneVertEnable(UINT8 code) const
{
	return m_ExtOneVertEnable;
}

INT32 CWeaponExtGunInfo::GetExtAutoPointShotEnable(UINT8 code) const
{
	return m_ExtAutoPointShotEnable;
}
