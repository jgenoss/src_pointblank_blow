#ifndef __WEAPONSLOT_H__
#define __WEAPONSLOT_H__

#include "WeaponSlotBase.h"
#include "Weapon.h"

// 상속 받은 함수는 Instance()를 통해 접근해야 함
class WeaponSlot : public WeaponSlotBase
{
	CWeapon m_weapon;
	UINT8 m_ui8Usage;
public:
	WeaponSlot() : m_ui8Usage(0), m_weapon() { }
	virtual ~WeaponSlot() { }
	virtual void InitWeapon( T_ItemID TItemID, UINT8 ui32Usage );
	virtual INT8 Add( CWeapon *pWeapon );
	virtual void Drop();
	virtual void Drop( UINT8 ui8DropIdx );
	virtual CWeapon* GetOwn(UINT8 ui8SlotIdx);
	virtual CWeapon *GetEquip();
	virtual bool IsFullSlot();
	virtual UINT8 GetEquipIdx();
	virtual void SetEquip( UINT8 ui8EquipIdx );
	bool IsEmpty( );
	virtual void Reset();
};

#endif