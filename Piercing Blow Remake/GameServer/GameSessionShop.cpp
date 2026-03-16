#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "ShopManager.h"
#include "GameSessionManager.h"

// ============================================================================
// Shop Handlers (Protocol_Shop 0x0400)
// ============================================================================

// Hardcoded fallback shop catalog (used when ShopManager has no data from DataServer)
struct ShopGoodsItem
{
	uint32_t	ui32GoodsId;
	uint32_t	ui32ItemId;
	uint8_t		ui8ItemType;
	int			i32PriceGP;
	int			i32PriceCash;
	uint32_t	ui32Duration;
};

static const ShopGoodsItem s_ShopCatalog[] =
{
	// GoodsId, ItemId, Type, GP, Cash, Duration
	// Primary weapons
	{ 1001, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, 1),   0, 5000,    0, 0 },	// M4A1
	{ 1002, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, 2),   0, 8000,    0, 0 },	// AK-47
	{ 1003, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, 3),   0, 12000,   0, 0 },	// SG550
	{ 1004, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, 4),   0, 3000,    0, 0 },	// K-2
	{ 1005, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, 1),       0, 4000,    0, 0 },	// P90
	{ 1006, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, 2),       0, 6000,    0, 0 },	// MP5
	{ 1007, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, 1),    0, 10000,   0, 0 },	// AWP
	{ 1008, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, 2),    0, 15000,   0, 0 },	// PSG-1
	{ 1009, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SHOTGUN, 1),   0, 7000,    0, 0 },	// Benelli
	{ 1010, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_MG, 1),        0, 20000,   0, 0 },	// MG36

	// Secondary weapons
	{ 2001, MAKE_ITEM_ID(ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, 1), 0, 2000,    0, 0 },	// Glock
	{ 2002, MAKE_ITEM_ID(ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, 2), 0, 3000,    0, 0 },	// Desert Eagle
	{ 2003, MAKE_ITEM_ID(ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, 3), 0, 1500,    0, 0 },	// K5

	// Melee
	{ 3001, MAKE_ITEM_ID(ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, 1),       0, 1000,    0, 0 },	// M7
	{ 3002, MAKE_ITEM_ID(ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, 2),       0, 5000,    0, 0 },	// Kukri

	// Characters
	{ 5001, MAKE_ITEM_ID(ITEM_TYPE_CHARA, 1, 1),                       0, 10000,   0, 0 },	// Character 1
	{ 5002, MAKE_ITEM_ID(ITEM_TYPE_CHARA, 1, 2),                       0, 15000,   0, 0 },	// Character 2
};

static const int SHOP_CATALOG_SIZE = sizeof(s_ShopCatalog) / sizeof(s_ShopCatalog[0]);

void GameSession::OnShopEnterReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_SHOP_ENTER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Send current GP and Cash balance
	memcpy(buffer + offset, &m_i32GP, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &m_i32Cash, sizeof(int32_t));	offset += sizeof(int32_t);

	// Use dynamic catalog from ShopManager if available, otherwise fallback to hardcoded
	if (g_pShopManager && g_pShopManager->IsLoaded())
	{
		int itemCount = g_pShopManager->GetItemCount();
		uint16_t count16 = (uint16_t)itemCount;
		memcpy(buffer + offset, &count16, sizeof(uint16_t));	offset += sizeof(uint16_t);

		const ShopItem* pItems = g_pShopManager->GetItems();
		for (int i = 0; i < itemCount; i++)
		{
			if (offset + 20 > (int)sizeof(buffer))
				break;

			memcpy(buffer + offset, &pItems[i].ui32GoodsId, 4);		offset += 4;
			memcpy(buffer + offset, &pItems[i].ui32ItemId, 4);		offset += 4;
			memcpy(buffer + offset, &pItems[i].i32PriceGP, 4);		offset += 4;
			memcpy(buffer + offset, &pItems[i].i32PriceCash, 4);	offset += 4;
			memcpy(buffer + offset, &pItems[i].ui32Duration, 4);	offset += 4;
		}
	}
	else
	{
		// Fallback to hardcoded catalog
		uint16_t itemCount = (uint16_t)SHOP_CATALOG_SIZE;
		memcpy(buffer + offset, &itemCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

		for (int i = 0; i < SHOP_CATALOG_SIZE; i++)
		{
			if (offset + 20 > (int)sizeof(buffer))
				break;

			const ShopGoodsItem& goods = s_ShopCatalog[i];
			memcpy(buffer + offset, &goods.ui32GoodsId, 4);		offset += 4;
			memcpy(buffer + offset, &goods.ui32ItemId, 4);		offset += 4;
			memcpy(buffer + offset, &goods.i32PriceGP, 4);		offset += 4;
			memcpy(buffer + offset, &goods.i32PriceCash, 4);	offset += 4;
			memcpy(buffer + offset, &goods.ui32Duration, 4);	offset += 4;
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopLeaveReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	SendSimpleAck(PROTOCOL_SHOP_LEAVE_ACK, 0);
}

void GameSession::OnShopBuyReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 5)
		return;

	uint8_t buyType = *(uint8_t*)pData;			// 0=GP, 1=Cash
	uint32_t goodsId = *(uint32_t*)(pData + 1);

	int32_t result = 0;
	uint32_t newGP = (uint32_t)m_i32GP;
	uint32_t newCash = (uint32_t)m_i32Cash;

	// Look up item in dynamic catalog first, then fallback
	uint32_t itemId = 0;
	uint8_t itemType = 0;
	int priceGP = 0;
	int priceCash = 0;
	uint32_t duration = 0;
	bool found = false;

	if (g_pShopManager && g_pShopManager->IsLoaded())
	{
		const ShopItem* pItem = g_pShopManager->FindByGoodsId(goodsId);
		if (pItem)
		{
			itemId = pItem->ui32ItemId;
			itemType = pItem->ui8ItemType;
			priceGP = pItem->i32PriceGP;
			priceCash = pItem->i32PriceCash;
			duration = pItem->ui32Duration;
			found = true;
		}
	}

	if (!found)
	{
		// Fallback to hardcoded catalog
		for (int i = 0; i < SHOP_CATALOG_SIZE; i++)
		{
			if (s_ShopCatalog[i].ui32GoodsId == goodsId)
			{
				itemId = s_ShopCatalog[i].ui32ItemId;
				itemType = s_ShopCatalog[i].ui8ItemType;
				priceGP = s_ShopCatalog[i].i32PriceGP;
				priceCash = s_ShopCatalog[i].i32PriceCash;
				duration = s_ShopCatalog[i].ui32Duration;
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		result = 1;	// Item not found
	}
	else if (m_i32InventoryCount >= MAX_INVEN_COUNT)
	{
		result = 2;	// Inventory full
	}
	else if (HasInventoryItem(itemId))
	{
		result = 3;	// Already owned
	}
	else
	{
		int price = (buyType == 0) ? priceGP : priceCash;
		if (price <= 0)
		{
			result = 4;	// Not available for this currency
		}
		else if (buyType == 0 && m_i32GP < price)
		{
			result = 5;	// Insufficient GP
		}
		else if (buyType == 1 && m_i32Cash < price)
		{
			result = 6;	// Insufficient Cash
		}
		else
		{
			// Deduct currency
			if (buyType == 0)
				m_i32GP -= price;
			else
				m_i32Cash -= price;

			// Add item to inventory
			GameInventoryItem newItem;
			newItem.ui32ItemDBIdx = (uint32_t)(GetTickCount() & 0xFFFFFF) | ((uint32_t)m_i32InventoryCount << 24);
			newItem.ui32ItemId = itemId;
			newItem.ui8ItemType = itemType;
			newItem.ui32ItemArg = duration;
			AddInventoryItem(newItem);

			newGP = (uint32_t)m_i32GP;
			newCash = (uint32_t)m_i32Cash;

			printf("[GameSession] Shop buy - UID=%lld, GoodsId=%u, ItemId=%u, BuyType=%d, Price=%d\n",
				m_i64UID, goodsId, itemId, buyType, price);
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &goodsId, 4);					offset += 4;
	memcpy(buffer + offset, &newGP, 4);						offset += 4;
	memcpy(buffer + offset, &newCash, 4);					offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopRepairReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	SendSimpleAck(PROTOCOL_SHOP_REPAIR_ACK, 0);
}

// ============================================================================
// Gift System (Phase 5B)
// ============================================================================

void GameSession::OnShopGiftReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Parse: buyType(1) + goodsId(4) + targetNickname(64)
	if (i32Size < 69)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK, 1);
		return;
	}

	uint8_t buyType = *(uint8_t*)pData;
	uint32_t goodsId = *(uint32_t*)(pData + 1);
	char targetNick[64] = {0};
	memcpy(targetNick, pData + 5, 64);
	targetNick[63] = '\0';

	int32_t result = 0;

	// Look up item in catalog
	uint32_t itemId = 0;
	uint8_t itemType = 0;
	int priceGP = 0;
	int priceCash = 0;
	uint32_t duration = 0;
	bool found = false;

	if (g_pShopManager && g_pShopManager->IsLoaded())
	{
		const ShopItem* pItem = g_pShopManager->FindByGoodsId(goodsId);
		if (pItem)
		{
			itemId = pItem->ui32ItemId;
			itemType = pItem->ui8ItemType;
			priceGP = pItem->i32PriceGP;
			priceCash = pItem->i32PriceCash;
			duration = pItem->ui32Duration;
			found = true;
		}
	}

	if (!found)
	{
		for (int i = 0; i < SHOP_CATALOG_SIZE; i++)
		{
			if (s_ShopCatalog[i].ui32GoodsId == goodsId)
			{
				itemId = s_ShopCatalog[i].ui32ItemId;
				itemType = s_ShopCatalog[i].ui8ItemType;
				priceGP = s_ShopCatalog[i].i32PriceGP;
				priceCash = s_ShopCatalog[i].i32PriceCash;
				duration = s_ShopCatalog[i].ui32Duration;
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		result = 1;		// Item not found
	}
	else if (strlen(targetNick) == 0)
	{
		result = 2;		// Invalid target
	}
	else if (strcmp(targetNick, m_szNickname) == 0)
	{
		result = 3;		// Cannot gift yourself
	}
	else
	{
		// Check currency
		int price = (buyType == 0) ? priceGP : priceCash;
		if (price <= 0)
			result = 4;	// Not available for this currency
		else if (buyType == 0 && m_i32GP < price)
			result = 5;	// Insufficient GP
		else if (buyType == 1 && m_i32Cash < price)
			result = 6;	// Insufficient Cash
		else
		{
			// Find target player online (simplified: search session manager)
			// For now, just deduct currency - actual delivery needs session lookup
			if (buyType == 0)
				m_i32GP -= price;
			else
				m_i32Cash -= price;

			// TODO: Find target session and add item to their inventory
			// For now, log the gift attempt
			printf("[GameSession] Gift - From=%s, To=%s, GoodsId=%u, Price=%d\n",
				m_szNickname, targetNick, goodsId, price);

			result = 0;	// Success (item delivery pending)
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &goodsId, 4);					offset += 4;

	uint32_t newGP = (uint32_t)m_i32GP;
	uint32_t newCash = (uint32_t)m_i32Cash;
	memcpy(buffer + offset, &newGP, 4);					offset += 4;
	memcpy(buffer + offset, &newCash, 4);					offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Item Period Extension (Phase 5B)
// ============================================================================

void GameSession::OnShopItemExtendReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Parse: buyType(1) + goodsId(4)
	if (i32Size < 5)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK, 1);
		return;
	}

	uint8_t buyType = *(uint8_t*)pData;
	uint32_t goodsId = *(uint32_t*)(pData + 1);

	int32_t result = 0;

	// Look up item to extend
	uint32_t itemId = 0;
	int priceGP = 0;
	int priceCash = 0;
	uint32_t duration = 0;
	bool found = false;

	if (g_pShopManager && g_pShopManager->IsLoaded())
	{
		const ShopItem* pItem = g_pShopManager->FindByGoodsId(goodsId);
		if (pItem)
		{
			itemId = pItem->ui32ItemId;
			priceGP = pItem->i32PriceGP;
			priceCash = pItem->i32PriceCash;
			duration = pItem->ui32Duration;
			found = true;
		}
	}

	if (!found)
	{
		for (int i = 0; i < SHOP_CATALOG_SIZE; i++)
		{
			if (s_ShopCatalog[i].ui32GoodsId == goodsId)
			{
				itemId = s_ShopCatalog[i].ui32ItemId;
				priceGP = s_ShopCatalog[i].i32PriceGP;
				priceCash = s_ShopCatalog[i].i32PriceCash;
				duration = s_ShopCatalog[i].ui32Duration;
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		result = 1;		// Item not found
	}
	else if (duration == 0)
	{
		result = 2;		// Permanent item, cannot extend
	}
	else
	{
		// Find the item in inventory
		GameInventoryItem* pOwned = FindInventoryItem(itemId);
		if (!pOwned)
		{
			result = 3;	// Don't own this item
		}
		else if (pOwned->ui8ItemType != ITEM_ATTR_PERIOD)
		{
			result = 4;	// Not a period item
		}
		else
		{
			int price = (buyType == 0) ? priceGP : priceCash;
			if (price <= 0)
				result = 5;
			else if (buyType == 0 && m_i32GP < price)
				result = 6;
			else if (buyType == 1 && m_i32Cash < price)
				result = 7;
			else
			{
				// Deduct currency
				if (buyType == 0)
					m_i32GP -= price;
				else
					m_i32Cash -= price;

				// Extend the item's expiration
				pOwned->ui32ItemArg += duration;

				printf("[GameSession] Item extend - UID=%lld, ItemId=%u, NewExpire=%u\n",
					m_i64UID, itemId, pOwned->ui32ItemArg);
			}
		}
	}

	// Reuse BUY_ACK format for response
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &goodsId, 4);					offset += 4;

	uint32_t newGP = (uint32_t)m_i32GP;
	uint32_t newCash = (uint32_t)m_i32Cash;
	memcpy(buffer + offset, &newGP, 4);					offset += 4;
	memcpy(buffer + offset, &newCash, 4);					offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Item Expiration (Phase 5B)
// ============================================================================

void GameSession::CheckExpiredItems()
{
	int removed = RemoveExpiredItems();
	if (removed > 0)
	{
		printf("[GameSession] Removed %d expired items - UID=%lld\n", removed, m_i64UID);
	}
}

int GameSession::RemoveExpiredItems()
{
	DWORD dwNow = (DWORD)time(nullptr);	// Unix timestamp
	int removedCount = 0;

	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (!m_Inventory[i].IsValid())
			continue;

		if (m_Inventory[i].ui8ItemType == ITEM_ATTR_PERIOD &&
			m_Inventory[i].ui32ItemArg > 0 &&
			m_Inventory[i].ui32ItemArg <= dwNow)
		{
			// Item has expired
			printf("[GameSession] Item expired - UID=%lld, ItemId=%u, DBIdx=%u\n",
				m_i64UID, m_Inventory[i].ui32ItemId, m_Inventory[i].ui32ItemDBIdx);

			// Shift remaining items down
			for (int j = i; j < m_i32InventoryCount - 1; j++)
				m_Inventory[j] = m_Inventory[j + 1];

			m_Inventory[m_i32InventoryCount - 1].Reset();
			m_i32InventoryCount--;
			removedCount++;
			i--;	// Recheck this index
		}
	}

	return removedCount;
}

void GameSession::OnShopVersionReq(char* pData, INT32 i32Size)
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_VERSION_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Shop version (incremented when catalog changes)
	uint32_t shopVersion = 1;
	if (g_pShopManager && g_pShopManager->IsLoaded())
		shopVersion = g_pShopManager->GetVersion();
	memcpy(buffer + offset, &shopVersion, sizeof(uint32_t));	offset += sizeof(uint32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopListReq(char* pData, INT32 i32Size)
{
	// Shop categories list
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Categories: Primary, Secondary, Melee, Grenade, Character, Parts
	uint8_t categoryCount = 6;
	memcpy(buffer + offset, &categoryCount, 1);			offset += 1;

	const char* categories[] = { "Primary", "Secondary", "Melee", "Grenade", "Character", "Parts" };
	for (int i = 0; i < categoryCount; i++)
	{
		uint8_t catId = (uint8_t)i;
		memcpy(buffer + offset, &catId, 1);					offset += 1;
		char catName[32] = {0};
		strncpy_s(catName, categories[i], _TRUNCATE);
		memcpy(buffer + offset, catName, 32);				offset += 32;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopGoodsListReq(char* pData, INT32 i32Size)
{
	// Goods list for a specific category
	uint8_t categoryId = 0;
	if (i32Size >= 1)
		categoryId = *(uint8_t*)pData;

	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_GOODSLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &categoryId, 1);				offset += 1;

	// Count items matching category
	int countPos = offset;
	uint16_t itemCount = 0;
	offset += sizeof(uint16_t);

	if (g_pShopManager && g_pShopManager->IsLoaded())
	{
		const ShopItem* pItems = g_pShopManager->GetItems();
		int totalItems = g_pShopManager->GetItemCount();
		for (int i = 0; i < totalItems && offset < 3900; i++)
		{
			if (pItems[i].ui8Category == categoryId)
			{
				memcpy(buffer + offset, &pItems[i].ui32GoodsId, 4);		offset += 4;
				memcpy(buffer + offset, &pItems[i].ui32ItemId, 4);		offset += 4;
				memcpy(buffer + offset, &pItems[i].i32PriceGP, 4);		offset += 4;
				memcpy(buffer + offset, &pItems[i].i32PriceCash, 4);	offset += 4;
				memcpy(buffer + offset, &pItems[i].ui32Duration, 4);	offset += 4;
				itemCount++;
			}
		}
	}
	else
	{
		// Fallback: filter hardcoded catalog by category
		for (int i = 0; i < SHOP_CATALOG_SIZE && offset < 3900; i++)
		{
			// Map item type to category: primary=0, secondary=1, melee=2, chara=4
			uint8_t itemCat = 0;
			uint32_t itemId = s_ShopCatalog[i].ui32ItemId;
			uint8_t itemType = (itemId >> 20) & 0x7;	// Extract item type from MAKE_ITEM_ID
			if (itemType == ITEM_TYPE_PRIMARY) itemCat = 0;
			else if (itemType == ITEM_TYPE_SECONDARY) itemCat = 1;
			else if (itemType == ITEM_TYPE_MELEE) itemCat = 2;
			else if (itemType == ITEM_TYPE_CHARA) itemCat = 4;
			else itemCat = 5;

			if (itemCat == categoryId)
			{
				memcpy(buffer + offset, &s_ShopCatalog[i].ui32GoodsId, 4);	offset += 4;
				memcpy(buffer + offset, &s_ShopCatalog[i].ui32ItemId, 4);	offset += 4;
				memcpy(buffer + offset, &s_ShopCatalog[i].i32PriceGP, 4);	offset += 4;
				memcpy(buffer + offset, &s_ShopCatalog[i].i32PriceCash, 4);	offset += 4;
				memcpy(buffer + offset, &s_ShopCatalog[i].ui32Duration, 4);	offset += 4;
				itemCount++;
			}
		}
	}

	memcpy(buffer + countPos, &itemCount, sizeof(uint16_t));

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopItemListReq(char* pData, INT32 i32Size)
{
	// Item details - same format as goods list but for specific items
	// Client sends list of item IDs it wants details for
	OnShopGoodsListReq(pData, i32Size);	// Reuse goods list logic
}

void GameSession::OnShopMatchingListReq(char* pData, INT32 i32Size)
{
	// Shop matching list - maps goods to categories
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Empty matching list (client uses default)
	uint16_t matchCount = 0;
	memcpy(buffer + offset, &matchCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetPointCashReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_GET_POINT_CASH_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint32_t gp = (uint32_t)m_i32GP;
	uint32_t cash = (uint32_t)m_i32Cash;
	memcpy(buffer + offset, &gp, 4);						offset += 4;
	memcpy(buffer + offset, &cash, 4);						offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
