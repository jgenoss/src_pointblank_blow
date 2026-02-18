#include "i3GFXType.h"
#include <stdio.h>
#include <stdlib.h>
#include "i3RenderContext.h"

#ifdef I3G_XBOX
#include "i3GfxGlobalVariableXbox.h"
#include "i3GfxUtilXbox.h"

I3_CLASS_INSTANCE( i3RenderContext, i3ElementBase);

i3RenderContext::i3RenderContext(void)
{
	m_pSetupParam = NULL;
	m_pRenderTarget = NULL;

	m_pD3D = NULL;
	m_pD3DDevice = NULL;

	m_bWindowed = TRUE;
	m_bReady = FALSE;

	m_ClearFlag = I3G_CLEAR_COLOR;
	i3Color::Set( &m_BackColor, 0.0f, 0.0f, 0.0f, 1.0f);
	m_ClearZValue = 1.0f;
	m_ClearStencilValue = 0;

	m_pCurrentVA = NULL;
	m_pCurrentIA = NULL;

	memset( &m_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
}

i3RenderContext::~i3RenderContext(void)
{
	if( m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	if( m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

BOOL i3RenderContext::FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
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
                return TRUE;
            }
        }
    }

  //  if( DepthBits <= 15 && StencilBits <= 1 )
  //  {
		//Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1 );
  //      if( SUCCEEDED( Rv) )
  //      {
		//	Rv = m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType, Format, Format, D3DFMT_D15S1 );
  //          if( SUCCEEDED( Rv ) )
  //          {
  //              *pDepthStencilFormat = D3DFMT_D15S1;
  //              return TRUE;
  //          }
  //      }
  //  }

  //  if( DepthBits <= 24 && StencilBits == 0 )
  //  {
		//Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 );
  //      if( SUCCEEDED( Rv ) )
  //      {
		//	Rv = m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType, Format, Format, D3DFMT_D24X8 );
  //          if( SUCCEEDED( Rv) )
  //          {
  //              *pDepthStencilFormat = D3DFMT_D24X8;
  //              return TRUE;
  //          }
  //      }
  //  }

    if( DepthBits <= 24 && StencilBits <= 8 )
    {
		Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 );
        if( SUCCEEDED( Rv) )
        {
			m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType, Format, Format, D3DFMT_D24S8 );
            if( SUCCEEDED( Rv) )
            {
                *pDepthStencilFormat = D3DFMT_D24S8;
                return TRUE;
            }
        }
    }

  //  if( DepthBits <= 24 && StencilBits <= 4 )
  //  {
		//Rv = m_pD3D->CheckDeviceFormat( iAdapter, DeviceType, Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4 );
  //      if( SUCCEEDED( Rv) )
  //      {
  //          if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
  //              Format, Format, D3DFMT_D24X4S4 ) ) )
  //          {
  //              *pDepthStencilFormat = D3DFMT_D24X4S4;
  //              return TRUE;
  //          }
  //      }
  //  }

    //if( DepthBits <= 32 && StencilBits == 0 )
    //{
    //    if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
    //        Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32 ) ) )
    //    {
    //        if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
    //            Format, Format, D3DFMT_D32 ) ) )
    //        {
    //            *pDepthStencilFormat = D3DFMT_D32;
    //            return TRUE;
    //        }
    //    }
    //}

    return FALSE;
}

void i3RenderContext::FindAppropriateVideoInfo( i3VideoInfo * pWantInfo, i3VideoInfo * pFindInfo)
{
	UINT32 i, j;
	D3DDISPLAYMODE Mode;
	D3DFORMAT		Format;

	ZeroMemory(pFindInfo, sizeof(i3VideoInfo));

	Format = i3_GetNativeImageFormat( pWantInfo->m_RenderTargetFormat);

	for( i = 0; i < m_pD3D->GetAdapterCount(); i++)
	{
		for( j = 0; j < m_pD3D->GetAdapterModeCount( i); j++)
		{
			m_pD3D->EnumAdapterModes( i, j, &Mode);

			if( (pWantInfo->m_Width == Mode.Width) &&
				(pWantInfo->m_Height == Mode.Height) &&
				(pWantInfo->m_Flags == Mode.Flags) &&
				(Format == Mode.Format)	)
			{
				i3mem::Copy( pFindInfo, pWantInfo, sizeof(i3VideoInfo));

				pFindInfo->m_AdapterIndex = i;
				pFindInfo->m_ModeIndex = j;
				pFindInfo->m_Width = Mode.Width;
				pFindInfo->m_Height = Mode.Height;
				pFindInfo->m_RenderTargetFormat = i3_GetI3ImageFormat( Mode.Format);
			}
		}
	}
}


bool i3RenderContext::Create( i3VideoInfo * pVFormat, void * pSetupParam)
{
	HRESULT					Rv;
	BOOL					bDepthBuffer;
	D3DFORMAT				DepthFormat = (D3DFORMAT) 0;

	m_pSetupParam = pSetupParam;

	m_pD3D = Direct3DCreate8( D3D_SDK_VERSION);
	if( m_pD3D == NULL)
	{
		DebugBox( NULL, "Could not create a Direct3D object.");
		return false;
	}

	// if( pVFormat->m_bFullScreen)
	// {
		FindAppropriateVideoInfo( pVFormat, &m_CurrentVideoInfo);
	// }
	// else
	// {
	//	// GetCurrentVideoInfo( &m_CurrentVideoInfo);
	// }

	if( pVFormat->m_ZBufferBits > 0)
	{
		bDepthBuffer = TRUE;

		if( pVFormat->m_StencilBufferBits != 0)
		{
			switch( pVFormat->m_ZBufferBits)
			{
				case 16 :
					// Xbox에선 지원하지 않는다.
					I3ASSERT(0);
					// DirectX 8.1에서 Depth:16 bits, Stencil:1 이외의 Format은 지원하지 않는다.
					// DepthFormat = D3DFMT_D15S1;
					break;

				case 24 :
					// Xbox에선 24,8의 조합만 지원.
					I3ASSERT( pVFormat->m_StencilBufferBits == 8);
					DepthFormat = D3DFMT_D24S8;
					// D3DFMT_F24S8;
					// D3DFMT_LIN_D24S8;
					// D3DFMT_LIN_F24S8;
					break;

				case 32 :
					// Xbox에선 지원하지 않는다.
					I3ASSERT(0);
					// DepthFormat = D3DFMT_D32;
					break;
			}
		}
		else
		{
			switch( pVFormat->m_ZBufferBits)
			{
				// Xbox에선 16비트 Depth buffer만 지원한다.
				case 16 :	DepthFormat = D3DFMT_D16;	break;
				// case 24 :	DepthFormat = D3DFMT_D24X8;	break;
				// case 32 :	DepthFormat = D3DFMT_D32;	break;
				default: I3ASSERT(0); break;
			}
		}
	}

	m_d3dpp.BackBufferWidth = m_CurrentVideoInfo.m_Width;
	m_d3dpp.BackBufferHeight = m_CurrentVideoInfo.m_Height;
	m_d3dpp.BackBufferFormat = i3_GetNativeImageFormat( m_CurrentVideoInfo.m_RenderTargetFormat);
	m_d3dpp.BackBufferCount = pVFormat->m_FrameBufferCount - 1;
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	m_d3dpp.hDeviceWindow = NULL;
	m_d3dpp.Windowed = ! pVFormat->m_bFullScreen;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = DepthFormat;
	m_d3dpp.Flags = 0;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_RATE_DEFAULT;

	Rv = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&m_d3dpp,
			&m_pD3DDevice);
	if( FAILED( Rv))
	{
		DebugBox( NULL, "Could not create DirectX device.", Rv);
		DebugBox( NULL, "Could not create DirectX device.");
		return false;
	}

	g_pD3D = m_pD3D;
	g_pD3DDevice = m_pD3DDevice;

	// Render Target의 정보를 업데이트한다.
	{
		IDirect3DSurface8 * pSurface;
		i3RenderTarget * pNewRT;
		INT32 i;

		for( i = 0; i < (INT32) m_d3dpp.BackBufferCount; i++)
		{
			Rv = m_pD3DDevice->GetBackBuffer( i, D3DBACKBUFFER_TYPE_MONO, &pSurface);

			if( FAILED( Rv))
			{
				DebugBox( NULL, "GetBackBuffer() failed.", Rv);
			}

			pSurface->Release();

			pNewRT = i3RenderTarget::NewObject();

			pNewRT->Create( (void *) pSurface);

			m_RenderTargetList.Add( pNewRT);
		}
	}

	// TODO: Porting
	// UpdateVideoInfo();

    // The app is ready to go
    m_bReady = TRUE;

	return true;
}

void i3RenderContext::UpdateVideoInfo(void)
{
	// 실제로 얻어진 Device의 속성을 파악한다.
	D3DFORMAT DepthFormat, ColorFormat;
	BOOL Rv;

	{
		i3RenderTarget * pRT = GetRenderTarget( 0);

		m_CurrentVideoInfo.m_Width = pRT->GetWidth();
		m_CurrentVideoInfo.m_Height = pRT->GetHeight();
		m_CurrentVideoInfo.m_RenderTargetFormat = pRT->GetFormat();
	}
	
	ColorFormat = i3_GetNativeImageFormat( m_CurrentVideoInfo.m_RenderTargetFormat);

	Rv = FindDepthStencilFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			ColorFormat, m_CurrentVideoInfo.m_ZBufferBits, m_CurrentVideoInfo.m_StencilBufferBits, &DepthFormat);
	if( Rv == FALSE)
	{
		DebugBox( NULL, "i3RenderContext::UpdateVideoInfo()", Rv);
		return;
	}

	UNSETBIT( m_ClearFlag, I3G_CLEAR_STENCIL | I3G_CLEAR_DEPTH);

	switch( DepthFormat)
	{
		case D3DFMT_D16_LOCKABLE :
		// case D3DFMT_D16 :
			m_CurrentVideoInfo.m_ZBufferBits = 16;
			m_CurrentVideoInfo.m_StencilBufferBits = 0;
			break;

		//case D3DFMT_D15S1 :
		//	m_CurrentVideoInfo.m_ZBufferBits = 15;
		//	m_CurrentVideoInfo.m_StencilBufferBits = 1;
		//	break;

		case D3DFMT_D24S8 :
			m_CurrentVideoInfo.m_ZBufferBits = 24;
			m_CurrentVideoInfo.m_StencilBufferBits = 8;
			break;

		//case D3DFMT_D24X8 :
		//	m_CurrentVideoInfo.m_ZBufferBits = 24;
		//	m_CurrentVideoInfo.m_StencilBufferBits = 0;
		//	break;

		//case D3DFMT_D24X4S4 :
		//	m_CurrentVideoInfo.m_ZBufferBits = 24;
		//	m_CurrentVideoInfo.m_StencilBufferBits = 4;
		//	break;

		//case D3DFMT_D32 :
		//	m_CurrentVideoInfo.m_ZBufferBits = 32;
		//	m_CurrentVideoInfo.m_StencilBufferBits = 0;
		//	break;
	}

	if( m_CurrentVideoInfo.m_ZBufferBits > 0)
	{
		SETBIT( m_ClearFlag, I3G_CLEAR_DEPTH);
	}

	if( m_CurrentVideoInfo.m_StencilBufferBits > 0)
	{
		SETBIT( m_ClearFlag, I3G_CLEAR_STENCIL);
	}

	i3Log( "", "-----------------------------------------\n");
	i3Log( "", " * Width : %d", m_CurrentVideoInfo.m_Width);
	i3Log( "", " * Height : %d", m_CurrentVideoInfo.m_Height);
	i3Log( "", " * Depth Buffer Bits : %d", m_CurrentVideoInfo.m_ZBufferBits);
	i3Log( "", " * Stencil Buffer Bits : %d", m_CurrentVideoInfo.m_StencilBufferBits);
	i3Log( "", " * Full-Screen : %d", m_CurrentVideoInfo.m_bFullScreen);
	i3Log( "", " * Interface Mode : %d", m_CurrentVideoInfo.m_bInterlaced);
	i3Log( "", " * Refresh Rate : %f", m_CurrentVideoInfo.m_RefreshRate);
	i3Log( "", " * Frame Buffer Count : %d", m_CurrentVideoInfo.m_FrameBufferCount);
}

HRESULT i3RenderContext::ResetContext(void)
{
    HRESULT Rv;
	D3DPRESENT_PARAMETERS d3dpp;

    // Release all vidmem objects
	// ResetAllResources();

    // Reset the device
	Rv = m_pD3DDevice->Reset( &d3dpp);
    if( FAILED( Rv))
        return Rv;

	m_CurrentVideoInfo = d3dpp;

    // Store render target surface desc
    LPDIRECT3DSURFACE8 pBackBuffer;
	D3DSURFACE_DESC d3dsd;

    m_pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &d3dsd );
    pBackBuffer->Release();

    // Initialize the app's device-dependent objects
    //hr = RestoreDeviceObjects();
    //if( FAILED(hr) )
    //    return hr;

    return S_OK;
}

bool	i3RenderContext::Begin(void)
{
	/*
	HRESULT Rv;

    // Test the cooperative level to see if it's okay to render
	Rv = m_pD3DDevice->TestCooperativeLevel();
    if( FAILED( Rv))
    {
        // If the device was lost, do not render until we get it back
        if( Rv == D3DERR_DEVICELOST)
            return false;

        // Check if the device needs to be resized.
        if( Rv == D3DERR_DEVICENOTRESET)
        {
            // If we are windowed, read the desktop mode and use the same format for
            // the back buffer
            if( m_bWindowed)
            {
				GetCurrentVideoInfo( &m_CurrentVideoInfo);
            }

            if( FAILED( Rv = ResetContext() ) )
                return false;
        }

        return false;
    }
	*/

	return true;
}

void i3RenderContext::End(void)
{
    // Show the frame on the primary surface.
	m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
}

bool i3RenderContext::BeginRender(void)
{
	return !FAILED( m_pD3DDevice->BeginScene());
}

void i3RenderContext::EndRender(void)
{
    // End the scene.
    m_pD3DDevice->EndScene();   
}

void i3RenderContext::Clear(void)
{
	HRESULT Rv;

    // Clear the viewport
    Rv = m_pD3DDevice->Clear( 0, NULL, 
				m_ClearFlag, 
				m_BackColor, 
				m_ClearZValue,							// Z Clear Value
				m_ClearStencilValue						// Stencil Clear Value
				);
	if( FAILED( Rv))
	{
		DebugBox( NULL, "Clear buffer error", Rv);
	}
}

void i3RenderContext::SetVertexArray( i3VertexArray * pVA)
{
	HRESULT Rv;
	IDirect3DVertexBuffer8 * pVB = (IDirect3DVertexBuffer8 *) pVA->GetInstance();

	m_pD3DDevice->SetVertexShader( D3DFVF_DIFFUSE | D3DFVF_XYZ);

	Rv = m_pD3DDevice->SetStreamSource( 0, pVB, pVA->GetStride());
	if( FAILED( Rv))
	{
		DebugBox( NULL, "i3RenderContext::SetVertexArray()", Rv);
	}

}

void i3RenderContext::SetMatrix( I3G_MATRIX_STACK Index, MATRIX * pMatrix)
{
	HRESULT Rv;

	Rv = m_pD3DDevice->SetTransform( (D3DTRANSFORMSTATETYPE) Index, (const MATRIX *) pMatrix);
	if( FAILED( Rv))
	{
		DebugBox( NULL, "i3RenderContext::SetMatrix() failed.", Rv);
	}
}

void i3RenderContext::SetRenderState( I3G_RENDER_STATE state, UINT32 val)
{
	HRESULT Rv;

	Rv = g_pD3DDevice->SetRenderState( (D3DRENDERSTATETYPE) state, val);
	if( FAILED( Rv))
	{
		DebugBox( NULL, "i3RenderContext::SetRenderState() fauiled.", Rv);
	}
}

void i3RenderContext::SetViewport( INT32 x, INT32 y, INT32 Width, INT32 Height)
{
	HRESULT Rv;
	D3DVIEWPORT8	ViewPort;

	ViewPort.X = x;
	ViewPort.Y = y;
	ViewPort.Width = Width;
	ViewPort.Height = Height;
	ViewPort.MinZ = 0.0f;
	ViewPort.MaxZ = 1.0f;

	Rv = g_pD3DDevice->SetViewport( &ViewPort);
	if( FAILED( Rv))
	{
		DebugBox( NULL, "i3RenderContext::SetViewport() failed.", Rv);
	}
}

void i3RenderContext::DrawPrim( I3G_PRIMITIVE Prim, UINT32 StartIdx, UINT32 PrimCount)
{
	HRESULT Rv;

	Rv = m_pD3DDevice->DrawPrimitive( (D3DPRIMITIVETYPE) Prim, StartIdx, PrimCount);

	if( FAILED( Rv))
	{
		DebugBox( NULL, "i3RenderContext::DrawPrim()", Rv);
	}
}

void i3RenderContext::SetCurrentRenderTarget( i3RenderTarget * pRT)
{

}

BOOL i3RenderContext::SetRenderTargetSize( UINT32 cx, UINT32 cy)
{
	HRESULT Rv;

	m_d3dpp.BackBufferWidth = cx;
	m_d3dpp.BackBufferHeight = cy;

	Rv = m_pD3DDevice->Reset( &m_d3dpp);
	if( FAILED( Rv))
	{
		DebugBox( NULL, "i3RenderContext::SetRenderTargetSize() - failed.", Rv);
		return FALSE;
	}

	return TRUE;
}

#ifdef I3_DEBUG
#define CHECKRESULT( a)\
{\
	HRESULT Rv;\
	\
	Rv = (a);\
	if( FAILED( Rv))\
	{\
		char conv[256];\
		\
		sprintf( conv, "%s(%d)", __FILE__, __LINE__);\
		DebugBox( NULL, "RESULT = %d", Rv);\
	}\
}
#else
#define CHECKRESULT(a)			(a)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render State


void i3RenderContext::SetAlphaTestEnable( BOOL flag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, (DWORD) flag));
}

BOOL i3RenderContext::GetAlphaTestEnable(void)
{
	DWORD flag;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &flag));

	return (BOOL) flag;
}

void i3RenderContext::SetBlendEnable( BOOL flag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) flag));
}

BOOL i3RenderContext::GetBlendEnable(void)
{
	DWORD val;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &val));

	return (BOOL) val;
}

void i3RenderContext::SetAlphaRefValue( REAL32 val)
{
	DWORD sp = *((DWORD *) & val);

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF, sp));
}

REAL32 i3RenderContext::GetAlphaRefValue(void)
{
	REAL32 val;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ALPHAREF, (DWORD *) &val));

	return val;
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
void i3RenderContext::SetZTestEnable( BOOL flag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, (DWORD) flag));
}

BOOL i3RenderContext::GetZTestEnable(void)
{
	BOOL rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ZENABLE, (DWORD *) & rv));

	return rv;
}

void i3RenderContext::SetZWriteEnable( BOOL flag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, (DWORD) flag));
}

BOOL i3RenderContext::GetZWriteEnable(void)
{
	BOOL rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_ZWRITEENABLE, (DWORD *) &rv));

	return rv;
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
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SHADEMODE, (DWORD) mode));
}

I3G_SHADE_MODE i3RenderContext::GetShadeMode(void)
{
	I3G_SHADE_MODE mode;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_SHADEMODE, (DWORD *) & mode));

	return mode;
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
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, blend));
}

I3G_BLEND i3RenderContext::GetSrcBlend(void)
{
	I3G_BLEND blend;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_SRCBLEND, (DWORD *) & blend));
	
	return blend;
}

void i3RenderContext::SetDestBlend( I3G_BLEND blend)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, (DWORD) blend));
}

I3G_BLEND i3RenderContext::GetDestBlend(void)
{
	I3G_BLEND blend;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_DESTBLEND, (DWORD *) &blend));

	return blend;
}

// Fog
void i3RenderContext::SetFogEnable( BOOL bFlag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGENABLE, (DWORD) bFlag));
}

BOOL i3RenderContext::GetFogEnable(void)
{
	BOOL rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGENABLE, (DWORD *) & rv));

	return rv;
}

void i3RenderContext::SetFogColor( COLORREAL * pColor)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGCOLOR, (DWORD) pColor));
}

void i3RenderContext::GetFogColor( COLORREAL * pColor)
{
	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGCOLOR, (DWORD *) pColor));
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

void i3RenderContext::SetFogTableEnable( BOOL bFlag)
{
}

BOOL i3RenderContext::GetFogTableEnable(void)
{
	return FALSE;
}

void i3RenderContext::SetFogNearFar( REAL32 fNear, REAL32 fFar)
{
	DWORD sp1, sp2;

	sp1 = *((DWORD *) & fNear);
	sp2 = *((DWORD *) & fFar);

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGSTART, sp1));

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGEND, sp2));
}

REAL32 i3RenderContext::GetFogNear(void)
{
	REAL32 fNear;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGSTART, (DWORD *) & fNear));
	
	return fNear;
}

REAL32 i3RenderContext::GetFogFar(void)
{
	REAL32 fFar;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGEND, (DWORD *) & fFar));

	return fFar;
}

void i3RenderContext::SetFogDensity( REAL32 fVal)
{
	DWORD sp = *((DWORD *) &fVal);

	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_FOGDENSITY, sp));
}

REAL32 i3RenderContext::GetFogDensity(void)
{
	REAL32 rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_FOGDENSITY, (DWORD *) &rv));

	return rv;
}

// Anti-aliase
void i3RenderContext::SetEdgeAntiAliasEnable( BOOL bFlag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, (DWORD) bFlag));
}

BOOL i3RenderContext::GetEdgeAntiAliasEnable(void)
{
	BOOL rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_EDGEANTIALIAS, (DWORD *) & rv));

	return rv;
}

// Stencil Test
BOOL i3RenderContext::GetStencilAvailable(void)
{
	return (m_ClearFlag & I3G_CLEAR_STENCIL) != 0;
}

void i3RenderContext::SetStencilEnable( BOOL bFlag)
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_STENCILENABLE, (DWORD) bFlag));
}

BOOL i3RenderContext::GetStencilEnable(void)
{
	BOOL rv;

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
void i3RenderContext::SetClippingEnable( BOOL bFlag)							// default == TRUE
{
	I3ASSERT(0);
	// D3DRS_CLIPPING is not supported on Xbox:
	// CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_CLIPPING, (DWORD) bFlag));
}

BOOL i3RenderContext::GetClippingEnable(void)
{
	// D3DRS_CLIPPING is not supported on Xbox:
	// CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_CLIPPING, (DWORD *) &rv));

	I3ASSERT(0);	
	return FALSE;
}

// Lighting
void i3RenderContext::SetLightingEnable( BOOL bFlag)							// default == FALSE
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, (DWORD) bFlag));
}

BOOL i3RenderContext::GetLightingEnable(void)
{
	BOOL rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_LIGHTING, (DWORD *) & rv));

	return rv;
}

// Vertex Color Enable
void i3RenderContext::SetVertexColorEnable( BOOL bFlag)						// default == TRUE
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_COLORVERTEX, (DWORD) bFlag));
}

BOOL i3RenderContext::GetVertexColorEnable(void)
{
	BOOL rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_COLORVERTEX, (DWORD *) & rv));

	return rv;
}

// Normalize Normal Enable
void i3RenderContext::SetNormalizeNormalEnable( BOOL bFlag)					// default == FALSE
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, (DWORD) bFlag));
}

BOOL i3RenderContext::GetNormalizeNormalEnable(void)
{
	BOOL rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_NORMALIZENORMALS, (DWORD *) &rv));

	return rv;
}

// Material
void i3RenderContext::SetMaterial(	COLORREAL * pAmbientColor,
									COLORREAL *	pDiffuseColor,
									COLORREAL * pSpecularColor,
									COLORREAL * pEmissiveColor)
{
	D3DMATERIAL8 mat;

	i3Color::Set( (COLORREAL *) &mat.Ambient, pAmbientColor);
	i3Color::Set( (COLORREAL *) &mat.Diffuse, pDiffuseColor);
	i3Color::Set( (COLORREAL *) &mat.Specular, pSpecularColor);
	i3Color::Set( (COLORREAL *) &mat.Emissive, pEmissiveColor);

	CHECKRESULT( m_pD3DDevice->SetMaterial( &mat));
}

void i3RenderContext::GetMaterialAmbient( COLORREAL * pColor)
{
	D3DMATERIAL8 mat;

	CHECKRESULT( m_pD3DDevice->GetMaterial( &mat));

	i3Color::Set( pColor, (COLORREAL *) &mat.Ambient);
}

void i3RenderContext::GetMaterialDiffuse( COLORREAL * pColor)
{
	D3DMATERIAL8 mat;

	CHECKRESULT( m_pD3DDevice->GetMaterial( &mat));

	i3Color::Set( pColor, (COLORREAL *) &mat.Diffuse);
}

void i3RenderContext::GetMaterialSpecular( COLORREAL * pColor)
{
	D3DMATERIAL8 mat;

	CHECKRESULT( m_pD3DDevice->GetMaterial( &mat));

	i3Color::Set( pColor, (COLORREAL *) &mat.Specular);
}

void i3RenderContext::GetMaterialEmissive( COLORREAL * pColor)
{
	D3DMATERIAL8 mat;

	CHECKRESULT( m_pD3DDevice->GetMaterial( &mat));

	i3Color::Set( pColor, (COLORREAL *) &mat.Emissive);
}

// Vertex Blend Mode
void i3RenderContext::SetVertexBlendEnable( BOOL bFlag)						// default == TRUE
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_VERTEXBLEND, (DWORD) bFlag));
}

BOOL i3RenderContext::GetVertexBlendEnable(void)
{
	BOOL rv;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_VERTEXBLEND, (DWORD *) & rv));

	return rv;
}

void i3RenderContext::SetSoftwareVertexBlendEnable( BOOL bFlag)				// default == FALSE
{
	I3ASSERT(0);
	// D3DRS_SOFTWAREVERTEXPROCESSING is not supported on Xbox
	// CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, (DWORD) bFlag));
}


BOOL i3RenderContext::GetSoftwareVertexBlendEnable(void)
{
	// D3DRS_SOFTWAREVERTEXPROCESSING is not supported on Xbox
	// CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, (DWORD *) &rv));
	I3ASSERT(0);
	return FALSE;
}

// Point-Sprite
void i3RenderContext::SetPointSpriteSize( REAL32 fVal)						// default == 1.0f
{
	CHECKRESULT( m_pD3DDevice->SetRenderState( D3DRS_POINTSIZE, *((DWORD *) & fVal)));
}

REAL32 i3RenderContext::GetPointSpriteSize(void)
{
	REAL32 val;

	CHECKRESULT( m_pD3DDevice->GetRenderState( D3DRS_POINTSIZE, (DWORD *) & val));

	return val;
}

void i3RenderContext::SetLight( i3Light * pLight)
{
	CHECKRESULT( m_pD3DDevice->LightEnable( pLight->GetIndex(), pLight->GetEnable()));

	if( pLight->GetEnable())
	{
		CHECKRESULT( m_pD3DDevice->SetLight( pLight->GetIndex(), pLight->GetDxLight()));
	}
}

//
///////////////////////////////////////////////////////////

#endif