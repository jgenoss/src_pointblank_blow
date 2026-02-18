#ifndef _MonitorThreads_H_
#define _MonitorThreads_H_

//	IOCP Server와 TaskManager에서 공통으로 사용
struct MONITOR_THREAD_PARAM
{
	BOOL    (*m_pfnCreateThread)(DS_THREAD_TYPE Type, INT32 i32ThreadIndex);	// 쓰레드 생성 함수
	HANDLE	m_hevtQuit;									// 종료 이벤트
	HANDLE*	m_phThreads;								// thread handles : 모니터 대상
	UINT32	m_ui32ThreadCount;							// count 
	DS_THREAD_TYPE	m_Type;								// thread type for recreation
};

// 스레드 모니터 
//	스레드가 종료 되었을 때 스레드를 다시 시작하는 기능을 수행한다
class CMonitorThread
{
public:
	CMonitorThread(void);
	~CMonitorThread(void);

	static DWORD WINAPI MonitorThread(LPVOID lpParameter);
};

#endif