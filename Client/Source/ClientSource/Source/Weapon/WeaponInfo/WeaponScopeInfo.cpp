#include "pch.h"
#include "WeaponScopeInfo.h"

#include "BattleHUD_Scope.h"

I3_CLASS_INSTANCE( CWeaponScopeInfo); //, CWeaponExtGunInfo);

void CWeaponScopeInfo::OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType)
{
	INT32 nTemp = 0;

	CWeaponExtGunInfo::OnReadInfo( pKey, extType);

	i3::rc_wstring	wstrScopeType_Path;
	FIND_REG_DATA( pKey, "ScopeType", wstrScopeType_Path);
	m_nScopeType = g_pWeaponInfoDataBase->InsertScopeHUDTexturePath(wstrScopeType_Path);

	nTemp = -1;
	FIND_REG_DATA(pKey, "SubScopeType", &nTemp);
	m_nSubScopeType = nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("MaxScopeLevel"),			&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_nMaxScopeLevel = (UINT8) nTemp;

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("FovOnScope"),				&rTemp);					// 1단 Scope시의 FOV값
	m_rScopeFOV = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("FovOnScope2"),				&rTemp);					// 2단 Scope시의 FOV값
	m_rScopeFOV2 = rTemp;
}

/*virtual*/ void CWeaponScopeInfo::OnEncrypt( UINT8 * pCode)
{
	CWeaponExtGunInfo::OnEncrypt( pCode);
}

INT32 CWeaponScopeInfo::GetMaxScopeLevel( UINT8 code) const
{
	return m_nMaxScopeLevel;
}

REAL32 CWeaponScopeInfo::GetScopeFOV( UINT8 code) const
{
	return m_rScopeFOV;
}
REAL32 CWeaponScopeInfo::GetScopeFOV2( UINT8 code) const
{
	return m_rScopeFOV2;
}

INT32 CWeaponScopeInfo::GetScopeType( UINT8 code) const
{
	return m_nScopeType;
}

INT32 CWeaponScopeInfo::GetSubScopeType() const
{
	return m_nSubScopeType;
}