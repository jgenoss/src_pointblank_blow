#ifndef __BATTLESESSION_H__
#define __BATTLESESSION_H__

#pragma once
#include "i3NetworkSession.h"
#include "i3NetworkPacket.h"
#include "InterServerProtocol.h"
#include "BattleDef.h"

// Sesion en el BattleServer
// Cada GameServer conectado tiene una BattleSession
// Recibe requests de creacion de batalla, migracion de jugadores, etc.
class BattleSession : public i3NetworkSession
{
	I3_CLASS_DEFINE(BattleSession);

public:
	BattleSession();
	virtual ~BattleSession();

	// i3NetworkSession overrides
	virtual BOOL		Create() override;
	virtual void		Destroy() override;
	virtual BOOL		OnConnect(SOCKET Socket, struct sockaddr_in* pAddr) override;
	virtual BOOL		OnDisconnect(BOOL bForceMainThread = FALSE) override;
	virtual INT32		PacketParsing(char* pPacket, INT32 iSize) override;

	// Estado
	bool				IsRegistered() const			{ return m_bRegistered; }
	int					GetGameServerId() const			{ return m_i32GameServerId; }
	const char*			GetGameServerName() const		{ return m_szGameServerName; }

	// Response senders (llamados desde BattleRoomManager/BattleRoom)
	void				SendBattleCreateAck(int i32RoomIdx, int i32Result,
										   int i32BattleRoomIdx, const char* pszUdpIP, uint16_t ui16UdpPort);
	void				SendBattleEndNotify(BattleResult* pResult);
	void				SendPlayerMigrateAck(int64_t i64UID, int i32Result);

	// Battle live event senders (direct to GameServer, replaces CastServer)
	void				SendKillNotify(int i32RoomIdx, int i32ChannelNum,
									   uint8_t ui8RoundNum,
									   const IS_BATTLE_KILL_INFO* pKills, uint8_t ui8KillCount);
	void				SendRoundStartNotify(int i32RoomIdx, int i32ChannelNum,
											  uint8_t ui8RoundNum, uint8_t ui8GameMode);
	void				SendRoundEndNotify(int i32RoomIdx, int i32ChannelNum,
											uint8_t ui8RoundNum, uint8_t ui8EndType,
											uint8_t ui8WinTeam,
											int i32RedScore, int i32BlueScore);
	void				SendHackNotify(int i32RoomIdx, int i32ChannelNum,
										uint32_t ui32SlotIdx, int64_t i64UID,
										uint8_t ui8HackType, uint8_t ui8Severity,
										const char* pszDescription);
	void				SendMissionNotify(int i32RoomIdx, int i32ChannelNum,
										   uint8_t ui8EventType, uint32_t ui32SlotIdx,
										   int64_t i64UID, int32_t i32Param1, int32_t i32Param2);

private:
	// Packet handlers
	void				OnHeartbeatReq(char* pData, INT32 i32Size);
	void				OnBattleRegisterReq(char* pData, INT32 i32Size);
	void				OnBattleCreateReq(char* pData, INT32 i32Size);
	void				OnPlayerMigrateReq(char* pData, INT32 i32Size);
	void				OnBattleStatusUpdate(char* pData, INT32 i32Size);

	// Helpers
	void				SendHeartbeatAck();

private:
	bool				m_bRegistered;
	int					m_i32GameServerId;
	char				m_szGameServerName[64];
};

#endif // __BATTLESESSION_H__
