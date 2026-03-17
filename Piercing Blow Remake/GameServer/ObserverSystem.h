#ifndef __OBSERVER_SYSTEM_H__
#define __OBSERVER_SYSTEM_H__

#pragma once
#include "RoomDef.h"

// Observer/Spectator constants (matches SLOT_MAX_OBSERVER_COUNT from CommonDef.h)
#define MAX_OBSERVERS_PER_ROOM		4
#define OBSERVER_UPDATE_INTERVAL_MS	100		// 10 Hz position update rate
#define OBSERVER_ANTIGHOST_DELAY_MS	5000	// 5-second anti-ghosting delay (configurable)
#define OBSERVER_POSITION_BUFFER_SIZE 128	// Circular buffer size for delayed positions

class GameSession;
class Room;
struct i3NetworkPacket;

// ============================================================================
// Observer view modes
// ============================================================================
enum ObserverViewMode
{
	OBSERVER_VIEW_FREE = 0,			// Free camera
	OBSERVER_VIEW_FOLLOW,			// Follow specific player (3rd person)
	OBSERVER_VIEW_FIRST_PERSON,		// First-person view of player
	OBSERVER_VIEW_OVERHEAD,			// Top-down / overhead view
	OBSERVER_VIEW_MAX,
};

// ============================================================================
// Per-observer state
// ============================================================================
struct ObserverState
{
	int				i32SessionIdx;		// Session index of observer (-1 = empty)
	int				i32SlotIdx;			// Slot index in room
	int				i32FollowSlot;		// Which player they're watching (-1 = free cam)
	ObserverViewMode eViewMode;
	bool			bShowScoreboard;
	bool			bShowMinimap;
	bool			bActive;			// Currently active observer
	DWORD			dwLastUpdateTime;	// Last time we sent them an update
	DWORD			dwJoinTime;			// When observer joined (for anti-ghost warmup)

	void Reset()
	{
		i32SessionIdx = -1;
		i32SlotIdx = -1;
		i32FollowSlot = -1;
		eViewMode = OBSERVER_VIEW_FREE;
		bShowScoreboard = false;
		bShowMinimap = true;
		bActive = false;
		dwLastUpdateTime = 0;
		dwJoinTime = 0;
	}
};

// ============================================================================
// Buffered position entry for anti-ghosting delay
// ============================================================================
struct ObserverPositionEntry
{
	DWORD		dwTimestamp;
	int			i32Slot;
	float		fX, fY, fZ;
	float		fYaw;					// Facing direction
	uint8_t		ui8Team;
	bool		bAlive;

	ObserverPositionEntry()
		: dwTimestamp(0), i32Slot(-1)
		, fX(0), fY(0), fZ(0), fYaw(0)
		, ui8Team(0), bAlive(true)
	{
	}
};

// ============================================================================
// Kill feed entry for delayed broadcast to observers
// ============================================================================
struct ObserverKillFeedEntry
{
	DWORD		dwTimestamp;
	int			i32KillerSlot;
	int			i32DeadSlot;
	uint32_t	ui32WeaponId;
	uint8_t		ui8HitPart;
	uint8_t		ui8MultiKill;		// MultiKillType enum

	ObserverKillFeedEntry()
		: dwTimestamp(0), i32KillerSlot(-1), i32DeadSlot(-1)
		, ui32WeaponId(0), ui8HitPart(0), ui8MultiKill(0)
	{
	}
};

// ============================================================================
// Round event entry for delayed broadcast to observers
// ============================================================================
struct ObserverRoundEventEntry
{
	DWORD		dwTimestamp;
	uint8_t		ui8EventType;		// 0=round start, 1=round end, 2=bomb plant,
									// 3=bomb defuse, 4=bomb explode, 5=VIP touchdown
	int			i32WinnerTeam;		// For round end (-1 = no winner)
	int			i32ActorSlot;		// Slot that triggered the event (-1 = none)
	uint8_t		ui8Extra;			// Extra data (bomb area, etc.)

	ObserverRoundEventEntry()
		: dwTimestamp(0), ui8EventType(0), i32WinnerTeam(-1)
		, i32ActorSlot(-1), ui8Extra(0)
	{
	}
};

// ============================================================================
// ObserverSystem - Per-room observer/spectator management
// ============================================================================
class ObserverSystem
{
public:
	ObserverSystem();
	~ObserverSystem();

	// Initialization (called when room is created)
	void		Init(Room* pRoom);
	void		Reset();

	// Observer lifecycle
	bool		OnObserverJoin(GameSession* pSession, int i32SlotIdx);
	void		OnObserverLeave(GameSession* pSession);

	// View mode controls (called from packet handlers)
	bool		OnViewModeChange(int i32SlotIdx, ObserverViewMode eMode);
	bool		OnFollowPlayer(int i32SlotIdx, int i32TargetSlot);
	bool		OnToggleScoreboard(int i32SlotIdx, bool bShow);

	// Game event feeds (called from Room when events happen)
	void		EnqueuePositionUpdate(int i32Slot, float fX, float fY, float fZ,
									  float fYaw, uint8_t ui8Team, bool bAlive);
	void		EnqueueKillFeed(int i32KillerSlot, int i32DeadSlot,
								uint32_t ui32WeaponId, uint8_t ui8HitPart,
								uint8_t ui8MultiKill);
	void		EnqueueScoreUpdate(int i32RedScore, int i32BlueScore,
								   int i32Round);
	void		EnqueueRoundEvent(uint8_t ui8EventType, int i32WinnerTeam,
								  int i32ActorSlot, uint8_t ui8Extra);

	// Periodic update (called from Room::OnUpdateRoom)
	void		Update(DWORD dwNow);

	// Queries
	int			GetObserverCount() const	{ return m_i32ObserverCount; }
	bool		HasObservers() const		{ return m_i32ObserverCount > 0; }
	bool		IsObserver(int i32SlotIdx) const;
	const ObserverState* GetObserverBySlot(int i32SlotIdx) const;

	// Anti-ghosting delay configuration
	void		SetAntiGhostDelay(DWORD dwDelayMs)	{ m_dwAntiGhostDelayMs = dwDelayMs; }
	DWORD		GetAntiGhostDelay() const			{ return m_dwAntiGhostDelayMs; }

private:
	// Internal send helpers
	void		SendPositionUpdate(DWORD dwNow);
	void		SendKillFeedUpdate(DWORD dwNow);
	void		SendScoreUpdate();
	void		SendRoundEvent(DWORD dwNow);

	// Send a packet to all active observers
	void		SendToObservers(i3NetworkPacket* pPacket);

	// Send a packet to a single observer by slot
	void		SendToObserver(int i32SlotIdx, i3NetworkPacket* pPacket);

	// Find observer state by slot index
	ObserverState*	FindObserver(int i32SlotIdx);
	int				FindEmptyObserverSlot() const;

private:
	Room*				m_pRoom;
	int					m_i32ObserverCount;
	ObserverState		m_Observers[MAX_OBSERVERS_PER_ROOM];

	// Anti-ghosting delay
	DWORD				m_dwAntiGhostDelayMs;

	// Circular buffers for delayed position data
	ObserverPositionEntry	m_PositionBuffer[OBSERVER_POSITION_BUFFER_SIZE];
	int						m_i32PositionWriteIdx;

	// Circular buffers for delayed kill feed
	ObserverKillFeedEntry	m_KillFeedBuffer[64];
	int						m_i32KillFeedWriteIdx;
	int						m_i32KillFeedReadIdx;

	// Pending score update (latest values, sent periodically)
	int					m_i32PendingRedScore;
	int					m_i32PendingBlueScore;
	int					m_i32PendingRound;
	bool				m_bScoreDirty;

	// Circular buffer for delayed round events
	ObserverRoundEventEntry	m_RoundEventBuffer[16];
	int						m_i32RoundEventWriteIdx;
	int						m_i32RoundEventReadIdx;

	// Timing
	DWORD				m_dwLastPositionBroadcast;
};

// ============================================================================
// Cast Protocol Structures (for external streaming/spectator tools)
// ============================================================================
// These structures define the wire format for battle events that can be
// relayed to external cast clients (web viewers, streaming overlays).
// The ObserverSystem generates these events internally; when an external
// cast relay is enabled, they are serialized using these structures.
// ============================================================================

#pragma pack(push, 1)

struct CastKillEvent
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8RoundNum;
	uint32_t	ui32KillerSlot;
	uint32_t	ui32VictimSlot;
	int64_t		i64KillerUID;
	int64_t		i64VictimUID;
	char		szKillerNick[64];
	char		szVictimNick[64];
	uint32_t	ui32WeaponID;
	uint8_t		ui8Headshot;
};

struct CastRoundEvent
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8RoundNum;
	uint8_t		ui8GameMode;
	uint8_t		ui8EventType;		// 0 = start, 1 = end
	uint8_t		ui8WinTeam;
	int			i32RedScore;
	int			i32BlueScore;
};

struct CastScoreEvent
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	int			i32RedScore;
	int			i32BlueScore;
	uint8_t		ui8NowRound;
};

struct CastSlotPosition
{
	uint8_t		ui8Slot;
	float		fX;
	float		fY;
	float		fZ;
	float		fYaw;
	uint8_t		ui8Team;
	uint8_t		ui8Alive;
};

#pragma pack(pop)

#endif // __OBSERVER_SYSTEM_H__
