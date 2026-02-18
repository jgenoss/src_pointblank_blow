#include "pch.h"
#include "Weapon.h"

CWeapon::CWeapon()
{
	Reset();
}

CWeapon::~CWeapon()
{
}

void CWeapon::Reset()
{
	m_TItemID = 0;
	i3mem::FillZero( m_ui16Bullets, sizeof(m_ui16Bullets) );
}

void CWeapon::CopyTo( CWeapon *pDest )
{
	pDest->m_eSlotType = m_eSlotType;
	pDest->m_TItemID = m_TItemID;
	i3mem::Copy( pDest->m_ui16Bullets, m_ui16Bullets, sizeof(m_ui16Bullets) );
}

void CWeapon::SetWeapon( T_ItemID TItemID, UINT8 ui32Usage )
{
	m_TItemID	= TItemID;
	m_eSlotType	= static_cast<WEAPON_SLOT_TYPE>(ui32Usage);
}

void CWeapon::SetBullets( REAL32* pr32PartsAbility, UINT32 ui32SlotItems )
{
	WeaponInfo* pWeaponInfo = Table::GetWeaponInfo( m_TItemID );
	if( NULL == pWeaponInfo  )					return;

	REAL32	r32Ability		= 1.0001f;
	UINT16 ui16AddBullet	= 0;

	// 기능성 아이템 처리
	switch( m_eSlotType )
	{
	case WEAPON_SLOT_PRIMARY:
		{
			// 기능성 아이템
			if( SLOT_ITEM_AMMO_UP & ui32SlotItems )						// 주무기만 적용 확인.
			{
				r32Ability += 0.400001f;
			}
			// 파츠
			if( pr32PartsAbility )
			{
				r32Ability += pr32PartsAbility[ EQUIP::eABILITY_MAXBULLET ];
			}
		}
		break;
	case WEAPON_SLOT_THROWING1:
		{
			if( SLOT_ITEM_INCREASE_GRENADE_SLOT & ui32SlotItems )
			{
				ui16AddBullet = pWeaponInfo->m_ui8IncreaseCount;
			}
		}
		break;
	case WEAPON_SLOT_THROWING2:
		{
			if (SLOT_ITEM_INCREASE_THR2_SLOT & ui32SlotItems)
			{
				ui16AddBullet = pWeaponInfo->m_ui8IncreaseCount;
			}
		}
		break;
	}

	UINT16		ui16MaxBullet;
	WeaponInfo* pExtWeapon;
	for( INT32 i = 0 ; i < WEAPON::EXTENSION_MAX ; i++ )
	{
		for( INT32 j = 0 ; j < WEAPON::HAND_TYPE_MAX ; j++ )
		{
			m_ui16Bullets[i][j] = 0;

			pExtWeapon = pWeaponInfo->GetExtInfo( i, (WEAPON::HAND_TYPE)j );
			if( NULL == pExtWeapon )	continue;

			if( 0 != pExtWeapon->m_ui16MaxBullet )		ui16MaxBullet = pExtWeapon->m_ui16MaxBullet;
			else										ui16MaxBullet = pExtWeapon->m_ui16LoadBullet;

			// 무한탄창 예외 처리
			if( 0xFFFF == ui16MaxBullet )
			{
				r32Ability = 1.f;
			}
			
			m_ui16Bullets[i][j] = static_cast<UINT16>(static_cast<REAL32>(ui16MaxBullet) * r32Ability) + ui16AddBullet;
		}
	}
}

void CWeapon::SetBullets( GAMEINFO_BULLET* pBullet )
{
	for( INT32 i = 0 ; i < WEAPON::EXTENSION_MAX ; i++ )
	{
		for( INT32 j = 0 ; j < WEAPON::HAND_TYPE_MAX ; j++ )
		{
			m_ui16Bullets[i][j] = pBullet->GetMaxBulletCount( (WEAPON::EXTENSION_TYPE)i, (WEAPON::HAND_TYPE)j );
		}
	}
}

BOOL CWeapon::IsDecreaseBullet(const GAMEINFO_CHARA_WEAPON* pWeapon, UINT16 ui16Dec, INT32 &i32BulletExtType)
{
	if (pWeapon->getWeaponID() != m_TItemID)
	{
		if (!pWeapon->getWeaponExtension())
		{
			//DSERROR("[ERR] CWeapon::IsDecreaseBullet : Weapon1(%d)\n", pWeapon->getWeaponID());
			return FALSE;
		}

		WeaponInfo* pBaseWeapon = Table::GetWeaponInfo(m_TItemID);
		if (pBaseWeapon)
		{
			WeaponInfo* pExtWeapon = pBaseWeapon->GetExtInfo(TRUE, pWeapon->GetAttackType());
			if (pExtWeapon && pExtWeapon->IsValid())
			{
				if (pWeapon->getWeaponID() != pExtWeapon->m_TItemID)
				{	// 들고있는 무기와 다릅니다.
					//DSERROR("[ERR] CWeapon::IsDecreaseBullet : Weapon(%d),ExtWaepon(%d)\n", pWeapon->getWeaponID(), pExtWeapon->m_TItemID);
					return FALSE;
				}
			}
		}
		else
		{
			DSERROR("[ERR] CWeapon::IsDecreaseBullet : Weapon2(%d)\n", pWeapon->getWeaponID());
			return FALSE;
		}
	}

	INT32 i32Ext = pWeapon->getWeaponExtension() == false ? 0 : 1;
	
	if (TRUE == IsExceptionBulletType(i32BulletExtType)) i32Ext = FALSE;
	
	UINT16* pui16Bullets = &m_ui16Bullets[i32Ext][pWeapon->getFireOrder()];
	if (0xFFFF == *pui16Bullets)
	{	// 무한탄창 무기		
		return TRUE;
	}
	else if (ui16Dec > *pui16Bullets)
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CWeapon::DecreaseBullet(const GAMEINFO_CHARA_WEAPON* pWeapon, UINT32 ui32Dec)
{
	bool bExtension = pWeapon->getWeaponExtension();
	UINT32 ui32BulletDec = GET_BULLET_DECREASE_COUNT(ui32Dec);

	if (TRUE == IsExceptionBulletType(ui32Dec)) bExtension = FALSE;	

	UINT16* pui16Bullets = &m_ui16Bullets[bExtension][pWeapon->getFireOrder()];
	if (0xFFFF == *pui16Bullets)
	{	// 무한탄창 무기
		return TRUE;
	}
	else if (ui32BulletDec > *pui16Bullets)
	{
		return FALSE;
	}

	*pui16Bullets -= ui32BulletDec;

	return TRUE;
}

bool CWeapon::IsExceptionBulletType(INT32 i32BulletType)
{
	for (INT32 i = 0; i < WEAPON_EXCEPTION_TYPE_COUNT; i++)
	{
		if (!GET_BULLET_EXCEPTION_TYPE(i32BulletType, i))continue;
		return TRUE;
	}
	return FALSE;
}

CWeaponManager::CWeaponManager()
{
}

CWeaponManager::~CWeaponManager()
{
}