#include "i3GFXType.h"
#include "../i3Base/i3Debug.h"

#ifdef I3G_XBOX
#include "i3GfxGlobalVariableXbox.h"
#include "i3RenderTarget.h"
#include "i3GfxUtilXbox.h"

I3_CLASS_INSTANCE( i3RenderTarget, i3GfxResource);

i3RenderTarget::i3RenderTarget(void)
{
	m_Width = 0;
	m_Height = 0;
	m_Format = I3G_IMAGE_FORMAT_NONE;
	m_bLockable = FALSE;
	m_bLocked = FALSE;
	m_Pitch = 0;

	m_ResType = I3G_RESOURCE_TYPE_RENDERTARGET;

	m_pSurface = NULL;
}

i3RenderTarget::~i3RenderTarget(void)
{
	if( m_pSurface != NULL)
	{
		m_pSurface->Release();
		m_pSurface = NULL;
	}
}

BOOL i3RenderTarget::Create( UINT32 width, UINT32 height, I3G_IMAGE_FORMAT Format, BOOL bLockable)
{
	HRESULT Rv;
	D3DFORMAT	d3dFormat;

	m_Width = width;
	m_Height = height;
	m_Format = Format;
	m_bLockable = bLockable;

	d3dFormat = i3_GetNativeImageFormat( Format);

	Rv = g_pD3DDevice->CreateRenderTarget( width, height,
						d3dFormat, 
						D3DMULTISAMPLE_NONE,		// with no anti-aliasing
						bLockable,
						&m_pSurface);
	I3ASSERT( !FAILED( Rv));

	m_ResType = GetType();

	return TRUE;
}

BOOL i3RenderTarget::Create( void * pNativeRTHandle)
{
	D3DSURFACE_DESC Desc;
	HRESULT Rv;

	m_pSurface = (IDirect3DSurface8 *) pNativeRTHandle;

	Rv = m_pSurface->GetDesc( &Desc);
	#if defined( __I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR( "i3RenderTarget::Create() failed.", Rv);
		return FALSE;
	}
	#endif

	m_Width = Desc.Width;
	m_Height = Desc.Height;
	m_Format = i3_GetI3ImageFormat( Desc.Format);
	m_ResType = GetType();

	return TRUE;
}

UINT32 i3RenderTarget::GetBufferSize(void)
{
	HRESULT Rv;
	D3DSURFACE_DESC Desc;

	I3ASSERT( m_pSurface);

	Rv = m_pSurface->GetDesc( &Desc);
	I3ASSERT( !FAILED( Rv));

	return Desc.Size;
}

I3G_RESOURCE_TYPE i3RenderTarget::GetSurfaceType(void)
{
	HRESULT Rv;
	D3DSURFACE_DESC Desc;

	I3ASSERT( m_pSurface);

	Rv = m_pSurface->GetDesc( &Desc);
	I3ASSERT( !FAILED( Rv));

	return i3_GetI3ResourceType( Desc.Type);
}

void * i3RenderTarget::Lock( I3G_LOCKMETHOD lockMethod)
{
	HRESULT Rv;
	D3DLOCKED_RECT 	lockInfo;

	I3ASSERT( m_pSurface);

	Rv = m_pSurface->LockRect( &lockInfo, NULL, lockMethod);
	I3ASSERT( !FAILED( Rv));

	m_Pitch = (UINT32) lockInfo.Pitch;

	return lockInfo.pBits;
}

void i3RenderTarget::Unlock(void)
{
	I3ASSERT( m_pSurface);

	m_pSurface->UnlockRect();
}

#endif