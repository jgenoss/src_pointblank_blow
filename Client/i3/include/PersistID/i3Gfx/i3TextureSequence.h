#if !defined( __I3_TEXTURE_SEQUENCE_H)
#define __I3_TEXTURE_SEQUENCE_H

#include "i3Base.h"
#include "i3Texture.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#define I3G_TSEQ_STYLE_REPEAT			0x00000001
#define	I3G_TSEQ_STYLE_STATICTIME		0x00000002
#define I3G_TSEQ_STYLE_RANDOM			0x00000004
#define I3G_TSEQ_STYLE_FIXEDFRAME		0x00000008
 
class I3_EXPORT_GFX i3TextureSequence : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3TextureSequence);
protected:
	// Persistent Members
	BOOL		m_Style;
	RT_REAL32	m_Width;
	RT_REAL32	m_Height;
	RT_REAL32	m_Interval;
	UINT32		m_Count;
	UINT32		m_StartIndex;
	RT_REAL32	*	m_pTimeTable;	
	//INT32		m_TimeTableIdx; 
	i3Texture * m_pTexture;
	
	
	// Volatile Members
	INT32		m_LineCount;
	RT_REAL32	m_LineTime;
	RT_REAL32	m_RLineTime;
	RT_REAL32	m_RInterval;
	RT_REAL32	m_WidthFactor, m_HeightFactor;
	RT_REAL32	m_Duration;

protected:
	void		CalcParams(void);

public:
	i3TextureSequence(void);
	virtual ~i3TextureSequence(void);

	BOOL		GetUseStaticTime(void)					{ return (m_Style & I3G_TSEQ_STYLE_STATICTIME) != 0; }
	void		SetUseStaticTime( BOOL bFlag);

	BOOL		IsRepeat(void)							{ return (m_Style & I3G_TSEQ_STYLE_REPEAT) != 0; }
	void		SetRepeat( BOOL bFlag)
	{
		if( bFlag)
			m_Style |= I3G_TSEQ_STYLE_REPEAT;
		else
			m_Style &= ~I3G_TSEQ_STYLE_REPEAT;
	}

	BOOL		IsRandomMode(void)						{ return (m_Style & I3G_TSEQ_STYLE_RANDOM) != 0; }
	void		SetRandomMode( BOOL bFlag)
	{
		if( bFlag)
			m_Style |= I3G_TSEQ_STYLE_RANDOM;
		else
			m_Style &= ~I3G_TSEQ_STYLE_RANDOM;
	}

	BOOL		IsFixedFrameMode(void)					{ return (m_Style & I3G_TSEQ_STYLE_FIXEDFRAME) != 0; }
	void		SetFixedFrameMode( BOOL bFlag)
	{
		if( bFlag)
			m_Style |= I3G_TSEQ_STYLE_FIXEDFRAME;
		else
			m_Style &= ~I3G_TSEQ_STYLE_FIXEDFRAME;
	}

	RT_REAL32		GetFrameWidth(void)						{ return m_Width; }
	RT_REAL32		GetFrameHeight(void)					{ return m_Height; }
	void		SetFrameSize( RT_REAL32 w, RT_REAL32 h)
	{
		m_Width = w;
		m_Height = h;

		CalcParams();
	}

	RT_REAL32		GetStaticInterval(void)					{ return m_Interval; }
	void		SetStaticInterval( RT_REAL32 val)
	{ 
		m_Interval = val;

		CalcParams();
	}

	RT_REAL32		GetDuration(void)						{ return m_Duration; }
	void		SetDuration( RT_REAL32 dur)
	{ 
		m_Duration = dur;
		CalcParams();
	}

	UINT32		GetCount(void)							{ return m_Count; }
	void		SetCount( UINT32 c);

	UINT32		GetStartIndex(void)						{ return m_StartIndex; }
	void		SetStartIndex( UINT32 idx)				{ m_StartIndex = idx; }

	RT_REAL32		GetTime( UINT32 idx)					{ return m_pTimeTable[idx]; }
	void		SetTime( UINT32 idx, RT_REAL32 t)			{ m_pTimeTable[idx] = t; }

	i3Texture *	GetTexture(void)						{ return m_pTexture; }
	void		SetTexture( i3Texture * pTex);

	void		GetUV( RT_REAL32 tm, RT_VEC2D * pUV1, RT_VEC2D * pUV2);

	virtual		void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual		void	OnBuildObjectList( i3List * pList);
	virtual		UINT32	OnSave( i3ResourceFile * pResFile);
	virtual		UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif

