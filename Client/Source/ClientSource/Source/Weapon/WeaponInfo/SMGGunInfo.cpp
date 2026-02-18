#include "pch.h"
#include "SMGGunInfo.h"
#include "GameCharaCollisionContext.h"


I3_CLASS_INSTANCE( CSMGGunInfo); //, CGunInfo);

void CSMGGunInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	CGunInfo::ReadWeaponInfo( pKey);

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("Deviation_DiminutionDistance"),	m_Deviation_DiminutionDistance);
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("Deviation_Decrement"),				m_Deviation_Decrement);
}
