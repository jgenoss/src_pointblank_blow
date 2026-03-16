#ifndef __MODULEDATASERVER_H__
#define __MODULEDATASERVER_H__

#pragma once
#include "ModuleBase.h"

class GameSession;

// Modulo de comunicacion con DataServer
// Envia requests de carga/guardado de jugadores, recibe responses
class ModuleDataServer : public ModuleBase
{
public:
	ModuleDataServer();
	virtual ~ModuleDataServer();

	// Request operations (enqueue for sending)
	void		RequestPlayerLoad(int i32SessionIdx, int64_t i64UID);
	void		RequestPlayerSave(int64_t i64UID, int i32Level, int64_t i64Exp, int i32Cash, int i32GP);
	void		RequestStatsSave(int64_t i64UID, int i32Kills, int i32Deaths,
								 int i32Headshots, int i32Wins, int i32Losses);
	void		RequestCreateNick(int64_t i64UID, int i32SessionIdx, const char* pszNickname);
	void		RequestCheckNick(int i32SessionIdx, const char* pszNickname);

protected:
	// ModuleBase overrides
	virtual bool	OnConnect() override;
	virtual void	OnDisconnect() override;
	virtual void	OnProcessPacket(char* pData, int i32Size) override;
	virtual void	OnHeartbeat() override;

private:
	// Response handlers
	void		OnPlayerLoadAck(char* pData, int i32Size);
	void		OnPlayerSaveAck(char* pData, int i32Size);
	void		OnCreateNickAck(char* pData, int i32Size);
	void		OnCheckNickAck(char* pData, int i32Size);
	void		OnStatsSaveAck(char* pData, int i32Size);
};

#endif // __MODULEDATASERVER_H__
