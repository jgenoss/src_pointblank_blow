#ifndef _CircularIndex_h
#define _CircularIndex_h

// BufferIdx 입출력을 위한 전용 구조
// - Circular Queue base
// - UINT32 전용
// - 중복 허용 안함
// - 저장값 n은 0 <= n < m_nSize
// - m_nCount <= m_nSize

// ConnectedUser에도 사용

class CCircularIndex: public i3ElementBase
{
	I3_CLASS_DEFINE(CCircularIndex);

public:
	CCircularIndex();
	virtual ~CCircularIndex();

	BOOL		Create(UINT32 ui32Size);
	void		Destroy(void);

	BOOL		Push(UINT32 ui32Value,BOOL bCheckUnique = TRUE);
	BOOL		Pop(UINT32* pui32Value);
	UINT32		Pop(void);			// I3ASSERT(0 != m_nCount);

	void		Clear(void);
	
	UINT32		getCount(void) const	{ return m_ui32Count;	}

private:
	UINT32*		m_pui32Buffer;
    INT32		m_i32idxFront;
	INT32		m_i32idxRear;
	UINT32		m_ui32Size;
	UINT32		m_ui32Count;
};

#endif //_CircularIndex_h
