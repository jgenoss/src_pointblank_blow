#ifndef __I3_MULTIPLEVECTOR_H
#define __I3_MULTIPLEVECTOR_H

#include "i3Base.h"
#include "i3MathDefine.h"
#include "i3MathType.h"
#include "i3MathUtil.h"
#include "i3MathGlobalVariable.h"

/** \brief N항의 Vector */
struct I3_EXPORT_MATH ALIGN32 VECND
{
	INT32	m_iLength = 0;				///< Vector size
	REAL32 * m_pVec = nullptr;				///< Vector data

public:
	VECND(void) {}
	VECND( INT32 iLen)
	{
		setSize( iLen);
	}

	/** \brief 소멸자
		\note 할당된 메모리를 해제합니다. */
	~VECND( void)
	{
		I3MEM_SAFE_FREE( m_pVec);
	}
	
	/** \brief N/3 사이즈를 반환한다.
		\return size */
	INT32	getSize( void)					{ return m_iLength; }

	/** \brief N/3 사이즈를 설정한다.
		\param[in] iLen N size
		\note iLen은 3의 배수로 설정한다.
		\return N/A */
	void	setSize( INT32 iLen)
	{
		if( m_iLength < iLen)
		{
			I3MEM_SAFE_FREE( m_pVec);
			m_pVec = (REAL32*) i3MemAlloc( sizeof( REAL32) * iLen);
			I3ASSERT( m_pVec != nullptr);
		}

		m_iLength = iLen;
	}

	/** \brief Vector 값을 반환한다.
		\param[in] idx VectorN의 Index
		\return 값 */
	REAL32  getValue( INT32 idx)										{ I3ASSERT( idx < m_iLength); return *(m_pVec + idx); }

	/** \brief Vector 값을 설정한다.
		\param[in] idx VectorN의 Index
		\param[in] value 값
		\return N/A */
	void	setValue( INT32 idx, REAL32 value)							{ I3ASSERT( idx < m_iLength); *(m_pVec + idx) = value; }
	
	/** \brief Vector 배열 포인터를 반환한다.
		\return Vector 배열 포인터 */
	REAL32 * getValues( void)											{ return m_pVec; }

	/** \brief Vector 배열 포인터를 설정한다.
		\param[in] idx VectorN의 Index
		\param[in] values Vector 배열 포인터
		\param[in] iLen Vector 길이
		\return N/A */
	void	setValues( INT32 idx, REAL32 * values, INT32 iLen)
	{
		I3ASSERT( idx <= 0 && (idx + iLen) < m_iLength);
		i3mem::Copy( &m_pVec[ idx], values, sizeof(REAL32) * iLen);
	}
} ;

namespace i3VectorN
{
	/** \brief pV에 rVal값으로 채운다.
		\param[in] pV VECND 구조체 포인터
		\param[in] rVal 초기화값 
		\return N/A */
	I3_EXPORT_MATH	void	Fill( VECND * pV, REAL32 rVal);

	/** \brief pV에 0값으로 채운다.
		\param[in] pV VECND 구조체 포인터
		\return N/A */
	I3_EXPORT_MATH	void	FillZero( VECND * pV);

	/** \brief pSrc를 pDest에 더한다.
		\param[in,out] pDest VECND 구조체 포인터
		\param[in] pSrc VECND 구조체 포인터
		\return N/A */
	I3_EXPORT_MATH	void	Add( VECND * pDest, VECND * pSrc);

	/** \brief pSrc를 Scale하여 pDest에 더한다.
		\param[in,out] pDest VECND 구조체 포인터
		\param[in] pSrc VECND 구조체 포인터
		\param[in] rScaleFactor Scale값
		\return N/A */
	I3_EXPORT_MATH	void	AddScaled( VECND * pDest, VECND * pSrc, REAL32 rScaleFactor );

	/** \brief pDest를 Scale한다.
		\param[in,out] pDest VECND 구조체 포인터
		\param[in] rScaleFactor Scale값
		\return N/A */
	I3_EXPORT_MATH	void	Scale( VECND * pDest, REAL32 rScaleFactor);

	/** \brief pOut = pV1 - pV2
		\param[out] pOut VECND 구조체 포인터
		\param[in] pV1 VECND 구조체 포인터
		\param[in] pV2 VECND 구조체 포인터
		\return N/A */
	I3_EXPORT_MATH	void	Sub( VECND * pOut, VECND * pV1, VECND * pV2);

	/** \brief pOut = pV1 - pV2
		\param[out] pOut VECND 구조체 포인터
		\param[in] pV1 VEC3D 구조체 포인터
		\param[in] pV2 VEC3D 구조체 포인터
		\return N/A */
	I3_EXPORT_MATH	void	Sub( VECND * pOut, VEC3D * pV1, VEC3D * pV2);

	/** \brief pV1과 pV2를 Dot한다.
		\param[in] pV1 VECND 구조체 포인터
		\param[in] pV2 VECND 구조체 포인터
		\return Dot값 */
	I3_EXPORT_MATH	REAL32	Dot( VECND * pV1, VECND * pV2);

	/** \brief pV의 LengthSq를 반환한다.
		\param[in] pV VECND 구조체 포인터
		\return LengthSq */
	I3_EXPORT_MATH	REAL32	LengthSq( VECND * pV);

	/** \brief pV의 Length를 반환한다.
		\param[in] pV VECND 구조체 포인터
		\return Length */
	I3_EXPORT_MATH	REAL32	Length( VECND * pV);

	/** \brief Max값을 반환한다.
		\param[in] pV VECND 구조체 포인터
		\return Max값 */
	I3_EXPORT_MATH	REAL32	MaxAbs( VECND * pV);

	/** \brief pIn 배열을 Scale하여 pOut에 담는다.
		\param[out] pOut VECND 구조체 포인터
		\param[in] pIn REAL32 배열 포인터
		\param[in] scale Scale값
		\return N/A */
	I3_EXPORT_MATH	void	LoadScaled( VECND * pOut, REAL32 * pIn, REAL32 scale);
	
};

#endif
