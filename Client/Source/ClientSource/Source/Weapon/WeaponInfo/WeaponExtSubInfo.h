#if !defined( __WEAPON_EXT_SUB_INFO_H__)
#define __WEAPON_EXT_SUB_INFO_H__

#include "WeaponExtInfoBase.h"

class CWeaponExtSubInfo : public CWeaponExtInfoBase
{
	I3_CLASS_DEFINE( CWeaponExtSubInfo, CWeaponExtInfoBase);

private:
	VAR_LR(REAL32, 115)		m_rExtRange;
	VAR_LR(REAL32, 116)		m_rExtDPD_Rate;
	VAR_LR(REAL32, 117)		m_rExtDamage;
	VAR_LR(REAL32, 118)		m_rExtAttackTime;

public:
	CWeaponExtSubInfo() 
	{
		m_rExtRange = 0.f, m_rExtDPD_Rate = 0.f, m_rExtDamage = 0.f, m_rExtAttackTime = 0.f;
	}

	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType) override;
	virtual void	OnEncrypt( UINT8 * pCode) override;

	REAL32			GetExtRange( UINT8 code) const;
	REAL32			GetExt_DPD_Rate( UINT8 code) const;
	REAL32			GetExtDamage( UINT8 code) const;
	REAL32			GetExtAttackTime( UINT8 code) const;
};

#endif
