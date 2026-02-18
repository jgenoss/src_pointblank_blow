#ifndef __MULTIWEAPONSLOT_H__
#define __MULTIWEAPONSLOT_H__

class WeaponSlotBase;
class CWeapon;

class MultiWeaponSlot : public WeaponSlotBase
{
	CWeapon m_weapon[MULTI_WEAPON_SLOT_MAX];
	UINT8 m_ui8Usage;
	UINT8 m_ui8Count;
	UINT8 m_ui8MaxCount;
	UINT8 m_ui8EquipIdx;
	UINT8 m_ui8InsertIdx;
public:
	MultiWeaponSlot() : m_ui8Count(0), m_ui8MaxCount(10), m_ui8EquipIdx(0), m_ui8Usage(0), m_ui8InsertIdx(0) { }
	virtual ~MultiWeaponSlot() { }
	virtual void InitWeapon( T_ItemID TItemID, UINT8 ui32Usage );
	virtual INT8 Add( CWeapon *pWeapon );
	virtual void Drop();
	virtual void Drop( UINT8 ui8DropIdx );
	virtual CWeapon* GetOwn(UINT8 ui8SlotIdx);
	virtual CWeapon *GetEquip();
	virtual bool IsFullSlot();
	virtual UINT8 GetEquipIdx();
	virtual void SetEquip( UINT8 ui8EquipIdx );
	void Init(UINT8 maxCount);
	bool IsEmpty(UINT8 insertIdx);
	virtual void Reset();
	void SetInsertIdx();
};

#endif
