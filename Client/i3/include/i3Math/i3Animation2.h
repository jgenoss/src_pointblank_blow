#if !defined( __I3_ANIMATION2_H)
#define __I3_ANIMATION2_H

#include "i3Base.h"
#include "i3Animation.h"

#define I3_ANIMATION2_FLAG_REAL16	0x00000001			///< REAL16을 사용합니다.

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace i3
{
	namespace pack
	{
		namespace animation
		{
			struct ALIGN4 TRACK
			{
				UINT32		m_Channels = 0;
				INT32		m_KeyCount = 0;
				REAL32		m_KeyInterval = 0.0f;

				INT32 *		m_pTIndex = nullptr;
				INT32 *		m_pRIndex = nullptr;
				INT32 *		m_pSIndex = nullptr;

				REAL32 *	m_pTime = nullptr;

				REAL32		m_TimeOffset = 0.0f;
				REAL32		m_Duration = 0.0f;

				INT8		m_TIMode = 0;
				INT8		m_RIMode = 0;
				INT8		m_SIMode = 0;

				UINT32		pad[8] = { 0 };

			public:
				INT32		_getTimeIndex(REAL32 tm);

				void		Create(i3TransformSequence * pSeq);
				INT32		CalcBufferSize(void);
				UINT32		StoreData(i3Stream * pFile);
				UINT32		LoadData(char * pData);
			};

			/** \brief Optimize된 Animation track info for 16Bits */
			struct ALIGN4 TRACK2
			{
				UINT32		m_Channels = 0;			///< Translation, Rotation,  Scale Channel
				UINT16		m_KeyCount = 0;			///< Frame Key count
				REAL32		m_KeyInterval = 0.0f;		///< Key interval

				UINT16 *	m_pTIndex = nullptr;			///< Translate Index array
				UINT16 *	m_pRIndex = nullptr;			///< Rotation Index array
				UINT16 *	m_pSIndex = nullptr;			///< Scale Index array

				REAL16 *	m_pTime = nullptr;			///< Frame Times

				REAL32		m_TimeOffset = 0.0f;		///< Time Offset
				REAL32		m_Duration = 0.0f;			///< total duration

				INT8		m_TIMode = 0;			///< transform mode
				INT8		m_RIMode = 0;			///< rotation mode
				INT8		m_SIMode = 0;			///< scale mode

				UINT32		pad[8] = { 0 };

			public:
				INT32		_getTimeIndex(REAL32 tm);

				/** \brief pSeq로 정보를 만든다.
					\param[in] pSeq i3TransformSequenct 객체 포인터
					\return N/A */
				void		Create(i3TransformSequence * pSeq);

				/** \brief Track buffer size를 계산한다.
					\return Buffer size */
				INT32		CalcBufferSize(void);

				/** \brief File로 저장한다.
					\param[in] pFile i3Stream 객체 포인터
					\return Size */
				UINT32		StoreData(i3Stream * pFile);

				/** \brief pData를 parsing하여 data를 구축한다.
					\param[in] pData Data
					\return size */
				UINT32		LoadData(char * pData);
			};
		}
	}
}


#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_MATH i3Animation2 : public i3Animation
{
	I3_EXPORT_CLASS_DEFINE( i3Animation2, i3Animation);

protected:
	INT32	 *			m_pBoneIndex = nullptr;

	VEC3D *				m_pTArray = nullptr;
	QUATERNION *		m_pRArray = nullptr;
	VEC3D *				m_pSArray = nullptr;

	i3::pack::animation::TRACK *		m_pSeq = nullptr;

	REAL16 *	m_pTArray16Bits = nullptr;		///< for 16Bits Translation Array
	REAL16 *	m_pRArray16Bits = nullptr;		///< for 16Bits Rotation Array
	REAL16 *	m_pSArray16Bits = nullptr;		///< for 16Bits Scale Array

	i3::pack::animation::TRACK2 *	m_pSeq2 = nullptr;		///< for 16Bits

	bool				m_bIs16Bits = false;


public:
	virtual ~i3Animation2(void);

	virtual bool		Create( INT32		TrackCount, bool b16BitsMode = false);

	void				Create( i3Animation * pAnim);

	INT32				CalcBufferSize(void);

	i3::pack::animation::TRACK *		getSequence( INT32 idx)					{ return & m_pSeq[idx]; }

	INT32 *				getBoneIndexTable(void)					{ return m_pBoneIndex; }
	void				setBoneIndexTable( INT32 * pTable)		{ m_pBoneIndex = pTable; }

	INT32				GetBoneIndex( INT32 idx)				{ return m_pBoneIndex[idx]; }
	void				SetBoneIndex( INT32 idx, INT32 bidx)	{ m_pBoneIndex[idx] = bidx; }

	virtual UINT32		GetInterpolatedKeyframe( INT32 idxTrack, I3MATRIXINFO * pInfo, bool bFirst);
	virtual void		GetKeyframe( INT32 idxTrack, INT32 idxKey, MATRIX * pMtx);

	UINT32				StoreData( i3Stream * pStream);
	UINT32				LoadData( i3MemoryBuffer * pBuff, INT32 offset, INT32 TrackCount, REAL32 duration);
	void				setArrays( VEC3D * pTArray, QUATERNION * pRArray, VEC3D * pSArray)
	{
		m_pTArray = pTArray;
		m_pRArray = pRArray;
		m_pSArray = pSArray;
	}

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

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
	i3::pack::animation::TRACK2 *	getSequence2( INT32 idx)					{ return & m_pSeq2[idx]; }

	/** \brief 16Bits Data 유무 확인
		\return true : 16Bits를 갖고 있다. false : 32Bits이다. */
	bool				is16Bits( void)				{ return m_bIs16Bits; }

	void				set16BitsMode( bool bVal)	{ m_bIs16Bits = bVal; }

};

#endif
