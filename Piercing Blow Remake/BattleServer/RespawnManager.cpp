#include "pch.h"
#include "RespawnManager.h"

RespawnManager::RespawnManager()
{
}

RespawnManager::~RespawnManager()
{
}

bool RespawnManager::GetSpawnPoint(CMapData* pMapData, int team, float* outPos) const
{
	if (!pMapData || !outPos)
		return false;

	const DS_RESPAWN_OBJECT* pSpawn = pMapData->GetRandomSpawnForTeam(team);
	if (!pSpawn)
	{
		// Fallback: try any respawn point
		if (pMapData->GetRespawnObjectCount() > 0)
		{
			pSpawn = &pMapData->GetRespawnObjects()[0];
		}
		else
		{
			// No respawn data at all - use origin
			outPos[0] = 0.0f;
			outPos[1] = 0.0f;
			outPos[2] = 0.0f;
			return false;
		}
	}

	outPos[0] = pSpawn->m_vPos[0];
	outPos[1] = pSpawn->m_vPos[1];
	outPos[2] = pSpawn->m_vPos[2];
	return true;
}
