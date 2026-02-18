#ifndef __WEAPONSLOTBASE_H__
#define __WEAPONSLOTBASE_H__

class CWeapon;

class WeaponSlotBase
{
public:
	WeaponSlotBase(){ }
	virtual ~WeaponSlotBase() { }
	virtual void InitWeapon( T_ItemID TItemID, UINT8 ui32Usage ) = 0;
	virtual INT8 Add( CWeapon *pWeapon ) = 0;
	virtual void Drop() = 0;
	virtual void Drop( UINT8 ui8DropIdx ) = 0;
	virtual CWeapon* GetOwn(UINT8 ui8SlotIdx) = 0;
	virtual CWeapon* GetEquip() = 0;
	virtual UINT8 GetEquipIdx() = 0;
	virtual void SetEquip( UINT8 ui8EquipIdx ) = 0;
	virtual bool IsFullSlot() = 0;
	virtual void Reset() { }
};

#endif
