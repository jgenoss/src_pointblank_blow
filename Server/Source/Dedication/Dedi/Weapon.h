#ifndef __WEAPON_H__
#define __WEAPON_H__

class CWeapon
{
	WEAPON_SLOT_TYPE		m_eSlotType;
	T_ItemID				m_TItemID;
	UINT16					m_ui16Bullets[ WEAPON::EXTENSION_MAX ][ WEAPON::HAND_TYPE_MAX ];
public:
	CWeapon();
	~CWeapon();

	void					Reset();
	void					CopyTo( CWeapon *pDest );
	void					SetWeapon( T_ItemID TItemID, UINT8 ui32Usage );
	void					SetBullets( REAL32* pr32PartsAbility, UINT32 ui32SlotItems );
	void					SetBullets( GAMEINFO_BULLET* pBullet );
		
	BOOL					IsDecreaseBullet( const GAMEINFO_CHARA_WEAPON* pWeapon, UINT16 ui16Dec, INT32 &i32BulletExtType);														// 남은 총알 유무
	BOOL					DecreaseBullet( const GAMEINFO_CHARA_WEAPON* pWeapon, UINT32 ui32Dec );															// 총알 줄이기

	T_ItemID				GetWeaponID()					{	return m_TItemID;		}
	bool					IsExceptionBulletType(INT32 i32BulletType);
};


class CWeaponManager
{
public:
	CWeaponManager();
	~CWeaponManager();
};

#endif