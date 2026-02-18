#if !defined( __I3_TRANSFORM_SEQUENCE_H)
#define __I3_TRANSFORM_SEQUENCE_H

#include "i3TransformSource.h"

class I3_EXPORT_MATH i3TransformSequence : public i3TransformSource
{
	I3_EXPORT_CLASS_DEFINE( i3TransformSequence, i3TransformSource);

public:
	enum I3_INTERPOLATE_MODE
	{
		NONE					= 0,
		LINEAR					= 1,
		BEZIER					= 2,			// for Translation, Scaling
		SPLINE					= 3,
		SLERP					= 4,			// for Rotation
	};

	enum I3_KEY_CHANNEL
	{
		TRANSLATE				= 0x00000001,
		ROTATE					= 0x00000002,
		SCALE					= 0x00000004,
		TIME					= 0x00000008,
		FIXED_TRANSLATE			= 0x00000010,
		FIXED_ROTATE			= 0x00000020,
		FIXED_SCALE				= 0x00000040,
		REAL16_FORMAT			= 0x00000080,	///< 16Bits Mode Format
	};

protected:
	I3_KEY_CHANNEL		m_Channels = (I3_KEY_CHANNEL)0;

	INT32				m_KeyCount = 0;

	i3MemoryBuffer *	m_pBindBuf = nullptr;
	char *				m_pBuffer = nullptr;

	REAL32			m_KeyInterval = 0.0f;
	VEC3D *			m_pTranslation = nullptr;
	QUATERNION *		m_pRotation = nullptr;
	VEC3D *			m_pScale = nullptr;
	REAL32 * 		m_pTime = nullptr;

	REAL32			m_TimeOffset = 0.0f;
	REAL32			m_Duration = 0.0f;

	I3_INTERPOLATE_MODE	m_TranslateInterpolateMode = LINEAR;
	I3_INTERPOLATE_MODE	m_RotateInterpolateMode = LINEAR;
	I3_INTERPOLATE_MODE	m_ScaleInterpolateMode = LINEAR;

protected:
	UINT32				CalcBufferSize( INT32 KeyCount, I3_KEY_CHANNEL Channels);

	UINT32				_GetTimeIndex( REAL32 tm);

public:
	i3TransformSequence(void);
	virtual ~i3TransformSequence(void);

	void		Create( INT32 KeyCount, REAL32 Duration, I3_KEY_CHANNEL Channels);

	void		Bind( char * pBuffer, INT32 KeyCount, REAL32 Duration, I3_KEY_CHANNEL Channels, i3MemoryBuffer * pBuf);

	void *		getBuffer(void)						{ return m_pBuffer; }
	UINT32		getBufferSize(void)					{ return CalcBufferSize( m_KeyCount, m_Channels); }

	INT32		getTranslateKeyCount(void);
	VEC3D *		getTranslateKeyBuffer(void)			{ return m_pTranslation; }

	INT32		getRotateKeyCount(void);
	QUATERNION *	getRotateKeyBuffer(void)			{ return m_pRotation; }

	INT32		getScaleKeyCount(void);
	VEC3D *		getScaleKeyBuffer(void)				{ return m_pScale; }

	INT32		getTimeKeyCount(void);
	REAL32 *	getTimeKeyBuffer(void)				{ return m_pTime; }

	void		SetMatrix( INT32 Idx, MATRIX * pMatrix);
	void		SetTranslation( INT32 Idx, VEC3D * pVec);
	void		SetRotation( INT32 Idx, QUATERNION * pQuat);
	void		SetScale( INT32 Idx, VEC3D * pVec);
	void		SetTime( INT32 Idx, REAL32 tm);

	void		GetKeyMatrix( INT32 Idx, MATRIX * pMatrix);
	void		GetTranslation( INT32 Idx, VEC3D * pVec);
	void		GetRotation( INT32 Idx, QUATERNION * pQuat);
	void		GetScale( INT32 Idx, VEC3D * pVec);
	REAL32	GetTime( INT32 Idx);

	void					SetInterpolateMode( I3_KEY_CHANNEL Channel, I3_INTERPOLATE_MODE mode);

	I3_INTERPOLATE_MODE		GetTranslateInterpolateMode(void)	{ return m_TranslateInterpolateMode; }
	void					SetTranslateInterpolateMode( I3_INTERPOLATE_MODE mode)
	{
		m_TranslateInterpolateMode = mode;
	}

	I3_INTERPOLATE_MODE		GetScaleInterpolateMode(void)	{ return m_ScaleInterpolateMode; }
	void					SetScaleInterpolateMode( I3_INTERPOLATE_MODE mode)
	{
		m_ScaleInterpolateMode = mode;
	}

	I3_INTERPOLATE_MODE		GetRotateInterpolateMode(void)	{ return m_RotateInterpolateMode; }
	void					SetRotateInterpolateMode( I3_INTERPOLATE_MODE mode)
	{
		m_RotateInterpolateMode = mode;
	}

	bool			HasTranslation(void)					{ return (m_Channels & TRANSLATE) != 0; }
	bool			HasRotation(void)						{ return (m_Channels & ROTATE) != 0; }
	bool			HasScale(void)							{ return (m_Channels & SCALE) != 0; }
	bool			HasTime(void)							{ return (m_Channels & TIME) != 0; }
	I3_KEY_CHANNEL	GetKeyframeChannels(void)				{ return m_Channels; }
	void			SetKeyframeChannels(I3_KEY_CHANNEL Channel){ m_Channels = Channel;}

	void			SetKeyframeInterval( REAL32 val)			{ m_KeyInterval = val; }
	REAL32		GetKeyframeInterval(void)					{ return m_KeyInterval; }

	UINT32			GetKeyframeCount(void)						{ return m_KeyCount; }

	void			SetTimeOffset( REAL32 val)				{ m_TimeOffset = val ; }
	REAL32		GetTimeOffset(void)							{ return m_TimeOffset; }

	#if defined( I3_DEBUG)
		void			CheckTimeTable(void);
	#endif
		 
	virtual REAL32	GetDuration(void) override;
	virtual void GetMatrix( I3MATRIXINFO * pInfo, bool bFirst) override;
	virtual UINT32	GetInterpolatedKeyframe( I3MATRIXINFO * pInfo, bool bFirst) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32 OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile) override;
};

#endif
