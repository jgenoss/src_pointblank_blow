#if !defined( __I3_RASTERIZE_CHAIN_H)
#define __I3_RASTERIZE_CHAIN_H

#if defined( I3G_DX)
#include "i3GfxResource.h"
#include "i3TextureDX.h"
//#include "i3RasterizeChainContext.h"
#endif

class i3RasterizeChainContext;
class i3RasterizeChainManager;
class i3VertexArray;

#define DEBUG_TEXTURE_COUNT	12

#define	CHAIN_TARGET_COLOR				0x00000001
#define CHAIN_TARGET_DEPTH				0x00000002
#define	CHAIN_TARGET_STENCIL			0x00000004

struct CHAIN_RT_STATE
{
	I3G_IMAGE_FORMAT			Format = I3G_IMAGE_FORMAT_NONE;
	UINT32						nWidth =0;
	UINT32						nHeight = 0;
	INT32						nIdx = 0;	
	I3G_TEXTURE_BIND_CONTEXT	BindCtx;
};

struct I3RC_UPDATE
{
	REAL32						m_time = 0.0f;
	i3RenderContext *			m_pGfx = nullptr;
	i3RasterizeChainContext *	m_pLastRT = nullptr;
};

struct I3_CHAIN_INPUT_SPEC
{
	I3G_IMAGE_FORMAT		m_Format = I3G_IMAGE_FORMAT_NONE;
	INT32					m_Width = 0;
	INT32					m_Height = 0;
	char					m_szName[64] = { 0 };


	I3_CHAIN_INPUT_SPEC(void)
	{
		setToDefault();
	}

	void	setToDefault(void)
	{
		m_Format	= I3G_IMAGE_FORMAT_NONE;
		m_Width		= -1;			// Use default
		m_Height	= -1;			// Use default
		m_szName[0] = 0;
	}
};

// Render Target Format
#define CHAIN_RT_TYPE_COLOR_DEFAULT	0x00000000	// None
#define CHAIN_RT_TYPE_BGRA_8888		0x00000001	// LDR
#define CHAIN_RT_TYPE_BGRX_8888		0x00000002	// LDR
#define CHAIN_RT_TYPE_RGBX_8888		0x00000004	// LDR
#define CHAIN_RT_TYPE_RGBA_8888		0x00000008	// LDR
#define CHAIN_RT_TYPE_ABGR_16F		0x00000010	// HDR
#define CHAIN_RT_TYPE_ABGR_32F		0x00000020	// HDR
#define CHAIN_RT_TYPE_GR_16F		0x00000040	// HDR
#define CHAIN_RT_TYPE_R_32F			0x00000080	// HDR
#define CHAIN_RT_TYPE_GR_32F		0x00000100	// HDR

// Deapth Stencil Format
#define CHAIN_RT_TYPE_D16			0x00000200
#define CHAIN_RT_TYPE_D24S8			0x00000400
#define CHAIN_RT_TYPE_D24X8			0x00000800
#define CHAIN_RT_TYPE_D32			0x00001000
#define CHAIN_RT_TYPE_D32F			0x00002000
#define CHAIN_RT_TYPE_D24FS8		0x00004000

class I3_EXPORT_GFX i3RasterizeChainBase : public i3GfxResource
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3RasterizeChainBase, i3GfxResource);

protected:
	bool							m_bEnable = false;
	bool							m_bCreated = false;
	UINT32							m_TargetMask = CHAIN_TARGET_COLOR | CHAIN_TARGET_DEPTH;

	i3RasterizeChainContext *		m_pBindRT = nullptr;

public:
	i3RasterizeChainBase(void);

	bool						isCreated(void)							{ return m_bCreated; }
	void						setCreated(void)						{ m_bCreated = true; }

	virtual void				SetEnable(bool bFlag);
	bool						getEnable(void)							{ return m_bEnable;											}

	UINT32						getTargetType(void)						{ return m_TargetMask; }
	bool						isColorOut(void)						{ return (m_TargetMask & CHAIN_TARGET_COLOR) != 0; }
	bool						isDepthOut(void)						{ return (m_TargetMask & CHAIN_TARGET_DEPTH) != 0; }
	bool						isStencilOut(void)						{ return (m_TargetMask & CHAIN_TARGET_STENCIL) != 0; }

	i3RasterizeChainContext *	getTarget(void)							{ return m_pBindRT; }
	void						setTarget( i3RasterizeChainContext * pRT)
	{
		// 이 함수는 Manager에서 호출된다.
		m_pBindRT = pRT;
	}

	void						Draw( i3VertexArray * pVA = nullptr);
	
	virtual bool				BeginChain( i3RenderContext * pCtx);
	virtual void				EndChain( i3RenderContext * pCtx);

	static i3RasterizeChainManager* getManager(void);

	virtual INT32				GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) = 0;
	virtual void				ApplyChain( I3RC_UPDATE * pInfo) = 0;
	virtual void				OnCreate( i3RasterizeChainManager * pManager);
	virtual void				OnSize( i3RenderContext * pCtx);
};

#endif	// __I3_RASTERIZE_CHAIN_H
