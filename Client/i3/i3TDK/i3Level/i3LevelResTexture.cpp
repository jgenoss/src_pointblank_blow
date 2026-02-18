#include "stdafx.h"
#include "i3LevelResTexture.h"

I3_CLASS_INSTANCE( i3LevelResTexture);

#define USE_THUMBNAIL_ALPHABLEND

i3LevelResTexture::i3LevelResTexture(void)
{
	m_ResType		= I3LV_TYPE_TEXTURE;
}

i3LevelResTexture::~i3LevelResTexture(void)
{
	I3_SAFE_RELEASE( m_pTexture);

	if( m_hBitmap != nullptr)
	{
		::DeleteObject( m_hBitmap);
		m_hBitmap = nullptr;
	}
}

void i3LevelResTexture::_UpdateInfo( i3Texture * pTex)
{
	m_Width = pTex->GetWidth();
	m_Height = pTex->GetHeight();
	m_Format = pTex->GetFormat();
	m_MipCount = pTex->GetLevelCount();
}

HBITMAP i3LevelResTexture::_MakeThumbnail( i3Texture * pTex)
{
	INT32 cx, cy, prx, pry, x, y, i, j;
	HBITMAP hBitmap;
	COLORREAL col, allcol;
	UINT32 *	pBits;

	if( i3::kind_of<i3TextureCube*>(pTex))
		return nullptr;

	//calculate size
	{
		INT32 max, min, d;

		max = ( pTex->GetWidth() >= pTex->GetHeight()) ? pTex->GetWidth() : pTex->GetHeight();
		min	= MIN( 64, max);
		d	= max/min;

		m_IconWidth		= cx = pTex->GetWidth() / d;
		m_IconHeight	= cy = pTex->GetHeight() / d;

		prx = pTex->GetWidth() / cx;
		pry = pTex->GetHeight() / cy;
	}

	pTex->Lock( 0);

	{
		BITMAPINFO bmi;
		HDC dc;

		memset( &bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = cx;
		bmi.bmiHeader.biHeight = - cy;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biPlanes = 1;

		dc = ::GetDC( nullptr);

		hBitmap = CreateDIBSection( dc, &bmi, DIB_RGB_COLORS, (void **) &pBits, nullptr, 0);
		I3ASSERT( hBitmap != nullptr);

		::ReleaseDC( nullptr, dc);
	}

	for( y = 0; y < cy; y ++)
	{
		for( x = 0; x < cx; x++)
		{
			//
			i3Color::Set( &allcol, 0.0f, 0.0f, 0.0f, 0.0f);

			// Sampling
			for( i = 0; i < pry; i++)
			{
				for( j = 0; j < prx; j++)
				{
					pTex->GetPixel( (x * prx) + j, (y * pry) + i, &col);

					i3Vector::Add( (VEC4D *) &allcol, (VEC4D *) &allcol, (VEC4D *) &col);
				}
			}

			// Averaging
			allcol.r *= 1.0f / (prx * pry);
			allcol.g *= 1.0f / (prx * pry);
			allcol.b *= 1.0f / (prx * pry);
			allcol.a *= 1.0f / (prx * pry);

#if defined( USE_THUMBNAIL_ALPHABLEND)
			REAL32 Back = (REAL32)(((((x >> 2) ^ (y >> 2)) & 0x01) << 6) + 128) * 0.003921568627450980392156862745098f;

			pBits[ (y * cx) + x]	=	((UINT32)((( allcol.r * allcol.a) + ((1.0f - allcol.a) * Back)) * 255.0f) & 0xFF) << 16 |
										((UINT32)((( allcol.g * allcol.a) + ((1.0f - allcol.a) * Back)) * 255.0f) & 0xFF) << 0 |
										((UINT32)((( allcol.b * allcol.a) + ((1.0f - allcol.a) * Back)) * 255.0f) & 0xFF) << 8 |
										0x00000000;
#else
			pBits[ (y * cx) + x]	=	(( (UINT32)(allcol.r * 255.0f) & 0xFF) << 16) |
										(( (UINT32)(allcol.g * 255.0f) & 0xFF) << 0) |
										(( (UINT32)(allcol.b * 255.0f) & 0xFF) << 8) |
										0x00000000;
#endif
		}
	}

	pTex->Unlock( 0);

	return hBitmap;
}

// 간략한 정보를 Validate 시키기 위해 Scene에서 호출된다.
// 실제 Resource를 위한 메모리를 할당하는 등의 처리는 하지 않는다.
bool	i3LevelResTexture::OnValidateBrief( bool bForce)
{
	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;

		if( m_hBitmap != nullptr)
		{
			addResState( I3LV_RES_STATE_BRIEF_LOADED);
			return true;
		}
	}

	if( m_hBitmap != nullptr)
	{
		::DeleteObject( m_hBitmap);
		m_hBitmap = nullptr;
	}

	i3ImageFile file;
	i3Texture * pTex;
	char szFull[ MAX_PATH];

	getFullPath( szFull);

	pTex = file.Load( szFull);

	if( pTex == nullptr)
		return false;

	_UpdateInfo( pTex);
	
	// VRAM의 과도한 낭비를 막기 위해 모든 Texture를 64x64 이하 크기로 줄인 Thumbnail Image만 생성해두고
	// 버린다.
	m_hBitmap = _MakeThumbnail( pTex);

	{
		/*
		SIZE sz;
		//An application can retrieve the dimensions assigned to a bitmap with the SetBitmapDimensionEx function 
		//by calling the GetBitmapDimensionEx function. 
		//The bitmap identified by hBitmap cannot be a DIB section,
		//which is a bitmap created by the CreateDIBSection function.
		//If the bitmap is a DIB section, the SetBitmapDimensionEx function fails.

		
		//If those dimensions have not yet been set, the structure that is returned will have zeroes in those fields.
		GetBitmapDimensionEx( m_hBitmap, &sz);

		if( sz.cx != 0 && sz.cy != 0)
		{
			m_IconWidth		= sz.cx;
			m_IconHeight	= sz.cy;
		}*/
	}

	I3_MUST_RELEASE(pTex);

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

// 실제 Resource를 메모리로 읽어 들이는 단계.
bool	i3LevelResTexture::OnValidate( i3LevelScene * pScene, bool bForce)
{
	if( bForce == false)
	{
		if( isLoaded())
			return true;
	}

	I3_SAFE_RELEASE( m_pTexture);

	BeginValidate();
	{
		// 실제 i3Texture를 읽어들인다.
		i3ImageFile file;
		char szFull[ MAX_PATH];

		getFullPath( szFull);

		m_pTexture = file.Load( szFull);
		if (m_pTexture)
			m_pTexture->SetName( getPath());
	}
	EndValidate();

	if( m_pTexture == nullptr)
		return false;

	m_pTexture->SetExternRes( TRUE);

	_UpdateInfo( m_pTexture);
	
	addResState( I3LV_RES_STATE_LOADED);

	return true;
}

i3GameRes *	i3LevelResTexture::CreateGameRes(void)
{
	i3GameResTexture * pRes;

	pRes = i3GameResTexture::new_object();
	pRes->setTexture( m_pTexture);

	return pRes;
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)
#endif

namespace pack
{
	struct RES_TEXTURE
	{
		UINT8		m_ID[4] = { 'R', 'S', 'T', '1' };
		UINT32		m_Width = 0;
		UINT32		m_Height = 0;
		UINT32		m_Format = 0;
		UINT32		m_LevelCount = 0;
		UINT32		m_IconWidth = 0;
		UINT32		m_IconHeight = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32	i3LevelResTexture::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_TEXTURE data;

	Result = i3LevelRes::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	data.m_Width = getWidth();
	data.m_Height = getHeight();
	data.m_Format = (UINT32) getFormat();
	data.m_LevelCount = getMipmapCount();
	data.m_IconWidth = m_IconWidth;
	data.m_IconHeight = m_IconHeight;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	// Thumbnail Image를 저장해둔다.
	if( m_hBitmap != nullptr)
	{
		BITMAPINFO bmi;
		HDC dc = ::GetDC( nullptr);
		char * pBits;
		UINT32 sz;

		sz = m_IconWidth * m_IconHeight * 4;

		pBits = new char[ sz];

		memset( &bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_IconWidth;
		bmi.bmiHeader.biHeight = -((INT32)m_IconHeight);
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biPlanes = 1;

		GetDIBits( dc, m_hBitmap, 0, m_IconHeight, (LPVOID) pBits, &bmi, DIB_RGB_COLORS);

		Rc = pResFile->Write( pBits, sz);
		I3_CHKIO( Rc);
		Result += Rc;

		::ReleaseDC( nullptr, dc);
		delete [] pBits;
	}

	return Result;
}

UINT32	i3LevelResTexture::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_TEXTURE data;

	Result = i3LevelRes::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_Width = data.m_Width;
	m_Height = data.m_Height;
	m_Format = (I3G_IMAGE_FORMAT) data.m_Format;
	m_MipCount = data.m_LevelCount;

	m_IconWidth = data.m_IconWidth;
	m_IconHeight = data.m_IconHeight;

	if( m_IconHeight > 0)
	{
		UINT32 sz;
		BITMAPINFO bmi;
		HDC dc;
		char * pBits;

		if( m_hBitmap != nullptr)
		{
			::DeleteObject( m_hBitmap);
			m_hBitmap = nullptr;
		}

		memset( &bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_IconWidth;
		bmi.bmiHeader.biHeight = -((INT32)m_IconHeight);
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biPlanes = 1;

		sz = (m_IconWidth * m_IconHeight * 4);

		dc = ::GetDC( nullptr);

		m_hBitmap = CreateDIBSection( dc, &bmi, DIB_RGB_COLORS, (void **) &pBits, nullptr, 0);
		I3ASSERT( m_hBitmap != nullptr);

		Rc = pResFile->Read( pBits, sz);
		I3_CHKIO( Rc);
		Result += Rc;

		::ReleaseDC( nullptr, dc);
	}

	return Result;
}
