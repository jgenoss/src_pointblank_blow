#include "i3GfxType.h"
#include "i3GfxCaps.h"
#include "i3GfxGlobalVariable.h"

#include "i3Base/string/ext/contain_string.h"

#ifdef I3G_OGLES
#include "i3GfxOGLES.h"
#endif

#if defined( I3G_DX)
#include "i3GfxGlobalVariableDx.h"
#include "i3GfxUtilDX.h"

#include <ddraw.h>
#endif

//I3_CLASS_INSTANCE( i3GfxCaps, i3ElementBase);
 I3_CLASS_INSTANCE( i3GfxCaps);

i3GfxCaps::i3GfxCaps(void)
{
#if defined( I3G_DX)
	D3DCAPS9		m_Caps;
	D3DADAPTER_IDENTIFIER9 m_Info;
	
	HRESULT hrCaps = g_pD3DDevice->GetDeviceCaps(&m_Caps);

	I3ASSERT(hrCaps == D3D_OK);

	g_pD3D->GetAdapterIdentifier(g_iCurrentAdapterIndex, 0, &m_Info);

	{
		HDC dc = ::GetDC( NULL);

		m_DefaultRefreshRate = ::GetDeviceCaps( dc, VREFRESH);

		m_DPI					= ::GetDeviceCaps( dc, LOGPIXELSY);

		::ReleaseDC( NULL, dc);
	}

	// GPU Name
	i3::safe_string_copy(m_ApapterInfo.m_szGPUName, m_Info.Description, 256);

	// Vendor ID
	switch(m_Info.VendorId)
	{
	case _NVIDIA_APAPTER:
		m_ApapterInfo.m_AdapterVendor = I3G_GPU_VENDOR_NVIDIA;
		break;

	case _ATI_APAPTER:
		m_ApapterInfo.m_AdapterVendor = I3G_GPU_VENDOR_ATI;
		break;
	default:
		m_ApapterInfo.m_AdapterVendor = I3G_GPU_VENDOR_STANDARD;
		I3TRACE("Unknown Graphic Card ( %s )", m_ApapterInfo.m_szGPUName);
		break;
	}

	// Device ID(하드웨어 ID)
	m_ApapterInfo.m_AdapterID = m_Info.DeviceId;

	m_MaxBoneMatrixCount	= m_Caps.MaxVertexBlendMatrixIndex + 1;
	m_MaxBlendIndexCount	= m_Caps.MaxVertexBlendMatrices;

	m_MaxPointSize			= m_Caps.MaxPointSize;
	m_bPerVertexPointSize	= ((m_Caps.FVFCaps & D3DFVFCAPS_PSIZE) != 0);
	m_MaxTextureStage		= m_Caps.MaxTextureBlendStages;

	m_bEnableUBYTE4			= (m_Caps.DeclTypes & D3DDTCAPS_UBYTE4) != 0;

	if( m_Caps.TextureOpCaps & D3DTEXOPCAPS_ADD)
		m_TexFuncBits |= I3GFXCAPS_TEXFUNC_ADD;

	if( m_Caps.TextureOpCaps & D3DTEXOPCAPS_MODULATE)
		m_TexFuncBits |= I3GFXCAPS_TEXFUNC_MODULATE;

	if( m_Caps.TextureOpCaps & D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA )
		m_TexFuncBits |= I3GFXCAPS_TEXFUNC_SPECULAR_HILIT;

	m_bAlphaClutEnable = ( m_Caps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE) != 0;

	m_PS_NumInstructionSlots = m_Caps.PS20Caps.NumInstructionSlots;

	// Vertex Shader version
	{
		DWORD v = m_Caps.VertexShaderVersion;

		m_MaxVSVersionMajor = (v >> 8) & 0xFF;
		m_MaxVSVersionMinor = v & 0xFF;

		v = m_Caps.PixelShaderVersion;

		m_MaxPSVersionMajor = (v >> 8) & 0xFF;
		m_MaxPSVersionMinor = v & 0xFF;
	}
	
	const char* szVSProf = D3DXGetVertexShaderProfile( g_pD3DDevice);
	const char* szPSProf = D3DXGetPixelShaderProfile( g_pD3DDevice);
	
	if (szVSProf) 
		m_strVertexShaderProfile =  szVSProf;
	else
		m_strVertexShaderProfile.clear();

	if (szPSProf)
		m_strPixelShaderProfile = szPSProf;
	else
		m_strPixelShaderProfile.clear();
	
	if(szVSProf != nullptr && szPSProf != nullptr)
	{
		// 현재의 그래픽 카드가 Shader가 지원되는 안되는지에 대하여 판단한다.
		if(i3::contain_string(m_strVertexShaderProfile, "vs") != -1)
		{				
			if(i3::contain_string(m_strPixelShaderProfile, "ps") != -1)
			{
				m_bShaderEnable = true;
			}
			else
			{
				m_bShaderEnable = false;
				I3PRINTLOG(I3LOG_WARN, "This Graphic card not surport pixel shader");
			}
		}
		else
		{
			m_bShaderEnable = false;
			I3PRINTLOG(I3LOG_WARN, "This Graphic card not surport vertex shader");
		} 
	}

	m_MaxAnisotropy = m_Caps.MaxAnisotropy;

	// 현재의 그래픽 카드가 감마 컨트롤을 지원하는지 확인합니다.
	m_bGammaControlEnable = ( m_Caps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) != 0;
	m_MaxVertexShaderConst	=	m_Caps.MaxVertexShaderConst;

	m_bRegisterSwizzle = (m_Caps.PS20Caps.Caps & D3DPS20CAPS_ARBITRARYSWIZZLE) != 0;
	m_bTexInstructionLimit = (m_Caps.PS20Caps.Caps & D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT) == 0;

	m_VRAMSize = _GetVRAMSize();

	m_uiMaxVertexIndex = m_Caps.MaxVertexIndex;


#elif defined( I3G_PSP)
	m_MaxBoneMatrixCount = 8;
	m_MaxBlendIndexCount = 0;
	m_MaxPointSize = 64.0f;
	m_bPerVertexPointSize = true;
	m_bAlphaClutEnable = true;
#elif defined( I3G_OGLES)
	m_MaxBoneMatrixCount = 0;
	m_MaxBlendIndexCount = 0;
	m_MaxPointSize = 0.0f;
	m_bPerVertexPointSize = false;
	m_bAlphaClutEnable = true;

#if defined( I3G_OGLES)
	{
		GLint val[32], i;

		I3OGL::glGetIntegerv( GL_ALPHA_BITS, val);
		I3TRACE1( "CAPS - Alpha Bits : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_RED_BITS, val);
		I3TRACE1( "CAPS - Red Bits : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_GREEN_BITS, val);
		I3TRACE1( "CAPS - Green Bits : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_BLUE_BITS, val);
		I3TRACE1( "CAPS - Blue Bits : %d\n", val[0]);
 
		I3OGL::glGetIntegerv( GL_DEPTH_BITS, val);
		I3TRACE1( "CAPS - Depth Bits : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_MAX_ELEMENTS_INDICES, val);
		I3TRACE1( "CAPS - Max. Index Length : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_MAX_ELEMENTS_VERTICES, val);
		I3TRACE1( "CAPS - Max. Vertex Length : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_MAX_LIGHTS, val);
		I3TRACE1( "CAPS - Max. Light Count : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_MAX_MODELVIEW_STACK_DEPTH, val);
		I3TRACE1( "CAPS - Max. Model View Matrix Stack Depth : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_MAX_PROJECTION_STACK_DEPTH, val);
		I3TRACE1( "CAPS - Max. Projection Matrix Stack Depth : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_MAX_TEXTURE_STACK_DEPTH, val);
		I3TRACE1( "CAPS - Max. Texture Matrix Stack Depth : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_MAX_TEXTURE_SIZE, val);
		I3TRACE1( "CAPS - Max. Texture Size : %d\n", val[0]);

		I3OGL::glGetIntegerv( GL_NUM_COMPRESSED_TEXTURE_FORMATS, val);
		m_CompressedTexCount = val[0];

		I3OGL::glGetIntegerv( GL_COMPRESSED_TEXTURE_FORMATS, val);

		for( i = 0; i < m_CompressedTexCount; i++)
		{
			I3TRACE2( "CAPS - Supported Compressed Texture format[%d] : %x\n", i, val[i]);
		}
	}
#endif
	I3OGL::glGetIntegerv( GL_NUM_COMPRESSED_TEXTURE_FORMATS, &m_CompressedTexCount);
	I3OGL::glGetIntegerv( GL_COMPRESSED_TEXTURE_FORMATS, m_CompressedTexFormat);

#elif defined( I3G_XBOX)
#elif defined( I3G_PS2)
#endif


	{
		//PC type을 가져온다
		m_Wmi.WmiComExcute("SELECT * from Win32_SystemEnclosure", L"ChassisTypes");
		I3PRINTLOG(I3LOG_NOTICE, "---------------------------------");
		I3PRINTLOG(I3LOG_NOTICE, "PC Machine : %s", m_Wmi.GetMachineName());
		I3PRINTLOG(I3LOG_NOTICE, "---------------------------------");
	}

	m_GPUClass = CheckGPUClass(DetermineGPUClass());

	{
		static char s_ClassName[][2] =
		{
			"F",
			"E",
			"D",
			"C",
			"B",
			"A",
		};

		I3TRACE( "---------------------------------\n");
		I3TRACE( "GPU Class : %s\n", s_ClassName[ m_GPUClass]);
		I3TRACE( "---------------------------------\n");
	}
}

#if defined( I3G_DX)

typedef HMONITOR (WINAPI* LPMONITORFROMWINDOW)(HWND, DWORD);
typedef HRESULT ( WINAPI* LPDIRECTDRAWCREATE )( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );

struct DDRAW_MATCH
{
    GUID guid;
    HMONITOR hMonitor = nullptr;
	CHAR strDriverName[512] = { 0 };
    bool bFound = false;
};

static HMONITOR _GetMonitorFromWindow( HWND hWnd)
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
        hMonitor = s_pFnGetMonitorFronWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
	else
		hMonitor = nullptr;

	return hMonitor;
}

//-----------------------------------------------------------------------------
static BOOL WINAPI DDEnumCallbackEx( GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm )
 {
    UNREFERENCED_PARAMETER( lpDriverDescription );

    DDRAW_MATCH* pDDMatch = ( DDRAW_MATCH* ) lpContext;
    if( pDDMatch->hMonitor == hm )
    {
        pDDMatch->bFound = true;
        strcpy_s( pDDMatch->strDriverName, 512, lpDriverName );
        memcpy( &pDDMatch->guid, lpGUID, sizeof( GUID ) );
    }
    return true;
}
#endif

UINT32 i3GfxCaps::_GetVRAMSize( void)
{
#if defined( I3G_DX)
    LPDIRECTDRAW pDDraw = nullptr;
    LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx = nullptr;
    HRESULT hr;
    bool bGotMemory = false;
    UINT32 VRAM = 0;
	HMONITOR hMonitor;
    HINSTANCE hInstDDraw;
    LPDIRECTDRAWCREATE pDDCreate = nullptr;

	hMonitor = _GetMonitorFromWindow( g_pRenderContext->getHWND());

    hInstDDraw = LoadLibrary( "ddraw.dll" );
    if( hInstDDraw )
    {
        DDRAW_MATCH match;

        ZeroMemory( &match, sizeof( DDRAW_MATCH ) );
        match.hMonitor = hMonitor;

        pDirectDrawEnumerateEx = ( LPDIRECTDRAWENUMERATEEXA )GetProcAddress( hInstDDraw, "DirectDrawEnumerateExA" );

        if( pDirectDrawEnumerateEx )
        {
            hr = pDirectDrawEnumerateEx( DDEnumCallbackEx, ( VOID* )&match, DDENUM_ATTACHEDSECONDARYDEVICES );
			 if( FAILED( hr))
			 {
				 I3PRINTLOG(I3LOG_FATAL,  "Fail DDEnumerate %x!!", hr);
			 }
        }

        pDDCreate = ( LPDIRECTDRAWCREATE )GetProcAddress( hInstDDraw, "DirectDrawCreate" );
        if( pDDCreate )
        {
            pDDCreate( &match.guid, &pDDraw, NULL );

            LPDIRECTDRAW7 pDDraw7;

            if( SUCCEEDED( pDDraw->QueryInterface( IID_IDirectDraw7, ( VOID** )&pDDraw7 ) ) )
            {
                DDSCAPS2 ddscaps;

                ZeroMemory( &ddscaps, sizeof( DDSCAPS2 ) );
                ddscaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;

                hr = pDDraw7->GetAvailableVidMem( &ddscaps, (LPDWORD) &VRAM, NULL );
                if( SUCCEEDED( hr ) )
                    bGotMemory = true;
                pDDraw7->Release();
            }
			pDDraw->Release();
        }
        FreeLibrary( hInstDDraw );
    }

    return VRAM;
#endif
}

#if defined(I3G_DX)
void i3GfxCaps::SetEnableShader(bool bFlag)
{
	if(bFlag == true)
	{
		if( g_pD3DDevice == nullptr) return;

		const char* szVSProf = D3DXGetVertexShaderProfile( g_pD3DDevice);
		const char* szPSProf = D3DXGetPixelShaderProfile( g_pD3DDevice);
		
		if (szVSProf)
			m_strVertexShaderProfile = szVSProf;
		else
			m_strVertexShaderProfile.clear();

		if (szPSProf)
			m_strPixelShaderProfile = szPSProf;
		else
			m_strPixelShaderProfile.clear();

		if( szVSProf == nullptr ) return;

		// 현재의 그래픽 카드가 Shader가 지원되는 안되는지에 대하여 판단한다.
		if(i3::contain_string(m_strVertexShaderProfile, "vs") != -1)
		{				
			if(i3::contain_string(m_strPixelShaderProfile, "ps") != -1)
			{
				m_bShaderEnable = bFlag;
			}
			else
			{
				m_bShaderEnable = false;
				I3PRINTLOG(I3LOG_WARN, "This Graphic card not serport pixel shader");
			}
		}
		else
		{
			m_bShaderEnable = false;
			I3PRINTLOG(I3LOG_WARN, "This Graphic card not serport vertex shader");
		} 
	}
	else
	{
		m_bShaderEnable = bFlag;
	}
}

INT32	i3GfxCaps::getDisplayModeCount( I3G_IMAGE_FORMAT fmt)
{
	D3DFORMAT nativeFmt;
	bool bExact;

	nativeFmt = i3DXUT_GetNativeImageFormat( fmt, &bExact);

	return g_pD3D->GetAdapterModeCount( g_iCurrentAdapterIndex, nativeFmt);		// 첫 인수 D3DADAPTER_DEFAULT가 버그를 유발시킬수 있다.
}

void i3GfxCaps::getDisplayMode( INT32 idx, I3G_IMAGE_FORMAT fmt, I3G_DISPLAY_MODE_INFO * pInfo)
{
	HRESULT hRv;
	D3DFORMAT nativeFmt;
	bool bExact;
	D3DDISPLAYMODE mode;

	nativeFmt = i3DXUT_GetNativeImageFormat( fmt, &bExact);

	hRv = g_pD3D->EnumAdapterModes( g_iCurrentAdapterIndex, nativeFmt, (UINT) idx, &mode);		// 첫 인수 D3DADAPTER_DEFAULT가 버그를 유발시킬수 있다.
	if( FAILED( hRv))
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not display mode information.");
	}

	pInfo->m_Width = mode.Width;
	pInfo->m_Height = mode.Height;
	pInfo->m_Format = i3DXUT_GetI3ImageFormat( nativeFmt, &bExact);
	pInfo->m_RefreshRate = (REAL32) mode.RefreshRate;
}

void i3GfxCaps::buildAAList( I3G_IMAGE_FORMAT fmt, bool bWindowed, i3::vector<I3G_AA_INFO*>& List)
{
	INT32 i;
	HRESULT hRv;
	D3DFORMAT	nativeFmt;
	bool bExact;
	DWORD		qCount;
	I3G_AA_INFO * pInfo;

	nativeFmt = i3DXUT_GetNativeImageFormat( fmt, &bExact);

	for( i = 0; i < 17; i++)
	{
		hRv = g_pD3D->CheckDeviceMultiSampleType( g_iCurrentAdapterIndex, D3DDEVTYPE_HAL, nativeFmt, bWindowed,
						(D3DMULTISAMPLE_TYPE) i, &qCount);

		if( FAILED( hRv))
		{
		}
		else
		{
			pInfo = (I3G_AA_INFO *) i3MemAlloc( sizeof(I3G_AA_INFO));

			pInfo->m_Sampling = i;
			pInfo->m_QualityCount = 0;

			List.push_back( pInfo);

			if( qCount > 1)
			{
				pInfo = (I3G_AA_INFO *) i3MemAlloc( sizeof(I3G_AA_INFO));

				pInfo->m_Sampling = i;
				pInfo->m_QualityCount = qCount - 1;

				List.push_back( pInfo);
			}
		}
	}
}
#endif

#if defined(I3_WIPI) || defined(I3_WINCE)
bool i3GfxCaps::CanCompressTexFormat(INT32 format)
{	
	for(INT32 i= 0; i < m_CompressedTexCount; i++)
	{
		if( m_CompressedTexFormat[i] & format)
			return true;
	}
	return false;
}
#endif
