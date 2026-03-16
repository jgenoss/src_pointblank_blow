#ifndef __GAMESESSION_H__
#define __GAMESESSION_H__

#pragma once
#include "i3NetworkSession.h"
#include "i3NetworkPacket.h"
#include "RoomDef.h"
#include "CharacterDef.h"
#include "InventoryDef.h"

class Room;

// GAME_TASK state machine (from Server/Source/Game/Game/ServerDef.h)
// GAME_TASK_NONE -> CONNECT -> LEAVE -> LOGIN_ING -> LOGIN -> INFO -> CHANNEL -> LOBBY -> READY_ROOM -> BATTLE
enum GameTask
{
	GAME_TASK_NONE = 0,
	GAME_TASK_CONNECT,
	GAME_TASK_LEAVE,
	GAME_TASK_LOGIN_ING,
	GAME_TASK_LOGIN,
	GAME_TASK_INFO,
	GAME_TASK_CHANNEL,
	GAME_TASK_LOBBY,
	GAME_TASK_READY_ROOM,
	GAME_TASK_BATTLE,
	GAME_TASK_COUNT,
};

// Timeout definitions (from ServerDef.h)
#define SESSION_CHECK_TIMEOUT_NORMAL		120		// 2 minutes
#define SESSION_CHECK_TIMEOUT_CHANNEL		3600	// 1 hour
#define SESSION_CHECK_TIMEOUT_LOGIN			600		// 10 minutes
#define SESSION_CHECK_TIMEOUT_NOT_LOGIN		30		// 30 seconds

// Simplified port of CUserSession from Server/Source/Game/Game/UserSession.h
// State machine, packet handlers for all GAME_TASK states
class GameSession : public i3NetworkSession
{
	I3_CLASS_DEFINE(GameSession);

public:
	GameSession();
	virtual ~GameSession();

	// i3NetworkSession overrides
	virtual BOOL	Create() override;
	virtual void	Destroy() override;
	virtual BOOL	OnConnect(SOCKET Socket, struct sockaddr_in* pAddr) override;
	virtual BOOL	OnConnectInit() override;
	virtual BOOL	OnDisconnect(BOOL bForceMainThread = FALSE) override;
	virtual INT32	PacketParsing(char* pPacket, INT32 iSize) override;

	// State
	GameTask		GetTask() const				{ return m_eMainTask; }
	void			SetTask(GameTask eTask)		{ m_eMainTask = eTask; }

	// Identity
	int64_t			GetUID() const				{ return m_i64UID; }
	void			SetUID(int64_t uid)			{ m_i64UID = uid; }
	const char*		GetUsername() const			{ return m_szUsername; }
	const char*		GetNickname() const			{ return m_szNickname; }
	uint32_t		GetXorKey() const			{ return m_ui32EncriptKey; }
	uint32_t		GetAuthToken() const		{ return m_ui32AuthToken; }
	void			SetAuthToken(uint32_t token){ m_ui32AuthToken = token; }

	// Channel & Room
	int				GetChannelNum() const		{ return m_i32ChannelNum; }
	void			SetChannelNum(int ch)		{ m_i32ChannelNum = ch; }
	int				GetRoomIdx() const			{ return m_i32RoomIdx; }
	void			SetRoomIdx(int idx)			{ m_i32RoomIdx = idx; }
	int				GetSlotIdx() const			{ return m_i32SlotIdx; }
	void			SetSlotIdx(int idx)			{ m_i32SlotIdx = idx; }
	Room*			GetRoom() const				{ return m_pRoom; }
	void			SetRoom(Room* pRoom)		{ m_pRoom = pRoom; }

	// Player data (loaded from DataServer)
	int				GetLevel() const			{ return m_i32Level; }
	int64_t			GetExp() const				{ return m_i64Exp; }
	int				GetCash() const				{ return m_i32Cash; }
	int				GetGP() const				{ return m_i32GP; }
	int				GetRankId() const			{ return m_i32RankId; }
	int				GetClanId() const			{ return m_i32ClanId; }

	// Stats
	int				GetKills() const			{ return m_i32Kills; }
	int				GetDeaths() const			{ return m_i32Deaths; }

	// Equipment (7E)
	uint8_t			GetActiveCharaSlot() const				{ return m_ui8ActiveCharaSlot; }
	const GameCharaEquip& GetActiveEquipment() const		{ return m_CharaSlots[m_ui8ActiveCharaSlot].equip; }
	const GameCharaSlot& GetCharaSlot(int idx) const		{ return m_CharaSlots[idx]; }

	// Inventory (7F)
	int					GetInventoryCount() const			{ return m_i32InventoryCount; }
	const GameInventoryItem* GetInventory() const			{ return m_Inventory; }
	GameInventoryItem*	FindInventoryItem(uint32_t itemId);
	GameInventoryItem*	FindInventoryItemByDBIdx(uint32_t dbIdx);
	bool				HasInventoryItem(uint32_t itemId) const;
	int					AddInventoryItem(const GameInventoryItem& item);
	bool				RemoveInventoryItem(uint32_t dbIdx);

	// Load player data from DataServer response
	void			OnPlayerDataLoaded(const char* pPayload, int i32PayloadSize);

	// Timeout
	bool			IsTimedOut() const;
	DWORD			GetLastPacketTime() const	{ return m_dwLastPacketTime; }

private:
	// Packet handlers - Connect phase
	void			OnBaseConnectReq(char* pData, INT32 i32Size);
	void			OnBaseHeartBitReq(char* pData, INT32 i32Size);

	// Packet handlers - Login phase (auth token validation)
	void			OnLoginReq(char* pData, INT32 i32Size);

	// Packet handlers - Info phase
	void			OnGetUserInfoReq(char* pData, INT32 i32Size);

	// Packet handlers - Channel (GameSessionChannel.cpp)
	void			OnChannelListReq(char* pData, INT32 i32Size);
	void			OnChannelEnterReq(char* pData, INT32 i32Size);
	void			OnChannelLeaveReq(char* pData, INT32 i32Size);

	// Packet handlers - Lobby (GameSessionChannel.cpp)
	void			OnLobbyEnterReq(char* pData, INT32 i32Size);
	void			OnLobbyLeaveReq(char* pData, INT32 i32Size);
	void			OnGetRoomListReq(char* pData, INT32 i32Size);
	void			OnLobbyChatReq(char* pData, INT32 i32Size);
	void			OnQuickJoinRoomReq(char* pData, INT32 i32Size);

	// Packet handlers - Room (GameSessionRoom.cpp)
	void			OnRoomCreateReq(char* pData, INT32 i32Size);
	void			OnRoomJoinReq(char* pData, INT32 i32Size);
	void			OnRoomLeaveReq(char* pData, INT32 i32Size);
	void			OnRoomGetSlotInfoReq(char* pData, INT32 i32Size);
	void			OnRoomGetPlayerInfoReq(char* pData, INT32 i32Size);
	void			OnRoomChangeRoomInfoReq(char* pData, INT32 i32Size);
	void			OnRoomChangePasswdReq(char* pData, INT32 i32Size);
	void			OnRoomChangeSlotReq(char* pData, INT32 i32Size);
	void			OnRoomTeamChangeReq(char* pData, INT32 i32Size);
	void			OnRoomRequestMainReq(char* pData, INT32 i32Size);
	void			OnRoomRequestMainChangeReq(char* pData, INT32 i32Size);
	void			OnRoomChangeOptionInfoReq(char* pData, INT32 i32Size);
	void			OnRoomChatReq(char* pData, INT32 i32Size);
	void			OnRoomGetUserEquipmentReq(char* pData, INT32 i32Size);

	// Packet handlers - Battle flow (GameSessionBattle.cpp)
	void			OnBattleReadyBattleReq(char* pData, INT32 i32Size);
	void			OnBattlePreStartBattleReq(char* pData, INT32 i32Size);
	void			OnBattleStartBattleReq(char* pData, INT32 i32Size);
	void			OnBattleGiveUpBattleReq(char* pData, INT32 i32Size);
	void			OnBattleDeathReq(char* pData, INT32 i32Size);
	void			OnBattleRespawnReq(char* pData, INT32 i32Size);
	void			OnBattleMissionRoundPreStartReq(char* pData, INT32 i32Size);
	void			OnBattleMissionRoundStartReq(char* pData, INT32 i32Size);
	void			OnBattleMissionRoundEndReq(char* pData, INT32 i32Size);

	// Packet handlers - Equipment (GameSessionEquipment.cpp)
	void			OnGetEquipmentInfoReq(char* pData, INT32 i32Size);
	void			OnEquipmentReq(char* pData, INT32 i32Size);
	void			OnGetCharaInfoReq(char* pData, INT32 i32Size);
	void			OnCharaCreateReq(char* pData, INT32 i32Size);
	void			OnCharaShiftPosReq(char* pData, INT32 i32Size);

	// Packet handlers - Inventory (GameSessionInventory.cpp)
	void			OnGetInvenInfoReq(char* pData, INT32 i32Size);
	void			OnInventoryEnterReq(char* pData, INT32 i32Size);
	void			OnInventoryLeaveReq(char* pData, INT32 i32Size);

	// Helpers
	void			SendConnectAck();
	void			SendHeartBitAck();
	void			SendLoginAck(int i32Result);
	void			SendSimpleAck(uint16_t protocolAck, int32_t result);
	void			ResetSessionData();

	// Battle helpers
	void			SendSlotInfoToAll();
	void			SendBattleEndToAll(int i32WinnerTeam);

private:
	// State machine
	GameTask		m_eMainTask;

	// Identity
	int64_t			m_i64UID;
	char			m_szUsername[64];
	char			m_szNickname[64];
	uint32_t		m_ui32EncriptKey;		// XOR encryption key
	uint32_t		m_ui32AuthToken;		// Token from ConnectServer

	// Location
	int				m_i32ChannelNum;		// Current channel (-1 = none)
	int				m_i32RoomIdx;			// Current room index (-1 = none)
	int				m_i32SlotIdx;			// Slot in room (-1 = none)
	Room*			m_pRoom;

	// Player data
	int				m_i32Level;
	int64_t			m_i64Exp;
	int				m_i32Cash;
	int				m_i32GP;
	int				m_i32RankId;
	int				m_i32ClanId;

	// Stats
	int				m_i32Kills;
	int				m_i32Deaths;
	int				m_i32Headshots;
	int				m_i32Wins;
	int				m_i32Losses;

	// Equipment (7E) - Character slots with weapon/parts loadouts
	uint8_t			m_ui8ActiveCharaSlot;					// Current active character slot (0-4)
	GameCharaSlot	m_CharaSlots[MAX_CHARA_SLOT];			// Up to 5 character loadouts

	// Inventory (7F) - Full inventory
	GameInventoryItem	m_Inventory[MAX_INVEN_COUNT];
	int				m_i32InventoryCount;

	// Timing
	DWORD			m_dwConnectTime;
	DWORD			m_dwLastPacketTime;
};

#endif // __GAMESESSION_H__
