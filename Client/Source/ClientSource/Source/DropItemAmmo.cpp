#include "pch.h"
#include "DropItemAmmo.h"
#include "GameCharaWeaponContext.h"

#include "Mode/Domination/DomiUserSkillMgr.h"
#include "Mode/Domination/DomiSkillObjBulletinBoard.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CDropItemAmmo);

/*virtual*/ bool CDropItemAmmo::CheckForItemPickUp( REAL32 fDeltaSeconds)
{
	if( CGameDropItem::CheckForItemPickUp( fDeltaSeconds) == false) return false;

	CGameCharaBase* p = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( p!= nullptr);

	bool bPickEnable = false;

	if( p)
	{
		WeaponBase* primary = p->getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY);
		WeaponBase* secondary = p->getWeaponContext()->getWeapon( WEAPON_SLOT_SECONDARY);

		bPickEnable = (SkillObjBulletinBoardUtil::NeedToSupplyAmmo(primary, p) || 
			SkillObjBulletinBoardUtil::NeedToSupplyAmmo(secondary, p));
		if( bPickEnable == false )
		{
			//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			if( m_debug_time > 0) m_debug_time -= fDeltaSeconds; 
			if( m_debug_time <= 0)
			{
				g_pFramework->PutSystemChatting( GAME_RCSTRING( "STR_DOMI_OBJECT_1") );
				m_debug_time = 3.5f;
			}
			//#endif
		}
	}

	return bPickEnable;
}

/*virtual*/ void CDropItemAmmo::process_authentication( CGameCharaBase* p)
{
	I3ASSERT( p != nullptr);
	if( p)
	{
		{
			WeaponBase* weapon = p->getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY);
			if(weapon != nullptr)
				weapon->ApplyBulletWithDropItem( 0.25f);
		}

		{
			WeaponBase* weapon = p->getWeaponContext()->getWeapon( WEAPON_SLOT_SECONDARY);
			if(weapon != nullptr)
				weapon->ApplyBulletWithDropItem( 0.25f);
		}
		pick_effect( p);
	}
}