#include "pch.h"
#include "WeaponZoomInfo.h"

I3_CLASS_INSTANCE( CWeaponZoomInfo); //, CWeaponExtGunInfo);

void CWeaponZoomInfo::OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType)
{
	CWeaponExtGunInfo::OnReadInfo( pKey, extType);

	I3ASSERT( (extType == WEAPON::EXT_STEADY_AIM) || (extType == WEAPON::EXT_ZOOM));

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("FovOnZoom"),	&rTemp);					// Zoom½ÃÀÇ FOV°ª
	m_rZoomFOV = rTemp;
}

REAL32 CWeaponZoomInfo::GetZoomFov( UINT8 code) const
{
	return m_rZoomFOV;
}
