#if !defined( __I3_RASTERIZE_CHAIN_NIGHTVISION_H__)
#define __I3_RASTERIZE_CHAIN_NIGHTVISION_H__

#include "i3RasterizeChainBase.h"

#define NIGHT_VISION_COUNT	2

class I3_EXPORT_GFX i3RasterizeChain_NightVision : public i3RasterizeChainBase
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChain_NightVision, i3RasterizeChainBase);

protected:
	i3VertexArray *				m_pVA = nullptr;
	i3Shader *					m_pShader = nullptr;
	
	REAL32						m_fNVMixRate = 0.0f;
	
	I3G_TEXTURE_BIND_CONTEXT	m_TexCtx;

	UINT32						m_nMaskType = 0;

	struct MaskingTextureInfo {
		i3::string		_strFileName;
		i3Texture *		_pTexture = nullptr;
	};
	i3::vector<MaskingTextureInfo*>		m_vMaskingTextureList;

protected:
	void						CreateVolatileResource(void);
	void						DestroyVolatileResource(void);

public:
	virtual ~i3RasterizeChain_NightVision(void);

	void						AddMaskingTexture( const char * pszFileName);

	void						SetMaskingType( UINT32 idx)					{	I3ASSERT( m_nMaskType < m_vMaskingTextureList.size()); m_nMaskType = idx; }
	void						SetMixRate( REAL32 rate)					{	m_fNVMixRate = MINMAX( 0.0f, rate, 1.0f);	}	

	virtual INT32				GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) override;
	virtual void				ApplyChain( I3RC_UPDATE * pInfo) override;
	virtual void				OnCreate( i3RasterizeChainManager * pManager) override;
};

#endif	//#define __I3_RASTERIZE_CHAIN_NIGHTVISION_H__