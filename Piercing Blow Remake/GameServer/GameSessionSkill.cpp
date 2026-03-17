#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"

// ============================================================================
// Skill Handlers (Protocol_Skill 0x1700)
// ============================================================================

void GameSession::OnSkillSetReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_SKILL_SET_REQ -> ACK
	// Upgrade a skill point
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 3)
		return;

	uint8_t classIdx = *(uint8_t*)pData;
	uint8_t skillCategory = *(uint8_t*)(pData + 1);	// 0=main, 1=assist, 2=common
	uint8_t skillIdx = *(uint8_t*)(pData + 2);

	int32_t result = 0;

	if (classIdx >= CHARA_CLASS_MAX)
	{
		result = 1;		// Invalid class
	}
	else
	{
		GameClassSkills& cs = m_SkillData.classSkills[classIdx];

		if (cs.ui16SkillPoints == 0)
		{
			result = 2;	// No skill points available
		}
		else
		{
			switch (skillCategory)
			{
			case 0:	// Main skill
				if (skillIdx >= MAX_MAIN_SKILLS)
					result = 3;
				else if (cs.ui8MainLevels[skillIdx] >= MAX_SKILL_LEVEL)
					result = 4;	// Already max level
				else
				{
					cs.ui8MainLevels[skillIdx]++;
					cs.ui16SkillPoints--;
					printf("[GameSession] Skill upgraded - UID=%lld, Class=%d, Main[%d]=%d\n",
						m_i64UID, classIdx, skillIdx, cs.ui8MainLevels[skillIdx]);
				}
				break;

			case 1:	// Assist skill
				if (skillIdx >= MAX_ASSIST_SKILLS)
					result = 3;
				else if (cs.ui8AssistLevels[skillIdx] >= MAX_ASSIST_SKILL_LEVEL)
					result = 4;
				else
				{
					cs.ui8AssistLevels[skillIdx]++;
					cs.ui16SkillPoints--;
				}
				break;

			case 2:	// Common skill
				if (skillIdx >= MAX_COMMON_SKILLS)
					result = 3;
				else if (cs.ui8CommonLevels[skillIdx] >= MAX_SKILL_LEVEL)
					result = 4;
				else
				{
					cs.ui8CommonLevels[skillIdx]++;
					cs.ui16SkillPoints--;
				}
				break;

			default:
				result = 3;
				break;
			}
		}
	}

	// Send ACK with updated skill data
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_SKILL_SET_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	if (result == 0 && classIdx < CHARA_CLASS_MAX)
	{
		const GameClassSkills& cs = m_SkillData.classSkills[classIdx];
		memcpy(buffer + offset, &classIdx, 1);					offset += 1;
		memcpy(buffer + offset, &cs.ui16SkillPoints, 2);		offset += 2;
		memcpy(buffer + offset, cs.ui8MainLevels, MAX_MAIN_SKILLS);		offset += MAX_MAIN_SKILLS;
		memcpy(buffer + offset, cs.ui8AssistLevels, MAX_ASSIST_SKILLS);	offset += MAX_ASSIST_SKILLS;
		memcpy(buffer + offset, cs.ui8CommonLevels, MAX_COMMON_SKILLS);	offset += MAX_COMMON_SKILLS;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnSkillResetReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_SKILL_RESET_REQ -> ACK
	// Reset all skills for a class (costs GP)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 1)
		return;

	uint8_t classIdx = *(uint8_t*)pData;
	int32_t result = 0;

	if (classIdx >= CHARA_CLASS_MAX)
	{
		result = 1;
	}
	else
	{
		// Reset cost: 5000 GP
		const int RESET_COST = 5000;
		if (m_i32GP < RESET_COST)
		{
			result = 2;	// Not enough GP
		}
		else
		{
			m_i32GP -= RESET_COST;

			GameClassSkills& cs = m_SkillData.classSkills[classIdx];
			// Refund all spent points
			int refund = cs.GetTotalSkillPoints();
			cs.ui16SkillPoints += (uint16_t)refund;
			memset(cs.ui8MainLevels, 0, sizeof(cs.ui8MainLevels));
			memset(cs.ui8AssistLevels, 0, sizeof(cs.ui8AssistLevels));
			memset(cs.ui8CommonLevels, 0, sizeof(cs.ui8CommonLevels));

			printf("[GameSession] Skills reset - UID=%lld, Class=%d, Refund=%d, GP=%d\n",
				m_i64UID, classIdx, refund, m_i32GP);
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_SKILL_RESET_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	if (result == 0 && classIdx < CHARA_CLASS_MAX)
	{
		const GameClassSkills& cs = m_SkillData.classSkills[classIdx];
		memcpy(buffer + offset, &classIdx, 1);					offset += 1;
		memcpy(buffer + offset, &cs.ui16SkillPoints, 2);		offset += 2;
		memcpy(buffer + offset, cs.ui8MainLevels, MAX_MAIN_SKILLS);		offset += MAX_MAIN_SKILLS;
		memcpy(buffer + offset, cs.ui8AssistLevels, MAX_ASSIST_SKILLS);	offset += MAX_ASSIST_SKILLS;
		memcpy(buffer + offset, cs.ui8CommonLevels, MAX_COMMON_SKILLS);	offset += MAX_COMMON_SKILLS;

		uint32_t gp = (uint32_t)m_i32GP;
		memcpy(buffer + offset, &gp, sizeof(uint32_t));		offset += sizeof(uint32_t);
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnSkillClassSetReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_SKILL_CLASS_SET_REQ -> ACK
	// Set active character class
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 1)
		return;

	uint8_t classIdx = *(uint8_t*)pData;
	int32_t result = 0;

	if (classIdx >= CHARA_CLASS_MAX)
	{
		result = 1;
	}
	else
	{
		m_SkillData.ui8ActiveClass = classIdx;
		printf("[GameSession] Active class set - UID=%lld, Class=%d\n", m_i64UID, classIdx);
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_SKILL_CLASS_SET_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &classIdx, 1);					offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnSkillGetInfoReq(char* pData, INT32 i32Size)
{
	// Send all skill info for all classes
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_SKILL_SET_ACK;	// Reuse SET_ACK for info response
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Active class
	memcpy(buffer + offset, &m_SkillData.ui8ActiveClass, 1);	offset += 1;

	// All classes
	uint8_t classCount = CHARA_CLASS_MAX;
	memcpy(buffer + offset, &classCount, 1);				offset += 1;

	for (int i = 0; i < CHARA_CLASS_MAX; i++)
	{
		const GameClassSkills& cs = m_SkillData.classSkills[i];
		memcpy(buffer + offset, &cs.ui8Class, 1);				offset += 1;
		memcpy(buffer + offset, &cs.ui16SkillPoints, 2);		offset += 2;
		memcpy(buffer + offset, cs.ui8MainLevels, MAX_MAIN_SKILLS);		offset += MAX_MAIN_SKILLS;
		memcpy(buffer + offset, cs.ui8AssistLevels, MAX_ASSIST_SKILLS);	offset += MAX_ASSIST_SKILLS;
		memcpy(buffer + offset, cs.ui8CommonLevels, MAX_COMMON_SKILLS);	offset += MAX_COMMON_SKILLS;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// New Speciality Handlers (Protocol_New 0x1100 - mapped to skill system)
// ============================================================================

void GameSession::OnNewSpecialitySetReq(char* pData, INT32 i32Size)
{
	// Set speciality skill levels for a specific class
	// Client sends: classId(1) + skillType(1) + skillIdx(1) + newLevel(1)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_BASE_NEW_SPECIALITY_SET_ACK, -1);
		return;
	}

	uint8_t classId = *(uint8_t*)pData;
	uint8_t skillType = *(uint8_t*)(pData + 1);	// 0=main, 1=assist, 2=common
	uint8_t skillIdx = *(uint8_t*)(pData + 2);
	uint8_t newLevel = *(uint8_t*)(pData + 3);

	int32_t result = 0;

	if (classId >= CHARA_CLASS_MAX)
	{
		result = -2;
	}
	else
	{
		GameClassSkills& cs = m_SkillData.classSkills[classId];

		if (skillType == 0 && skillIdx < MAX_MAIN_SKILLS)
		{
			if (newLevel > MAX_SKILL_LEVEL) newLevel = MAX_SKILL_LEVEL;
			uint8_t diff = (newLevel > cs.ui8MainLevels[skillIdx]) ?
				(newLevel - cs.ui8MainLevels[skillIdx]) : 0;
			if (diff > cs.ui16SkillPoints)
				result = -3;
			else
			{
				cs.ui16SkillPoints -= diff;
				cs.ui8MainLevels[skillIdx] = newLevel;
			}
		}
		else if (skillType == 1 && skillIdx < MAX_ASSIST_SKILLS)
		{
			if (newLevel > MAX_SKILL_LEVEL) newLevel = MAX_SKILL_LEVEL;
			uint8_t diff = (newLevel > cs.ui8AssistLevels[skillIdx]) ?
				(newLevel - cs.ui8AssistLevels[skillIdx]) : 0;
			if (diff > cs.ui16SkillPoints)
				result = -3;
			else
			{
				cs.ui16SkillPoints -= diff;
				cs.ui8AssistLevels[skillIdx] = newLevel;
			}
		}
		else if (skillType == 2 && skillIdx < MAX_COMMON_SKILLS)
		{
			if (newLevel > MAX_SKILL_LEVEL) newLevel = MAX_SKILL_LEVEL;
			uint8_t diff = (newLevel > cs.ui8CommonLevels[skillIdx]) ?
				(newLevel - cs.ui8CommonLevels[skillIdx]) : 0;
			if (diff > cs.ui16SkillPoints)
				result = -3;
			else
			{
				cs.ui16SkillPoints -= diff;
				cs.ui8CommonLevels[skillIdx] = newLevel;
			}
		}
		else
		{
			result = -4;
		}
	}

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_NEW_SPECIALITY_SET_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &classId, 1);				offset += 1;
	memcpy(buffer + offset, &skillType, 1);				offset += 1;
	memcpy(buffer + offset, &skillIdx, 1);				offset += 1;
	memcpy(buffer + offset, &newLevel, 1);				offset += 1;

	if (result == 0)
	{
		uint16_t remainPts = m_SkillData.classSkills[classId].ui16SkillPoints;
		memcpy(buffer + offset, &remainPts, 2);			offset += 2;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnNewSpecialityInitReq(char* pData, INT32 i32Size)
{
	// Reset all skill levels for a class, refund points
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_BASE_NEW_SPECIALITY_INIT_ACK, -1);
		return;
	}

	uint8_t classId = *(uint8_t*)pData;
	int32_t result = 0;

	if (classId >= CHARA_CLASS_MAX)
	{
		result = -2;
	}
	else
	{
		const int SKILL_RESET_COST = 5000;
		if (m_i32GP < SKILL_RESET_COST)
		{
			result = -3;
		}
		else
		{
			m_i32GP -= SKILL_RESET_COST;
			GameClassSkills& cs = m_SkillData.classSkills[classId];
			int refund = cs.GetTotalSkillPoints();
			memset(cs.ui8MainLevels, 0, sizeof(cs.ui8MainLevels));
			memset(cs.ui8AssistLevels, 0, sizeof(cs.ui8AssistLevels));
			memset(cs.ui8CommonLevels, 0, sizeof(cs.ui8CommonLevels));
			cs.ui16SkillPoints += (uint16_t)refund;
		}
	}

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_NEW_SPECIALITY_INIT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &classId, 1);				offset += 1;

	if (result == 0)
	{
		uint16_t remainPts = m_SkillData.classSkills[classId].ui16SkillPoints;
		int32_t remainGP = m_i32GP;
		memcpy(buffer + offset, &remainPts, 2);			offset += 2;
		memcpy(buffer + offset, &remainGP, 4);			offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnNewSpecialitySetClassReq(char* pData, INT32 i32Size)
{
	// Change active speciality class
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_BASE_NEW_SPECIALITY_SET_CLASS_ACK, -1);
		return;
	}

	uint8_t classId = *(uint8_t*)pData;
	int32_t result = 0;

	if (classId >= CHARA_CLASS_MAX)
		result = -2;
	else
		m_SkillData.ui8ActiveClass = classId;

	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_NEW_SPECIALITY_SET_CLASS_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &classId, 1);				offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
