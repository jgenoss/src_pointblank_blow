#if !defined( __I3_VECTOR4_LIST_16BITS_H)
#define __I3_VECTOR4_LIST_16BITS_H

#include "i3Base.h"
#include "i3MathType.h"
#include "i3Matrix.h"

/** \brief 16Bits Vector4 list */
class I3_EXPORT_MATH i3Vector4List16Bits : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3Vector4List16Bits, i3ElementBase);

protected :

	INT32	m_MaxCount = 0;
	INT32	m_Count = 0;
	INT32	m_AllocUnit;
	REAL16	*	m_pList = nullptr;
	bool	m_bSort = false;
	
protected:

	bool _ResizeBuffer( INT32 Count);
	void	_QuickSort( INT32 idxLeft, INT32 idxRight, REAL32 eps);

public:

	i3Vector4List16Bits(INT32 AllocUnit = 16) : m_AllocUnit(AllocUnit) {}
	~i3Vector4List16Bits(void);

	/** \brief Sort여부를 확인한다.
		\return true : Sort. false : Not sort */
	bool	getSortState(void)						{ return m_bSort; }

	/** \brief Sort여부를 설정한다.
		\param[in] bState true : Sort. false : Not sort
		\return N/A */
	void	setSortState( bool bState)				{ m_bSort = bState; }

	/** \brief Vector값을 추가한다.
		\param[in] p VEC4D 구조체 포인터
		\param[in] eps Epsilon값 (default I3_EPS)
		\return N/A */
	INT32	Add( VEC4D * p, REAL32 eps = I3_EPS);

	/** \brief Vector를 중간에 넣는다.
		\param[in] Idx List Index
		\param[in] p VEC4D 구조체 포인터
		\return Index */
	INT32	Insert( INT32 Idx, VEC4D * p);

	/** \brief Vector를 List에서 제거한다.
		\param[in] Idx List Index
		\return Count */
	INT32	Delete( INT32 Idx);

	/** \brief p를 List에서 찾는다.
		\param[in] p VEC4D 구조체 포인터
		\param[in] bBilinear Linear search여부
		\param[in] eps Epsilon값 (default I3_EPS)
		\return Index */
	INT32	Find( VEC4D * p, bool bBilinear, REAL32 eps = I3_EPS);

	/** \brief Vector data를 Sort한다.
		\param[in] eps Epsilon (default I3_EPS)
		\return N/A */
	void	Sort( REAL32 eps = I3_EPS);

	/** \brief Redundant한 Vector를 제거한다.
		\param[in] eps Epsilon (default I3_EPS)
		\return N/A */
	void	RemoveRedundant( REAL32 eps = I3_EPS);

	/** \brief List를 초기화한다.
		\param[in] bFreeBuffer Buffer 해제 여부 (default false)
		\return N/A */
	void	Reset( bool bFreeBuffer = false);

	/** \brief List의 크기를 설정한다.
		\param[in] AllocUnit Unit size
		\return N/A */
	void	setAllocationUnit( INT32 AllocUnit);

	/** \brief List Count를 반환한다.
		\return count */
	INT32	getCount(void)					{ return m_Count; }

	/** \brief List Count를 설정한다.
		\param[in] Count count
		\return N/A */
	void	setCount( INT32 Count);

	/** \brief Vector를 반환한다.
		\param[in] idx List Index
		\return VEC4D 구조체 포인터 */
	void	GetVector( INT32 idx, VEC4D * p)
	{
		i3Vector::Set( p, m_pList[idx*4], m_pList[idx*4+1], m_pList[idx*4+2], m_pList[idx*4+3]);
	}

	/** \brief Vector를 설정한다.
		\param[in] idx List Index
		\param[in] p VEC4D 구조체 포인터
		\return N/A */
	void	SetVector( INT32 idx, VEC4D * p);

	/** \brief sidx부터 eidx까지 Vector를 치환시킨다.
		\param[in] sidx Start Index
		\param[in] eidx End Index
		\return N/A */
	void	Reverse( INT32 sidx, INT32 eidx);

	/**	\brief pDest에 복사한다.
		\param[out] pDest 복사할 대상 object
		\param[in] method 복사하는 방식
		\return N/A	*/
	virtual void CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	/** \brief Buffer를 반환한다.
		\return REAL16 배열 포인터 */
	REAL16 * getData( void)					{ return m_pList; }

	/** \brief Buffer Size를 반환한다. Vector * Count * sizeof(REAL16)
		\return size */
	UINT32	getDataSize( void)				{ return sizeof(REAL16) * 4 * m_Count; }
};

#endif
