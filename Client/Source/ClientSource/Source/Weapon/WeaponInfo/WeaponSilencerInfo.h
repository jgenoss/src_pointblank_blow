#if !defined( __WEAPON_SILENCER_INFO_H__)
#define __WEAPON_SILENCER_INFO_H__

#include "WeaponExtGunInfo.h"

class CWeaponSilencerInfo : public CWeaponExtGunInfo
{
	I3_CLASS_DEFINE( CWeaponSilencerInfo, CWeaponExtGunInfo);
private:
	VAR_LR(REAL32, 121)		m_ExtSilencerDmgRate = 1.0f;					// Extension : Damage Rate

public:

	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType) override;
	virtual void	OnEncrypt( UINT8 * pCode) override;

	REAL32		GetExtSilencerDmgRate( UINT8 code) const;
};

#endif
