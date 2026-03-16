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

// Singleton shop catalog manager
// Loaded from DataServer on connect, used by GameSessionShop
class ShopManager
{
public:
	ShopManager();
	~ShopManager();

	// Load items from DataServer response
	void			LoadFromDataServer(IS_SHOP_ITEM_ENTRY* pEntries, int i32Count);

	// Queries
	int				GetItemCount() const			{ return m_i32ItemCount; }
	const ShopItem*	GetItem(int i32Index) const;
	const ShopItem*	FindByGoodsId(uint32_t ui32GoodsId) const;
	bool			IsLoaded() const				{ return m_bLoaded; }

	// Access all items (for sending to client)
	const ShopItem*	GetItems() const				{ return m_Items; }

private:
	ShopItem		m_Items[MAX_SHOP_ITEMS];
	int				m_i32ItemCount;
	bool			m_bLoaded;
};

extern ShopManager* g_pShopManager;

#endif // __SHOPMANAGER_H__
