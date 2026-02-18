#include "pch.h"
#include "WeaponExtSubInfo.h"

I3_CLASS_INSTANCE( CWeaponExtSubInfo); //, CWeaponExtInfoBase);

void CWeaponExtSubInfo::OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType)
{
	CWeaponExtInfoBase::OnReadInfo( pKey, extType);

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("ExtensionRange"),			&rTemp);
	m_rExtRange = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("ExtensionDPD_Rate"),		&rTemp);
	m_rExtDPD_Rate = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("ExtensionDamage"),			&rTemp);
	m_rExtDamage = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("ExtensionAttackTime"),		&rTemp);
	m_rExtAttackTime = rTemp;
}

void CWeaponExtSubInfo::OnEncrypt( UINT8 * pCode)
{
	CWeaponExtInfoBase::OnEncrypt( pCode);
}

REAL32 CWeaponExtSubInfo::GetExtRange( UINT8 code) const
{
	return m_rExtRange;
}

REAL32 CWeaponExtSubInfo::GetExt_DPD_Rate( UINT8 code) const
{
	return m_rExtDPD_Rate;
}

REAL32 CWeaponExtSubInfo::GetExtDamage( UINT8 code) const
{
	return m_rExtDamage;
}

REAL32 CWeaponExtSubInfo::GetExtAttackTime( UINT8 code) const
{
	return m_rExtAttackTime;
}
