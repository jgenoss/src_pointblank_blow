// iocpWorker.h
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	iocp worker thread class
//	현재 객체를 생성하지 않고 static function만 사용하고 있음
//
// Dependency: 
//		
//
#ifndef _iocpWorker_h_
#define _iocpWorker_h_

#include "iocpServer.h"
#include "iocpWorkerImpl.h"

class CUdpBuffer;

///////////////////////////////////////////////////////////////////////////////
//
// Class: CIocpWorker
//
// Description:
//	- IOCP worker thread wrapper
//		
class CIocpWorker: public i3ElementBase
{	
	I3_CLASS_DEFINE(CIocpWorker);

public:
	static DWORD WINAPI CompletionThread( LPVOID lpParameter);
	
	CIocpWorker();
	virtual ~CIocpWorker();

private:
	static void		HandleIo( SOCKET_OBJ *pSock, CUdpBuffer* pBuf, DWORD dwBytesTransferred, DWORD dwError);
};

#endif
