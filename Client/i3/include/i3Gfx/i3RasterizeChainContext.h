#if !defined( __I3_RASTERIZE_CHAIN_CONTEXT_H)
#define __I3_RASTERIZE_CHAIN_CONTEXT_H

#define CTS_DEFAULT_WIDTH			0x00000001
#define CTS_DEFAULT_HEIGHT			0x00000002
#define CTS_DEFAULT_FORMAT			0x00000004
#define CTS_DEFAULT_SCALED			0x00000008

class i3Texture;
class i3RenderTarget;
class i3ShaderParam;
class i3VertexArray;

class I3_EXPORT_GFX i3RasterizeChainContext : public i3NamedElement				// 익스포트 추가.. i3Framework의 i3Viewer.cpp에서 쓰이고 있음.(2012.08.30.수빈)
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChainContext, i3NamedElement);

protected:			
	i3Texture * 				m_pColorTarget = nullptr;
	i3Texture *					m_pDepthTarget = nullptr;
	
	I3G_TEXTURE_BIND_CONTEXT	m_BindCtx;
	
	bool						m_bUse = false;
	bool						m_bOwnDepth = false;

	UINT32						m_ChainStyle = 0;

	INT32						m_nDownScaleRate = 1;		// 1 / m_nScaleRate

public:
	i3RasterizeChainContext(void);
	virtual ~i3RasterizeChainContext(void);

	// Style관련
	UINT32						getStyle(void)							{ return m_ChainStyle; }
	void						setStyle(UINT32 val)					{ m_ChainStyle = val; }
	bool						isStyle(UINT32 val)						{ return (m_ChainStyle & val) != 0; }
	void						addStyle(UINT32 val)					{ m_ChainStyle |= val; }
	void						removeStyle(UINT32 val)					{ m_ChainStyle &= ~val; }

	void						setDownScaleRate( INT32 nRate)			{	m_nDownScaleRate = nRate;}
	INT32						getDownScaleRate(void)					{	return m_nDownScaleRate;}
	bool						hasDepthTarget(void)						{ return m_bOwnDepth; }

	// Texture생성
	bool						Create( UINT32 width, UINT32 height, UINT32 levels, I3G_IMAGE_FORMAT format, bool bDepth = true);
	void						Destroy(void);
	void						MakeMultiSampledTarget(bool bForceDepth = false);
	void						MakeDepthTargetIfNULL( i3Texture * pDepth);

	i3Texture *					getColorTarget(void)						{ return m_pColorTarget;														}
	i3Texture *					getDepthTarget(void)						{ return m_pDepthTarget; }

	UINT32						getWidth(void)								{ return m_pColorTarget->GetWidth();											}
	UINT32						getHeight(void)								{ return m_pColorTarget->GetHeight();											}
	I3G_IMAGE_FORMAT			getFormat(void)								{ return m_pColorTarget->GetFormat();											}

	void						setBindCtx(I3G_TEXTURE_BIND_CONTEXT* pBind)	{ i3mem::Copy( &m_BindCtx, pBind, sizeof(I3G_TEXTURE_BIND_CONTEXT));		}
	I3G_TEXTURE_BIND_CONTEXT*	getBindCtx(void)							{ return &m_BindCtx;														}

	bool						isUsed(void)								{ return m_bUse;															}
	void						setLock(bool bFlag)							{ m_bUse = bFlag;															}

	bool						isNamedContext(void)						{ return (GetName()[0] != 0);										}

	void						UpdateShaderParam( i3ShaderParam * pParam);

	void						Draw(i3VertexArray* pArray);

	void						MakeCurrentTarget( INT32 idx = 0);

	void						SetSamplerUVWrapEnable( bool bEnable = true);
	void						setFilter( I3G_TEXTURE_FILTER minFilter, I3G_TEXTURE_FILTER magFilter);
};

#endif	// __I3_RASTERIZE_CHAIN_CONTEXT_H
