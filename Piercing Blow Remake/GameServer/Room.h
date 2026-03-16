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
	bool		CheckPassword(const char* pw) const;
	bool		IsClanMatch() const			{ return m_bIsClanMatch; }

	// Score
	const GameRoomScore& GetScore() const	{ return m_Score; }

	// Slot queries
	bool		IsSlotEmpty(int i32Slot) const;
	GameSession* GetSlotSession(int i32Slot) const;
	int			GetOwnerSlot() const		{ return m_i32OwnerSlot; }
	const GameSlotInfo& GetSlotInfo(int i32Slot) const { return m_Slots[i32Slot]; }

	// Broadcast
	void		SendToAll(i3NetworkPacket* pPacket);
	void		SendToTeam(int i32Team, i3NetworkPacket* pPacket);
	void		SendToAllExcept(GameSession* pExcept, i3NetworkPacket* pPacket);

	// Room info for lobby list (ROOM_INFO_BASIC format)
	void		FillRoomInfoBasic(char* pBuffer, int* pSize) const;

private:
	int			FindEmptySlot(int i32Team) const;
	void		UpdateOwner();

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

	// Slots
	GameSlotInfo		m_Slots[SLOT_MAX_COUNT];
	GameSession*		m_pSlotSession[SLOT_MAX_COUNT];
};

#endif // __ROOM_H__
