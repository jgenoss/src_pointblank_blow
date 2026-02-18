#ifndef _TaskMgr_h
#define _TaskMgr_h

// task processor 스레드 파라미터:
//	udp packet 처리 및 broadcasting을 담당한다.
struct TASK_PROCESSOR_PARAM
{
	INT32	m_i32ThreadIdx;				// thread id - Create 함수에서 설정됨
	INT32	m_i32TaskThreadCount;		// task processor count : 작업 대상을 분할 하기 위함
	INT32	m_i32GroupMax;				// 최대 group의 개수 (total)
	UINT32	m_ui32PollingTime;			// timeout check 용
}; 

// task security 스레드 파라미터: 
//	1. checkList에 비트필드로 표시된 검사항목을 check
//	2. control server와 tcp 통신만 필요하기 iocp 관련 정보 필요 없음.
//  3. g_pRoomManager(그룹 관리자)와 g_pMapManager(맵 관리자)에 접근해야 한다.
struct TASK_SECURITY_PARAM
{
	INT32	m_i32ThreadIdx;					// thread id - Create 함수에서 설정됨
	INT32	m_i32GroupMax;					// 최대 group의 개수 (total)
	UINT32	m_ui32CheckList;				// check list for cheating (config.ini)
};


// ------------------------------------------------------------------
// CTaskMgr
//	: task processor 스레드 관리 모듈
//
class CTaskMgr 
{
public:
	BOOL Create(	INT32	i32TaskThreadCount, 
					INT32	i32GroupCount, 
					UINT32	ui32PollingTime
				);	// launch task processor threads

	CTaskMgr(void);
	virtual ~CTaskMgr(void);

private:
	static INT32	m_i32GroupMax;
	static UINT32	m_ui32PollingTime;

	static HANDLE	m_ahTaskThreads[ DS_MAX_TASK_THREAD_COUNT];			
	static INT32	m_i32TaskThreadCount;
};

extern CTaskMgr* g_pTaskMgr;

#endif
