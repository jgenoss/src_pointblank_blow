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
