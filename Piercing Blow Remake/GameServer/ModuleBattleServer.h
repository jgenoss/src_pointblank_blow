#ifndef __MODULEBATTLESERVER_H__
#define __MODULEBATTLESERVER_H__

#pragma once
#include "ModuleBase.h"
#include "InterServerProtocol.h"

class GameSession;

// Modulo de comunicacion con BattleServer
// Se registra al conectar, envia requests de creacion de batalla,
// recibe notificaciones de fin de batalla con resultados
class ModuleBattleServer : public ModuleBase
{
public:
	ModuleBattleServer();
	virtual ~ModuleBattleServer();

	// Server info (set before Initialize)
	void		SetServerInfo(int i32ServerId, const char* pszName,
							  const char* pszPublicIP, uint16_t ui16PublicPort,
							  int i32MaxPlayers);

	// Request operations
	void		RequestBattleCreate(int i32RoomIdx, int i32ChannelNum,
								   uint8_t ui8GameMode, uint8_t ui8MapIndex,
								   uint8_t ui8MaxPlayers, int i32PlayerCount);
	void		RequestPlayerMigrate(int64_t i64UID, int i32BattleRoomIdx,
									 int i32SlotIdx, int i32Team,
									 uint32_t ui32ClientIP, uint16_t ui16ClientPort);

	// State
	bool		IsRegistered() const			{ return m_bRegistered; }

protected:
	// ModuleBase overrides
	virtual bool	OnConnect() override;
	virtual void	OnDisconnect() override;
	virtual void	OnProcessPacket(char* pData, int i32Size) override;
	virtual void	OnHeartbeat() override;

private:
	// Response handlers
	void			OnBattleRegisterAck(char* pData, int i32Size);
	void			OnBattleCreateAck(char* pData, int i32Size);
	void			OnBattleEndNotify(char* pData, int i32Size);
	void			OnPlayerMigrateAck(char* pData, int i32Size);

private:
	// Server identity
	int				m_i32ServerId;
	char			m_szServerName[64];
	char			m_szPublicIP[MAX_SERVER_IP_LENGTH];
	uint16_t		m_ui16PublicPort;
	int				m_i32MaxPlayers;

	// Registration state
	bool			m_bRegistered;
};

#endif // __MODULEBATTLESERVER_H__
