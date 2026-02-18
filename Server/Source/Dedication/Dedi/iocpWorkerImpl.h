// IocpWorkerImpl.h
//
// YouJong Ha
//	Last update : 2010-10-04 (yyyy:mm:dd)
//	
// Description:
//	- 프로토콜 세부 사항을 처리하기 위한 파일
//
// Dependency: 
//		
//	
#ifndef _iocpWorkerImpl_H
#define _iocpWorkerImpl_H

class CPacketLocker;

///////////////////////////////////////////////////////////////////////////////
// thread status definitions
typedef enum  {	READ, 
				PREPROCESS, 
				PROCESS, 
				POSTPROCESS, 
				QUEUE, 
				INVALID_CONTEXT = -1 } CONTEXT_STATUS; 

///////////////////////////////////////////////////////////////////////////////
// thread local storage
struct IOCP_WORKER_THREAD_CONTEXT  
{

	i3ElementBase*	m_pPacket;			// input buffer  (CUdpBuffer)
	INT32			m_i32ThreadIdx;		// thread index
	INT32			m_i32GroupIdx;		// room index (0-1023) // test
	CONTEXT_STATUS	state;			// thread status flag

	// context rest : called after inserting a udp buffer
	void	Reset() { m_pPacket=NULL; m_i32ThreadIdx = -1; m_i32GroupIdx=-1; state=INVALID_CONTEXT; }	
	// constructor
	IOCP_WORKER_THREAD_CONTEXT()  {	Reset(); }
};


///////////////////////////////////////////////////////////////////////////////
// iocp worker thread internal functions
// pStop이 TRUE로 설정되어 있으면 다음 작업을 중지한다
// 프로토콜 처리 함수에서 결과값을 직접 클라이언트로 전송 (또는 오류)
//
BOOL InitIocpWorker (void* pThreadParam);	// one time initialization
BOOL CheckPacket	(CUdpBuffer* pBuffer, DWORD dwBytes );
BOOL ReadContext	(i3ElementBase* pIn, DWORD dwBytes, IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop = NULL); // setup context
void OnPreprocess	(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop = NULL);	// preprocess
void OnProcess		(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop = NULL);	// process
void OnPostprocess	(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop = NULL); // postprocess
BOOL PushContext	(CPacketLocker* pQueue, IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop = NULL); // save results

int		EchoPacket(CUdpBuffer* pUdpBuffer);
BOOL	SendUdpData(char* pBuf, UINT32 ui32Size, INT32 i32SocketIdx, UINT32 ui32IP, UINT16 ui16Port);
BOOL	GetPacketAt();

#endif // _iocpWorkerImpl_H