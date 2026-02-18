#if !defined( __I3_RENDER_CONTEXT_H)
#define __I3_RENDER_CONTEXT_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3ElementBase.h"
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

#include "i3Base/smart_ptr/shared_ptr.h"

#if defined( I3G_DX)
	#include "i3ShaderCode.h"
	#include "i3ShaderContext.h"
	#include "i3RasterizeChainManager.h"
	#include "i3RasterizeChainBase.h"
	//#include "i3PostProcessManager.h"
	#include "i3PostProcess.h"
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

typedef UINT32	I3RENDER_FLAG;
#define		I3RENDER_FLAG_WINDOWED				0x00000001
#define		I3RENDER_FLAG_READY					0x00000002
#define		I3RENDER_FLAG_SKIP_BLENDMODE		0x00000004
#define		I3RENDER_FLAG_PRESENT				0x00000008
#define		I3RENDER_FLAG_FRESNEL_ENABLE		0x00000010
#define		I3RENDER_FLAG_INSTANCING			0x00000020
#define		I3RENDER_FLAG_TEXTURE_MODIFIED		0x00000040
#define		I3RENDER_FLAG_ADJUST_WINDOWPOS		0x00000080
#define		I3RENDER_FLAG_LIGHTING_ENABLE		0x00000100
#define		I3RENDER_FLAG_VERTEXCOLOR_ENABLE	0x00000200
#define		I3RENDER_FLAG_PPL_ENABLE			0x00000400
#define		I3RENDER_FLAG_ALPHATEST_ENABLE		0x00000800
#define		I3RENDER_FLAG_BLEND_ENABLE			0x00001000
#define		I3RENDER_FLAG_ZTEST_ENABLE			0x00002000
#define		I3RENDER_FLAG_ZWRITE_ENABLE			0x00004000
#define		I3RENDER_FLAG_SKIP_RENDER			0x00008000
#define		I3RENDER_FLAG_SHADER_ENABLE			0x00010000
#define		I3RENDER_FLAG_ENABLE_COMPRESSEDTEX	0x00020000
#define		I3RENDER_FLAG_REVIVED				0x00040000
#define		I3RENDER_FLAG_DLGBOXMODE_ENABLE		0x00080000
#define		I3RENDER_FLAG_SHOW_FPS				0x00100000
#define		I3RENDER_FLAG_SWAPCHAIN_ENABLE		0x00200000
#define		I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR	0x00400000			// Device->Reset() 리턴 오류에서...디바이스 로스트가 아닌, 제대로 에러가 난 상태

typedef void	(* WAITVSYNCPROC)( i3RenderContext * pCtx, void * pUserData);
typedef void	(* ADJUSTGFXCONFIGPROC)( i3VideoInfo * pVideoInfo);
typedef void	(* I3_GFX_LOSTDEVICE_PROC)( bool bLostDevice);
typedef void	(* I3_GFX_REVIVE_PROC)( i3RenderContext * pCtx);
typedef void	(* I3_GFX_RECREATE_PROC)(void);

struct	WAIT_VSYNC_INFO
{
	WAITVSYNCPROC		m_pProc = nullptr;
	void *				m_pUserData = nullptr;
};

struct	DEVICE_SWITCH_CTX
{
	HRESULT			m_hLastError = 0;

	bool			m_bInsideDeviceCallback = false;
	bool			m_bIgnoreResize = false;

	INT32			m_FullScreenWidth = 0;
	INT32			m_FullScreenHeight = 0;

	INT32			m_WindowedWidth = 0;
	INT32			m_WindowedHeight = 0;

	UINT32			m_WindowStyle = 0;
	UINT32			m_WindowExStyle = 0;

	bool			m_bIsTopMostWindow = false;

	WINDOWPLACEMENT	m_Placement;

	HMENU			m_hMenu = nullptr;
	bool			m_bDeviceLost = false;
//	HMONITOR		m_hMonitor;
};

#if defined( I3G_DX)
struct I3GFX_INSTANCING_INFO
{
	INT32 count = 0;
	i3VertexArray	*				m_pVStream = nullptr;
	i3VertexArray	*				m_pSecondVStream = nullptr;
	IDirect3DVertexDeclaration9	*	m_pVBDecl = nullptr;
	INT32							m_Offset_Stream0 = 0;
	INT32							m_Offset_Stream1 = 0;
};

enum I3G_INSTANCING_METHOD
{
	I3G_INSTANCING_METHOD_NA = -1,
	I3G_INSTANCING_METHOD_HW = 0,
	I3G_INSTANCING_METHOD_STREAM
};
#endif

typedef i3::shared_ptr<i3::vector<class i3Light*> >	i3LightListPtr;
typedef i3::shared_ptr<i3::vector<class i3Camera*> >	i3CameraListPtr;

struct LightSortProc { 	bool operator()( const i3Light* p1, const i3Light* p2) const ; };

namespace i3 
{
	I3_EXPORT_GFX void	at_3d_device_exit( void (*fn)() );	// 우선은 어플 종료시만 고려함..
}															// 제대로하려면, device생성과 reset쪽도 고려해서 함수양상이 달라야함.

class i3FullScreenWindowModeHelper;

/// 그래픽 하드웨어를 제어하고 렌더링을 수행한다.
class I3_EXPORT_GFX i3RenderContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3RenderContext, i3ElementBase);

	friend class i3ShaderContext;
	friend class i3ShaderParam;


protected:
	void *	m_pSetupParam = nullptr;
	i3Timer *			m_pTimer = nullptr;
	i3RenderTarget *	m_pRenderTarget = nullptr;


	i3Texture *			m_pLastFrameTargetTexture = nullptr;
		
	i3VertexArray *		m_pCurrentVA = nullptr;
	i3IndexArray *		m_pCurrentIA = nullptr;

	I3RENDER_FLAG		m_RenderFlag = I3RENDER_FLAG_WINDOWED | I3RENDER_FLAG_PRESENT | I3RENDER_FLAG_TEXTURE_MODIFIED |
		I3RENDER_FLAG_VERTEXCOLOR_ENABLE | I3RENDER_FLAG_PPL_ENABLE | I3RENDER_FLAG_SHADER_ENABLE;

	i3GfxOption			g_GfxOption;

	I3G_CLEAR			m_ClearFlag = I3G_CLEAR_COLOR;
	COLOR				m_BackColor = { 0, 0, 0, 255 };
	
	i3::vector<i3RenderTarget*>	m_RenderTargetList;
	i3::vector<WAIT_VSYNC_INFO*> m_WaitProcList;

	REAL32				m_ClearZValue = 1.0f;
	UINT8				m_ClearStencilValue = 0;

	INT32				m_VertexBlendWeightCount = 0;

	INT32						m_UsedClutCount = 0;

	static i3GfxCaps *			m_pCaps;

	i3GfxPerformanceReport *	m_pPerfReport = nullptr;
	UINT32						m_TriangleCount = 0;
	UINT32						m_DrawCallCount = 0;
	REAL32						m_secFrameTime = 0.0f;
	UINT32						m_FrameCountPerSec = 0;
	REAL32						m_DeltaSec = 0.0f;

	MATRIX					m_LastViewMatrix;
	MATRIX					m_LastProjectMatrix;
	MATRIX					m_OriProjectMatrix;
	MATRIX					m_LastWorldMatrix;
	MATRIX					m_BoneMatrixArray[MAX_BONE_MATRIX];
	INT32						m_BoneMatrixCount = 0;

	i3::vector_multiset<i3Light*, LightSortProc>	m_AffectLightList;
	i3LightListPtr				m_pLightList;

	COLORREAL					m_Color = { 0.0f, 0.0f, 0.0f, 0.0f };
	COLORREAL					m_Ambient;
	COLORREAL					m_MaterialDiffuse = { 0.0f, 0.0f, 0.0f, 0.0f };
	COLORREAL					m_MaterialSpecular = { 0.0f, 0.0f, 0.0f, 0.0f };
	COLORREAL					m_MaterialEmissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	REAL32						m_MaterialShininess = 0.0f;

	COLORREAL					m_SecondaryColor = { 0.6f, 0.6f, 0.1f, 1.0f };
	COLORREAL					m_TetraColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	REAL32						m_FresnelIOR = 0.0f;
	COLORREAL					m_FresnelColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	I3G_TEXTURE_BIND_CONTEXT 	m_TexBindInfo[ I3G_TEXTURE_BIND_MAX];

	i3Shader *					m_pCurShader = nullptr;
	i3ShaderCache *				m_pShaderCache = nullptr;
	char						m_szShaderCachePath[MAX_PATH] = { 0 };

	i3VideoInfo					m_CurVideoInfo;
	HMONITOR					m_CurHMonitor = 0;

	i3ShaderCode				m_ShaderCode;
	i3ShaderCode				m_OldShaderCode;

	I3G_SHADE_MODE				m_ShadeMode = I3G_SHADE_GOURAUD;
	I3G_SHADE_MODE				m_OldShadeMode = (I3G_SHADE_MODE)-1;
	// 강제적으로 Shade Mode를 지정합니다.
	I3G_SHADE_MODE				m_ForceShadeMode = (I3G_SHADE_MODE)-1;
	bool						m_doFindShaderFromModulePath = false;

	DEVICE_SWITCH_CTX			m_Switch;

	i3Texture *					m_texLightProbe = nullptr;
	COLORREAL					m_XrayColor = { 0.0f, 0.0f, 0.0f, 0.0f };

#if defined( I3G_DX)
	IDirect3D9 *				m_pD3D = nullptr;
	IDirect3DDevice9 *			m_pD3DDevice = nullptr;
	IDirect3DSwapChain9 *		m_pSwapChain = nullptr;
	IDirect3DSurface9 *			m_pSwapChainBackBuffer = nullptr;

	i3RenderTarget *			m_pDefaultRenderTarget = nullptr;
	i3RenderTarget *			m_pOriginalRenderTarget = nullptr;

	HWND						m_hWnd = nullptr;

	i3FullScreenWindowModeHelper*	m_FullScreenWindowModeHelper;


	D3DPRESENT_PARAMETERS		m_d3dpp;
	UINT32						m_TexOpMask = 0;
	UINT32						m_OldTexOpMask = 0;
	UINT32						m_TraceOpMask = 0;

	I3G_BLEND					m_SrcBlend = I3G_BLEND_ZERO;
	I3G_BLEND					m_DestBlend = I3G_BLEND_ZERO;
	I3G_BLEND_OP				m_BlendOp = I3G_BLEND_OP_NONE;

	VEC3D						m_TextureCoordScale = { 1.0f, 1.0f, 1.0f };
	VEC3D						m_TextureCoordOffset;

	RECT*						m_pPresentRect = nullptr;
	RECT						m_pSourcePresentRect = { 0, 0, 0, 0 };
	RECT						m_pDestPresentRect = { 0, 0, 0, 0 };

	i3GfxFontDXManager *		m_pFontDxManager;
	i3GfxFontDX2Manager *		m_pFontDx2Manager;

	i3GfxVertexBank *			m_pVertexBank = nullptr;

	D3DRECT*					m_pClearRectArray = nullptr;
	INT32						m_nClearRectCount = 0;

	RECT						m_curViewportRect = { 0, 0, 0, 0 };

	i3::vector<struct HIDE_WND_INFO*>	m_HideWndList;

	I3GFX_INSTANCING_INFO		m_InstancingInfo;
	I3G_INSTANCING_METHOD		m_InstancingMethod = I3G_INSTANCING_METHOD_HW;

	INT32						m_CurrentRTWidth = 0;
	INT32						m_CurrentRTHeight = 0;

	bool						m_bUseDoubleCalcul = false;

#elif defined( I3G_XBOX)
	LPDIRECT3D8             m_pD3D = nullptr; // Used to create the D3DDevice
	LPDIRECT3DDEVICE8       m_pD3DDevice = nullptr ; // Our rendering device
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

	REAL32					m_FogNear;
	REAL32					m_FogFar;

	UINT8						m_AlphaRef;
	I3G_COMP_FUNC				m_AlphaFunc;
	I3G_COMP_FUNC				m_ZFunc;
	I3G_FACE_CULL				m_FaceCull;
	I3G_BLEND					m_SrcBlend = I3G_BLEND_ZERO;
	I3G_BLEND					m_DestBlend = I3G_BLEND_ZERO;
	I3G_BLEND_OP				m_BlendOp = I3G_BLEND_OP_NONE;
	int							m_SrcBlendMask;
	int							m_DestBlendMask;
	bool						m_bFogEnable;
	COLOR						m_FogColor;
	bool						m_bAALine;
	bool						m_bStencilEnable;
	I3G_STENCIL_OP				m_StencilSFail;
	I3G_STENCIL_OP				m_StencilSPass;
	I3G_STENCIL_OP				m_StencilZFail;
	I3G_COMP_FUNC				m_StencilFunc;
	UINT8						m_StencilRef;
	UINT8						m_StencilMask;
	bool						m_bClipping;
	COLOR						m_AmbientColor;
	COLOR						m_DiffuseColor;
	COLOR						m_SpecularColor;
	COLOR						m_EmissiveColor;
	COLOR						m_Color = { 0, 0, 0, 0 };
	I3G_TEXTURE_FILTER			m_TexMinFilter;
	I3G_TEXTURE_FILTER			m_TexMagFilter;
	
	bool						m_bNormalizeNormal;
	bool						m_bBCE;

	INT32						m_TexFunc;
	INT32						m_OldTexFunc;

	bool						m_bProjectiveTexture;

	bool						m_bFullFrameTimeRendering;
	i3Timer *					m_pFPSTimer;
	bool						m_bFPS30;	
#elif defined( I3G_PS2)
#endif

#if defined( I3G_DX) || defined( I3G_XBOX)
protected:

	bool		FindAppropriateVideoInfo( i3VideoInfo * pWantInfo, i3VideoInfo * pFindInfo);
	void		GetCurrentVideoInfo( i3VideoInfo * pInfo);
	bool		FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
				D3DFORMAT Format, UINT32 DepthBits, UINT32 StencilBits, D3DFORMAT* pDepthStencilFormat );
	bool		IsCompressedTextureFormatOk( D3DFORMAT AdapterFormat );

	bool		CanBeReset( i3VideoInfo * pInfo);

	void		MinimizeOtherWindows(void);
	void		RestoreOtherWindows(void);

	bool		_CreateDevice(void);
	void		_InvalidateGfxResource( bool bLostDevice);
	void		_ValidateGfxResource(void);
//	INT32		_GetMonitorFromWindow( HWND hwnd, HMONITOR* outHMonitor = nullptr);
	void		UpdateVideoInfo(void);
	void		UpdateRenderTarget(void);
	void		OnInit(void);

	void		_SetCurrentViewportRect( INT32 x, INT32 y, INT32 width, INT32 height);

public:

	HMONITOR	FindCurrentMonitorHandle() const;
	INT32		FindAdapterIndex(HMONITOR hMonitor) const;
	bool		FindCurrentMonitorHandleAndAdapterIndex(HMONITOR& outCurrHMon, INT32& outAdapterIndex);


	void	addRenderFlag( I3RENDER_FLAG flag)							{ m_RenderFlag |= flag; }
	void	removeRenderFlag( I3RENDER_FLAG flag)						{ m_RenderFlag &= ~flag; }
	bool	IsRenderFlag( I3RENDER_FLAG flag)							{ return (m_RenderFlag & flag) == flag; }

	void *	GetNativeDevice(void)										{ return m_pD3DDevice; }
	HWND	getHWND(void)												{ return m_hWnd; }

	RECT	GetSourcePresentRect() const								{ return m_pSourcePresentRect; }
	RECT	GetDestPresentRect() const									{ return m_pDestPresentRect; }
	void	SetPresentRect( RECT* rt)									{ m_pPresentRect = rt; }
	void	SetSourcePresentRect( RECT& rt)								{ m_pSourcePresentRect = rt; }
	void	SetDestPresentRect( RECT& rt)								{ m_pDestPresentRect = rt; }

	void	SetSkipRender( bool bRender)								{ bRender?addRenderFlag(I3RENDER_FLAG_SKIP_RENDER):removeRenderFlag(I3RENDER_FLAG_SKIP_RENDER);	}
	bool	GetSkipRender( void)										{ return IsRenderFlag( I3RENDER_FLAG_SKIP_RENDER);	}
	bool	IsCompressedTexture( void)									{ return IsRenderFlag( I3RENDER_FLAG_ENABLE_COMPRESSEDTEX); }
	void	SetPPLEnable( bool bFlag)									{ bFlag?addRenderFlag( I3RENDER_FLAG_PPL_ENABLE):removeRenderFlag(I3RENDER_FLAG_PPL_ENABLE); }
	bool	getPPLEnable()												{ return IsRenderFlag( I3RENDER_FLAG_PPL_ENABLE);		}	
	i3GfxFontDXManager * getFontDxManager(void)							{ return m_pFontDxManager;	}
	i3GfxFontDX2Manager * getFontDx2Manager(void)						{ return m_pFontDx2Manager;	}
	i3GfxVertexBank * getVertexBank(void)								{ return m_pVertexBank;		}
	bool	isContextRivieved(void)										{ return IsRenderFlag(I3RENDER_FLAG_REVIVED);		}

	void	SetDefaultRenderTarget(i3RenderTarget * pRT)				{ m_pDefaultRenderTarget = pRT; }
	i3RenderTarget*	GetDefaultRenderTarget()							{ return m_pDefaultRenderTarget; }
	void	SetOriginalRenderTarget(i3RenderTarget * pRT)				{ m_pOriginalRenderTarget = pRT; }
	void	ResetDefaultRenderTarget()									{ m_pDefaultRenderTarget = m_pOriginalRenderTarget; }

	void	setDialogBoxMode(bool bFlag)								{ bFlag?addRenderFlag(I3RENDER_FLAG_DLGBOXMODE_ENABLE):removeRenderFlag(I3RENDER_FLAG_DLGBOXMODE_ENABLE); }

	void	setAdjustWindowPosState( bool bState)						{ bState?addRenderFlag(I3RENDER_FLAG_ADJUST_WINDOWPOS):removeRenderFlag(I3RENDER_FLAG_ADJUST_WINDOWPOS); }

	

#elif defined( I3G_PSP)

protected:
	bool	m_bDumpDispList;
	bool	m_bDumpDispListOnce;
public:
	void	SetHalfFPSEnable( bool bFlag)				{ m_bFPS30 = bFlag; }
	void	EndDispListRender();

#elif defined( I3G_PS2)
#elif defined( I3G_OGLES)
	void				SetMatrix( I3G_MATRIX_STACK Index, MATRIX * pMatrix);
	void				GetMatrix( I3G_MATRIX_STACK Index, MATRIX * pMatrix);
	void				SetCurrentRenderTarget( i3Texture * pTex);
	bool				SetRenderTargetSize( UINT32 cx, UINT32 cy);
	void				GetMaterialAmbient( COLORREAL * pColor);
	void				GetMaterialDiffuse( COLORREAL * pColor);
	void				GetMaterialSpecular( COLORREAL * pColor);
	void				GetMaterialEmissive( COLORREAL * pColor);
	void				SetVertexBlendMatrix( INT32 Idx, MATRIX * pMatrix);
	void				SetTextureEnable( INT32 StageIdx, bool bState);
	void				SetTexture( INT32 StageIdx, i3Texture * pTexture, bool bTextureChange = false);
	void				SetTextureFilter( INT32 StageIdx, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter);
	void				SetTextureWrap( INT32 StageIdx, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap);
	void				SetTextureFunction( INT32 StageIdx, I3G_TEXTURE_FUNCTION func);
	void				SetTextureMatrix( INT32 StageIdx, MATRIX * pMatrix, INT32 CoordCount, bool bProjective);
	void				SetTextureCoordScale( INT32 StageIdx, VEC3D * pScale, INT32 CoordCount);
	void				SetTextureCoordSource( INT32 StageIdx, I3G_TEXTURE_COORD_SOURCE source);
	void				SetLight( i3Light * pLight);
	bool				Reset(void);
	void				SetTextureCoordOffset( INT32 StageIdx, VEC3D * pOffset, INT32 CoordCount);
	//bool				Create( void * pNativeRTHandle);

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
	void				_SetTextureMatrix( INT32 StageIdx, MATRIX * pMatrix, INT32 CoordCount = 2, bool bProjective = false);
	void				_SetTextureBorderColor( INT32 StageIdx, UINT32 color);
	void				_SetMaxAnisotropy( INT32 idxStage, INT32 val);

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

	bool				ChangeVideoInfo( i3VideoInfo * pInfo, bool bForceRecreate = false);

	bool				getSkipBlendModeAttr( void)			{ return IsRenderFlag( I3RENDER_FLAG_SKIP_BLENDMODE);	}
	void				setSkipBlendModeAttr( bool bFlag)	{ bFlag ? addRenderFlag( I3RENDER_FLAG_SKIP_BLENDMODE):removeRenderFlag( I3RENDER_FLAG_SKIP_BLENDMODE);	}

	i3VideoInfo *		GetVideoInfo(void)					{ return &m_CurVideoInfo; }
	HMONITOR			GetCurrentMonitorHandle() const		{ return m_CurHMonitor;  }

	INT32				getWindowWidth(void)				{ return m_CurVideoInfo.m_Width; }
	INT32				getWindowHeight(void)				{ return m_CurVideoInfo.m_Height; }
	I3G_IMAGE_FORMAT	getWindowColorFormat(void)			{ return m_CurVideoInfo.m_RenderTargetFormat; }
	I3G_IMAGE_FORMAT	getWindowDepthFormat(void)			{ return m_CurVideoInfo.m_DepthFormat; }

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

	bool				IsReady(void)						{	return IsRenderFlag(I3RENDER_FLAG_READY); }
	bool				isWindowed(void)					{	return IsRenderFlag(I3RENDER_FLAG_WINDOWED);}

	void				modifyMouseCursorPosition(POINT& inout_point);
	void				Process_WM_ACTIVATE(bool isActive);

	bool				getPresentState(void)				{ return IsRenderFlag(I3RENDER_FLAG_PRESENT); }
	void				setPresentState( bool bState)		{ bState?addRenderFlag(I3RENDER_FLAG_PRESENT):removeRenderFlag(I3RENDER_FLAG_PRESENT); }

	void				SetCurrentRenderTarget( i3RenderTarget * pRT);
	void				SetColorRenderTarget( i3Texture * pTex);
	void				SetDepthRenderTarget( i3Texture * pTex);

	i3RenderTarget *	GetRenderTarget( INT32 Idx)			{ return m_RenderTargetList[Idx]; }
	INT32				GetRenderTargetCount(void)			{ return (INT32)m_RenderTargetList.size(); }
	bool				SetRenderTargetSize( INT32 cx, INT32 cy);

	INT32				GetCurrentRTWidth()							{ return m_CurrentRTWidth; }
	INT32				GetCurrentRTHeight()						{ return m_CurrentRTHeight; }

	i3GfxOption*		getGfxOption(void)										{ return &g_GfxOption;		}

	void				SetViewport( INT32 x, INT32 y, INT32 Width, INT32 Height);

	void				SetWorldMatrix( MATRIX * pMatrix);
	void				SetViewMatrix( MATRIX * pMatrix);
	void				SetProjectMatrix( MATRIX * pMatrix);
	void				SetProjectOffset(const VEC3D &vec);
	void				SetVertexBlendMatricesByRef( MATRIX ** pMatrix, INT32 count);
	void				SetVertexBlendMatrices( MATRIX * pMatrix, INT32 count);

	INT32				getVertexBlendMatrixCount(void)				{ return m_BoneMatrixCount; }

	MATRIX *			GetWorldMatrix(void)						{ return &m_LastWorldMatrix; }
	MATRIX *			GetViewMatrix(void)							{ return &m_LastViewMatrix; }
	MATRIX *			GetProjectMatrix(void)						{ return &m_LastProjectMatrix; }
	MATRIX *			GetTextureMatrix( INT32 idx)				{ return &m_TexBindInfo[idx].m_Matrix; }
	MATRIX *			GetVertexBlendMatrixArray(void)				{ return m_BoneMatrixArray; }

	void				SetVertexArray( i3VertexArray * pVA);

	#if defined( I3G_DX)
		void			SetInstancingArray( I3GFX_INSTANCING_INFO * pInfo);
	#endif

	i3VertexArray *		GetVertexArray(void)						{ return m_pCurrentVA; }

	void				SetIndexArray( i3IndexArray * pIA);
	i3IndexArray *		GetIndexArray(void)							{ return m_pCurrentIA; }

	bool				Begin(void);
	virtual void		End(void);
	void				Clear( I3G_CLEAR AddMask = 0, I3G_CLEAR RemoveMask = 0);
	bool				BeginRender(void);
	void				EndRender(void);

	void				DrawPrimUP( I3G_PRIMITIVE Prim, UINT32 PrimCount, void * pStream, UINT32 stride);
	i3Shader *			DrawPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount);
	i3Shader *			DrawStripPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count);
	i3Shader *			DrawIndexedPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount);
	i3Shader *			DrawStripIndexedPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count);
	void				DrawPointSprite( UINT32 StartIdx, UINT32 PrimCount);
	i3Shader *			DrawDXMesh( LPD3DXMESH pMesh, i3Shader * pShader, UINT32 Subset, UINT32 PrimCount);

	void				DrawCommandList( i3GPUCommandList * pCmdList);
	void				DrawDisplayList( i3DisplayList * pDispList);

	i3GfxPerformanceReport *	GetPerformanceReport(void)			{ return m_pPerfReport; }

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Render State

	// Alpha Blend
	void				SetAlphaTestEnable( bool flag);
	bool				GetAlphaTestEnable(void);

	void				SetBlendEnable( bool flag);
	bool				GetBlendEnable(void);

	void				SetAlphaRefValue( UINT8 refval);
	UINT8				GetAlphaRefValue(void);

	void				SetAlphaFunction( I3G_COMP_FUNC func);
	I3G_COMP_FUNC		GetAlphaFunction(void);

	// Z-Buffer
	void				SetZTestEnable( bool flag);
	bool				GetZTestEnable(void);

	void				SetZWriteEnable( bool flag);
	bool				GetZWriteEnable(void);

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
	void				SetFogEnable( bool bFlag);
	bool				GetFogEnable(void);

	void				SetFogColor( COLOR * pColor);
	void				GetFogColor( COLOR * pColor);

	void				SetFogMode( I3G_FOG_MODE mode);
	I3G_FOG_MODE		GetFogMode(void);

	void				SetFogTableEnable( bool bFlag);
	bool				GetFogTableEnable(void);

	void				SetFogNearFar( REAL32 fNear, REAL32 fFar);
	REAL32			GetFogNear(void);
	REAL32			GetFogFar(void);

	void				SetFogDensity( REAL32 fVal);
	REAL32			GetFogDensity(void);

	// Anti-aliase
	void				SetEdgeAntiAliasEnable( bool bFlag);
	bool				GetEdgeAntiAliasEnable(void);

	// Stencil Test
	bool				GetStencilAvailable(void);

	void				SetStencilEnable( bool bFlag);
	bool				GetStencilEnable(void);

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
	void				SetClippingEnable( bool bFlag);							// default == true
	bool				GetClippingEnable(void);

	// Lighting
	void				SetLightingEnable( bool bFlag);							// default == false
	bool				GetLightingEnable(void);

	// Vertex Color Enable
	void				SetVertexColorEnable( bool bFlag);						// default == true
	bool				GetVertexColorEnable(void);

	// Vertex Color Write
	void				SetColorWriteEnable( UINT32 Mask);
	UINT32				GetColorWriteEnable( void);

	// Normalize Normal Enable
	void				SetNormalizeNormalEnable( bool bFlag);					// default == false
	bool				GetNormalizeNormalEnable(void);

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

	void				SetColor( COLORREAL * pColor);
	void				GetColor( COLORREAL * pColor);

#if defined( I3G_OGLES)
	COLOR *				GetColor(void)										{ return &m_Color; }
#else
	COLORREAL *			GetColor(void)										{ return &m_Color; }
#endif

	COLORREAL *			getSecondaryColor(void)								{ return &m_SecondaryColor; }
	void				setSecondaryColor( COLORREAL * pCol);

	COLORREAL *			getTetraColor(void)									{ return &m_TetraColor; }
	void				setTetraColor( COLORREAL * pCol);

	// Vertex Blend Mode
	void				SetVertexBlendEnable( bool bFlag);						// default == true
	bool				GetVertexBlendEnable(void);

	void				SetVertexBlendWeightCount( INT32 count);
	INT32				GetVertexBlendWeightCount(void)							{ return m_VertexBlendWeightCount; }

	void				SetSoftwareVertexBlendEnable( bool bFlag);				// default == false
	bool				GetSoftwareVertexBlendEnable(void);

	// Fresnel
	bool				GetFresnelEnable(void)									{ return IsRenderFlag( I3RENDER_FLAG_FRESNEL_ENABLE); }
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

	// Instancing
	bool				getInstancingEnable( void)								{ return IsRenderFlag( I3RENDER_FLAG_INSTANCING);}
	void				setInstancingEnable( bool bEnable)						{ bEnable?addRenderFlag(I3RENDER_FLAG_INSTANCING):removeRenderFlag(I3RENDER_FLAG_INSTANCING);}

	// Texture Enable
	void				SetTextureEnable( I3G_TEXTURE_BIND bind, bool bState);
	void				SetTextureEnableByBindAttr(  I3G_TEXTURE_BIND bind, bool bState);
	
private:
	void				SetTextureEnableImpl( I3G_TEXTURE_BIND bind, bool bState);

public:

	void				SetTexture( I3G_TEXTURE_BIND bind, i3Texture * pTexture, bool bTextureChange = false);
	i3Texture *			GetTexture( I3G_TEXTURE_BIND bind)							{ return m_TexBindInfo[bind].m_pTexture; }

	void				SetTextureWrap( I3G_TEXTURE_BIND bind, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap);
	void				SetTextureBorderColor( I3G_TEXTURE_BIND bind, COLOR * pColor);
	void				SetTextureFilter( I3G_TEXTURE_BIND bind, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter);
	void				SetTextureCoordSource( I3G_TEXTURE_BIND bind, I3G_TEXTURE_COORD_SOURCE source);
	void				SetTextureCoordScale( I3G_TEXTURE_BIND bind, VEC3D * pScale, INT32 CoordCount);
	void				SetTextureCoordOffset( I3G_TEXTURE_BIND bind, VEC3D * pOffset, INT32 CoordCount);
	void				SetTextureMatrix( I3G_TEXTURE_BIND bind, MATRIX * pMatrix, INT32 CoordCount = 2, bool bProjective = false);
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
	void				setLightList(const i3LightListPtr& pList)		{ m_pLightList = pList; }
	INT32				getLightCount(void)					
	{ 
		if( m_pLightList == nullptr)
			return 0;
		return (INT32)m_pLightList->size(); 
	}
	i3Light *			getLight( INT32 idx)				{ return (*m_pLightList)[idx]; }	

	void				resetAffectLight(void);
	void				addAffectLight( i3Light * pLight);
	INT32				getAffectLightCount(void)			{ return (INT32)m_AffectLightList.size(); }
	i3Light *			getAffectLight( INT32 idx)			{ return m_AffectLightList.get_vector()[idx]; }

	/*
	void				AddShadowMap( i3Texture* pTex, MATRIX * pView2Light, MATRIX * pTN, VEC3D * pShadowLightPos, VEC3D * pShadowLightDir, COLORREAL * pShadowColor, REAL32 fShadowMapSize, REAL32 cutoff);
	void				SetShadowMapNormalizeMatrix( MATRIX * pMtx);
	I3G_SHADOWMAP_INFO *	getShadowMapInfo(void)			{ return &m_ShadowMapInfo; }
	*/

	void				GetBackBufferTexture( i3Texture *pTex); 
	void				CopyRenderTarget( i3Texture * pDest, i3Texture * pSrc);
	void				ColorFill( i3Texture * pDest, COLOR * pCol);

#if defined( I3G_DX)
	void				SaveScreenShotToFile( char* pszPath, char* fileformat); 
	void				SaveScreenShotToFileW( const wchar_t* pszPath, char* fileformat); 
	void				SetClearRect( D3DRECT* pRect, INT32 nCount);

	// shader
	void				setShaderEnable( bool bFlag)		{ bFlag?addRenderFlag(I3RENDER_FLAG_SHADER_ENABLE):removeRenderFlag(I3RENDER_FLAG_SHADER_ENABLE);}
	i3Shader	*		getCurrentShader(void)				{ return m_pCurShader;}
	void				SetForceShadeMode( I3G_SHADE_MODE ShadeMode);
	I3G_SHADE_MODE		getForceShadeMode( void)			{ return m_ForceShadeMode;		}

	// 맥스익스포터와같이 DLL로 실행이될경우, 셰이더파일위치를 찾기가 모호해지므로, 현재 DLL이있는폴더에서 찾기위해 추가.
	void				setShaderFindFromModulePath(bool enable) { m_doFindShaderFromModulePath = enable; }

	void				SetShader( i3Shader * pCtx);
	void				SelectShader( i3Shader * pShader, INT32 pass = 0);
	bool				ValidateShader( i3ShaderCode * pShaderCode);
	i3ShaderCache *		getShaderCache(void)				{ return m_pShaderCache; }

	i3Texture *			getLightProbeTexture(void)			{ return m_texLightProbe; }

	void				SaveLastFrameTexture(void);
	i3Texture *			getLastFrameTargetTexture(void)		{ return m_pLastFrameTargetTexture;}

	void				getRenderContextModulePath(char* path, INT32 sizBuf=MAX_PATH);
#endif

	//
	///////////////////////////////////////////////////////////

	i3VertexArray *		CreateVertexArray(void);

	static i3GfxCaps *	GetCaps(void)						{ return m_pCaps; }
	static void			setConfigProc( ADJUSTGFXCONFIGPROC pProc);
	static INT32		getDisplayModeCount( I3G_IMAGE_FORMAT fmt);
	static void			getDisplayMode( INT32 idx, I3G_IMAGE_FORMAT fmt, I3G_DISPLAY_MODE_INFO * pInfo);

	void				SetDefaultRenderState(void);
	

	bool				OnResetDevice();
	bool				PrevResetDevice();
	bool				ResetDevice(void);
	bool				PostResetDevice();

	void				UpdateDevice(void);
	void				RefreshDevice(void);

	void				BeginRecordDisplayList( i3DisplayList * pDispList);
	void				EndRecordDisplayList( i3DisplayList * pDispList);

	void				AddWaitVSyncProc( WAITVSYNCPROC pUserProc, void * pUserData);

	void				FlushVideoMemory(void);

	bool				CheckSupportedFormat( I3G_IMAGE_FORMAT fmt, I3G_USAGE usage);

	static void		SetLostDeviceHandler( I3_GFX_LOSTDEVICE_PROC pProc);
	static void		SetReviveHandler( I3_GFX_REVIVE_PROC pProc);
	static void		SetRecreateHandler( I3_GFX_RECREATE_PROC proc);

	void				SetXrayColor(COLORREAL * pColor);
	COLORREAL*			GetXrayColor(void) { return &m_XrayColor; }

	void	Dump(void);

#if defined( I3G_DX)
	void	Print_D3DPRESENT_PARAMETERS(D3DPRESENT_PARAMETERS& d3dpp);
#endif

#ifdef I3_DEBUG
	#if defined( I3G_DX) || defined( I3G_XBOX)

	#elif defined( I3G_PSP)

		void EnableFullFrameTimeRendering( bool bTrue = true){ m_bFullFrameTimeRendering = bTrue; }
		bool IsFullFrameTimeRendering(){ return m_bFullFrameTimeRendering; }

        void SetDispListTestFunc( DispListTestFunc pFunc );

		// 디스플레이 리스트 버퍼 얻기
		void DumpDispList( bool bOnce = true);

	#elif defined( I3G_PS2)
	#elif defined( I3G_OGLES)
	#endif

	void ShowFPS( bool bTrue = true){ bTrue?addRenderFlag(I3RENDER_FLAG_SHOW_FPS):removeRenderFlag(I3RENDER_FLAG_SHOW_FPS); }
#endif
	void SetEnableSwapChain(bool bEnable) { bEnable?addRenderFlag(I3RENDER_FLAG_SWAPCHAIN_ENABLE):removeRenderFlag(I3RENDER_FLAG_SWAPCHAIN_ENABLE); }

	void SetBackBufferSize(INT32 nX, INT32 nY);

	bool IsIgnoreResize() { return m_Switch.m_bIgnoreResize;}

	void	OnRecreate( void);
	void	SetDoubleCalcul(bool bState) { m_bUseDoubleCalcul = bState; }

#if defined( I3_DEBUG)
private:
	struct tagPRIMITIVEINFO {
		i3::string			_name;
		INT32				_startIdx;
		INT32				_primCnt;

		tagPRIMITIVEINFO(const char * pszname, INT32 start, INT32 cnt)
			: _startIdx(start), _primCnt(cnt) {
				if( pszname != nullptr)	_name = pszname;
		}
	};
	i3::vector<tagPRIMITIVEINFO*>	m_DrawPrimitiveList;

	bool		m_bDumpDrawPrimitive;

public:
	void		AddDrawPrimitiveList( const char * pszname, INT32 start, INT32 cnt);

	void		DumpDrawPrimitiveList( void)		{ m_bDumpDrawPrimitive = true; }
#endif

	void		SetMipmapBias(REAL32 fBias);
};
	
#endif

