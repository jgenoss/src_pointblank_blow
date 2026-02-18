#include "pch.h"
#include "WeaponSlot.h"

void WeaponSlot::InitWeapon( T_ItemID TItemID, UINT8 ui32Usage )
{
	Reset();
	m_weapon.SetWeapon(TItemID, ui32Usage);
}

bool WeaponSlot::IsEmpty( )
{
	return (m_weapon.GetWeaponID() == 0);
}

INT8 WeaponSlot::Add( CWeapon *pWeapon )
{
	if (IsEmpty( ) == true)
	{
		pWeapon->CopyTo( &m_weapon );
		return 0;
	}
	return -1;
}

CWeapon*  WeaponSlot::GetOwn(UINT8 ui8SlotIdx)
{
	if( ui8SlotIdx == 0)
		return &m_weapon;
	return NULL;
}

CWeapon *WeaponSlot::GetEquip()
{
	return &m_weapon;
}

bool WeaponSlot::IsFullSlot()
{
	return !IsEmpty();
}

UINT8 WeaponSlot::GetEquipIdx()
{
	return 0;
}

void WeaponSlot::SetEquip( UINT8 ui8EquipIdx )
{
}

void WeaponSlot::Drop()
{
	Reset();
}

void WeaponSlot::Drop( UINT8 ui8DropIdx )
{
	if( ui8DropIdx == 0)
		Reset();
}

void WeaponSlot::Reset()
{
	m_weapon.Reset();
}