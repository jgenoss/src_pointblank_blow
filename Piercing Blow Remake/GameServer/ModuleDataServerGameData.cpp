#include "pch.h"
#include "ModuleDataServer.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameServerContext.h"

// ============================================================================
// Request operations - Equipment
// ============================================================================

void ModuleDataServer::RequestEquipmentSave(int64_t i64UID, uint8_t ui8CharaSlot, uint8_t ui8SlotState,
											 uint32_t ui32CharaId, const uint32_t* pWeaponIds, const uint32_t* pPartsIds)
{
	IS_EQUIPMENT_SAVE_REQ req;
	req.i64UID = i64UID;
	req.ui8CharaSlot = ui8CharaSlot;
	req.ui8SlotState = ui8SlotState;
	req.ui32CharaId = ui32CharaId;
	memcpy(req.ui32WeaponIds, pWeaponIds, sizeof(req.ui32WeaponIds));
	memcpy(req.ui32PartsIds, pPartsIds, sizeof(req.ui32PartsIds));
	SendRequest(PROTOCOL_IS_EQUIPMENT_SAVE_REQ, &req, sizeof(req));
}

// ============================================================================
// Request operations - Medal
// ============================================================================

void ModuleDataServer::RequestMedalSave(int64_t i64UID, uint16_t ui16MedalIdx, uint8_t ui8ActionId,
										 uint16_t ui16Count, uint8_t ui8GetReward)
{
	IS_MEDAL_SAVE_REQ req;
	req.i64UID = i64UID;
	req.ui16MedalIdx = ui16MedalIdx;
	req.ui8ActionId = ui8ActionId;
	req.ui16Count = ui16Count;
	req.ui8GetReward = ui8GetReward;
	SendRequest(PROTOCOL_IS_MEDAL_SAVE_REQ, &req, sizeof(req));
}

// ============================================================================
// Request operations - Attendance
// ============================================================================

void ModuleDataServer::RequestAttendanceSave(int64_t i64UID, int i32TotalDays, int i32CurrentStreak,
											  uint32_t ui32LastDate, const uint8_t* pDays)
{
	IS_ATTENDANCE_SAVE_REQ req;
	req.i64UID = i64UID;
	req.i32TotalDays = i32TotalDays;
	req.i32CurrentStreak = i32CurrentStreak;
	req.ui32LastAttendDate = ui32LastDate;
	if (pDays)
		memcpy(req.ui8Days, pDays, sizeof(req.ui8Days));
	else
		memset(req.ui8Days, 0, sizeof(req.ui8Days));
	SendRequest(PROTOCOL_IS_ATTENDANCE_SAVE_REQ, &req, sizeof(req));
}

// ============================================================================
// Request operations - Skill
// ============================================================================

void ModuleDataServer::RequestSkillSave(int64_t i64UID, uint8_t ui8ClassId,
										 const uint8_t* pMainLevels, const uint8_t* pAssistLevels,
										 const uint8_t* pCommonLevels, uint16_t ui16Points)
{
	IS_SKILL_SAVE_REQ req;
	req.i64UID = i64UID;
	req.ui8ClassId = ui8ClassId;
	memcpy(req.ui8MainLevels, pMainLevels, sizeof(req.ui8MainLevels));
	memcpy(req.ui8AssistLevels, pAssistLevels, sizeof(req.ui8AssistLevels));
	memcpy(req.ui8CommonLevels, pCommonLevels, sizeof(req.ui8CommonLevels));
	req.ui16SkillPoints = ui16Points;
	SendRequest(PROTOCOL_IS_SKILL_SAVE_REQ, &req, sizeof(req));
}

// ============================================================================
// Request operations - Quest
// ============================================================================

void ModuleDataServer::RequestQuestSave(int64_t i64UID, uint8_t ui8SetIndex, uint8_t ui8SetType,
										 uint8_t ui8ActiveCard, const char* pQuestData, uint16_t ui16DataSize)
{
	if (!IsConnected())
		return;

	// Quest has variable-length payload after the struct
	offset += sizeof(uint16_t);

	IS_QUEST_SAVE_REQ req;
	req.i64UID = i64UID;
	req.ui8SetIndex = ui8SetIndex;
	req.ui8SetType = ui8SetType;
	req.ui8ActiveCard = ui8ActiveCard;
	req.ui16DataSize = ui16DataSize;


	if (pQuestData && ui16DataSize > 0 && offset + ui16DataSize < (int)sizeof(buffer))
	{
		memcpy(buffer + offset, pQuestData, ui16DataSize);
		offset += ui16DataSize;
	}
	i3NetworkPacket packet(PROTOCOL_IS_QUEST_SAVE_REQ);
	packet.WriteData(&req, sizeof(req));
	SendPacket(&packet);
}

// ============================================================================
// Response handlers - Game data save ACKs
// ============================================================================

void ModuleDataServer::OnEquipmentSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_EQUIPMENT_SAVE_ACK))
		return;
	IS_EQUIPMENT_SAVE_ACK* pAck = (IS_EQUIPMENT_SAVE_ACK*)pData;
	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Equipment save failed - UID=%lld\n", pAck->i64UID);
}

void ModuleDataServer::OnMedalSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_MEDAL_SAVE_ACK))
		return;
	IS_MEDAL_SAVE_ACK* pAck = (IS_MEDAL_SAVE_ACK*)pData;
	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Medal save failed - UID=%lld\n", pAck->i64UID);
}

void ModuleDataServer::OnAttendanceSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_ATTENDANCE_SAVE_ACK))
		return;
	IS_ATTENDANCE_SAVE_ACK* pAck = (IS_ATTENDANCE_SAVE_ACK*)pData;
	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Attendance save failed - UID=%lld\n", pAck->i64UID);
}

void ModuleDataServer::OnSkillSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_SKILL_SAVE_ACK))
		return;
	IS_SKILL_SAVE_ACK* pAck = (IS_SKILL_SAVE_ACK*)pData;
	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Skill save failed - UID=%lld\n", pAck->i64UID);
}

void ModuleDataServer::OnQuestSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_QUEST_SAVE_ACK))
		return;
	IS_QUEST_SAVE_ACK* pAck = (IS_QUEST_SAVE_ACK*)pData;
	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Quest save failed - UID=%lld\n", pAck->i64UID);
}

// ============================================================================
// Request operations - Options
// ============================================================================

void ModuleDataServer::RequestOptionSave(int64_t i64UID, const char* pOptionsData, uint16_t ui16DataSize)
{
	if (!IsConnected() || !pOptionsData || ui16DataSize == 0)
		return;

	offset += sizeof(uint16_t);

	IS_OPTION_SAVE_REQ req;
	req.i64UID = i64UID;
	req.ui16DataSize = ui16DataSize;


	if (offset + ui16DataSize < (int)sizeof(buffer))
	{
		memcpy(buffer + offset, pOptionsData, ui16DataSize);
		offset += ui16DataSize;
	}
	i3NetworkPacket packet(PROTOCOL_IS_OPTION_SAVE_REQ);
	packet.WriteData(&req, sizeof(req));
	SendPacket(&packet);
}

void ModuleDataServer::OnOptionSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_OPTION_SAVE_ACK))
		return;
	IS_OPTION_SAVE_ACK* pAck = (IS_OPTION_SAVE_ACK*)pData;
	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Option save failed - UID=%lld\n", pAck->i64UID);
}

void ModuleDataServer::RequestOptionLoad(int64_t i64UID, int i32SessionIdx)
{
	if (!IsConnected())
		return;

	IS_OPTION_LOAD_REQ req;
	req.i64UID       = i64UID;
	req.i32SessionIdx = i32SessionIdx;

	SendRequest(PROTOCOL_IS_OPTION_LOAD_REQ, &req, sizeof(req));
}

void ModuleDataServer::OnOptionLoadAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_OPTION_LOAD_ACK))
		return;

	IS_OPTION_LOAD_ACK* pAck = (IS_OPTION_LOAD_ACK*)pData;

	GameSession* pSession = g_pGameSessionManager
		? g_pGameSessionManager->GetSessionByIndex(pAck->i32SessionIdx)
		: nullptr;
	if (!pSession)
		return;

	// Send PROTOCOL_BASE_GET_OPTION_ACK to client
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = 0;

	if (pAck->i32Result == 0 && pAck->ui16DataSize > 0)
	{
		// Send saved options blob
		const char* pBlob = pData + sizeof(IS_OPTION_LOAD_ACK);
		uint16_t blobSize  = pAck->ui16DataSize;
		if (offset + blobSize <= (int)sizeof(buffer))
		{
			memcpy(buffer + offset, pBlob, blobSize);
			offset += blobSize;
		}
	}
	else
	{
		// No saved options — send 128 bytes of zeroed defaults
		memset(buffer + offset, 0, 128);
		offset += 128;
	}
	i3NetworkPacket packet(PROTOCOL_BASE_GET_OPTION_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	pSession->SendPacketMessage(&packet);
}

void ModuleDataServer::RequestMedalSetLoad(int64_t i64UID, int i32SessionIdx)
{
	if (!IsConnected())
		return;

	IS_MEDAL_SET_LOAD_REQ req;
	req.i64UID        = i64UID;
	req.i32SessionIdx = i32SessionIdx;

	SendRequest(PROTOCOL_IS_MEDAL_SET_LOAD_REQ, &req, sizeof(req));
}

void ModuleDataServer::OnMedalSetLoadAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_MEDAL_SET_LOAD_ACK))
		return;

	IS_MEDAL_SET_LOAD_ACK* pAck = (IS_MEDAL_SET_LOAD_ACK*)pData;

	GameSession* pSession = g_pGameSessionManager
		? g_pGameSessionManager->GetSessionByIndex(pAck->i32SessionIdx)
		: nullptr;
	if (!pSession)
		return;

	int i32Count = (int)pAck->ui16Count;
	const IS_MEDAL_SET_CURRENT_ENTRY* pSets = (i32Count > 0)
		? (const IS_MEDAL_SET_CURRENT_ENTRY*)(pData + sizeof(IS_MEDAL_SET_LOAD_ACK))
		: nullptr;

	// Send PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK to client
	int32_t result = 0;
	uint16_t totalCount = (uint16_t)i32Count;
	uint16_t count      = (uint16_t)i32Count;
	uint16_t startIdx   = 0;

	i3NetworkPacket packet(PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&totalCount, sizeof(uint16_t));
	packet.WriteData(&count, sizeof(uint16_t));
	packet.WriteData(&startIdx, sizeof(uint16_t));

	for (int i = 0; i < i32Count && pSets; i++)
	{
		const IS_MEDAL_SET_CURRENT_ENTRY& s = pSets[i];
		uint32_t recvDate = 0;
		packet.WriteData(&s.ui8SetType, sizeof(uint8_t));
		packet.WriteData(&s.i16MedalSetIdx, sizeof(int16_t));
		packet.WriteData(&recvDate, sizeof(uint32_t));
		packet.WriteData(&s.i16Medal1Count, sizeof(int16_t));
		packet.WriteData(&s.i16Medal2Count, sizeof(int16_t));
		packet.WriteData(&s.i16Medal3Count, sizeof(int16_t));
		packet.WriteData(&s.i16Medal4Count, sizeof(int16_t));
		packet.WriteData(&s.i16Medal5Count, sizeof(int16_t));
		packet.WriteData(&s.i16Medal6Count, sizeof(int16_t));
		packet.WriteData(&s.ui8GetReward, sizeof(uint8_t));
	}

	pSession->SendPacketMessage(&packet);
}
