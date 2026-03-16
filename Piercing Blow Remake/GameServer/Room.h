#ifndef __ROOM_H__
#define __ROOM_H__

#pragma once
#include "i3ElementBase.h"
#include "i3NetworkPacket.h"
#include "RoomDef.h"

class GameSession;

// Expanded port of CRoom from Server/Source/Game/Game/Room.h
// Manages a single game room with slots, teams, game mode, and battle state
class Room : public i3ElementBase
{
	I3_CLASS_DEFINE(Room);

public:
	Room();
	virtual ~Room();

	// Lifecycle
	bool		OnCreate(GameSession* pOwner, GameRoomCreateInfo* pInfo, int i32ChannelNum);
	void		OnDestroy();

	// Player management
	int			OnJoin(GameSession* pSession, int i32Team);		// Returns slot index, -1 on error
	void		OnLeave(GameSession* pSession);
	void		OnReady(GameSession* pSession, bool bReady);

	// Battle control
	bool		OnStartBattle();								// Returns false if not all ready
	void		OnEndBattle();

	// Timer-driven state machine update (called from RoomManager tick)
	void		OnUpdateRoom(DWORD dwNow);

	// Room operations
	bool		OnChangeTeam(GameSession* pSession, int i32NewTeam);
	bool		OnChangeRoomInfo(GameSession* pSession, GameRoomCreateInfo* pNewInfo);
	bool		OnChangePassword(GameSession* pSession, const char* pszNewPassword);
	bool		OnKickPlayer(GameSession* pOwner, int i32TargetSlot);
	bool		OnTransferOwner(GameSession* pOwner, int i32TargetSlot);

	// Score & round management
	void		OnRoundEnd(int i32WinnerTeam);
	void		OnAddKill(int i32Team);
	bool		CheckMatchEnd() const;

	// Bomb mode (Phase 2A)
	void		OnBombInstall(int i32InstallerSlot, uint8_t ui8BombArea);
	void		OnBombUninstall(int i32DefuserSlot);
	void		OnBombExplode();
	bool		IsBombInstalled() const		{ return m_bBombInstalled; }
	int			GetBombInstallerSlot() const	{ return m_i32BombInstallerSlot; }
	uint8_t		GetBombArea() const			{ return m_ui8BombArea; }
	bool		IsBombMode() const			{ return m_ui8GameMode == STAGE_MODE_BOMB; }
	void		SwapTeams();				// ATK/DEF swap between rounds

	// Destroy mode (Phase 2C) - Generator/objective HP tracking
	void		OnGeneratorDamage(int i32GeneratorIdx, int i32Damage, int i32AttackerSlot);
	void		OnGeneratorDestroy(int i32GeneratorIdx);
	int			GetGeneratorHP(int i32Idx) const;
	int			GetGeneratorCount() const		{ return m_i32GeneratorCount; }
	bool		IsDestroyMode() const			{ return m_ui8GameMode == STAGE_MODE_DESTROY; }

	// Escape/VIP mode (Phase 2E) - VIP selection and touchdown
	void		SelectVIP();					// Randomly select VIP from ATK team
	void		OnTouchdown(int i32Slot);		// VIP reached escape zone
	int			GetVIPSlot() const				{ return m_i32VIPSlot; }
	bool		IsEscapeMode() const			{ return m_ui8GameMode == STAGE_MODE_ESCAPE; }

	// Death & Kill processing
	void		OnPlayerDeath(int i32DeadSlot, int i32KillerSlot, uint32_t ui32WeaponId,
						 uint8_t ui8HitPart, float fX, float fY, float fZ, int i32AssistSlot);
	void		OnPlayerRespawn(int i32Slot);
	bool		IsPlayerAlive(int i32Slot) const;
	int			GetAliveCount(int i32Team) const;

	// Multi-kill tracking
	int			GetConsecutiveKills(int i32Slot) const;

	// Respawn timing
	int			GetRespawnTime() const;

	// State queries
	bool		IsCreated() const			{ return m_bIsCreate; }
	int			GetChannelNum() const		{ return m_i32ChannelNum; }
	int			GetPlayerCount() const		{ return m_i32PlayerCount; }
	int			GetMaxPlayers() const		{ return m_i32MaxPlayers; }
	uint8_t		GetRoomState() const		{ return m_ui8RoomState; }
	void		SetRoomState(uint8_t s)		{ m_ui8RoomState = s; }
	const char*	GetTitle() const			{ return m_szTitle; }
	uint32_t	GetStageId() const			{ return m_ui32StageId; }
	int			GetRoomIdx() const			{ return m_i32RoomIdx; }
	void		SetRoomIdx(int idx)			{ m_i32RoomIdx = idx; }

	// Game mode
	uint8_t		GetGameMode() const			{ return m_ui8GameMode; }
	uint8_t		GetMapIndex() const			{ return m_ui8MapIndex; }
	uint8_t		GetRoundType() const		{ return m_ui8RoundType; }
	uint8_t		GetSubType() const			{ return m_ui8SubType; }
	uint8_t		GetWeaponFlag() const		{ return m_ui8WeaponFlag; }
	uint8_t		GetInfoFlag() const			{ return m_ui8InfoFlag; }
	bool		HasPassword() const			{ return m_szPassword[0] != '\0'; }

	// Escape/VIP mode
	bool		IsVIPSlot(int slot) const	{ return m_i32VIPSlot == slot && m_ui8GameMode == STAGE_MODE_ESCAPE; }
	int			GetVIPSlot() const			{ return m_i32VIPSlot; }
	int			GetEffectiveHP(int slot) const { return IsVIPSlot(slot) ? ESCAPE_VIP_EXTRA_HP : 100; }
	bool		CheckPassword(const char* pw) const;
	bool		IsClanMatch() const			{ return m_bIsClanMatch; }
	void		SetClanMatch(bool b)		{ m_bIsClanMatch = b; }

	// Score
	const GameRoomScore& GetScore() const	{ return m_Score; }

	// Battle info (from BattleServer)
	void		SetBattleInfo(int i32BattleRoomIdx, const char* pszUdpIP, uint16_t ui16UdpPort);
	void		ClearBattleInfo();
	int			GetBattleRoomIdx() const	{ return m_i32BattleRoomIdx; }
	const char*	GetBattleUdpIP() const		{ return m_szBattleUdpIP; }
	uint16_t	GetBattleUdpPort() const	{ return m_ui16BattleUdpPort; }
	uint32_t	GetBattleUdpIPAddr() const	{ return m_ui32BattleUdpIPAddr; }

	// Slot queries
	bool		IsSlotEmpty(int i32Slot) const;
	GameSession* GetSlotSession(int i32Slot) const;
	int			GetOwnerSlot() const		{ return m_i32OwnerSlot; }
	const GameSlotInfo& GetSlotInfo(int i32Slot) const { return m_Slots[i32Slot]; }
	GameSlotInfo& GetSlotInfoMutable(int i32Slot) { return m_Slots[i32Slot]; }

	// Per-slot battle state
	bool		AllSlotsInState(uint8_t ui8State) const;
	bool		AllSlotsMinState(uint8_t ui8MinState) const;
	int			CountSlotsInState(uint8_t ui8State) const;
	void		SetSlotState(int i32Slot, uint8_t ui8State);

	// Battle stats per-slot
	struct SlotBattleStats
	{
		int		i32Kills;
		int		i32Deaths;
		int		i32Headshots;
		int		i32Assists;
		int		i32ConsecutiveKills;	// Current streak (resets on death)
		int		i32MaxConsecutiveKills;	// Max streak in match
		uint16_t ui16AccExp;
		uint16_t ui16AccPoint;
		bool	bAlive;
		DWORD	dwDeathTime;			// Tick when died (for respawn timer)
		bool	bRespawnPending;		// Waiting for respawn timer

		void Reset()
		{
			i32Kills = i32Deaths = i32Headshots = i32Assists = 0;
			i32ConsecutiveKills = i32MaxConsecutiveKills = 0;
			ui16AccExp = ui16AccPoint = 0;
			bAlive = true;
			dwDeathTime = 0;
			bRespawnPending = false;
		}
	};

	const SlotBattleStats& GetSlotBattleStats(int i32Slot) const { return m_SlotStats[i32Slot]; }
	SlotBattleStats& GetSlotBattleStatsMutable(int i32Slot) { return m_SlotStats[i32Slot]; }

	// Broadcast
	void		SendToAll(i3NetworkPacket* pPacket);
	void		SendToTeam(int i32Team, i3NetworkPacket* pPacket);
	void		SendToAllExcept(GameSession* pExcept, i3NetworkPacket* pPacket);

	// Room info for lobby list (ROOM_INFO_BASIC format)
	void		FillRoomInfoBasic(char* pBuffer, int* pSize) const;

	// Battle result calculation
	void		CalculateBattleRewards(int i32WinnerTeam);
	void		SendBattleResultToAll(int i32WinnerTeam);

private:
	// State machine update handlers
	void		OnUpdateRoom_CountdownR(DWORD dwNow);
	void		OnUpdateRoom_Loading(DWORD dwNow);
	void		OnUpdateRoom_Rendezvous(DWORD dwNow);
	void		OnUpdateRoom_PreBattle(DWORD dwNow);
	void		OnUpdateRoom_CountdownB(DWORD dwNow);
	void		OnUpdateRoom_Battle(DWORD dwNow);
	void		OnUpdateRoom_BattleResult(DWORD dwNow);
	void		OnUpdateRoom_BattleEnd(DWORD dwNow);

	// Helpers
	int			FindEmptySlot(int i32Team) const;
	void		UpdateOwner();
	void		BroadcastRoomStateChange();

public:
	// Manager access
	bool				m_bIsCreate;
	int					m_i32ChannelNum;

private:
	int					m_i32RoomIdx;
	char				m_szTitle[NET_ROOM_NAME_SIZE];
	char				m_szPassword[NET_ROOM_PW_SIZE];
	uint32_t			m_ui32StageId;
	uint8_t				m_ui8RoomState;		// ROOM_STATE enum
	int					m_i32MaxPlayers;
	int					m_i32PlayerCount;
	int					m_i32OwnerSlot;

	// Game mode settings
	uint8_t				m_ui8GameMode;		// StageMode enum
	uint8_t				m_ui8MapIndex;		// STAGE_UID
	uint8_t				m_ui8RoundType;		// BattleRoundType enum
	uint8_t				m_ui8SubType;		// Kill/time settings
	uint8_t				m_ui8WeaponFlag;	// Weapon restrictions
	uint8_t				m_ui8InfoFlag;		// Room flags
	uint32_t			m_ui32OptionFlag;	// Extended options
	bool				m_bIsClanMatch;

	// Score tracking
	GameRoomScore		m_Score;

	// Battle timing
	DWORD				m_dwBattleStartTime;
	DWORD				m_dwStateStartTime;		// When current state was entered
	DWORD				m_dwRoundStartTime;		// When current round started

	// Per-slot battle stats
	SlotBattleStats		m_SlotStats[SLOT_MAX_COUNT];

	// Respawn type (from room SubType)
	uint8_t				m_ui8RespawnType;

	// Bomb mode state (Phase 2A)
	bool				m_bBombInstalled;		// C4 currently planted
	int					m_i32BombInstallerSlot;	// Slot that planted the bomb
	uint8_t				m_ui8BombArea;			// 0=A, 1=B
	DWORD				m_dwBombInstallTime;	// When bomb was planted
	uint32_t			m_ui32BombExplosionTime;// Explosion delay in ms (default 40000 = 40s)
	bool				m_bAtkDefSwap;			// true = teams have been swapped (ATK was RED at start)

	// Destroy mode state (Phase 2C)
	int					m_i32GeneratorHP[GENERATOR_COUNT_MAX];	// HP per generator
	int					m_i32GeneratorMaxHP;	// Max HP (from config)
	int					m_i32GeneratorCount;	// Number of active generators (1-2)
	bool				m_bGeneratorDestroyed[GENERATOR_COUNT_MAX];	// Destroyed flags

	// Escape/VIP mode state (Phase 2E)
	int					m_i32VIPSlot;			// Slot index of the VIP (-1 = none)

	// Loading timeout tracking
	DWORD				m_dwLoadingTimeout;		// Max loading time (60s default)

	// BattleServer info (stored from IS_BATTLE_CREATE_ACK)
	int					m_i32BattleRoomIdx;
	char				m_szBattleUdpIP[MAX_SERVER_IP_LENGTH];
	uint16_t			m_ui16BattleUdpPort;
	uint32_t			m_ui32BattleUdpIPAddr;	// Packed IPv4 for client

	// Kick vote state (Phase 3A)
	bool				m_bKickVoteActive;
	int					m_i32KickVoteTarget;	// Slot being voted on (-1 = none)
	int					m_i32KickVoteSuggestor;	// Slot that started the vote
	DWORD				m_dwKickVoteStartTime;	// When vote started
	DWORD				m_dwLastKickVoteTime;	// Cooldown tracking
	uint8_t				m_KickVotes[SLOT_MAX_COUNT];	// 0=not voted, 1=agree, 2=disagree

	// Map rotation
	#define MAX_MAP_ROTATION	16
	uint8_t				m_MapRotation[MAX_MAP_ROTATION];
	int					m_i32MapRotationCount;
	int					m_i32MapRotationIdx;
	bool				m_bMapRotationEnabled;

public:
	// Kick vote helpers
	bool		StartKickVote(int suggestSlot, int targetSlot);
	bool		CastKickVote(int voterSlot, int targetSlot, uint8_t vote);
	void		ResolveKickVote();
	bool		IsKickVoteActive() const { return m_bKickVoteActive; }

	// Map rotation
	void		SetMapRotation(const uint8_t* maps, int count);
	void		AdvanceMapRotation();
	bool		IsMapRotationEnabled() const { return m_bMapRotationEnabled; }

	// GM debug/cheat helpers
	bool		ToggleSoloPlay()				{ m_bSoloPlay = !m_bSoloPlay; return m_bSoloPlay; }
	bool		IsSoloPlay() const				{ return m_bSoloPlay; }
	void		ReduceBattleTime(int seconds)	{ m_dwBattleStartTime -= (DWORD)(seconds * 1000); }

	// Battle log
	uint64_t	GetBattleUniqueNumber() const { return m_ui64BattleUniqueNum; }
	void		LogBattleResult();

private:
	// Slots
	GameSlotInfo		m_Slots[SLOT_MAX_COUNT];
	GameSession*		m_pSlotSession[SLOT_MAX_COUNT];

	// GM debug flags
	bool				m_bSoloPlay;			// Allow 1-player start (GM cheat)

	// Battle unique number for logging
	uint64_t			m_ui64BattleUniqueNum;
	static volatile long s_BattleCounter;		// Global counter for unique IDs
};

#endif // __ROOM_H__
