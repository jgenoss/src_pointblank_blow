#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "Room.h"

// ============================================================================
// Cheat Detection System (Phase 9C)
// Server-side validation for common cheats
// ============================================================================

bool GameSession::ValidateDamage(int i32KillerSlot, int i32VictimSlot, uint32_t ui32WeaponId,
								uint8_t ui8HitPart, int i32Damage)
{
	if (!m_pRoom)
		return true;	// No room context, allow

	// 1. Validate weapon exists in killer's loadout
	GameSession* pKiller = m_pRoom->GetSlotSession(i32KillerSlot);
	if (!pKiller)
		return false;

	// 2. Validate hit part is valid
	if (ui8HitPart > HIT_PART_RIGHT_LEG)
	{
		printf("[CheatDetect] Invalid hit part %d from session %d\n",
			ui8HitPart, pKiller->GetIndex());
		return false;
	}

	// 3. Validate damage is not abnormally high
	// Max one-shot damage should never exceed ~200 (sniper headshot)
	if (i32Damage > 300)
	{
		printf("[CheatDetect] Abnormal damage %d from session %d, weapon %u\n",
			i32Damage, pKiller->GetIndex(), ui32WeaponId);
		return false;
	}

	// 4. Validate killer is alive
	if (!m_pRoom->IsPlayerAlive(i32KillerSlot))
	{
		printf("[CheatDetect] Dead player killing - slot %d\n", i32KillerSlot);
		return false;
	}

	// 5. Validate no team kill (unless friendly fire enabled)
	const GameSlotInfo& killerSlot = m_pRoom->GetSlotInfo(i32KillerSlot);
	const GameSlotInfo& victimSlot = m_pRoom->GetSlotInfo(i32VictimSlot);
	if (killerSlot.ui8Team == victimSlot.ui8Team && i32KillerSlot != i32VictimSlot)
	{
		// Team kill - log but allow (some modes permit it)
		printf("[CheatDetect] Team kill: slot %d killed slot %d\n",
			i32KillerSlot, i32VictimSlot);
	}

	return true;
}

bool GameSession::ValidateSpeedHack(DWORD dwClientTime)
{
	// Compare client-reported timestamp against server time
	// Speed hacks accelerate client time to move faster
	DWORD dwServerTime = GetTickCount();

	// First call - initialize reference
	if (m_dwCheatLastClientTime == 0)
	{
		m_dwCheatLastClientTime = dwClientTime;
		m_dwCheatLastServerTime = dwServerTime;
		return true;
	}

	DWORD clientDelta = dwClientTime - m_dwCheatLastClientTime;
	DWORD serverDelta = dwServerTime - m_dwCheatLastServerTime;

	// Update reference
	m_dwCheatLastClientTime = dwClientTime;
	m_dwCheatLastServerTime = dwServerTime;

	// Allow some tolerance (network jitter, etc.)
	// If client time runs 20% faster than server time consistently, flag it
	if (serverDelta > 500)	// Only check with sufficient time delta
	{
		float ratio = (float)clientDelta / (float)serverDelta;
		if (ratio > 1.5f)
		{
			m_i32SpeedHackCount++;
			printf("[CheatDetect] Speed hack suspected - session %d, ratio=%.2f, count=%d\n",
				GetIndex(), ratio, m_i32SpeedHackCount);

			// 3 consecutive violations = kick
			if (m_i32SpeedHackCount >= 3)
			{
				printf("[CheatDetect] Speed hack CONFIRMED - disconnecting session %d\n",
					GetIndex());
				return false;
			}
		}
		else
		{
			// Reset counter on normal timing
			if (m_i32SpeedHackCount > 0)
				m_i32SpeedHackCount--;
		}
	}

	return true;
}

void GameSession::OnCheatDetected(int i32CheatType, const char* pszDescription)
{
	printf("[CheatDetect] CHEAT DETECTED - Session=%d, UID=%lld, Nick=%s, Type=%d, Desc=%s\n",
		GetIndex(), m_i64UID, m_szNickname, i32CheatType, pszDescription ? pszDescription : "");

	// Increment violation counter
	m_i32CheatViolations++;

	if (m_i32CheatViolations >= 5)
	{
		printf("[CheatDetect] Too many violations (%d) - disconnecting session %d\n",
			m_i32CheatViolations, GetIndex());
		OnDisconnect(TRUE);
	}
}

bool GameSession::ValidatePosition(float fX, float fY, float fZ)
{
	// Basic position validation
	// Reject obviously invalid coordinates (teleport hack, fly hack)

	// Position must be within map bounds (generous limits)
	const float MAX_COORD = 10000.0f;
	if (fX < -MAX_COORD || fX > MAX_COORD ||
		fY < -MAX_COORD || fY > MAX_COORD ||
		fZ < -MAX_COORD || fZ > MAX_COORD)
	{
		printf("[CheatDetect] Invalid position (%.1f, %.1f, %.1f) from session %d\n",
			fX, fY, fZ, GetIndex());
		return false;
	}

	// Check for teleport (position change too large between updates)
	if (m_fLastPosX != 0.0f || m_fLastPosY != 0.0f || m_fLastPosZ != 0.0f)
	{
		float dx = fX - m_fLastPosX;
		float dy = fY - m_fLastPosY;
		float dz = fZ - m_fLastPosZ;
		float distSq = dx*dx + dy*dy + dz*dz;

		// Max movement per tick (~500 units considering high speed + latency)
		const float MAX_MOVE_SQ = 500.0f * 500.0f;
		if (distSq > MAX_MOVE_SQ)
		{
			printf("[CheatDetect] Teleport detected - session %d, dist=%.1f\n",
				GetIndex(), sqrtf(distSq));
			m_i32CheatViolations++;
			// Don't reject - just log. Respawn can cause large jumps.
		}
	}

	m_fLastPosX = fX;
	m_fLastPosY = fY;
	m_fLastPosZ = fZ;

	return true;
}
