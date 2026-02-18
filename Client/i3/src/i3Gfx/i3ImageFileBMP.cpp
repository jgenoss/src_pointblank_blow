#include "i3GfxType.h"

#include "i3ImageFile.h"
#include "i3ImageFileBMP.h"
#include "i3ImageUtil.h"
#include "i3Texture.h"
#include "i3TextureMem.h"
#include "i3GfxUtil.h"

#if defined( I3G_DX)
#include "i3TextureDX.h"
#elif defined( I3G_XBOX)
#include "i3TextureXbox.h"
#elif defined( I3G_PSP)
#include "i3TexturePSP.h" 
#elif defined( I3G_OGLES)
#include "i3TextureOGLES.h" 
#endif

bool i3ImageFile::LoadBMP( i3Stream * pStream, i3Texture * pTex)
{
	BITMAPFILEHEADER	FileHeader;
	BITMAPINFOHEADER	InfoHeader;
	UINT32 Rc; 
	i3Clut *	pClut = nullptr;
	INT32 i;
	I3G_IMAGE_FORMAT PixelFormat;
	bool bResult = false;
	char * pLineBuf = nullptr, *pDest;
	INT32	LineSz, RealSz , absHeight; //, Width;	//, Height;
	char *		pLockBuf;	
	bool	bFlip;

	I3ASSERT( pTex != nullptr);

	pTex->setRestorePoint( pStream);

	#if defined( I3G_PSP) || defined( I3G_OGLES)
	Rc = pStream->Read( &FileHeader, 14);
	#else
	Rc = pStream->Read( &FileHeader, sizeof(BITMAPFILEHEADER));
	#endif
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ImageFile::LoadBMP()", "Could not read the file header.");
		goto ExitPart;
	}

	Rc = pStream->Read( &InfoHeader, sizeof(BITMAPINFOHEADER));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ImageFile::LoadBMP()", "Could not read the Bitmap Information.");
		goto ExitPart;
	}

	if( InfoHeader.biSize > sizeof(BITMAPINFOHEADER))
	{
		// 개량 Version의 BMP 파일인 경우 추가 데이터가 있을 수 있기 때문에...
		pStream->SetPosition( InfoHeader.biSize - sizeof(BITMAPINFOHEADER), STREAM_CURRENT);
	}

	if( InfoHeader.biHeight > 0)
		bFlip = true;
	else
		bFlip = false;

	absHeight = ::abs(InfoHeader.biHeight);
	switch( InfoHeader.biBitCount)
	{
		case 4 :	PixelFormat = I3G_IMAGE_FORMAT_I4;			break;
		case 8 :	PixelFormat = I3G_IMAGE_FORMAT_I8;			break;
		case 16 :	PixelFormat = I3G_IMAGE_FORMAT_XRGB_1555;	break;
		case 24 :	PixelFormat = I3G_IMAGE_FORMAT_BGR_888;		break;
		default :	PixelFormat = I3G_IMAGE_FORMAT_BGRA_8888;	break;
	}


	// Clut
	if( InfoHeader.biBitCount <= 8)
	{
		RGBQUAD Quad;
		int ColorCount;
		COLOR col;

		ColorCount = 1 << InfoHeader.biBitCount;

		pClut = i3Clut::new_object();

		pClut->Create( ColorCount, 32, 8);

		if( InfoHeader.biClrUsed == 0)
			InfoHeader.biClrUsed = ColorCount;

		for( i = 0; i < (INT32) InfoHeader.biClrUsed; i++)
		{
			Rc = pStream->Read( &Quad, sizeof(RGBQUAD));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3ImageFile::LoadBMP()", "Could not read CLUT data.");
				goto ExitPart;
			}

			i3Color::Set( &col, (UINT8)Quad.rgbRed, Quad.rgbGreen, Quad.rgbBlue, 255);

			pClut->SetColor( i, &col);
		}
	}

	// Image
	if( pTex->Create( InfoHeader.biWidth, absHeight , 1, PixelFormat, 0) == false)
	{
		i3Log( "i3ImageFile::LoadBMP()", "Could not create texture object.");

		goto ExitPart;
	}

	if( pClut != nullptr)
	{
		pTex->SetClut( 0, pClut);
	}

	//Width = InfoHeader.biWidth;
	//Height = InfoHeader.biHeight;
	INT32 ImageSz = i3Gfx::CalcRasterBufferSize( InfoHeader.biWidth, absHeight, 1, pTex->GetPersistFormat());

	if (pTex->GetPersistFormat() != pTex->GetFormat())
	{
		char * pPersistData = (char*)i3MemAlloc( ImageSz);

		RealSz = (InfoHeader.biWidth * InfoHeader.biBitCount + 7) >> 3;
		LineSz = ((RealSz + 3) >> 2) << 2;

		pLineBuf = (char *) i3MemAlloc( LineSz);

		for( i = 0; i < absHeight ; i++)
		{
			Rc = pStream->Read( pLineBuf, LineSz);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3ImageFile::LoadBMP()", "Could not read image data.");
				i3MemFree( pPersistData);
				goto ExitPart;
			}

			if( bFlip)
				pDest = pPersistData + (RealSz * (pTex->GetHeight() - i - 1));
			else
				pDest = pPersistData + (RealSz * i);

			memcpy(pDest,pLineBuf,RealSz);
		}

		// pPersisData는 내부적으로 보관하게 되는 Buffer이므로
		// free하지 않습니다.
		pTex->SetPersistData( 0, (INT8*)pPersistData);
		pTex->MakeRuntimeForm();
	}
	else
	{
		pLockBuf = pTex->Lock( 0);
		I3ASSERT( pLockBuf != nullptr);

		RealSz = (InfoHeader.biWidth * InfoHeader.biBitCount + 7) >> 3;
		LineSz = ((RealSz + 3) >> 2) << 2;

		//hMem = i3MemAlloc( LineSz);
		pLineBuf = (char *) i3MemAlloc( LineSz);

		for( i = 0; i < absHeight ; i++)
		{
			Rc = pStream->Read( pLineBuf, LineSz);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3ImageFile::LoadBMP()", "Could not read image data.");
				goto ExitPart;
			}

			if( bFlip)
				pDest = pLockBuf + (RealSz * (pTex->GetHeight() - i - 1));
			else
				pDest = pLockBuf + (RealSz * i);

			memcpy(pDest,pLineBuf,RealSz);
		}

		pTex->Unlock( 0);
	}

	bResult = true;

ExitPart:
	I3_SAFE_RELEASE( pClut);

	if( pLineBuf != nullptr)
	{
		i3MemFree( pLineBuf);
	}

	if( bResult == false)
	{
		I3_MUST_RELEASE( pTex);
	}

	return bResult == true;
}

bool i3ImageFile::SaveBMP( i3Stream * pStream, i3Texture * pTexture )
{
	i3Clut * pClut;
	UINT32 Rc, i;
	UINT32 ClutSz, ImageSz;
	UINT32 BMPBitsCount, BMPLineStride;
	UINT32 Width, Height;

	Width = pTexture->GetWidth();
	Height = pTexture->GetHeight();
	pClut = pTexture->GetClut(0);

	if( pClut != nullptr)
		ClutSz = pClut->GetColorCount() * sizeof(RGBQUAD);
	else
		ClutSz = 0;

	BMPBitsCount = pTexture->GetBitsPerPixel(0);

	BMPLineStride = ((BMPBitsCount * Width) + 7) >> 3;		// Bytes 단위로 환산
	BMPLineStride = ((BMPLineStride + 1) >> 1) << 1;		// Line당 2 Bytes align
	ImageSz = BMPLineStride * Height;

	// BITMAPFILEHEADER
	{
		BITMAPFILEHEADER FileHeader;

		memset( &FileHeader, 0, sizeof(BITMAPFILEHEADER));

		memcpy( &FileHeader.bfType, "BM", 2);
		FileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ClutSz + ImageSz;
		FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ClutSz;

		#if defined( I3G_PSP) || defined( I3G_OGLES)
			Rc = pStream->Write( &FileHeader, 14);
		#else
			Rc = pStream->Write( &FileHeader, sizeof(BITMAPFILEHEADER));
		#endif
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ImageFile::SaveBMP()", "Could not write down the bitmap image header.");
			return false;
		}
	}

	// BITMAPINFOHEADER
	{
		BITMAPINFOHEADER Header;

		memset( &Header, 0, sizeof(BITMAPINFOHEADER));

		Header.biSize = sizeof(BITMAPINFOHEADER);
		Header.biWidth = Width;
		Header.biHeight = -(INT32)Height;
		Header.biPlanes = 1;
		Header.biBitCount = (UINT16) BMPBitsCount;
		Header.biCompression = BI_RGB;

		Rc = pStream->Write( &Header, sizeof(BITMAPINFOHEADER));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ImageFile::SaveBMP()", "Could not write down the bitmap informations.");
			return false;
		}
	}

	// Clut (if any)
	if( pClut != nullptr)
	{
		UINT32 ColorCount;
		RGBQUAD Quad;
		COLOR col;

		ColorCount = pClut->GetColorCount();

		for( i = 0; i < ColorCount; i++)
		{
			pClut->GetColor( i, &col);

			Quad.rgbRed			= i3Color::GetR( &col);
			Quad.rgbGreen		= i3Color::GetG( &col);
			Quad.rgbBlue		= i3Color::GetB( &col);
			Quad.rgbReserved	= 0xFF;

			Rc = pStream->Write( &Quad, sizeof(RGBQUAD));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3ImageFile::SaveBMP()", "Could not write down CLUT informations.");
				return false;
			}
		}
	}

	// Image Data
	{
		char * pLineBuf, * pData;
		UINT32 stride;
		UINT32 j;		

		// $ bytes Alignment
		stride = ((pTexture->GetBitsPerPixel(0) * Width) + 7) >> 3;

		//INT32 hMem;
		//hMem = i3MemAlloc( BMPLineStride);
		pLineBuf = (char *) i3MemAlloc( BMPLineStride);

		pData = pTexture->Lock( 0);

		for( i = 0; i < Height; i++)
		{
			if( pTexture->GetBitsPerPixel(0) < 32)
			{
				i3mem::Copy( pLineBuf, pData, BMPLineStride);
			}
			else
			{
				char * pSrc = pData;
				char * pDest = pLineBuf;

				for( j = 0; j < Width; j++)
				{
					// Blue
					pDest[0]	=	pSrc[0];

					// Green
					pDest[1]	=	pSrc[1];

					// Red
					pDest[2]	=	pSrc[2];

					// Dummy
					pDest[3]	=	0;

					pSrc += 4;
					pDest += 4;
				}
			}

			Rc = pStream->Write( pLineBuf, BMPLineStride);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3ImageFile::SaveBMP()", "Could not write down image pixel data.");
				i3MemFree( pLineBuf);
				return false;
			}

			pData += stride;
		}

		pTexture->Unlock( 0);

		i3MemFree( pLineBuf);
	}

	return true;
}
