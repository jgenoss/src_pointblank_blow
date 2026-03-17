#ifndef __MODULECAST_H__
#define __MODULECAST_H__

#pragma once

// ============================================================================
// ModuleCast - TCP communication to GameServer
// Port of original CModuleCast from Dedication/Dedi/ModuleCast.h
// Sends battle events: kills, round start/end, hack reports, bomb events,
// equipment info, touchdown, kick notifications
// ============================================================================

class BattleRoom;

// Battle room info for protocol messages
struct BattleRoomInfo
{
	uint32_t	ui32RoomIdx;
	uint32_t	ui32MapIdx;
	uint8_t		ui8GameMode;
	uint8_t		ui8MaxPlayers;
	uint8_t		ui8Pad[2];
};

// Kill info for reporting
struct DeathInfoChara
{
	uint32_t	ui32KillerSlot;
	uint32_t	ui32VictimSlot;
	uint32_t	ui32WeaponID;
	uint8_t		ui8HitPart;
	uint8_t		ui8Headshot;
	uint8_t		ui8Pad[2];
};

// Hack info for reporting
struct BattleHackInfo
{
	uint32_t	ui32SlotIdx;
	int64_t		i64UID;
	uint8_t		ui8HackType;
	uint8_t		ui8Severity;
	uint8_t		ui8Pad[2];
	char		szDescription[128];
};

class ModuleCast
{
public:
	ModuleCast();
	~ModuleCast();

	// Lifecycle
	bool OnCreate(uint32_t ui32IP, uint16_t ui16Port);
	void OnDestroy();

	// Connection management
	bool IsConnected() const { return m_bConnected; }
	void Update();	// Call periodically to maintain connection

	// ============================================================
	// Battle event notifications (to GameServer)
	// ============================================================

	// Room creation result
	bool Send_RoomCreateAck(int32_t i32WorkIdx, uint32_t ui32RoomIdx, int32_t i32Result);

	// Kill notification
	bool Send_BattleKillAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
							 const DeathInfoChara* pCharacters, uint8_t ui8Count,
							 uint8_t ui8RoundNum);

	// Round events
	bool Send_BattleRoundStart(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo);
	bool Send_BattleRoundEnd(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
							  uint8_t ui8RoundEndType, uint8_t ui8WinTeam);

	// Mission events
	bool Send_BombInstallAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
							  uint32_t ui32SlotIdx);
	bool Send_BombUninstallAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
								uint32_t ui32SlotIdx);
	bool Send_DestructionHPAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
								uint32_t ui32ObjectIdx, int32_t i32HP);
	bool Send_DefenceAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
						  uint32_t ui32ObjectIdx);
	bool Send_TouchDown(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
						 uint32_t ui32SlotIdx);

	// Hack report
	bool Send_HackUserAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
						   const BattleHackInfo* pUsers, uint8_t ui8Count);

	// Kick notification
	bool Send_KickUserAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
						   uint32_t ui32SlotIdx, int64_t i64UID);

	// Equipment sync
	bool Send_SlotEquipmentAck(int32_t i32WorkIdx, uint32_t ui32RoomIdx,
								uint32_t ui32SlotIdx, const uint32_t* pEquipIDs,
								uint8_t ui8Count);

	// Server init notification
	bool ServerInit(int32_t i32WorkIdx, int32_t i32Result);

private:
	bool		m_bConnected;
	uint32_t	m_ui32IP;
	uint16_t	m_ui16Port;
	uint32_t	m_ui32LastConnectAttempt;
	uint32_t	m_ui32ReconnectInterval;
};

#endif // __MODULECAST_H__
