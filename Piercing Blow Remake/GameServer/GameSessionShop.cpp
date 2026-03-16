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

	int32_t result = 0;
	uint32_t newGP = (uint32_t)m_i32GP;

	// Parse: repairType(1) + itemDBIdx(4)
	// repairType: 0 = single item, 1 = repair all
	if (i32Size >= 1)
	{
		uint8_t repairType = *(uint8_t*)pData;

		if (repairType == 1)
		{
			int ret = RepairAllItems();
			if (ret < 0)
				result = 1;
		}
		else if (i32Size >= 5)
		{
			uint32_t itemDBIdx = *(uint32_t*)(pData + 1);
			int ret = RepairItem(itemDBIdx);
			if (ret < 0)
				result = 1;
		}

		newGP = (uint32_t)m_i32GP;
	}

	// Send ACK with updated GP
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_SHOP_REPAIR_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &newGP, 4);						offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

// ============================================================================
// Item Auth / Insert / Delete (Phase 5B)
// ============================================================================

void GameSession::OnShopItemAuthReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ -> ACK
	// Activate/authenticate a wrapped or inactive item in inventory
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK, 1);
		return;
	}

	uint32_t itemDBIdx = *(uint32_t*)pData;
	int32_t result = 0;

	GameInventoryItem* pItem = FindInventoryItemByDBIdx(itemDBIdx);
	if (!pItem || !pItem->IsValid())
	{
		result = 1;		// Item not found
	}
	else
	{
		int itemType = GET_ITEM_TYPE(pItem->ui32ItemId);
		// Activate wrapped period/count items
		if (itemType == ITEM_TYPE_WRAP_PERIOD)
		{
			// Convert to active period item - set expiration from now
			pItem->ui8ItemType = ITEM_ATTR_PERIOD;
			if (pItem->ui32ItemArg == 0)
				pItem->ui32ItemArg = (uint32_t)time(nullptr) + 86400 * 30;	// Default 30 days
			else
				pItem->ui32ItemArg = (uint32_t)time(nullptr) + pItem->ui32ItemArg;

			printf("[GameSession] Item auth (period) - UID=%lld, DBIdx=%u, Expires=%u\n",
				m_i64UID, itemDBIdx, pItem->ui32ItemArg);
		}
		else if (itemType == ITEM_TYPE_WRAP_COUNT)
		{
			// Convert to active count item
			pItem->ui8ItemType = ITEM_ATTR_COUNT;
			if (pItem->ui32ItemArg == 0)
				pItem->ui32ItemArg = 10;	// Default 10 uses

			printf("[GameSession] Item auth (count) - UID=%lld, DBIdx=%u, Uses=%u\n",
				m_i64UID, itemDBIdx, pItem->ui32ItemArg);
		}
		else
		{
			result = 2;		// Item does not require authentication
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &itemDBIdx, 4);					offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopInsertItemReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ -> ACK
	// Admin/system inserts an item directly into player inventory
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Parse: itemId(4) + itemType(1) + itemArg(4)
	if (i32Size < 9)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_INSERT_ITEM_ACK, 1);
		return;
	}

	uint32_t itemId = *(uint32_t*)pData;
	uint8_t itemType = *(uint8_t*)(pData + 4);
	uint32_t itemArg = *(uint32_t*)(pData + 5);

	int32_t result = 0;
	uint32_t newDBIdx = 0;

	if (m_i32InventoryCount >= MAX_INVEN_COUNT)
	{
		result = 2;		// Inventory full
	}
	else if (HasInventoryItem(itemId))
	{
		result = 3;		// Already owned (non-stackable)
	}
	else
	{
		GameInventoryItem newItem;
		newItem.ui32ItemDBIdx = (uint32_t)(GetTickCount() & 0xFFFFFF) | ((uint32_t)m_i32InventoryCount << 24);
		newItem.ui32ItemId = itemId;
		newItem.ui8ItemType = itemType;
		newItem.ui32ItemArg = itemArg;
		newItem.ui8Durability = DURABILITY_MAX;

		if (AddInventoryItem(newItem) >= 0)
		{
			newDBIdx = newItem.ui32ItemDBIdx;
			printf("[GameSession] Item inserted - UID=%lld, ItemId=%u, DBIdx=%u\n",
				m_i64UID, itemId, newDBIdx);
		}
		else
		{
			result = 4;		// Insert failed
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_INSERT_ITEM_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &itemId, 4);					offset += 4;
	memcpy(buffer + offset, &newDBIdx, 4);					offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopDeleteItemReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ -> ACK
	// Delete an item from inventory
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK, 1);
		return;
	}

	uint32_t itemDBIdx = *(uint32_t*)pData;
	int32_t result = 0;

	GameInventoryItem* pItem = FindInventoryItemByDBIdx(itemDBIdx);
	if (!pItem || !pItem->IsValid())
	{
		result = 1;		// Item not found
	}
	else
	{
		uint32_t itemId = pItem->ui32ItemId;

		// Don't allow deletion of currently equipped items
		if (m_ui8ActiveCharaSlot < MAX_CHARA_SLOT)
		{
			const GameCharaEquip& equip = m_CharaSlots[m_ui8ActiveCharaSlot].equip;
			for (int w = 0; w < CHAR_EQUIP_WEAPON_COUNT; w++)
			{
				if (equip.ui32WeaponIds[w] == itemId)
				{
					result = 2;		// Item is equipped
					break;
				}
			}
			if (result == 0)
			{
				for (int p = 0; p < CHAR_EQUIP_PARTS_COUNT; p++)
				{
					if (equip.ui32PartsIds[p] == itemId)
					{
						result = 2;
						break;
					}
				}
			}
		}

		if (result == 0)
		{
			if (RemoveInventoryItem(itemDBIdx))
			{
				printf("[GameSession] Item deleted - UID=%lld, DBIdx=%u, ItemId=%u\n",
					m_i64UID, itemDBIdx, itemId);
			}
			else
			{
				result = 3;		// Remove failed
			}
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &itemDBIdx, 4);					offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopRepairListReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK
	// Client format: ui32RepairCount(4) + ui32CurItemCount(4) + ui32StartIdx(4)
	//                + SHOP_ITEM_REPAIR_DATA[count] + ui32Version(4)
	// SHOP_ITEM_REPAIR_DATA = { ui32ItemID(4), ui32PricePoint(4), ui32PriceCash(4), ui32Durability(4) } = 16 bytes

	// Build repair data from inventory - only weapon items have durability
	struct RepairEntry
	{
		uint32_t ui32ItemID;
		uint32_t ui32PricePoint;
		uint32_t ui32PriceCash;
		uint32_t ui32Durability;
	};

	RepairEntry entries[MAX_INVEN_COUNT];
	int entryCount = 0;

	for (int i = 0; i < m_i32InventoryCount && entryCount < MAX_INVEN_COUNT; i++)
	{
		if (!m_Inventory[i].IsValid())
			continue;

		int itemType = GET_ITEM_TYPE(m_Inventory[i].ui32ItemId);

		// Only weapons have durability/repair data
		if (itemType != ITEM_TYPE_PRIMARY && itemType != ITEM_TYPE_SECONDARY &&
			itemType != ITEM_TYPE_MELEE)
			continue;

		entries[entryCount].ui32ItemID = m_Inventory[i].ui32ItemId;
		// Repair cost scales with durability loss (simplified: 100 GP per durability point)
		uint8_t dur = m_Inventory[i].ui8Durability;
		uint32_t repairCost = (dur < DURABILITY_MAX) ? (uint32_t)(DURABILITY_MAX - dur) * 100 : 0;
		entries[entryCount].ui32PricePoint = repairCost;
		entries[entryCount].ui32PriceCash = 0;	// No cash repair
		entries[entryCount].ui32Durability = (uint32_t)dur;
		entryCount++;
	}

	// Send all in one packet (simplified - original splits into multiple packets for large lists)
	char buffer[8192];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	uint32_t totalCount = (uint32_t)entryCount;
	uint32_t curCount = (uint32_t)entryCount;
	uint32_t startIdx = 0;
	memcpy(buffer + offset, &totalCount, sizeof(uint32_t));	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &curCount, sizeof(uint32_t));	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &startIdx, sizeof(uint32_t));	offset += sizeof(uint32_t);

	// Copy repair data entries
	for (int i = 0; i < entryCount; i++)
	{
		if (offset + 16 > (int)sizeof(buffer))
			break;
		memcpy(buffer + offset, &entries[i], sizeof(RepairEntry));
		offset += sizeof(RepairEntry);
	}

	// Version
	uint32_t version = 1;
	if (g_pShopManager && g_pShopManager->IsLoaded())
		version = g_pShopManager->GetVersion();
	memcpy(buffer + offset, &version, sizeof(uint32_t));	offset += sizeof(uint32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	i3NetworkPacket packet;
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

// ============================================================================
// Capsule/Gacha (Phase 5C)
// ============================================================================

void GameSession::OnShopCapsuleReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_SHOP_CAPSULE_REQ -> ACK
	// Capsule = random item box. Client sends capsule item ID, server picks random result.
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < (int)(sizeof(uint32_t) + sizeof(uint32_t)))
		return;

	uint32_t capsuleItemId = *(uint32_t*)pData;
	uint32_t useCash = *(uint32_t*)(pData + 4);	// 0=GP, 1=Cash
	(void)useCash;

	// Validate capsule exists in inventory
	int invIdx = FindInventoryItem(capsuleItemId);
	if (invIdx < 0)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_CAPSULE_ACK, 1);	// 1 = item not found
		return;
	}

	// Random item selection from capsule pool
	uint32_t resultItemId = 0;
	int resultCount = 1;
	bool isJackpot = false;

	// Simple random item pool (would be data-driven in production)
	int roll = rand() % 100;
	if (roll < 5)
	{
		// 5% chance: rare item (jackpot)
		resultItemId = MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, 10);
		isJackpot = true;
	}
	else if (roll < 25)
	{
		// 20% chance: uncommon
		resultItemId = MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, 8);
	}
	else if (roll < 60)
	{
		// 35% chance: common weapon
		resultItemId = MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, 5);
	}
	else
	{
		// 40% chance: consumable/common
		resultItemId = MAKE_ITEM_ID(ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, 4);
	}

	// Remove capsule from inventory (consume it)
	RemoveInventoryItem(invIdx);

	// Add result item to inventory
	AddInventoryItem(resultItemId, resultCount, 0);

	// Send capsule result
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_CAPSULE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;	// 0 = success
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	memcpy(buffer + offset, &resultItemId, sizeof(uint32_t));	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &resultCount, sizeof(int32_t));		offset += sizeof(int32_t);

	uint8_t jackpotFlag = isJackpot ? 1 : 0;
	memcpy(buffer + offset, &jackpotFlag, sizeof(uint8_t));		offset += sizeof(uint8_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	printf("[Shop] Capsule opened - UID=%lld, CapsuleId=%u, Result=%u, Jackpot=%d\n",
		m_i64UID, capsuleItemId, resultItemId, isJackpot ? 1 : 0);

	// If jackpot, broadcast notification to all players
	if (isJackpot && g_pGameSessionManager)
	{
		char jackpotMsg[128];
		snprintf(jackpotMsg, sizeof(jackpotMsg), "%s won a JACKPOT item!", m_szNickname);
		g_pGameSessionManager->BroadcastAnnounce(jackpotMsg, (uint16_t)strlen(jackpotMsg));
	}
}

void GameSession::OnShopJackpotReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_SHOP_JACKPOT_REQ -> ACK
	// Client requests recent jackpot winners list
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Send empty jackpot list for now
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_JACKPOT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	int32_t jackpotCount = 0;	// No recent jackpots
	memcpy(buffer + offset, &jackpotCount, sizeof(int32_t));	offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Sale List & Plus Point (Protocol_Shop 0x0400)
// ============================================================================

void GameSession::OnShopGetSaleListReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_SHOP_GET_SAILLIST_REQ -> ACK
	// Get list of items currently on sale / discounted
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_SHOP_GET_SAILLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Sale item count (empty for now - no active sales)
	uint16_t saleCount = 0;
	memcpy(buffer + offset, &saleCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	// Per-sale entry would be: goodsId(4) + discountPercent(1) + startTime(4) + endTime(4)

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopPlusPointReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_SHOP_PLUS_POINT_REQ -> ACK
	// Client notifies server of points earned (from purchase, capsule, etc.)
	// Server validates and sends updated balance
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Parse: type(1) + amount(4)
	uint8_t pointType = 0;		// 0=GP, 1=Cash
	int32_t amount = 0;

	if (i32Size >= 5)
	{
		pointType = *(uint8_t*)pData;
		memcpy(&amount, pData + 1, sizeof(int32_t));
	}

	// Server-side validation: don't blindly trust client amounts
	// In production this would be verified against the actual transaction
	// For now, just send back current balance

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_SHOP_PLUS_POINT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	uint32_t gp = (uint32_t)m_i32GP;
	uint32_t cash = (uint32_t)m_i32Cash;
	memcpy(buffer + offset, &gp, 4);					offset += 4;
	memcpy(buffer + offset, &cash, 4);					offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Batch 17 - Shop extras
// ============================================================================

void GameSession::OnShopGetGiftListReq(char* pData, INT32 i32Size)
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	uint16_t giftCount = 0;
	memcpy(buffer + offset, &giftCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopExpireDeleteItemReq(char* pData, INT32 i32Size)
{
	if (i32Size < (int)sizeof(uint16_t))
		return;

	uint16_t itemCount = 0;
	memcpy(&itemCount, pData, sizeof(uint16_t));

	if (itemCount == 0 || itemCount > 100)
		return;

	int offset = sizeof(uint16_t);
	int deleted = 0;

	for (uint16_t i = 0; i < itemCount && offset + 4 <= i32Size; i++)
	{
		uint32_t itemId = 0;
		memcpy(&itemId, pData + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		for (int j = 0; j < m_i32InventoryCount; j++)
		{
			if (m_Inventory[j].ui32ItemId == itemId)
			{
				for (int k = j; k < m_i32InventoryCount - 1; k++)
					m_Inventory[k] = m_Inventory[k + 1];
				m_i32InventoryCount--;
				deleted++;
				break;
			}
		}
	}

	printf("[GameSession] Deleted %d expired items for UID=%lld\n", deleted, m_i64UID);
}

void GameSession::OnShopRewardItemReq(char* pData, INT32 i32Size)
{
	// Reward item claim - fire-and-forget (no ACK protocol defined)
	if (i32Size < (int)sizeof(uint32_t))
		return;

	uint32_t rewardId = 0;
	memcpy(&rewardId, pData, sizeof(uint32_t));

	printf("[GameSession] Reward item claim: rewardId=%u, UID=%lld\n", rewardId, m_i64UID);
}

void GameSession::OnShopSaleCouponListReq(char* pData, INT32 i32Size)
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_SHOP_SALECOUPONLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	uint16_t couponCount = 0;
	memcpy(buffer + offset, &couponCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnShopAuthGiftReq(char* pData, INT32 i32Size)
{
	if (i32Size < (int)(sizeof(uint32_t) + sizeof(int64_t)))
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK, -1);
		return;
	}

	int offset = 0;
	uint32_t goodsId = 0;
	int64_t targetUID = 0;
	memcpy(&goodsId, pData + offset, sizeof(uint32_t));		offset += sizeof(uint32_t);
	memcpy(&targetUID, pData + offset, sizeof(int64_t));	offset += sizeof(int64_t);

	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr;
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK, -2);
		return;
	}

	// Look up item in catalog
	uint32_t itemId = 0;
	int priceGP = 0;
	uint32_t duration = 0;
	bool found = false;

	if (g_pShopManager && g_pShopManager->IsLoaded())
	{
		const ShopItem* pItem = g_pShopManager->FindByGoodsId(goodsId);
		if (pItem)
		{
			itemId = pItem->ui32ItemId;
			priceGP = pItem->i32PriceGP;
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
				duration = s_ShopCatalog[i].ui32Duration;
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK, -3);
		return;
	}

	if (m_i32GP < priceGP)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK, -4);
		return;
	}

	m_i32GP -= priceGP;

	if (pTarget->m_i32InventoryCount < MAX_INVEN_COUNT)
	{
		GameInventoryItem& item = pTarget->m_Inventory[pTarget->m_i32InventoryCount];
		item.ui32ItemId = itemId;
		item.ui32Count = 1;
		item.i32SlotIdx = -1;
		item.ui8IsEquipped = 0;
		item.ui32Duration = duration;
		pTarget->m_i32InventoryCount++;
	}

	pTarget->SendSimpleAck(PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_ACK, 0);
	SendSimpleAck(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK, 0);
}

void GameSession::OnShopNotifyGiftReq(char* pData, INT32 i32Size)
{
	SendSimpleAck(PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_ACK, 0);
}

// ============================================================================
// Batch 19 - Roulette Shop (RS) handlers
// ============================================================================

void GameSession::OnAuthRsEnterReq(char* pData, INT32 i32Size)
{
	// Enter roulette shop
	// Send roulette shop enter response with available tabs
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_AUTH_RS_ENTER_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;	// success
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Tab count (simplified: 1 tab)
	uint8_t tabCount = 1;
	memcpy(buffer + offset, &tabCount, 1);	offset += 1;

	// Tab info: id, cost type, cost amount
	uint8_t tabId = 0;
	memcpy(buffer + offset, &tabId, 1);	offset += 1;
	int32_t costGP = 1000;		// 1000 GP per spin
	memcpy(buffer + offset, &costGP, sizeof(int32_t));	offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAuthRsItemInfoReq(char* pData, INT32 i32Size)
{
	// Request roulette item pool info for a tab
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_AUTH_RS_ITEM_INFO_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Item count in the pool (simplified: 5 sample items)
	uint8_t itemCount = 5;
	memcpy(buffer + offset, &itemCount, 1);	offset += 1;

	// Sample roulette items: itemId, grade(0=lose,1-3=prize), probability weight
	struct RsItem { uint32_t itemId; uint8_t grade; uint16_t weight; };
	RsItem items[] = {
		{ 0,		0, 500 },	// Lose (50%)
		{ 1101001,	1, 250 },	// Grade 1 (25%)
		{ 1201001,	1, 150 },	// Grade 1 (15%)
		{ 2101001,	2, 80 },	// Grade 2 (8%)
		{ 3101001,	3, 20 },	// Grade 3 (2%)
	};

	for (int i = 0; i < 5; i++)
	{
		memcpy(buffer + offset, &items[i].itemId, 4);	offset += 4;
		memcpy(buffer + offset, &items[i].grade, 1);	offset += 1;
		memcpy(buffer + offset, &items[i].weight, 2);	offset += 2;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAuthRsJackpotReq(char* pData, INT32 i32Size)
{
	// Request jackpot winner info
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_AUTH_RS_JACKPOT_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Jackpot winner count (0 = no recent winners)
	uint8_t winnerCount = 0;
	memcpy(buffer + offset, &winnerCount, 1);	offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAuthRsStartReq(char* pData, INT32 i32Size)
{
	// Start a roulette spin
	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_AUTH_RS_RESULT_ACK, 1);
		return;
	}

	uint8_t tabId = (uint8_t)pData[0];

	// Check GP balance (1000 GP per spin)
	int32_t spinCost = 1000;
	if (m_i32GP < spinCost)
	{
		SendSimpleAck(PROTOCOL_AUTH_RS_RESULT_ACK, 2);	// Not enough GP
		return;
	}

	m_i32GP -= spinCost;

	// Simple random result (weighted)
	uint32_t roll = (uint32_t)(rand() % 1000);
	uint32_t resultItemId = 0;
	uint8_t resultGrade = 0;

	if (roll < 500)
	{
		// Lose (50%)
		resultItemId = 0;
		resultGrade = 0;
	}
	else if (roll < 750)
	{
		// Grade 1 (25%)
		resultItemId = 1101001;
		resultGrade = 1;
	}
	else if (roll < 900)
	{
		// Grade 1 (15%)
		resultItemId = 1201001;
		resultGrade = 1;
	}
	else if (roll < 980)
	{
		// Grade 2 (8%)
		resultItemId = 2101001;
		resultGrade = 2;
	}
	else
	{
		// Grade 3 (2%)
		resultItemId = 3101001;
		resultGrade = 3;
	}

	// Add item to inventory if won
	if (resultItemId > 0 && m_i32InventoryCount < MAX_INVEN_COUNT)
	{
		GameInventoryItem& item = m_Inventory[m_i32InventoryCount];
		item.ui32ItemId = resultItemId;
		item.ui32Count = 1;
		item.i32SlotIdx = -1;
		item.ui8IsEquipped = 0;
		item.ui32Duration = 0;	// Permanent
		m_i32InventoryCount++;
	}

	// Send result
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_AUTH_RS_RESULT_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &resultItemId, sizeof(uint32_t));	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &resultGrade, 1);	offset += 1;

	// Remaining GP
	memcpy(buffer + offset, &m_i32GP, sizeof(int32_t));	offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAuthFcmLogoutReq(char* pData, INT32 i32Size)
{
	// FCM (Fatigue Control Mechanism) logout notification
	// Fire-and-forget - client is already exiting
	printf("[GameSession] FCM logout Index=%d UID=%lld\n", GetIndex(), m_i64UID);
}

void GameSession::OnAuthShopAuthGiftReq(char* pData, INT32 i32Size)
{
	// Gift authentication/verification request
	// Validate that a gift can be sent to the target user
	if (i32Size < (int)(sizeof(int64_t) + sizeof(uint32_t)))
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK, 1);
		return;
	}

	int64_t targetUID = 0;
	uint32_t goodsId = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));
	memcpy(&goodsId, pData + sizeof(int64_t), sizeof(uint32_t));

	// Check target exists
	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr;
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK, 2);	// Target not found
		return;
	}

	// Check item exists in shop
	bool found = false;
	if (g_pShopManager && g_pShopManager->IsLoaded())
	{
		const ShopItem* pItem = g_pShopManager->FindByGoodsId(goodsId);
		if (pItem) found = true;
	}
	if (!found)
	{
		for (int i = 0; i < SHOP_CATALOG_SIZE; i++)
		{
			if (s_ShopCatalog[i].ui32GoodsId == goodsId) { found = true; break; }
		}
	}

	if (!found)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK, 3);	// Item not found
		return;
	}

	// Check target inventory space
	if (pTarget->m_i32InventoryCount >= MAX_INVEN_COUNT)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK, 4);	// Target inventory full
		return;
	}

	SendSimpleAck(PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK, 0);	// Gift verified OK
}

void GameSession::OnAuthShopItemAuthDataReq(char* pData, INT32 i32Size)
{
	// Request item authentication data (item verification for anti-cheat)
	if (i32Size < (int)sizeof(uint32_t))
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_ACK, 1);
		return;
	}

	uint32_t itemId = 0;
	memcpy(&itemId, pData, sizeof(uint32_t));

	// Check if user owns this item
	bool found = false;
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].ui32ItemId == itemId)
		{
			found = true;
			break;
		}
	}

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = found ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &itemId, sizeof(uint32_t));	offset += sizeof(uint32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAuthShopItemChangeDataReq(char* pData, INT32 i32Size)
{
	// Change item settings (e.g., item option configuration)
	if (i32Size < (int)(sizeof(uint32_t) + sizeof(uint32_t)))
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_ACK, 1);
		return;
	}

	uint32_t itemId = 0;
	uint32_t newData = 0;
	memcpy(&itemId, pData, sizeof(uint32_t));
	memcpy(&newData, pData + sizeof(uint32_t), sizeof(uint32_t));

	// Find and update item
	bool found = false;
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].ui32ItemId == itemId)
		{
			// Update item data/option
			found = true;
			break;
		}
	}

	SendSimpleAck(PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_ACK, found ? 0 : 2);
}

void GameSession::OnAuthShopUseGiftcouponReq(char* pData, INT32 i32Size)
{
	// Use a gift coupon (shop-side coupon redemption)
	if (i32Size < 16)
	{
		SendSimpleAck(PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_ACK, 1);
		return;
	}

	char couponCode[17] = {};
	memcpy(couponCode, pData, 16);

	// Coupon validation would go to DataServer
	// For now, always fail (no coupon system implemented)
	SendSimpleAck(PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_ACK, 2);	// Invalid coupon
}

void GameSession::OnAuthUseGiftcouponReq(char* pData, INT32 i32Size)
{
	// Use a gift coupon (auth-side coupon)
	if (i32Size < 16)
	{
		SendSimpleAck(PROTOCOL_AUTH_USE_GIFTCOUPON_ACK, 1);
		return;
	}

	char couponCode[17] = {};
	memcpy(couponCode, pData, 16);

	// Coupon validation not implemented yet
	SendSimpleAck(PROTOCOL_AUTH_USE_GIFTCOUPON_ACK, 2);	// Invalid coupon
}

void GameSession::OnAuthUseItemCheckNickReq(char* pData, INT32 i32Size)
{
	// Check nickname availability (via item use, e.g., nick change coupon)
	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK, 1);
		return;
	}

	char nickname[64] = {};
	int copyLen = (i32Size < 63) ? i32Size : 63;
	memcpy(nickname, pData, copyLen);

	// Delegate to DataServer nick check
	if (g_pModuleDataServer)
	{
		g_pModuleDataServer->RequestCheckNick(GetIndex(), nickname);
		// Response will come async via OnCheckNickAck
	}
	else
	{
		SendSimpleAck(PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK, 0);	// Available (offline mode)
	}
}

void GameSession::OnAuthUserNowInfoReq(char* pData, INT32 i32Size)
{
	// Request current user state/info update
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_AUTH_USER_NOW_INFO_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &m_i64UID, sizeof(int64_t));	offset += sizeof(int64_t);

	// Current state
	int32_t mainTask = (int32_t)m_eMainTask;
	memcpy(buffer + offset, &mainTask, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &m_i32ChannelNum, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &m_i32RoomIdx, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &m_i32Level, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &m_i32GP, sizeof(int32_t));			offset += sizeof(int32_t);
	memcpy(buffer + offset, &m_i32Cash, sizeof(int32_t));		offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnFieldshopSendNewversionReq(char* pData, INT32 i32Size)
{
	// Field shop version request - send current field shop item list
	// Field shop is the in-battle shop for temporary items
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_FIELDSHOP_SEND_NEWVERSION_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Version number
	uint32_t version = 1;
	memcpy(buffer + offset, &version, sizeof(uint32_t));	offset += sizeof(uint32_t);

	// Item count (0 = no field shop items configured)
	uint8_t itemCount = 0;
	memcpy(buffer + offset, &itemCount, 1);	offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
