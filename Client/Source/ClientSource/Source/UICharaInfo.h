#pragma once

struct DesigApplyInfo;

class UI_CHARA_INFO
{
private:
	CHARA_PARTS_INFO			_CharacterParts[TEAM_COUNT];
	ITEM_INFO					_WeaponList[WEAPON_SLOT_COUNT];
	COMMON_PARTS_INFO			_CommonParts;
public:
	UI_CHARA_INFO();
	~UI_CHARA_INFO();
	
	void	Reset( void);
	void	CopyTo( UI_CHARA_INFO * pDest);

	// 아...좀.....이런 디폴트 매개변수 쓰지 말라고..좀...
	const CHARA_PARTS_INFO* getCharaParts( INT Team = TEAM_RED ) const								  { return &_CharacterParts[Team];			}
	CHARA_PARTS_INFO *		setCharaParts( INT Team = TEAM_RED )									  { return &_CharacterParts[Team];			}
	void					setCharaParts( const CHARA_PARTS_INFO * pInfo, INT Team = TEAM_RED )	  { pInfo->CopyTo( &_CharacterParts[Team]); }
	void					setCharaParts( const CHARA_PARTS_INFO_OTHER * pInfo, INT Team = TEAM_RED) { pInfo->CopyTo( &_CharacterParts[Team]); }
	
	void setWeaponItemID( WEAPON_SLOT_TYPE iSlot, T_ItemID ItemID, T_ItemDBIdx ItemIdx = -1, const char * pszFile = nullptr, INT32 line = 0 );
	T_ItemID getWeaponItemID( WEAPON_SLOT_TYPE iSlot) const
	{
		if (iSlot < 0 || iSlot >= WEAPON_SLOT_COUNT)
		{
			I3_BOUNDCHK(iSlot, WEAPON_SLOT_COUNT);
			return 0;				// 나도 모르것다!
		}
		return _WeaponList[ iSlot].m_TItemID;
	}

	T_ItemDBIdx getWeaponItemDBIdx( WEAPON_SLOT_TYPE iSlot) const
	{
		I3_BOUNDCHK( iSlot, WEAPON_SLOT_COUNT);
		return _WeaponList[ iSlot].m_TItemDBIdx;
	}	

	void setWeapon( WEAPON_SLOT_TYPE idx, ITEM_INFO * pInfo)		{ pInfo->CopyTo( &_WeaponList[ idx]);}
	const ITEM_INFO* getWeaponList( INT32 idx) const				{ return &_WeaponList[ idx];}
	
	void SetWeaponList( const ITEM_INFO * pWeaponList)
	{
		for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
			((ITEM_INFO*)pWeaponList)[i].CopyTo( &_WeaponList[i]);
	}

	void	SetWeaponList( const CHARA_WEAPON_INFO_OTHER * pInfo)
	{
		for( INT32 i = 0; i < WEAPON_SLOT_COUNT - 1; i++)
			setWeaponItemID( (WEAPON_SLOT_TYPE) i, pInfo->m_Info[i]);
	}

	void SetWeaponList( const CHARA_WEAPON_INFO * pInfo)
	{
		i3mem::Copy( _WeaponList, pInfo->m_Info, sizeof( ITEM_INFO ) * CHAR_EQUIPMENT_WEAPON_COUNT );
	}

	void GetWeaponList( CHARA_WEAPON_INFO * pDest)
	{
		i3mem::Copy( pDest->m_Info, _WeaponList,  sizeof( ITEM_INFO ) * CHAR_EQUIPMENT_WEAPON_COUNT );
	}

	const COMMON_PARTS_INFO * getCommonParts( void) const					{ return &_CommonParts; }
	void					setCommonParts( COMMON_PARTS_INFO * pInfo)		{ pInfo->CopyTo( &_CommonParts); }
	void					setCommonParts( COMMON_PARTS_INFO_OTHER * pInfo){ pInfo->CopyTo( &_CommonParts); }
};