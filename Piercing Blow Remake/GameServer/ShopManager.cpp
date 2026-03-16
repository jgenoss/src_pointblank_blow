#include "pch.h"
#include "ShopManager.h"

ShopManager* g_pShopManager = nullptr;

ShopManager::ShopManager()
	: m_i32ItemCount(0)
	, m_bLoaded(false)
	, m_ui32Version(1)
{
	for (int i = 0; i < MAX_SHOP_ITEMS; i++)
		m_Items[i].Reset();
}

ShopManager::~ShopManager()
{
}

void ShopManager::LoadFromDataServer(IS_SHOP_ITEM_ENTRY* pEntries, int i32Count)
{
	if (!pEntries || i32Count <= 0)
		return;

	if (i32Count > MAX_SHOP_ITEMS)
		i32Count = MAX_SHOP_ITEMS;

	m_i32ItemCount = i32Count;

	for (int i = 0; i < i32Count; i++)
	{
		m_Items[i].ui32GoodsId = pEntries[i].ui32GoodsId;
		m_Items[i].ui32ItemId = pEntries[i].ui32ItemId;
		m_Items[i].ui8ItemType = pEntries[i].ui8ItemType;
		m_Items[i].i32PriceGP = pEntries[i].i32PriceGP;
		m_Items[i].i32PriceCash = pEntries[i].i32PriceCash;
		m_Items[i].ui32Duration = pEntries[i].ui32Duration;
		m_Items[i].ui8Category = pEntries[i].ui8Category;
	}

	m_bLoaded = true;
	m_ui32Version++;

	printf("[ShopManager] Loaded %d items from DataServer (v%u)\n", m_i32ItemCount, m_ui32Version);
}

const ShopItem* ShopManager::GetItem(int i32Index) const
{
	if (i32Index < 0 || i32Index >= m_i32ItemCount)
		return nullptr;

	return &m_Items[i32Index];
}

const ShopItem* ShopManager::FindByGoodsId(uint32_t ui32GoodsId) const
{
	for (int i = 0; i < m_i32ItemCount; i++)
	{
		if (m_Items[i].ui32GoodsId == ui32GoodsId)
			return &m_Items[i];
	}
	return nullptr;
}
