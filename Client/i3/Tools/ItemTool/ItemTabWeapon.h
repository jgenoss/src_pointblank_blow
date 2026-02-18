#pragma once

#include "ItemTabBase.h"
#include "DlgWeaponEditor.h"

class cItemTabWeapon : public cItemTabBase
{
public:
	I3_CLASS_DEFINE(cItemTabWeapon, cItemTabBase);

	cItemTabWeapon(void);
	virtual ~cItemTabWeapon(void);
private:
	cDlgWeaponEditor* m_pDlgWeapon;
};
