#ifndef _UdpBufferPool_h
#define _UdpBufferPool_h

class CUdpBuffer;

class CUdpBufferPool: public i3ElementBase
{
	I3_CLASS_DEFINE(CUdpBufferPool);
public:
	CUdpBufferPool();
	virtual ~CUdpBufferPool();

	BOOL Create(INT32 bufferCount);
	BOOL GetBuffer(CUdpBuffer** pBuffer);
	void ReleaseBuffer(CUdpBuffer* pBuffer);

private:
	CUdpBuffer* m_pBuffer;
	INT32 m_count;

	INT32* m_pEmptyBuffer;
	INT32 m_emptyCount;
	INT32 m_emptyFront;
	INT32 m_emptyRear;

	i3Mutex2 m_emptyMutex;
};

#endif
