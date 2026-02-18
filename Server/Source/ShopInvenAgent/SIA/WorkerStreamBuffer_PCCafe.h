#ifndef __WORKER_STREAM_BUFFER_PCCAFE_H__
#define __WORKER_STREAM_BUFFER_PCCAFE_H__

class CHashTable;

class CWorkerStreamBuffer_PCCafe
{
private:
	INT32		m_i32TotalBufferCount;			// Create시 생성한 버퍼수
	INT32		m_i32UseBufferCount;			// 사용중인 버퍼수

	INT32		m_i32BlankIdx;
	INT32		m_i32PushIdx;
	INT32*		m_pi32BufferIndex;

	CHashTable*	m_pUIDTable;

	WORKER_STREAM_BUFFER_PCCAFE*	m_pBuffer;
	
	INT32		_GetNextBufferIdx();								// 빈자리 인덱스 구하기

public:
	CWorkerStreamBuffer_PCCafe();
	virtual ~CWorkerStreamBuffer_PCCafe();

	INT32		CreateBuffer( INT32 i32Count );

	INT32		Push( TYPE_PCCAFE_INPUT eType, T_UID i64UID, UINT32 ui32IP, UINT32 ui32PCGUID );
	void		SetBlankBuffer( INT32 i32BufferIdx );					// 빈버퍼로 만듬
	void		SetBlankBuffer( WORKER_STREAM_BUFFER_PCCAFE* pBuffer );	// 빈버퍼로 만듬
	void		UpdateBuffer();											// Timeout 검사

	WORKER_STREAM_BUFFER_PCCAFE*	FindByUID( T_UID i64UID );			// UID로 버퍼 찾음
};

#endif