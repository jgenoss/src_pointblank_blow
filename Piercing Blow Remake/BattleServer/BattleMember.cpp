#include "pch.h"
#include "BattleMember.h"

BattleMember::BattleMember()
{
	Init();
}

BattleMember::~BattleMember()
{
}

void BattleMember::Init()
{
	m_bActive			= false;
	m_eState			= BATTLE_MEMBER_NONE;
	m_i64UID			= 0;
	m_ui32IP			= 0;
	m_ui16Port			= 0;
	m_i32Team			= BATTLE_TEAM_RED;
	m_i32SlotIdx		= -1;
	m_i32Kills			= 0;
	m_i32Deaths			= 0;
	m_i32Headshots		= 0;
	m_bAlive			= false;
	m_dwLastPacketTime	= 0;

	// Character state
	m_fPos[0] = m_fPos[1] = m_fPos[2] = 0.0f;
	m_fRot[0] = m_fRot[1] = m_fRot[2] = 0.0f;
	m_i32HP				= 100;
	m_i32MaxHP			= 100;
	m_dwLastPosUpdate	= 0;
	m_dwDeathTime		= 0;
	m_i32RespawnCount	= 0;
	m_ui32WeaponID		= 0;
}

bool BattleMember::Join(int64_t i64UID, uint32_t ui32IP, uint16_t ui16Port, int i32Team, int i32Slot)
{
	if (m_bActive)
		return false;

	m_bActive			= true;
	m_eState			= BATTLE_MEMBER_JOINED;
	m_i64UID			= i64UID;
	m_ui32IP			= ui32IP;
	m_ui16Port			= ui16Port;
	m_i32Team			= i32Team;
	m_i32SlotIdx		= i32Slot;
	m_i32Kills			= 0;
	m_i32Deaths			= 0;
	m_i32Headshots		= 0;
	m_bAlive			= true;
	m_dwLastPacketTime	= GetTickCount();

	return true;
}

void BattleMember::Leave()
{
	m_bActive	= false;
	m_eState	= BATTLE_MEMBER_LEFT;
}

bool BattleMember::IsSameAddress(uint32_t ui32IP, uint16_t ui16Port) const
{
	return (m_ui32IP == ui32IP && m_ui16Port == ui16Port);
}

void BattleMember::FillPlayerResult(BattlePlayerResult* pResult) const
{
	if (!pResult)
		return;

	pResult->i64UID			= m_i64UID;
	pResult->i32Kills		= m_i32Kills;
	pResult->i32Deaths		= m_i32Deaths;
	pResult->i32Headshots	= m_i32Headshots;
	pResult->i32Team		= m_i32Team;
}

// ============================================================================
// Character State (Phase 11)
// ============================================================================

void BattleMember::SetPosition(float x, float y, float z)
{
	m_fPos[0] = x;
	m_fPos[1] = y;
	m_fPos[2] = z;
	m_dwLastPosUpdate = GetTickCount();
}

void BattleMember::SetRotation(float rx, float ry, float rz)
{
	m_fRot[0] = rx;
	m_fRot[1] = ry;
	m_fRot[2] = rz;
}

void BattleMember::ApplyDamage(int dmg)
{
	if (!m_bAlive || dmg <= 0)
		return;

	m_i32HP -= dmg;
	if (m_i32HP <= 0)
	{
		m_i32HP = 0;
		OnDeath();
	}
}

void BattleMember::OnDeath()
{
	m_bAlive = false;
	m_eState = BATTLE_MEMBER_DEAD;
	m_dwDeathTime = GetTickCount();
	m_i32Deaths++;
}

void BattleMember::OnRespawn(const float* spawnPos)
{
	m_bAlive = true;
	m_eState = BATTLE_MEMBER_PLAYING;
	m_i32HP = m_i32MaxHP;
	m_i32RespawnCount++;
	m_dwDeathTime = 0;

	if (spawnPos)
	{
		m_fPos[0] = spawnPos[0];
		m_fPos[1] = spawnPos[1];
		m_fPos[2] = spawnPos[2];
	}

	m_dwLastPosUpdate = GetTickCount();
}
