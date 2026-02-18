#if !defined( __I3_ANIMATION2_H)
#define __I3_ANIMATION2_H

#include "i3Base.h"
#include "i3Animation.h"

#define I3_ANIMATION2_FLAG_REAL16	0x00000001			///< REAL16을 사용합니다.

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

typedef struct ALIGN4 _tagi3PersistAnimTrack
{
	UINT32		m_Channels;
	INT32		m_KeyCount;
	REAL32		m_KeyInterval;

	INT32 *		m_pTIndex;
	INT32 *		m_pRIndex;
	INT32 *		m_pSIndex;

	REAL32 *	m_pTime;

	REAL32		m_TimeOffset;
	REAL32		m_Duration;

	INT8		m_TIMode;
	INT8		m_RIMode;
	INT8		m_SIMode;

	UINT32		pad[8];

public:
	INT32		_getTimeIndex( REAL32 tm);

	void		Create( i3TransformSequence * pSeq);
	INT32		CalcBufferSize(void);
	UINT32		StoreData( i3Stream * pFile);
	UINT32		LoadData( char * pData);
} I3_ANIM_TRACK;

/** \brief Optimize된 Animation track info for 16Bits */
typedef struct ALIGN4 _tagi3PersistAnimTrack2
{
	UINT32		m_Channels;			///< Translation, Rotation,  Scale Channel
	UINT16		m_KeyCount;			///< Frame Key count
	REAL32		m_KeyInterval;		///< Key interval

	UINT16 *	m_pTIndex;			///< Translate Index array
	UINT16 *	m_pRIndex;			///< Rotation Index array
	UINT16 *	m_pSIndex;			///< Scale Index array

	REAL16 *	m_pTime;			///< Frame Times

	REAL32		m_TimeOffset;		///< Time Offset
	REAL32		m_Duration;			///< total duration

	INT8		m_TIMode;			///< transform mode
	INT8		m_RIMode;			///< rotation mode
	INT8		m_SIMode;			///< scale mode

	UINT32		pad[8];

public:
	INT32		_getTimeIndex( REAL32 tm);

	/** \brief pSeq로 정보를 만든다.
		\param[in] pSeq i3TransformSequenct 객체 포인터
		\return N/A */
	void		Create( i3TransformSequence * pSeq);

	/** \brief Track buffer size를 계산한다.
		\return Buffer size */
	INT32		CalcBufferSize(void);

	/** \brief File로 저장한다.
		\param[in] pFile i3Stream 객체 포인터
		\return Size */
	UINT32		StoreData( i3Stream * pFile);

	/** \brief pData를 parsing하여 data를 구축한다.
		\param[in] pData Data
		\return size */
	UINT32		LoadData( char * pData);
} I3_ANIM_TRACK2;


#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_MATH i3Animation2 : public i3Animation
{
	I3_CLASS_DEFINE( i3Animation2);

protected:
	INT32	 *			m_pBoneIndex;

	VEC3D *				m_pTArray;
	QUATERNION *		m_pRArray;
	VEC3D *				m_pSArray;

	I3_ANIM_TRACK *		m_pSeq;

	REAL16 *	m_pTArray16Bits;		///< for 16Bits Translation Array
	REAL16 *	m_pRArray16Bits;		///< for 16Bits Rotation Array
	REAL16 *	m_pSArray16Bits;		///< for 16Bits Scale Array

	I3_ANIM_TRACK2 *	m_pSeq2;		///< for 16Bits

	bool				m_bIs16Bits;

public:
	i3Animation2(void);
	virtual ~i3Animation2(void);

	virtual BOOL		Create( INT32		TrackCount, bool b16BitsMode = false);

	void				Create( i3Animation * pAnim);

	INT32				CalcBufferSize(void);

	I3_ANIM_TRACK *		getSequence( INT32 idx)					{ return & m_pSeq[idx]; }

	INT32 *				getBoneIndexTable(void)					{ return m_pBoneIndex; }
	void				setBoneIndexTable( INT32 * pTable)		{ m_pBoneIndex = pTable; }

	INT32				GetBoneIndex( INT32 idx)				{ return m_pBoneIndex[idx]; }
	void				SetBoneIndex( INT32 idx, INT32 bidx)	{ m_pBoneIndex[idx] = bidx; }

	virtual UINT32		GetInterpolatedKeyframe( INT32 idxTrack, I3MATRIXINFO * pInfo, BOOL bFirst);
	virtual void		GetKeyframe( INT32 idxTrack, INT32 idxKey, MATRIX * pMtx);

	UINT32				StoreData( i3Stream * pStream);
	UINT32				LoadData( i3MemoryBuffer * pBuff, INT32 offset, INT32 TrackCount, REAL32 duration);
	void				setArrays( VEC3D * pTArray, QUATERNION * pRArray, VEC3D * pSArray)
	{
		m_pTArray = pTArray;
		m_pRArray = pRArray;
		m_pSArray = pSArray;
	}

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	/** \brief pBuff에서 읽어온다. (for 16Bits)
		\param[in] pBuff data memory pointer
		\param[in] offset memory offset
		\param[in] TrackCount Track Count
		\param[in] duration animation duration
		\param[in] nTrackType 1이면 32bit
		\return Size */
	UINT32				LoadData16Bits( i3MemoryBuffer * pBuff, INT32 offset, INT32 TrackCount, REAL32 duration, INT32 nTrackType = 0);

	/** \brief 16Bits용 Array를 Bind한다.
		\param[in] pTArray REAL16 배열 포인터
		\param[in] pRArray REAL16 배열 포인터
		\param[in] pSArray REAL16 배열 포인터
		\return N/A */
	void				setArrays( REAL16 * pTArray, REAL16 * pRArray, REAL16 * pSArray)
	{
		m_pTArray16Bits = pTArray;
		m_pRArray16Bits = pRArray;
		m_pSArray16Bits = pSArray;
	}

	/** \brief Track info를 반환한다.
		\param[in] idx Track Index
		\return I3_ANIM_TRACK2 구조체 포인터 */
	I3_ANIM_TRACK2 *	getSequence2( INT32 idx)					{ return & m_pSeq2[idx]; }

	/** \brief 16Bits Data 유무 확인
		\return true : 16Bits를 갖고 있다. false : 32Bits이다. */
	bool				is16Bits( void)				{ return m_bIs16Bits; }

	void				set16BitsMode( bool bVal)	{ m_bIs16Bits = bVal; }

};

#endif
