#include "i3GFXType.h"
#include "../i3Base/i3Debug.h"

#ifdef I3G_DX
#include "i3GfxGlobalVariableDX.h"
#include "i3RenderTarget.h"
#include "i3GfxUtil.h"
#include "i3GfxUtilDX.h"
#include "i3ImageUtil.h"
#include "d3dx9tex.h"

#include "i3TextureDX.h"

#include "i3Base/string/compare/generic_compare.h"

I3_CLASS_INSTANCE( i3RenderTarget);

i3RenderTarget::i3RenderTarget(void)
{
	m_ResType = I3G_RESOURCE_TYPE_RENDERTARGET;
}

i3RenderTarget::~i3RenderTarget(void)
{
	I3_SAFE_RELEASE(m_pColorTex);
	I3_SAFE_RELEASE(m_pDepthTex);

	if( m_pColorSurface != nullptr)
	{
		m_pColorSurface->Release();
		m_pColorSurface = nullptr;
	}

	if( m_pDepthSurface != nullptr)
	{
		m_pDepthSurface->Release();
		m_pDepthSurface = nullptr;
	}
}

bool i3RenderTarget::Create( UINT32 width, UINT32 height, I3G_IMAGE_FORMAT Format, char * ppName, bool isVolatile)
{
	m_Width = width;
	m_Height = height;
	m_Format = Format;

	m_pColorTex = i3Texture::new_object();
	m_bTextureCreated = m_pColorTex->Create(width, height, 1, m_Format, I3G_USAGE_RENDERTARGET);

	m_pDepthTex = i3Texture::new_object();
	m_bTextureCreated = m_pDepthTex->Create(width, height, 1, I3G_IMAGE_FORMAT_D24S8, I3G_USAGE_RENDERTARGET);

	m_ResType = GetType();

	if(ppName != nullptr)
	{
		SetName(ppName);
	}

	m_bIsVolatile = isVolatile;

	return true;
}

bool i3RenderTarget::Bind( IDirect3DSurface9 * pColor, IDirect3DSurface9 * pDepth)
{
	I3ASSERT(m_bTextureCreated != true);

	D3DSURFACE_DESC Desc;
	HRESULT Rv;

	m_pColorSurface = pColor;
	m_pDepthSurface = pDepth;

	if( m_pColorSurface != nullptr)
	{
		Rv = m_pColorSurface->GetDesc( &Desc);
		#if defined( __I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR( "i3RenderTarget::Bind() failed.", Rv);
			return false;
		}
		#endif

		m_Width = Desc.Width;
		m_Height = Desc.Height;
		m_Format = i3DXUT_GetI3ImageFormat( Desc.Format);
		m_ResType = GetType();
	
		I3_SAFE_ADDREF_NODBG( m_pColorSurface);
	}

	if( m_pDepthSurface != nullptr)
	{
		I3_SAFE_ADDREF_NODBG( m_pDepthSurface);

		Rv = m_pDepthSurface->GetDesc( &Desc);
		#if defined( __I3_DEBUG)
		if( FAILED( Rv))
		{
			DXTRACE_ERR( "i3RenderTarget::Bind() failed.", Rv);
			return false;
		}
		#endif

		m_DepthFormat = i3DXUT_GetI3ImageFormat( Desc.Format);
	}

	m_bTextureCreated = false;
	return true;
}

I3G_RESOURCE_TYPE i3RenderTarget::GetSurfaceType(void)
{
	HRESULT Rv;
	D3DSURFACE_DESC Desc;

	I3ASSERT( m_pColorSurface);

	Rv = m_pColorSurface->GetDesc( &Desc);
	I3ASSERT( !FAILED( Rv));

	return i3DXUT_GetI3ResourceType( Desc.Type);
}

void * i3RenderTarget::Lock( I3G_LOCKMETHOD lockMethod)
{
	HRESULT Rv;
	D3DLOCKED_RECT 	lockInfo;

	I3ASSERT( m_pColorSurface);

	Rv = m_pColorSurface->LockRect( &lockInfo, nullptr, lockMethod);
	I3ASSERT( !FAILED( Rv));

	m_Pitch = (UINT32) lockInfo.Pitch;

	return lockInfo.pBits;
}

void i3RenderTarget::Unlock(void)
{
	I3ASSERT( m_pColorSurface);

	m_pColorSurface->UnlockRect();
}

bool i3RenderTarget::Destroy(bool bLostDevice)
{
	if(m_bTextureCreated)
	{
		I3_SAFE_RELEASE(m_pColorTex);
		I3_SAFE_RELEASE(m_pDepthTex);
	}

	return i3GfxResource::Destroy(bLostDevice);
}

bool i3RenderTarget::isDestroyed()
{
	if(m_bTextureCreated)
	{
		return (m_pColorTex == nullptr && m_pDepthTex == nullptr);
	}
	else
	{
		return (m_pColorSurface == nullptr && m_pDepthSurface == nullptr);
	}
}

bool i3RenderTarget::Revive(i3RenderContext * pCtx)
{
	bool isRevived = false;
	if(m_bTextureCreated)
	{

		isRevived = Create(m_Width, m_Height, m_Format);
	}

	return isRevived;
}

#if defined( I3_DEBUG)
void i3RenderTarget::OnQueryStat( I3G_RES_STAT * pInfo)
{
	pInfo->m_RTCount++;

	pInfo->m_RTSize += i3Gfx::CalcRasterBufferSize( m_Width, m_Height, 4, m_Format);
	pInfo->m_RTSize += i3Gfx::CalcRasterBufferSize( m_Width, m_Height, 4, m_DepthFormat);
}

void i3RenderTarget::SaveDebugImage(char * pszPath)
{
	IDirect3DTexture9 * pTex = (IDirect3DTexture9 *) ((i3TextureDX *)m_pColorTex)->GetDXImage();

	D3DXIMAGE_FILEFORMAT fmt = D3DXIFF_PNG;

	{
		char  szExt[128];

		i3::extract_fileext(pszPath, szExt);

		if( i3::generic_compare( szExt, "BMP") == 0)
			fmt = D3DXIFF_BMP;
		else if( i3::generic_compare( szExt, "JPG") == 0)
			fmt = D3DXIFF_JPG;
		else if( i3::generic_compare( szExt, "TGA") == 0)
			fmt = D3DXIFF_TGA;
		else if( i3::generic_compare( szExt, "DDS") == 0)
			fmt = D3DXIFF_DDS;
		else if( i3::generic_compare( szExt, "PPM") == 0)
			fmt = D3DXIFF_PPM;
		else if( i3::generic_compare( szExt, "HDR") == 0)
			fmt = D3DXIFF_HDR;
	}

	::D3DXSaveTextureToFile( pszPath, fmt, pTex, NULL);
}

#endif


#endif