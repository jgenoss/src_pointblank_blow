#include "pch.h"
#include "WeaponDualMagInfo.h"

I3_CLASS_INSTANCE( CWeaponDualMagInfo); //, CWeaponExtInfoBase);

void CWeaponDualMagInfo::OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType)
{
	CWeaponExtInfoBase::OnReadInfo( pKey, extType);

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("ReloadTimeDual"), &rTemp );
	m_rReloadTimeDual = rTemp;
}

void CWeaponDualMagInfo::OnEncrypt( UINT8 * pCode)
{
	CWeaponExtInfoBase::OnEncrypt( pCode);
}

// µà¾ó ÅºÃ¢ ÀåÀü ½Ã°£
REAL32 CWeaponDualMagInfo::GetReloadTimeDual( UINT8 code) const
{
	return m_rReloadTimeDual;
}
