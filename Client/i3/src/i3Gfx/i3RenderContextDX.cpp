#include "i3GFXType.h"

#ifdef I3G_DX
#include <stdio.h>
#include <stdlib.h>
#include "i3Input/dxerr.h"
#include "i3RenderContext.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3GfxUtil.h"
#include "i3GfxUtilDX.h"
#include "i3GfxglobalVariable.h"
#include "i3TextureDX.h" 
#include "i3ShaderContext.h"
#include "i3VertexArrayDX.h"

#include <sstream>
#include "i3ShaderSourceCodeMgr.h"

#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/compare/generic_is_equal.h" 

#include "i3Base/string/ext/get_exec_path.h"

#include "i3FullScreenWindowModeHelper.h"

namespace i3
{
	namespace 
	{
		i3::vector< void(*)() > g_d3d_exit_fn_list;
	}
	
	void	at_3d_device_exit( void (*fn)() )
	{
		g_d3d_exit_fn_list.push_back(fn);
	}
	
	void	process_3d_device_exit()		// 종료때 m_pd3dDevice소거 직전에 호출 처리하면 될것 같다.
	{
		while ( !g_d3d_exit_fn_list.empty() )
		{
			void (*fn)() = g_d3d_exit_fn_list.back();
			(*fn)();
			g_d3d_exit_fn_list.pop_back();
		}
	}

	
	
}


#if defined( _LIGHT_PROBE_SHADING_)
#include "i3ImageFile.h"
#endif

//PerfHUD 사용할시 밑의 USE_PERF_HUD 주석을 제거하셔야 됩니다.(PB_ENG_DONGIK.HA)

//#define USE_PERF_HUD
#if defined( I3_DEBUG)							
	#if defined( USE_PERF_HUD)
		#define		NV_PERF_HUD_ENABLE
	#endif
#endif

#define		TEXOP_MASK_BINARY		0x00000010

#define		TEXOP_DISABLE			0x00000001
#define		TEXOP_SELECTARG0		0x00000002
#define		TEXOP_MODULATE			0x00000003 | TEXOP_MASK_BINARY
#define		TEXOP_ADD				0x00000004 | TEXOP_MASK_BINARY
#define		TEXOP_SPECULAR			0x00000005 | TEXOP_MASK_BINARY

#define		TEXARG0_TEXTURE			0x00000100
#define		TEXARG0_DIFFUSE			0x00000200
#define		TEXARG0_TFACTOR			0x00000300
#define		TEXARG0_CURRENT			0x00000400

#define		TEXARG1_TEXTURE			0x00010000
#define		TEXARG1_DIFFUSE			0x00020000
#define		TEXARG1_TFACTOR			0x00030000
#define		TEXARG1_CURRENT			0x00040000

#define		TEXMASK_OP				0x000000FF
#define		TEXMASK_ARG0			0x0000FF00
#define		TEXMASK_ARG1			0x00FF0000

#define		TEXSHIFT_ARG0			8
#define		TEXSHIFT_ARG1			16

static UINT32	s_TexOp[4] =
{
	D3DTOP_DISABLE,
	D3DTOP_DISABLE,			// 1
	D3DTOP_SELECTARG1,		// 2
	D3DTOP_MODULATE,		// 3
};

static UINT32	s_TexArg[4] =
{
	D3DTA_TEXTURE,
	D3DTA_TEXTURE,			// 1
	D3DTA_DIFFUSE,			// 2
	D3DTA_TFACTOR,			// 3
};

static UINT32 s_TexMask[2][2][2] =
{
	// Lighting : false
	{
		// Texture : false
		{
			// Vertex Color : false
			TEXOP_SELECTARG0 | TEXARG0_TFACTOR,
		
			// Vertex Color : true
			TEXOP_MODULATE | TEXARG0_DIFFUSE | TEXARG1_TFACTOR
		},

		// Texture : true
		{
			// Vertex Color : false
			TEXARG0_TEXTURE | TEXARG1_TFACTOR,

			// Vertex Color : true
			TEXARG0_TEXTURE | TEXARG1_DIFFUSE
		}
	},

	// Lighting : true
	{
		// Texture : false
		{
			// Vertex Color : false
			TEXOP_DISABLE,
		
			// Vertex Color : true
			TEXOP_SELECTARG0 | TEXARG0_DIFFUSE
		},

		// Texture : true
		{
			// Vertex Color : false
			TEXARG0_TEXTURE | TEXARG1_DIFFUSE,

			// Vertex Color : true
			TEXARG0_TEXTURE | TEXARG1_DIFFUSE
		}
	}
};

//--------------------------------------------------------------------------------------
// Error codes
//--------------------------------------------------------------------------------------
#define DXUTERR_NODIRECT3D              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0901)
#define DXUTERR_NOCOMPATIBLEDEVICES     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0902)
#define DXUTERR_MEDIANOTFOUND           MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0903)
#define DXUTERR_NONZEROREFCOUNT         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0904)
#define DXUTERR_CREATINGDEVICE          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0905)
#define DXUTERR_RESETTINGDEVICE         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0906)
#define DXUTERR_CREATINGDEVICEOBJECTS   MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0907)
#define DXUTERR_RESETTINGDEVICEOBJECTS  MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0908)
#define DXUTERR_DEVICEREMOVED           MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x090A)

//I3_CLASS_INSTANCE( i3RenderContext, i3ElementBase);
I3_CLASS_INSTANCE( i3RenderContext);

typedef HMONITOR (WINAPI* LPMONITORFROMWINDOW)(HWND, DWORD);

static bool						s_bSoftwareProcess = false;
static I3_GFX_LOSTDEVICE_PROC	s_pGfxLostDeviceHandler = nullptr;
static I3_GFX_REVIVE_PROC		s_pGfxReviveHandler = nullptr;
static I3_GFX_RECREATE_PROC		s_pGfxRecreateHandler = nullptr;

static INT32		s_svX, s_svY, s_svWidth, s_svHeight;

/*
static INT32		_LightSortProc( i3Light * p1, i3Light * p2)
{
	if( p1->getSqDistance() > p2->getSqDistance())
		return 1;
	else if( p1->getSqDistance() < p2->getSqDistance())
		return -1;

	return 0;
}
*/
/*
void Delayed_Release_D3D()
{
	if (g_pD3DDevice)
	{
		int ref_count =	g_pD3DDevice->Release();
	
		if (ref_count > 0)
		{
			I3TRACE("\nDirect3DDevice Ref Count not zero : %d counts exists.\n", ref_count);
		}
		g_pD3DDevice = nullptr;
	}

	if (g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = nullptr;
	}
}
*/

bool LightSortProc::operator()( const i3Light* p1, const i3Light* p2) const  
{	
	return p1->getSqDistance() < p2->getSqDistance(); 
}

i3RenderContext::i3RenderContext(void)
	: m_FullScreenWindowModeHelper(nullptr)
{
	g_pD3Dpp = &m_d3dpp;
	
	memset( &m_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	m_d3dpp.Windowed = TRUE;

	g_pRenderContext = this;
	
	m_OldShaderCode.setCode( 0xFFFFFFFF00000000L);
	
	m_AffectLightList.reserve( 64);

	for( INT32 i = 0; i < I3G_TEXTURE_BIND_MAX; i++)
	{
		m_TexBindInfo[i].m_BindType				= (I3G_TEXTURE_BIND) i;
		m_TexBindInfo[i].m_HWrap				= I3G_TEXTURE_WRAP_WRAP;
		m_TexBindInfo[i].m_VWrap				= I3G_TEXTURE_WRAP_WRAP;
		m_TexBindInfo[i].m_ModifyFlag			= 1;
		m_TexBindInfo[i].m_BorderColor			= 0xFFFFFFFF;
	}

#if defined( I3G_DX)
	m_pFontDxManager = i3GfxFontDXManager::new_object();
	m_pFontDx2Manager = i3GfxFontDX2Manager::new_object();
#endif

	#if 0
		// Offset Table
		static INT32 s_OffTable[32] = 
		{
			0,	0,		1,	0,
			0,	1,		1,	1,

			-2,	-2,		-1,	-2,
			-2,	-1,		-1,	-1,

			0,	-2,		1,	-2,
			0,	-1,		1,	-1,

			-2,	0,		-1,	0,
			-2,	1,		-1,	1
		};

		for( i = 0; i < 8; i++)
		{
			INT32 idx = (i * 4);

			/*
			i3Vector::Set( &m_ShadowMapInfo.m_OffsetTable[ i], 
				s_OffTable[idx + 0] + i3Math::Randf2() * 0.5f, s_OffTable[ idx + 1] + i3Math::Randf2() * 0.5f, 
				s_OffTable[idx + 2] + i3Math::Randf2() * 0.5f, s_OffTable[ idx + 3] + i3Math::Randf2() * 0.5f);
				*/

			i3Vector::Set( &m_ShadowMapInfo.m_OffsetTable[ i], 
				s_OffTable[idx + 0], s_OffTable[ idx + 1], 
				s_OffTable[idx + 2], s_OffTable[ idx + 3]);
		}
	#else
		/*
		#define NUM_POISSON_TAPS 12
		const REAL32 PoissonDiscOffsets[NUM_POISSON_TAPS][2] = 
		{
			{-0.326212f, -0.40581f},
			{-0.840144f, -0.07358f},
			{-0.695914f, 0.457137f},
			{-0.203345f, 0.620716f},
			{0.96234f, -0.194983f},
			{0.473434f, -0.480026f},
			{0.519456f, 0.767022f},
			{0.185461f, -0.893124f},
			{0.507431f, 0.064425f},
			{0.89642f, 0.412458f},
			{-0.32194f, -0.932615f},
			{-0.791559f, -0.59771f}
		};
		*/

		/*
		#define NUM_POISSON_TAPS 7
		const REAL32 PoissonDiscOffsets[NUM_POISSON_TAPS][2] = 
		{
			{ 0.527837f,-0.085868f },
			{-0.040088f, 0.536087f },
			{-0.670445f,-0.179949f },
			{-0.419418f,-0.616039f },
			{ 0.440453f,-0.639399f },
			{-0.757088f, 0.349334f },
			{ 0.574619f, 0.685879f }
		};

		for( i = 0; i < (NUM_POISSON_TAPS >> 1); i++)
		{
			i3Vector::Set( &m_ShadowMapInfo.m_OffsetTable[i],
				PoissonDiscOffsets[i << 1][0],	PoissonDiscOffsets[i << 1][1],
				PoissonDiscOffsets[(i << 1) + 1][0], PoissonDiscOffsets[(i << 1) + 1][1]);
		}

		#if( NUM_POISSON_TAPS & 1)
		{
			i3Vector::Set( &m_ShadowMapInfo.m_OffsetTable[i],
				PoissonDiscOffsets[i << 1][0],	PoissonDiscOffsets[i << 1][1],
				0.0f, 0.0f);
		}
		#endif
		*/
	#endif

	// Render State와 유사하나, 이것은 Direct3D에 설정될 내용이
	// 아니고, i3RenderAttr에 의해 설정되는 것이 아니기 때문에
	// constructor에서 한 번만 설정한다.
	{
		COLORREAL col;

		i3Color::Set( &col, 0.3f, 0.3f, 0.3f, 0.3f);

		setAmbient(  &col);
	}
	
	i3mem::FillZero( &m_Switch, sizeof(m_Switch));
	i3mem::FillZero( m_BoneMatrixArray, sizeof( m_BoneMatrixArray));
}

i3RenderContext::~i3RenderContext(void)
{
	if( m_pShaderCache != nullptr)
	{
		m_pShaderCache->FlushCacheFile();
	}

	i3ShaderCache::ResetShaderSearchPath();

	I3_SAFE_RELEASE( m_texLightProbe);

	I3_SAFE_RELEASE( m_pFontDxManager);
	I3_SAFE_RELEASE( m_pFontDx2Manager);
	I3_SAFE_RELEASE( m_pVertexBank);

	
	I3_SAFE_RELEASE( m_pTimer);
	I3_SAFE_RELEASE( m_pCaps);
	I3_SAFE_RELEASE( m_pPerfReport);
	I3_SAFE_RELEASE( m_pCurShader);
	I3_SAFE_RELEASE( m_pShaderCache);
	I3_SAFE_RELEASE( m_pLastFrameTargetTexture);
	{
		
		i3RenderTarget * pRT;

		for(size_t i = 0; i < m_RenderTargetList.size(); i++)
		{
			pRT = m_RenderTargetList[ i];

			if( pRT->GetColorSurface() != nullptr)
			{
				pRT->GetColorSurface()->Release();
			}

			if( pRT->GetDepthSurface() != nullptr)
			{
				pRT->GetDepthSurface()->Release();
			}

			pRT->Release();
		}
	}

	I3_SAFE_RELEASE_NODBG( m_pSwapChain);
	//
	i3::process_3d_device_exit();
	//

	//I3_SAFE_RELEASE( m_pD3DDevice);
	//::atexit( Delayed_Release_D3D );

	if (m_pD3DDevice)
	{
		int ref_count =	m_pD3DDevice->Release();

		if (ref_count > 0)
		{
			I3TRACE("\n###########################################################\n");
			I3TRACE("Direct3DDevice Ref Count not zero : %d count(s) exist(s).\n", ref_count);
			I3TRACE("###########################################################\n");
		}
		else
		{
			I3TRACE("\n###########################################################\n");
			I3TRACE("Direct3DDevice Removed Safe. \n");
			I3TRACE("###########################################################\n");
		}

		m_pD3DDevice = nullptr;
	}

	if (m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D = nullptr;
	}

	//I3_SAFE_RELEASE( m_pD3D);

	for( size_t i = 0;i < m_WaitProcList.size(); i++)
	{
		WAIT_VSYNC_INFO * pInfo = m_WaitProcList[i];

		I3MEM_SAFE_FREE( pInfo);
	}

	delete m_FullScreenWindowModeHelper;
	

	//RestoreOtherWindows();
}

bool i3RenderContext::FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
    D3DFORMAT Format, UINT32 DepthBits, UINT32 StencilBits, D3DFORMAT* pDepthStencilFormat )
{
	HRESULT Rv;

    if( DepthBits <= 16 && StencilBits == 0 )
    {
		Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 );

        if( SUCCEEDED( Rv ) )
        {
			Rv = m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType, Format, Format, D3DFMT_D16 );
            if( SUCCEEDED( Rv) )
            {
                *pDepthStencilFormat = D3DFMT_D16;
                return true;
            }
        }
    }

    if( DepthBits <= 15 && StencilBits <= 1 )
    {
		Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1 );
        if( SUCCEEDED( Rv) )
        {
			Rv = m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType, Format, Format, D3DFMT_D15S1 );
            if( SUCCEEDED( Rv ) )
            {
                *pDepthStencilFormat = D3DFMT_D15S1;
                return true;
            }
        }
    }

    if( DepthBits <= 24 && StencilBits == 0 )
    {
		Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 );
        if( SUCCEEDED( Rv ) )
        {
			Rv = m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType, Format, Format, D3DFMT_D24X8 );
            if( SUCCEEDED( Rv) )
            {
                *pDepthStencilFormat = D3DFMT_D24X8;
                return true;
            }
        }
    }

    if( DepthBits <= 24 && StencilBits <= 8 )
    {
		Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 );
        if( SUCCEEDED( Rv) )
        {
			m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType, Format, Format, D3DFMT_D24S8 );
            if( SUCCEEDED( Rv) )
            {
                *pDepthStencilFormat = D3DFMT_D24S8;
                return true;
            }
        }
    }

    if( DepthBits <= 24 && StencilBits <= 4 )
    {
		Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4 );
        if( SUCCEEDED( Rv) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                Format, Format, D3DFMT_D24X4S4 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24X4S4;
                return true;
            }
        }
    }

    if( DepthBits <= 32 && StencilBits == 0 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                Format, Format, D3DFMT_D32 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D32;
                return true;
            }
        }
    }

    return false;
}

bool i3RenderContext::IsCompressedTextureFormatOk( D3DFORMAT AdapterFormat )
{
	HRESULT hr;

	hr = g_pD3D->CheckDeviceFormat( g_iCurrentAdapterIndex,
                                    D3DDEVTYPE_HAL,
                                    AdapterFormat,
                                    0,
                                    D3DRTYPE_TEXTURE,
                                    D3DFMT_DXT1);
	if( FAILED( hr ))	return false;

	hr = g_pD3D->CheckDeviceFormat( g_iCurrentAdapterIndex,
                                    D3DDEVTYPE_HAL,
                                    AdapterFormat,
                                    0,
                                    D3DRTYPE_TEXTURE,
                                    D3DFMT_DXT2);
	if( FAILED( hr ))	return false;

	hr = g_pD3D->CheckDeviceFormat( g_iCurrentAdapterIndex,
                                    D3DDEVTYPE_HAL,
                                    AdapterFormat,
                                    0,
                                    D3DRTYPE_TEXTURE,
                                    D3DFMT_DXT3);
	if( FAILED( hr ))	return false;

	hr = g_pD3D->CheckDeviceFormat( g_iCurrentAdapterIndex,
                                    D3DDEVTYPE_HAL,
                                    AdapterFormat,
                                    0,
                                    D3DRTYPE_TEXTURE,
                                    D3DFMT_DXT4);
	if( FAILED( hr ))	return false;

	hr = g_pD3D->CheckDeviceFormat( g_iCurrentAdapterIndex,
                                    D3DDEVTYPE_HAL,
                                    AdapterFormat,
                                    0,
                                    D3DRTYPE_TEXTURE,
                                    D3DFMT_DXT5);
	if( FAILED( hr ))	return false;

	return true;
}


// 

bool i3RenderContext::FindAppropriateVideoInfo( i3VideoInfo * pWantInfo, i3VideoInfo * pFindInfo)
{
	UINT32  j;
	D3DDISPLAYMODE Mode;
	D3DFORMAT		Format;
	INT32		diff, minDiff;
	INT32		idxFind = -1;
	
//	INT32 AdapterIdx = -1;
//	if( pWantInfo->m_AdapterIndex == -1)
//	{
//		AdapterIdx = _GetMonitorFromWindow( m_hWnd, &hCurrMonitor);
//	}

	HMONITOR hCurrMonitor =	i3RenderContext::FindCurrentMonitorHandle();
	INT32 AdapterIdx = i3RenderContext::FindAdapterIndex(hCurrMonitor);
		
	g_iCurrentAdapterIndex = AdapterIdx;		// 그냥 여기서 변경..
	pWantInfo->m_AdapterIndex = AdapterIdx;

	Format = i3DXUT_GetNativeImageFormat( pWantInfo->m_RenderTargetFormat);

	minDiff = 0x7FFFFFFF;

	HRESULT result_enum = D3DERR_NOTAVAILABLE;

	for( j = 0; j < m_pD3D->GetAdapterModeCount( pWantInfo->m_AdapterIndex, Format); j++)
	{
		result_enum = m_pD3D->EnumAdapterModes( pWantInfo->m_AdapterIndex, Format, j, &Mode);

		if (result_enum != D3D_OK)
			continue;

		// 求 滿捉동 見 磯.
		diff = abs(pWantInfo->m_Width - (INT32) Mode.Width);
		diff += abs(pWantInfo->m_Height - (INT32) Mode.Height);

		if( pWantInfo->m_bFullScreen)
			diff += abs( (INT32) (pWantInfo->m_RefreshRate - Mode.RefreshRate) );

		if( Format != Mode.Format)
			diff += 1;

		if( diff < minDiff)
		{
			minDiff = diff;
			idxFind = j;
		}
	}

	if (idxFind == -1)
	{
		I3PRINTLOG(I3LOG_NOTICE, "Can't Find AppropriateVideoInfo : %dX%d in Adapter No%0d", 
			pWantInfo->m_Width, pWantInfo->m_Height, pWantInfo->m_AdapterIndex);
		return false;
	}
	

	{
		m_pD3D->EnumAdapterModes( pWantInfo->m_AdapterIndex, Format, idxFind, &Mode);

		I3PRINTLOG(I3LOG_NOTICE, "AppropriateVideoInfo Found : %dX%d into %dX%d in Adapter No%0d ( %d Hz )", 
			pWantInfo->m_Width, pWantInfo->m_Height, Mode.Width, Mode.Height, pWantInfo->m_AdapterIndex, Mode.RefreshRate);
	
//		I3TRACE( "FIND MODE : %dx%d %dHz\n", Mode.Width, Mode.Height, Mode.RefreshRate);

		pFindInfo->m_AdapterIndex			= AdapterIdx;				//	pWantInfo->m_AdapterIndex;
		pFindInfo->m_ZBufferBits			= pWantInfo->m_ZBufferBits;
		pFindInfo->m_StencilBufferBits		= pWantInfo->m_StencilBufferBits;
		pFindInfo->m_bFullScreen			= pWantInfo->m_bFullScreen;
		pFindInfo->m_bInterlaced			= pWantInfo->m_bInterlaced;
		pFindInfo->m_FrameBufferCount		= pWantInfo->m_FrameBufferCount;
		pFindInfo->m_bHalfSizeFrameBuffer	= pWantInfo->m_bHalfSizeFrameBuffer;
		pFindInfo->m_VideoType				= pWantInfo->m_VideoType;
		pFindInfo->m_bMultiThread			= pWantInfo->m_bMultiThread;
		pFindInfo->m_Flags					= pWantInfo->m_Flags;
		pFindInfo->m_MultiSampling			= pWantInfo->m_MultiSampling;
		pFindInfo->m_MultiSamplingQuality	= pWantInfo->m_MultiSamplingQuality;

		pFindInfo->m_ModeIndex				= idxFind;

		if( pFindInfo->m_bFullScreen)
		{
			pFindInfo->m_Width					= Mode.Width;
			pFindInfo->m_Height					= Mode.Height;
			pFindInfo->m_RenderTargetFormat		= i3DXUT_GetI3ImageFormat( Mode.Format);
			pFindInfo->m_RefreshRate			= (REAL32) Mode.RefreshRate;
		}
		else
		{
			pFindInfo->m_Width					= pWantInfo->m_Width;
			pFindInfo->m_Height					= pWantInfo->m_Height;
			pFindInfo->m_RenderTargetFormat		= pWantInfo->m_RenderTargetFormat;
			pFindInfo->m_RefreshRate			= pWantInfo->m_RefreshRate;
		}

	}

	return true;
}

void i3RenderContext::GetCurrentVideoInfo( i3VideoInfo * pInfo)
{
	HRESULT Rv;
	D3DDISPLAYMODE Mode;

//	if( pInfo->m_AdapterIndex == -1)
//	{
//		pInfo->m_AdapterIndex = _GetMonitorFromWindow( m_hWnd);
//	}
	
	HMONITOR hMon = i3RenderContext::FindCurrentMonitorHandle();
	pInfo->m_AdapterIndex = i3RenderContext::FindAdapterIndex(hMon);
	
	if (pInfo->m_AdapterIndex == -1)
	{
		pInfo->m_ModeIndex = -1;
		return;
	}

	Rv = m_pD3D->GetAdapterDisplayMode( pInfo->m_AdapterIndex, &Mode);
	if( FAILED( Rv))
	{
		pInfo->m_AdapterIndex = -1;
		pInfo->m_ModeIndex = -1;
		return;
	}

	pInfo->m_ModeIndex = 0;					// ?? 아무래도 윈도우 모드에서만 0으로 놓는 모양인데...함수 구성이 좋지 않은듯?? 풀스크린이면 이 함수를 쓰지 말것..
	//pInfo->m_Width = Mode.Width;
	//pInfo->m_Height = Mode.Height;
	pInfo->m_RefreshRate = (REAL32) Mode.RefreshRate;

	if( (pInfo->m_RenderTargetFormat == I3G_IMAGE_FORMAT_NONE)
		|| ( pInfo->m_RenderTargetFormat == I3G_IMAGE_FORMAT_DEFAULT_COLOR))
	{
		pInfo->m_RenderTargetFormat = i3DXUT_GetI3ImageFormat( Mode.Format);
	}
}

static ADJUSTGFXCONFIGPROC		s_pConfigProc = nullptr;

void i3RenderContext::setConfigProc( ADJUSTGFXCONFIGPROC pProc)
{
	s_pConfigProc  = pProc;
}

static HINSTANCE GetRenderContextModuleInstance()
{
	HINSTANCE hInst = nullptr;

	if(!::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
		(LPCSTR)GetRenderContextModuleInstance, (HMODULE *)&hInst))
		return nullptr;

	return hInst;
}

void i3RenderContext::getRenderContextModulePath(char* path, INT32 sizBuf)
{
	if(!path)
		return;

	HINSTANCE hinst = GetRenderContextModuleInstance();

	if(hinst)
	{
		GetModuleFileName(hinst, path, sizBuf);

		char* p = path;
		while(*p)
		{
			if(*p == '\\')
				*p = '/';

			p++;
		}
		
		p = strrchr(path, '/');
		if (p != nullptr)
			*p = 0;
	}
}

bool i3RenderContext::Create( i3VideoInfo * pVFormat, void * pSetupParam)
{
	char szExecPath[MAX_PATH], szTemp[MAX_PATH];

	if(m_doFindShaderFromModulePath)
	{
		getRenderContextModulePath(szTemp, MAX_PATH);

		strcat_s(szTemp, MAX_PATH, "/Shader");
		i3ShaderCache::AddShaderSearchPath(szTemp);
	}

#if defined( I3_DEBUG)
	i3ShaderCache::AddShaderSearchPath( "$(I3_ROOT)/Shader");
	//i3ShaderCache::AddShaderSearchPath( "E:\\Zepetto\\Project\\PointBlank_15\\Trunk\\i3\\Shader");
#endif

	{
		::GetCurrentDirectory( sizeof(szExecPath) - 1, szExecPath);

		sprintf( szTemp, "%s/Shader", szExecPath);
		i3ShaderCache::AddShaderSearchPath( szTemp);

	//	i3System::GetExecPath( szExecPath);
		i3::get_exec_path(szExecPath);

		sprintf( szTemp, "%s/Shader", szExecPath);
		i3ShaderCache::AddShaderSearchPath( szTemp);
		//i3ShaderCache::AddShaderSearchPath( szExecPath);

		IShaderSourceCodeMgr* shaderCodeMgr = GetShaderSourceCodeMgr();
		if (::strcmp(shaderCodeMgr->GetName(), "ResourcePackShader") == 0)
			shaderCodeMgr->Load("/Pack/HLSL.i3Pack");
	}

	m_pSetupParam = pSetupParam;

	m_hWnd = (HWND) pSetupParam;

	{
		m_Switch.m_WindowStyle		= ::GetWindowLong( m_hWnd, GWL_STYLE);
		m_Switch.m_WindowExStyle	= ::GetWindowLong( m_hWnd, GWL_EXSTYLE);

		RECT rt;

		::GetWindowRect( m_hWnd, &rt);
		s_svX = rt.left;
		s_svY = rt.top;
		s_svWidth = rt.right - rt.left;
		s_svHeight = rt.bottom - rt.top;
	}

	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION);
	if( m_pD3D == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Hardware test Error:: Could not create a Direct3D object.");
		DebugBox( m_hWnd, "Could not create a Direct3D object.");
		return false;
	}
	
	char conv[1024];

	m_CurHMonitor = FindCurrentMonitorHandle();
	g_iCurrentAdapterIndex = FindAdapterIndex(m_CurHMonitor);

	if (m_CurHMonitor == nullptr)
	{
		sprintf(conv, "Hmonitor == nullptr");
		I3PRINTLOG(I3LOG_FATAL, conv);
	}

	sprintf(conv, "g_iCurrentAdapterIndex = %d", g_iCurrentAdapterIndex);
	I3PRINTLOG(I3LOG_NOTICE, conv);

	{
		D3DCAPS9 caps;
		UINT32	major, minor;

		m_pD3D->GetDeviceCaps( g_iCurrentAdapterIndex, D3DDEVTYPE_HAL, &caps);

		major = (caps.PixelShaderVersion >> 8) & 0xFF;
		minor = (caps.PixelShaderVersion) & 0xFF;

		if((major == 0))
		{
			D3DADAPTER_IDENTIFIER9	adapter;

			m_pD3D->GetAdapterIdentifier( g_iCurrentAdapterIndex, 0, &adapter);

			sprintf( conv, "Graphic Device does not support PixelShader: %s",	adapter.Description );

			I3PRINTLOG(I3LOG_FATAL,  conv);
			MessageBox( m_hWnd, conv, "", MB_OK);

			return false;
		}
	}

	g_pD3D = m_pD3D;

	{
		// Shader Cache  치 .
		_FindShaderWorkDir( m_szShaderCachePath);

		i3Shader::SetWorkDir( m_szShaderCachePath);

		_LoadShaderCache( m_szShaderCachePath);
	}

	if( s_pConfigProc != nullptr)
	{
		s_pConfigProc( pVFormat);
	}
	
	pVFormat->m_AdapterIndex = g_iCurrentAdapterIndex;

	if( !ChangeVideoInfo( pVFormat))
	{
		return false;
	}

	m_pTimer = i3Timer::new_object();

	// Performance Report
	{
		m_pPerfReport = i3GfxPerformanceReport::new_object();
		I3ASSERT( m_pPerfReport != nullptr);
	}

    // The app is ready to go
	m_RenderFlag |= I3RENDER_FLAG_READY;

	// Vertex Bank 
	{
		m_pVertexBank = i3GfxVertexBank::new_object();
	}

	#if defined( _LIGHT_PROBE_SHADING_)
	{
		i3ImageFile file;
		char conv[MAX_PATH];

		//sprintf( conv, "%s/lightProbe_Uffzi.dds", m_szShaderCachePath);
		//sprintf( conv, "%s/lightProbe_SuperIntense.dds", m_szShaderCachePath);
		//sprintf( conv, "%s/lightProbe_RnL.dds", m_szShaderCachePath);
		//sprintf( conv, "%s/lightProbe_RedStorm.dds", m_szShaderCachePath);
		//sprintf( conv, "%s/lightProbe_Kitchen.dds", m_szShaderCachePath);
		//sprintf( conv, "%s/lightProbe_Grace.dds", m_szShaderCachePath);
		//sprintf( conv, "%s/lightProbe_Galileo.dds", m_szShaderCachePath);
		//sprintf( conv, "%s/lightProbe_Campus.dds", m_szShaderCachePath);
		//sprintf( conv, "%s/lightProbe_Building.dds", m_szShaderCachePath);
		sprintf( conv, "%s/lightProbe_Beach.dds", m_szShaderCachePath);

		m_texLightProbe = file.Load( conv);

		I3_SAFE_ADDREF( m_texLightProbe);
	}
	#endif

	return true;
}

bool i3RenderContext::CanBeReset( i3VideoInfo * pInfo)
{
	if( m_pD3DDevice == nullptr)
		return false;

	if( m_CurVideoInfo.m_AdapterIndex != pInfo->m_AdapterIndex)
		return false;

	return true;
}


void i3RenderContext::Process_WM_ACTIVATE(bool isActive)
{
	if (m_FullScreenWindowModeHelper)
		m_FullScreenWindowModeHelper->Process_WM_ACTIVATE(isActive);
}


enum CVI_CASE
{
	CVI_CASE_W2W = 0,
	CVI_CASE_W2F,
	CVI_CASE_F2W,
	CVI_CASE_F2F
};

bool i3RenderContext::ChangeVideoInfo( i3VideoInfo * pInfo, bool bForceRecreate)
{
	if (m_FullScreenWindowModeHelper)
		m_FullScreenWindowModeHelper->Lock_WM_ACTIVATE();
	
	bool					bDepthBuffer;
	D3DFORMAT				DepthFormat = (D3DFORMAT) 0;
	bool					bRv;
	bool bNeedToCreate = false;
	CVI_CASE				change_case;

//	m_CurVideoInfo = *pInfo;

	m_Switch.m_bIgnoreResize = true;

	HMONITOR hCurMonitor = i3RenderContext::FindCurrentMonitorHandle();
	INT32    currAdapterIndex = i3RenderContext::FindAdapterIndex(hCurMonitor);

	if ( currAdapterIndex != pInfo->m_AdapterIndex || m_CurHMonitor != hCurMonitor)			// 모니터 케이블을 뽑았거나 하는 이상상태..
	{
		pInfo->m_AdapterIndex = currAdapterIndex;
		m_CurHMonitor = hCurMonitor;

		//if (pInfo->m_bFullScreen)
		//{
		//	if (m_pD3D)
		//		m_pD3D->Release();
		//	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION);		// 재생성..
		//	g_pD3D = m_pD3D;
		//	bNeedToCreate = true;

		//	I3PRINTLOG(I3LOG_NOTICE, "Recreate IDirect3D9 Object... because of HMONITOR Change in FullScreenMode");
		//}
	}
	
	g_iCurrentAdapterIndex = currAdapterIndex;		// 그냥 여기서 변경..

	if( m_pD3DDevice == nullptr)
	{
		bNeedToCreate = true;
	}
	else
	{
		//win8이상에서 전체모드에서 창모드로 전환시 크래시현상발생하여 해당내용 주석처리
		// Window Mode -> Fullscreen
		// Fullscreen -> Window Mode
		//if( m_CurVideoInfo.m_bFullScreen != pInfo->m_bFullScreen)
		//	bNeedToCreate = true;
	}

	bNeedToCreate = bNeedToCreate || bForceRecreate;

//	if( m_d3dpp.Windowed == FALSE)
	if ( m_CurVideoInfo.m_bFullScreen )
	{
		if( pInfo->m_bFullScreen)
			change_case = CVI_CASE_F2F;
		else
			change_case = CVI_CASE_F2W;
	}
	else
	{
		if( pInfo->m_bFullScreen)
			change_case = CVI_CASE_W2F;
		else
			change_case = CVI_CASE_W2W;
	}

//	bool bFullScreenWindowModeChanged = false;
//	if (m_CurVideoInfo.m_bFullScreenWindowModeEnable != pInfo->m_bFullScreenWindowModeEnable)
//	{
//		bFullScreenWindowModeChanged = true;
//	}

	m_CurVideoInfo.m_AdapterIndex			= pInfo->m_AdapterIndex;
	m_CurVideoInfo.m_ModeIndex				= pInfo->m_ModeIndex;
	m_CurVideoInfo.m_Width					= pInfo->m_Width;
	m_CurVideoInfo.m_Height					= pInfo->m_Height;
	m_CurVideoInfo.m_RenderTargetFormat		= pInfo->m_RenderTargetFormat;
	m_CurVideoInfo.m_ZBufferBits			= pInfo->m_ZBufferBits;
	m_CurVideoInfo.m_StencilBufferBits		= pInfo->m_StencilBufferBits;
	m_CurVideoInfo.m_bFullScreen			= pInfo->m_bFullScreen;
	m_CurVideoInfo.m_bInterlaced			= pInfo->m_bInterlaced;
	m_CurVideoInfo.m_RefreshRate			= pInfo->m_RefreshRate;
	m_CurVideoInfo.m_FrameBufferCount		= pInfo->m_FrameBufferCount;
	m_CurVideoInfo.m_bHalfSizeFrameBuffer	= pInfo->m_bHalfSizeFrameBuffer;
	m_CurVideoInfo.m_VideoType				= pInfo->m_VideoType;
	m_CurVideoInfo.m_bMultiThread			= pInfo->m_bMultiThread;
	m_CurVideoInfo.m_Flags					= pInfo->m_Flags;
	m_CurVideoInfo.m_MultiSampling			=	pInfo->m_MultiSampling;
	m_CurVideoInfo.m_MultiSamplingQuality	= pInfo->m_MultiSamplingQuality;
	m_CurVideoInfo.m_bVSync					= pInfo->m_bVSync;

	//
	m_CurVideoInfo.m_bFullScreenWindowModeEnable = pInfo->m_bFullScreenWindowModeEnable;
	//

	_SetCurrentViewportRect( 0, 0, pInfo->m_Width, pInfo->m_Height);

	if( pInfo->m_bFullScreen)
	{
		bool bResultFindVideoInfo = FindAppropriateVideoInfo( pInfo, &m_CurVideoInfo);
		
		if (bResultFindVideoInfo == false)
		{
			I3PRINTLOG(I3LOG_NOTICE, "Failed To Find VideoInfo From FindAppropriateVideoInfo : %dX%d", pInfo->m_Width, pInfo->m_Height);
		}

		if (pInfo->m_Width != m_CurVideoInfo.m_Width || 
			pInfo->m_Height != m_CurVideoInfo.m_Height )		// 다른 해상도로 잡힌 경우.. _SetCurrentViewportRect재호출..
		{
			_SetCurrentViewportRect( 0, 0, m_CurVideoInfo.m_Width, m_CurVideoInfo.m_Height);
		}														 

		DWORD removeStyle = WS_VISIBLE | WS_CAPTION | WS_BORDER | WS_DLGFRAME | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

		::SetWindowLong( m_hWnd, GWL_STYLE, m_Switch.m_WindowStyle & ~removeStyle);
		::SetWindowLong( m_hWnd, GWL_EXSTYLE, m_Switch.m_WindowExStyle | WS_EX_TOPMOST);

		{
			WINDOWPLACEMENT info;
			info.length = sizeof(WINDOWPLACEMENT);
			::GetWindowPlacement( m_hWnd, &info);
			info.showCmd = SW_MAXIMIZE;
			//::SetWindowPlacement( m_hWnd, &info);
		}

		if (pInfo->m_bFullScreenWindowModeEnable == false)
		{
			if (m_FullScreenWindowModeHelper)
			{
				delete m_FullScreenWindowModeHelper;
				m_FullScreenWindowModeHelper = NULL;
			}
		}
	}
	else
	{
		GetCurrentVideoInfo( &m_CurVideoInfo);

		::SetWindowLong( m_hWnd, GWL_STYLE, m_Switch.m_WindowStyle & ~WS_VISIBLE);
		::SetWindowLong( m_hWnd, GWL_EXSTYLE, m_Switch.m_WindowExStyle & ~(WS_EX_TOPMOST));

		{
			WINDOWPLACEMENT info;

			info.length = sizeof(WINDOWPLACEMENT);
			::GetWindowPlacement( m_hWnd, &info);

			info.showCmd = SW_RESTORE;

			//::SetWindowPlacement( m_hWnd, &info);
		}

		if (m_FullScreenWindowModeHelper)
		{
			delete m_FullScreenWindowModeHelper;
			m_FullScreenWindowModeHelper = NULL;
		}
			
	}

	if( pInfo->m_ZBufferBits > 0)
	{
		bDepthBuffer = true;

		if( pInfo->m_StencilBufferBits != 0)
		{
			switch( pInfo->m_ZBufferBits)
			{
				case 16 :
					// DirectX 8.1 Depth:16 bits, Stencil:1 結 Format  苛쨈.
					DepthFormat = D3DFMT_D15S1;
					break;

				case 24 :
					if( pInfo->m_StencilBufferBits == 4)
						DepthFormat = D3DFMT_D24X4S4;
					else
						DepthFormat = D3DFMT_D24S8;
					break;

				case 32 :
					// 32bits Depth buffer Stencil  苛쨈.
					DepthFormat = D3DFMT_D32;
					break;
			}
		}
		else
		{
			switch( pInfo->m_ZBufferBits)
			{
				case 16 :	DepthFormat = D3DFMT_D16;	break;
				case 24 :	DepthFormat = D3DFMT_D24X8;	break;
				case 32 :	DepthFormat = D3DFMT_D32;	break;
			}
		}
	}

	if ( m_CurVideoInfo.m_bFullScreenWindowModeEnable ) //	m_FullScreenWindowModeHelper ) //	m_hFrameWnd_FullScreenEmulation != 0)
	{
		m_d3dpp.Windowed = TRUE;
	}
	else
	{
		m_d3dpp.Windowed = !m_CurVideoInfo.m_bFullScreen;
	}

	m_d3dpp.BackBufferWidth					= m_CurVideoInfo.m_Width;
	m_d3dpp.BackBufferHeight				= m_CurVideoInfo.m_Height;
	m_d3dpp.BackBufferFormat				= i3DXUT_GetNativeImageFormat( m_CurVideoInfo.m_RenderTargetFormat);
	/*m_d3dpp.BackBufferCount					= m_CurVideoInfo.m_FrameBufferCount - 1;
	if( m_d3dpp.BackBufferCount < 1)*/
	m_d3dpp.BackBufferCount = 1;			// 생성시에는 2개의 백버퍼를 생성해야 한다. (Windows8.1에서 창모드 변경 문제)
	m_d3dpp.hDeviceWindow					= m_hWnd;	
	m_d3dpp.EnableAutoDepthStencil			= TRUE;
	m_d3dpp.AutoDepthStencilFormat			= DepthFormat;	

	if( IsRenderFlag(I3RENDER_FLAG_DLGBOXMODE_ENABLE) && m_d3dpp.Windowed == false)
	{
		m_d3dpp.MultiSampleType					= D3DMULTISAMPLE_NONE;
		m_d3dpp.MultiSampleQuality				= 0;
		m_d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
		m_d3dpp.Flags							= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	}
	else
	{
		m_d3dpp.MultiSampleType					= (D3DMULTISAMPLE_TYPE) m_CurVideoInfo.m_MultiSampling;
		m_d3dpp.MultiSampleQuality				= m_CurVideoInfo.m_MultiSamplingQuality;
		m_d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
		m_d3dpp.Flags							= 0;
	}

	if( m_d3dpp.Windowed == false)
	{
		m_d3dpp.FullScreen_RefreshRateInHz		= (INT32) m_CurVideoInfo.m_RefreshRate;
	}
	else
	{
		m_d3dpp.FullScreen_RefreshRateInHz		= 0;
	}

	//MinimizeOtherWindows();

	if( pInfo->m_bVSync)
		m_d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_ONE;
	else
		m_d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;

	if( bNeedToCreate)
	{
		bRv = _CreateDevice();

		if (bRv == false && m_CurVideoInfo.m_bFullScreen != FALSE &&
			m_CurVideoInfo.m_bFullScreenWindowModeEnable == false) // 일반적인 풀스크린 모드 실패시, 풀스크린 윈도우 모드로 전환한 뒤 재시도처리한다...
		{
			m_CurVideoInfo.m_bFullScreenWindowModeEnable = true;
			return this->ChangeVideoInfo(&m_CurVideoInfo, false);			  // 디바이스재생성을 강제합니다.. 리턴합니다..
		}
		
		if( bRv == false && IsRenderFlag( I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) == false 
			&& m_CurHMonitor == hCurMonitor )		// 모니터가 갑자기 변경된 경우는 제외시킨다..
		{
			m_d3dpp.BackBufferWidth = 1024;
			m_d3dpp.BackBufferHeight = 768;
			m_d3dpp.FullScreen_RefreshRateInHz = 60;
			m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
			m_d3dpp.MultiSampleQuality = 0;

			m_CurVideoInfo.m_Width = 1024;
			m_CurVideoInfo.m_Height = 768;
			m_CurVideoInfo.m_RefreshRate = 60;
			m_CurVideoInfo.m_MultiSampling = 0;
			m_CurVideoInfo.m_MultiSamplingQuality = 0;

			if( m_d3dpp.Windowed)
			{
				m_d3dpp.FullScreen_RefreshRateInHz = 0;
				m_CurVideoInfo.m_RefreshRate = 0;
			}

			bRv = _CreateDevice();

			if (bRv == true)
			{
				_SetCurrentViewportRect( 0, 0, m_CurVideoInfo.m_Width, m_CurVideoInfo.m_Height);
			}
		}		
		
		if( bRv == true)
		{
			// Dialog Box Mode일경우.. D3D상에서.. API Dialog박스등을 사용할 수 있음.
			if( m_d3dpp.Windowed == FALSE && IsRenderFlag(I3RENDER_FLAG_DLGBOXMODE_ENABLE))
				m_pD3DDevice->SetDialogBoxMode(TRUE);

			OnRecreate();
		}
	}
	else
	{
		bRv = OnResetDevice();

		if (bRv == false)
		{
			if (m_CurVideoInfo.m_bFullScreen != FALSE &&
				m_CurVideoInfo.m_bFullScreenWindowModeEnable == false) // 일반적인 풀스크린 모드 실패시, 풀스크린 윈도우 모드로 전환한 뒤 재시도처리한다...
			{
				m_CurVideoInfo.m_bFullScreenWindowModeEnable = true;
				return this->ChangeVideoInfo(&m_CurVideoInfo, false);			  // 디바이스재생성을 강제하지 않습니다... 리턴합니다..
			}
		}

	}

	m_Switch.m_bIgnoreResize = false;

	if( m_d3dpp.Windowed)
	{
		//::ShowWindow( m_hWnd, SW_RESTORE);
	
		//::SetWindowLong( m_hWnd, GWL_STYLE, s_svStyle & ~WS_VISIBLE);

		{
			MSG msg;

			// 원인은 알 수 없으나, 아래의 SetWindowPos 함수로 설정한 좌표가
			// 이전에 Post된 Message에 의해 무효화된다.
			// 이것을 방지하기 위해, SetWindowPos() 함수를 호출하기 전에
			// Queueing된 다른 Window Message들을 모두 처리해준다.

			for( ;PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );)
			{
				// Translate and dispatch the message
				if( 0 == TranslateAccelerator( m_hWnd, NULL, &msg ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
		}

		bool bResizeWindow = false;

		if( change_case == CVI_CASE_F2W)
			bResizeWindow = true;

		if( (change_case == CVI_CASE_W2W) && ( GetParent( m_hWnd) == nullptr))
			bResizeWindow = true;

		if ( m_CurVideoInfo.m_bFullScreenWindowModeEnable && m_CurVideoInfo.m_bFullScreen )
		{

			if (m_FullScreenWindowModeHelper == NULL) // 
			{
				m_FullScreenWindowModeHelper = new i3FullScreenWindowModeHelper(this);
				m_FullScreenWindowModeHelper->Lock_WM_ACTIVATE();
			}

			bool bNeedToCreateDevice_FullscreenWindowMode = false;

			m_FullScreenWindowModeHelper->CreateFullScreenWindow(bNeedToCreateDevice_FullscreenWindowMode);

			//	if (bNeedToCreateDevice_FullscreenWindowMode)
			//	{
			//		bNeedToCreate = true;
			//	}
				/*
				m_CurHMonitor = hPrimaryMon;
				*/

			m_FullScreenWindowModeHelper->ResizeAndMoveWindow();

			m_CurHMonitor = i3RenderContext::FindCurrentMonitorHandle();
					
			bResizeWindow = true;
		}
		else
		{
			if (bResizeWindow && IsRenderFlag(I3RENDER_FLAG_ADJUST_WINDOWPOS))
			{
				// if (m_FullScreenWindowModeHelper == NULL)
				{
					RECT Rect;
					BOOL bMenu;
					INT32 cx, cy, scx, scy, x, y;

					Rect.left = 0;
					Rect.top = 0;
					Rect.right = pInfo->m_Width;
					Rect.bottom = pInfo->m_Height;

					bMenu = (GetMenu(m_hWnd) != NULL);

					AdjustWindowRect(&Rect, m_Switch.m_WindowStyle, bMenu);

					cx = Rect.right - Rect.left;
					cy = Rect.bottom - Rect.top;

					scx = ::GetSystemMetrics(SM_CXFULLSCREEN);
					scy = ::GetSystemMetrics(SM_CYFULLSCREEN);

					if (::GetParent(m_hWnd) == NULL)
					{
						x = max(0, (scx >> 1) - (cx >> 1));
						y = max(0, (scy >> 1) - (cy >> 1));
					}
					else
					{
						x = y = 0;
					}

					::SetWindowPos(m_hWnd, HWND_NOTOPMOST, x, y, cx, cy, SWP_NOACTIVATE | SWP_FRAMECHANGED);
					m_CurHMonitor = i3RenderContext::FindCurrentMonitorHandle();
				}

			}


		}
	}
	else
	{
		{
			MSG msg;

			// 원인은 알 수 없으나, 아래의 SetWindowPos 함수로 설정한 좌표가
			// 이전에 Post된 Message에 의해 무효화된다.
			// 이것을 방지하기 위해, SetWindowPos() 함수를 호출하기 전에
			// Queueing된 다른 Window Message들을 모두 처리해준다.

			for( ;PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );)
			{
				// Translate and dispatch the message
				if( 0 == TranslateAccelerator( m_hWnd, NULL, &msg ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
		}

		::SetWindowPos( m_hWnd, HWND_TOP, 0, 0, m_CurVideoInfo.m_Width, m_CurVideoInfo.m_Height, SWP_NOACTIVATE | SWP_NOMOVE);
	}
	
	::ShowWindow( m_hWnd, SW_SHOW);	

	if (m_FullScreenWindowModeHelper)
		m_FullScreenWindowModeHelper->Unlock_WM_ACTIVATE();

	return bRv;
}

void	i3RenderContext::modifyMouseCursorPosition(POINT& inout_point)
{
	if (m_FullScreenWindowModeHelper)
		m_FullScreenWindowModeHelper->modifyMouseCursorPosition(inout_point);
}


#define MAX_DEVICE_TYPE	9

struct RENDER_DEVICE_ARRAY
{
	D3DDEVTYPE	DeviceType = D3DDEVTYPE_HAL;
	DWORD		dwBehavior = 0;
};

#if defined( I3G_DX)
void i3RenderContext::Print_D3DPRESENT_PARAMETERS(D3DPRESENT_PARAMETERS& d3dpp)
{
	I3PRINTLOG(I3LOG_NOTICE, "BackBufferWidth %d", d3dpp.BackBufferWidth);
	I3PRINTLOG(I3LOG_NOTICE, "BackBufferHeight %d", d3dpp.BackBufferHeight);
	I3PRINTLOG(I3LOG_NOTICE, "BackBufferFormat %d", d3dpp.BackBufferFormat);
	I3PRINTLOG(I3LOG_NOTICE, "BackBufferCount %d", d3dpp.BackBufferCount);
	I3PRINTLOG(I3LOG_NOTICE, "MultiSampleType %d", d3dpp.MultiSampleType);
	I3PRINTLOG(I3LOG_NOTICE, "MultiSampleQuality %d", d3dpp.MultiSampleQuality);
	I3PRINTLOG(I3LOG_NOTICE, "SwapEffect %d", d3dpp.SwapEffect);
	I3PRINTLOG(I3LOG_NOTICE, "hDeviceWindow %d", d3dpp.hDeviceWindow);
	I3PRINTLOG(I3LOG_NOTICE, "Windowed %d", d3dpp.Windowed);
	I3PRINTLOG(I3LOG_NOTICE, "EnableAutoDepthStencil %d", d3dpp.EnableAutoDepthStencil);
	I3PRINTLOG(I3LOG_NOTICE, "AutoDepthStencilFormat %d", d3dpp.AutoDepthStencilFormat);
	I3PRINTLOG(I3LOG_NOTICE, "Flags %d", d3dpp.Flags);
	I3PRINTLOG(I3LOG_NOTICE, "FullScreen_RefreshRateInHz %d", d3dpp.FullScreen_RefreshRateInHz);
	I3PRINTLOG(I3LOG_NOTICE, "PresentationInterval %d", d3dpp.PresentationInterval);
}
#endif

bool i3RenderContext::_CreateDevice(void)
{
	INT32 i = 0;
	HRESULT Rv;
	UINT	adapterIdx = m_CurVideoInfo.m_AdapterIndex;
	RENDER_DEVICE_ARRAY deviceArr[MAX_DEVICE_TYPE];
	char szTemp1[1024] = {0,};
	char szTemp2[1024] = {0,};

	// Deviec Type Array Init
	{
		if (m_bUseDoubleCalcul)
		{
			// HAL
			deviceArr[0].DeviceType = D3DDEVTYPE_HAL;
			deviceArr[0].dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
			deviceArr[1].DeviceType = D3DDEVTYPE_HAL;
			deviceArr[1].dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
			deviceArr[2].DeviceType = D3DDEVTYPE_HAL;
			deviceArr[2].dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;

			// SW
			deviceArr[3].DeviceType = D3DDEVTYPE_SW;
			deviceArr[3].dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
			deviceArr[4].DeviceType = D3DDEVTYPE_SW;
			deviceArr[4].dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
			deviceArr[5].DeviceType = D3DDEVTYPE_SW;
			deviceArr[5].dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;

			// REF
			deviceArr[6].DeviceType = D3DDEVTYPE_REF;
			deviceArr[6].dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
			deviceArr[7].DeviceType = D3DDEVTYPE_REF;
			deviceArr[7].dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
			deviceArr[8].DeviceType = D3DDEVTYPE_REF;
			deviceArr[8].dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
		}
		else
		{
			// HAL
			deviceArr[0].DeviceType = D3DDEVTYPE_HAL;
			deviceArr[0].dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			deviceArr[1].DeviceType = D3DDEVTYPE_HAL;
			deviceArr[1].dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
			deviceArr[2].DeviceType = D3DDEVTYPE_HAL;
			deviceArr[2].dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

			// SW
			deviceArr[3].DeviceType = D3DDEVTYPE_SW;
			deviceArr[3].dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
			deviceArr[4].DeviceType = D3DDEVTYPE_SW;
			deviceArr[4].dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			deviceArr[5].DeviceType = D3DDEVTYPE_SW;
			deviceArr[5].dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

			// REF
			deviceArr[6].DeviceType = D3DDEVTYPE_REF;
			deviceArr[6].dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
			deviceArr[7].DeviceType = D3DDEVTYPE_REF;
			deviceArr[7].dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			deviceArr[8].DeviceType = D3DDEVTYPE_REF;
			deviceArr[8].dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		
	}
	
//	I3_SAFE_RELEASE_NODBG( m_pD3DDevice);
	if (m_pD3DDevice)
	{
		I3PRINTLOG(I3LOG_NOTICE, "i3RenderContext::_InvalidateGfxResource() Enter");
		_InvalidateGfxResource(true);
		I3PRINTLOG(I3LOG_NOTICE, "i3RenderContext::_InvalidateGfxResource() Leave");

		int ref_count =	m_pD3DDevice->Release();

		if (ref_count > 0)
		{
			I3TRACE("\n###########################################################\n");
			I3TRACE("Direct3DDevice Ref Count not zero : %d count(s) exist(s).\n", ref_count);
			I3TRACE("###########################################################\n");
		}
		else
		{
			I3TRACE("\n###########################################################\n");
			I3TRACE("Direct3DDevice Removed Safe. \n");
			I3TRACE("###########################################################\n");
		}

		m_pD3DDevice = nullptr;
	}

	static bool bEnalbePerf = false;
	#if defined( I3_DEBUG_SHADER)
		deviceArr[0].DeviceType = D3DDEVTYPE_REF;
		deviceArr[0].dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// PerfHUD 작동시 Device 세팅 부분입니다.(PerfHUD Document 참조)
	#elif defined( NV_PERF_HUD_ENABLE)

		for(UINT Adapter=0; Adapter<m_pD3D->GetAdapterCount(); Adapter++)
		{
			D3DADAPTER_IDENTIFIER9  Identifier;
			HRESULT					Res;

			Res = m_pD3D->GetAdapterIdentifier(Adapter, 0, &Identifier);

			if( i3::contain_string( Identifier.Description,"PerfHUD") >= 0)
			{
				adapterIdx = Adapter;
				deviceArr[0].DeviceType = D3DDEVTYPE_REF;
				deviceArr[0].dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
				bEnalbePerf = true;
				break;
			}
		}
	#endif

	if(bEnalbePerf)
	{
		I3PRINTLOG(I3LOG_NOTICE, "PerfHUD Activate.");
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "PerfHUD not Activate.");
	}

	Rv = S_OK;

	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::CreateDevice() Enter");	
	for( i=0; i<MAX_DEVICE_TYPE; i++)
	{
		DWORD Behavior;

		Behavior = deviceArr[i].dwBehavior;

		if( m_CurVideoInfo.m_bMultiThread)
			deviceArr[i].dwBehavior |= D3DCREATE_MULTITHREADED;

		Rv = m_pD3D->CreateDevice( adapterIdx, deviceArr[i].DeviceType,
									m_hWnd,
									deviceArr[i].dwBehavior,
									&m_d3dpp,
									&m_pD3DDevice);
		

		//		
		{	
			switch( deviceArr[i].DeviceType)
			{
			case D3DDEVTYPE_HAL:
				i3::safe_string_copy( szTemp1, "D3DDEVTYPE_HAL", 1024);
				break;
			case D3DDEVTYPE_SW:
				i3::safe_string_copy( szTemp1, "D3DDEVTYPE_SW", 1024);
				break;
			case D3DDEVTYPE_REF:
				i3::safe_string_copy( szTemp1, "D3DDEVTYPE_REF", 1024);
				break;
			}
/*
			switch( deviceArr[i].dwBehavior)
			{
				case D3DCREATE_MIXED_VERTEXPROCESSING:
					i3::safe_string_copy( szTemp2, "D3DCREATE_MIXED_VERTEXPROCESSING", 1024);
					break;
				case D3DCREATE_HARDWARE_VERTEXPROCESSING:
					i3::safe_string_copy( szTemp2, "D3DCREATE_HARDWARE_VERTEXPROCESSING", 1024);
					break;
				case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
					i3::safe_string_copy( szTemp2, "D3DCREATE_SOFTWARE_VERTEXPROCESSING", 1024);
					break;
			}
*/
		

			if (deviceArr[i].dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING)
			{
				if (  deviceArr[i].dwBehavior & D3DCREATE_PUREDEVICE)
				{
					i3::safe_string_copy( szTemp2, "D3DCREATE_PUREDEVICE + D3DCREATE_HARDWARE_VERTEXPROCESSING", 1024);
				}
				else
				{
					i3::safe_string_copy( szTemp2, "D3DCREATE_HARDWARE_VERTEXPROCESSING", 1024);
				}
			}

			if (deviceArr[i].dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING)
			{
				i3::safe_string_copy( szTemp2, "D3DCREATE_MIXED_VERTEXPROCESSING", 1024);
			}

			if (deviceArr[i].dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
			{
				i3::safe_string_copy( szTemp2, "D3DCREATE_SOFTWARE_VERTEXPROCESSING", 1024);			
			}


			I3PRINTLOG(I3LOG_NOTICE, "dwBehavior Type : %d", deviceArr[i].dwBehavior);	
		}

		if( SUCCEEDED( Rv))
		{
			I3PRINTLOG(I3LOG_NOTICE,  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
			I3PRINTLOG(I3LOG_NOTICE,  ">>>>>Create Device Type: %s", szTemp1);
			I3PRINTLOG(I3LOG_NOTICE,  ">>>>>Create Device Behavior: %s", szTemp2);

			

			I3TRACE( "Create Device Type: %s, Behavior: %s\n", szTemp1, szTemp2);

			if( deviceArr[i].dwBehavior == D3DCREATE_SOFTWARE_VERTEXPROCESSING)
			{
				s_bSoftwareProcess = true;
			}

			break;
		}		
		else
		{
			I3PRINTLOG(I3LOG_NOTICE,  ">>>>>Create Device Type: %s Failed!!", szTemp1);
			I3PRINTLOG(I3LOG_NOTICE,  ">>>>>Create Device Behavior: %s  Failed!!", szTemp2);

			I3TRACE( "Create Device Type: %s Failed, Behavior: %s Failed\n", szTemp1, szTemp2);
		}
	}
	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::CreateDevice() Leave");	

	if( FAILED( Rv))
	{
		DXTRACE_ERR( L"Could not create DirectX device.", Rv);

		I3PRINTLOG(I3LOG_NOTICE,  " ---------------------- Config ----------------------");
		I3PRINTLOG(I3LOG_NOTICE,  " * Resolution : %dx%d  (Windowed:%d)", m_CurVideoInfo.m_Width, m_CurVideoInfo.m_Height, !m_CurVideoInfo.m_bFullScreen);
		I3PRINTLOG(I3LOG_NOTICE,  " * Color Format : %s", i3Gfx::GetImageFormatName( m_CurVideoInfo.m_RenderTargetFormat));
		I3PRINTLOG(I3LOG_NOTICE,  " * Depth Format : %s", i3Gfx::GetImageFormatName( m_CurVideoInfo.m_DepthFormat));
		I3PRINTLOG(I3LOG_NOTICE,  " * Z Buffer bits : %d", m_CurVideoInfo.m_ZBufferBits);
		I3PRINTLOG(I3LOG_NOTICE,  " * Stencil bits : %d", m_CurVideoInfo.m_StencilBufferBits);
		I3PRINTLOG(I3LOG_NOTICE,  " * Refresh rate : %f", m_CurVideoInfo.m_RefreshRate);
		I3PRINTLOG(I3LOG_NOTICE,  " * Multi-thread : %d", m_CurVideoInfo.m_bMultiThread);
		I3PRINTLOG(I3LOG_NOTICE,  " * Multi-Sampling : %d (x%d)", m_CurVideoInfo.m_MultiSampling, m_CurVideoInfo.m_MultiSamplingQuality);
		I3PRINTLOG(I3LOG_NOTICE, " * AdapterIdx Index : %d", m_CurVideoInfo.m_AdapterIndex);
		I3PRINTLOG(I3LOG_NOTICE, " * Mode Index : %d", m_CurVideoInfo.m_ModeIndex);
		switch( Rv)
		{
		case D3DERR_DEVICELOST:
			I3PRINTLOG(I3LOG_FATAL,  "Hardware test Error:: Could not create DirectX9 Device Object >> D3DERR_DEVICELOST");
			break;
		case D3DERR_INVALIDCALL:
			I3PRINTLOG(I3LOG_FATAL,  "Hardware test Error:: Could not create DirectX9 Device Object >> D3DERR_INVALIDCALL");
			break;
		case D3DERR_NOTAVAILABLE:
			I3PRINTLOG(I3LOG_FATAL,  "Hardware test Error:: Could not create DirectX9 Device Object >> D3DERR_NOTAVAILABLE");
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			I3PRINTLOG(I3LOG_FATAL,  "Hardware test Error:: Could not create DirectX9 Device Object >> D3DERR_OUTOFVIDEOMEMORY");
			break;
		}
		return false;
	}

	{
		D3DCAPS9		caps;
		HRESULT hrCaps = m_pD3DDevice->GetDeviceCaps(&caps);
		//  중복호출이지만, DeviceCaps를 못받아올거 같으면 실패로 간주해주어야 한다...일단은 풀스크린 윈도우 모드가 되도록 조절하자..
		if (hrCaps != D3D_OK)
		{
			const wchar_t* strErr = DXGetErrorString(hrCaps);
			I3PRINTLOG(I3LOG_NOTICE, "Cannot Get D3DDeviceCaps : errMsg = %s errHR = %d", strErr, hrCaps);		// 제대로 못받아오는 경우가 생각보다 많습니다...
			//	D3DERR_INVALIDCALL, D3DERR_INVALIDDEVICE, D3DERR_OUTOFVIDEOMEMORY, and D3DERR_NOTAVAILABLE
			m_pD3DDevice->Release();
			m_pD3DDevice = NULL;
			g_pD3DDevice = NULL;
			return false;
		}
	}

	//
	g_pD3DDevice = m_pD3DDevice;

	// Device Capacity Information
	I3_SAFE_RELEASE( m_pCaps);
	{
		m_pCaps = i3GfxCaps::new_object();
		I3ASSERT( m_pCaps != nullptr);

		if( m_pCaps->getVertexShaderVersionMajor() >= 3)
			m_InstancingMethod = I3G_INSTANCING_METHOD_HW;
		else
			m_InstancingMethod = I3G_INSTANCING_METHOD_STREAM;

		//m_InstancingMethod = I3G_INSTANCING_METHOD_STREAM;

		std::ostringstream stream;

		stream.width(0);
		stream.fill(' ');

		struct fmtflags_reset
		{ 
			static void call(std::ostringstream& stream, i3::true_type) { stream.setf(std::ios::fmtflags(0));  } 
			static void call(std::ostringstream& stream, i3::false_type) {} 
		};
		fmtflags_reset::call(stream, i3::integral_constant<bool, std::ios::fmtflags(0) != 0 >() );
	
//		if (std::ios::fmtflags(0))
//			stream.setf(std::ios::fmtflags(0));

		stream << g_pD3DDevice->GetAvailableTextureMem();

		I3PRINTLOG(I3LOG_NOTICE,  ">>>>>Current GPU Name: %s AvailableTextureMem %s", m_pCaps->getGPUName(), stream.str().c_str());
		I3PRINTLOG(I3LOG_NOTICE,  ">>>>>VS_Major_Ver %d VS_Minor_Ver %d", m_pCaps->getVertexShaderVersionMajor(), m_pCaps->getVertexShaderVersionMinor());
		I3PRINTLOG(I3LOG_NOTICE,  ">>>>>PS_Major_Ver %d PS_Minor_Ver %d", m_pCaps->getPixelShaderVersionMajor(), m_pCaps->getPixelShaderVersionMinor());
	}

	// Shader  
	m_pCaps->SetEnableShader( IsRenderFlag(I3RENDER_FLAG_SHADER_ENABLE));

	//g_GfxOption.setSpecularMapEnable( false);

	if( m_pCaps->getEnableShader())
	{
		CreateDefaultShaderContext();
	}	

	return OnResetDevice() != FALSE;
	
	// return true;
}

void i3RenderContext::UpdateRenderTarget(void)
{
	IDirect3DSurface9 * pSurface, * pDepthSurface;
	i3RenderTarget * pNewRT;
	INT32 i;
	HRESULT Rv;

	// revision 2796
	INT32 nBackBufferCnt = static_cast<INT32>(m_d3dpp.BackBufferCount);
	
	//최소한 1개의 BackBuffer가 필요하다..(윈8에서 0개로 받아오는 경우가 있어 강제로 1개 생성한다)
	if (nBackBufferCnt == 0)
		nBackBufferCnt = 1;

	for( i = 0; i < nBackBufferCnt; i++)
	{
		Rv = m_pD3DDevice->GetBackBuffer( i, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"GetBackBuffer() failed.", Rv);
			return;
		}

		Rv = m_pD3DDevice->GetDepthStencilSurface( &pDepthSurface);
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"GetBackBuffer() failed.", Rv);
			return;
		}

		pNewRT = i3RenderTarget::new_object();

		pNewRT->Bind( pSurface, pDepthSurface);

		if( i == 0)
		{
			m_pRenderTarget = pNewRT;
			SetOriginalRenderTarget(m_pRenderTarget);
			SetDefaultRenderTarget(m_pRenderTarget);
		}

		m_RenderTargetList.push_back( pNewRT);
	}

	m_pD3DDevice->GetSwapChain( 0, &m_pSwapChain);
	
	{
		I3_SAFE_RELEASE( m_pLastFrameTargetTexture);

		D3DSURFACE_DESC Desc;

		Rv = m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
		Rv = pSurface->GetDesc( &Desc);

		m_pLastFrameTargetTexture = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pLastFrameTargetTexture->SetName( "LastFrameTarget");
#endif
		m_pLastFrameTargetTexture->Create( Desc.Width, Desc.Height, 1, I3G_IMAGE_FORMAT_BGRA_8888, I3G_USAGE_RENDERTARGET);

		pSurface->Release();
	}
}

void i3RenderContext::UpdateVideoInfo(void)
{
	//   Device 憺 컁磯.
	D3DFORMAT DepthFormat, ColorFormat;
	bool Rv;

	{
		i3RenderTarget * pRT = m_pOriginalRenderTarget;

		m_CurVideoInfo.m_Width = pRT->GetWidth();
		m_CurVideoInfo.m_Height = pRT->GetHeight();
		m_CurVideoInfo.m_RenderTargetFormat = pRT->GetFormat();

		_SetCurrentViewportRect( 0, 0, pRT->GetWidth(), pRT->GetHeight());
	}
	
	ColorFormat = i3DXUT_GetNativeImageFormat( m_CurVideoInfo.m_RenderTargetFormat);

	Rv = FindDepthStencilFormat( g_iCurrentAdapterIndex, D3DDEVTYPE_HAL, 
			ColorFormat, m_CurVideoInfo.m_ZBufferBits, m_CurVideoInfo.m_StencilBufferBits, &DepthFormat);
	if( Rv == false)
	{
		DXTRACE_ERR(L"i3RenderContext::UpdateVideoInfo()", Rv);
		return;
	}

	m_CurVideoInfo.m_DepthFormat = i3DXUT_GetI3ImageFormat( DepthFormat);

	IsCompressedTextureFormatOk( ColorFormat) ? addRenderFlag(I3RENDER_FLAG_ENABLE_COMPRESSEDTEX):removeRenderFlag(I3RENDER_FLAG_ENABLE_COMPRESSEDTEX);

	UNSETBIT( m_ClearFlag, I3G_CLEAR_STENCIL | I3G_CLEAR_DEPTH);

	switch( DepthFormat)
	{
		case D3DFMT_D16_LOCKABLE :
		case D3DFMT_D16 :
			m_CurVideoInfo.m_ZBufferBits = 16;
			m_CurVideoInfo.m_StencilBufferBits = 0;
			break;

		case D3DFMT_D15S1 :
			m_CurVideoInfo.m_ZBufferBits = 15;
			m_CurVideoInfo.m_StencilBufferBits = 1;
			break;

		case D3DFMT_D24S8 :
			m_CurVideoInfo.m_ZBufferBits = 24;
			m_CurVideoInfo.m_StencilBufferBits = 8;
			break;

		case D3DFMT_D24X8 :
			m_CurVideoInfo.m_ZBufferBits = 24;
			m_CurVideoInfo.m_StencilBufferBits = 0;
			break;

		case D3DFMT_D24X4S4 :
			m_CurVideoInfo.m_ZBufferBits = 24;
			m_CurVideoInfo.m_StencilBufferBits = 4;
			break;

		case D3DFMT_D32 :
			m_CurVideoInfo.m_ZBufferBits = 32;
			m_CurVideoInfo.m_StencilBufferBits = 0;
			break;
	}

	if( m_CurVideoInfo.m_ZBufferBits > 0)
	{
		SETBIT( m_ClearFlag, I3G_CLEAR_DEPTH);
	}

	if( m_CurVideoInfo.m_StencilBufferBits > 0)
	{
		SETBIT( m_ClearFlag, I3G_CLEAR_STENCIL);
	}

	//if( !m_CurVideoInfo.m_bFullScreen)
	m_CurVideoInfo.m_bFullScreen ? removeRenderFlag( I3RENDER_FLAG_WINDOWED) : addRenderFlag( I3RENDER_FLAG_WINDOWED);

	i3Log( "", "-----------------------------------------\n");
	i3Log( "", " * Width : %d", m_CurVideoInfo.m_Width);
	i3Log( "", " * Height : %d", m_CurVideoInfo.m_Height);
	i3Log( "", " * Depth Buffer Bits : %d", m_CurVideoInfo.m_ZBufferBits);
	i3Log( "", " * Stencil Buffer Bits : %d", m_CurVideoInfo.m_StencilBufferBits);
	i3Log( "", " * Full-Screen : %d", m_CurVideoInfo.m_bFullScreen);
	i3Log( "", " * Interface Mode : %d", m_CurVideoInfo.m_bInterlaced);
	i3Log( "", " * Refresh Rate : %f", m_CurVideoInfo.m_RefreshRate);
	i3Log( "", " * Frame Buffer Count : %d", m_CurVideoInfo.m_FrameBufferCount);
}

bool i3RenderContext::OnResetDevice()
{
	PrevResetDevice();
	if (!ResetDevice()) return false;
	PostResetDevice();
	return true;
}

bool i3RenderContext::PrevResetDevice()
{
	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() Enter");	
	if( m_pD3DDevice == nullptr)
		return true;

	// Device is rivived!
	addRenderFlag(I3RENDER_FLAG_REVIVED);

	m_Switch.m_hLastError = S_OK;

	// Release all vidmem objects
	_InvalidateGfxResource( false);

	return true;
}

bool i3RenderContext::ResetDevice(void)
{
	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() 1");

	// revision 2781
	//Recreate 전에.. 을 넣어주지 않으면 생성 실패한다.
	// 창모드때 BackBufferCount = 1;
	// 전체화면 BackBufferCount = 0;
	if (m_d3dpp.Windowed == TRUE)
	{
		if (m_d3dpp.BackBufferCount < 1)
			m_d3dpp.BackBufferCount = 1;
	}
	else
	{
		if (m_d3dpp.BackBufferCount != 0)
			m_d3dpp.BackBufferCount = 0;
	}

    // Reset the device
	// PerfHUD 작동시 DX9 디바이스 Reset 호출하면 크래쉬 발생합니다.(정확한 원인은 아직 파악중입니다.)
	// 일반적인 Debug,Release 상태에서는 속도와 Device Resize, Reset 문제를 고려하여 작동 시키지 않습니다.
	#if !defined( NV_PERF_HUD_ENABLE)
		m_Switch.m_hLastError = m_pD3DDevice->Reset( &m_d3dpp);
	#endif

    if( FAILED( m_Switch.m_hLastError))
	{
		const wchar_t* strErr  = DXGetErrorString(m_Switch.m_hLastError);
		I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() Failed Msg(%d) = %s DesErr = %s ", m_Switch.m_hLastError ,strErr);

		if( m_Switch.m_hLastError == D3DERR_DEVICELOST)
		{
			removeRenderFlag(I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR);
			I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() Device Lost.");
		}
		else
		{
			addRenderFlag(I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR);
		}
		return false;
	}
	else
	{
		removeRenderFlag(I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR);
	}

	return true;
}

bool	i3RenderContext::PostResetDevice()
{
	bool fullscreenModeEnable =	m_CurVideoInfo.m_bFullScreenWindowModeEnable;
	bool fullScreenWindowActivated = (m_CurVideoInfo.m_bFullScreen && m_CurVideoInfo.m_bFullScreenWindowModeEnable);
	
	m_CurVideoInfo = m_d3dpp;

	if (fullScreenWindowActivated)
		m_CurVideoInfo.m_bFullScreen = true;
	
	if (fullscreenModeEnable)
		m_CurVideoInfo.m_bFullScreenWindowModeEnable = true;

	UpdateRenderTarget();

	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() 2");

	UpdateVideoInfo();

	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() 3");

	OnInit();

	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() 4");

	_ValidateGfxResource();

	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() 5");

	SetDefaultRenderState();

	//m_pD3DDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE);

	I3PRINTLOG(I3LOG_NOTICE,  "i3RenderContext::ResetDevice() Leave");	

	return true;
}

void	i3RenderContext::SetClearColor( COLOR * pColor)
{
	i3Color::Set( &m_BackColor, pColor);
}

void	i3RenderContext::SetClearZValue( REAL32 val)
{
	m_ClearZValue = val;
}

void	i3RenderContext::SetClearStencilValue( UINT8 val)
{
	m_ClearStencilValue = val;
}

void	i3RenderContext::SetClearRect( D3DRECT* pRect, INT32 nCount)
{
	m_pClearRectArray = pRect;
	m_nClearRectCount = nCount;
}


bool	i3RenderContext::Begin(void)
{
	HRESULT Rv;

	m_nClearRectCount = 0;
	m_UsedClutCount = 0;

	m_RenderFlag |= I3RENDER_FLAG_READY;

    // Test the cooperative level to see if it's okay to render
	Rv = m_pD3DDevice->TestCooperativeLevel();
    if( FAILED( Rv))
    {
        // If the device was lost, do not render until we get it back
		/*
        if( Rv == D3DERR_DEVICELOST)
            return false;
			*/

        // Check if the device needs to be resized.
		switch( Rv)
		{
			case D3DERR_DEVICENOTRESET :
				{
					D3DDISPLAYMODE mode;
					m_pD3D->GetAdapterDisplayMode( m_CurVideoInfo.m_AdapterIndex, &mode);
					if( m_d3dpp.BackBufferFormat != mode.Format)
					{
						RefreshDevice();
					}
				}

				// If we are windowed, read the desktop mode and use the same format for
				// the back buffer
				if( m_CurVideoInfo.m_bFullScreen == false)
				{
					GetCurrentVideoInfo( &m_CurVideoInfo);

					if( OnResetDevice() == false)
					{
						m_RenderFlag &= ~I3RENDER_FLAG_READY;
						return false;
					}
				}
				else
				{
// 					i3VideoInfo* video = i3VideoInfo::new_object();
// 					*video = m_CurVideoInfo;
// 					video->CopyTo()
// 					ChangeVideoInfo(video);
// 					video->Release();

					i3VideoInfo video;
					m_CurVideoInfo.CopyTo(&video,I3_COPY_REF);

					ChangeVideoInfo( &video);
				}
				break;

			case D3DERR_DEVICELOST:		// 모니터 케이블 뽑을때도 나오는데..
				{						// 이 때 D3DERR_DEVICENOTRESET로 진행되지 않는것 같다..
					// 강제진행을 위해서 _GetMonitorFromWindow와 비교하는게 좋겠다..

					if( m_CurVideoInfo.m_bFullScreen == true)
					{
						HMONITOR hMonitor = FindCurrentMonitorHandle();
					//	_GetMonitorFromWindow(m_hWnd, &hMonitor);
						
						if (hMonitor != 0 && m_CurHMonitor != hMonitor)
						{
							I3PRINTLOG(I3LOG_NOTICE, "Monitor Cable Problem Occurred..");
							i3VideoInfo video;
							m_CurVideoInfo.CopyTo(&video,I3_COPY_REF);
							ChangeVideoInfo( &video);
						}
					}	
				}
			break;
		}

		m_RenderFlag &= ~I3RENDER_FLAG_READY;

        return false;
    }

	//m_pPerfReport->Begin();

	if( m_pVertexBank)	m_pVertexBank->ResetIndex();

	return true;
}

void i3RenderContext::End(void)
{
	// Show the frame on the primary surface.
	//m_pD3DDevice->Present( NULL, NULL, NULL, NULL );

	if( IsRenderFlag( I3RENDER_FLAG_PRESENT))
	{
		/*
		while( m_pSwapChain->Present( m_pPresentRect, m_pPresentRect, NULL, NULL, D3DPRESENT_DONOTWAIT) == D3DERR_WASSTILLDRAWING)
		{
			INT32 i;

			for( i = 0; i < m_WaitProcList.GetCount(); i++)
			{
				WAIT_VSYNC_INFO * pInfo = (WAIT_VSYNC_INFO *) m_WaitProcList.Items[i];
				WAITVSYNCPROC pProc = pInfo->m_pProc;

				pProc(  this, pInfo->m_pUserData);
			}
		}
		*/
		if (m_pSwapChain)
		{
			if (m_pPresentRect || !IsRenderFlag(I3RENDER_FLAG_SWAPCHAIN_ENABLE))
			{
				m_pSwapChain->Present( m_pPresentRect, m_pPresentRect, m_hWnd, NULL, 0);
			}
			else
			{
				m_pSwapChain->Present( &m_pSourcePresentRect, &m_pDestPresentRect, m_hWnd, NULL, 0);
			}
		}

		if (IsRenderFlag(I3RENDER_FLAG_SWAPCHAIN_ENABLE))
		{
// 			if (m_pSwapChainBackBuffer)
// 			{
// 				m_pSwapChainBackBuffer->Release();
// 				m_pSwapChainBackBuffer = nullptr;
// 				//I3TRACE("Release SwapChain Back Buffer 000000000000000000000000000000000000\n");
// 			}
		}
	}

	{
		m_pPerfReport->SetDrawCallCountPerFrame( m_DrawCallCount);
		m_pPerfReport->SetTriangleCountPerFrame( m_TriangleCount);
		m_pPerfReport->End();

		m_FrameCountPerSec++;

		m_DeltaSec = m_pTimer->GetDeltaSec();

		m_secFrameTime += m_DeltaSec;

		if( m_secFrameTime >= 1.0f)
		{
			// 1歌 .
			m_pPerfReport->SetFPS( m_FrameCountPerSec / m_secFrameTime);
			m_FrameCountPerSec = 0;
			m_secFrameTime = 0.0f;
		}

		m_DrawCallCount = 0;
		m_TriangleCount = 0;

#if defined( I3_DEBUG)
		if( m_bDumpDrawPrimitive)
		{
			I3TRACE( "Total Primitive : %d\n", m_DrawPrimitiveList.size());
			I3TRACE( "--------------------------------------------------------\n");
			for( UINT32 i = 0; i < m_DrawPrimitiveList.size(); i++)
			{
				tagPRIMITIVEINFO * pInfo = m_DrawPrimitiveList.at(i);
				I3TRACE( " - %s %d %d\n", pInfo->_name, pInfo->_startIdx, pInfo->_primCnt);
			}
			I3TRACE( "--------------------------------------------------------\n");

			m_bDumpDrawPrimitive = false;
		}

		for( UINT32 i = 0; i < m_DrawPrimitiveList.size(); i++)
		{
			tagPRIMITIVEINFO * pInfo = m_DrawPrimitiveList.at(i);
			I3_MUST_DELETE( pInfo);
		}
		m_DrawPrimitiveList.clear();
#endif
	}

	removeRenderFlag(I3RENDER_FLAG_REVIVED);
}

bool i3RenderContext::BeginRender(void)
{
	INT32 i;

	resetAffectLight();

	for( i = 0; i < I3G_TEXTURE_BIND_MAX; i++)
	{
		m_TexBindInfo[i].m_pTexture = nullptr;
	}

	return !FAILED( m_pD3DDevice->BeginScene());
}

void i3RenderContext::EndRender(void)
{
    // End the scene.
	HRESULT Rv;

    Rv = m_pD3DDevice->EndScene();   

	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3RenderContext::EndRender()", Rv);
	}
	#endif
}

void i3RenderContext::Clear( I3G_CLEAR AddMask, I3G_CLEAR RemoveMask)
{
	I3G_CLEAR flag;
	D3DCOLOR color;

	flag = m_ClearFlag;
	flag |= AddMask;

	flag &= ~(RemoveMask);

	color = i3Color::ConvertARGB32( &m_BackColor);
	//color = 0xff7f0000;

	if( m_nClearRectCount <= 0) m_pClearRectArray = nullptr;

	if (IsRenderFlag(I3RENDER_FLAG_SWAPCHAIN_ENABLE) && !m_pSwapChainBackBuffer)
	{
// 		if (m_pSwapChain)
// 		{
// 			m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pSwapChainBackBuffer);
// 			//I3TRACE("Get SwapChain Back Buffer !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
// 			m_pD3DDevice->SetRenderTarget(0, m_pSwapChainBackBuffer);
// 		}
	}

    #if defined( I3_DEBUG)
	HRESULT Rv;

    // Clear the viewport
    Rv = 
	#endif
/*		m_pD3DDevice->Clear( 0, NULL, 
				flag, 
				color, 
				m_ClearZValue,							// Z Clear Value
				m_ClearStencilValue						// Stencil Clear Value
				);
*/
		m_pD3DDevice->Clear( m_nClearRectCount, m_pClearRectArray, 
				flag, 
				color, 
				m_ClearZValue,							// Z Clear Value
				m_ClearStencilValue						// Stencil Clear Value
				);

	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"Clear buffer error", Rv);
	}
	#endif
}

void i3RenderContext::SetVertexArray( i3VertexArray * pVA)
{
	if( m_pCurrentVA != pVA)
	{
		IDirect3DVertexBuffer9 * pVB = (IDirect3DVertexBuffer9 *) pVA->GetInstance();
		IDirect3DVertexDeclaration9 * pVBDecl = ((i3VertexArrayDX *) pVA)->getDXVertexDecl();

		//Rv = m_pD3DDevice->SetFVF( (pVA)->GetVertexFormatCode());
		
		CHECKRESULT( m_pD3DDevice->SetVertexDeclaration( pVBDecl));
		CHECKRESULT( m_pD3DDevice->SetStreamSource( 0, pVB, 0, pVA->GetStride()));

		#if defined( VTX_STREAM)
		{
			pVB = pVADX->getDXVertex2();

			CHECKRESULT( m_pD3DDevice->SetStreamSource( 1, pVB, 0, pVADX->getStride2()));
		}
		#endif

		m_pCurrentVA = pVA;
	}
}

void i3RenderContext::SetInstancingArray( I3GFX_INSTANCING_INFO * pInfo)
{
	if( pInfo != nullptr)
	{
		I3ASSERT( pInfo->count > 0);
		I3ASSERT( pInfo->m_pVStream != nullptr);
		I3ASSERT( pInfo->m_pSecondVStream != nullptr);
		I3ASSERT( pInfo->m_pVBDecl != nullptr);

		i3mem::Copy( &m_InstancingInfo, pInfo, sizeof( m_InstancingInfo));

		i3VertexArray * pVA = pInfo->m_pVStream;
		i3VertexArray * pInsVA = pInfo->m_pSecondVStream;

		IDirect3DVertexBuffer9 * pVB = (IDirect3DVertexBuffer9 *) pInfo->m_pVStream->GetInstance();
		IDirect3DVertexBuffer9 * pInsVB = (IDirect3DVertexBuffer9 *) pInsVA->GetInstance();

		INT32 off0, off1;

		off0 = pInfo->m_Offset_Stream0 * pInfo->m_pVStream->GetStride();
		off1 = pInfo->m_Offset_Stream1 * pInfo->m_pSecondVStream->GetStride();

		if( m_InstancingMethod == I3G_INSTANCING_METHOD_HW)
		{
			CHECKRESULT( m_pD3DDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | pInfo->count));
			CHECKRESULT( m_pD3DDevice->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | (UINT)1));

			CHECKRESULT( m_pD3DDevice->SetStreamSource( 0, pVB, off0, pVA->GetStride()));
			CHECKRESULT( m_pD3DDevice->SetStreamSource( 1, pInsVB, off1, pInsVA->GetStride()));
		}
		else
		{
			CHECKRESULT( m_pD3DDevice->SetStreamSource( 0, pVB, off0, pVA->GetStride()));
		}

		CHECKRESULT( m_pD3DDevice->SetVertexDeclaration( pInfo->m_pVBDecl));

		m_pCurrentVA = pVA;

		setInstancingEnable( true);
	}
	else
	{
		CHECKRESULT( m_pD3DDevice->SetStreamSourceFreq( 0, 1));
		CHECKRESULT( m_pD3DDevice->SetStreamSourceFreq( 1, 1));

		setInstancingEnable( false);
	}
}

void i3RenderContext::SetIndexArray( i3IndexArray * pIA)
{
	if( m_pCurrentIA != pIA)
	{
		HRESULT Rv;
		IDirect3DIndexBuffer9 * pIB = (IDirect3DIndexBuffer9 *) (pIA)->GetDxBuffer();

		Rv = m_pD3DDevice->SetIndices( pIB);
		#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3Rendercontext::SetIndexArray()", Rv);
		}
		#endif

		m_pCurrentIA = pIA;
	}
}

void i3RenderContext::SetWorldMatrix( MATRIX * pMatrix)
{
	HRESULT Rv;

	if( m_pCaps->getEnableShader() == false)
	{
		#if defined( I3_FIXED_POINT)
			MATRIX tmx;

			i3Matrix::Copy( &tmx, pMatrix);
			Rv = m_pD3DDevice->SetTransform( D3DTS_WORLD, (const D3DMATRIX *) &tmx);
		#else
			Rv = m_pD3DDevice->SetTransform( D3DTS_WORLD, (const D3DMATRIX *) pMatrix);
		#endif

		#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetWorldMatrix() failed.", Rv);
		}
		#endif
	}
	else
	{
		if( m_pCurShader != nullptr)
			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_MATRIX_WORLD);
	}

	m_LastWorldMatrix = *pMatrix;
}

void i3RenderContext::SetViewMatrix( MATRIX * pMatrix)
{
	HRESULT Rv;

	if( m_pCaps->getEnableShader() == false)
	{
		#if defined( I3_FIXED_POINT)
			MATRIX tmx;

			i3Matrix::Copy( &tmx, pMatrix);
			Rv = m_pD3DDevice->SetTransform( D3DTS_VIEW, (const D3DMATRIX *) &tmx);
		#else
			Rv = m_pD3DDevice->SetTransform( D3DTS_VIEW, (const D3DMATRIX *) pMatrix);
		#endif

		#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetViewMatrix() failed.", Rv);
		}
		#endif
	}
	else
	{
		if( m_pCurShader != nullptr)
			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_MATRIX_VIEW);
	}

	m_LastViewMatrix = *pMatrix;
}

void i3RenderContext::SetProjectMatrix( MATRIX * pMatrix)
{
	HRESULT Rv;

	if( m_pCaps->getEnableShader() == false)
	{
		#if defined( I3_FIXED_POINT)
			MATRIX tmx;

			i3Matrix::Copy( &tmx, pMatrix);
			Rv = m_pD3DDevice->SetTransform( D3DTS_PROJECTION, (const D3DMATRIX *) &tmx);
		#else
			Rv = m_pD3DDevice->SetTransform( D3DTS_PROJECTION, (const D3DMATRIX *) pMatrix);
		#endif

		#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetViewMatrix() failed.", Rv);
		}
		#endif
	}
	else
	{
		if( m_pCurShader != nullptr)
			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_MATRIX_PROJECT);
	}

	m_OriProjectMatrix = m_LastProjectMatrix = *pMatrix;
}

void i3RenderContext::SetProjectOffset(const VEC3D& vec)
{
	MATRIX mat;
	i3Matrix::Copy(&mat, &m_OriProjectMatrix);

	i3Matrix::PostTranslate(&mat, vec.x, vec.y, vec.z);

	m_LastProjectMatrix = mat;
}


static I3G_MATRIX_STACK		s_TextureMatrixTable[8] =
{
	I3G_MATRIX_TEXTURE0,
	I3G_MATRIX_TEXTURE1,
	I3G_MATRIX_TEXTURE2,
	I3G_MATRIX_TEXTURE3,
	I3G_MATRIX_TEXTURE4,
	I3G_MATRIX_TEXTURE5,
	I3G_MATRIX_TEXTURE6,
	I3G_MATRIX_TEXTURE7
};

static D3DTEXTURETRANSFORMFLAGS s_TextureFlagTable[4] =
{
	D3DTTFF_COUNT1,
	D3DTTFF_COUNT2,
	D3DTTFF_COUNT3,
	D3DTTFF_COUNT4
};

void i3RenderContext::_SetTextureMatrix( INT32 StageIndex, MATRIX * pMatrix, INT32 CoordCount, bool bProjective)
{
	HRESULT Rv;
	D3DTRANSFORMSTATETYPE Index;
	DWORD Flag;

	I3ASSERT((StageIndex >= 0) && (StageIndex < 8));

	if( m_pCaps->getEnableShader() == false)
	{
		Index = (D3DTRANSFORMSTATETYPE) s_TextureMatrixTable[ StageIndex];

		if((pMatrix == nullptr) || (CoordCount <= 0))
		{
			// Disable Texture Matrix
			CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIndex, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE));
		}
		else
		{
			Flag = s_TextureFlagTable[ CoordCount - 1];
			
			if( bProjective)
				Flag |= D3DTTFF_PROJECTED;

			CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIndex, D3DTSS_TEXTURETRANSFORMFLAGS, Flag));

			#if defined( I3_FIXED_POINT)
				MATRIX tmx;

				i3Matrix::Copy( &tmx, pMatrix);
				Rv = m_pD3DDevice->SetTransform( Index, (const D3DMATRIX *) &tmx);
			#else
				Rv = m_pD3DDevice->SetTransform( Index, (const D3DMATRIX *) pMatrix);
			#endif
				
			#if defined( I3_DEBUG)
			if( FAILED( Rv))
			{
				DXTRACE_ERR(L"i3RenderContext::SetMatrix() failed.", Rv);
			}
			#endif
		}
	}
	else
	{
		//m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_MATRIX_TEXTURE);
	}

	//m_LastTextureMatrix[ StageIndex] = *pMatrix;
}

void i3RenderContext::SetVertexBlendMatricesByRef( MATRIX ** pMatrix, INT32 cnt)
{
	INT32 i;

	if( m_pCaps->getEnableShader() == false)
	{
		#if defined( I3_FIXED_POINT)
			MATRIX tmx;

			for( i = 0; i < cnt; i++)
			{
				i3Matrix::Copy( &tmx, pMatrix[i]);
				CHECKRESULT( m_pD3DDevice->SetTransform( D3DTS_WORLDMATRIX(i), (CONST D3DMATRIX *) &tmx));
			}
		#else
			for( i = 0; i < cnt; i++)
			{
				CHECKRESULT( m_pD3DDevice->SetTransform( D3DTS_WORLDMATRIX(i), (CONST D3DMATRIX *) pMatrix[i]));
			}
		#endif
	}
	else
	{
		for( i = 0; i < cnt; i++)
		{
			i3Matrix::Mul( &m_BoneMatrixArray[i], pMatrix[i], &m_LastViewMatrix);
		}

		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_MATRIX_BONE);
	}

	m_BoneMatrixCount = cnt;
}

void i3RenderContext::SetVertexBlendMatrices( MATRIX * pMatrix, INT32 cnt)
{
	INT32 i;

	if( m_pCaps->getEnableShader() == false)
	{
		#if defined( I3_FIXED_POINT)
			MATRIX tmx;

			for( i = 0; i < cnt; i++)
			{
				i3Matrix::Copy( &tmx, &pMatrix[i]);
				CHECKRESULT( m_pD3DDevice->SetTransform( D3DTS_WORLDMATRIX(i), (CONST D3DMATRIX *) &tmx));
			}
		#else
			for( i = 0; i < cnt; i++)
			{
				CHECKRESULT( m_pD3DDevice->SetTransform( D3DTS_WORLDMATRIX(i), (CONST D3DMATRIX *) &pMatrix[i]));
			}
		#endif
	}
	else
	{
		for( i = 0; i < cnt; i++)
		{
			i3Matrix::Mul( &m_BoneMatrixArray[i], &pMatrix[i], &m_LastViewMatrix);
		}

		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_MATRIX_BONE);
	}
	m_BoneMatrixCount = cnt;
}


void i3RenderContext::SetViewport( INT32 x, INT32 y, INT32 Width, INT32 Height)
{
	HRESULT Rv;
	D3DVIEWPORT9	ViewPort;

	if((Width <= 0) || (Height <= 0))
	{
		Width	= m_curViewportRect.right;
		Height	= m_curViewportRect.bottom;
	}

	ViewPort.X = x;
	ViewPort.Y = y;
	ViewPort.Width = Width;
	ViewPort.Height = Height;
	ViewPort.MinZ = 0.0f;
	ViewPort.MaxZ = 1.0f;

	Rv = m_pD3DDevice->SetViewport( &ViewPort);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3RenderContext::SetViewport() failed.", Rv);
	}
	#endif
}

static IDirect3DVertexShader9 * s_pOldVS = nullptr;

inline void _syncShader( i3RenderContext * pRenderCtx, i3Shader * pShader, i3VertexArray * pVA)
{
	IDirect3DVertexShader9 * pVS;
	IDirect3DPixelShader9 * pPS;
	i3ShaderContext * pCtx = pShader->getContext();

	if((pVA != nullptr) && ((pVA->GetFormat()->GetBlendIndexCount() > 0) || (pVA->GetFormat()->GetBlendWeightCount() > 0)))
	{
		INT32 numBones = MIN( 4, pVA->GetFormat()->GetBlendIndexCount());

		pVS = (IDirect3DVertexShader9 *) pCtx->getShader( (I3G_SHADER_TYPE) numBones);

		if( pCtx->isState( I3G_SHADER_CTX_STATE_SKINNING) == false)
		{
			pCtx->setModifiedFlag( 0xFFFFFFFF);
			pCtx->addState( I3G_SHADER_CTX_STATE_SKINNING);
		}

		pCtx->UpdateParams( (I3G_SHADER_TYPE) numBones, pRenderCtx);
	}
	else
	{
		pVS = (IDirect3DVertexShader9 *) pCtx->getShader( I3G_SHADER_TYPE_VERTEX);

		if( pCtx->isState( I3G_SHADER_CTX_STATE_SKINNING))
		{
			pCtx->setModifiedFlag( 0xFFFFFFFF);
			pCtx->removeState( I3G_SHADER_CTX_STATE_SKINNING);
		}

		pCtx->UpdateParams( I3G_SHADER_TYPE_VERTEX, pRenderCtx);
	}

	pPS = (IDirect3DPixelShader9 *) pCtx->getShader( I3G_SHADER_TYPE_PIXEL);

	pCtx->UpdateParams( I3G_SHADER_TYPE_PIXEL, pRenderCtx, false);

	if( s_pOldVS != pVS)
	{
		CHECKRESULT( g_pD3DDevice->SetVertexShader( pVS));
		CHECKRESULT( g_pD3DDevice->SetPixelShader( pPS));

		s_pOldVS = pVS;
	}

	pCtx->setModifiedFlag( 0);
}

void i3RenderContext::DrawPrimUP( I3G_PRIMITIVE Prim, UINT32 PrimCount, void * pStream, UINT32 stride)
{
	HRESULT Rv;

	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

	// Shader  瀏 카 풔磯.
	if( GetCaps()->getEnableShader() )
	{
		SelectShader( nullptr);
	}

	//START_PROFILE( 1);
	Rv = m_pD3DDevice->DrawPrimitiveUP( (D3DPRIMITIVETYPE) Prim, PrimCount, (const void *) pStream, stride);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3RenderContext::DrawPrimUP()", Rv);
	}
	#endif

}

i3Shader * i3RenderContext::DrawDXMesh( LPD3DXMESH pMesh, i3Shader * pShader, UINT32 Subset, UINT32 PrimCount)
{
	HRESULT Rv;

	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

	// Shader  瀏 카 풔磯.
	if( GetCaps()->getEnableShader() )
	{
		SelectShader( pShader);
	}

	Rv = pMesh->DrawSubset( Subset);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3RenderContext::DrawPrim()", Rv);
	}
	#endif

	return m_pCurShader;

}

i3Shader * i3RenderContext::DrawPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount)
{
	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

	// Shader  瀏 카 풔磯.
	if( GetCaps()->getEnableShader() )
	{
		SelectShader( pShader);
	}

	if( IsRenderFlag(I3RENDER_FLAG_INSTANCING) && (m_InstancingMethod == I3G_INSTANCING_METHOD_STREAM))
	{
		I3ASSERT( StartIdx == 0);

		i3VertexArray * pInstanceVA = m_InstancingInfo.m_pSecondVStream;
		IDirect3DVertexBuffer9 * pInstanceVB = (IDirect3DVertexBuffer9 *) pInstanceVA->GetInstance();
		UINT32 stride = pInstanceVA->GetStride();
		UINT32 baseOffset = m_InstancingInfo.m_Offset_Stream1 * stride;

		for( INT32 i = 0; i < m_InstancingInfo.count; i++)
		{
			CHECKRESULT( m_pD3DDevice->SetStreamSource( 1, pInstanceVB, baseOffset + (i * stride), 0));
			CHECKRESULT( m_pD3DDevice->DrawPrimitive( (D3DPRIMITIVETYPE) Prim, StartIdx, PrimCount));
		}
	}
	else
	{
		CHECKRESULT( m_pD3DDevice->DrawPrimitive( (D3DPRIMITIVETYPE) Prim, StartIdx, PrimCount));
	}

	return m_pCurShader;
}

i3Shader * i3RenderContext::DrawStripPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count)
{
	HRESULT Rv;
	UINT32 i;

	// Shader  瀏 카 풔磯.
	if( GetCaps()->getEnableShader() )
	{
		SelectShader( pShader);
	}

	for( i = 0; i < Count; i++)
	{
		m_DrawCallCount++;
		m_TriangleCount += *pPrimCount;

		Rv = m_pD3DDevice->DrawPrimitive( (D3DPRIMITIVETYPE) Prim, StartIdx, *pPrimCount);
		#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::DrawPrim()", Rv);
		}
		#endif

		StartIdx += i3Gfx::CalcVertexCount( Prim, * pPrimCount);

		pPrimCount++;
	}

	return m_pCurShader;
}

i3Shader * i3RenderContext::DrawIndexedPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount)
{
	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

	// Shader  瀏 카 풔磯.
	if( GetCaps()->getEnableShader() )
	{
		SelectShader( pShader);
	}

	if( IsRenderFlag(I3RENDER_FLAG_INSTANCING) && (m_InstancingMethod == I3G_INSTANCING_METHOD_STREAM))
	{
		I3ASSERT( StartIdx == 0);

		i3VertexArray * pInstanceVA = m_InstancingInfo.m_pSecondVStream;
		IDirect3DVertexBuffer9 * pInstanceVB = (IDirect3DVertexBuffer9 *) pInstanceVA->GetInstance();
		UINT32 stride = pInstanceVA->GetStride();
		UINT32 baseOffset = m_InstancingInfo.m_Offset_Stream1 * stride;

		for( INT32 i = 0; i < m_InstancingInfo.count; i++)
		{
			CHECKRESULT( m_pD3DDevice->SetStreamSource( 1, pInstanceVB, baseOffset + (i * stride), 0));
			CHECKRESULT( m_pD3DDevice->DrawIndexedPrimitive( (D3DPRIMITIVETYPE) Prim, 0, 0, m_pCurrentVA->GetCount(), StartIdx, PrimCount));
		}
	}
	else
	{
		CHECKRESULT( m_pD3DDevice->DrawIndexedPrimitive( (D3DPRIMITIVETYPE) Prim, 0, 0, m_pCurrentVA->GetCount(), StartIdx, PrimCount));
	}

	return m_pCurShader;
}

i3Shader * i3RenderContext::DrawStripIndexedPrim( i3Shader * pShader, I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 * pPrimCount, UINT32 Count)
{
	UINT32 i;
	HRESULT Rv;

	// Shader  瀏 카 풔磯.
	if( GetCaps()->getEnableShader() )
	{
		SelectShader( pShader);
	}

	for( i = 0; i < Count; i++)
	{
		m_DrawCallCount++;
		m_TriangleCount += *pPrimCount;

		Rv = m_pD3DDevice->DrawIndexedPrimitive( (D3DPRIMITIVETYPE) Prim, 0, 0, m_pCurrentVA->GetCount(), StartIdx, *pPrimCount);
		#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::DrawIndexedPrim()", Rv);
		}
		#endif

		StartIdx += i3Gfx::CalcVertexCount( Prim, *pPrimCount);

		pPrimCount++;
	}

	return m_pCurShader;
}

void i3RenderContext::DrawCommandList( i3GPUCommandList * pCmdList)
{
}

void i3RenderContext::DrawPointSprite( UINT32 StartIdx, UINT32 PrimCount)
{
	HRESULT Rv;

	m_DrawCallCount++;
	m_TriangleCount += PrimCount;

	// Shader  瀏 카 풔磯.
	if( GetCaps()->getEnableShader() )
	{
		SelectShader( nullptr);
	}

	Rv = m_pD3DDevice->DrawPrimitive( (D3DPRIMITIVETYPE) I3G_PRIM_POINTSPRITE, StartIdx, PrimCount);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3RenderContext::DrawPointSprite()", Rv);
	}
	#endif
}

void i3RenderContext::_SetCurrentViewportRect( INT32 x, INT32 y, INT32 width, INT32 height)
{
	m_curViewportRect.left		= x;
	m_curViewportRect.top		= y;
	m_curViewportRect.right		= width;
	m_curViewportRect.bottom	= height;

	m_CurrentRTWidth = width;
	m_CurrentRTHeight = height;
}

void i3RenderContext::SetCurrentRenderTarget( i3RenderTarget * pRT)
{
	HRESULT Rv;

	//if (IsRenderFlag(I3RENDER_FLAG_SWAPCHAIN_ENABLE))
		//return;

	if( pRT == nullptr)
	{
		pRT = GetDefaultRenderTarget();
	}

	I3ASSERT( pRT != nullptr);

	if(pRT->GetColorTexture() != nullptr)
	{
		IDirect3DSurface9 * pSurf = pRT->GetColorTexture()->getSurface(0)->getSurface();

		I3ASSERT( pSurf != nullptr);
		Rv = m_pD3DDevice->SetRenderTarget(0, pSurf);
#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetCurrentRenderTarget()", Rv);
			return;
		}
#endif

		pSurf = pRT->GetDepthTexture()->getSurface(0)->getSurface();
		I3ASSERT( pSurf != nullptr);

		Rv = m_pD3DDevice->SetDepthStencilSurface( pSurf);
#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetCurrentRenderTarget()", Rv);
			return;
		}
#endif
	}
	else
	{
		I3ASSERT( pRT->GetColorSurface() != nullptr);
		I3ASSERT( pRT->GetDepthSurface() != nullptr);

		Rv = m_pD3DDevice->SetRenderTarget( 0, pRT->GetColorSurface());
#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetCurrentRenderTarget()", Rv);
			return;
		}
#endif
		Rv = g_pD3DDevice->SetDepthStencilSurface( pRT->GetDepthSurface());
#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetCurrentRenderTarget()", Rv);
			return;
		}
#endif
	}

	SetViewport( 0, 0, pRT->GetWidth(), pRT->GetHeight());

	{
		_SetCurrentViewportRect( 0, 0, pRT->GetWidth(), pRT->GetHeight());

		CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE));
		CHECKRESULT( m_pD3DDevice->SetScissorRect( &m_curViewportRect));
	}
}

void i3RenderContext::SetColorRenderTarget( i3Texture * pTex)
{
	HRESULT Rv;

	I3ASSERT( pTex->GetUsageFlag() & I3G_USAGE_RENDERTARGET);

	i3Surface * pSurface;
	
	if( pTex->getSurfaceCount() > 0)
	{
		pSurface = pTex->getSurface( 0);
		Rv = m_pD3DDevice->SetRenderTarget( 0, pSurface->getSurface());
	}
	else
	{
		Rv = m_pD3DDevice->SetRenderTarget( 0, NULL);
	}

	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3RenderContext::SetCurrentRenderTarget()", Rv);
		return;
	}
	#endif

	SetViewport( 0, 0, pTex->GetWidth(), pTex->GetHeight());

	{
		_SetCurrentViewportRect( 0, 0, pTex->GetWidth(), pTex->GetHeight());

		CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE));
		CHECKRESULT( m_pD3DDevice->SetScissorRect( &m_curViewportRect));
	}
}

void i3RenderContext::SetDepthRenderTarget( i3Texture * pTex)
{
	HRESULT Rv;

	i3Surface * pSurface;
	
	if( pTex == nullptr)
	{
		Rv = m_pD3DDevice->SetDepthStencilSurface( m_pOriginalRenderTarget->GetDepthSurface());
	}
	else
	{
		if( pTex->getSurfaceCount() > 0)
		{
			pSurface = pTex->getSurface( 0);
			Rv = m_pD3DDevice->SetDepthStencilSurface( pSurface->getSurface());
		}
		else
		{
			Rv = m_pD3DDevice->SetDepthStencilSurface( NULL);
		}
	}

	
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3RenderContext::SetCurrentRenderTarget()", Rv);
		return;
	}
	#endif
}

void i3RenderContext::_InvalidateGfxResource( bool bLostDevice)
{
	i3GfxResource * pRes;
	i3RenderTarget * pNewRT;

	m_pCurrentVA = nullptr;
	m_pCurrentIA = nullptr;
	I3_SAFE_RELEASE( m_pCurShader);

	if( s_pGfxLostDeviceHandler != nullptr)
	{
		s_pGfxLostDeviceHandler( bLostDevice);
	}

	I3_SAFE_RELEASE( m_pLastFrameTargetTexture);

	for(size_t i = 0; i < m_RenderTargetList.size(); i++)
	{
		pNewRT = m_RenderTargetList[i];

		if( pNewRT->GetColorSurface() != nullptr)
		{
			pNewRT->GetColorSurface()->Release();
		}

		if( pNewRT->GetDepthSurface() != nullptr)
		{
			pNewRT->GetDepthSurface()->Release();
		}

		pNewRT->Release();
	}
	m_RenderTargetList.clear();

	I3_SAFE_RELEASE_NODBG( m_pSwapChain);

	pRes = i3GfxResource::getHeadOfGfxResource();

	while( pRes != nullptr)
	{
		pRes->Destroy( bLostDevice);

		pRes = pRes->getNextRes();
	}

	pRes = i3GfxResource::getHeadOfGfxResource();

	while( pRes != nullptr)
	{
		if( !(pRes->isDestroyed()))
		{
			i3ClassMeta * pMeta = pRes->meta();

			if( pRes->GetUsageFlag() & (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE) ||
				pRes->GetUsageFlag() & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY))
			{
				I3TRACE("GfxResource Not Destroyed!! -> ""%s"" ""%s"" \n", pRes->GetName(), pMeta->class_name()); 
			}
		}

		pRes = pRes->getNextRes();
	}
}

void i3RenderContext::_ValidateGfxResource(void)
{
	i3GfxResource * pRes = i3GfxResource::getHeadOfGfxResource();

	while( pRes != nullptr)
	{
		pRes->Revive( this);

		pRes = pRes->getNextRes();
	}

	if( s_pGfxReviveHandler != nullptr)
	{
		s_pGfxReviveHandler( this);
	}
}

bool i3RenderContext::SetRenderTargetSize( INT32 cx, INT32 cy)
{
	if( m_Switch.m_bIgnoreResize)
		return true;

	if((cx <= 0) || (cy <= 0))
		return false;

	m_d3dpp.BackBufferWidth = cx;
	m_d3dpp.BackBufferHeight = cy;

	OnResetDevice();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render State


void i3RenderContext::SetAlphaTestEnable( bool flag)
{
	flag?addRenderFlag(I3RENDER_FLAG_ALPHATEST_ENABLE):removeRenderFlag(I3RENDER_FLAG_ALPHATEST_ENABLE);

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, (DWORD) flag));
}

bool i3RenderContext::GetAlphaTestEnable(void)
{
	return IsRenderFlag(I3RENDER_FLAG_ALPHATEST_ENABLE);
}

void i3RenderContext::SetBlendEnable( bool flag)
{
	flag?addRenderFlag(I3RENDER_FLAG_BLEND_ENABLE):removeRenderFlag(I3RENDER_FLAG_BLEND_ENABLE);

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) flag));
}

bool i3RenderContext::GetBlendEnable(void)
{
	return IsRenderFlag(I3RENDER_FLAG_BLEND_ENABLE);
}

void i3RenderContext::SetAlphaRefValue( UINT8 val)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF, (DWORD) val));
}

UINT8 i3RenderContext::GetAlphaRefValue(void)
{
	DWORD val;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ALPHAREF, (DWORD *) &val));

	return (UINT8) val;
}

void i3RenderContext::SetAlphaFunction( I3G_COMP_FUNC func)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, (DWORD) func));
}

I3G_COMP_FUNC i3RenderContext::GetAlphaFunction(void)
{
	I3G_COMP_FUNC val;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ALPHAFUNC, (DWORD *) & val));

	return val;
}

// Z-Buffer
void i3RenderContext::SetZTestEnable( bool flag)
{
	flag?addRenderFlag(I3RENDER_FLAG_ZTEST_ENABLE):removeRenderFlag(I3RENDER_FLAG_ZTEST_ENABLE);

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, (DWORD) flag));
}

bool i3RenderContext::GetZTestEnable(void)
{
	return IsRenderFlag(I3RENDER_FLAG_ZTEST_ENABLE);
}

void i3RenderContext::SetZWriteEnable( bool flag)
{
	flag?addRenderFlag(I3RENDER_FLAG_ZWRITE_ENABLE):removeRenderFlag(I3RENDER_FLAG_ZWRITE_ENABLE);

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, (DWORD) flag));
}

bool i3RenderContext::GetZWriteEnable(void)
{
	return IsRenderFlag(I3RENDER_FLAG_ZWRITE_ENABLE);
}

void i3RenderContext::SetZFunction( I3G_COMP_FUNC	func)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ZFUNC, (DWORD) func));
}

I3G_COMP_FUNC i3RenderContext::GetZFunction(void)
{
	I3G_COMP_FUNC func;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ZFUNC, (DWORD *) & func));
	
	return func;
}

void i3RenderContext::SetZBias( REAL32 fSlope, REAL32 fBias)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, (DWORD) *((DWORD *)&fSlope)));
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_DEPTHBIAS, (DWORD) *((DWORD *)&fBias)));
}

// Render Parameters
void i3RenderContext::SetRenderMode( I3G_RENDER_MODE	mode)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FILLMODE, (DWORD) mode));
}

I3G_RENDER_MODE i3RenderContext::GetRenderMode(void)
{
	I3G_RENDER_MODE mode;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FILLMODE, (DWORD *) & mode));

	return mode;
}

void i3RenderContext::SetShadeMode( I3G_SHADE_MODE	mode)
{
	if( m_pCaps->getEnableShader() == false)
	{
		CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SHADEMODE, (DWORD) mode));
	}
		
	if( !IsRenderFlag(I3RENDER_FLAG_PPL_ENABLE) || ( mode == I3G_SHADE_GOURAUD))
	{
		// Per Pixel Lighting 활화
		m_ShadeMode = I3G_SHADE_GOURAUD;
		m_ShaderCode.setPPLShader( false);
	}
	else
	{
		// Per Pixel Lighting 활화
		m_ShadeMode = mode;
		m_ShaderCode.setPPLShader( true);
	}
}

I3G_SHADE_MODE i3RenderContext::GetShadeMode(void)
{
	return m_ShadeMode;
}

void i3RenderContext::SetFaceCullMode( I3G_FACE_CULL	mode)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, (DWORD) mode));
}

I3G_FACE_CULL i3RenderContext::GetFaceCullMode(void)
{
	I3G_FACE_CULL mode;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_CULLMODE, (DWORD *) & mode));

	return mode;
}

// Blend State
void i3RenderContext::SetSrcBlend( I3G_BLEND	blend)
{
	m_SrcBlend = blend;

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, blend));
}

I3G_BLEND i3RenderContext::GetSrcBlend(void)
{
	return m_SrcBlend;
}

void i3RenderContext::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, (DWORD) blend));
}

I3G_BLEND i3RenderContext::GetDestBlend(void)
{
	return m_DestBlend;
}

void i3RenderContext::SetBlendOperation( I3G_BLEND_OP blendop)
{
	m_BlendOp = blendop;

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_BLENDOP, (DWORD) blendop));
}

I3G_BLEND_OP i3RenderContext::GetBlendOperation( void)
{
	return m_BlendOp;
}

void i3RenderContext::SetBlendFactor( COLOR * pCol)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_BLENDFACTOR, (DWORD) pCol->GetARGB()));
}

// Fog
void i3RenderContext::SetFogEnable( bool bFlag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGENABLE, (DWORD) bFlag));
}

bool i3RenderContext::GetFogEnable(void)
{
	bool rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGENABLE, (DWORD *) & rv));

	return rv;
}
// FIXME: Optimization - Earring
void i3RenderContext::SetFogColor( COLOR * pColor)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGCOLOR, (DWORD) pColor->GetARGB()));
}

void i3RenderContext::GetFogColor( COLOR * pColor)
{
	DWORD val;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGCOLOR, (DWORD *) &val));

	i3Color::Set( pColor, (UINT8)((val >> 8) & 0xFF),
							(UINT8)((val >> 16) & 0xFF),
							(UINT8)((val >> 24) & 0xFF),
							(UINT8)(val & 0xFF));
}

void i3RenderContext::SetFogMode( I3G_FOG_MODE mode)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGTABLEMODE, (DWORD) mode));
}

I3G_FOG_MODE i3RenderContext::GetFogMode(void)
{
	I3G_FOG_MODE mode;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGTABLEMODE, (DWORD *) &mode));

	return mode;
}

void i3RenderContext::SetFogTableEnable( bool /*bFlag*/)
{
}

bool i3RenderContext::GetFogTableEnable(void)
{
	return false;
}

void i3RenderContext::SetFogNearFar( REAL32 fNear, REAL32 fFar)
{
	DWORD sp1, sp2;

	#if defined( I3_FIXED_POINT)
		REAL32 f1, f2;

		f1 = i3FXD::ToReal( fNear);
		f2 = i3FXD::ToReal( fFar);

		sp1 = *((DWORD *) & f1);
		sp2 = *((DWORD *) & f2);
	#else
		sp1 = *((DWORD *) & fNear);
		sp2 = *((DWORD *) & fFar);
	#endif

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGSTART, sp1));

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGEND, sp2));
}

REAL32 i3RenderContext::GetFogNear(void)
{
	REAL32 fNear;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGSTART, (DWORD *) & fNear));
	
	#if defined( I3_FIXED_POINT)
		return i3FXD::FromReal( fNear);
	#else
		return fNear;
	#endif
}

REAL32 i3RenderContext::GetFogFar(void)
{
	REAL32 fFar;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGEND, (DWORD *) & fFar));

	#if defined( I3_FIXED_POINT)
		return i3FXD::FromReal( fFar);
	#else
		return fFar;
	#endif
}

void i3RenderContext::SetFogDensity( REAL32 fVal)
{
	#if defined( I3_FIXED_POINT)
		REAL32 f1 = i3FXD::ToReal( fVal);
		DWORD sp = *((DWORD *) &f1);
	#else
		DWORD sp = *((DWORD *) &fVal);
	#endif

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGDENSITY, sp));
}

REAL32 i3RenderContext::GetFogDensity(void)
{
	REAL32 rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGDENSITY, (DWORD *) &rv));

	#if defined( I3_FIXED_POINT)
		return i3FXD::FromReal( rv);
	#else
		return rv;
	#endif
}

// Anti-aliase
void i3RenderContext::SetEdgeAntiAliasEnable( bool bFlag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, (DWORD) bFlag));
}

bool i3RenderContext::GetEdgeAntiAliasEnable(void)
{
	bool rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ANTIALIASEDLINEENABLE, (DWORD *) & rv));

	return rv;
}

// Stencil Test
bool i3RenderContext::GetStencilAvailable(void)
{
	return (m_ClearFlag & I3G_CLEAR_STENCIL) != 0;
}

void i3RenderContext::SetStencilEnable( bool bFlag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILENABLE, (DWORD) bFlag));
}

bool i3RenderContext::GetStencilEnable(void)
{
	bool rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_STENCILENABLE, (DWORD *) & rv));

	return rv;
}

void i3RenderContext::SetStencilOperation( I3G_STENCIL_OP SFail, I3G_STENCIL_OP SPass, I3G_STENCIL_OP ZFail)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILFAIL, SFail));

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILZFAIL, ZFail));

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILPASS, SPass));
}

I3G_STENCIL_OP i3RenderContext::GetStencilOperationSFail(void)
{
	I3G_STENCIL_OP op;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_STENCILFAIL, (DWORD *) &op));

	return op;
}

I3G_STENCIL_OP i3RenderContext::GetStencilOperationSPass(void)
{
	I3G_STENCIL_OP op;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_STENCILPASS, (DWORD *) & op));

	return op;
}

I3G_STENCIL_OP i3RenderContext::GetStencilOperationZFail(void)
{
	I3G_STENCIL_OP op;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_STENCILZFAIL, (DWORD *) & op));

	return op;
}

void i3RenderContext::SetStencilFunction( I3G_COMP_FUNC	func)			// default == I3G_STENCIL_FUNC_ALWAYS
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILFUNC, (DWORD) func));
}

I3G_COMP_FUNC i3RenderContext::GetStencilFunction( void)
{
	I3G_COMP_FUNC func;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_STENCILFUNC, (DWORD *) &func));

	return func;
}

void i3RenderContext::SetStencilRefValue( UINT8 val)							// default == 0
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILREF, (DWORD) val));
}

UINT8 i3RenderContext::GetStencilRefValue(void)
{
	DWORD rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_STENCILREF, (DWORD *) & rv));

	return (UINT8) rv;
}

void i3RenderContext::SetStencilReadMask( UINT8 MaskVal)						// default == 0xFF
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILMASK, (DWORD) MaskVal));
}

UINT8 i3RenderContext::GetStencilReadMask(void)
{
	DWORD rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_STENCILMASK, (DWORD *) & rv));

	return (UINT8) rv;
}

void i3RenderContext::SetStencilWriteMask( UINT8 MaskVal)						// default == 0xFF
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, (DWORD) MaskVal));
}

UINT8 i3RenderContext::GetStencilWriteMask(void)
{
	DWORD rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_STENCILWRITEMASK, (DWORD *) &rv));

	return (UINT8) rv;
}

// Clipping
void i3RenderContext::SetClippingEnable( bool bFlag)							// default == true
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_CLIPPING, (DWORD) bFlag));
}

bool i3RenderContext::GetClippingEnable(void)
{
	bool rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_CLIPPING, (DWORD *) &rv));

	return rv;
}


// Lighting
void i3RenderContext::SetLightingEnable( bool bFlag)							// default == false
{
	//CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, (DWORD) bFlag));

	bFlag ? addRenderFlag(I3RENDER_FLAG_LIGHTING_ENABLE) : removeRenderFlag( I3RENDER_FLAG_LIGHTING_ENABLE);

	m_ShaderCode.setLightingEnable( bFlag);
}

bool i3RenderContext::GetLightingEnable(void)
{
	return IsRenderFlag( I3RENDER_FLAG_LIGHTING_ENABLE);
}

// Vertex Color Enable
void i3RenderContext::SetVertexColorEnable( bool bFlag)						// default == true
{
	bFlag ? addRenderFlag( I3RENDER_FLAG_VERTEXCOLOR_ENABLE) : removeRenderFlag( I3RENDER_FLAG_VERTEXCOLOR_ENABLE);
	//CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_COLORVERTEX, (DWORD) bFlag));
}

bool i3RenderContext::GetVertexColorEnable(void)
{
	bool rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_COLORVERTEX, (DWORD *) & rv));

	return rv;
}

// Vertex Color Write
void i3RenderContext::SetColorWriteEnable( UINT32 Mask)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE, (DWORD)Mask));
}

UINT32 i3RenderContext::GetColorWriteEnable( void)
{
	DWORD rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_COLORWRITEENABLE, (DWORD *) &rv));

	return (UINT32) rv;
}

// Normalize Normal Enable
void i3RenderContext::SetNormalizeNormalEnable( bool bFlag)					// default == false
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, (DWORD) bFlag));
}

bool i3RenderContext::GetNormalizeNormalEnable(void)
{
	bool rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_NORMALIZENORMALS, (DWORD *) &rv));

	return rv;
}

// Material
void i3RenderContext::SetMaterial(	COLORREAL *	pDiffuseColor,
									COLORREAL * pSpecularColor,
									COLORREAL * pEmissiveColor,
									REAL32		Shininess,
									UINT32		ValidMask)
{
	D3DMATERIAL9 mat;

	if( ValidMask & I3G_MATERIAL_DIFFUSE)
		i3Color::Set( &m_MaterialDiffuse, pDiffuseColor);

	if( ValidMask & I3G_MATERIAL_SPECULAR)
		i3Color::Set( &m_MaterialSpecular, pSpecularColor);

	if( ValidMask & I3G_MATERIAL_EMISSIVE)
		i3Color::Set( &m_MaterialEmissive, pEmissiveColor);

	if( ValidMask & I3G_MATERIAL_SHININESS)
		m_MaterialShininess = Shininess;

	if( m_pCaps->getEnableShader() == false)
	{
		//i3Color::Set( (COLORREAL *) &mat.Ambient, &m_MaterialAmbient);
		i3Color::Set( (COLORREAL *) &mat.Diffuse, &m_MaterialDiffuse);
		i3Color::Set( (COLORREAL *) &mat.Specular, &m_MaterialSpecular);
		i3Color::Set( (COLORREAL *) &mat.Emissive, &m_MaterialEmissive);

		mat.Power = m_MaterialShininess;

		if( Shininess == 0.0f)
		{
			// Specular Disable
			CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SPECULARENABLE, (DWORD) FALSE));
		}
		else
		{
			CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SPECULARENABLE, (DWORD) TRUE));
		}

		CHECKRESULT( m_pD3DDevice->SetMaterial( &mat));
	}
	else
	{
		if( m_pCurShader != nullptr)
			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_MATERIAL);
	}
}


void i3RenderContext::setAmbient( COLORREAL * pCol)
{
	if( m_pCurShader != nullptr)
		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_AMBIENT);

	i3Color::Set( &m_Ambient, pCol);
}

void i3RenderContext::SetColor( COLORREAL * pColor)
{
	if( m_pCaps->getEnableShader() == false)
	{
		//CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, (DWORD) pColor->GetARGB()));
	}
	else
	{
		if( m_pCurShader != nullptr)
			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_COLOR);
	}

	i3Color::Set( &m_Color, pColor);
}

void i3RenderContext::setSecondaryColor( COLORREAL * pCol)
{
	i3Color::Set( &m_SecondaryColor, pCol);

	if( m_pCurShader != nullptr)
		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_SECONDARY_COLOR);
}

void i3RenderContext::setTetraColor( COLORREAL * pCol)
{
	i3Color::Set( &m_TetraColor, pCol);

	if( m_pCurShader != nullptr)
		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TETRA_COLOR);
}

// Vertex Blend Mode
void i3RenderContext::SetVertexBlendEnable( bool bFlag)						// default == true
{
	if( m_pCaps->getEnableShader() == false)
	{
		CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, (DWORD) bFlag));
	}
}

bool i3RenderContext::GetVertexBlendEnable(void)
{
	bool rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, (DWORD *) &rv));

	return rv;
}

void i3RenderContext::SetVertexBlendWeightCount( INT32 count)
{
	m_VertexBlendWeightCount = count;
	static D3DVERTEXBLENDFLAGS s_vbflags[5] = 
	{
		D3DVBF_DISABLE,
		D3DVBF_0WEIGHTS,
		D3DVBF_1WEIGHTS,
		D3DVBF_2WEIGHTS,
		D3DVBF_3WEIGHTS,
	};

	if( m_pCaps->getEnableShader() == false)
	{
		CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_VERTEXBLEND, (DWORD) s_vbflags[ count]));
	}
}

void i3RenderContext::SetSoftwareVertexBlendEnable( bool bFlag)				// default == false
{
	if( m_pCaps->getEnableShader() == false)
	{
		if( s_bSoftwareProcess == true)
			bFlag = true;

		#if defined( I3_DEBUG_SHADER)
			//CHECKRESULT( m_pD3DDevice->SetSoftwareVertexProcessing( TRUE));
		#else
			CHECKRESULT( m_pD3DDevice->SetSoftwareVertexProcessing( bFlag ? TRUE : FALSE));
		#endif
	}
}


bool i3RenderContext::GetSoftwareVertexBlendEnable(void)
{
	return m_pD3DDevice->GetSoftwareVertexProcessing() ? true : false;
}


// Texture State
void	i3RenderContext::SetTextureEnableImpl( I3G_TEXTURE_BIND bind, bool bState)
{
	switch( bind)
	{
		case I3G_TEXTURE_BIND_DIFFUSE :	
			m_ShaderCode.setDiffuseMapEnable( bState);	

			if( m_pCurShader != nullptr)
				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_DIFFUSE);
			break;

		case I3G_TEXTURE_BIND_SPECULAR :	
			bState = bState && getGfxOption()->getSpecularMapEnable();

			m_ShaderCode.setSpecularMapEnable( bState);

			if( m_pCurShader != nullptr)
				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_SPECULAR);
			break;

		case I3G_TEXTURE_BIND_NORMAL :		
			bState = bState && getGfxOption()->getNormalMapEnable();
			m_ShaderCode.setNormalMapEnable( bState);

			if( m_pCurShader != nullptr)
				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_NORMAL);
			break;

		case I3G_TEXTURE_BIND_LUX :	
			bState = bState && getGfxOption()->getLuxMapEnable();
			m_ShaderCode.setLuxMapEnable( bState);

			if( m_pCurShader != nullptr)
				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_LUX);
			break;

		case I3G_TEXTURE_BIND_REFLECT :
			bState = bState && getGfxOption()->getEnvironmentMapEnable();
			m_ShaderCode.setReflectMapEnable( bState);

			if( m_pCurShader != nullptr)
				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT);
			break;

		case I3G_TEXTURE_BIND_REFLECT_MASK :
			bState = bState && getGfxOption()->getEnvironmentMapEnable();
			m_ShaderCode.setReflectMaskMapEnable( bState);

			if( m_pCurShader != nullptr)
				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT_MASK);
			break;

		case I3G_TEXTURE_BIND_EMISSIVE :
			bState = bState && getGfxOption()->getEmissiveMapEnable();
			m_ShaderCode.setEmissiveMapEnable( bState);

			if( m_pCurShader != nullptr)
				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_EMISSIVE);
			break;
	}

	m_TexBindInfo[ bind].m_bEnable = bState;
	m_TexBindInfo[ bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_ENABLE;
		
}



// Texture State
void i3RenderContext::SetTextureEnable( I3G_TEXTURE_BIND bind, bool bState)
{
	i3RenderContext::SetTextureEnableImpl( bind, bState);		// 원래버전을 먼저 호출하고...
	I3G_TEXTURE_BIND_CONTEXT*	bind_context = &m_TexBindInfo[bind];
	bind_context->m_bEnableChangedByChangeTex = false;
}

void	i3RenderContext::SetTextureEnableByBindAttr(  I3G_TEXTURE_BIND bind, bool bState)
{
	I3G_TEXTURE_BIND_CONTEXT*	bind_context = &m_TexBindInfo[bind];
	
	if ( bState == bind_context->m_bEnable )
		return;

	i3RenderContext::SetTextureEnableImpl(bind, bState);
	bind_context->m_bEnableChangedByChangeTex = !bind_context->m_bEnableChangedByChangeTex;
}


void	i3RenderContext::SetTexture( I3G_TEXTURE_BIND bind, i3Texture * pTexture, bool bTextureChange)
{
	if (m_TexBindInfo[bind].m_bEnableChangedByChangeTex == true)
	{
		if (bTextureChange == false)
		{
			if (bind != I3G_TEXTURE_BIND_REFLECT)
				i3RenderContext::SetTextureEnable( bind, !(m_TexBindInfo[bind].m_bEnable) );

			if (bind == I3G_TEXTURE_BIND_REFLECT_MASK)
			{
				if (m_TexBindInfo[I3G_TEXTURE_BIND_REFLECT].m_bEnableChangedByChangeTex == true)
					i3RenderContext::SetTextureEnable( I3G_TEXTURE_BIND_REFLECT, !(m_TexBindInfo[I3G_TEXTURE_BIND_REFLECT].m_bEnable) );
			}
		}
	}

	if( pTexture != nullptr)
	{
		if( pTexture->GetFullLoadedTexture() != nullptr)
			pTexture = pTexture->GetFullLoadedTexture();

		if( pTexture->GetWidth() <= 0)
			pTexture = nullptr;
	}

	m_TexBindInfo[ bind].m_pTexture = pTexture;
	m_TexBindInfo[ bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_TEXTURE;

	if( m_pCurShader != nullptr)
	{
		switch( bind)
		{
			case I3G_TEXTURE_BIND_DIFFUSE :
			case I3G_TEXTURE_BIND_DIFFUSE1 :
			case I3G_TEXTURE_BIND_DIFFUSE2 :
			case I3G_TEXTURE_BIND_DIFFUSE3 :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_DIFFUSE);	break;

			case I3G_TEXTURE_BIND_SPECULAR :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_SPECULAR);	break;
			case I3G_TEXTURE_BIND_REFLECT :			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT);	break;
			case I3G_TEXTURE_BIND_NORMAL :			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_NORMAL);	break;
			case I3G_TEXTURE_BIND_LUX :				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_LUX);	break;
			case I3G_TEXTURE_BIND_EMISSIVE :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_EMISSIVE);	break;
			case I3G_TEXTURE_BIND_REFLECT_MASK :	m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT_MASK);	break;
		}
	}
}

void	i3RenderContext::SetTextureWrap( I3G_TEXTURE_BIND bind, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap)
{
	m_TexBindInfo[ bind].m_HWrap = HorzWrap;
	m_TexBindInfo[ bind].m_VWrap = VertWrap;
	m_TexBindInfo[ bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_WRAP;

	if( bind == I3G_TEXTURE_BIND_DIFFUSE)
	{
		// DiffuseMap에 대한 Filter 및 Wrapping 설정은 Emissive Map에서도 그대로 공유한다.
		SetTextureWrap( I3G_TEXTURE_BIND_EMISSIVE, HorzWrap, VertWrap);
	}

	if( m_pCurShader != nullptr)
	{
		switch( bind)
		{
			case I3G_TEXTURE_BIND_DIFFUSE :
			case I3G_TEXTURE_BIND_DIFFUSE1 :
			case I3G_TEXTURE_BIND_DIFFUSE2 :
			case I3G_TEXTURE_BIND_DIFFUSE3 :
				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_DIFFUSE);
				break;

			case I3G_TEXTURE_BIND_SPECULAR :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_SPECULAR);	break;
			case I3G_TEXTURE_BIND_REFLECT :			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT);	break;
			case I3G_TEXTURE_BIND_NORMAL :			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_NORMAL);	break;
			case I3G_TEXTURE_BIND_LUX :				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_LUX);	break;
			case I3G_TEXTURE_BIND_EMISSIVE :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_EMISSIVE);	break;
			case I3G_TEXTURE_BIND_REFLECT_MASK :	m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT_MASK);	break;
		}
	}
}

void i3RenderContext::SetTextureBorderColor( I3G_TEXTURE_BIND bind, COLOR * pColor)
{
	m_TexBindInfo[ bind].m_BorderColor = i3Color::ConvertARGB32( pColor);
	m_TexBindInfo[ bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_BORDER_COLOR;

	if( m_pCurShader != nullptr)
	{
		switch( bind)
		{
			case I3G_TEXTURE_BIND_DIFFUSE :
			case I3G_TEXTURE_BIND_DIFFUSE1 :
			case I3G_TEXTURE_BIND_DIFFUSE2 :
			case I3G_TEXTURE_BIND_DIFFUSE3 :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_DIFFUSE);	break;

			case I3G_TEXTURE_BIND_SPECULAR :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_SPECULAR);	break;
			case I3G_TEXTURE_BIND_REFLECT :			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT);	break;
			case I3G_TEXTURE_BIND_NORMAL :			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_NORMAL);	break;
			case I3G_TEXTURE_BIND_LUX :				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_LUX);	break;
			case I3G_TEXTURE_BIND_EMISSIVE :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_EMISSIVE);	break;
			case I3G_TEXTURE_BIND_REFLECT_MASK :	m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT_MASK);	break;
		}
	}
}

void	i3RenderContext::SetTextureFilter( I3G_TEXTURE_BIND bind, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter)
{
	m_TexBindInfo[ bind].m_MinFilter = MinFilter;
	m_TexBindInfo[ bind].m_MagFilter = MagFilter;
	m_TexBindInfo[ bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_FILTER;

	if( bind == I3G_TEXTURE_BIND_DIFFUSE)
	{
		// DiffuseMap에 대한 Filter 및 Wrapping 설정은 Emissive Map에서도 그대로 공유한다.
		SetTextureFilter( I3G_TEXTURE_BIND_EMISSIVE, MagFilter, MinFilter);
	}

	if( m_pCurShader != nullptr)
	{
		switch( bind)
		{
			case I3G_TEXTURE_BIND_DIFFUSE :
			case I3G_TEXTURE_BIND_DIFFUSE1 :
			case I3G_TEXTURE_BIND_DIFFUSE2 :
			case I3G_TEXTURE_BIND_DIFFUSE3 :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_DIFFUSE);	break;

			case I3G_TEXTURE_BIND_SPECULAR :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_SPECULAR);	break;
			case I3G_TEXTURE_BIND_REFLECT :			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT);	break;
			case I3G_TEXTURE_BIND_NORMAL :			m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_NORMAL);	break;
			case I3G_TEXTURE_BIND_LUX :				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_LUX);	break;
			case I3G_TEXTURE_BIND_EMISSIVE :		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_EMISSIVE);	break;
			case I3G_TEXTURE_BIND_REFLECT_MASK :	m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_REFLECT_MASK);	break;
		}
	}
}

void	i3RenderContext::SetTextureCoordSource( I3G_TEXTURE_BIND bind, I3G_TEXTURE_COORD_SOURCE source)
{
}


void	i3RenderContext::SetTextureCoordScale( I3G_TEXTURE_BIND bind, VEC3D * pScale, INT32 CoordCount)
{
}

void	i3RenderContext::SetTextureCoordOffset( I3G_TEXTURE_BIND bind, VEC3D * pOffset, INT32 CoordCount)
{
}

void	i3RenderContext::SetTextureMatrix( I3G_TEXTURE_BIND bind, MATRIX * pMatrix, INT32 CoordCount, bool bProjective)
{
	i3Matrix::Copy( &m_TexBindInfo[ bind].m_Matrix, pMatrix);
	m_TexBindInfo[bind].m_CoordCount = CoordCount;
	m_TexBindInfo[bind].m_bProjective = bProjective;
	m_TexBindInfo[ bind].m_ModifyFlag |= I3G_TEXTURE_MODIFY_MATRIX;

	if( m_pCurShader != nullptr)
	{
		switch( bind)
		{
			case I3G_TEXTURE_BIND_DIFFUSE :
			case I3G_TEXTURE_BIND_DIFFUSE1 :
			case I3G_TEXTURE_BIND_DIFFUSE2 :
			case I3G_TEXTURE_BIND_DIFFUSE3 :		
				if( i3Matrix::IsIdentity( pMatrix))
					m_ShaderCode.setTexCoordTransform( false);
				else
					m_ShaderCode.setTexCoordTransform( true);

				m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_MATRIX_TEX);	
				break;
		}
	}
}

void	i3RenderContext::NotifyTextureBindChange(void)
{
	INT32 i = 0;

	for(i=0; i<I3G_TEXTURE_BIND_MAX; i++)
	{
		m_TexBindInfo[ i].m_ModifyFlag = 0xffffffff;
	}

	if( m_pCurShader != nullptr)
	{
		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_TEX_ALL);
	}	
}

/*
void i3RenderContext::_UpdateTextureBind(void)
{
	I3G_TEXTURE_BIND_CONTEXT * pCtx = & m_TexBindInfo[ I3G_TEXTURE_BIND_DIFFUSE];

	if( pCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_TEXTURE)
		_SetTexture( 0, pCtx->m_pTexture);

	if( pCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_WRAP)
		_SetTextureWrap( 0, pCtx->m_HWrap, pCtx->m_VWrap);

	if( pCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_FILTER)
		_SetTextureFilter( 0, pCtx->m_MagFilter, pCtx->m_MinFilter);

	if( pCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_BORDER_COLOR)
		_SetTextureBorderColor( 0, pCtx->m_BorderColor);

	if( pCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_FUNCTION)
		_SetTextureFunction( 0, pCtx->m_Function);

	if( pCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_MATRIX)
		_SetTextureMatrix( 0, & pCtx->m_Matrix, pCtx->m_CoordCount, pCtx->m_bProjective);

	pCtx->m_ModifyFlag = 0;
	removeRenderFlag( I3RENDER_FLAG_TEXTURE_MODIFIED);
}
*/

void i3RenderContext::_SetTexture( INT32 StageIdx, i3Texture * pTexture)
{
	if( pTexture != nullptr)
	{
		i3TextureDX * pDXTex = (i3TextureDX *) pTexture;

		if( pDXTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED)
		{
			i3Clut * pClut = pDXTex->GetClut( 0);

			CHECKRESULT( m_pD3DDevice->SetPaletteEntries( 0, (PALETTEENTRY *) pClut->GetData()));

			CHECKRESULT( m_pD3DDevice->SetCurrentTexturePalette( 0));

			//if( m_UsedClutCount < 16)
				m_UsedClutCount++;
		}

		CHECKRESULT( m_pD3DDevice->SetTexture( StageIdx, pDXTex->GetDXImage()));
	}
	else
	{
		//CHECKRESULT( m_pD3DDevice->SetTexture( StageIdx, nullptr));
	}
}

static UINT32 s_FilterTable[] =
{
	D3DTEXF_NONE,
	D3DTEXF_POINT,
	D3DTEXF_LINEAR,
	D3DTEXF_POINT,
	D3DTEXF_POINT,
	D3DTEXF_LINEAR,
	D3DTEXF_LINEAR,
	D3DTEXF_ANISOTROPIC,
	D3DTEXF_PYRAMIDALQUAD,
	D3DTEXF_GAUSSIANQUAD
};

static UINT32 s_MipFilterTable[] =
{
	D3DTEXF_NONE,
	D3DTEXF_NONE,
	D3DTEXF_NONE,
	D3DTEXF_POINT,
	D3DTEXF_LINEAR,
	D3DTEXF_POINT,
	D3DTEXF_LINEAR,
	D3DTEXF_LINEAR,
	D3DTEXF_LINEAR,
	D3DTEXF_LINEAR
};

void i3RenderContext::_SetTextureFilter( INT32 StageIdx, I3G_TEXTURE_FILTER MagFilter, I3G_TEXTURE_FILTER MinFilter)
{
	if( g_GfxOption.getForcedMipmapFilter() != I3G_TEXTURE_FILTER_NONE)
	{
		MinFilter = g_GfxOption.getForcedMipmapFilter();
	}

	CHECKRESULT( m_pD3DDevice->SetSamplerState( StageIdx, D3DSAMP_MAGFILTER, s_FilterTable[ MagFilter]));
	CHECKRESULT( m_pD3DDevice->SetSamplerState( StageIdx, D3DSAMP_MINFILTER, s_FilterTable[ MinFilter]));

	if( MinFilter >= I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT)
	{
		// Enable Mipmap
		CHECKRESULT( m_pD3DDevice->SetSamplerState( StageIdx, D3DSAMP_MIPFILTER, s_MipFilterTable[ MinFilter]));	
	}
	else
	{
		// Disable Mipmap
		CHECKRESULT( m_pD3DDevice->SetSamplerState( StageIdx, D3DSAMP_MIPFILTER, D3DTEXF_NONE));
	}

	if( (MinFilter == I3G_TEXTURE_FILTER_ANISOTROPIC) || (MagFilter == I3G_TEXTURE_FILTER_ANISOTROPIC))
	{
		CHECKRESULT( m_pD3DDevice->SetSamplerState( StageIdx, D3DSAMP_MAXANISOTROPY, 16));
	}
}

void i3RenderContext::_SetTextureWrap( INT32 StageIdx, I3G_TEXTURE_WRAP HorzWrap, I3G_TEXTURE_WRAP VertWrap)
{
	CHECKRESULT( m_pD3DDevice->SetSamplerState( StageIdx, D3DSAMP_ADDRESSU, HorzWrap));
	CHECKRESULT( m_pD3DDevice->SetSamplerState( StageIdx, D3DSAMP_ADDRESSV, VertWrap));
}

void i3RenderContext::_SetTextureBorderColor( INT32 idxStage, UINT32 color)
{
	CHECKRESULT( m_pD3DDevice->SetSamplerState( idxStage, D3DSAMP_BORDERCOLOR, color));
}

void i3RenderContext::_SetTextureCoordSource( INT32 StageIdx, I3G_TEXTURE_COORD_SOURCE source)
{
	CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIdx, D3DTSS_TEXCOORDINDEX, source));
}

void i3RenderContext::_SetTextureFunction( INT32 StageIdx, I3G_TEXTURE_FUNCTION func)
{
	m_TraceOpMask &= ~TEXMASK_OP;

	switch( func)
	{
		case I3G_TEXTURE_FUNC_ADD :
			CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIdx, D3DTSS_COLOROP, D3DTOP_ADD));
			CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIdx, D3DTSS_ALPHAOP, D3DTOP_ADD));
			m_TraceOpMask |= TEXOP_ADD;
			break;

//		case I3G_TEXTURE_FUNC_MODULATE :
		default:
			CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIdx, D3DTSS_COLOROP, D3DTOP_MODULATE));
			CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIdx, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
			m_OldTexOpMask |= TEXOP_MODULATE;
			break;

		case I3G_TEXTURE_FUNC_SPECULAR_HIGHLIGHT :
			CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIdx, D3DTSS_COLOROP, D3DTOP_MODULATECOLOR_ADDALPHA));
			CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIdx, D3DTSS_ALPHAOP, D3DTOP_MODULATECOLOR_ADDALPHA));
			m_TraceOpMask |= TEXOP_SPECULAR;
			break;
	}

	// Render Context Trace求 Texture Operator  牝퓐
	// 珂화求 퓜結 訃 Texture Operator  溝 峨먈?
	//  SetTextureStageState() 獨 호, Texture Operator 求 
	//  Rendering SetVertexArray() 獨 Call 甄.
	
}
 
void i3RenderContext::_SetTextureCoordScale( INT32 StageIndex, VEC3D * pScale, INT32 CoordCount)
{
	HRESULT Rv;
	D3DTRANSFORMSTATETYPE Index;
	DWORD Flag;

	I3ASSERT((StageIndex >= 0) && (StageIndex < 8));

	Index = (D3DTRANSFORMSTATETYPE) s_TextureMatrixTable[ StageIndex];

	if( CoordCount <= 0)
	{
		// Disable Texture Matrix
		CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIndex, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE));
	}
	else
	{
		MATRIX tmp;

		Flag = s_TextureFlagTable[ CoordCount - 1];
		
		CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIndex, D3DTSS_TEXTURETRANSFORMFLAGS, Flag));

		i3Vector::Copy( &m_TextureCoordScale, pScale);

		i3Matrix::SetScale( &tmp, i3Vector::GetX( pScale), i3Vector::GetY( pScale), i3Vector::GetZ( pScale));

		if( CoordCount == 2)
			i3Matrix::SetAt( &tmp, &m_TextureCoordOffset);
		else
			i3Matrix::SetPos( &tmp, &m_TextureCoordOffset);

		Rv = m_pD3DDevice->SetTransform( Index, (const D3DMATRIX *) &tmp);
		#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetMatrix() failed.", Rv);
		}
		#endif
	}
}

void i3RenderContext::_SetTextureCoordOffset( INT32 StageIndex, VEC3D * pOffset, INT32 CoordCount)
{
	HRESULT Rv;
	D3DTRANSFORMSTATETYPE Index;
	DWORD Flag;

	I3ASSERT((StageIndex >= 0) && (StageIndex < 8));

	Index = (D3DTRANSFORMSTATETYPE) s_TextureMatrixTable[ StageIndex];

	if( CoordCount <= 0)
	{
		// Disable Texture Matrix
		CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIndex, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE));
	}
	else
	{
		MATRIX tmp;

		Flag = s_TextureFlagTable[ CoordCount - 1];
		
		CHECKRESULT( m_pD3DDevice->SetTextureStageState( StageIndex, D3DTSS_TEXTURETRANSFORMFLAGS, Flag));

		i3Vector::Copy( &m_TextureCoordOffset, pOffset);

		i3Matrix::SetScale( &tmp,	i3Vector::GetX( &m_TextureCoordScale), 
									i3Vector::GetY( &m_TextureCoordScale), 
									i3Vector::GetZ( &m_TextureCoordScale));

		if( CoordCount == 2)
			i3Matrix::SetAt( &tmp, &m_TextureCoordOffset);
		else
			i3Matrix::SetPos( &tmp, &m_TextureCoordOffset);

		Rv = m_pD3DDevice->SetTransform( Index, (const D3DMATRIX *) &tmp);
		#if defined( I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3RenderContext::SetMatrix() failed.", Rv);
		}
		#endif
	}
}

void i3RenderContext::_SetMaxAnisotropy( INT32 idxStage, INT32 val)
{
	CHECKRESULT( m_pD3DDevice->SetSamplerState( idxStage, D3DSAMP_MAXANISOTROPY, val));
}

void i3RenderContext::addAffectLight( i3Light * pLight)
{
	I3ASSERT( pLight != nullptr);

	if( g_GfxOption.getForceShadowMapEnable() == false)
	{
		// PixelShader Instruction Slot  치 苛鳴 풔,
		// Shadow .
		pLight->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);
	}

	m_AffectLightList.insert( pLight);
}

void i3RenderContext::SetFresnelEnable( bool bFlag)
{
	bFlag ? addRenderFlag(I3RENDER_FLAG_FRESNEL_ENABLE) : removeRenderFlag( I3RENDER_FLAG_FRESNEL_ENABLE);

	m_ShaderCode.setFresnelEnable( bFlag);

	if( m_pCurShader != nullptr)
		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_FRESNEL);
}

void i3RenderContext::SetFresnelIndexOfReflect( REAL32 val)
{
	m_FresnelIOR = val;

	if( m_pCurShader != nullptr)
		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_FRESNEL);
}

void i3RenderContext::SetFresnelColor( COLORREAL * pColor)
{
	i3Color::Set( &m_FresnelColor, pColor);

	if( m_pCurShader != nullptr)
		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_FRESNEL);
}

void i3RenderContext::resetAffectLight(void)
{
	m_AffectLightList.clear();
}


void i3RenderContext::SetForceShadeMode( I3G_SHADE_MODE ShadeMode)
{
	m_ForceShadeMode = ShadeMode;
}

void i3RenderContext::SetShader( i3Shader * pShader)
{
	i3Shader * pOldShader = m_pCurShader;

	I3_REF_CHANGE( m_pCurShader, pShader);

	if( m_pCurShader == nullptr)
	{
		m_OldShaderCode.setCode( 0xFFFFFFFF00000000L);

		SelectShader( nullptr);
	}
	else
	{
		if( pOldShader != pShader)
		{
			m_OldShaderCode.setCode( 0xFFFFFFFF00000000L);

			pShader->Validate( this);

			i3ShaderContext * pCtx = pShader->getContext();

			pCtx->Apply( this);
			NotifyTextureBindChange();

			// Lights
			{
				pCtx->resetLight();
			}
		}
	}
}

void i3RenderContext::SelectShader( i3Shader * pReqShader, INT32 pass)
{
	if( !(m_pCaps->getEnableShader()) )
	{
		return;
	}

	// Shader 玖庸?퓐  Shader Code
	//   Render State   濫풩풔  歐 
	//  Render State 駙  Code 磯.
	//UINT64 oldCode = m_ShaderCode.getCode();
	i3ShaderCode code;

	code.setCode( m_ShaderCode.getCode());

	code.setFresnelEnable( m_ShaderCode.isPPLShader() && m_ShaderCode.getFresnelEnable());
	code.setInstancingEnable( getInstancingEnable());

	// 0.
	if( m_pCurrentVA != nullptr)
	{
		bool bTexCoord = (m_pCurrentVA->GetFormat()->GetTextureCoordSetCount() > 0);
		bool bIsTex;

		bIsTex = GetTexture(I3G_TEXTURE_BIND_DIFFUSE) != nullptr;
		code.setDiffuseMapEnable( m_ShaderCode.getDiffuseMapEnable() && bTexCoord && bIsTex);

		code.setVertexColorEnable( m_pCurrentVA->GetFormat()->GetHasColor() && IsRenderFlag( I3RENDER_FLAG_VERTEXCOLOR_ENABLE) && !IsRenderFlag(I3RENDER_FLAG_LIGHTING_ENABLE));

		bIsTex = GetTexture(I3G_TEXTURE_BIND_NORMAL) != nullptr;
		code.setNormalMapEnable( m_ShaderCode.isPPLShader() && m_ShaderCode.getNormalMapEnable() && bTexCoord && bIsTex);

		bIsTex = GetTexture(I3G_TEXTURE_BIND_SPECULAR) != nullptr;
		code.setSpecularMapEnable( m_ShaderCode.isPPLShader() && m_ShaderCode.getSpecularMapEnable() && bTexCoord && bIsTex);

		bIsTex = GetTexture(I3G_TEXTURE_BIND_EMISSIVE) != nullptr;
		code.setEmissiveMapEnable( /*m_ShaderCode.isPPLShader() &&*/ m_ShaderCode.getEmissiveMapEnable() && bTexCoord && bIsTex);

		bIsTex = GetTexture(I3G_TEXTURE_BIND_REFLECT_MASK) != nullptr;
		code.setReflectMaskMapEnable( m_ShaderCode.getReflectMaskMapEnable() && bTexCoord && bIsTex);

		bool bNormal = (m_pCurrentVA->GetFormat()->GetHasNormal());
		bIsTex = GetTexture(I3G_TEXTURE_BIND_REFLECT) != nullptr;
		code.setReflectMapEnable( m_ShaderCode.getReflectMapEnable() & bNormal && bIsTex);
		
		bTexCoord = m_pCurrentVA->GetFormat()->GetTextureCoordSetCount() > 1;
		bIsTex = GetTexture(I3G_TEXTURE_BIND_LUX) != nullptr;
		code.setLuxMapEnable( m_ShaderCode.getLuxMapEnable() && bTexCoord && bIsTex);

		if( code.getLuxMapEnable())
			code.setLightingEnable( true);
	}
	// 0.418210

	COLOR Color;
	i3Color::Set( &Color, &m_XrayColor);

	UINT32 colValue = i3Color::ConvertRGBA32(&Color);
	if(  colValue > 0 )
	{
		code.setXrayColorEnable(true);
	}
	else
	{
		code.setXrayColorEnable(false);
	}

	i3Light * pLight1 = nullptr, * pLight2 = nullptr;

	if( code.getLightingEnable())
	{
		// 0.306464
		INT32 idx = (pass << 1);

		switch( m_ShadeMode)
		{
			default :					code.setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_PHONG);	break;
			case I3G_SHADE_CUSTOM :		code.setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_CUSTOM);	break;

			case I3G_SHADE_HEMISPHERE :	code.setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_HSL);	break;
			case I3G_SHADE_GI1 :		code.setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_GI1);	break;
		}

		if( getAffectLightCount() > idx)
		{
			pLight1 = getAffectLight( idx);
		}

		if( getAffectLightCount() > (idx + 1))
		{
			pLight2 = getAffectLight( idx + 1);

		}

		if((pLight1 != nullptr) && (pLight2 != nullptr))
		{
			switch( pLight1->GetLightType())
			{
				// case I3G_LIGHT_DIRECTIONAL :		//   侮 却.
				case I3G_LIGHT_POINT :
					if( pLight2->GetLightType() == I3G_LIGHT_DIRECTIONAL)
					{
						i3Light * pTemp = pLight2;
						pLight2 = pLight1;		pLight1 = pTemp;
					}
					break;

				case I3G_LIGHT_SPOT :
					{
						//  체
						i3Light * pTemp = pLight2;
						pLight2 = pLight1;		pLight1 = pTemp;
					}
					break;
			}
		}

		code.setLights( pLight1, pLight2);
		// 0.306464
	}
	else
	{
		//m_ShaderCode.setPPLShader( false);

		code.setLights( nullptr, nullptr);
		code.setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_NOLIGHT);
	}
	
	//  Shader Mask  체크爛求.
	if((m_pCurShader != nullptr) && (m_pCurShader->isCustomShader()))
	{
		// Lights
		{
			i3ShaderContext * pCtx = m_pCurShader->getContext();

			pCtx->resetLight();
			pCtx->setLight( pLight1, pLight2);
		}
	}
	else
	{
		if( code.getCode() != m_OldShaderCode.getCode())
		{
			m_OldShaderCode.setCode( code.getCode());

			i3Shader * pShader;
			
			if( (pReqShader != nullptr) && (pReqShader->getCode() == code.getCode()))
				pShader = pReqShader;
			else
				pShader = m_pShaderCache->Instanciate(  &code);

			if( pShader != m_pCurShader)
			{
				pShader->Validate( this);

				i3ShaderContext * pCtx = pShader->getContext();

				I3_REF_CHANGE( m_pCurShader, pShader);

				pCtx->Apply( this);

				NotifyTextureBindChange();
			}

			// Lights
			{
				i3ShaderContext * pCtx = pShader->getContext();

				pCtx->resetLight();
				pCtx->setLight( pLight1, pLight2);
			}
		}
	}

	//m_ShaderCode.setCode( oldCode);

	_syncShader( this, m_pCurShader, GetVertexArray());
}

bool	i3RenderContext::ValidateShader( i3ShaderCode * pCode)
{
	i3Shader * pShader;

	pShader = m_pShaderCache->Instanciate( pCode);

	if( pShader->getCode() != pCode->getCode())
	{
		pCode->setCode( pShader->getCode());

		return false;
	}

	return true;
}

INT32	i3RenderContext::getDisplayModeCount( I3G_IMAGE_FORMAT fmt)
{
	D3DFORMAT nativeFmt;
	bool bExact;

	nativeFmt = i3DXUT_GetNativeImageFormat( fmt, &bExact);

	return g_pD3D->GetAdapterModeCount( g_iCurrentAdapterIndex, nativeFmt);
}

void	i3RenderContext::getDisplayMode( INT32 idx, I3G_IMAGE_FORMAT fmt, I3G_DISPLAY_MODE_INFO * pInfo)
{
	HRESULT hRv;
	D3DFORMAT nativeFmt;
	bool bExact;
	D3DDISPLAYMODE mode;

	nativeFmt = i3DXUT_GetNativeImageFormat( fmt, &bExact);

	hRv = g_pD3D->EnumAdapterModes( g_iCurrentAdapterIndex, nativeFmt, (UINT) idx, &mode);
	if( FAILED( hRv))
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not display mode information.");
	}

	pInfo->m_Width = mode.Width;
	pInfo->m_Height = mode.Height;
	pInfo->m_Format = i3DXUT_GetI3ImageFormat( nativeFmt, &bExact);
	pInfo->m_RefreshRate = (REAL32) mode.RefreshRate;
}

//
///////////////////////////////////////////////////////////

void i3RenderContext::OnInit(void)
{
	INT32 i;

	m_TexOpMask = 0;
	m_OldTexOpMask = 0;
	m_TraceOpMask = 0;
	addRenderFlag( I3RENDER_FLAG_TEXTURE_MODIFIED);
	removeRenderFlag( I3RENDER_FLAG_LIGHTING_ENABLE);

	for( i = 0; i < I3G_TEXTURE_BIND_MAX; i++)
	{
		m_TexBindInfo[i].m_bEnable = false;
		m_TexBindInfo[i].m_ModifyFlag = 0xFFFFFFFF;
		m_TexBindInfo[i].m_pTexture = nullptr;
		m_TexBindInfo[i].m_MinFilter = I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT;
		m_TexBindInfo[i].m_MagFilter = I3G_TEXTURE_FILTER_LINEAR;
		m_TexBindInfo[i].m_HWrap = I3G_TEXTURE_WRAP_WRAP;
		m_TexBindInfo[i].m_VWrap = I3G_TEXTURE_WRAP_WRAP;
		i3Matrix::Identity( &m_TexBindInfo[i].m_Matrix);
		i3Vector::Set( &m_TexBindInfo[i].m_Scale, 0.0f, 0.0f, 0.0f);
		i3Vector::Set( &m_TexBindInfo[i].m_Offset, 0.0f, 0.0f, 0.0f);

		m_TexBindInfo[i].m_bEnableChangedByChangeTex = false;
	}
}

void i3RenderContext::Dump(void)
{
	DWORD val;

	CHECKRESULT( m_pD3DDevice->GetTextureStageState( 0, D3DTSS_COLOROP,  &val));
	I3TRACE( "COLOROP : %d\n", val);

	CHECKRESULT( m_pD3DDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &val));
	I3TRACE( "COLORARG1 : %d\n", val);

	CHECKRESULT( m_pD3DDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &val));
	I3TRACE( "COLORARG2 : %d\n", val);

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_TEXTUREFACTOR, &val));
	I3TRACE( "TFACTOR : %08X\n", val);


}

HMONITOR	i3RenderContext::FindCurrentMonitorHandle() const
{
	static bool s_bInited = false;
	static LPMONITORFROMWINDOW s_pFnGetMonitorFronWindow = nullptr;
	HMONITOR	hMonitor;

	if( !s_bInited)
	{
		s_bInited = true;
		HMODULE hUser32 = GetModuleHandle( "USER32" );
		if( hUser32 )
			s_pFnGetMonitorFronWindow = (LPMONITORFROMWINDOW) GetProcAddress(hUser32,"MonitorFromWindow");
	}

	if( s_pFnGetMonitorFronWindow ) 
		hMonitor = s_pFnGetMonitorFronWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);  // MONITOR_DEFAULTTOPRIMARY);
	else
		hMonitor = nullptr;

	return hMonitor;
}

// 어댑터 인덱스는 HMONITOR가 달라지더라도, 바뀌지 않을수 있다... // 반대로, HMONITOR가 같더라도 어댑터인덱스가 바뀔수 있을지도 모른다고 가정은 해야함..
INT32	i3RenderContext::FindAdapterIndex( HMONITOR hMonitor) const
{
	const INT32 numAdapter = INT32(m_pD3D->GetAdapterCount());

	for(INT32 i = 0; i < numAdapter ; ++i )
	{
		if( hMonitor == m_pD3D->GetAdapterMonitor( i))
		{
			return i;
		}
	}

	return -1;
}

bool	i3RenderContext::FindCurrentMonitorHandleAndAdapterIndex(HMONITOR& outCurrHMon, INT32& outAdapterIndex)
{
	outCurrHMon = FindCurrentMonitorHandle();
	outAdapterIndex = FindAdapterIndex(outCurrHMon);
	return outCurrHMon != 0 && outAdapterIndex > -1;
}


/*
INT32 i3RenderContext::_GetMonitorFromWindow( HWND hWnd, HMONITOR* outHMonitor )
{
	HMONITOR hMonitor = FindCurrentMonitorHandle();

	const INT32 numAdapter = INT32(m_pD3D->GetAdapterCount());

	for(INT32 i = 0; i < numAdapter ; ++i )
	{
		if( hMonitor == m_pD3D->GetAdapterMonitor( i))
		{
			if (outHMonitor)
				*outHMonitor = hMonitor;
			return i;
		}
	}
	
	if (outHMonitor)
		*outHMonitor = 0;
    return -1;
}
*/

void i3RenderContext::UpdateDevice(void)
{
	HMONITOR hCurMonitor = FindCurrentMonitorHandle();
	INT32 newMon = FindAdapterIndex(hCurMonitor);
   // INT32 newMon = _GetMonitorFromWindow( m_hWnd, &hCurMonitor);
    
    if( newMon != m_CurVideoInfo.m_AdapterIndex || hCurMonitor != m_CurHMonitor)
    {
		// Monitor 杉.
		m_CurVideoInfo.m_AdapterIndex = newMon;
		m_CurHMonitor = hCurMonitor;			// 성공시에만 갱신한다..(실패시에는 함수호출을 다시 할수 있는 여지를 주도록)
		g_iCurrentAdapterIndex = newMon;

		{
			D3DADAPTER_IDENTIFIER9 info;

			m_pD3D->GetAdapterIdentifier( newMon, 0, &info);

			I3TRACE( "DRIVER   : %s\n", info.Driver);
			I3TRACE( "DESC     : %s\n", info.Description);
			I3TRACE( "DEVICE   : %s\n", info.DeviceName);
			I3TRACE( "VENDOR   : %X\n", info.VendorId);
			I3TRACE( "ID       : %X\n", info.DeviceId);
		}

		if( FindAppropriateVideoInfo( &m_CurVideoInfo, &m_CurVideoInfo))
        {
			if( _CreateDevice())
			{
			
			}
			else
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not recreate Direct3D Device object.");
				return;
			}
        }
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not refresh monitor information.");
			return;
		}
    }    
}

void i3RenderContext::RefreshDevice(void)
{
	_CreateDevice();
	//Reset();
}

void i3RenderContext::SaveLastFrameTexture()
{
	if( m_pLastFrameTargetTexture == nullptr)	return;

	HRESULT hr;
	

#if 0
	IDirect3DSurface9* pDestBuffer = nullptr;
	hr = m_pSwapChain->GetFrontBufferData( pDestBuffer);
	if( FAILED( hr))
	{
		DXTRACE_ERR("m_pD3DDevice->GetFrontBufferData", hr);
	}
#else
	IDirect3DSurface9* pBackBuffer = nullptr; 
	IDirect3DSurface9* pDestBuffer = m_pLastFrameTargetTexture->getSurface(0)->getSurface(); 
	hr = m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	
	if( pBackBuffer != nullptr)
	{
		if( FAILED( hr = m_pD3DDevice->StretchRect( pBackBuffer, NULL, pDestBuffer, NULL, D3DTEXF_NONE)))
		{
			DXTRACE_ERR(L"m_pD3DDevice->StretchRect", hr);
		}

		pBackBuffer->Release(); 
	}
#endif
}

void i3RenderContext::GetBackBufferTexture( i3Texture *pTex)
{
	HRESULT hr;

	IDirect3DSurface9* pBackBuffer = nullptr; 
	IDirect3DSurface9* pDestBuffer = pTex->getSurface(0)->getSurface(); 
	hr = m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ); 
	hr = m_pD3DDevice->StretchRect( pBackBuffer, NULL, pDestBuffer, NULL, D3DTEXF_NONE);
	pBackBuffer->Release(); 
}

void i3RenderContext::CopyRenderTarget( i3Texture * pDest, i3Texture * pSrc)
{
	i3TextureDX * pDestTex = (i3TextureDX *) pDest;
	i3TextureDX * pSrcTex = (i3TextureDX *) pSrc;
	IDirect3DSurface9 * pSrcSurface = pSrcTex->getSurface(0)->getSurface();
	IDirect3DSurface9 * pDestSurface = pDestTex->getSurface(0)->getSurface();

	CHECKRESULT( m_pD3DDevice->GetRenderTargetData( pSrcSurface, pDestSurface));
}

void i3RenderContext::ColorFill( i3Texture * pDest, COLOR * pCol)
{
	IDirect3DSurface9 * pSurface = pDest->getSurface(0)->getSurface();
	RECT rt;
	D3DCOLOR c;

	rt.left = 0;
	rt.top = 0;
	rt.right = pDest->GetWidth();
	rt.bottom = pDest->GetHeight();

	c = i3Color::ConvertARGB32( pCol);

	CHECKRESULT( m_pD3DDevice->ColorFill( pSurface, &rt, c));
}

void i3RenderContext::SaveScreenShotToFile( char* pszPath, char* fileformat)
{	
	char szTemp[1024] = {0,};

	i3::generic_string_cat( szTemp, pszPath);

	IDirect3DSurface9* pBackBuffer = nullptr; 
	_D3DXIMAGE_FILEFORMAT SCfileformat = D3DXIFF_BMP;

	if( i3::generic_is_iequal( fileformat, "bmp") )
	{
		SCfileformat = D3DXIFF_BMP;
		i3::generic_string_cat( szTemp, ".bmp");
	}
	else if( i3::generic_is_iequal( fileformat, "jpg") )
	{
		SCfileformat = D3DXIFF_JPG;
		i3::generic_string_cat( szTemp, ".jpg");
	}
	else if( i3::generic_is_iequal( fileformat, "tga") )
	{
		SCfileformat = D3DXIFF_TGA;
		i3::generic_string_cat( szTemp, ".tga");
	}
	else if( i3::generic_is_iequal( fileformat, "png") )
	{
		SCfileformat = D3DXIFF_PNG;
		i3::generic_string_cat( szTemp, ".png");
	}
	
	HRESULT hr = m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ); 
	if( !FAILED( hr))
	{
		::D3DXSaveSurfaceToFile( szTemp, SCfileformat, pBackBuffer, NULL, NULL ); 
		pBackBuffer->Release(); 
	}
}

void i3RenderContext::SaveScreenShotToFileW( const wchar_t* pszPath, char* fileformat)
{	
	wchar_t szTemp[1024] = L"";

	i3::generic_string_cat( szTemp, pszPath);

	IDirect3DSurface9* pBackBuffer = nullptr; 
	_D3DXIMAGE_FILEFORMAT SCfileformat = D3DXIFF_BMP;

	if( i3::generic_is_iequal( fileformat, "bmp") )
	{
		SCfileformat = D3DXIFF_BMP;
		i3::generic_string_cat( szTemp, L".bmp");
	}
	else if( i3::generic_is_iequal( fileformat, "jpg") )
	{
		SCfileformat = D3DXIFF_JPG;
		i3::generic_string_cat( szTemp, L".jpg");
	}
	else if( i3::generic_is_iequal( fileformat, "tga") )
	{
		SCfileformat = D3DXIFF_TGA;
		i3::generic_string_cat( szTemp, L".tga");
	}
	else if( i3::generic_is_iequal( fileformat, "png") )
	{
		SCfileformat = D3DXIFF_PNG;
		i3::generic_string_cat( szTemp, L".png");
	}
	
	HRESULT hr = m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ); 
	if( !FAILED( hr))
	{
		::D3DXSaveSurfaceToFileW( szTemp, SCfileformat, pBackBuffer, NULL, NULL ); 
		pBackBuffer->Release(); 
	}
}

void i3RenderContext::DrawDisplayList( i3DisplayList * pDispList)
{
	IDirect3DStateBlock9 * pStateBlock = pDispList->getDXStateBlock();

	CHECKRESULT( pStateBlock->Apply());
}

void i3RenderContext::BeginRecordDisplayList( i3DisplayList * pDispList)
{
	CHECKRESULT( m_pD3DDevice->BeginStateBlock());
}

void i3RenderContext::EndRecordDisplayList(  i3DisplayList * pDispList)
{
	IDirect3DStateBlock9 * pStateBlock;

	CHECKRESULT( m_pD3DDevice->EndStateBlock( & pStateBlock));

	pDispList->setDXStateBlock( pStateBlock);
}

void i3RenderContext::AddWaitVSyncProc( WAITVSYNCPROC pUserProc, void * pUserData)
{
	WAIT_VSYNC_INFO * pInfo;

	pInfo = (WAIT_VSYNC_INFO *) i3MemAlloc( sizeof(WAIT_VSYNC_INFO));

	pInfo->m_pProc = pUserProc;
	pInfo->m_pUserData = pUserData;

	m_WaitProcList.push_back( pInfo);
}

struct HIDE_INFO
{
	i3::vector<HIDE_WND_INFO*> * m_pList = nullptr;
	HWND	m_hwndOwner = nullptr;
} ;

struct HIDE_WND_INFO
{
	HWND		m_hWnd = nullptr;
	UINT32		m_style = 0;
	RECT		m_Rect = { 0, 0, 0, 0 };
} ;

static BOOL CALLBACK _ListWindowProc( HWND hwnd, LPARAM lParam)
{
	HIDE_INFO * pInfo = (HIDE_INFO *) lParam;

	if( pInfo->m_hwndOwner == hwnd)
		return true;

	UINT32 style = ::GetWindowLong( hwnd, GWL_STYLE);
	
	if((style & (WS_VISIBLE | WS_MINIMIZEBOX)) == (WS_VISIBLE | WS_MINIMIZEBOX))
	{
		HIDE_WND_INFO * pWndInfo = new HIDE_WND_INFO;
		NetworkDump_Malloc( pWndInfo, sizeof( HIDE_WND_INFO), __FILE__, __LINE__);

		pWndInfo->m_hWnd = hwnd;
		pWndInfo->m_style = style;

		pInfo->m_pList->push_back( pWndInfo);

		//::GetWindowRect( hwnd, & pWndInfo->m_Rect);
		//::SetWindowLong( hwnd, GWL_STYLE, style | WS_MINIMIZE);
		::ShowWindow( hwnd, SW_HIDE);
	}

	return true;
}


// Fullscreen 화 환求 , CPU  薩 
//  화涌? 獵 摸 창  Minimize 킨.

static bool		s_bMinimized = false;

void i3RenderContext::MinimizeOtherWindows(void)
{
	HIDE_INFO info;

	if( s_bMinimized)
		return;

	info.m_hwndOwner = m_hWnd;
	info.m_pList = &m_HideWndList;

	::EnumWindows( _ListWindowProc, (LPARAM) &info);

	s_bMinimized = true;
}

void i3RenderContext::RestoreOtherWindows(void)
{
	
	HIDE_WND_INFO * pWndInfo;

	if( s_bMinimized == false)
		return;

	for(size_t i = 0; i < m_HideWndList.size(); i++)
	{
		pWndInfo = m_HideWndList[i];

		/*
		::SetWindowPos( pWndInfo->m_hWnd, NULL, 
			pWndInfo->m_Rect.left, pWndInfo->m_Rect.top, 
			pWndInfo->m_Rect.right - pWndInfo->m_Rect.left, pWndInfo->m_Rect.bottom - pWndInfo->m_Rect.top,
			SWP_NOACTIVATE | SWP_NOZORDER);
			*/

		//::SetWindowLong( pWndInfo->m_hWnd, GWL_STYLE, pWndInfo->m_style);
		::ShowWindow( pWndInfo->m_hWnd, SW_SHOW);

		I3_MUST_DELETE( pWndInfo);
	}

	m_HideWndList.clear();

	s_bMinimized = false;
}

void i3RenderContext::SetLostDeviceHandler( I3_GFX_LOSTDEVICE_PROC pProc)
{
	s_pGfxLostDeviceHandler = pProc;
}

void i3RenderContext::SetReviveHandler( I3_GFX_REVIVE_PROC pProc)
{
	s_pGfxReviveHandler = pProc;
}

void i3RenderContext::SetRecreateHandler( I3_GFX_RECREATE_PROC proc)
{
	s_pGfxRecreateHandler = proc;
}

void i3RenderContext::SetXrayColor(COLORREAL * pColor)
{
	i3Color::Set( &m_XrayColor, pColor);


	if( m_pCurShader != nullptr)
		m_pCurShader->getContext()->addModifiedFlag( I3G_SHADER_PMASK_XRAYCOLOR);
}

bool i3RenderContext::CheckSupportedFormat( I3G_IMAGE_FORMAT fmt, I3G_USAGE usage)
{
	HRESULT rv;
	D3DFORMAT adaptorFormat, nativeFmt;
	DWORD dxUsage;

	adaptorFormat = i3DXUT_GetNativeImageFormat( m_CurVideoInfo.m_RenderTargetFormat);
	nativeFmt = i3DXUT_GetNativeImageFormat( fmt);

	dxUsage = i3DXUT_GetNativeUsage( usage);
	
	rv = m_pD3D->CheckDeviceFormat( m_CurVideoInfo.m_AdapterIndex, D3DDEVTYPE_HAL, adaptorFormat, usage, D3DRTYPE_TEXTURE, nativeFmt);

	return SUCCEEDED( rv);
}
void i3RenderContext::SetBackBufferSize(INT32 nX, INT32 nY)
{
	m_d3dpp.BackBufferWidth = nX;
	m_d3dpp.BackBufferHeight = nY;
}
#endif I3G_DX


void i3RenderContext::OnRecreate( void)
{
	/*i3GfxResource * pRes = i3GfxResource::getHeadOfGfxResource();

	while( pRes != nullptr)
	{
		pRes->Recreate();

		pRes = pRes->getNextRes();
	}*/

	if( s_pGfxRecreateHandler != nullptr)
		s_pGfxRecreateHandler();
}

void i3RenderContext::SetMipmapBias(REAL32 fBias)
{
	I3G_TEXTURE_FILTER MinFilter = I3G_TEXTURE_FILTER_NONE;

	if (g_GfxOption.getForcedMipmapFilter() != I3G_TEXTURE_FILTER_NONE)
	{
		MinFilter = g_GfxOption.getForcedMipmapFilter();
	}

	if (MinFilter >= I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT)
	{
		for (INT32 i = 0; i < I3G_TEXTURE_BIND_MAX; i++)
		{
			m_pD3DDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&fBias)));
		}
	}
}