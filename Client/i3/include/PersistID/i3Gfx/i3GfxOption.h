#if !defined( __I3G_GFX_OPTION_H)
#define __I3G_GFX_OPTION_H

#include "i3GfxDefine.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

typedef struct _tagi3GfxOptionInfo
{
	UINT8			m_ID[4];
	UINT16			m_Version[2];
	INT32			m_bFullScreen;
	INT32			m_bKeepPersist;
	INT32			m_bNormalMapEnable;
	INT32			m_bSpecularMapEnable;
	INT32			m_bEnvironmentMapEnable;
	INT32			m_bLuxMapEnable;
	INT32			m_ShadowMapSize;
	INT32			m_ShadowType;
	INT32			m_TextureQuality;
	UINT32			m_Format;
	INT32			m_ForceMipmapFilter;
	UINT32			pad[510];
} I3G_OPTION_INFO;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_GFX i3GfxOption : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3GfxOption);
protected:
	I3G_OPTION_INFO		m_Option;

	bool				m_bForceNormalMapEnable;
	bool				m_bForceSpecularMapEnable;
	bool				m_bForceReflectMapEnable;
	bool				m_bForceShadowMapEnable;
	
public:
	i3GfxOption(void);
	virtual ~i3GfxOption(void);

	bool				getFullScreenEnable(void)				{ return m_Option.m_bFullScreen != FALSE; }
	void				setFullScreenEnable( bool bState)		{ m_Option.m_bFullScreen = bState; }

	bool				getKeepPersistDataEnable(void)			{ return m_Option.m_bKeepPersist != FALSE; }
	void				setKeepPersistDataEnable( bool bState)	{ m_Option.m_bKeepPersist = bState; }

	bool				getNormalMapEnable(void)				{ return m_Option.m_bNormalMapEnable != FALSE; }
	void				setNormalMapEnable( bool bState)		{ m_Option.m_bNormalMapEnable = m_bForceNormalMapEnable && bState; }

	void				setForceNormalMapEnable( bool bState)	{ m_bForceNormalMapEnable = bState; }

	bool				getSpecularMapEnable(void)				{ return m_Option.m_bSpecularMapEnable != FALSE; }
	void				setSpecularMapEnable( bool bState)		{ m_Option.m_bSpecularMapEnable = m_bForceSpecularMapEnable && bState; }

	void				setForceSpecularMapEnable( bool bState)	{ m_bForceSpecularMapEnable = bState; }

	bool				getEnvironmentMapEnable(void)			{ return m_Option.m_bEnvironmentMapEnable != FALSE;	}
	void				setEnvironmentMapEnable(bool bState)	{ m_Option.m_bEnvironmentMapEnable = m_bForceReflectMapEnable && bState;	}

	void				setForceEnvironmentMapEnable( bool bState)	{ m_bForceReflectMapEnable = bState; }

	bool				getLuxMapEnable(void)					{ return m_Option.m_bLuxMapEnable != FALSE; }
	void				setLuxMapEnable( bool bState)			{ m_Option.m_bLuxMapEnable = bState; }

	INT32				getShadowMapSize(void)					{ return m_Option.m_ShadowMapSize; }
	void				setShadowMapSize( INT32 Sz)				{ m_Option.m_ShadowMapSize = Sz; }

	INT32				getShadowType(void)						{ return m_Option.m_ShadowType; }
	void				setShadowType( INT32 type)				{ m_Option.m_ShadowType = type; }

	void				setForceShadowMapEnable( bool bState)	{ m_bForceShadowMapEnable = bState; }
	bool				getForceShadowMapEnable(void)			{ return m_bForceShadowMapEnable; }

	INT32				getTextureQuality(void)					{ return m_Option.m_TextureQuality;	}
	void				setTextureQuality( INT32 type)			{ m_Option.m_TextureQuality = type;	}

	I3G_IMAGE_FORMAT	getDisplayFormat(void)					{ return (I3G_IMAGE_FORMAT) m_Option.m_Format; }
	void				setDisplayFormat( I3G_IMAGE_FORMAT fmt)	{ m_Option.m_Format = (I3G_IMAGE_FORMAT) fmt; }

	I3G_TEXTURE_FILTER	getForcedMipmapFilter(void)				{ return (I3G_TEXTURE_FILTER) m_Option.m_ForceMipmapFilter; }
	void				setForcedMipmapFilter(I3G_TEXTURE_FILTER filter)
	{ 
		m_Option.m_ForceMipmapFilter = filter;
	}

	UINT32				Save( const char * pszPath);
	UINT32				Save( i3Stream * pStream);

	UINT32				Load( const char * pszPath);
	UINT32				Load( i3Stream * pStream);

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif