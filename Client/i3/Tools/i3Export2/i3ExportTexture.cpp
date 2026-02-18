#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"
#include "i3Base/string/compare/generic_compare.h"
void			i3Export::_AddTexture( Bitmap * pBitmap, i3Texture * pTex)
{
	i3tool::exporter::TEXTURE_INFO * pInfo;

	pInfo = new i3tool::exporter::TEXTURE_INFO;
	
	pInfo->m_pBitmap = pBitmap;
	pInfo->m_pTex = pTex;

	m_TexList.push_back( pInfo);
}

i3Texture *		i3Export::_FindTextureByBitmap(const char * pszName, Bitmap * pBitmap)
{

	i3tool::exporter::TEXTURE_INFO * pInfo;

	for(size_t i = 0; i < m_TexList.size(); i++)
	{
		pInfo = m_TexList[i];

		if( pInfo->m_pBitmap == pBitmap)
			return pInfo->m_pTex;

		if (i3::generic_compare(pInfo->m_pTex->GetName(), pszName) == 0)
			return pInfo->m_pTex;
	}

	return NULL;
}

void			i3Export::_RemoveAllTexture(void)
{
	i3tool::exporter::TEXTURE_INFO * pInfo;

	for(size_t i = 0; i < m_TexList.size(); i++)
	{
		pInfo = m_TexList[i];

		delete pInfo;
	}

	m_TexList.clear();
}

i3Texture *		i3Export::_ConvertBitmap( Bitmap * pBitmap)
{
	INT32 width, height;
	I3G_IMAGE_FORMAT fmt;
	INT32 i, j;
	BitmapStorage * pStorage = (BitmapStorage *) pBitmap->Storage();
	i3Texture * pTex;

	width = pBitmap->Width();
	height = pBitmap->Height();

	if( pStorage->Paletted() <= 0)
	{
		// 24 bits or 32 bits
		// 16bits는 3DSMAX Internal Format으로 쓰이지 않는다.
		if( pBitmap->HasAlpha())
			fmt = I3G_IMAGE_FORMAT_RGBA_8888;
		else
			fmt = I3G_IMAGE_FORMAT_RGBX_8888;
	}
	else
	{
		if( pStorage->Paletted() <= 16)
			fmt = I3G_IMAGE_FORMAT_I4;
		else
			fmt = I3G_IMAGE_FORMAT_I8;
	}

	pTex = i3Texture::new_object();

	if( pTex->Create( width, height, 1, fmt, 0) == FALSE)
	{
		I3TRACE( "ERROR\n");
		//delete pTex;
		i3::destroy_instance(pTex);
		return NULL;
	}

	// Pixel Data Copy
	if( pStorage->Paletted() <= 0)
	{
		BMM_Color_fl * pSrcLine;
		UINT32 * pDestLine;
		UINT8 r, g, b, a;
		COLOR col;

		// 32 bits image
		pSrcLine = new BMM_Color_fl[ width];
		if( pSrcLine == NULL)
		{
			pTex->Release();
			return NULL;
		}

		pDestLine = (UINT32 *) pTex->Lock( 0);
		
		for( i = 0; i < height; i++)
		{
			pBitmap->GetPixels( 0, i, width, pSrcLine);

			for( j = 0; j < width; j++)
			{
				r = (UINT8)(pSrcLine[j].r * 255.0f);
				g = (UINT8)(pSrcLine[j].g * 255.0f);
				b = (UINT8)(pSrcLine[j].b * 255.0f);
				a = (UINT8)(pSrcLine[j].a * 255.0f);

				i3Color::Set( &col, r, g, b, a);

				pDestLine[j] = i3Color::ConvertRGBA32( &col);
			}

			pDestLine += (pTex->GetPitch() >> 2);		// 4 bytes unit
		}

		delete pSrcLine;
	}
	else
	{
		BYTE * pSrcLine;
		UINT8 * pDestLine;

		// CLUT
		{
			BMM_Color_48 * pSrc;

			pSrc = new BMM_Color_48[ pStorage->Paletted()];

			pBitmap->GetPalette( 0, pStorage->Paletted(), pSrc);

			i3Clut * pClut = i3Clut::new_object_ref();
			pClut->Create( pStorage->Paletted());
			pTex->SetClut( 0, pClut);

			for( i = 0; i < pStorage->Paletted(); i++)
			{
				COLOR col;

				i3Color::Set( &col,		(UINT8)(pSrc[i].r >> 8), 
										(UINT8)(pSrc[i].g >> 8), 
										(UINT8)(pSrc[i].b >> 8),
										(UINT8)(255));
				pClut->SetColor( i, &col);

			}

			delete pSrc;
		}

		// 4 or 8 bits image
		pSrcLine = new BYTE[ width];
		if( pSrcLine == NULL)
		{
			pTex->Release();
			return NULL;
		}

		pDestLine = (UINT8 *) pTex->Lock( 0);
		
		for( i = 0; i < height; i++)
		{
			if( pStorage->Paletted() <= 16)
			{
				pBitmap->GetIndexPixels( 0, i, width, pSrcLine);

				for( j = 0; j < width; j++)
				{
					if(( j & 1) == 0)
						pDestLine[ (j >> 1)] = (UINT8)((pSrcLine[j] << 4) & 0xF0);
					else
						pDestLine[ (j >> 1)] |= (UINT8)(pSrcLine[j] & 0x0F);
				}
			}
			else
			{
				pBitmap->GetIndexPixels( 0, i, width, pDestLine);
			}

			pDestLine += pTex->GetPitch();		// 4 bytes unit
		}

		delete pSrcLine;
	}

	pTex->Unlock(0);

	pTex->MakePersistForm();

	return pTex;
}

i3Texture *	 i3Export::_CreateTexture( char * pszName, Bitmap * pBitmap)
{
	i3Texture * pTex = NULL;
	bool bRelease = false;

	// 우선 이전에 발견된 Bitmap 중에서 동일한 것이 있다면
	// 동일한 Texture를 사용하도록...
	pTex = _FindTextureByBitmap(pszName, pBitmap);

	if( pTex == NULL)
	{
		// 만약 원본 파일을 읽어들일 수 있다면
		// 그것으로 대체한다.
		// 그것은 MAX가 읽어들여 보유하고 있는 Palette Data에서 Alpha의 값이
		// 소실되기 때문에, 실제적으로 Alpha가 있는 Image는 32 bits Format으로의
		// 생성이 불가피하다.
		pTex = TryLoadTexture( pszName);

		if( pTex != NULL)
		{
			// 이후의 참조를 위해 Texture Dic.에 추가해 둔다.
			_AddTexture( pBitmap, pTex);
		}
		else
		{
			// MAX가 가지고 있는 Bitmap 정보에서 i3Texture를 생성해 낸다.
			pTex = _ConvertBitmap( pBitmap);

			if( pTex != NULL)
			{
				pTex->SetName( pszName);

				// 이후의 참조를 위해 Texture Dic.에 추가해 둔다.
				_AddTexture( pBitmap, pTex);
			}
			else
			{
				Log( NULL, "Texture를 생성할 수 없습니다.");
			}
		}
	}
	else
	{
		pTex->AddRef();
	}

	return pTex;
}

void i3Export::_OnExportTexture( char * pszName, Bitmap * pBitmap)
{
	i3Texture * pTex;

	pTex = _CreateTexture( pszName, pBitmap);

	if( pTex != NULL)
	{
		i3TextureBindAttr * pAttr = i3TextureBindAttr::new_object_ref();

		pAttr->SetTexture( pTex);
		m_pCurProcessInfo->pushAttr( pAttr);

		pTex->Release(); 
	}
}