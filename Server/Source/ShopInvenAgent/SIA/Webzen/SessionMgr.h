#pragma once

#include <WBANetwork/Net/SessionManager.h>
#include <WBANetwork/Net/ServerSession.h>


class CSessionMgr : public WBANetwork::SessionManager
{
public:
	CSessionMgr();
	~CSessionMgr();

private:
	WBANetwork::ServerSession* m_ClinetSession;

public:
	void	BindClient(WBANetwork::ServerSession* _clientSession);

protected:	
	virtual	WBANetwork::ServerSession*	CreateSession();
	virtual	void						DeleteSession(WBANetwork::ServerSession* _session);

};
