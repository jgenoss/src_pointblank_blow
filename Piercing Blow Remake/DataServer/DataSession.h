#ifndef __DATASESSION_H__
#define __DATASESSION_H__

#pragma once
#include "i3NetworkSession.h"
#include "InterServerProtocol.h"

// Tipo de servidor que se conecto al DataServer
enum DataSessionType
{
	DATA_SESSION_UNKNOWN = 0,
	DATA_SESSION_CONNECT_SERVER,
	DATA_SESSION_GAME_SERVER,
	DATA_SESSION_BATTLE_SERVER,
};

// Sesion en el DataServer
// Cada ConnectServer, GameServer o BattleServer conectado tiene una DataSession
// Recibe requests inter-servidor y los despacha al TaskProcessor
class DataSession : public i3NetworkSession
{
	I3_CLASS_DEFINE(DataSession);

public:
	DataSession();
	virtual ~DataSession();

	virtual BOOL		Create() override;
	virtual void		Destroy() override;
	virtual BOOL		OnConnect(SOCKET Socket, struct sockaddr_in* pAddr) override;
	virtual BOOL		OnDisconnect(BOOL bForceMainThread = FALSE) override;
	virtual INT32		PacketParsing(char* pPacket, INT32 iSize) override;

	// Tipo de servidor conectado
	DataSessionType		GetSessionType() const			{ return m_eType; }
	void				SetSessionType(DataSessionType t) { m_eType = t; }
	int					GetRemoteServerId() const		{ return m_i32RemoteServerId; }

private:
	// Packet handlers inter-servidor
	void				OnHeartbeatReq(char* pData, INT32 i32Size);
	void				OnAuthLoginReq(char* pData, INT32 i32Size);
	void				OnPlayerLoadReq(char* pData, INT32 i32Size);
	void				OnPlayerSaveReq(char* pData, INT32 i32Size);
	void				OnPlayerCreateNickReq(char* pData, INT32 i32Size);
	void				OnPlayerCheckNickReq(char* pData, INT32 i32Size);
	void				OnStatsSaveReq(char* pData, INT32 i32Size);

	// Game data handlers
	void				OnEquipmentSaveReq(char* pData, INT32 i32Size);
	void				OnMedalSaveReq(char* pData, INT32 i32Size);
	void				OnAttendanceSaveReq(char* pData, INT32 i32Size);
	void				OnSkillSaveReq(char* pData, INT32 i32Size);
	void				OnQuestSaveReq(char* pData, INT32 i32Size);

	// Social handlers
	void				OnClanCreateReq(char* pData, INT32 i32Size);
	void				OnClanDisbandReq(char* pData, INT32 i32Size);
	void				OnClanJoinReq(char* pData, INT32 i32Size);
	void				OnClanLeaveReq(char* pData, INT32 i32Size);
	void				OnFriendAddReq(char* pData, INT32 i32Size);
	void				OnFriendRemoveReq(char* pData, INT32 i32Size);
	void				OnFriendListReq(char* pData, INT32 i32Size);
	void				OnBlockAddReq(char* pData, INT32 i32Size);
	void				OnBlockRemoveReq(char* pData, INT32 i32Size);
	void				OnBlockListReq(char* pData, INT32 i32Size);

	// Helpers
	void				SendHeartbeatAck();

private:
	DataSessionType		m_eType;
	int					m_i32RemoteServerId;
};

#endif // __DATASESSION_H__
