#if !defined( __I3_RENDER_CONTEXT_H)
#define __I3_RENDER_CONTEXT_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3ElementBase.h"
#include "../i3Base/i3MetaTemplate.h"
#include "i3GfxType.h"
#include "i3GfxOption.h"
#include "i3RenderTarget.h"
#include "i3RenderState.h"
#include "i3VertexArray.h"
#include "i3IndexArray.h"
#include "i3VideoInfo.h"
#include "i3Texture.h"
#include "i3Light.h"
#include "i3GfxCaps.h"
#include "i3PerformanceReport.h"
#include "i3GPUCommandList.h"
#include "i3DisplayList.h"
#include "i3Shader.h"
#include "i3ShaderCache.h"

#if defined( I3G_DX)
	#include "i3ShaderCode.h"
	#include "i3ShaderContext.h"
	#include "i3RasterizeChainBase.h"
	#include "i3GfxFontDX.h"
	#include "i3GfxFontDX2.h"
	#include "i3GfxVertexBank.h"
#endif

#if defined(I3_DEBUG) && defined(I3G_PSP)
	typedef void (*DispListTestFunc)();
#endif

#define		MAX_BONE_MATRIX			256
#define		MAX_TEXTURE_STAGE		8
#define		MAX_LIGHT				2

	/*
typedef struct _tagi3GfxShadowMapInfo
{
	i3Texture *			m_pShadowMap;
	MATRIX				m_mView2Light;
	MATRIX				m_mNormalize;
	COLORREAL			m_ShadowColor;
	VEC4D				m_ShadowMapFactors;			// [ ShadowMapSize, 1 / ShadowMapSize, CutoffAngle]
	VEC3D				m_ShadowLightPos;
	VEC3D				m_ShadowLightDir;
	VEC4D				m_OffsetTable[32];
} I3G_SHADOWMAP_INFO;
*/

typedef void	(* WAITVSYNCPROC)( i3RenderContext * pCtx, void * pUserData);
typedef void	(* ADJUSTGFXCONFIGPROC)( i3VideoInfo * pVideoInfo);

typedef struct	_tagWaitVSyncInfo
{
	WAITVSYNCPROC		m_pProc;
	void *				m_pUserData;
} WAIT_VSYNC_INFO;

/// 그래픽 하드웨어를 제어하고 렌더링을 수행한다.
class I3_EXPORT_GFX i3RenderContext : public i3ElementBase
{
	I3_CLASS_DEFINE( i3RenderContext);

	friend class i3ShaderContext;
	friend class i3ShaderParam;

protected:
	void *	m_pSetupParam;
	i3Timer *			m_pTimer;
	i3RenderTarget *	m_pRenderTarget;
	
	i3Texture *			m_pRasterizeChain1;	
	i3Texture *			m_pRasterizeChain2;
	i3Texture *			m_pRasterizeChainDepth;
	

	i3VertexArray *		m_pCurrentVA;
	i3IndexArray *		m_pCurrentIA;

	BOOL				m_bWindowed;
	BOOL				m_bReady;
	BOOL				m_bSkipBlendModeAttr;

	i3GfxOption			g_GfxOption;

	I3G_CLEAR			m_ClearFlag;
	COLOR				m_BackColor;
	
	i3List				m_RenderTargetList;
	i3List				m_RasterizeChainList;					// Rasterize Chain이 들어가 있음

	i3List				m_WaitProcList;

	REAL32				m_ClearZValue;
	UINT8				m_ClearStencilValue;

	// Shader가 실패된 것들에 대한 정보가 들어가 있습니다.
	i3List				m_VSFailedList;
	i3List				m_PSFailedList;

	INT32				m_VertexBlendWeightCount;

	INT32						m_UsedClutCount;

	static i3GfxCaps *			m_pCaps;

	i3GfxPerformanceReport *	m_pPerfReport;
	UINT32						m_TriangleCount;
	UINT32						m_DrawCallCount;
	REAL32						m_secFrameTime;
	UINT32						m_FrameCountPerSec;
	REAL32						m_DeltaSec;

	BOOL						m_bConstantPointSpriteSizeEnable;
	REAL32						m_fConstantPointSpriteSize;
	I3G_POINT_SPRITE_SIZE_SPACE	m_PointSpriteSizeSpace;

	RT_MATRIX					m_LastViewMatrix;
	RT_MATRIX					m_LastProjectMatrix;
	RT_MATRIX					m_LastWorldMatrix;
	RT_MATRIX					m_LastTextureMatrix[ MAX_TEXTURE_STAGE];
	RT_MATRIX					m_BoneMatrixArray[MAX_BONE_MATRIX];
	INT32						m_BoneMatrixCount;

	i3BinList					m_AffectLightList;
	i3List *					m_pLightList;

	//I3G_SHADOWMAP_INFO			m_ShadowMapInfo;

	COLORREAL					m_Color;
	COLORREAL					m_Ambient;
	COLORREAL					m_MaterialDiffuse;
	COLORREAL					m_MaterialSpecular;
	COLORREAL					m_MaterialEmissive;
	REAL32						m_MaterialShininess;

	bool						m_bFresnelEnable;
	REAL32						m_FresnelIOR;
	COLORREAL					m_FresnelColor;

#ifndef I3G_OGLES

#endif

	I3G_TEXTURE_BIND_CONTEXT 	m_TexBindInfo[ I3G_TEXTURE_BIND_MAX];

	INT32						m_UsedTextureStageCount;

	i3Shader *					m_pCurShader;
	i3ShaderCache *				m_pShaderCache;
	char						m_szShaderCachePath[MAX_PATH];

	i3VideoInfo					m_CurrentVideoInfo;

	i3ShaderCode				m_ShaderCode;
	i3ShaderCode				m_OldShaderCode;

	I3G_SHADE_MODE				m_ShadeMode;
	I3G_SHADE_MODE				m_OldShadeMode;
	// 강제적으로 Shade Mode를 지정합니다.
	I3G_SHADE_MODE				m_ForceShadeMode;
	bool						m_bTextureModified;

#if defined( I3G_DX)
	IDirect3D9 *				m_pD3D;
	IDirect3DDevice9 *			m_pD3DDevice;
	IDirect3DSwapChain9 *		m_pSwapChain;	
	IDirect3DSurface9 *			m_pOriginalD3DSurface;
	IDirect3DSurface9 *			m_pOriginalD3DDeapthSurface;
	IDirect3DSurface9 *			m_pDefaultD3DSurface;
	IDirect3DSurface9 *			m_pDefaultD3DDeapthSurface;
	HWND						m_hWnd;
	D3DPRESENT_PARAMETERS		m_d3dpp;
	BOOL						m_bShadowMapEnable;
	BOOL						m_bTextureEnable;
	BOOL						m_bLightingEnable;
	BOOL						m_bVertexColorEnable;
	BOOL						m_bPPLEnable;
	UINT32						m_TexOpMask;
	UINT32						m_OldTexOpMask;
	UINT32						m_TraceOpMask;

	BOOL						m_bAlphaTestEnable;
	BOOL						m_bBlendEnable;
	I3G_BLEND					m_SrcBlend;
	I3G_BLEND					m_DestBlend;
	I3G_BLEND_OP				m_BlendOp;
	BOOL						m_bZTestEnable;
	BOOL						m_bZWriteEnable;

	VEC3D						m_TextureCoordScale;
	VEC3D						m_TextureCoordOffset;

	RECT*						m_pPresentRect;
	BOOL						m_bSkipRender;
	BOOL						m_bShaderEnable;

	BOOL						m_bEnableCompressedTexture;
	BOOL						m_bRivived;

	i3GfxFontDXManager *		m_pFontDxManager;
	i3GfxFontDX2Manager *		m_pFontDx2Manager;

	i3GfxVertexBank *			m_pVertexBank;

	D3DRECT*					m_pClearRectArray;
	INT32						m_nClearRectCount;		

	RECT						m_curViewportRect;

	i3List						m_HideWndList;

#elif defined( I3G_XBOX)
	LPDIRECT3D8             m_pD3D       ; // Used to create the D3DDevice
	LPDIRECT3DDEVICE8       m_pD3DDevice ; // Our rendering device
	D3DPRESENT_PARAMETERS	m_d3dpp;

#elif defined( I3G_PSP) || defined( I3G_OGLES)
	
	#ifdef I3G_PSP
	I3G_TEXTURE_COORD_SOURCE	m_TextureCoordSource;
	#endif

	#if defined( I3G_OGLES)
		EGLDisplay						m_LCD;
		#if defined( I3G_WIPI2D)
			MC_GrpContext					m_grpContext;
		#endif
	#endif

	RT_REAL32					m_FogNear;
	RT_REAL32					m_FogFar;

	BOOL						m_bAlphaTestEnable;
	BOOL						m_bBlendEnable;
	UINT8						m_AlphaRef;
	I3G_COMP_FUNC				m_AlphaFunc;
	BOOL						m_bZTestEnable;
	BOOL						m_bZWriteEnable;
	I3G_COMP_FUNC				m_ZFunc;
	I3G_FACE_CULL				m_FaceCull;
	I3G_BLEND					m_SrcBlend;
	I3G_BLEND					m_DestBlend;
	I3G_BLEND_OP				m_BlendOp;
	int							m_SrcBlendMask;
	int							m_DestBlendMask;
	BOOL						m_bFogEnable;
	COLOR						m_FogColor;
	BOOL						m_bAALine;
	BOOL						m_bStencilEnable;
	I3G_STENCIL_OP				m_StencilSFail;
	I3G_STENCIL_OP				m_StencilSPass;
	I3G_STENCIL_OP				m_StencilZFail;
	I3G_COMP_FUNC				m_StencilFunc;
	UINT8						m_StencilRef;
	UINT8						m_StencilMask;
	BOOL						m_bClipping;
	COLOR						m_AmbientColor;
	COLOR						m_DiffuseColor;
	COLOR						m_SpecularColor;
	COLOR						m_EmissiveColor;
	COLOR						m_Color;
	I3G_TEXTURE_FILTER			m_TexMinFilter;
	I3G_TEXTURE_FILTER			m_TexMagFilter;
	
	BOOL						m_bTextureEnable;
	BOOL						m_bLightingEnable;
	BOOL						m_bNormalizeNormal;
	BOOL						m_bBCE;

	INT32						m_TexFunc;
	INT32						m_OldTexFunc;

	BOOL						m_bProjectiveTexture;

	BOOL						m_bFullFrameTimeRendering;
	i3Timer *					m_pFPSTimer;
	BOOL						m_bFPS30;
#elif defined( I3G_PS2)
#endif

#ifdef I3_DEBUG
	BOOL						m_bShowFPS;
#endif


#if defined( I3G_DX) || defined( I3G_XBOX)
protected:

	bool		FindAppropriateVideoInfo( i3VideoInfo * pWantInfo, i3VideoInfo * pFindInfo);
	void		GetCurrentVideoInfo( i3VideoInfo * pInfo);
	BOOL		FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
				D3DFORMAT Format, UINT32 DepthBits, UINT32 StencilBits, D3DFORMAT* pDepthStencilFormat );
	BOOL		IsCompressedTextureFormatOk( D3DFORMAT AdapterFormat );

	void		MinimizeOtherWindows(void);
	void		RestoreOtherWindows(void);

	bool		_CreateDevice(void);
	void		_InvalidateGfxResource( bool bLostDevice);
	void		_ValidateGfxResource(void);
	INT32		_GetMonitorFromWindow( HWND hwnd);
	void		UpdateVideoInfo(void);
	void		UpdateRenderTarget(void);
	BOOL		CreateRasterizeChain(void);
	void		OnInit(void);

	void		_SetCurrentViewportRect( INT32 x, INT32 y, INT32 width, INT32 height);

	inline	void UpdateTextureStageState( BOOL bIsVertexColor);
public:
	void *	GetNativeDevice(void)										{ return m_pD3DDevice; }
	void	SetPresentRect( RECT* rt)									{ m_pPresentRect = rt; }
	void	SetSkipRender( BOOL bRender)								{ m_bSkipRender = bRender;	}
	BOOL	GetSkipRender( void)										{ return m_bSkipRender;	}
	BOOL	IsCompressedTexture( void)									{ return m_bEnableCompressedTexture; }
	void	SetPPLEnable( BOOL bFlag)									{ m_bPPLEnable = bFlag;		}
	BOOL	getPPLEnable()												{ return m_bPPLEnable;		}	
	i3GfxFontDXManager * getFontDxManager(void)							{ return m_pFontDxManager;	}
	i3GfxFontDX2Manager * getFontDx2Manager(void)						{ return m_pFontDx2Manager;	}
	i3GfxVertexBank * getVertexBank(void)								{ return m_pVertexBank;		}
	BOOL	isContextRivieved(void)										{ return m_bRivived;		}	
	void	setDefaultD3DSurface(IDirect3DSurface9 * pSurface)			{ m_pDefaultD3DSurface = pSurface;			}
	void	setDefaultD3DDeapthSurface(IDirect3DSurface9 * pSurface)	{ m_pDefaultD3DDeapthSurface = pSurface;	}	
	void	resetD3DSurface(void)										{ m_pDefaultD3DSurface = m_pOriginalD3DSurface;				}
	void	resetD3DDeapthSurface(void)									{ m_pDefaultD3DDeapthSurface = m_pOriginalD3DDeapthSurface;	}

#elif defined( I3G_PSP)

protected:
	inline	void UpdateTextureStageState( BOOL bIsVertexColor);
	BOOL	m_bDumpDispList;
	BOOL	m_bDumpDispListOnce;
public:
	void	SetHalfFPSEnable( BOOL bFlag)				{ m_bFPS30 = bFlag; }
	void	EndDispListRender();

#elif defined( I3G_PS2)
#elif defined( I3G_OGLES)
	void				SetMatrix( I3G_MATRIX_STACK Index, RT_MATRIX * pMatrix);
	void				GetMatrix( I3G_MATRIX_STACK Index, RT_MATRIX * pMatrix);
	void				SetCurrentRenderTarget( i3Texture * pTex);
	BOOL				SetRenderTargetSize( UINT32 cx, UINT32 cy);
	void				GetMaterialAmbient( COLORREAL * pColor);
	void				GetMaterialDiffuse( COLORREAL * pColor);
	void				GetMaterialSpecular( COLORREAL * pColor);
	void				GetMaterialEmissive( COLORREAL * pColor);
	void				SetVertexBlendMatrix( INT32 Idx, RT_MATRIX * pMatrix);
	void				SetTextureEnable( INT32 StageIdx, BOOL bState);
	void				SetTexture( INT32 StageIdx, i3Texture * pTexture);
	void				SetTextureFilter( INT32 StageIdx, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter);
	void				SetTextureWrap( INT32 StageIdx, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap);
	void				SetTextureFunction( INT32 StageIdx, I3G_TEXTURE_FUNCTION func);
	void				SetTextureMatrix( INT32 StageIdx, RT_MATRIX * pMatrix, INT32 CoordCount, BOOL bProjective);
	void				SetTextureCoordScale( INT32 StageIdx, VEC3D * pScale, INT32 CoordCount);
	void				SetTextureCoordSource( INT32 StageIdx, I3G_TEXTURE_COORD_SOURCE source);
	void				SetLight( i3Light * pLight);
	BOOL				Reset(void);
	void				SetTextureCoordOffset( INT32 StageIdx, VEC3D * pOffset, INT32 CoordCount);
	//BOOL				Create( void * pNativeRTHandle);

#endif

protected:
	bool				CreateDefaultShaderContext(void);

	void				_SetTexture( INT32 StageIdx, i3Texture * pTexture);

	void				_SetTextureWrap( INT32 StageIdx, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap);
	void				_SetTextureFilter( INT32 StageIdx, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter);
	void				_SetTextureCoordSource( INT32 StageIdx, I3G_TEXTURE_COORD_SOURCE source);
	void				_SetTextureFunction( INT32 StageIdx, I3G_TEXTURE_FUNCTION func);
	void				_SetTextureCoordScale( INT32 StageIdx, VEC3D * pScale, INT32 CoordCount);
	void				_SetTextureCoordOffset( INT32 StageIdx, VEC3D * pOffset, INT32 CoordCount);
	void				_SetTextureMatrix( INT32 StageIdx, RT_MATRIX * pMatrix, INT32 CoordCount = 2, BOOL bProjective = FALSE);
	void				_SetTextureBorderColor( INT32 StageIdx, UINT32 color);
	void				_SetMaxAnisotropy( INT32 idxStage, INT32 val);

	//void				_UpdateTextureBind(void);

	void				_FindShaderWorkDir( char * pszPath);
	bool				_LoadShaderCache( const char * pszPath);
	bool				_SaveShaderCache( const char * pszPath);
	
public:
	i3RenderContext(void) NOTHROW;
	virtual ~i3RenderContext(void) NOTHROW;

	/// 렌더 컨텍스트를 생성합니다.
	/// \return		true : 생성에 성공한 경우, false : 생성에 실패한 경우
	bool				Create(
		i3VideoInfo * pFormat,	///<	i3RenderContext를 생성하는데 있어, 화면 해상도 및 Color Depth 등의 값을 설정한다.
		void * pSetupParam		///<	i3RenderContext를 생성하는데 필요한 인자를 지정한다.
								///		Win32 Version의 경우에는 Rendering 화면이 출력된 Window의 HWND값이 지정되어야 한다.
		);

	bool				ChangeVideoInfo( i3VideoInfo * pInfo);

	BOOL				getSkipBlendModeAttr( void)			{ return m_bSkipBlendModeAttr;	}
	void				setSkipBlendModeAttr( BOOL bFlag)	{ m_bSkipBlendModeAttr = bFlag;	}

	i3VideoInfo *		GetVideoInfo(void)					{ return &m_CurrentVideoInfo; }

	i3Timer *			GetTimer(void)						{ return m_pTimer;				}
	REAL32				GetDeltaSec()						{ return m_DeltaSec;			}

	void				SetClearColor( COLOR * pColor);
	void				SetClearZValue( REAL32 val);
	void				SetClearStencilValue( UINT8 val);	

	COLOR *				GetClearColor(void)					{	return & m_BackColor; }
	REAL32				GetClearZValue(void)				{	return m_ClearZValue; }
	UINT8				GetClearStencilValue(void)			{	return m_ClearStencilValue; }

	UINT32				getClearFlag(void)					{	return m_ClearFlag; }
	void				addClearFlag( UINT32 mask)			{	m_ClearFlag |= mask; }
	void				removeClearFlag( UINT32 mask)		{	m_ClearFlag &= ~mask; }

	BOOL				IsReady(void)						{	return m_bReady; }
	BOOL				isWindowed(void)					{	return m_bWindowed;		}

	void				SetCurrentRenderTarget( i3RenderTarget * pRT);
	void				SetColorRenderTarget( i3Texture * pTex);
	void				SetDepthRenderTarget( i3Texture * pTex);

	i3RenderTarget *	GetRenderTarget( INT32 Idx)			{ return (i3RenderTarget *) m_RenderTargetList.Items[Idx]; }
	INT32				GetRenderTargetCount(void)			{ return m_RenderTargetList.GetCount(); }
	BOOL				SetRenderTargetSize( INT32 cx, INT32 cy);

	i3GfxOption*		getGfxOption(void)										{ return &g_GfxOption;		}

	// Shader가 실패된 것들에 대한 list를 얻습니다.
	i3List *			getVSFailedList(void)				{ return &m_VSFailedList;				}
	i3List *			getPSFailedList(void)				{ return &m_PSFailedList;				}


	void				SetViewport( INT32 x, INT32 y, INT32 Width, INT32 Height);

	void				SetWorldMatrix( RT_MATRIX * pMatrix);
	void				SetViewMatrix( RT_MATRIX * pMatrix);
	void				SetProjectMatrix( RT_MATRIX * pMatrix);
	void				SetVertexBlendMatricesByRef( RT_MATRIX ** pMatrix, INT32 count);
	void				SetVertexBlendMatrices( RT_MATRIX * pMatrix, INT32 count);

	INT32				getVertexBlendMatrixCount(void)				{ return m_BoneMatrixCount; }

	MATRIX *			GetWorldMatrix(void)						{ return &m_LastWorldMatrix; }
	MATRIX *			GetViewMatrix(void)							{ return &m_LastViewMatrix; }
	MATRIX *			GetProjectMatrix(void)						{ return &m_LastProjectMatrix; }
	MATRIX *			GetTextureMatrix( INT32 idx)				{ return &m_LastTextureMatrix[idx]; }
	MATRIX *			GetVertexBlendMatrixArray(void)				{ return m_BoneMatrixArray; }

	void				SetVertexArray( i3VertexArray * pVA);
	i3VertexArray *		GetVertexArray(void)						{ return m_pCurrentVA; }

	void				SetIndexArray( i3IndexArray * pIA);
	i3IndexArray *		GetIndexArray(void)							{ return m_pCurrentIA; }

	bool				Begin(void);
	void				End(void);
	void				Clear( I3G_CLEAR AddMask = 0, I3G_CLEAR RemoveMask = 0);
	bool				BeginRender(void);
	void				EndRender(void);

	void				DrawPrimUP( I3G_PRIMITIVE Prim, UINT32 PrimCount, void * pStream, UINT32 stride);
	i3Shader *			DrawPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount);
	i3Shader *			DrawStripPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count);
	i3Shader *			DrawIndexedPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount);
	i3Shader *			DrawStripIndexedPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count);
	void				DrawPointSprite( UINT32 StartIdx, UINT32 PrimCount);

	void				DrawCommandList( i3GPUCommandList * pCmdList);
	void				DrawDisplayList( i3DisplayList * pDispList);

	i3GfxPerformanceReport *	GetPerformanceReport(void)			{ return m_pPerfReport; }

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Render State

	// Alpha Blend
	void				SetAlphaTestEnable( BOOL flag);
	BOOL				GetAlphaTestEnable(void);

	void				SetBlendEnable( BOOL flag);
	BOOL				GetBlendEnable(void);

	void				SetAlphaRefValue( UINT8 refval);
	UINT8				GetAlphaRefValue(void);

	void				SetAlphaFunction( I3G_COMP_FUNC func);
	I3G_COMP_FUNC		GetAlphaFunction(void);

	// Z-Buffer
	void				SetZTestEnable( BOOL flag);
	BOOL				GetZTestEnable(void);

	void				SetZWriteEnable( BOOL flag);
	BOOL				GetZWriteEnable(void);

	void				SetZFunction( I3G_COMP_FUNC	func);
	I3G_COMP_FUNC		GetZFunction(void);

	void				SetZBias( REAL32 fSlope, REAL32 fBias);

	// Render Parameters
	void				SetRenderMode( I3G_RENDER_MODE	mode);
	I3G_RENDER_MODE		GetRenderMode(void);

	void				SetShadeMode( I3G_SHADE_MODE	mode);
	I3G_SHADE_MODE		GetShadeMode(void);

	void				SetFaceCullMode( I3G_FACE_CULL	mode);
	I3G_FACE_CULL		GetFaceCullMode(void);

	// Blend State
	void				SetSrcBlend( I3G_BLEND	blend);
	I3G_BLEND			GetSrcBlend(void);

	void				SetDestBlend( I3G_BLEND blend);
	I3G_BLEND			GetDestBlend(void);

	void				SetBlendOperation( I3G_BLEND_OP blendop);
	I3G_BLEND_OP		GetBlendOperation(void);

	void				SetBlendFactor( COLOR * pCol);

	// Fog
	void				SetFogEnable( BOOL bFlag);
	BOOL				GetFogEnable(void);

	void				SetFogColor( COLOR * pColor);
	void				GetFogColor( COLOR * pColor);

	void				SetFogMode( I3G_FOG_MODE mode);
	I3G_FOG_MODE		GetFogMode(void);

	void				SetFogTableEnable( BOOL bFlag);
	BOOL				GetFogTableEnable(void);

	void				SetFogNearFar( RT_REAL32 fNear, RT_REAL32 fFar);
	RT_REAL32			GetFogNear(void);
	RT_REAL32			GetFogFar(void);

	void				SetFogDensity( RT_REAL32 fVal);
	RT_REAL32			GetFogDensity(void);

	// Anti-aliase
	void				SetEdgeAntiAliasEnable( BOOL bFlag);
	BOOL				GetEdgeAntiAliasEnable(void);

	// Stencil Test
	BOOL				GetStencilAvailable(void);

	void				SetStencilEnable( BOOL bFlag);
	BOOL				GetStencilEnable(void);

	void				SetStencilOperation( I3G_STENCIL_OP SFail, I3G_STENCIL_OP SPass, I3G_STENCIL_OP ZFail);
	I3G_STENCIL_OP		GetStencilOperationSFail(void);
	I3G_STENCIL_OP		GetStencilOperationSPass(void);
	I3G_STENCIL_OP		GetStencilOperationZFail(void);

	void				SetStencilFunction( I3G_COMP_FUNC	func);			// default == I3G_COMP_ALWAYS
	I3G_COMP_FUNC		GetStencilFunction( void);
	
	void				SetStencilRefValue( UINT8 val);							// default == 0
	UINT8				GetStencilRefValue(void);

	void				SetStencilReadMask( UINT8 MaskVal);						// default == 0xFF
	UINT8				GetStencilReadMask(void);

	void				SetStencilWriteMask( UINT8 MaskVal);					// default == 0xFF
	UINT8				GetStencilWriteMask(void);


	// Clipping
	void				SetClippingEnable( BOOL bFlag);							// default == TRUE
	BOOL				GetClippingEnable(void);

	// Lighting
	void				SetLightingEnable( BOOL bFlag);							// default == FALSE
	BOOL				GetLightingEnable(void);

	// Vertex Color Enable
	void				SetVertexColorEnable( BOOL bFlag);						// default == TRUE
	BOOL				GetVertexColorEnable(void);

	// Vertex Color Write
	void				SetColorWriteEnable( UINT32 Mask);
	UINT32				GetColorWriteEnable( void);

	// Normalize Normal Enable
	void				SetNormalizeNormalEnable( BOOL bFlag);					// default == FALSE
	BOOL				GetNormalizeNormalEnable(void);

	COLORREAL *			getAmbient(void)									{ return &m_Ambient; }
	void				setAmbient( COLORREAL * pCol);

	// Material
	void				SetMaterial(	COLORREAL *	pDiffuseColor,
										COLORREAL * pSpecularColor,
										COLORREAL * pEmissiveColor,
										REAL32		Shininess,
										UINT32		ValidFlag = I3G_MATERIAL_DIFFUSE | I3G_MATERIAL_SPECULAR | I3G_MATERIAL_EMISSIVE | I3G_MATERIAL_SHININESS);
	COLORREAL *			getMaterialDiffuse(void)							{ return &m_MaterialDiffuse; }
	COLORREAL *			getMaterialSpecular(void)							{ return &m_MaterialSpecular; }
	COLORREAL *			getMaterialEmissive(void)							{ return &m_MaterialEmissive; }
	REAL32				getMaterialShininess(void)							{ return m_MaterialShininess; }

	void				SetColor( COLOR * pColor);
	void				GetColor( COLOR * pColor);

#if defined( I3G_OGLES)
	COLOR *				GetColor(void)										{ return &m_Color; }
#else
	COLORREAL *			GetColor(void)										{ return &m_Color; }
#endif

	// Vertex Blend Mode
	void				SetVertexBlendEnable( BOOL bFlag);						// default == TRUE
	BOOL				GetVertexBlendEnable(void);

	void				SetVertexBlendWeightCount( INT32 count);
	INT32				GetVertexBlendWeightCount(void)							{ return m_VertexBlendWeightCount; }

	void				SetSoftwareVertexBlendEnable( BOOL bFlag);				// default == FALSE
	BOOL				GetSoftwareVertexBlendEnable(void);

	// Fresnel
	bool				GetFresnelEnable(void)									{ return (m_bFresnelEnable == true); }
	void				SetFresnelEnable( bool bFlag);

	REAL32				GetFresnelIndexOfReflect(void)							{ return m_FresnelIOR; }
	void				SetFresnelIndexOfReflect( REAL32 val);

	COLORREAL *			GetFresnelColor(void)									{ return &m_FresnelColor; }
	void				SetFresnelColor( COLORREAL * pColor);
	void				SetFresnelColor( REAL32 r, REAL32 g, REAL32 b, REAL32 a)
	{
		COLORREAL col;

		i3Color::Set( &col, r, g, b, a);

		SetFresnelColor( &col);
	}

	// Texture Enable
	void				SetTextureEnable( I3G_TEXTURE_BIND bind, BOOL bState);

	void				SetTexture( I3G_TEXTURE_BIND bind, i3Texture * pTexture);
	i3Texture *			GetTexture( I3G_TEXTURE_BIND bind)							{ return m_TexBindInfo[bind].m_pTexture; }

	void				SetTextureWrap( I3G_TEXTURE_BIND bind, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap);
	void				SetTextureBorderColor( I3G_TEXTURE_BIND bind, COLOR * pColor);
	void				SetTextureFilter( I3G_TEXTURE_BIND bind, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter);
	void				SetTextureCoordSource( I3G_TEXTURE_BIND bind, I3G_TEXTURE_COORD_SOURCE source);
	void				SetTextureFunction( I3G_TEXTURE_BIND bind, I3G_TEXTURE_FUNCTION func);
	void				SetTextureCoordScale( I3G_TEXTURE_BIND bind, VEC3D * pScale, INT32 CoordCount);
	void				SetTextureCoordOffset( I3G_TEXTURE_BIND bind, VEC3D * pOffset, INT32 CoordCount);
	void				SetTextureMatrix( I3G_TEXTURE_BIND bind, RT_MATRIX * pMatrix, INT32 CoordCount = 2, BOOL bProjective = FALSE);
	I3G_TEXTURE_BIND_CONTEXT *	getTextureBindContext( I3G_TEXTURE_BIND bind)
	{
		return &m_TexBindInfo[ bind];
	}

	void				SaveTextureBindContext( I3G_TEXTURE_BIND bind, I3G_TEXTURE_BIND_CONTEXT * pCtx)
	{
		i3mem::Copy( pCtx, &m_TexBindInfo[bind], sizeof(I3G_TEXTURE_BIND_CONTEXT));
	}

	void				RestoreTextureBindContext( I3G_TEXTURE_BIND bind, I3G_TEXTURE_BIND_CONTEXT * pCtx)
	{
		i3mem::Copy( &m_TexBindInfo[bind], pCtx, sizeof(I3G_TEXTURE_BIND_CONTEXT));

		m_TexBindInfo[bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_ALL;
	}

	void				NotifyTextureBindChange(void);

	I3G_TEXTURE_FILTER	getTextureFilterMin( I3G_TEXTURE_BIND bind)					{ return m_TexBindInfo[ bind].m_MinFilter; }
	I3G_TEXTURE_FILTER	getTextureFilterMag( I3G_TEXTURE_BIND bind)					{ return m_TexBindInfo[ bind].m_MagFilter; }

	// Light
	void				setLightList( i3List * pList)		{ I3_REF_CHANGE( m_pLightList, pList); }
	INT32				getLightCount(void)					
	{ 
		if( m_pLightList == NULL)
			return 0;

		return m_pLightList->GetCount(); 
	}
	i3Light *			getLight( INT32 idx)				{ return (i3Light *) m_pLightList->Items[idx]; }	

	void				resetAffectLight(void);
	void				addAffectLight( i3Light * pLight);
	INT32				getAffectLightCount(void)			{ return m_AffectLightList.GetCount(); }
	i3Light *			getAffectLight( INT32 idx)			{ return (i3Light *) m_AffectLightList.Items[idx]; }

	/*
	void				AddShadowMap( i3Texture* pTex, MATRIX * pView2Light, MATRIX * pTN, VEC3D * pShadowLightPos, VEC3D * pShadowLightDir, COLORREAL * pShadowColor, REAL32 fShadowMapSize, REAL32 cutoff);
	void				SetShadowMapNormalizeMatrix( MATRIX * pMtx);
	I3G_SHADOWMAP_INFO *	getShadowMapInfo(void)			{ return &m_ShadowMapInfo; }
	*/

	void				GetBackBufferTexture( i3Texture *pTex); 
	void				CopyRenderTarget( i3Texture * pDest, i3Texture * pSrc);

#if defined( I3G_DX)
	void				SaveScreenShotToFile( char* pszPath, char* fileformat); 
	void				SetClearRect( D3DRECT* pRect, INT32 nCount);

	// shader
	void				setShaderEnable( BOOL bFlag)		{ m_bShaderEnable = bFlag;}
	void				SetForceShadeMode( I3G_SHADE_MODE ShadeMode);
	I3G_SHADE_MODE		getForceShadeMode( void)			{ return m_ForceShadeMode;		}

	void				SetShader( i3Shader * pCtx);
	void				SelectShader( i3Shader * pShader, INT32 pass = 0);
	bool				ValidateShader( i3ShaderCode * pShaderCode);
	i3ShaderCache *		getShaderCache(void)				{ return m_pShaderCache; }

	// Rasterize Chain관련 함수
	void				AddRasterizeChain( i3RasterizeChainBase * pChain) { pChain->AddRef();  m_RasterizeChainList.Add( pChain);	}
	i3RasterizeChainBase* GetRasterizeChain( INT32 Idx)		{ return (i3RasterizeChainBase *) m_RasterizeChainList.Items[Idx]; }
	INT32				GetRasterizeChainCount(void)		{ return m_RasterizeChainList.GetCount(); }
	i3Texture *			getRasterizeChain1(void)			{ return m_pRasterizeChain1;			}
	i3Texture *			getRasterizeChain2(void)			{ return m_pRasterizeChain2;			}
	i3Texture *			getRasterizeChainDepth(void)		{ return m_pRasterizeChainDepth;		}	
#endif

	//
	///////////////////////////////////////////////////////////

	i3VertexArray *		CreateVertexArray(void);

	static i3GfxCaps *	GetCaps(void)						{ return m_pCaps; }
	static void			setConfigProc( ADJUSTGFXCONFIGPROC pProc);
	static INT32		getDisplayModeCount( I3G_IMAGE_FORMAT fmt);
	static void			getDisplayMode( INT32 idx, I3G_IMAGE_FORMAT fmt, I3G_DISPLAY_MODE_INFO * pInfo);

	void				SetDefaultRenderState(void);
	BOOL				ResetDevice(void);
	void				UpdateDevice(void);
	void				RefreshDevice(void);

	void				BeginRecordDisplayList( i3DisplayList * pDispList);
	void				EndRecordDisplayList( i3DisplayList * pDispList);

	void				AddWaitVSyncProc( WAITVSYNCPROC pUserProc, void * pUserData);

	void				FlushVideoMemory(void);

	void	Dump(void);

#ifdef I3_DEBUG
	#if defined( I3G_DX) || defined( I3G_XBOX)

	#elif defined( I3G_PSP)

		void EnableFullFrameTimeRendering( BOOL bTrue = TRUE ){ m_bFullFrameTimeRendering = bTrue; }
		BOOL IsFullFrameTimeRendering(){ return m_bFullFrameTimeRendering; }

        void SetDispListTestFunc( DispListTestFunc pFunc );

		// 디스플레이 리스트 버퍼 얻기
		void DumpDispList( BOOL bOnce = TRUE );

	#elif defined( I3G_PS2)
	#elif defined( I3G_OGLES)
	#endif

	void ShowFPS( BOOL bTrue = TRUE ){ m_bShowFPS = bTrue; }
#endif
};

#endif

