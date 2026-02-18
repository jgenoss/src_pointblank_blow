#if !defined( __I3_OPT_MIPMAP_GEN_H)
#define __I3_OPT_MIPMAP_GEN_H

#include "i3SceneOptimizer.h"

struct TEX_REF_INFO
{
	i3Texture *		m_pTex = nullptr;
	i3Texture *		m_pNewTex = nullptr;
} ;

class I3_EXPORT_OPT i3OptMipmapGen : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptMipmapGen, i3SceneOptimizer);
public:
	enum FILTER
	{
		FILTER_D3D_POINT			= 0,
		FILTER_D3D_LINEAR			= 1,
		FILTER_D3D_ANISOTROPIC		= 2,
		FILTER_D3D_PYRAMIDALQUAD	= 3,
		FILTER_D3D_GAUSSIANQUAD		= 4,

		FILTER_D3DX_POINT			= 5,
		FILTER_D3DX_LINEAR			= 6,
		FILTER_D3DX_TRIANGLE		= 7,
		FILTER_D3DX_BOX				= 8,

		FILTER_COUNT
	};

protected:
	i3::vector<TEX_REF_INFO*>	m_TexBindList;

	bool			m_bLimitSize = true;
	INT32			m_MinCX = 32;
	INT32			m_MinCY = 32;

	bool			m_bLimitLevel = true;
	INT32			m_MaxLevel = 4;

	FILTER			m_Filter = FILTER_D3DX_TRIANGLE;
	bool			m_bDither = false;
	bool			m_bSRGBIn = false;
	bool			m_bSRGBOut = false;
	bool			m_bMirrorU = true;
	bool			m_bMirrorV = true;
	bool			m_bMirrorW = true;

protected:
	void			_RemoveAllTexBindInfo(void);
	INT32			_FindTexRef( i3Texture * pTex);

	i3Texture *		_GenD3DMipmap( i3Texture * pSrc, UINT32 filter, INT32 maxLevel);
	i3Texture *		_GenD3DXMipmap( i3Texture * pSrc, UINT32 filter, INT32 maxLevel);

public:
	i3OptMipmapGen(void);

	bool			getLimitSizeState(void)			{ return m_bLimitSize; }
	void			setLimitSizeState( bool bState)	{ m_bLimitSize = bState; }

	INT32			getMinWidth(void)				{ return m_MinCX; }
	void			setMinWidth( INT32 w)			{ m_MinCX = w; }

	INT32			getMinHeight(void)				{ return m_MinCY; }
	void			setMinHeight( INT32 h)			{ m_MinCY = h; }

	bool			getLimitLevelState(void)		{ return m_bLimitLevel; }
	void			setLimitLevelState( bool bState){ m_bLimitLevel = bState; }

	INT32			getLimitMaxLevel(void)				{ return m_MaxLevel; }
	void			setLimitMaxLevel( INT32 lv)			{ m_MaxLevel = lv; }

	FILTER			getFilter(void)					{ return m_Filter; }
	void			setFilter( FILTER filter)		{ m_Filter = filter; }

	INT32			getFilterCount(void)			{ return FILTER_COUNT; }
	const char *	getFilterName( INT32 idx);

	bool			getDitherState(void)			{ return m_bDither; }
	void			setDitherState( bool bState)	{ m_bDither = bState; }

	bool			getSRGBInState(void)			{ return m_bSRGBIn; }
	void			setSRGBInState( bool bState)	{ m_bSRGBIn = bState; }

	bool			getSRGBOutState(void)			{ return m_bSRGBOut; }
	void			setSRGBOutState( bool bState)	{ m_bSRGBOut = bState; }

	bool			getMirrorUState(void)			{ return m_bMirrorU; }
	void			setMirrorUState( bool bState)	{ m_bMirrorU = bState; }

	bool			getMirrorVState(void)			{ return m_bMirrorV; }
	void			setMirrorVState( bool bState)	{ m_bMirrorV = bState; }

	bool			getMirrorWState(void)			{ return m_bMirrorW; }
	void			setMirrorWState( bool bState)	{ m_bMirrorW = bState; }

	i3Texture *		GenMipmap( i3Texture * pSrcTex);

	virtual void		Trace( i3Node * pRoot) override;
	virtual void		SetProperty( char * pszFieldName, char * pszValue) override;

	virtual const char *	getDesc(void) override;
};

#endif
