#if !defined( __I3G_GFX_OPTION_H)
#define __I3G_GFX_OPTION_H

#include "i3GfxDefine.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace i3
{
	namespace pack
	{
		struct GFX_OPTION_INFO
		{
			UINT8			m_ID[4] = { 'G', 'F', 'X', 'O' };
			UINT16			m_Version[2] = { 1, 0 };
			INT32			m_bFullScreen = 0;
			INT32			m_bKeepPersist = 0;
			INT32			m_bNormalMapEnable = 0;
			INT32			m_bSpecularMapEnable = 0;
			INT32			m_bEnvironmentMapEnable = 0;
			INT32			m_bLuxMapEnable = 0;
			INT32			m_ShadowMapSize = 0;
			INT32			m_ShadowType = 0;
			INT32			m_TextureQuality = 0;
			UINT32			m_Format = 0;
			INT32			m_ForceMipmapFilter = 0;
			INT32			m_bEmissiveMapEnable = 0;
			UINT32			pad[509] = { 0 };
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_GFX i3GfxOption : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3GfxOption, i3PersistantElement);
protected:
	i3::pack::GFX_OPTION_INFO		m_Option;

	bool				m_bForceNormalMapEnable = true;
	bool				m_bForceSpecularMapEnable = true;
	bool				m_bForceReflectMapEnable = true;
	bool				m_bForceShadowMapEnable = true;
	bool				m_bForceEmissiveMapEnable = true;
	
public:
	i3GfxOption(void);

	bool				getFullScreenEnable(void)				{ return m_Option.m_bFullScreen != 0; }
	void				setFullScreenEnable( bool bState)		{ m_Option.m_bFullScreen = bState; }

	bool				getKeepPersistDataEnable(void)			{ return m_Option.m_bKeepPersist != 0; }
	void				setKeepPersistDataEnable( bool bState)	{ m_Option.m_bKeepPersist = bState; }

	bool				getNormalMapEnable(void)				{ return m_Option.m_bNormalMapEnable != 0; }
	void				setNormalMapEnable( bool bState)		{ m_Option.m_bNormalMapEnable = m_bForceNormalMapEnable && bState; }

	void				setForceNormalMapEnable( bool bState)	{ m_bForceNormalMapEnable = bState; }

	bool				getSpecularMapEnable(void)				{ return m_Option.m_bSpecularMapEnable != 0; }
	void				setSpecularMapEnable( bool bState)		{ m_Option.m_bSpecularMapEnable = m_bForceSpecularMapEnable && bState; }

	void				setForceSpecularMapEnable( bool bState)	{ m_bForceSpecularMapEnable = bState; }

	bool				getEnvironmentMapEnable(void)			{ return m_Option.m_bEnvironmentMapEnable != 0;	}
	void				setEnvironmentMapEnable(bool bState)	{ m_Option.m_bEnvironmentMapEnable = m_bForceReflectMapEnable && bState;	}

	void				setForceEnvironmentMapEnable( bool bState)	{ m_bForceReflectMapEnable = bState; }

	bool				getLuxMapEnable(void)					{ return m_Option.m_bLuxMapEnable != 0; }
	void				setLuxMapEnable( bool bState)			{ m_Option.m_bLuxMapEnable = bState; }

	bool				getEmissiveMapEnable(void)				{ return m_Option.m_bEmissiveMapEnable != 0;	}
	void				setEmissiveMapEnable(bool bState)		{ m_Option.m_bEmissiveMapEnable = m_bForceEmissiveMapEnable && bState;	}

	void				setForceEmissiveMapEnable( bool bState)	{ m_bForceEmissiveMapEnable = bState; }

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

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif