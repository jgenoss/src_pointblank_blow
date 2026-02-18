#include "StdAfx.h"
#include "ItemTabWeapon.h"
#include "DlgWeaponEditor.h"

I3_CLASS_INSTANCE(cItemTabWeapon); //, cItemTabBase)

cItemTabWeapon::cItemTabWeapon(void)
:	m_pDlgWeapon( NULL )
{

}

cItemTabWeapon::~cItemTabWeapon(void)
{
	I3_SAFE_DELETE(m_pDlgWeapon);
}
