#include "pch.h"
#include "ValidCheck.h"

UI_CHARA_INFO::UI_CHARA_INFO()
{
}

UI_CHARA_INFO::~UI_CHARA_INFO()
{
}

void UI_CHARA_INFO::Reset(void)
{
	for(int i = 0 ; i < TEAM_COUNT; i++)
	{
		_CharacterParts[i].Reset();
	}

	_CommonParts.Reset();

	memset(_WeaponList, 0 , sizeof(ITEM_INFO) * WEAPON_SLOT_COUNT);			// 주석풀고 초기화 복구.(2012.09.28.수빈)
}

void UI_CHARA_INFO::CopyTo( UI_CHARA_INFO * pDest) 
{
	for(int i = 0 ; i < TEAM_COUNT; i++)
	{
		_CharacterParts[i].CopyTo( &pDest->_CharacterParts[i]);
	}

	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		pDest->_WeaponList[i]		= _WeaponList[i];
	}	

	memcpy(&pDest->_CommonParts, &_CommonParts, sizeof(COMMON_PARTS_INFO));
}

void UI_CHARA_INFO::setWeaponItemID( WEAPON_SLOT_TYPE iSlot, T_ItemID ItemID, T_ItemDBIdx ItemIdx, const char * pszFile, INT32 line)
{
#if !defined(USE_I3EXEC)
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	I3_BOUNDCHK_RETURN( iSlot, WEAPON_SLOT_COUNT);

	if( ValidCheck::Item::Weapon::isWeaponItemID( iSlot, ItemID) == false)
	{
		if( pszFile != nullptr)
		{
			i3Error::SetEnv( pszFile, line, __FUNCSIG__, I3LOG_FATAL);
			i3Error::Log( "Invalid Weapon ID %d", ItemID);
		}
		else
		{
			I3PRINTLOG(I3LOG_NOTICE,  "Invalid Weapon ID %d", ItemID);
		}
	}
#endif
#endif // end of if !defined(USE_I3EXEC)


	_WeaponList[ iSlot].m_TItemID = ItemID;

	if( ItemIdx != -1)
		_WeaponList[ iSlot].m_TItemDBIdx = ItemIdx;
}