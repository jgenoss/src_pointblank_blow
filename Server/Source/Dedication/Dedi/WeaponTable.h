#pragma once

/////////////////////////////////////////////////////////////////////////////////
//
// WeaponTable.h: Define Weapon Damage Information type and container class
//
/////////////////////////////////////////////////////////////////////////////////


#include <map>
#include <baseTsd.h>
#include <string>

#include "WeaponInfo.h"

//////////////////////////////////////////////////////////////////////////////////
//
// Weapon Table manager class
//
//////////////////////////////////////////////////////////////////////////////////

struct IDispatch;

class WeaponTable
{
	static WeaponTable m_Instance;

	// National code on config file
	UINT8 m_ui8NationalCode;

	std::map<UINT32, WeaponInfo>  m_mapWeaponTable;

	std::string m_strNationalColumnName;

	WeaponInfo*	m_pWeaponTable[WEAPON_CLASS_COUNT];
	UINT32		m_ui32WeaponTableCount[WEAPON_CLASS_COUNT];

public:
	// Constructor / Destructor
	WeaponTable(UINT8 ui8NationalCode = 1); 
	~WeaponTable(void);

public:
	// Get singleton Instance
	static WeaponTable&		GetInstance()			{ return m_Instance; }
	WeaponInfo*				GetWeaponInfo( T_ItemID TItemID )
	{
		UINT32 ui32Usage	= GET_ITEM_TYPE( TItemID );
		if(MAX_ITEM_TYPE_COUNT <= ui32Usage )								return NULL;
		UINT32 ui32Class	= GET_ITEM_SUBTYPE( TItemID );
		if( WEAPON_CLASS_COUNT <= ui32Class )								return FALSE;
		UINT32 ui32Number	= GET_ITEM_NUMBER( TItemID );	
		if( ui32Number >= m_ui32WeaponTableCount[ ui32Class ] )				return FALSE;

		if( false == m_pWeaponTable[ ui32Class ][ ui32Number ].IsValid() )	return FALSE;
		
		return &m_pWeaponTable[ui32Class][ui32Number];
	}
	UINT32					GetMaxWeaponNumber( INT32 ui32Class ) const
	{
		if( ui32Class < WEAPON_CLASS_COUNT  )
		{
			return m_ui32WeaponTableCount[ui32Class];
		}
		return 0;
	}

public:
	// external interface
	// Find a damage information using key value
	bool Find(const UINT32 ui32Key, WeaponInfo& Info);

	// Parse xml file to load weapon damage value
	void ParseWeaponTable(IDispatch *pNode);
	void ParseWeaponTable_New();

	// Set national Code
	void SetNationalCode(UINT8 ui8NationalCode) { m_ui8NationalCode = ui8NationalCode; 
	SetNationalColumnName(ui8NationalCode); } 

private:
	// internal function
	// Create a Unique Key

	// Insert a weapon information to the map
	bool Insert(UINT32 ui32Key, WeaponInfo pInfo);

	void SetNationalColumnName(UINT8 ui8NationalCode);
};

WEAPON::EXT_TYPE GetExtensionType( const char * strVal );

// -----------------------------------------------
// get weapon info.
// -----------------------------------------------

namespace Table
{
	inline 	WeaponInfo* GetWeaponInfo( T_ItemID TItemID )
	{
		return WeaponTable::GetInstance().GetWeaponInfo( TItemID );
	}
	
	// 발사 무기의 ItemID를 얻는다.
	inline	T_ItemID GetExtWeaponID(UINT32 ui32ItemID )
	{
		UINT8 ui8ExtIdx = (UINT8)GET_ITEM_EXT_STATE(ui32ItemID);
		if( DS_MAX_WEAPON_REGISTER <= ui8ExtIdx ) return 0;

		WeaponInfo * pInfo = GetWeaponInfo(ui32ItemID);
		if( NULL == pInfo ) return 0;
		return pInfo->m_RegWeaponInfo[ui8ExtIdx].m_TItemID;
	}

	// 발사 무기의 Extension Type을 얻는다.
	inline	WEAPON::EXT_TYPE GetWeaponExtType(UINT32 ui32ItemID)
	{
		UINT8 ui8ExtIdx = (UINT8)GET_ITEM_EXT_STATE(ui32ItemID);
		if( DS_MAX_WEAPON_REGISTER <= ui8ExtIdx ) return WEAPON::EXT_NONE;

		WeaponInfo * pInfo = GetWeaponInfo(ui32ItemID);
		if( NULL == pInfo ) return WEAPON::EXT_NONE;
		return pInfo->m_RegWeaponInfo[ui8ExtIdx].m_aExtType;
	}

	inline	BOOL IsThrowWeapon( UINT32 ui32ItemID, BOOL bExtType, WEAPON::ATTACK_TYPE eAttackType )
	{
		WeaponInfo * pInfo = GetWeaponInfo(ui32ItemID);
		if( NULL == pInfo )																	return FALSE;

		if( WEAPON::BULLET_SHOT_DROPPING == pInfo->GetBulletType( bExtType, eAttackType ) )	return TRUE;

		return FALSE;
	}
	
	inline	BOOL IsThrowWeapon( N_PCINFO_HIT_WEAPON* pWeapon )
	{
		WeaponInfo * pInfo = GetWeaponInfo( pWeapon->GetWeaponID() );
		if( NULL == pInfo )																	return FALSE;

		if( WEAPON::BULLET_SHOT_DROPPING == pInfo->GetBulletType( pWeapon->IsExtension(), static_cast<WEAPON::ATTACK_TYPE>(pWeapon->GetAttackType()) ) )	return TRUE;

		return FALSE;
	}

	// 똑같은 함수가 세개인 이유는 위에 두개 함수를 제거할 예정이기 때문입니다. 20150423 김현우
	inline	BOOL IsThrowWeapon( const GAMEINFO_CHARA_WEAPON* pWeapon )
	{
		WeaponInfo * pInfo = GetWeaponInfo( pWeapon->getWeaponID() );
		if( NULL == pInfo )																										return FALSE;

		if( WEAPON::BULLET_SHOT_DROPPING == pInfo->GetBulletType( pWeapon->getWeaponExtension(), pWeapon->GetAttackType() ) )	return TRUE;

		return FALSE;
	}

	inline	REAL32 GetWeaponRange( N_PCINFO_HIT_WEAPON* pWeapon )
	{
		WeaponInfo * pInfo = GetWeaponInfo( pWeapon->GetWeaponID() );
		if( NULL == pInfo )																	return 0.f;

		return pInfo->GetWeaponRange( pWeapon->IsExtension(), static_cast<WEAPON::ATTACK_TYPE>(pWeapon->GetAttackType()) );
	}

	// 발사 무기 정보를 얻는다.
	inline WeaponInfo * GetExtWeaponInfo(UINT32 ui32ItemID)
	{
		T_ItemID FireWeaponID = GetExtWeaponID(ui32ItemID);
		return FireWeaponID == 0 ? NULL : GetWeaponInfo(FireWeaponID);
	}
	//예외처리가 필요한 무기의 익스텐션 정보를 얻는다.
	inline INT32 GetWeaponExtType(WeaponInfo * pBaseWeapon)
	{
		if (!pBaseWeapon) return 0;
		INT32 i32ExtType = 0;

		for (INT32 i = 0; i < DS_MAX_WEAPON_EXTENSION; i++)
		{
			switch (pBaseWeapon->m_ExtInfo[i].m_aExtType)
			{
			case WEAPON::EXT_SILENCER:
				i32ExtType = SET_BULLET_EXCEPTION_TYPE(i32ExtType, WEAPON_EXCEPTION_TYPE_SILENCE);
				break;
			case WEAPON::EXT_BAYONET:
				i32ExtType = SET_BULLET_EXCEPTION_TYPE(i32ExtType, WEAPON_EXCEPTION_TYPE_BAYONET);
				break;
			case WEAPON::EXT_CROSSFIRE:
				i32ExtType = SET_BULLET_EXCEPTION_TYPE(i32ExtType, WEAPON_EXCEPTION_TYPE_CROSSFIRE);
			default:
				break;
			}		
		}
		return i32ExtType;
	}
}