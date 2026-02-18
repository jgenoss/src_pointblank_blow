#pragma once

#include <WBANetwork/Net/ServerSession.h>
#include "NetworkManager.h"


class CClientSession : public WBANetwork::ServerSession
{
public:
	CClientSession();
	virtual ~CClientSession();

protected:
	CNetworkManager		m_NetworkManager;

public:
	bool CreateSession(WBANetwork::LPFN_ErrorHandler _errhandler = NULL);
	//void Connect(TCHAR* ipAddress, unsigned short portNo);
	//void CloseSocket();
	bool SendData(void* _buffer, DWORD _size);

protected:
	virtual	void OnAccept()			{};
	virtual	void OnSend(int _size)	{};
	//virtual	void OnReceive( PBYTE buffer, int size ) {};
	virtual	void OnClose(DWORD _error)	{};

};
