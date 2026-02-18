// TaskProcessorImpl.h
//
// YouJong Ha
//	Last update : 2011-08-26 (yyyy:mm:dd)
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

class CUdpBuffer;
void	ProcessRecvPacket( INT32 i32ThreadIdx, CUdpBuffer* pRecvBuff ) ; 

#endif