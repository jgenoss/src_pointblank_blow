#ifndef __RING_BUFFEER_H__
#define __RING_BUFFEER_H__

#define RINGBUFFER_SUCCESS					0x00000001
#define RINGBUFFER_ERROR_BUFFER_FULLIDX		0x80000001
#define RINGBUFFER_ERROR_BUFFER_FULL		0x80000002
/* 
만든 목적은 2개의 Thead에서 
1. Write  
2. Read
를 할때 Critical Section을 사용하지 않고 버퍼를 제공한다. 

*/ 
class S2RingBuffer
{
protected: 
	UINT32					m_ui32BufferSize; 
	UINT32					m_ui32BufferCount;
	
	volatile UINT32			m_ui32WriteIdx;
	volatile UINT32			m_ui32ReadIdx;

	char	*				m_pBuffer; 

	UINT32					m_ui32WriteMax;

public:
	S2RingBuffer(); 
	virtual ~S2RingBuffer(); 

	//버퍼 사이즈와 갯수로 저장소를 만든다. 
	bool					Create(INT32 i32BufferSize, INT32 i32BufferCount); 
	bool					Destroy(void);

	UINT32					GetEmptyBufferCount(void);	// 남은 버퍼의 카운트를 전달 한다. 
	UINT32					GetBufferCount(void);		// 사용하는 버퍼의 카운트를 전달한다. 
	UINT32					GetWriteMax()						{	return m_ui32WriteMax;	}			// 최대 Write 한 횟수

	//Data Save 
	INT32					Push(void * pBuffer);		//데이터 Save 
	INT32					Push(void * pBuffer, INT32 i32Size);
	void *					PushPointer(void);			//데이터 Save Pointer		-	PushPointerIdx	<- 이것과 한쌍입니다.
	void					PushPointerIdx(void);		//데이터 Save (Add Idx)		-	PushPointer		<- 이것과 한쌍입니다.

	//Data Load 
	void *					Pop(void);					//데이터 Read
	void					PopIdx(void);				//데이터 ReadIdx 
}; 

#endif
