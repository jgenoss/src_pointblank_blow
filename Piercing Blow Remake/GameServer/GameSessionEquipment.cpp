#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "Room.h"

// ============================================================================
// Equipment Handlers (7E)
// ============================================================================

void GameSession::OnGetEquipmentInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_GET_EQUIPMENT_INFO_REQ -> ACK
	// Send current active character slot equipment to client
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	const GameCharaSlot& slot = m_CharaSlots[m_ui8ActiveCharaSlot];

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = 0;

	// Active character slot index

	// Weapon loadout (5 weapon ItemIDs)
	for (int i = 0; i < CHAR_EQUIP_WEAPON_COUNT; i++)
	{
		memcpy(buffer + offset, &slot.equip.ui32WeaponIds[i], 4);	offset += 4;
	}

	// Parts loadout (10 parts ItemIDs)
	for (int i = 0; i < CHAR_EQUIP_PARTS_COUNT; i++)
	{
		memcpy(buffer + offset, &slot.equip.ui32PartsIds[i], 4);	offset += 4;
	}
	i3NetworkPacket packet(PROTOCOL_BASE_GET_EQUIPMENT_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&m_ui8ActiveCharaSlot, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnEquipmentReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_EQUIPMENT_REQ -> ACK
	// Client requests to equip/unequip an item
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Parse: charaSlotIdx(1) + equipSlot(1) + itemId(4)
	if (i32Size < 6)
		return;

	uint8_t charaSlot = *(uint8_t*)pData;
	uint8_t equipSlot = *(uint8_t*)(pData + 1);
	uint32_t itemId = *(uint32_t*)(pData + 2);

	int32_t result = 0;

	if (charaSlot >= MAX_CHARA_SLOT)
	{
		result = 1;	// Invalid slot
	}
	else if (equipSlot >= CHAR_EQUIP_TOTAL_COUNT)
	{
		result = 2;	// Invalid equip slot
	}
	else if (itemId != 0 && !HasInventoryItem(itemId))
	{
		result = 3;	// Item not in inventory
	}
	else
	{
		// Apply equipment change
		GameCharaSlot& slot = m_CharaSlots[charaSlot];
		if (equipSlot < CHAR_EQUIP_WEAPON_COUNT)
		{
			// Weapon slot
			slot.equip.ui32WeaponIds[equipSlot] = itemId;
		}
		else
		{
			// Parts slot
			int partsIdx = equipSlot - CHAR_EQUIP_WEAPON_COUNT;
			slot.equip.ui32PartsIds[partsIdx] = itemId;
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);
	i3NetworkPacket packet(PROTOCOL_BASE_EQUIPMENT_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&charaSlot, 1);
	packet.WriteData(&equipSlot, 1);
	packet.WriteData(&itemId, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnGetCharaInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_GET_CHARA_INFO_REQ -> ACK
	// Send all character slots info
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = 0;

	// Active slot

	// Number of character slots
	uint8_t slotCount = MAX_CHARA_SLOT;

	// Per-slot info
	for (int i = 0; i < MAX_CHARA_SLOT; i++)
	{
		const GameCharaSlot& cs = m_CharaSlots[i];
		memcpy(buffer + offset, &cs.ui8State, 1);			offset += 1;
		memcpy(buffer + offset, &cs.ui32CharaId, 4);		offset += 4;

		// Weapon IDs
		for (int w = 0; w < CHAR_EQUIP_WEAPON_COUNT; w++)
		{
			memcpy(buffer + offset, &cs.equip.ui32WeaponIds[w], 4);	offset += 4;
		}

		// Parts IDs
		for (int p = 0; p < CHAR_EQUIP_PARTS_COUNT; p++)
		{
			memcpy(buffer + offset, &cs.equip.ui32PartsIds[p], 4);	offset += 4;
		}
	}
	i3NetworkPacket packet(PROTOCOL_BASE_GET_CHARA_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&m_ui8ActiveCharaSlot, 1);
	packet.WriteData(&slotCount, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnCharaCreateReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CHAR_CREATE_CHARA_REQ -> ACK
	// Create a new character in a slot
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	if (i32Size < 5)
		return;

	uint8_t slotIdx = *(uint8_t*)pData;
	uint32_t charaItemId = *(uint32_t*)(pData + 1);

	int32_t result = 0;

	if (slotIdx >= MAX_CHARA_SLOT)
	{
		result = 1;
	}
	else if (m_CharaSlots[slotIdx].ui8State == MULTI_SLOT_NORMAL)
	{
		result = 2;	// Slot already in use
	}
	else
	{
		// Create character with default equipment
		m_CharaSlots[slotIdx].ui8State = MULTI_SLOT_NORMAL;
		m_CharaSlots[slotIdx].ui32CharaId = charaItemId;
		m_CharaSlots[slotIdx].equip.Reset();

		// Set defaults: K-2 assault rifle, K5 pistol, M7 knife, K400 grenade, smoke
		m_CharaSlots[slotIdx].equip.ui32WeaponIds[EQUIP_WEAPON_PRIMARY]		= MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, 4);		// K-2
		m_CharaSlots[slotIdx].equip.ui32WeaponIds[EQUIP_WEAPON_SECONDARY]	= MAKE_ITEM_ID(ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, 3);	// K5
		m_CharaSlots[slotIdx].equip.ui32WeaponIds[EQUIP_WEAPON_MELEE]		= MAKE_ITEM_ID(ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, 1);			// M7

		// Character body
		m_CharaSlots[slotIdx].equip.ui32PartsIds[EQUIP_PARTS_CHARA] = charaItemId;
	}

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);
	i3NetworkPacket packet(PROTOCOL_CHAR_CREATE_CHARA_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&charaItemId, 4);
	SendPacketMessage(&packet);

	if (result == 0)
		printf("[GameSession] Character created - UID=%lld, Slot=%d, CharaId=%u\n", m_i64UID, slotIdx, charaItemId);
}

void GameSession::OnCharaShiftPosReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_REQ -> ACK
	// Change active character slot
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	if (i32Size < 1)
		return;

	uint8_t newSlot = *(uint8_t*)pData;
	int32_t result = 0;

	if (newSlot >= MAX_CHARA_SLOT)
	{
		result = 1;
	}
	else if (m_CharaSlots[newSlot].ui8State != MULTI_SLOT_NORMAL)
	{
		result = 2;	// Slot not active
	}
	else
	{
		m_ui8ActiveCharaSlot = newSlot;
	}

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);
	i3NetworkPacket packet(PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&newSlot, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnCharaDeleteReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CHAR_DELETE_CHARA_REQ -> ACK
	// Delete a character from a slot
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	if (i32Size < 1)
		return;

	uint8_t slotIdx = *(uint8_t*)pData;
	int32_t result = 0;

	if (slotIdx >= MAX_CHARA_SLOT)
	{
		result = 1;	// Invalid slot
	}
	else if (m_CharaSlots[slotIdx].ui8State != MULTI_SLOT_NORMAL)
	{
		result = 2;	// Slot not active
	}
	else if (slotIdx == m_ui8ActiveCharaSlot)
	{
		result = 3;	// Cannot delete active character
	}
	else
	{
		// Clear the slot
		m_CharaSlots[slotIdx].Reset();
		printf("[GameSession] Character deleted - UID=%lld, Slot=%d\n", m_i64UID, slotIdx);
	}

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);
	i3NetworkPacket packet(PROTOCOL_CHAR_DELETE_CHARA_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnCharaChangeNickReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CHAR_CHANGE_CHARA_NICK_REQ -> ACK
	// Change character nickname (batch rename of all characters)
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Parse: slotCount(1) + per slot: slotIdx(1) + name(64)
	if (i32Size < 1)
		return;

	uint8_t slotCount = *(uint8_t*)pData;
	if (slotCount > MAX_CHARA_SLOT)
		slotCount = MAX_CHARA_SLOT;

	// Acknowledge with success
	SendSimpleAck(PROTOCOL_CHAR_CHANGE_CHARA_NICK_ACK, 0);
}

void GameSession::OnCharaChangeEquipReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CHAR_CHANGE_EQUIP_REQ -> ACK
	// Change equipment for a character slot (all slots at once, used from shop/quick equip)
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Parse: charaSlotIdx(1) + weaponIds[5](20) + partsIds[10](40) = minimum 61 bytes
	if (i32Size < 1)
		return;

	int pos = 0;
	uint8_t charaSlot = *(uint8_t*)(pData + pos);		pos += 1;
	int32_t result = 0;

	if (charaSlot >= MAX_CHARA_SLOT)
	{
		result = 1;
	}
	else if (m_CharaSlots[charaSlot].ui8State != MULTI_SLOT_NORMAL)
	{
		result = 2;	// Slot not active
	}
	else
	{
		// Parse weapon IDs if present
		if (pos + (int)(CHAR_EQUIP_WEAPON_COUNT * 4) <= i32Size)
		{
			for (int w = 0; w < CHAR_EQUIP_WEAPON_COUNT; w++)
			{
				uint32_t weaponId = *(uint32_t*)(pData + pos);
				m_CharaSlots[charaSlot].equip.ui32WeaponIds[w] = weaponId;
				pos += 4;
			}
		}

		// Parse parts IDs if present
		if (pos + (int)(CHAR_EQUIP_PARTS_COUNT * 4) <= i32Size)
		{
			for (int p = 0; p < CHAR_EQUIP_PARTS_COUNT; p++)
			{
				uint32_t partsId = *(uint32_t*)(pData + pos);
				m_CharaSlots[charaSlot].equip.ui32PartsIds[p] = partsId;
				pos += 4;
			}
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	// Echo back the equipment
	if (result == 0)
	{
		const GameCharaEquip& equip = m_CharaSlots[charaSlot].equip;
		for (int w = 0; w < CHAR_EQUIP_WEAPON_COUNT; w++)
		{
			memcpy(buffer + offset, &equip.ui32WeaponIds[w], 4);	offset += 4;
		}
		for (int p = 0; p < CHAR_EQUIP_PARTS_COUNT; p++)
		{
			memcpy(buffer + offset, &equip.ui32PartsIds[p], 4);	offset += 4;
		}
	}
	i3NetworkPacket packet(PROTOCOL_CHAR_CHANGE_EQUIP_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&charaSlot, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnRoomGetUserEquipmentReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_ROOM_GET_USER_EQUIPMENT_REQ -> ACK
	// View another player's equipment in room
	if (!m_pRoom || m_eMainTask < GAME_TASK_READY_ROOM)
		return;

	if (i32Size < 1)
		return;

	uint8_t targetSlot = *(uint8_t*)pData;
	if (targetSlot >= SLOT_MAX_COUNT)
		return;

	GameSession* pTarget = m_pRoom->GetSlotSession(targetSlot);
	if (!pTarget)
		return;

	const GameCharaEquip& equip = pTarget->GetActiveEquipment();

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);


	// Weapon IDs
	for (int w = 0; w < CHAR_EQUIP_WEAPON_COUNT; w++)
	{
		memcpy(buffer + offset, &equip.ui32WeaponIds[w], 4);	offset += 4;
	}

	// Parts IDs
	for (int p = 0; p < CHAR_EQUIP_PARTS_COUNT; p++)
	{
		memcpy(buffer + offset, &equip.ui32PartsIds[p], 4);	offset += 4;
	}
	i3NetworkPacket packet(PROTOCOL_ROOM_GET_USER_EQUIPMENT_ACK);
	packet.WriteData(&targetSlot, 1);
	SendPacketMessage(&packet);
}
