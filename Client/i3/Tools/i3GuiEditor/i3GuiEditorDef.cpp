#include "stdafx.h"
#include "i3GuiEditorDef.h"

I3_CLASS_INSTANCE( i3GuiToolImageElement); //, i3ElementBase);
I3_CLASS_INSTANCE( i3GuiToolRectElement); //, i3ElementBase);

i3GuiToolImageElement::i3GuiToolImageElement()
{
	m_pTexture	= NULL;
	m_hBitmap	= NULL;
}

i3GuiToolImageElement::~i3GuiToolImageElement()
{
	I3_SAFE_RELEASE( m_pTexture);

	_ReleaseBitmap();
}
void i3GuiToolImageElement::SetTexture( i3Texture * pTexture)
{
	I3_REF_CHANGE( m_pTexture, pTexture);

	_ReleaseBitmap();
}

HBITMAP i3GuiToolImageElement::getBitmap()
{
	I3ASSERT( m_pTexture != NULL);

	if( m_hBitmap == NULL)
	{
		_CreateBitmap();

		I3ASSERT( m_hBitmap != NULL);
	}

	return m_hBitmap;
}

void i3GuiToolImageElement::_CreateBitmap()
{
	if( m_pTexture == NULL)	return;

	I3ASSERT( m_hBitmap == NULL);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Bitmap
	BITMAPINFOHEADER fi;
	PVOID pRaster;
	BITMAPINFO ih;
	INT32	nHeight, nWidth;
	//
	UINT16		DestBitCount	= 32;
	nWidth	= m_pTexture->GetWidth();
	nHeight	= m_pTexture->GetHeight();

	::ZeroMemory(&fi,sizeof(BITMAPINFOHEADER));
	fi.biSize		= sizeof(BITMAPINFOHEADER);
	fi.biBitCount	= DestBitCount;

	fi.biHeight		= - nHeight;
	fi.biWidth		= nWidth;
	fi.biPlanes		= 1;
	fi.biCompression= BI_RGB;

	memcpy( &ih.bmiHeader,&fi,sizeof(BITMAPINFOHEADER));
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Create DC
	//For Image
	m_hBitmap = CreateDIBSection(NULL,&ih,DIB_RGB_COLORS,&pRaster,NULL,0);

	if( m_pTexture == NULL )	return ;
	if( pRaster == NULL )	return ;

	//get rect in image
	INT32	nSrcLineSize		= i3Gfx::CalcRasterBufferSize( nWidth, 1, 1, m_pTexture->GetFormat()); //원본이미지의 한라인 길이
	INT32	nBufferSize			= nSrcLineSize * nHeight;

	char	*pSrc = (char*)i3MemAlloc( nBufferSize );

	{
		INT32	i;
		char *  pTemp = pSrc;
		char *	pLocked	= (char*)m_pTexture->Lock(0);

		for( i = 0; i < nHeight; ++i)
		{
			memcpy( pTemp, pLocked, nSrcLineSize );

			pLocked += nSrcLineSize;
			pTemp	+= nSrcLineSize;
		}

		m_pTexture->Unlock(0);
	}

	i3Image_Convert( m_pTexture->GetFormat(), pSrc, nWidth, nHeight, 1, m_pTexture->GetClut(0),
					 I3G_IMAGE_FORMAT_BGRA_8888, (char*)pRaster, 4, NULL);

	/*
	//alpha blend
	UINT8 * pPixel = (UINT8 *) pRaster;
	INT32 i, j;
	UINT32 back;

	for( i = 0 ; i < nHeight; i++)
	{
		for( j = 0; j < nWidth; j++)
		{
			back = ((((i >> 3) ^ (j >> 3)) & 0x01) << 6) + 128;

			pPixel[0] = (UINT8)((((UINT32)pPixel[0] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
			pPixel[1] = (UINT8)((((UINT32)pPixel[1] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
			pPixel[2] = (UINT8)((((UINT32)pPixel[2] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);

			pPixel += 4;
		}
	}*/

	//
	i3MemFree(pSrc);
}

void i3GuiToolImageElement::_ReleaseBitmap()
{
	if( m_hBitmap != NULL)
	{
		DeleteObject( m_hBitmap);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
i3GuiToolRectElement::i3GuiToolRectElement()
{
	m_pElmImage = NULL;
}

i3GuiToolRectElement::~i3GuiToolRectElement()
{
	m_pElmImage = NULL;
}