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
	I3_EXPORT_CLASS_DEFINE( i3TextureSequence, i3PersistantElement);
protected:
	// Persistent Members
	INT32		m_Style = I3G_TSEQ_STYLE_STATICTIME;
	REAL32	m_Width = 0.0f;
	REAL32	m_Height = 0.0f;
	REAL32	m_Interval = 0.016666666666666666666666666666667f;	// 60 FPS;
	UINT32		m_Count = 0;
	UINT32		m_StartIndex = 0;
	REAL32	*	m_pTimeTable = nullptr;
	i3Texture * m_pTexture = nullptr;
	
	
	// Volatile Members
	INT32		m_LineCount = 0;
	REAL32	m_LineTime = 0.0f;
	REAL32	m_RLineTime = 1.0f;
	REAL32	m_RInterval = 0.0f;
	REAL32	m_WidthFactor = 1.0f, m_HeightFactor = 1.0f;
	REAL32	m_Duration = 0.0f;

protected:
	void		CalcParams(void);

public:
	virtual ~i3TextureSequence(void);

	bool		GetUseStaticTime(void)					{ return (m_Style & I3G_TSEQ_STYLE_STATICTIME) != 0; }
	void		SetUseStaticTime( bool bFlag);

	bool		IsRepeat(void)							{ return (m_Style & I3G_TSEQ_STYLE_REPEAT) != 0; }
	void		SetRepeat( bool bFlag)
	{
		if( bFlag)
			m_Style |= I3G_TSEQ_STYLE_REPEAT;
		else
			m_Style &= ~I3G_TSEQ_STYLE_REPEAT;
	}

	bool		IsRandomMode(void)						{ return (m_Style & I3G_TSEQ_STYLE_RANDOM) != 0; }
	void		SetRandomMode( bool bFlag)
	{
		if( bFlag)
			m_Style |= I3G_TSEQ_STYLE_RANDOM;
		else
			m_Style &= ~I3G_TSEQ_STYLE_RANDOM;
	}

	bool		IsFixedFrameMode(void)					{ return (m_Style & I3G_TSEQ_STYLE_FIXEDFRAME) != 0; }
	void		SetFixedFrameMode( bool bFlag)
	{
		if( bFlag)
			m_Style |= I3G_TSEQ_STYLE_FIXEDFRAME;
		else
			m_Style &= ~I3G_TSEQ_STYLE_FIXEDFRAME;
	}

	REAL32		GetFrameWidth(void)						{ return m_Width; }
	REAL32		GetFrameHeight(void)					{ return m_Height; }
	void		SetFrameSize( REAL32 w, REAL32 h)
	{
		m_Width = w;
		m_Height = h;

		CalcParams();
	}

	REAL32		GetStaticInterval(void)					{ return m_Interval; }
	void		SetStaticInterval( REAL32 val)
	{ 
		m_Interval = val;

		CalcParams();
	}

	REAL32		GetDuration(void)						{ return m_Duration; }
	void		SetDuration( REAL32 dur)
	{ 
		m_Duration = dur;
		CalcParams();
	}

	UINT32		GetCount(void)							{ return m_Count; }
	void		SetCount( UINT32 c);

	UINT32		GetStartIndex(void)						{ return m_StartIndex; }
	void		SetStartIndex( UINT32 idx)				{ m_StartIndex = idx; }

	REAL32		GetTime( UINT32 idx)					{ return m_pTimeTable[idx]; }
	void		SetTime( UINT32 idx, REAL32 t)			{ m_pTimeTable[idx] = t; }

	i3Texture *	GetTexture(void)						{ return m_pTexture; }
	void		SetTexture( i3Texture * pTex);

	void		GetUV( REAL32 tm, VEC2D * pUV1, VEC2D * pUV2);

	virtual		void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual		void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual		UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual		UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual		bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual		bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif

