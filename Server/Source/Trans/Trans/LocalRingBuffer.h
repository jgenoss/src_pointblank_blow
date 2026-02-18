#ifndef __LOCAL_RING_BUFFEER_H
#define __LOCAL_RING_BUFFEER_H

#define RINGBUFFER_SUCCESS					0x00000001
#define RINGBUFFER_ERROR_BUFFER_FULLIDX		0x80000001
#define RINGBUFFER_ERROR_BUFFER_FULL		0x80000002

class Local_CRingBuffer
{
protected: 
	UINT32	m_iBufferSize; 
	UINT32	m_iBufferCount; 
	
	UINT32	m_iWriteIdx; 
	UINT32	m_iReadIdx; 

	char	* m_pBuffer; 
	
public:
	Local_CRingBuffer(); 
	virtual ~Local_CRingBuffer(); 

	//버퍼 사이즈와 갯수로 저장소를 만든다. 
	BOOL	OnCreate(INT32 iBufferSize, INT32 iBufferCount); 
	void	OnDestroy(void);

	INT32	GetEmptyBufferCount(void);	// 남은 버퍼의 카운트를 전달 한다. 
	INT32	GetBufferCount(void);		// 사용하는 버퍼의 카운트를 전달한다. 

	//Data Save 
	INT32	Push(void * pBuffer);		//데이터 Save 
	void *	PushPointer(void);			//데이터 Save Pointer		-	PushPointerIdx	<- 이것과 한쌍입니다.
    void	PushPointerIdx(void);		//데이터 Save (Add Idx)		-	PushPointer		<- 이것과 한쌍입니다.

	//Data Load 
	void *	Pop(void);					//데이터 Read
	void	PopIdx(void);				//데이터 ReadIdx 
}; 

#endif
