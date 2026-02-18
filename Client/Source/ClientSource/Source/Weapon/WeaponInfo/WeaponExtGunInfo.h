#if !defined( __WEAPON_EXT_GUN_INFO_H__)
#define __WEAPON_EXT_GUN_INFO_H__

#include "WeaponExtSubInfo.h"

class CWeaponExtGunInfo : public CWeaponExtSubInfo
{
	I3_CLASS_DEFINE( CWeaponExtGunInfo, CWeaponExtSubInfo);
private:
	VAR_LR(REAL32,106)			m_ExtRecoilHorzFactor = 1.0f;
	VAR_LR(INT32,107)			m_ExtRecoilHorzCount = 0;
	VAR_LR(REAL32,108)			m_ExtRecoilVertFactor = 1.0f;
	VAR_LR(INT32,109)			m_ExtRecoilVertCount = 0;
	VAR_LR(REAL32,110)			m_ExtDeviationFactor = 1.0f;
	VAR_LR(INT32,111)			m_ExtAutoFireCount = 0;
	VAR_LR(REAL32,112)			m_rExtFireDelay = 0.0f;
	VAR_LR(INT32,113)			m_ExtOneHorzEnable = 0;
	VAR_LR(INT32,114)			m_ExtOneVertEnable = 0;
	INT32			m_ExtAutoPointShotEnable = 0;
public:

	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType) override;
	virtual void	OnEncrypt( UINT8 * pCode) override;
	
	REAL32			GetExtRecoilHorzFactor( UINT8 code) const;
	INT32			GetExtRecoilHorzCount( UINT8 code) const;
	REAL32			GetExtRecoilVertFactor( UINT8 code) const;
	INT32			GetExtRecoilVertCount(UINT8 code) const;
	REAL32			GetExtDeviationFactor( UINT8 code) const;
	INT32			GetExtAutoFireCount( UINT8 code) const;
	REAL32			GetExtFireDelay( UINT8 code) const;
	INT32			GetExtOneHorzEnable( UINT8 code) const;
	INT32			GetExtOneVertEnable(UINT8 code) const;
	INT32			GetExtAutoPointShotEnable(UINT8 code) const;
};

#endif
