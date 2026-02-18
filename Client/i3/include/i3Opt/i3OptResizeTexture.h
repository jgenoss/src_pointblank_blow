#if !defined( __I3_OPT_RESIZE_TEXTURE_H)
#define __I3_OPT_RESIZE_TEXTURE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptResizeTexture : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptResizeTexture, i3SceneOptimizer);
public:
	enum TYPE
	{
		TYPE_SCALE	= 0,			// 원본 크기를 일정 비율로 줄인다.
		TYPE_LIMIT,					// 지정된 상,하한 크기로 맞춘다.
	};

protected:
	TYPE		m_Type = TYPE_SCALE;
	INT32		m_UpperWidth = 512;
	INT32		m_UpperHeight = 512;
	INT32		m_LowerWidth = 16;
	INT32		m_LowerHeight = 16;
	REAL32		m_fScale = 0.5f;

protected:
	void			_GetAvgPixel( i3Texture * pTex, INT32 x, INT32 y, INT32 cx, INT32 cy, COLORREAL * pCol);

public:
	i3OptResizeTexture(void);

	TYPE			getType(void)				{ return m_Type; }
	void			setType( TYPE type)			{ m_Type = type; }

	UINT32			getUpperLimitWidth(void)	{ return m_UpperWidth; }
	UINT32			getUpperLimitHeight(void)	{ return m_UpperHeight; }
	UINT32			getLowerLimitWidth(void)	{ return m_LowerWidth; }
	UINT32			getLowerLimitHeight(void)	{ return m_LowerHeight; }

	void			setLimit( UINT32 uw, UINT32 uh, UINT32 lw, UINT32 lh)
	{
		m_UpperWidth = uw;
		m_UpperHeight = uh;
		m_LowerWidth = lw;
		m_LowerHeight = lh;
	}

	REAL32			getScaleFactor(void)			{ return m_fScale; }
	void			setScaleFactor( REAL32 fVal)	{ m_fScale = fVal; }

	i3Texture *		Resize( i3Texture * pSrcTex, INT32 targetW, INT32 targetH);

	void			Scale( i3Texture * pSrc, i3Texture * pDest);

	virtual void	Trace( i3Node * pRoot) override;
	virtual void	SetProperty( char * pszFieldName, char * pszValue) override;
};

#endif
