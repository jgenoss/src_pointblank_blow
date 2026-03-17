#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "RouletteDef.h"

// ============================================================================
// Roulette/Gacha Handlers (PROTOCOL_Rs_Igs 0x0E00)
// ============================================================================

void GameSession::OnRouletteEnterReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_RS_ENTER_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t result = 0;

	if (!g_pRouletteData)
		result = 1;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_RS_ENTER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Tab count
	uint8_t tabCount = g_pRouletteData ? (uint8_t)g_pRouletteData->i32TabCount : 0;
	memcpy(buffer + offset, &tabCount, 1);					offset += 1;

	// GP and Cash balance
	uint32_t gp = (uint32_t)m_i32GP;
	uint32_t cash = (uint32_t)m_i32Cash;
	memcpy(buffer + offset, &gp, 4);						offset += 4;
	memcpy(buffer + offset, &cash, 4);						offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnRouletteLeaveReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_RS_LEAVE_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	SendSimpleAck(PROTOCOL_RS_LEAVE_ACK, 0);
}

void GameSession::OnRouletteItemInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_RS_ITEM_INFO_REQ -> ACK
	// Send items for a specific tab and grade
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2 || !g_pRouletteData)
		return;

	uint8_t tabIdx = *(uint8_t*)pData;
	uint8_t gradeFilter = *(uint8_t*)(pData + 1);	// Which grade to show (0xFF = all)

	if (tabIdx >= g_pRouletteData->i32TabCount)
		return;

	const RouletteTab& tab = g_pRouletteData->tabs[tabIdx];

	i3NetworkPacket packet;
	char buffer[2048];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_RS_ITEM_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Tab info
	memcpy(buffer + offset, &tabIdx, 1);					offset += 1;
	memcpy(buffer + offset, &tab.i32CostGP, 4);			offset += 4;
	memcpy(buffer + offset, &tab.i32CostCash, 4);			offset += 4;

	// Count matching items
	uint16_t itemCount = 0;
	int countPos = offset;
	offset += sizeof(uint16_t);

	// Per-item: itemId(4) + grade(1) + duration(4) + count(4) = 13 bytes
	for (int i = 0; i < tab.i32ItemCount; i++)
	{
		if (gradeFilter != 0xFF && tab.items[i].ui8Grade != gradeFilter)
			continue;

		if (offset + 13 > (int)sizeof(buffer))
			break;

		memcpy(buffer + offset, &tab.items[i].ui32ItemId, 4);		offset += 4;
		memcpy(buffer + offset, &tab.items[i].ui8Grade, 1);		offset += 1;
		memcpy(buffer + offset, &tab.items[i].ui32Duration, 4);	offset += 4;
		memcpy(buffer + offset, &tab.items[i].i32Count, 4);		offset += 4;
		itemCount++;
	}

	memcpy(buffer + countPos, &itemCount, sizeof(uint16_t));

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnRouletteStartReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_RS_ROULETTE_START_REQ -> PROTOCOL_RS_ROULETTE_RESULT_ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2 || !g_pRouletteData)
		return;

	uint8_t tabIdx = *(uint8_t*)pData;
	uint8_t spinCount = *(uint8_t*)(pData + 1);	// 1 = single, 10 = multi
	if (spinCount == 0) spinCount = 1;
	if (spinCount > 10) spinCount = 10;

	if (tabIdx >= g_pRouletteData->i32TabCount)
	{
		SendSimpleAck(PROTOCOL_RS_ROULETTE_RESULT_ACK, 1);
		return;
	}

	const RouletteTab& tab = g_pRouletteData->tabs[tabIdx];
	if (!tab.bActive)
	{
		SendSimpleAck(PROTOCOL_RS_ROULETTE_RESULT_ACK, 2);
		return;
	}

	// Check currency
	int totalCostGP = tab.i32CostGP * spinCount;
	int totalCostCash = tab.i32CostCash * spinCount;

	if (tab.i32CostGP > 0 && m_i32GP < totalCostGP)
	{
		SendSimpleAck(PROTOCOL_RS_ROULETTE_RESULT_ACK, 3);	// Not enough GP
		return;
	}
	if (tab.i32CostCash > 0 && m_i32Cash < totalCostCash)
	{
		SendSimpleAck(PROTOCOL_RS_ROULETTE_RESULT_ACK, 4);	// Not enough Cash
		return;
	}

	// Deduct currency
	if (tab.i32CostGP > 0) m_i32GP -= totalCostGP;
	if (tab.i32CostCash > 0) m_i32Cash -= totalCostCash;

	// Do spins
	struct SpinResult
	{
		uint32_t itemId;
		uint8_t grade;
		uint32_t duration;
		int count;
	};

	SpinResult results[10];
	int actualSpins = 0;
	bool hasJackpot = false;

	for (int s = 0; s < spinCount; s++)
	{
		int idx = tab.SpinRoulette();
		if (idx < 0) continue;

		const RouletteItem& won = tab.items[idx];
		results[actualSpins].itemId = won.ui32ItemId;
		results[actualSpins].grade = won.ui8Grade;
		results[actualSpins].duration = won.ui32Duration;
		results[actualSpins].count = won.i32Count;
		actualSpins++;

		if (won.ui8Grade == ROULETTE_GRADE_JACKPOT)
			hasJackpot = true;

		// Add to inventory or grant GP consolation
		if (won.ui32ItemId == 0)
		{
			// GP consolation
			m_i32GP += won.i32Count;
		}
		else
		{
			// Add item to inventory
			GameInventoryItem newItem;
			newItem.Reset();
			newItem.ui32ItemDBIdx = (uint32_t)(GetTickCount() + s);
			newItem.ui32ItemId = won.ui32ItemId;
			newItem.ui8ItemType = 0;
			newItem.ui32ItemArg = won.ui32Duration;
			AddInventoryItem(newItem);
		}
	}

	printf("[GameSession] Roulette spin - UID=%lld, Tab=%d, Spins=%d, Jackpot=%s\n",
		m_i64UID, tabIdx, actualSpins, hasJackpot ? "YES" : "no");

	// Send result ACK
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_RS_ROULETTE_RESULT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Updated balance
	uint32_t gp = (uint32_t)m_i32GP;
	uint32_t cash = (uint32_t)m_i32Cash;
	memcpy(buffer + offset, &gp, 4);						offset += 4;
	memcpy(buffer + offset, &cash, 4);						offset += 4;

	// Spin results
	uint8_t resultCount = (uint8_t)actualSpins;
	memcpy(buffer + offset, &resultCount, 1);				offset += 1;

	for (int i = 0; i < actualSpins; i++)
	{
		if (offset + 13 > (int)sizeof(buffer))
			break;

		memcpy(buffer + offset, &results[i].itemId, 4);	offset += 4;
		memcpy(buffer + offset, &results[i].grade, 1);		offset += 1;
		memcpy(buffer + offset, &results[i].duration, 4);	offset += 4;
		memcpy(buffer + offset, &results[i].count, 4);		offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnRouletteJackpotNotifyReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_RS_JACKPOT_NOTIFY_REQ -> ACK
	// Client reports jackpot win - broadcast to all players
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// For now, just send ACK back
	SendSimpleAck(PROTOCOL_RS_JACKPOT_NOTIFY_ACK, 0);
}

// ============================================================================
// Field Shop Handlers (in-battle shop)
// ============================================================================

void GameSession::OnFieldShopOpenReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_FIELDSHOP_OPEN_REQ -> ACK
	// Client requests to open field shop during battle
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Send ACK with current version (0 = no field shop data available)
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_FIELDSHOP_OPEN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Field shop version (client compares to decide if it needs to re-download)
	uint32_t version = 1;
	memcpy(buffer + offset, &version, sizeof(uint32_t));	offset += sizeof(uint32_t);

	// Item count in field shop
	uint16_t itemCount = 0;
	memcpy(buffer + offset, &itemCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnFieldShopGoodsListReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_FIELDSHOP_GOODSLIST_REQ -> ACK
	// Client requests the field shop goods list
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Send empty goods list (field shop not populated yet)
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_FIELDSHOP_GOODSLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint16_t goodsCount = 0;
	memcpy(buffer + offset, &goodsCount, sizeof(uint16_t)); offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Gacha System (0x1400)
// Gacha is a separate random item shop distinct from the roulette (RS_IGS).
// ============================================================================

void GameSession::OnGachaItemInfoReq(char* pData, INT32 i32Size)
{
	// Return gacha item info - empty item list for now
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_GACHA_ITEM_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint16_t itemCount = 0;
	memcpy(buffer + offset, &itemCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGachaShopStateReq(char* pData, INT32 i32Size)
{
	// Return gacha shop state: open=1, closed=0
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_GACHA_SHOP_STATE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint8_t isOpen = 1;	// Shop is open
	memcpy(buffer + offset, &isOpen, 1);					offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGachaEnterReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_GACHA_ENTER_ACK, -1);
		return;
	}

	SendSimpleAck(PROTOCOL_GACHA_ENTER_ACK, 0);
}

void GameSession::OnGachaLeaveReq(char* pData, INT32 i32Size)
{
	SendSimpleAck(PROTOCOL_GACHA_LEAVE_ACK, 0);
}

void GameSession::OnGachaPurchaseReq(char* pData, INT32 i32Size)
{
	// Gacha purchase - random item selection
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_GACHA_PURCHASE_ACK, -1);
		return;
	}

	if (i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_GACHA_PURCHASE_ACK, -2);
		return;
	}

	int gachaId = 0;
	memcpy(&gachaId, pData, sizeof(int));

	// Check if player has enough currency (GP)
	int gachaCost = 5000;	// Default gacha cost
	if (m_i32GP < gachaCost)
	{
		SendSimpleAck(PROTOCOL_GACHA_PURCHASE_ACK, -3); // Insufficient GP
		return;
	}

	// Deduct currency
	m_i32GP -= gachaCost;

	// Generate random reward item
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_GACHA_PURCHASE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;	// Success
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Reward item info
	int32_t rewardItemId = 1001 + (rand() % 100);	// Random item ID
	int32_t rewardCount = 1;
	int32_t remainGP = m_i32GP;
	memcpy(buffer + offset, &rewardItemId, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &rewardCount, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &remainGP, sizeof(int32_t));		offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	printf("[Gacha] Purchase - UID=%lld, GachaId=%d, Reward=%d, RemainGP=%d\n",
		m_i64UID, gachaId, rewardItemId, m_i32GP);
}

void GameSession::OnGachaGetPurchaseCountReq(char* pData, INT32 i32Size)
{
	// Return purchase count for this session
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_GACHA_GET_PURCHASE_COUNT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	int32_t purchaseCount = 0;	// No purchases tracked yet
	memcpy(buffer + offset, &purchaseCount, sizeof(int32_t)); offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// MyInfo UI State (0x1900)
// ============================================================================

void GameSession::OnMyInfoEnterReq(char* pData, INT32 i32Size)
{
	// Client entered MyInfo screen - acknowledge
	SendSimpleAck(PROTOCOL_MYINFO_ENTER_ACK, 0);
}

void GameSession::OnMyInfoLeaveReq(char* pData, INT32 i32Size)
{
	// Client left MyInfo screen - acknowledge
	SendSimpleAck(PROTOCOL_MYINFO_LEAVE_ACK, 0);
}
