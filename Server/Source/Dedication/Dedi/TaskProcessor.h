#ifndef _TaskProcessor_h
#define _TaskProcessor_h

class CDediRoom; 

//	- 패킷 저장소(g_pDataArchive)에 적재된 작업을 처리한다.
//		- 최소한 63ms에 한번씩 OnUpdate가 실행되어야 한다.
//      - 참고: 현재는 '간격'을 이용하고 있으나 '시간'을 이용하도록 수정할 예정.
//		-		선행조건으로 'Task 63ms 이내 완료'가 보장되어야 한다.
// 
class CTaskProcessor
{
public:
	// thread function
	static DWORD WINAPI TaskProcessorThread(LPVOID lpParameter);	// task processor main thread

	CTaskProcessor();
	virtual ~CTaskProcessor();

protected:
	// call back functions
	static	void OnUpdate( void* pParam, DWORD dwDelta = 1);		// 스레드 루프 내에서 주기적으로 실행됨

	static	void ProcessRoom_Relay( INT32 i32TaskThreadIdx, CDediRoom* pRoom, INT32 i32GroupIdx, UINT32 ui32Delta ); 
	static	void ProcessRoom( INT32 i32TaskIdx, CDediRoom* pRoom, INT32 i32GroupIdx ); 
};



extern CTaskProcessor* g_pTaskProcessor;

#endif //_TaskProcessor_h
