// TaskProcessorImpl.h
//
// YouJong Ha
//	Last update : 2010-10-12 (yyyy:mm:dd)
//	
// Description:
//	- Task Processor 구현 파일
//	- Task scheduling 작업은 상위 모듈인 task processor에서 구현되어 있고, 
//		이 파일에서는 할당된 룸 1개에 대한 세부적인 작업을 구현한다.
//
// Dependency: 
//		
#ifndef _TaskProcessorImpl_H_
#define _TaskProcessorImpl_H_


void	ProcessRoomData		 ( INT32 i32TaskIdx, CDediGroup* pGroup,  INT32 i32GroupIdx );
void	Relay_ProcessRoomData( INT32 i32TaskIdx, CDediGroup* pGroup,  INT32 i32GroupIdx, UINT32 ui32Delta); 


#endif