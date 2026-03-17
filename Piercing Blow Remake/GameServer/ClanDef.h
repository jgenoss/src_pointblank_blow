#ifndef __CLANDEF_GAME_H__
#define __CLANDEF_GAME_H__

#pragma once

// Simplified clan system for GameServer remake
// Original uses a separate ClanServer with full DB. This is in-memory.

#define MAX_CLAN_MEMBERS		50		// Starting member limit
#define MAX_CLAN_NAME_LEN		32
#define MAX_CLAN_NOTICE_LEN		256
#define MAX_CLAN_INTRO_LEN		128
#define MAX_CLANS				100		// Max clans in-memory
#define CLAN_CREATE_COST_GP		10000	// GP cost to create clan
#define MAX_CLAN_JOIN_REQUESTS	20		// Pending join request queue size

// Clan member roles
enum GameClanMemberLevel
{
	CLAN_MEMBER_UNKNOWN = 0,
	CLAN_MEMBER_MASTER = 1,
	CLAN_MEMBER_STAFF = 2,
	CLAN_MEMBER_REGULAR = 3,
	CLAN_MEMBER_MAX,
};

// Clan unit size tiers
enum GameClanUnit
{
	CLAN_UNIT_SQUAD = 0,		// Up to 10
	CLAN_UNIT_PLATOON,			// Up to 30
	CLAN_UNIT_COMPANY,			// Up to 50
	CLAN_UNIT_BATTALION,		// Up to 100
	CLAN_UNIT_REGIMENT,			// Up to 150
	CLAN_UNIT_BRIGADE,			// Up to 200
	CLAN_UNIT_DIVISION,			// Up to 250
	CLAN_UNIT_CORPS,			// 250 max
	CLAN_UNIT_MAX,
};

#pragma pack(push, 1)

// Pending join request (queued until master accepts or denies)
struct ClanJoinRequest
{
	int64_t		i64UID;
	char		szNickname[64];
	int			i32Level;
	int			i32RankId;
	DWORD		dwRequestTime;	// GetTickCount() when submitted
	bool		bActive;

	void Reset()
	{
		i64UID       = 0;
		szNickname[0] = '\0';
		i32Level     = 0;
		i32RankId    = 0;
		dwRequestTime = 0;
		bActive      = false;
	}
};

// Clan member info
struct GameClanMember
{
	int64_t		i64UID;
	char		szNickname[64];
	uint8_t		ui8Level;			// GameClanMemberLevel
	int			i32PlayerLevel;		// Player level
	int			i32RankId;
	bool		bOnline;

	void Reset()
	{
		i64UID = 0;
		szNickname[0] = '\0';
		ui8Level = CLAN_MEMBER_UNKNOWN;
		i32PlayerLevel = 0;
		i32RankId = 0;
		bOnline = false;
	}

	bool IsActive() const { return i64UID != 0; }
};

// Clan info
struct GameClanInfo
{
	int			i32ClanId;
	char		szName[MAX_CLAN_NAME_LEN];
	char		szNotice[MAX_CLAN_NOTICE_LEN];
	char		szIntro[MAX_CLAN_INTRO_LEN];
	int64_t		i64MasterUID;
	char		szMasterNickname[64];
	int			i32MemberCount;
	int			i32MaxMembers;
	int			i32ClanExp;
	int			i32ClanRank;
	int			i32Wins;
	int			i32Losses;
	uint8_t		ui8Unit;			// GameClanUnit
	uint16_t	ui16MarkId;			// Clan emblem
	uint8_t		ui8MarkColor;
	bool		bActive;

	GameClanMember		members[MAX_CLAN_MEMBERS];
	ClanJoinRequest		joinRequests[MAX_CLAN_JOIN_REQUESTS];
	int					i32JoinRequestCount;

	void Reset()
	{
		i32ClanId = 0;
		szName[0] = '\0';
		szNotice[0] = '\0';
		szIntro[0] = '\0';
		i64MasterUID = 0;
		szMasterNickname[0] = '\0';
		i32MemberCount = 0;
		i32MaxMembers = 10;
		i32ClanExp = 0;
		i32ClanRank = 0;
		i32Wins = 0;
		i32Losses = 0;
		ui8Unit = CLAN_UNIT_SQUAD;
		ui16MarkId = 0;
		ui8MarkColor = 0;
		bActive = false;
		for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
			members[i].Reset();
		i32JoinRequestCount = 0;
		for (int i = 0; i < MAX_CLAN_JOIN_REQUESTS; i++)
			joinRequests[i].Reset();
	}

	// Join request helpers
	bool AddJoinRequest(int64_t uid, const char* nick, int level, int rankId)
	{
		// Reject duplicates
		for (int i = 0; i < MAX_CLAN_JOIN_REQUESTS; i++)
		{
			if (joinRequests[i].bActive && joinRequests[i].i64UID == uid)
				return false;
		}
		// Find empty slot
		for (int i = 0; i < MAX_CLAN_JOIN_REQUESTS; i++)
		{
			if (!joinRequests[i].bActive)
			{
				joinRequests[i].i64UID        = uid;
				strncpy_s(joinRequests[i].szNickname, nick, _TRUNCATE);
				joinRequests[i].i32Level      = level;
				joinRequests[i].i32RankId     = rankId;
				joinRequests[i].dwRequestTime = GetTickCount();
				joinRequests[i].bActive       = true;
				i32JoinRequestCount++;
				return true;
			}
		}
		return false;	// Queue full
	}

	bool RemoveJoinRequest(int64_t uid)
	{
		for (int i = 0; i < MAX_CLAN_JOIN_REQUESTS; i++)
		{
			if (joinRequests[i].bActive && joinRequests[i].i64UID == uid)
			{
				joinRequests[i].Reset();
				i32JoinRequestCount--;
				if (i32JoinRequestCount < 0) i32JoinRequestCount = 0;
				return true;
			}
		}
		return false;
	}

	ClanJoinRequest* FindJoinRequest(int64_t uid)
	{
		for (int i = 0; i < MAX_CLAN_JOIN_REQUESTS; i++)
		{
			if (joinRequests[i].bActive && joinRequests[i].i64UID == uid)
				return &joinRequests[i];
		}
		return nullptr;
	}

	bool HasJoinRequest(int64_t uid) const
	{
		for (int i = 0; i < MAX_CLAN_JOIN_REQUESTS; i++)
		{
			if (joinRequests[i].bActive && joinRequests[i].i64UID == uid)
				return true;
		}
		return false;
	}

	GameClanMember* FindMember(int64_t uid)
	{
		for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
		{
			if (members[i].i64UID == uid)
				return &members[i];
		}
		return nullptr;
	}

	int FindEmptyMemberSlot() const
	{
		for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
		{
			if (!members[i].IsActive())
				return i;
		}
		return -1;
	}

	bool AddMember(int64_t uid, const char* nickname, uint8_t level, int playerLevel, int rankId)
	{
		if (i32MemberCount >= i32MaxMembers)
			return false;

		int slot = FindEmptyMemberSlot();
		if (slot < 0)
			return false;

		members[slot].i64UID = uid;
		strncpy_s(members[slot].szNickname, nickname, _TRUNCATE);
		members[slot].ui8Level = level;
		members[slot].i32PlayerLevel = playerLevel;
		members[slot].i32RankId = rankId;
		members[slot].bOnline = true;
		i32MemberCount++;
		return true;
	}

	bool RemoveMember(int64_t uid)
	{
		for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
		{
			if (members[i].i64UID == uid)
			{
				members[i].Reset();
				i32MemberCount--;
				return true;
			}
		}
		return false;
	}

	bool IsMaster(int64_t uid) const { return i64MasterUID == uid; }

	bool IsStaffOrMaster(int64_t uid) const
	{
		for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
		{
			if (members[i].i64UID == uid)
				return members[i].ui8Level <= CLAN_MEMBER_STAFF;
		}
		return false;
	}
};

#pragma pack(pop)

// Global clan manager (simple array)
class GameClanManager
{
public:
	GameClanManager() : m_i32NextClanId(1)
	{
		for (int i = 0; i < MAX_CLANS; i++)
			m_Clans[i].Reset();
	}

	int CreateClan(const char* name, int64_t masterUID, const char* masterNickname,
		int masterLevel, int masterRankId)
	{
		// Check name uniqueness
		for (int i = 0; i < MAX_CLANS; i++)
		{
			if (m_Clans[i].bActive && _stricmp(m_Clans[i].szName, name) == 0)
				return -1;	// Name taken
		}

		// Find empty slot
		for (int i = 0; i < MAX_CLANS; i++)
		{
			if (!m_Clans[i].bActive)
			{
				GameClanInfo& clan = m_Clans[i];
				clan.Reset();
				clan.i32ClanId = m_i32NextClanId++;
				strncpy_s(clan.szName, name, _TRUNCATE);
				clan.i64MasterUID = masterUID;
				strncpy_s(clan.szMasterNickname, masterNickname, _TRUNCATE);
				clan.bActive = true;
				clan.i32MaxMembers = 10;

				// Add master as first member
				clan.AddMember(masterUID, masterNickname, CLAN_MEMBER_MASTER, masterLevel, masterRankId);
				return clan.i32ClanId;
			}
		}
		return -2;	// No slots
	}

	GameClanInfo* FindClan(int clanId)
	{
		for (int i = 0; i < MAX_CLANS; i++)
		{
			if (m_Clans[i].bActive && m_Clans[i].i32ClanId == clanId)
				return &m_Clans[i];
		}
		return nullptr;
	}

	GameClanInfo* FindClanByName(const char* name)
	{
		for (int i = 0; i < MAX_CLANS; i++)
		{
			if (m_Clans[i].bActive && _stricmp(m_Clans[i].szName, name) == 0)
				return &m_Clans[i];
		}
		return nullptr;
	}

	bool DisbandClan(int clanId)
	{
		GameClanInfo* pClan = FindClan(clanId);
		if (!pClan)
			return false;

		pClan->Reset();
		return true;
	}

	int GetActiveClanCount() const
	{
		int count = 0;
		for (int i = 0; i < MAX_CLANS; i++)
		{
			if (m_Clans[i].bActive)
				count++;
		}
		return count;
	}

	const GameClanInfo* GetClans() const { return m_Clans; }

private:
	GameClanInfo	m_Clans[MAX_CLANS];
	int				m_i32NextClanId;
};

extern GameClanManager* g_pClanManager;

#endif // __CLANDEF_GAME_H__
