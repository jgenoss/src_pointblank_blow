#ifndef _ModulePhysics_H
#define _ModulePhysics_H

class CUdpBuffer;
class CTrackBuffer;

class CModulePhysics
{
public:
	
	BOOL	SendToPhysicsServer		( CUdpBuffer*	pSendBuf );
	
	BOOL	Create( INT32 i32BufferSize );
	static BOOL	EnqueueToHackChecker	( CUdpBuffer*	pRecvBuf  );
	
	static DWORD WINAPI ThreadPhysicsRecv( LPVOID lpParameter ); // recv thread
	
	CModulePhysics();
	~CModulePhysics();

private:
	SOCKET	_CreateUdpSocket();
	
	static HANDLE	m_hThreadPhysicsRecv;
	static SOCKET	m_socket;	// client socket
};

extern CModulePhysics* g_pModulePhysics;

#endif