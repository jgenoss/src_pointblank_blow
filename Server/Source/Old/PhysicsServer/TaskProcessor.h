// TaskProcessor.h
//
// YouJong Ha
//	Last update : 2010-10-06 (yyyy:mm:dd)
//	
// Description:
//	
//
//
// Dependency: 
//		
//	
#ifndef _TaskProcessor_h
#define _TaskProcessor_h

///////////////////////////////////////////////////////////////////////////////
//
// Class: CTaskProcessor
//
// Description:
//	- 패킷 저장소(g_pDataArchive)에 적재된 작업을 처리한다.
//		- 최소한 63ms에 한번씩 OnUpdate가 실행되어야 한다.
//      - 참고: 현재는 '간격'을 이용하고 있으나 '시간'을 이용하도록 수정할 예정.
//		-		선행조건으로 'Task 63ms 이내 완료'가 보장되어야 한다.
// 
class CTaskProcessor
{

public:
	CTaskProcessor();
	virtual ~CTaskProcessor();
	
public:
	// thread function
	static DWORD WINAPI TaskProcessorThread(LPVOID lpParameter);	// task processor main thread

protected:
	
	// call back functions
	static	void OnUpdate( void* pParam, DWORD dwDelta );		// 스레드 루프 내에서 주기적으로 실행됨
	
private:	
};



extern CTaskProcessor* g_pTaskProcessor;

#endif //_TaskProcessor_h
