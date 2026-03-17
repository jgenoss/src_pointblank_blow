#ifndef __SHOPMANAGER_H__
#define __SHOPMANAGER_H__

#pragma once
#include "InterServerProtocol.h"

// Max shop items from DataServer
#define MAX_SHOP_ITEMS	256

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

private:
	// Double-buffered: m_Items[0] and m_Items[1]
	ShopItem		m_Items[2][MAX_SHOP_ITEMS];
	int				m_i32ItemCount[2];
	volatile int	m_i32Side;		// Active read side (0 or 1)
	bool			m_bLoaded;
	uint32_t		m_ui32Version;
};

extern ShopManager* g_pShopManager;

#endif // __SHOPMANAGER_H__
