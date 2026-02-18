#include "pch.h"
#include "WeaponExtInfoBase.h"

I3_CLASS_INSTANCE( CWeaponExtInfoBase); //, i3ElementBase);

CWeaponExtInfoBase::CWeaponExtInfoBase():m_ExtType(WEAPON::EXT_NONE),m_ItemID(0),m_bUseExtShape(false)
{
}

CWeaponExtInfoBase::~CWeaponExtInfoBase()
{

}

/*virtual*/ void CWeaponExtInfoBase::OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType)
{
	m_ExtType = extType;
	FIND_REG_DATA( pKey, LS_STR("ITEMID"), (INT32*) &m_ItemID);
	FIND_REG_DATA( pKey, "UseExtShape", (INT32*)&m_bUseExtShape);
}

/*virtual*/ void CWeaponExtInfoBase::OnEncrypt( UINT8 * pCode)
{
}

WEAPON::EXT_TYPE CWeaponExtInfoBase::GetExtType( UINT8 code) const
{
	return (WEAPON::EXT_TYPE) m_ExtType;
}

T_ItemID CWeaponExtInfoBase::GetItemID( UINT8 code) const
{
	return m_ItemID;
}
