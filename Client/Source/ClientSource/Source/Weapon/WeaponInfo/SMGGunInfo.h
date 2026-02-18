#if !defined( __SMG_GUN_INFO_H)
#define __SMG_GUN_INFO_H

#include "GunInfo.h"

class CSMGGunInfo :	public CGunInfo
{
	I3_CLASS_DEFINE( CSMGGunInfo, CGunInfo);
private:
	VAR_LR(REAL32, 201)		m_Deviation_DiminutionDistance = 0.0f;
	VAR_LR(REAL32, 202)		m_Deviation_Decrement = 0.0f;
	
public:
	
	virtual void	ReadWeaponInfo( i3RegKey * pKey);
	
};

#endif