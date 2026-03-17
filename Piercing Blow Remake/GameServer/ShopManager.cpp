#include "pch.h"
#include "ShopManager.h"

ShopManager* g_pShopManager = nullptr;

ShopManager::ShopManager()
	: m_i32Side(0)
	, m_bLoaded(false)
	, m_ui32Version(1)
{
	m_i32ItemCount[0] = 0;
	m_i32ItemCount[1] = 0;

	for (int s = 0; s < 2; s++)
		for (int i = 0; i < MAX_SHOP_ITEMS; i++)
			m_Items[s][i].Reset();
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

	// Write to back buffer (opposite of current read side)
	int backSide = 1 - m_i32Side;

	for (int i = 0; i < i32Count; i++)
	{
		m_Items[backSide][i].ui32GoodsId = pEntries[i].ui32GoodsId;
		m_Items[backSide][i].ui32ItemId = pEntries[i].ui32ItemId;
		m_Items[backSide][i].ui8ItemType = pEntries[i].ui8ItemType;
		m_Items[backSide][i].i32PriceGP = pEntries[i].i32PriceGP;
		m_Items[backSide][i].i32PriceCash = pEntries[i].i32PriceCash;
		m_Items[backSide][i].ui32Duration = pEntries[i].ui32Duration;
		m_Items[backSide][i].ui8Category = pEntries[i].ui8Category;
	}

	// Clear remaining slots in back buffer
	for (int i = i32Count; i < MAX_SHOP_ITEMS; i++)
		m_Items[backSide][i].Reset();

	m_i32ItemCount[backSide] = i32Count;

	// Atomic swap: readers now see the new data
	m_i32Side = backSide;

	m_bLoaded = true;
	m_ui32Version++;

	printf("[ShopManager] Loaded %d items from DataServer (v%u, side=%d)\n",
		i32Count, m_ui32Version, m_i32Side);
}

const ShopItem* ShopManager::GetItem(int i32Index) const
{
	int side = m_i32Side;
	if (i32Index < 0 || i32Index >= m_i32ItemCount[side])
		return nullptr;

	return &m_Items[side][i32Index];
}

const ShopItem* ShopManager::FindByGoodsId(uint32_t ui32GoodsId) const
{
	int side = m_i32Side;
	int count = m_i32ItemCount[side];

	for (int i = 0; i < count; i++)
	{
		if (m_Items[side][i].ui32GoodsId == ui32GoodsId)
			return &m_Items[side][i];
	}
	return nullptr;
}
