#if !defined( __WEAPON_ZOOM_INFO_H__)
#define __WEAPON_ZOOM_INFO_H__

#include "WeaponExtGunInfo.h"

class CWeaponZoomInfo : public CWeaponExtGunInfo
{
	I3_CLASS_DEFINE( CWeaponZoomInfo, CWeaponExtGunInfo);
private:
	VAR_LR(REAL32, 122)	m_rZoomFOV = 0.0f;

public:
	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType) override;

	REAL32			GetZoomFov( UINT8 code) const;
};

#endif
