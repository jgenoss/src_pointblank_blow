#include "pch.h"
#include "UpdateMapSector.h"
#include "StageBattle.h"
#include "BattleHud.h"

#include "UI/UIHudManager.h"
#include "UI/UIHudMinimap.h"
#include "BattleSlotContext.h"


void update_map_sector::ProcessCreateStage()
{
	m_OldWorldSectionMask = 0xFFFFFFFFFFFFFFFF;
	m_OldWorldSector = -1;
}

void update_map_sector::call(CStageBattle* sb)
{
	update_map_sector::_UpdateWorldSection(sb);			//?
	update_map_sector::_UpdateMapSector(sb);
}


void	update_map_sector::_UpdateWorldSection(CStageBattle* sb)
{

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( pPlayer == nullptr)
		return;

	if( m_OldWorldSectionMask == pPlayer->getPortalMask())
	{
		return;
	}

	m_OldWorldSectionMask = pPlayer->getPortalMask();

	g_pFramework->SetCurrentPortalMask( pPlayer->getPortalMask());

	// Character의 갱신
	for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		i3GameObj * pChara = g_pCharaManager->getChara( i);

		if( pChara == nullptr)
			continue;

		pChara->OnUpdateWorldSectionMask();
	}

	// Object의 갱신
	StageObjectListPtr	pObjectList = sb->GetStageObjectList();

	if( pObjectList != nullptr)
	{
		for(size_t i = 0; i < pObjectList->size(); i++)
		{
			i3GameObj * pObj = (*pObjectList)[i];

			if( pObj == nullptr)
				continue;

			pObj->OnUpdateWorldSectionMask();
		}
	}
	
}

void	update_map_sector::_UpdateMapSector(CStageBattle* sb)
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
		return;
#endif

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	
	if( pPlayer == nullptr)
		return;

	if( m_OldWorldSector == pPlayer->getPortalID())
	{
		return;
	}

	g_pFramework->setCurrentPortal( pPlayer->getPortalID());

	m_OldWorldSector = pPlayer->getPortalID();

	i3World * pWorld = sb->getWorld();
	if( pWorld == nullptr)
		return;

	INT32 floor = 0;

	i3MapSectorTable * pTable = pWorld->getMapSectorTable();
	if( pTable != nullptr)
	{
		INT32 idNew = pPlayer->getPortalID();

		I3ASSERT( (idNew >= 0) && (idNew < pTable->getCount()));

		i3::pack::MAP_SECTOR_INFO * pSector = pTable->getMapSector( idNew);
		I3ASSERT( pSector != nullptr);

		floor = pSector->m_Floor;

		if( HUD::instance() != nullptr)
		{
			if( i3::generic_string_size( pSector->m_szName) > 0 && idNew != 0)
			{
				UIHudMinimap* pMinmap = static_cast<UIHudMinimap*>( HUD::instance()->GetHud(HUD::MINIMAP) );
				pMinmap->SetMiniMapLocationText( pSector->m_szName);
			}
			else
			{
				UIHudMinimap* pMinmap = static_cast<UIHudMinimap*>( HUD::instance()->GetHud(HUD::MINIMAP) );
				pMinmap->SetMiniMapLocationText("");
			}
		}
	}

	if( CHud::i() != nullptr)
		CHud::i()->SetMiniMapFloorIndex( floor);
}