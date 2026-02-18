#if !defined( __I3_OPT_CONVERT_DXT_TEXTURE_H)
#define __I3_OPT_CONVERT_DXT_TEXTURE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptConvertDXTTexture : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptConvertDXTTexture);
public:
	enum QUALITY
	{
		QUALITY_FASTEST = 0,
		QUALITY_NORMAL,
		QUALITY_HIGHEST
	};

protected:
	QUALITY				m_Quality;
	bool				m_bBinaryAlpha;
	UINT8				m_AlphaRef;
	bool				m_bDither;
	I3G_IMAGE_FORMAT	m_TargetFormat;

	static int	_AfterGenProc( void * pData, size_t count, const void * pMip, void * pUserData);

	I3G_IMAGE_FORMAT	_findProperFormat( i3TextureBindAttr * pTexAttr);
public:
	i3OptConvertDXTTexture(void);
	virtual ~i3OptConvertDXTTexture(void);

	QUALITY				getQuality(void)					{ return m_Quality; }
	void				setQuality( QUALITY qual)			{ m_Quality = qual; }

	bool				getBinaryAlphaEnable(void)			{ return m_bBinaryAlpha; }
	void				setBinaryAlphaEnable( bool bFlag)	{ m_bBinaryAlpha = bFlag; }

	UINT8				getAlphaRef(void)					{ return m_AlphaRef; }
	void				setAlphaRef( UINT8 ref)				{ m_AlphaRef = ref; }

	bool				getDitherEnable(void)				{ return m_bDither; }
	void				setDitherEnable( bool bFlag)		{ m_bDither = bFlag; }

	I3G_IMAGE_FORMAT	getTargetImageFormat(void)			{ return m_TargetFormat; }
	void				setTargetImageFormat( I3G_IMAGE_FORMAT fmt)
	{
		m_TargetFormat = fmt;
	}

	i3Texture *			MakeToDXT( i3Texture * pSrc);
	i3Texture *			MakeFromDXT( i3Texture * pSrc);

	virtual void		Trace( i3Node * pRoot);
	virtual void		SetProperty( char * pszFieldName, char * pszValue);

	virtual const char *	getDesc(void);
};

#endif
