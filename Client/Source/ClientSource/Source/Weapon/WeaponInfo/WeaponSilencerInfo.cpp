#include "pch.h"
#include "WeaponSilencerInfo.h"

I3_CLASS_INSTANCE( CWeaponSilencerInfo); //, CWeaponExtGunInfo);

void CWeaponSilencerInfo::OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType)
{
	CWeaponExtGunInfo::OnReadInfo( pKey, extType);

	REAL32 rTemp = 1.0f;
	FIND_REG_DATA( pKey, LS_STR("Ext_SilencerDamageRate"),	&rTemp);	// Silencer Damage Rate
	m_ExtSilencerDmgRate = rTemp;
}

void CWeaponSilencerInfo::OnEncrypt( UINT8 * pCode)
{
	CWeaponExtGunInfo::OnEncrypt( pCode);
}

REAL32 CWeaponSilencerInfo::GetExtSilencerDmgRate( UINT8 code) const
{
	return m_ExtSilencerDmgRate;
}
