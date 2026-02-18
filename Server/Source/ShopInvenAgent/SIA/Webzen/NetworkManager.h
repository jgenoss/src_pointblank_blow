#pragma once

#include <WBANetwork/Net/ServerNetwork.h>
#include "SessionMgr.h"


class CNetworkManager : public WBANetwork::Thread
{
public:
	CNetworkManager();
	virtual ~CNetworkManager();

public:
	// 네트웍 작업을 하기 위한 초기화
	bool Initialize(WBANetwork::LPFN_ErrorHandler _errhandler, WBANetwork::ServerSession* _clinetSession);

private:
	WBANetwork::ServerNetwork	m_Network;			// WBA 네트웍 라이브러리 네트웍 관련 객체
	CSessionMgr					m_SessionMgr;		// WBA 네트웍 라이브러리 세션관리 관련 객체
	
	HANDLE						m_hTerminateThread;	// 스레드의 종료 여부를 검새하는 이벤트 핸들
	bool						m_bIsInit;			// 초기화가 되어있는지 검사하는 플레그

protected:
	// 부모 클래스에서 선언된 메소드 재정의 (스레드 실행 프로시져)
	virtual void Run();

private:
	//클라이언트 종료 처리
	void terminate();

};
