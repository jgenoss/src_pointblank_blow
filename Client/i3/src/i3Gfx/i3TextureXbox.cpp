#include "i3GfxType.h"

#if defined( I3G_XBOX)
#include "i3GfxUtil.h"
#include "i3TextureXbox.h"
#include "i3GfxUtilXbox.h"
#include "i3GfxGlobalVariableXbox.h"
#include "i3ImageFile.h"

//#if !defined( I3G_SOFTWARE)
//I3_CONCRETE_CLASS_INSTANCE( i3TextureXbox, i3Texture);
//#else
//I3_CLASS_INSTANCE( i3TextureXbox, i3Texture);
//#endif
I3_CLASS_INSTANCE( i3TextureXbox);


i3TextureXbox::i3TextureXbox(void)
{
	m_pTexture = nullptr;

	m_LockedCount = 0;
}

i3TextureXbox::~i3TextureXbox(void)
{
	if( m_pTexture != nullptr)
	{
		m_pTexture->Release();
		m_pTexture = nullptr;
	}
}

void i3TextureXbox::SetPlatformTexture( IDirect3DTexture8 * pTexture)
{
	HRESULT Rv;
	D3DSURFACE_DESC desc;

	if( m_pTexture != nullptr)
	{
		m_pTexture->Release();
	}

	m_pTexture = pTexture;

	Rv = m_pTexture->GetLevelDesc( 0, &desc);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR( "i3TextureXbox::SetDXTexture() - Could not retreive information about the image.", Rv);
		return;
	}
	#endif

	// setting up actual image informations.
	m_Width			= desc.Width;
	m_Height		= desc.Height;
	m_Format		= i3_GetI3ImageFormat( desc.Format);
	m_BitsPerPixel	= i3Gfx::GetImageFormatBitCount( m_Format);
	m_Stride		= (m_BitsPerPixel + 7) >> 3;
	m_Pitch			= (m_Width * m_BitsPerPixel + 7) >> 3;
	m_DataSize		= desc.Size;
}

bool i3TextureXbox::Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_USAGE Usage, I3G_IMAGE_FORMAT format)
{
	HRESULT Rv;
	IDirect3DTexture8 * pTex;
	DWORD rUsage;

	i3Texture::Create( width, height, Levels, Usage, format);

	rUsage = i3_GetNativeUsage( Usage);

	Rv = D3DXCreateTexture( g_pD3DDevice, width, height, Levels, rUsage, 
			i3_GetNativeImageFormat( format), D3DPOOL_MANAGED, &pTex);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR( "i3TextureXbox::Create() - Could not create IDirect3DTexture8 object.", Rv);
		return false;
	}
	#endif

	SetPlatformTexture( pTex);

	return true;
}

bool i3TextureXbox::Create( const char * pszPath, UINT32 /*Levels*/)
{
	i3ImageFile file;
	i3FileStream Stream;

	if( Stream.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		i3Log( "i3TextureXbox::Create()", "Could not open %s file.", pszPath);
		return false;
	}

	if( file.Load( pszPath, this) == false)
	{
		return false;
	}

	return true;
}

char * i3TextureXbox::Lock( INT32 /*level*/, I3G_LOCKMETHOD lockmode)
{
	HRESULT Rv;

	I3ASSERT( m_pTexture != nullptr);

	m_LockedCount++;

	if( m_LockedCount >= 2)
		return (char *) m_LockRect.pBits;

	Rv = m_pTexture->LockRect( 0, &m_LockRect, NULL, lockmode);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR( "i3TextureXbox::Lock() - Could not lock the image.", Rv);
		return nullptr;
	}
	#endif

	return (char *) m_LockRect.pBits;
}

void i3TextureXbox::Unlock( INT32 level)
{
	HRESULT Rv;

	I3ASSERT( m_pTexture != nullptr);

	m_LockedCount --;

	if( m_LockedCount > 0)
		return;

	Rv = m_pTexture->UnlockRect( level);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR( "i3TextureXbox::Unlock() - Could not unlock the image.", Rv);
		return;
	}
	#endif
}

void i3TextureXbox::MakeRuntimeForm(void)
{
	char * pSrc, * pDest;
	INT32 i, j;
	UINT8 r, g, b, a;
	UINT32 ImageSize;

	// Memory Alignment에 의해 Runtime Form이 필요로 하는 메모리 공간이 더 클 수 있기 때문에,
	// Persist Form Data의 뒤에서부터 앞의 방향으로 Data Convertion을 한다.

	if( GetBitsPerPixel() < 24)
		return;

	ImageSize = ((GetWidth() * GetBitsPerPixel() + 7) >> 3) * GetHeight();

	pSrc = Lock( 0);

	pDest = pSrc + GetDataSize();
	pSrc = pSrc + ImageSize;

	for( i = 0; i < (INT32) GetHeight(); i++)
	{
		// Line Alignment


		for( j = 0; j < (INT32) GetWidth(); j++)
		{
			// Persist format			(RGBA [ A, B, G, R])
			// to
			// DirectX Runtime format	(ARGB [ G, B, R, A])
			switch( GetFormat())
			{
				case I3G_IMAGE_FORMAT_RGB_888		:
					pSrc -= 3;
					pDest -= 3;

					r = pSrc[0];
					g = pSrc[1];
					b = pSrc[2];

					pDest[2] = r;
					pDest[0] = b;
					pDest[1] = g;
					break;

				case I3G_IMAGE_FORMAT_XRGB_8888	:
				case I3G_IMAGE_FORMAT_ARGB_8888	:
					pSrc -= 4;
					pDest -= 4;

					r = pSrc[0];
					g = pSrc[1];
					b = pSrc[2];
					a = pSrc[3];

					pDest[2] = r;
					pDest[0] = b;
					pDest[1] = g;
					pDest[3] = a;
					break;

			}
		}
	}

	Unlock();
}

/*
void i3TextureXbox::MakePersistForm(void)
{
	char * pSrc, * pDest;
	INT32 i, j;
	UINT8 r, g, b, a;

	if( GetBitsPerPixel() < 24)
		return;

	pSrc = Lock( 0);
	pDest = pSrc;

	for( i = 0; i < (INT32) GetHeight(); i++)
	{
		for( j = 0; j < (INT32) GetWidth(); j++)
		{
			// DirectX Runtime format	(ARGB [ G, B, R, A])
			// to
			// Persist format			(RGBA [ A, B, G, R])
			switch( GetFormat())
			{
				case I3G_IMAGE_FORMAT_RGB_888		:
					r = pSrc[2];
					g = pSrc[0];
					b = pSrc[1];

					pDest[2] = r;
					pDest[1] = g;
					pDest[0] = b;

					pSrc += 3;
					pDest += 3;
					break;

				case I3G_IMAGE_FORMAT_XRGB_8888	:
				case I3G_IMAGE_FORMAT_ARGB_8888	:
					r = pSrc[2];
					g = pSrc[0];
					b = pSrc[1];
					a = pSrc[3];

					pDest[3] = r;
					pDest[2] = g;
					pDest[1] = b;
					pDest[0] = a;

					pSrc += 4;
					pDest += 4;
					break;

			}
		}

		// Line alignment
	}

	Unlock();
}
*/
#endif