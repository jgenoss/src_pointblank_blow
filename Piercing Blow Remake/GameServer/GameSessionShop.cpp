#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "ShopManager.h"

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
