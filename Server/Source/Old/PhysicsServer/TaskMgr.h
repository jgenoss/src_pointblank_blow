#ifndef _TaskMgr_h
#define _TaskMgr_h

// task processor 스레드 파라미터:
//	udp packet 처리 및 broadcasting을 담당한다.
struct PX_TASK_PROCESSOR_PARAM
{
	INT32	m_i32ThreadIdx;			// thread id - Create 함수에서 설정됨
	INT32	m_i32TaskThreadCount;	// task processor count : 작업 대상을 분할 하기 위함
	UINT32	m_ui32PollingTime;		// timeout check 용
	UINT32	m_ui32EpsilonTime;		// 미소 시간

	PX_TASK_PROCESSOR_PARAM()
	{
		m_i32ThreadIdx = -1;
		m_i32TaskThreadCount = 0;
		m_ui32PollingTime	 = 0;
		m_ui32EpsilonTime	 = 0;
	}
}; 

// ------------------------------------------------------------------
// CTaskMgr
//	: task processor 스레드와 security check 스레드를 관리한다
//
class CTaskMgr 
{
public:
	BOOL Create(	INT32	i32TaskThreadCount, 
					UINT32	ui32PollingTime,
					UINT32	ui32EpsilonTime ); // launch task processor threads
	
	BOOL Start();
	void Stop();

	CTaskMgr(void);
	virtual ~CTaskMgr(void);

private:
	static BOOL		ReCreateThread( DS_THREAD_TYPE Type, INT32 i32ThreadIdx);
	
	static UINT32	m_ui32PollingTime;
	static UINT32	m_ui32EpsilonTime;
	static HANDLE	m_phTaskThreads[ PX_MAX_TASK_THREAD_COUNT];			
	static INT32	m_i32TaskThreadCount;
	static HANDLE	m_hTaskMonitorThread;		// monitor threads
};

extern CTaskMgr* g_pTaskMgr;

#endif
