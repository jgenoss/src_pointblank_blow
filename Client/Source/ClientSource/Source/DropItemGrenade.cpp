#include "pch.h"
#include "DropItemGrenade.h"
#include "GameCharaWeaponContext.h"
#include "BattleSlotContext.h"
namespace
{
	bool check_weapon( WeaponBase* weapon)
	{
		if (weapon == nullptr)
			return false;
		
		INT32 max = 0;
		if( i3::same_of<WeaponBombTrigger*>(weapon))
			max = 1;
		else
			max = weapon->GetMaxBulletWithCashItem();

		INT32 cur = weapon->getLoadedBulletCount();
		return (cur < max);

	}
}

I3_CLASS_INSTANCE( CDropItemGrenade);

/*virtual*/ bool CDropItemGrenade::CheckForItemPickUp( REAL32 fDeltaSeconds)
{
	if( CGameDropItem::CheckForItemPickUp( fDeltaSeconds) == false) return false;

	CGameCharaBase* p = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( p!= nullptr);

	if( p)
	{
		WeaponBase* throwing1 = p->getWeaponContext()->getWeapon( WEAPON_SLOT_THROWING1);
		WeaponBase* throwing2 = p->getWeaponContext()->getWeapon( WEAPON_SLOT_THROWING2);

		if( check_weapon( throwing1) == false && check_weapon( throwing2) == false)
		{

//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			if( m_debug_time > 0) m_debug_time -= fDeltaSeconds; 
			if( m_debug_time <= 0)
			{
				g_pFramework->PutSystemChatting(  GAME_RCSTRING( "STR_DOMI_OBJECT_3") );
				m_debug_time = 3.5f;
			}
//#endif
			return false;
		}
	}

	return true;
}

/*virtual*/ void CDropItemGrenade::process_authentication( CGameCharaBase* p)
{
	I3ASSERT( p != nullptr);
	if( p)
	{
		CGameCharaWeaponContext* context = p->getWeaponContext();
		I3ASSERT( context != nullptr);

		//* C5는 채워 지지 않고 있음!
		{
			WeaponBase* weapon = context->getWeapon( WEAPON_SLOT_THROWING1);
			if( weapon)
				weapon->ApplyBulletWithDropItem( 1.f);
		}

		{
			WeaponBase* weapon = context->getWeapon( WEAPON_SLOT_THROWING2);
			if( weapon)
				weapon->ApplyBulletWithDropItem( 1.f);
		}
		pick_effect( p);
	}
}