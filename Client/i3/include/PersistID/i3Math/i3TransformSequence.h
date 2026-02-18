#if !defined( __I3_TRANSFORM_SEQUENCE_H)
#define __I3_TRANSFORM_SEQUENCE_H

#include "i3TransformSource.h"

class I3_EXPORT_MATH i3TransformSequence : public i3TransformSource
{
	I3_CLASS_DEFINE( i3TransformSequence);

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
	};

protected:
	I3_KEY_CHANNEL		m_Channels;

	INT32				m_KeyCount;

	i3MemoryBuffer *	m_pBindBuf;
	char *				m_pBuffer;

	RT_REAL32			m_KeyInterval;
	RT_VEC3D *			m_pTranslation;
	QUATERNION *			m_pRotation;
	RT_VEC3D *			m_pScale;
	RT_REAL32 * 		m_pTime;

	RT_REAL32			m_TimeOffset;
	RT_REAL32			m_Duration;

	I3_INTERPOLATE_MODE	m_TranslateInterpolateMode;
	I3_INTERPOLATE_MODE	m_RotateInterpolateMode;
	I3_INTERPOLATE_MODE	m_ScaleInterpolateMode;

protected:
	UINT32				CalcBufferSize( INT32 KeyCount, I3_KEY_CHANNEL Channels);

	UINT32				_GetTimeIndex( RT_REAL32 tm);

public:
	i3TransformSequence(void);
	virtual ~i3TransformSequence(void);

	void		Create( INT32 KeyCount, RT_REAL32 Duration, I3_KEY_CHANNEL Channels);

	void		Bind( char * pBuffer, INT32 KeyCount, RT_REAL32 Duration, I3_KEY_CHANNEL Channels, i3MemoryBuffer * pBuf);

	void *		getBuffer(void)						{ return m_pBuffer; }
	UINT32		getBufferSize(void)					{ return CalcBufferSize( m_KeyCount, m_Channels); }

	void		SetMatrix( INT32 Idx, RT_MATRIX * pMatrix);
	void		SetTranslation( INT32 Idx, RT_VEC3D * pVec);
	void		SetRotation( INT32 Idx, RT_QUAT * pQuat);
	void		SetScale( INT32 Idx, RT_VEC3D * pVec);
	void		SetTime( INT32 Idx, RT_REAL32 tm);

	void		GetKeyMatrix( INT32 Idx, RT_MATRIX * pMatrix);
	void		GetTranslation( INT32 Idx, RT_VEC3D * pVec);
	void		GetRotation( INT32 Idx, RT_QUAT * pQuat);
	void		GetScale( INT32 Idx, RT_VEC3D * pVec);
	RT_REAL32		GetTime( INT32 Idx);

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

	BOOL			HasTranslation(void)					{ return (m_Channels & TRANSLATE) != 0; }
	BOOL			HasRotation(void)						{ return (m_Channels & ROTATE) != 0; }
	BOOL			HasScale(void)							{ return (m_Channels & SCALE) != 0; }
	BOOL			HasTime(void)							{ return (m_Channels & TIME) != 0; }
	I3_KEY_CHANNEL	GetKeyframeChannels(void)				{ return m_Channels; }
	void			SetKeyframeChannels(I3_KEY_CHANNEL Channel){ m_Channels = Channel;}

	void			SetKeyframeInterval( RT_REAL32 val)			{ m_KeyInterval = val; }
	RT_REAL32		GetKeyframeInterval(void)					{ return m_KeyInterval; }

	UINT32			GetKeyframeCount(void)						{ return m_KeyCount; }

	void			SetTimeOffset( RT_REAL32 val)				{ m_TimeOffset = val ; }
	RT_REAL32		GetTimeOffset(void)							{ return m_TimeOffset; }

	void				Optimize(void);
 
	virtual RT_REAL32	GetDuration(void);
	virtual void GetMatrix( I3MATRIXINFO * pInfo, BOOL bFirst);
	virtual UINT32	GetQuaternion( I3MATRIXINFO * pInfo, RT_QUAT * pQ, RT_VEC3D * pPos, RT_VEC3D * pScale, BOOL bFirst);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};

#endif
