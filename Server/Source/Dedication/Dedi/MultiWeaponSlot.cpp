#include "pch.h"
#include "WeaponSlotBase.h"
#include "Weapon.h"
#include "MultiWeaponSlot.h"

void MultiWeaponSlot::Init(UINT8 maxCount )
{
	m_ui8MaxCount = MINMAX(MULTI_WEAPON_SLOT_MIN, maxCount, MULTI_WEAPON_SLOT_MAX);
}
// 리스폰시에만 호출
void MultiWeaponSlot::InitWeapon( T_ItemID TItemID, UINT8 ui32Usage )
{
	Reset();
	m_ui8Count = (TItemID == 0) ? 0 : 1;
	m_weapon[m_ui8EquipIdx].SetWeapon(TItemID, ui32Usage);
}

bool MultiWeaponSlot::IsEmpty(UINT8 Idx)
{
	return (m_weapon[Idx].GetWeaponID() == 0);
}

void MultiWeaponSlot::SetInsertIdx()
{
	// 무조건 뒤에 추가
	UINT8 ui8NewInsert = m_ui8InsertIdx + 1;

	for( UINT8 i = 0; i < m_ui8MaxCount; i++ )
	{
		if( ui8NewInsert >= m_ui8MaxCount )  ui8NewInsert = 0;
		if( IsEmpty(ui8NewInsert) )
		{
			m_ui8InsertIdx = ui8NewInsert;
			return;
		}
		ui8NewInsert++;
	}
}

INT8 MultiWeaponSlot::Add( CWeapon *pWeapon )
{
	if( m_ui8Count < m_ui8MaxCount )
	{
		m_ui8Count++;
		SetInsertIdx();
		pWeapon->CopyTo( &m_weapon[m_ui8InsertIdx] );
		m_ui8EquipIdx = m_ui8InsertIdx;
		return m_ui8EquipIdx;
	}
	return -1;
}

CWeapon*  MultiWeaponSlot::GetOwn(UINT8 ui8SlotIdx)
{
	if( ui8SlotIdx < m_ui8MaxCount )
		return &m_weapon[ui8SlotIdx];
	return NULL;
}

CWeapon *MultiWeaponSlot::GetEquip()
{
	return &m_weapon[m_ui8EquipIdx];
}

bool MultiWeaponSlot::IsFullSlot()
{
	return (m_ui8Count == m_ui8MaxCount);
}

UINT8 MultiWeaponSlot::GetEquipIdx()
{
	return m_ui8EquipIdx;
}

void MultiWeaponSlot::SetEquip( UINT8 ui8EquipIdx )
{
	if( ui8EquipIdx < m_ui8MaxCount )
	{
		m_ui8EquipIdx = ui8EquipIdx;
	}
}

void MultiWeaponSlot::Drop()
{
	Drop( m_ui8EquipIdx );
}

void MultiWeaponSlot::Drop(UINT8 ui8DropIdx)
{
	if( m_ui8Count > 0 && ui8DropIdx < m_ui8MaxCount )
	{
		m_ui8Count--;
		m_weapon[ui8DropIdx].Reset();
	}
}
void MultiWeaponSlot::Reset()
{
	m_ui8Count = 0;
	m_ui8EquipIdx = 0;
	m_ui8InsertIdx = 0;
	for( INT32 i = 0; i < m_ui8MaxCount; i++ )
	{
		m_weapon[i].Reset();
	}
}