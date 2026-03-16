#ifndef __MODULEDBGAMEDATA_H__
#define __MODULEDBGAMEDATA_H__

#pragma once
#include "InterServerProtocol.h"

class DBConnectionPool;
class DataServerContext;

// Modulo de operaciones de datos de juego
// Maneja: equipment, medals, attendance, skills, quests
// Patron sincronico v1 (sin i3Thread)
class ModuleDBGameData
{
public:
	ModuleDBGameData(DBConnectionPool* pPool);
	~ModuleDBGameData();

	// Equipment
	bool SaveEquipment(int64_t i64UID, IS_EQUIPMENT_SAVE_REQ* pReq);

	// Medals
	bool SaveMedal(int64_t i64UID, IS_MEDAL_SAVE_REQ* pReq);

	// Attendance
	bool SaveAttendance(int64_t i64UID, IS_ATTENDANCE_SAVE_REQ* pReq);

	// Skills
	bool SaveSkill(int64_t i64UID, IS_SKILL_SAVE_REQ* pReq);

	// Quests
	bool SaveQuest(int64_t i64UID, IS_QUEST_SAVE_REQ* pReq, const char* pQuestData, int i32DataSize);

	// Placeholder para futuro async
	void ProcessResponses(DataServerContext* pContext);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBGAMEDATA_H__
