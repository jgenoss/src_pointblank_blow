#include "i3GfxType.h"
#include "i3GfxDefine.h"
#include "i3ImageFile.h"
#include "i3ImageFileDDS.h"
#include "i3GfxUtil.h"
#include "i3ImageUtil.h"
#include "i3Texture.h"
#include "i3TextureCube.h"

static INT32 _CalcTotalBits( UINT32 val)
{
	INT32 count = 0, mask = 1, i;

	for( i = 0; i < 32; i++)
	{
		if( val & mask)
		{
			count++;
		}

		mask = mask << 1;
	}

	return count;
}

static I3G_IMAGE_FORMAT _Findi3ImageFormat( UINT32 RMask, UINT32 GMask, UINT32 BMask, UINT32 AMask)
{
	INT32 TotalBits, GBits;

	TotalBits = _CalcTotalBits( RMask | GMask | BMask | AMask);

	switch( TotalBits)
	{
		case 8 :	return I3G_IMAGE_FORMAT_I8;

		case 16 :
			switch( _CalcTotalBits( AMask))
			{
				case 0 :
					GBits = _CalcTotalBits( GMask);

					switch( GBits)
					{
						case 4 :			// 4444
							return I3G_IMAGE_FORMAT_XRGB_4444;

						case 5 :			// x555
							return I3G_IMAGE_FORMAT_XRGB_1555;

						case 6 :			// 565
							return I3G_IMAGE_FORMAT_RGB_565;
					}
					break;

				case 1 :
					// 1555
					return I3G_IMAGE_FORMAT_ARGB_1555;
			
				case 4 :
					// 4444
					return I3G_IMAGE_FORMAT_ARGB_4444;
			}
			break;

		case 24 :
			if( RMask == 0x00FF0000)
			{
				// BGR
				return I3G_IMAGE_FORMAT_BGR_888;
			}
			else
			{
				// RGB
				return I3G_IMAGE_FORMAT_RGB_888;
			}

		case 32 :
			if( RMask == 0x00FF0000)
			{
				if( AMask != 0)
				{
					// BGRA
					return I3G_IMAGE_FORMAT_BGRA_8888;
				}
				else
				{
					// BGRX
					return I3G_IMAGE_FORMAT_BGRX_8888;
				}
			}
			else
			{
				if( AMask != 0)
				{
					// RGBA
					return I3G_IMAGE_FORMAT_RGBA_8888;
				}
				else
				{
					// RGBX
					return I3G_IMAGE_FORMAT_RGBX_8888;
				}
			}
	}

	return I3G_IMAGE_FORMAT_NONE;
}

static UINT32 _LoadRasterData( i3Stream * pStream, char * pSrc, char * pDest, 
								UINT32 Width, UINT32 Height, 
								I3G_IMAGE_FORMAT SrcFormat, I3G_IMAGE_FORMAT DestFormat)
{
	UINT32 i, bits, Rc, Result = 0;
	UINT32 SrcLineSz, DestLineSz;

	// DDS 파일에서 각 MIPMAP Level Image의 Scan Line은 4 bytes align되어 있다.
	SrcLineSz = i3Gfx::CalcRasterBufferSize( Width, 1, 4, SrcFormat);	// DWORD align

	DestLineSz = i3Gfx::CalcRasterBufferSize( Width, 1, 1, DestFormat);

	bits = i3Gfx::GetImageFormatBitCount( SrcFormat);

	for( i = 0; i < Height; i++)
	{
		Rc = pStream->Read( pSrc, SrcLineSz);
		if( Rc == STREAM_ERR)
		{
			i3Log( "LoadUncomressedTexture()", "Could not read pixel data.");
			return STREAM_ERR;
		}
		Result += Rc;

		switch( bits)
		{
			/*
			case 4 :
				switch( DestFormat)
				{
					case I3G_IMAGE_FORMAT_I4 :			i3Image_Convert4to4( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_I8 :			i3Image_Convert4to8( pSrc, Width, 1, 4, pClut, pDest, 1, pClut);	break;
					case I3G_IMAGE_FORMAT_RGB_565 :		i3Image_Convert4to16_565( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_XRGB_1555 :	i3Image_Convert4to16_X555( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_ARGB_1555 :	i3Image_Convert4to16_1555( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGB_888 :		i3Image_Convert4to24_RGB( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGR_888 :		i3Image_Convert4to24_BGR( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGBX_8888 :	i3Image_Convert4to32_RGBX( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRX_8888 :	i3Image_Convert4to32_BGRX( pSrc, Width, 1, 4, pClut, pDest,	1);	break;
					case I3G_IMAGE_FORMAT_RGBA_8888 :	i3Image_Convert4to32_RGBA( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRA_8888 :	i3Image_Convert4to32_BGRA( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					default :
						I3TRACE( "LoadBMP() - Invalid Texture Format : %d\n", pTex->GetFormat());
						break;
				}
				break;

			case 8 :
				switch( pTex->GetFormat())
				{
					case I3G_IMAGE_FORMAT_I8 :			i3Image_Convert8to8( pSrc, Width, 1, 4, pClut, pDest, 1, pClut);	break;
					case I3G_IMAGE_FORMAT_RGB_565 :		i3Image_Convert8to16_565( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_XRGB_1555 :	i3Image_Convert8to16_X555( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_ARGB_1555 :	i3Image_Convert8to16_1555( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGB_888 :		i3Image_Convert8to24_RGB( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGR_888 :		i3Image_Convert8to24_BGR( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGBX_8888 :	i3Image_Convert8to32_RGBX( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRX_8888 :	i3Image_Convert8to32_BGRX( pSrc, Width, 1, 4, pClut, pDest,	1);	break;
					case I3G_IMAGE_FORMAT_RGBA_8888 :	i3Image_Convert8to32_RGBA( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRA_8888 :	i3Image_Convert8to32_BGRA( pSrc, Width, 1, 4, pClut, pDest, 1);	break;
					default :
						I3TRACE( "LoadBMP() - Invalid Texture Format : %d\n", pTex->GetFormat());
						break;
				}
				break;
				*/

			case 16 :
				switch( DestFormat)
				{
					case I3G_IMAGE_FORMAT_RGB_565 :		i3Image_Convert16_X555_to_16_565( (UINT16 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_XRGB_1555 :	i3Image_Convert16_X555_to_16_X555( (UINT16 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_ARGB_1555 :	i3Image_Convert16_X555_to_16_1555( (UINT16 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGB_888 :		i3Image_Convert16_X555_to_24_RGB( (UINT16 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGR_888 :		i3Image_Convert16_X555_to_24_BGR( (UINT16 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGBX_8888 :	i3Image_Convert16_X555_to_32_RGBA( (UINT16 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRX_8888 :	i3Image_Convert16_X555_to_32_BGRA( (UINT16 *)pSrc, Width, 1, 4, (UINT8 *)pDest,	1);	break;
					case I3G_IMAGE_FORMAT_RGBA_8888 :	i3Image_Convert16_X555_to_32_RGBA( (UINT16 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRA_8888 :	i3Image_Convert16_X555_to_32_BGRA( (UINT16 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					default :
						I3TRACE( "LoadBMP() - Invalid Texture Format : %d\n", DestFormat);
						break;
				}
				break;

			case 24 :
				switch( DestFormat)
				{
					case I3G_IMAGE_FORMAT_RGB_565 :		i3Image_Convert24_RGB_to_16_RGB_565( (UINT8 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_XRGB_1555 :	i3Image_Convert24_RGB_to_16_ARGB_X555( (UINT8 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_ARGB_1555 :	i3Image_Convert24_RGB_to_16_ARGB_1555( (UINT8 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGB_888 :		i3Image_Convert24_RGB_to_24_RGB( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGR_888 :		i3Image_Convert24_RGB_to_24_BGR( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGBX_8888 :	i3Image_Convert24_RGB_to_32_BGRA( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRX_8888 :	i3Image_Convert24_RGB_to_32_RGBA( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest,	1);	break;
					case I3G_IMAGE_FORMAT_RGBA_8888 :	i3Image_Convert24_RGB_to_32_BGRA( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRA_8888 :	i3Image_Convert24_RGB_to_32_RGBA( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					default :
						I3TRACE( "_LoadRasterData() - Invalid Texture Format : %d\n", DestFormat);
						break;
				}
				break;

			default :			// 32 bits
				switch( DestFormat)
				{
					case I3G_IMAGE_FORMAT_RGB_565 :		i3Image_Convert32_RGBA_to_16_RGB_565( (UINT8 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_XRGB_1555 :	i3Image_Convert32_RGBX_to_16_ARGB_1555( (UINT8 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_ARGB_1555 :	i3Image_Convert32_RGBX_to_16_ARGB_1555( (UINT8 *)pSrc, Width, 1, 4, (UINT16 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGB_888 :		i3Image_Convert32_RGBA_to_24_RGB( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGR_888 :		i3Image_Convert32_RGBA_to_24_BGR( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_RGBX_8888 :	i3Image_Convert32_RGBA_to_32_RGBA( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRX_8888 :	i3Image_Convert32_RGBX_to_32_BGRA( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest,	1);	break;
					case I3G_IMAGE_FORMAT_RGBA_8888 :	i3Image_Convert32_RGBX_to_32_RGBA( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					case I3G_IMAGE_FORMAT_BGRA_8888 :	i3Image_Convert32_RGBX_to_32_BGRA( (UINT8 *)pSrc, Width, 1, 4, (UINT8 *)pDest, 1);	break;
					default :
						I3TRACE( "_LoadRasterData() - Invalid Texture Format : %d\n", DestFormat);
						break;
				}
				break;
		}

		pDest += DestLineSz;
	}

	return Result;
}

static UINT32 LoadUncompressedTexture( i3Texture * pTexture, i3Stream * pStream, I3G_IMAGE_FORMAT SrcFormat)
{
	char * pSrc = nullptr, * pDest;
	INT32 mipmap;
	UINT32 SrcLineSz, Width, Height;
	UINT32 Rc = STREAM_ERR, Result = 0;	

	Width = pTexture->GetWidth();
	Height = pTexture->GetHeight();

	SrcLineSz = i3Gfx::CalcRasterBufferSize( Width, 1, 4, SrcFormat);	// DWORD align

	//INT32 hMem;
	//hMem = i3MemAlloc( SrcLineSz);
	pSrc = (char *) i3MemAlloc( SrcLineSz);
	I3ASSERT( pSrc != nullptr);

	for( mipmap = 0; mipmap < (INT32) pTexture->GetLevelCount(); mipmap++)
	{
		pDest = pTexture->Lock( mipmap);
		I3ASSERT( pDest != nullptr);

		Rc = _LoadRasterData( pStream, pSrc, pDest, Width, Height, SrcFormat, pTexture->GetFormat());

		pTexture->Unlock( mipmap);

		if( Rc == STREAM_ERR)
		{
			goto ExitPart;
		}
		Result += Rc;

		Width = Width >> 1;
		Height = Height >> 1;
	}

ExitPart:
	if( pSrc != nullptr)
	{
		i3MemFree( pSrc);
	}

	if( Rc == STREAM_ERR)
		return STREAM_ERR;

	return Result;
}

static UINT32 _LoadDXTData( i3Stream * pStream, char * pDest, UINT32 Width, UINT32 Height, I3G_IMAGE_FORMAT Format)
{
	UINT32 Rc, Result = 0;
	UINT32 LineSize;
	
	// Compressed Texture의 각 MIPMAP Level Image의 크기는 1/4로 줄어든다.
	// 그러나, 아무리 작은 크기라 하더라도 8 bytes 보다 작아지지 않는다.
	// Compressed Tesxture의 데이터 크기를 구하는 식은 아래와 같다.
	//
	//  max(1, Width ÷ 4) x max(1, Height ÷ 4) x 8(DXT1) or 16(DXT2-5)
	//
	LineSize = i3Gfx::CalcRasterBufferSize( Width, Height, 1, Format);

	Rc = pStream->Read( pDest, LineSize);
	if( Rc == STREAM_ERR)
	{
		i3Log( "_LoadDXTData()", "Could not DXT compressed Texture line data.");
		return STREAM_ERR;
	}
	Result += Rc;	

	return Result;
}

static UINT32 LoadCompressedTexture( UINT32 Width, UINT32 Height, i3Texture * pTexture, i3Stream * pStream)
{
	UINT32 Rc, Result = 0;
	UINT32 i;
	char * pDest;

	for( i = 0; i < pTexture->GetLevelCount(); i++)
	{
		pDest = pTexture->Lock( i);
		I3ASSERT( pDest != nullptr);

		Rc = _LoadDXTData( pStream, pDest, Width, Height, pTexture->GetFormat());

		pTexture->Unlock( i);

		if( Rc == STREAM_ERR)
		{
			i3Log( "LoadCompressedTexture()", "Could not read compressed image data.");
			return STREAM_ERR;
		}
		Result += Rc;

		Width = Width >> 1;
		Height = Height >> 1;
	}

	return Result;
}

#if defined( I3G_DX) || defined( I3G_XBOX)
static I3G_CUBE_TEXTURE_FACE s_CubeFaceTable[6] =
{
	I3G_CUBE_TEXTURE_FACE_PX,
	I3G_CUBE_TEXTURE_FACE_NX,
	I3G_CUBE_TEXTURE_FACE_PY,
	I3G_CUBE_TEXTURE_FACE_NY,
	I3G_CUBE_TEXTURE_FACE_PZ,
	I3G_CUBE_TEXTURE_FACE_NZ
};

static UINT32 LoadCubeTexture( i3TextureCube * pTex, i3Stream * pStream, I3G_IMAGE_FORMAT SrcFormat)
{
	UINT32 face, mipmap;
	char * pSrc = nullptr, * pDest;
	UINT32 Width, Height;
	UINT32 Rc = STREAM_ERR, Result = 0;
	//INT32 hMem;

	Width = pTex->GetWidth();
	Height = pTex->GetHeight();

	if( (pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED) == 0)
	{
		// Uncompressed Format

		UINT32 Sz = i3Gfx::CalcRasterBufferSize( Width, 1, 4, SrcFormat);

		pSrc = (char *) i3MemAlloc( Sz );
	}

	for( face = 0; face < 6; face++)
	{
		Width = pTex->GetWidth();
		Height = pTex->GetHeight();

		for( mipmap = 0; mipmap < pTex->GetLevelCount(); mipmap++)
		{
			pDest = pTex->Lock( s_CubeFaceTable[ face], mipmap);

			if( pSrc != nullptr)
			{
				// Uncompressed Texture
				Rc = _LoadRasterData( pStream, pSrc, pDest, Width, Height, SrcFormat, pTex->GetFormat());
			}
			else
			{
				// Compressed Texture
				Rc = _LoadDXTData( pStream, pDest, Width, Height, pTex->GetFormat());
			}

			pTex->Unlock( s_CubeFaceTable[ face], mipmap);

			if( Rc == STREAM_ERR)
			{
				i3Log( "LoadCubeTexture()", "Could not read Uncompressed Cube Texture data.");
				goto ExitPart;
			}
			Result += Rc;

			Width = Width >> 1;
			Height = Height >> 1;
		}
	}
	
ExitPart:
	if( pSrc != nullptr)
	{
		i3MemFree( pSrc);
	}

	if( Rc == STREAM_ERR)
		return STREAM_ERR;

	return Result;
}
#endif

bool i3ImageFile::LoadDDS( i3Stream * pStream, i3Texture * pTexture)
{
	UINT32	Magic, Rc, HeaderFlags, PixelFormatFlags;
	DDSURFACEDESC2 Header;
	INT32 MipMapCount;
	I3G_IMAGE_FORMAT format;
	UINT32 ReadSize;

	m_ReadBytes = 0;

	pTexture->setRestorePoint( pStream);

	// Magic Code
	Rc = pStream->Read( &Magic, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ImageFile::LoadDDS()", "Could not read magic node from DDS file.");
		return false;
	}
	m_ReadBytes += Rc;

	// Header
	Rc = pStream->Read( &Header, sizeof(DDSURFACEDESC2));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ImageFile::LoadDDS()", "Could not read surface description information.");
		return false;
	}
	m_ReadBytes += Rc;

	HeaderFlags = Header.dwFlags;
	PixelFormatFlags = Header.ddpfPixelFormat.dwFlags;

	if( HeaderFlags & DDSD_MIPMAPCOUNT)
		MipMapCount = Header.dwMipMapCount;
	else
		MipMapCount = 1;

	if( PixelFormatFlags & DDPF_FOURCC)
	{
		// Compressed Format
		UINT32 cc = Header.ddpfPixelFormat.dwFourCC;

		switch( cc)
		{
			case FOURCC_DXT1 :		format = I3G_IMAGE_FORMAT_DXT1;	break;
			case FOURCC_DXT2 :		format = I3G_IMAGE_FORMAT_DXT2;	break;
			case FOURCC_DXT3 :		format = I3G_IMAGE_FORMAT_DXT3;	break;
			case FOURCC_DXT4 :		format = I3G_IMAGE_FORMAT_DXT4;	break;
			case FOURCC_DXT5 :		format = I3G_IMAGE_FORMAT_DXT5;	break;
			default :
				i3Log( "i3ImageFileDDS::LoadDDS()", "Unsupported format : %08X\n", cc);
				return false;
		}
	}
	else
	{
		// Uncompressed Format
		if( PixelFormatFlags & DDPF_RGB)
		{
			format = _Findi3ImageFormat(	Header.ddpfPixelFormat.dwRBitMask,
											Header.ddpfPixelFormat.dwGBitMask,
											Header.ddpfPixelFormat.dwBBitMask,
											Header.ddpfPixelFormat.dwRGBAlphaBitMask);

			if( format == I3G_IMAGE_FORMAT_NONE)
			{
				i3Log( "i3ImageFileDDS::LoadDDS()", "Could not find corresponding image format.");
				return false;
			}
		}
		else
		{
			i3Log( "i3ImageFileDDS::LoadDDS()", "Could not recognize pixel format of DDS file.");
			return false;
		}
	}

	if( HeaderFlags & DDSD_LINEARSIZE)
		ReadSize = Header.dwLinearSize;		// Total Data Size
	else
		ReadSize = Header.dwLinearSize * Header.dwHeight;

	if((Header.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP) == 0)
	{
		pTexture->Create( Header.dwWidth, Header.dwHeight, MipMapCount, format, 0);

		if( PixelFormatFlags & DDPF_FOURCC)
		{
			// Compressed Texture (DXT1, DXT2, DXT3, DXT4, DXT5)
			Rc = LoadCompressedTexture( Header.dwWidth, Header.dwHeight, pTexture, pStream);
		}
		else
		{
			// Uncompressed Texture
			Rc = LoadUncompressedTexture( pTexture, pStream, format);
		}
	}
	#if defined( I3G_DX) || defined( I3G_XBOX)
	else
	{
		// Cube Texture
		i3TextureCube * pCubeTex = (i3TextureCube *) pTexture;

		I3ASSERT( i3::kind_of<i3TextureCube*>(pCubeTex));

		pCubeTex->Create( Header.dwWidth, Header.dwHeight, MipMapCount, format, 0);

		Rc = LoadCubeTexture( pCubeTex, pStream, format);
	}
	#endif

	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ImageFileDDS::LoadDDS()", "Could not load image data.");
		return false;
	}

	m_ReadBytes += Rc;

	return true;
}

bool i3ImageFile::SaveDDS( i3Stream * /*pStream*/, i3Texture * /*pTexture*/)
{
	return false;
}

bool i3ImageFile::isCubeDDS( const char * pszPath)
{
	UINT32	Magic, Rc;
	DDSURFACEDESC2 Header;
	i3FileStream strm;

	if( strm.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	// Magic Code
	Rc = strm.Read( &Magic, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not read magic node from %s DDS file.", pszPath);
		return false;
	}

	// Header
	Rc = strm.Read( &Header, sizeof(DDSURFACEDESC2));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not read surface description information. : %s", pszPath);
		return false;
	}

	if((Header.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP) == 0)
		return false;

	return true;
}
