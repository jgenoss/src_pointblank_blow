#include "i3GfxType.h"

#if defined( I3G_DX)
#include "i3TextureCubeDX.h"
#include "i3GfxUtil.h"
#include "i3GfxUtilDX.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3ImageFile.h"

I3_CLASS_INSTANCE( i3TextureCubeDX);

static D3DCUBEMAP_FACES s_Face[6] =
{
	D3DCUBEMAP_FACE_POSITIVE_X,
	D3DCUBEMAP_FACE_NEGATIVE_X,
	D3DCUBEMAP_FACE_POSITIVE_Y,
	D3DCUBEMAP_FACE_NEGATIVE_Y,
	D3DCUBEMAP_FACE_POSITIVE_Z,
	D3DCUBEMAP_FACE_NEGATIVE_Z
};

i3TextureCubeDX::i3TextureCubeDX(void)
{
	memset(&m_LockRect, 0 , sizeof(D3DLOCKED_RECT));
}

i3TextureCubeDX::~i3TextureCubeDX(void)
{
	if( m_pCubeTex != nullptr)
	{
		m_pCubeTex->Release();
		m_pCubeTex = nullptr;
	}
}

void i3TextureCubeDX::SetDXTexture( IDirect3DCubeTexture9 * pTexture)
{
	HRESULT Rv;
	INT32 i, j;
	i3Surface * pSurface;
	IDirect3DSurface9 * pDXSurface;

	if( m_pCubeTex != nullptr)
	{
		m_pCubeTex->Release();
	}

	m_pCubeTex = pTexture;

	{
		INT32 gap = (m_pCubeTex->GetLevelCount() * 6) - getSurfaceCount();

		if( gap > 0)
		{
			for( i = 0; i < gap; i++)
			{
				pSurface = i3Surface::new_object();

				m_SurfaceList.push_back( pSurface);
			}
		}
		else if( gap < 0)
		{
			for( i = 0; i < -gap; i++)
			{
				pSurface = getSurface( m_pCubeTex->GetLevelCount());

				I3_MUST_RELEASE(pSurface);

				m_SurfaceList.erase(m_SurfaceList.begin() + m_pCubeTex->GetLevelCount());
			}
		}
	}

	for( i = 0; i < (INT32) m_pCubeTex->GetLevelCount(); i++)
	{
		for( j = 0; j < 6; j++)
		{
			pSurface = getSurface( (i * 6) + j);

			Rv = m_pCubeTex->GetCubeMapSurface( s_Face[j], i, &pDXSurface);
			#if defined( I3_DEBUG)
			if( FAILED( Rv))
			{
				DXTRACE_ERR(L"i3TextureDX::SetDXTexture() - Could not retreive information about the image.", Rv);
				return;
			}
			#endif

			pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag());
		}
	}

	// setting up actual image informations.
	pSurface = getSurface( 0);

	m_Width			= pSurface->getWidth();
	m_Height		= pSurface->getHeight();
	_setFormat(		 pSurface->getFormat());

	if( m_PersistFormat == I3G_IMAGE_FORMAT_NONE)
	{
		m_PersistFormat = pSurface->getFormat();
	}
}

bool i3TextureCubeDX::Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage)
{
	HRESULT Rv;
	IDirect3DCubeTexture9 * pTex;
	DWORD rUsage;

	I3ASSERT( g_pD3DDevice != nullptr);

	i3Texture::Create( width, height, Levels, format, Usage);

	rUsage = i3DXUT_GetNativeUsage( Usage);

	Rv = D3DXCreateCubeTexture( g_pD3DDevice, width, Levels, rUsage, 
			i3DXUT_GetNativeImageFormat( format), D3DPOOL_MANAGED, &pTex);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3TextureCubeDX::Create() - Could not create IDirect3DCubeTexture9 object.", Rv);
		return false;
	}
	#endif

	SetDXTexture( pTex);

	return true;
}

char * i3TextureCubeDX::Lock( I3G_CUBE_TEXTURE_FACE face, INT32 level, I3G_LOCKMETHOD lockmode)
{
	HRESULT Rv;

	I3ASSERT( m_pCubeTex != nullptr);

	m_LockedCount++;

	if( m_LockedCount >= 2)
		return (char *) m_LockRect.pBits;

	Rv = m_pCubeTex->LockRect( (D3DCUBEMAP_FACES)face, level, &m_LockRect, NULL, lockmode);
	if( FAILED( Rv))
	{
		#if defined( I3_DEBUG)
			DXTRACE_ERR(L"i3TextureCubeDX::Lock() - Could not lock the image.", Rv);
		#endif
		return nullptr;
	}

	return (char *) m_LockRect.pBits;
}

bool i3TextureCubeDX::Unlock( I3G_CUBE_TEXTURE_FACE face, INT32 level)
{
	HRESULT Rv;

	I3ASSERT( m_pCubeTex != nullptr);

	m_LockedCount --;

	if( m_LockedCount > 0)
		return false;

	Rv = m_pCubeTex->UnlockRect( (D3DCUBEMAP_FACES) face, level);
	if( FAILED( Rv))
	{
		#if defined( I3_DEBUG)
		DXTRACE_ERR(L"i3TextureCubeDX::Unlock() - Could not unlock the image.", Rv);
		#endif
		return false;
	}

	return true;
}

static I3G_CUBE_TEXTURE_FACE s_CubeFaceTable[6] =
{
	I3G_CUBE_TEXTURE_FACE_PX,
	I3G_CUBE_TEXTURE_FACE_NX,
	I3G_CUBE_TEXTURE_FACE_PY,
	I3G_CUBE_TEXTURE_FACE_NY,
	I3G_CUBE_TEXTURE_FACE_PZ,
	I3G_CUBE_TEXTURE_FACE_NZ
};

void i3TextureCubeDX::MakeRuntimeForm(void)
{
	char * pSrc, * pDest;
	INT32 i, j, Width, Height, face, level;
	UINT8 r, g, b, a;
	register UINT32 ImageSize;
	UINT64 t1, t2, t3;

	// Memory Alignment에 의해 Runtime Form이 필요로 하는 메모리 공간이 더 클 수 있기 때문에,
	// Persist Form Data의 뒤에서부터 앞의 방향으로 Data Convertion을 한다.

	if( GetBitsPerPixel(0) < 24)
		return;

	for( face = 0; face < 6; face++)
	{
		Width = GetWidth();
		Height = GetHeight();

		for( level = 0; level < (INT32) GetLevelCount(); level++)
		{
			ImageSize = (((i3Gfx::GetImageFormatBitCount( GetFormat()) * Width) + 7) >> 3) * Height;

			pSrc = Lock( s_CubeFaceTable[face], level, 0);

			pDest = pSrc + GetDataSize(level);
			pSrc = pSrc + ImageSize;

			// Persist format			(RGBA [ A, B, G, R])
			// to
			// DirectX Runtime format	(ARGB [ B, G, R, A])

			switch( GetFormat())
			{
				case I3G_IMAGE_FORMAT_RGB_888		:
					for( i = 0; i < (INT32) Height; i++)
					{
						for( j = 0; j < (INT32) Width; j++)
						{
							pSrc -= 3;
							pDest -= 3;

							r = pSrc[0];
							g = pSrc[1];
							b = pSrc[2];

							pDest[2] = r;
							pDest[1] = g;
							pDest[0] = b;
						}
					}
					break;

				case I3G_IMAGE_FORMAT_RGBX_8888	:
				case I3G_IMAGE_FORMAT_RGBA_8888	:
					for( i = 0; i < (INT32) Height; i++)
					{
						for( j = 0; j < (INT32) Width; j += 2)
						{
							pSrc -= 8;
							pDest -= 8;

							t1 = *((UINT64 *) pSrc);
							t2 = (t1 & 0x00FF000000FF0000L) >> 16;
							t3 = (t1 & 0x000000FF000000FFL) << 16;
							*((UINT64 *) pDest) = (t1 & 0xFF00FF00FF00FF00) | t2 | t3;
						}

						if( GetWidth() & 1)
						{
							pSrc -= 4;
							pDest -= 4;

							a = pSrc[3];
							b = pSrc[2];
							g = pSrc[1];
							r = pSrc[0];

							pDest[3] = a;
							pDest[2] = r;
							pDest[1] = g;
							pDest[0] = b;
						}
					}
					break;
			}

			Unlock( s_CubeFaceTable[ face], level);

			Width = Width >> 1;
			Height = Height >> 1;
		}
	}
}

bool i3TextureCubeDX::Destroy( bool bLostDevice)
{
	INT32 i;
	bool bLeaveSurface = false;
	bool bVolatile = false;

	// Surface를 제거하지 않고 남겨 두어야 하는 경우 
	// - Persist Data가 없으며, 내용이 Volatile이 아닌 경우
	bLeaveSurface	= (getRestoreType() == I3G_RESTORE_MEM);
	bVolatile		= (GetUsageFlag() & I3G_USAGE_RENDERTARGET) || (GetFormat() & I3G_IMAGE_FORMAT_MASK_DEPTH);

	{
		for( i = 0; i < getSurfaceCount(); i++)
		{
			i3Surface * pSurface = getSurface( i);

			if( bLeaveSurface)
				pSurface->Destroy();
			else
			{
				I3_MUST_RELEASE(pSurface);
			}
		}

		if( bLeaveSurface == false)
		{
			m_SurfaceList.clear();
		}
	}

	// DX_Texture가 제거되는 경우
	// 1. 모든 경우 : Lost Device (FullScreen <-> Window Screen)
	// 2. Render Target : Resize가 되는 경우
	if( bLostDevice || bVolatile || bLeaveSurface)
	{
		I3_SAFE_RELEASE_NODBG( m_pCubeTex);
	}

	return i3GfxResource::Destroy( bLostDevice);
}

bool	i3TextureCubeDX::isDestroyed( void)
{
	return m_pCubeTex == nullptr;
}


bool i3TextureCubeDX::Revive( i3RenderContext * pCtx)
{
	bool Rv = false;
	INT32 i;
	bool bVolatile;

	if( m_pCubeTex != nullptr)
		return true;

	if( getRestoreType() != I3G_RESTORE_FILE)
	{
		Rv = Create( GetWidth(), GetHeight(), m_LevelCount, m_PersistFormat, GetUsageFlag());

		bVolatile		= (GetUsageFlag() & I3G_USAGE_RENDERTARGET) || (GetFormat() & I3G_IMAGE_FORMAT_MASK_DEPTH);

		for( i = 0; i < getSurfaceCount(); i++)
		{
			i3Surface * pSurface = getSurface( i);

			pSurface->Revive();
		}

		SetLoaded(true);
	}
	else
	{
		i3ImageFile file;

		if( file.Load( getResourceLoadPath(), this, getResourceLoadOffset()) == false)
			return false;
	}

	return Rv;
}

#if defined( I3_DEBUG)
void i3TextureCubeDX::OnQueryStat( I3G_RES_STAT * pInfo)
{
	UINT32 i;
	INT32 w, h;

	pInfo->m_TexCount ++;

	w = GetWidth();
	h = GetHeight();

	for( i = 0; i < GetLevelCount(); i++)
	{
		UINT32 sz = i3Gfx::CalcRasterBufferSize( w, h, 4, GetFormat());

		pInfo->m_TexSize += sz * 6;

		w = w >> 1;
		h = h >> 1;
	}
}
#endif

#endif