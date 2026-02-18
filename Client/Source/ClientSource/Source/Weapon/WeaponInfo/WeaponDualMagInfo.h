#if !defined( __WEAPON_DUALMAG_INFO_H__)
#define __WEAPON_DUALMAG_INFO_H__

#include "WeaponExtInfoBase.h"

class CWeaponDualMagInfo : public CWeaponExtInfoBase
{
	I3_CLASS_DEFINE( CWeaponDualMagInfo, CWeaponExtInfoBase);
private:
	VAR_LR(REAL32, 105)			m_rReloadTimeDual = 0.0f;			// µà¾ó ÅºÃ¢ ÀåÀü ½Ã°£

public:
	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType) override;
	virtual void	OnEncrypt( UINT8 * pCode) override;

	REAL32			GetReloadTimeDual( UINT8 code) const;
};

#endif
