#include "pch.h"
#include "DropItemHp.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CDropItemHp);

/*virtual*/ bool CDropItemHp::CheckForItemPickUp( REAL32 fDeltaSeconds)
{
	if( CGameDropItem::CheckForItemPickUp( fDeltaSeconds) == false) return false;

	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	if( myPlayer->getFullHP() <= myPlayer->getCurHP())
	{
//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		if( m_debug_time > 0) m_debug_time -= fDeltaSeconds; 
		if( m_debug_time <= 0)
		{
			g_pFramework->PutSystemChatting( GAME_RCSTRING( "STR_DOMI_OBJECT_2"));
			m_debug_time = 3.5f;
		}
//#endif
		return false;
	}

	return true;
}

/*virtual*/ void CDropItemHp::process_authentication( CGameCharaBase* p)
{
	I3ASSERT( p != nullptr);
	if( p)
	{
		pick_effect( p);
	}
}