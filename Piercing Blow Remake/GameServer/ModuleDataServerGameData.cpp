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
	char buffer[8192];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_QUEST_SAVE_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_QUEST_SAVE_REQ req;
	req.i64UID = i64UID;
	req.ui8SetIndex = ui8SetIndex;
	req.ui8SetType = ui8SetType;
	req.ui8ActiveCard = ui8ActiveCard;
	req.ui16DataSize = ui16DataSize;

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	if (pQuestData && ui16DataSize > 0 && offset + ui16DataSize < (int)sizeof(buffer))
	{
		memcpy(buffer + offset, pQuestData, ui16DataSize);
		offset += ui16DataSize;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	i3NetworkPacket packet;
	packet.SetPacketData(buffer, offset);
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
