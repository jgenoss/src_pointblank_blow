#pragma once

// -------------------------------------------------------------------------------------
// iocp worker parameter
// -------------------------------------------------------------------------------------
struct THREAD_OBJ
{
	SOCKET		m_socket;						// 감시 스레드
	HANDLE		m_hEvent;								// 소켓 이벤트
	
	THREAD_OBJ()
	{
		m_socket			= INVALID_SOCKET;
		m_hEvent			= NULL;
	}
	THREAD_OBJ( SOCKET s, HANDLE hEvent )
	{
		m_socket	= s;
		m_hEvent	= hEvent;
	}
};

class CUdpBuffer;
// udp send / receive
class CUdpService
{
public:
	BOOL Create();
	BOOL Start();
	void Stop();

	// send
	static  int	SendPacket( CUdpBuffer* pBuf );	// 일반 데이터용
	
	static	int HandleIo( THREAD_OBJ* pThreadObj );
	static	int SendPendingData( SOCKET s );
	static  int ReceivePendingData( SOCKET s );
	
	CUdpService(void);
	~CUdpService(void);

static DWORD WINAPI ChildThread(LPVOID lpParameter);
	
static	SOCKET			m_socket;	// server socket
static 	HANDLE			m_hEvent;	// socket event
static	HANDLE			m_hThread;	// io thread
};

extern CUdpService*	g_pUdpService;