#ifndef _CIRCULAR_INDEX_H
#define _CIRCULAR_INDEX_H

// BufferIdx 입출력을 위한 전용 구조
// - Circular Queue base
// - UINT32 전용
// - 중복 허용 안함
// - 저장값 n은 0 <= n < m_nSize
// - m_nCount <= m_nSize
class CCircularIndex: public i3ElementBase
{
	I3_CLASS_DEFINE(CCircularIndex);

private:
	UINT32*					m_pBuffer;
    INT32					m_idxFront;
	INT32					m_idxRear;
	UINT32					m_iMaxCount;
	UINT32					m_nCount;

public:
	CCircularIndex();
	virtual ~CCircularIndex();

	bool					Create(UINT32 nSize);
	void					Destroy(void);

	bool					Push(UINT32 nValue,BOOL bCheckUnique = TRUE);
	BOOL					Pop(UINT32* pValue);
	
	UINT32					getCount(void) const	{ return m_nCount; }
};

#endif //_CIRCULAR_INDEX_H
