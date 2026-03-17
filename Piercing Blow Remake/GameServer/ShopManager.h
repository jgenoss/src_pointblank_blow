#ifndef __SHOPMANAGER_H__
#define __SHOPMANAGER_H__

#pragma once
#include <ctime>
#include "InterServerProtocol.h"

// Max shop items from DataServer
#define MAX_SHOP_ITEMS	256

// Max active sale entries
#define MAX_SHOP_SALE_ITEMS	64

// A discounted item entry (server-side config, loaded at startup)
struct ShopSaleEntry
{
	uint32_t	ui32GoodsId;
	uint8_t		ui8DiscountPercent;
	uint32_t	ui32StartTime;	// Unix timestamp
	uint32_t	ui32EndTime;	// Unix timestamp (0 = no expiry)

	void Reset()
	{
		ui32GoodsId = 0;
		ui8DiscountPercent = 0;
		ui32StartTime = 0;
		ui32EndTime = 0;
	}

	bool IsActive() const
	{
		uint32_t now = (uint32_t)time(nullptr);
		if (ui32StartTime > 0 && now < ui32StartTime) return false;
		if (ui32EndTime   > 0 && now > ui32EndTime)   return false;
		return ui32GoodsId > 0;
	}
};

// Shop item stored in GameServer memory
struct ShopItem
{
	uint32_t	ui32GoodsId;
	uint32_t	ui32ItemId;
	uint8_t		ui8ItemType;
	int			i32PriceGP;
	int			i32PriceCash;
	uint32_t	ui32Duration;
	uint8_t		ui8Category;

	void Reset()
	{
		ui32GoodsId = 0;
		ui32ItemId = 0;
		ui8ItemType = 0;
		i32PriceGP = 0;
		i32PriceCash = 0;
		ui32Duration = 0;
		ui8Category = 0;
	}
};

// Double-buffered shop catalog manager
// Pattern from original CRoomManager m_InfoSide: write to back buffer, swap atomically
// Readers always see consistent data without locks
class ShopManager
{
public:
	ShopManager();
	~ShopManager();

	// Load items from DataServer response (writes to back buffer, then swaps)
	void			LoadFromDataServer(IS_SHOP_ITEM_ENTRY* pEntries, int i32Count);

	// Read-side accessors (always use front buffer via m_i32Side)
	int				GetItemCount() const			{ return m_i32ItemCount[m_i32Side]; }
	const ShopItem*	GetItem(int i32Index) const;
	const ShopItem*	FindByGoodsId(uint32_t ui32GoodsId) const;
	bool			IsLoaded() const				{ return m_bLoaded; }
	uint32_t		GetVersion() const				{ return m_ui32Version; }

	// Access all items (for sending to client) - front buffer
	const ShopItem*	GetItems() const				{ return m_Items[m_i32Side]; }

	// Sale list (single buffer, rarely updated)
	void			SetSaleList(const ShopSaleEntry* pEntries, int i32Count);
	int				GetActiveSales(ShopSaleEntry* pOut, int i32MaxCount) const;

private:
	// Double-buffered: m_Items[0] and m_Items[1]
	ShopItem		m_Items[2][MAX_SHOP_ITEMS];
	int				m_i32ItemCount[2];
	volatile int	m_i32Side;		// Active read side (0 or 1)
	bool			m_bLoaded;
	uint32_t		m_ui32Version;

	// Sale list (single buffer — updates are infrequent and non-critical)
	ShopSaleEntry	m_Sales[MAX_SHOP_SALE_ITEMS];
	int				m_i32SaleCount;
};

extern ShopManager* g_pShopManager;

#endif // __SHOPMANAGER_H__
