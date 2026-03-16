#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"

// ============================================================================
// Inventory Handlers (7F)
// ============================================================================

void GameSession::OnGetInvenInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_GET_INVEN_INFO_REQ -> ACK
	// Send full inventory to client (called after login/info phase)
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Calculate needed buffer: header + per-item data
	// Each item: DBIdx(4) + ItemID(4) + ItemType(1) + ItemArg(4) = 13 bytes
	const int ITEM_ENTRY_SIZE = 13;
	int payloadSize = 4 + 2 + 2 + 4 + 2 + (m_i32InventoryCount * ITEM_ENTRY_SIZE);

	// Use stack buffer for reasonable inventory sizes, or cap
	char buffer[8192];
	if (payloadSize > (int)sizeof(buffer))
		payloadSize = (int)sizeof(buffer);

	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_INVEN_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Item count
	uint16_t itemCount = (uint16_t)m_i32InventoryCount;
	memcpy(buffer + offset, &itemCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	// Item entries
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (offset + ITEM_ENTRY_SIZE > (int)sizeof(buffer))
			break;

		const GameInventoryItem& item = m_Inventory[i];
		memcpy(buffer + offset, &item.ui32ItemDBIdx, 4);	offset += 4;
		memcpy(buffer + offset, &item.ui32ItemId, 4);		offset += 4;
		memcpy(buffer + offset, &item.ui8ItemType, 1);		offset += 1;
		memcpy(buffer + offset, &item.ui32ItemArg, 4);		offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	i3NetworkPacket packet;
	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnInventoryEnterReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_INVENTORY_ENTER_REQ -> ACK
	// Client enters inventory screen
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	i3NetworkPacket packet;
	char buffer[8192];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_INVENTORY_ENTER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Count items by category
	uint16_t charaCount = 0, weaponCount = 0, itemCount = 0;
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		int type = GET_ITEM_TYPE(m_Inventory[i].ui32ItemId);
		if (type == ITEM_TYPE_CHARA)
			charaCount++;
		else if (type >= ITEM_TYPE_PRIMARY && type <= ITEM_TYPE_THROWING2)
			weaponCount++;
		else
			itemCount++;
	}

	memcpy(buffer + offset, &charaCount, 2);	offset += 2;
	memcpy(buffer + offset, &weaponCount, 2);	offset += 2;
	memcpy(buffer + offset, &itemCount, 2);		offset += 2;

	// Send all items (simplified - original sends by category)
	const int ITEM_ENTRY_SIZE = 13;
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (offset + ITEM_ENTRY_SIZE > (int)sizeof(buffer))
			break;

		const GameInventoryItem& item = m_Inventory[i];
		memcpy(buffer + offset, &item.ui32ItemDBIdx, 4);	offset += 4;
		memcpy(buffer + offset, &item.ui32ItemId, 4);		offset += 4;
		memcpy(buffer + offset, &item.ui8ItemType, 1);		offset += 1;
		memcpy(buffer + offset, &item.ui32ItemArg, 4);		offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnInventoryLeaveReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_INVENTORY_LEAVE_REQ -> ACK
	// Client leaves inventory screen. May contain equipment changes.
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Parse optional equipment change data
	if (i32Size >= 1)
	{
		uint8_t charaSlotIdx = *(uint8_t*)pData;
		if (charaSlotIdx < MAX_CHARA_SLOT)
		{
			int pos = 1;

			// Parse weapon changes if present: 5 weapon IDs
			if (pos + (int)(CHAR_EQUIP_WEAPON_COUNT * 4) <= i32Size)
			{
				for (int w = 0; w < CHAR_EQUIP_WEAPON_COUNT; w++)
				{
					uint32_t weaponId = *(uint32_t*)(pData + pos);
					m_CharaSlots[charaSlotIdx].equip.ui32WeaponIds[w] = weaponId;
					pos += 4;
				}
			}

			// Parse parts changes if present: 10 parts IDs
			if (pos + (int)(CHAR_EQUIP_PARTS_COUNT * 4) <= i32Size)
			{
				for (int p = 0; p < CHAR_EQUIP_PARTS_COUNT; p++)
				{
					uint32_t partsId = *(uint32_t*)(pData + pos);
					m_CharaSlots[charaSlotIdx].equip.ui32PartsIds[p] = partsId;
					pos += 4;
				}
			}

			m_ui8ActiveCharaSlot = charaSlotIdx;
		}
	}

	SendSimpleAck(PROTOCOL_INVENTORY_LEAVE_ACK, 0);
}

// ============================================================================
// Item Durability System (Phase 14C)
// ============================================================================

void GameSession::DecreaseEquippedDurability()
{
	if (m_ui8ActiveCharaSlot >= MAX_CHARA_SLOT)
		return;

	const GameCharaEquip& equip = m_CharaSlots[m_ui8ActiveCharaSlot].equip;
	bool bNotify = false;

	// Decrease weapon durability
	for (int w = 0; w < CHAR_EQUIP_WEAPON_COUNT; w++)
	{
		if (equip.ui32WeaponIds[w] == 0)
			continue;

		GameInventoryItem* pItem = FindInventoryItem(equip.ui32WeaponIds[w]);
		if (pItem && pItem->HasDurability() && pItem->ui8Durability > 0)
		{
			int decrease = DURABILITY_DECREASE_PER_BATTLE;
			if (pItem->ui8Durability <= (uint8_t)decrease)
				pItem->ui8Durability = 0;
			else
				pItem->ui8Durability -= (uint8_t)decrease;

			if (pItem->ui8Durability == 0)
				bNotify = true;
		}
	}

	// Decrease parts durability
	for (int p = 0; p < CHAR_EQUIP_PARTS_COUNT; p++)
	{
		if (equip.ui32PartsIds[p] == 0)
			continue;

		GameInventoryItem* pItem = FindInventoryItem(equip.ui32PartsIds[p]);
		if (pItem && pItem->HasDurability() && pItem->ui8Durability > 0)
		{
			int decrease = DURABILITY_DECREASE_PER_BATTLE;
			if (pItem->ui8Durability <= (uint8_t)decrease)
				pItem->ui8Durability = 0;
			else
				pItem->ui8Durability -= (uint8_t)decrease;

			if (pItem->ui8Durability == 0)
				bNotify = true;
		}
	}

	// Notify client if any item broke
	if (bNotify)
	{
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t sz = 0;
		uint16_t proto = PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t reason = 2;	// 2 = item broken
		memcpy(buffer + offset, &reason, 1);				offset += 1;

		sz = (uint16_t)offset;
		memcpy(buffer, &sz, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendMessage(&packet);

		printf("[GameSession] Item broke! UID=%lld\n", m_i64UID);
	}
}

int GameSession::RepairItem(uint32_t ui32ItemDBIdx)
{
	GameInventoryItem* pItem = FindInventoryItemByDBIdx(ui32ItemDBIdx);
	if (!pItem || !pItem->IsValid())
		return -1;	// Item not found

	if (!pItem->HasDurability())
		return -2;	// Not repairable

	if (pItem->ui8Durability >= DURABILITY_MAX)
		return -3;	// Already full

	int cost = pItem->GetRepairCost();
	if (m_i32GP < cost)
		return -4;	// Not enough GP

	m_i32GP -= cost;
	pItem->ui8Durability = DURABILITY_MAX;

	printf("[GameSession] Repaired item - UID=%lld, ItemDBIdx=%u, Cost=%d GP\n",
		m_i64UID, ui32ItemDBIdx, cost);
	return 0;
}

int GameSession::RepairAllItems()
{
	int totalCost = 0;
	int repairedCount = 0;

	// Calculate total cost first
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].HasDurability() && m_Inventory[i].ui8Durability < DURABILITY_MAX)
			totalCost += m_Inventory[i].GetRepairCost();
	}

	if (totalCost == 0)
		return 0;	// Nothing to repair

	if (m_i32GP < totalCost)
		return -4;	// Not enough GP

	m_i32GP -= totalCost;

	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].HasDurability() && m_Inventory[i].ui8Durability < DURABILITY_MAX)
		{
			m_Inventory[i].ui8Durability = DURABILITY_MAX;
			repairedCount++;
		}
	}

	printf("[GameSession] Repaired all items - UID=%lld, Count=%d, Cost=%d GP\n",
		m_i64UID, repairedCount, totalCost);
	return repairedCount;
}
