#include "i3GfxType.h"

#include "i3Gfx.h"

#include "i3ImageFile.h"
#include "i3ImageFilePNG.h"
#include "i3Texture.h"

bool i3ImageFile::LoadPNG( i3Stream * pStream, i3Texture * pTexture)
{
	// PNGFILE struct 생성
	PNGFILE* pPng;

	pPng = (PNGFILE*)i3MemAlloc( sizeof(PNGFILE));
	memset( pPng, 0, sizeof(PNGFILE));

	pTexture->setRestorePoint( pStream);
    
	// PNG 읽기 정보 초기화
	if( i3LibPng::PngInitReadInfo( pStream, pPng) == false)
	{
		I3PRINTF("Could not read PNG info.\n");
		i3MemFree( pPng);
		return false;
	}

	// PixelFormat 확인
	I3G_IMAGE_FORMAT PixelFormat = I3G_IMAGE_FORMAT_NONE;

	switch( pPng->color_type )
	{
	case PNG_COLOR_TYPE_GRAY:	
		//switch( pPng->bit_depth)
		//{
		//case 1:		PixelFormat = I3_IMAGE_FORMAT_G1;	break;
		//case 2:		PixelFormat = I3_IMAGE_FORMAT_G2;	break;
		//case 4:		PixelFormat = I3_IMAGE_FORMAT_G4;	break;
		//case 8:		PixelFormat = I3_IMAGE_FORMAT_G8;	break;
		//case 16:		PixelFormat = I3_IMAGE_FORMAT_G16;	break;
		//}
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		//switch( pPng->bit_depth)
		//{
		//case 8:		PixelFormat = I3_IMAGE_FORMAT_GA8;	break;
		//case 16:		PixelFormat = I3_IMAGE_FORMAT_GA16;	break;
		//}
		break;
	case PNG_COLOR_TYPE_PALETTE:
		switch( pPng->bit_depth)
		{
		//case 1:		PixelFormat = I3_IMAGE_FORMAT_P1;	break;
		//case 2:		PixelFormat = I3_IMAGE_FORMAT_P2;	break;
		case 4:		PixelFormat = I3G_IMAGE_FORMAT_I4;	break;
		case 8:		PixelFormat = I3G_IMAGE_FORMAT_I8;	break;
		}
		break;
	case PNG_COLOR_TYPE_RGB:
		switch( pPng->bit_depth)
		{
		case 8:		PixelFormat = I3G_IMAGE_FORMAT_RGB_888;	break;
		//case 16:	PixelFormat = I3G_IMAGE_FORMAT_C16;		break;
		}
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		switch( pPng->bit_depth)
		{
		case 8:		PixelFormat = I3G_IMAGE_FORMAT_RGBA_8888;	break;
		//case 16:	PixelFormat = I3G_IMAGE_FORMAT_CA16;		break;
		}
		break;
	}

	if( PixelFormat == I3G_IMAGE_FORMAT_NONE)
	{	// 잘못되었거나 I3G_IMAGE_FORMAT이 지원하지 않는 PixelFormat
		I3PRINTLOG(I3LOG_WARN, "Non supported pixel format. color_type = %d, bit_depth = %d", pPng->color_type, pPng->bit_depth);
		i3MemFree( pPng);
		return false;
	}

	// Texture 생성
	if( pTexture->Create( pPng->width, pPng->height, 1, PixelFormat, 0) == false)
	{
		I3PRINTF("Could not create Texture object.");
		i3MemFree( pPng);
		return false;
	}

	// Read CLUT Data
	if( pPng->color_type == PNG_COLOR_TYPE_PALETTE)
	{
		i3Clut * pClut;

		pClut = i3LibPng::ReadCLUT(pPng);
		if( pClut == nullptr)
		{
			I3_MUST_RELEASE( pTexture);

			I3PRINTF("Could not create CLUT object.\n");
			i3MemFree( pPng);
			return false;
		}

		pTexture->SetClut( 0, pClut);

		I3_MUST_RELEASE( pClut);
	}	

	char* pBuf = nullptr;
	if( pTexture->GetPersistFormat() != pTexture->GetFormat())
	{
		UINT32 ImageSz;
		ImageSz = i3Gfx::CalcRasterBufferSize( pTexture->GetWidth(), pTexture->GetHeight(), 1, pTexture->GetPersistFormat());
		pBuf = (char*) i3MemAlloc( ImageSz);
	}
	else
	{
		pBuf = pTexture->Lock( 0, 0);
	}

	pPng->pImageBuf = (unsigned char*)pBuf;


	// PNG image data 읽기
	i3LibPng::PngReadImage(pPng);


	if( pTexture->GetPersistFormat() != pTexture->GetFormat())
	{
		// pBuf는 내부에서 계속 보관하게 되므로, free 하지 않습니다.
		pTexture->SetPersistData( 0, (INT8 *) pBuf);
		pTexture->MakeRuntimeForm();
	}
	else
	{
		pTexture->Unlock( 0);
	}
	
	// PNG 읽기 종료
	i3LibPng::PngFreeReadInfo( pPng);
	i3MemFree( pPng);

	return true;
}

bool i3ImageFile::SavePNG( i3Stream * pStream, i3Texture * pTexture)
{
	// PNGFILE 할당
	PNGFILE* pPng;

	pPng = (PNGFILE*)i3MemAlloc( sizeof(PNGFILE));
	memset( pPng, 0, sizeof(PNGFILE));

	// PNG 쓰기 정보 초기화
	if( i3LibPng::PngInitWriteInfo( pStream, pPng) == false)
	{
		I3PRINTF("Could not init PNG info.\n");
		i3MemFree( pPng);
		return false;
	}

	// PNG 정보 쓰기
	{
		pPng->width = pTexture->GetWidth();
		pPng->height = pTexture->GetHeight();
		
		switch( pTexture->GetPersistFormat() & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB))
		{
		case I3G_IMAGE_FORMAT_I4:
			pPng->color_type = PNG_COLOR_TYPE_PALETTE;
			pPng->bit_depth = 4;			
			break;
		case I3G_IMAGE_FORMAT_I8:
			pPng->color_type = PNG_COLOR_TYPE_PALETTE;
			pPng->bit_depth = 8;
			break;			
		case I3G_IMAGE_FORMAT_RGB_888:
			pPng->color_type = PNG_COLOR_TYPE_RGB;
			pPng->bit_depth = 8;
			break;
		case I3G_IMAGE_FORMAT_BGR_888:
			pPng->color_type = PNG_COLOR_TYPE_RGB;
			pPng->bit_depth = 8;
			png_write_png( pPng->pStruct, pPng->pInfo, PNG_TRANSFORM_BGR, NULL);
			break;
		case I3G_IMAGE_FORMAT_RGBA_8888:
			pPng->color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			pPng->bit_depth = 8;
			break;
		case I3G_IMAGE_FORMAT_BGRA_8888:
			pPng->color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			pPng->bit_depth = 8;
			png_write_png( pPng->pStruct, pPng->pInfo, PNG_TRANSFORM_BGR, NULL);
			break;
		default :
			I3PRINTLOG(I3LOG_WARN, "Non supported pixel format. PersistFormat = 0x%x\n", pTexture->GetPersistFormat() & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB));
			i3MemFree( pPng);
			return false;
		}
	
		if( pPng->color_type == PNG_COLOR_TYPE_PALETTE)
		{
			i3LibPng::WriteCLUT(pPng, pTexture->GetClut(0));
		}
		
		// PNG info 쓰기
		i3LibPng::PngWriteInfo(pPng);
	}

	char* pBuf;
	if( pTexture->GetPersistFormat() != pTexture->GetFormat())
	{
		UINT32 ImageSz;
		ImageSz = i3Gfx::CalcRasterBufferSize( pTexture->GetWidth(), pTexture->GetHeight(), 1, pTexture->GetPersistFormat());
		pBuf = (char*) i3MemAlloc( ImageSz);

		i3Image_Convert( pTexture->GetPersistFormat(), (char*)pTexture->GetPersistData(0), pTexture->GetWidth(), pTexture->GetHeight(), 1, pTexture->GetClut(0),
			pTexture->GetPersistFormat(), pBuf, 1, nullptr);
	}
	else
	{
		pBuf = pTexture->Lock( 0, 0);
	}

	pPng->pImageBuf = (unsigned char*)pBuf;

	// PNG image 쓰기
	i3LibPng::PngWriteImage(pPng);
	
	if( pTexture->GetPersistFormat() != pTexture->GetFormat())
	{
		I3MEM_SAFE_FREE( pBuf);
	}
	else
	{
		pTexture->Unlock( 0);
	}
	
	// PNG 쓰기 종료
	i3LibPng::PngFreeWriteInfo( pPng);
	i3MemFree( pPng);

	return true;
}

#define PNG_SIGNATURE_BYTE	(8)

bool i3LibPng::PngInitReadInfo(i3Stream* pStream, PNGFILE* pPng)
{
	// PNG 파일 확인
	{
		png_byte pSignature[PNG_SIGNATURE_BYTE] = {0,};
	
		if( pStream->Read( pSignature, PNG_SIGNATURE_BYTE) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN,  "Could not read a signature data");
			return false;
		}

		if(!png_check_sig(pSignature, PNG_SIGNATURE_BYTE))
		{			
			I3PRINTLOG(I3LOG_WARN, "Not PNG file.");
			return false;
		}
	}

	// PNG struct 생성
	{
		png_structp pStruct = nullptr;
		png_infop	pInfo = nullptr;

		pStruct = png_create_read_struct_2( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL, NULL, _MemAlloc, _MemFree);
		if( !pStruct)
		{
			I3PRINTF("Could not create png struct.\n");
			return false;
		}

		pInfo = png_create_info_struct( pStruct);
		if( !pInfo)
		{
			png_destroy_read_struct( &pStruct, nullptr, nullptr);
			
			I3PRINTF("Could not create png info.\n");
			return false;
		}

		pPng->pStruct = pStruct;
		pPng->pInfo = pInfo;
	}

	// PNG custom I/O 설정
	png_set_read_fn( pPng->pStruct, pStream, _ReadData);

	// PNG 파일 확인하려 읽어들였던 bytes 통보
	png_set_sig_bytes( pPng->pStruct, PNG_SIGNATURE_BYTE);

	// PNG 정보 읽기
	png_read_info( pPng->pStruct, pPng->pInfo);

	// PNGFILE에 기본 정보 기록
    pPng->width = pPng->pInfo->width;
	pPng->height = pPng->pInfo->height;
	pPng->bit_depth = pPng->pInfo->bit_depth;
	pPng->color_type = pPng->pInfo->color_type;
	pPng->rowbytes = pPng->pInfo->rowbytes;
	pPng->pPalette = pPng->pInfo->palette;
	pPng->num_palette = pPng->pInfo->num_palette;
	pPng->pTrans = pPng->pInfo->trans;
	pPng->num_trans = pPng->pInfo->num_trans;

	return true;
}

/*
bool i3LibPng::PngReadInfo(PNGFILE* pPng)
{
	// PNG 정보 읽기
	png_read_info( pPng->pStruct, pPng->pInfo);

	// PNGFILE에 기본 정보 기록
    pPng->width = pPng->pInfo->width;
	pPng->height = pPng->pInfo->height;
	pPng->bit_depth = pPng->pInfo->bit_depth;
	pPng->color_type = pPng->pInfo->color_type;
	pPng->rowbytes = pPng->pInfo->rowbytes;
	pPng->pPalette = pPng->pInfo->palette;
	pPng->num_palette = pPng->pInfo->num_palette;
	pPng->pTrans = pPng->pInfo->trans;
	pPng->num_trans = pPng->pInfo->num_trans;
	
	return true;
}
*/

bool i3LibPng::PngReadImage(PNGFILE* pPng)
{	
	// png_read_image()는 image data를 각 row의 시작 주소로 읽는다

	png_bytepp ppRowPointers = (png_bytepp)i3MemAlloc( pPng->height * sizeof(png_bytep));

	for( int i = 0; i < pPng->height; i++)
	{
		ppRowPointers[i] = pPng->pImageBuf + i * pPng->rowbytes;
	}

	png_read_image( pPng->pStruct, ppRowPointers);
	
	i3MemFree( ppRowPointers);
	
	return true;
}

void i3LibPng::PngFreeReadInfo(PNGFILE* pPng)
{
	png_read_end( pPng->pStruct, pPng->pInfo);
	png_destroy_read_struct( &pPng->pStruct, &pPng->pInfo, nullptr);
}

bool i3LibPng::PngInitWriteInfo(i3Stream* pStream, PNGFILE* pPng)
{
	// PNG stucture 생성
	{
		png_structp pStruct = nullptr;
		png_infop	pInfo = nullptr;

		pStruct = png_create_write_struct_2( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr, nullptr, _MemAlloc, _MemFree);
		if( !pStruct)
		{
			I3PRINTF("Could not create png struct.\n");
			return false;
		}
		
		pInfo = png_create_info_struct( pStruct);
		if( !pInfo)
		{
			png_destroy_write_struct( &pStruct, nullptr);

			I3PRINTF("Could not create png info.\n");
			return false;
		}

		pPng->pStruct = pStruct;
		pPng->pInfo = pInfo;
	}

	// PNG custom I/O 설정
	png_set_write_fn( pPng->pStruct, pStream, _WriteData, _FlushData);

	return true;
}

bool i3LibPng::PngWriteInfo(PNGFILE* pPng)
{
	// HDR header 설정
	png_set_IHDR( pPng->pStruct, pPng->pInfo,
		pPng->width,
		pPng->height,
		pPng->bit_depth,
		pPng->color_type,
		PNG_INTERLACE_NONE, 
		PNG_COMPRESSION_TYPE_DEFAULT, 
		PNG_FILTER_TYPE_DEFAULT);

	// PNG 정보 쓰기
	png_write_info( pPng->pStruct, pPng->pInfo);

	pPng->rowbytes = pPng->pInfo->rowbytes;

	return true;
}

bool i3LibPng::PngWriteImage(PNGFILE* pPng)
{
	// png_write_image()는 image data를 각 row의 시작 주소로 쓴다

	png_bytepp ppRowPointers = (png_bytepp)i3MemAlloc( pPng->height * sizeof(png_bytep));

	for( INT32 i = 0; i < pPng->height; i++)
	{
		ppRowPointers[i] = pPng->pImageBuf + i * (pPng->rowbytes);
	}

	png_write_image( pPng->pStruct, ppRowPointers);

	i3MemFree( ppRowPointers);

	return true;
}

void i3LibPng::PngFreeWriteInfo(PNGFILE* pPng)
{
	png_write_end( pPng->pStruct, pPng->pInfo);
	png_destroy_write_struct( &pPng->pStruct, &pPng->pInfo);
}

i3Clut* i3LibPng::ReadCLUT(PNGFILE* pPng)
{
	i3Clut * pClut;
	pClut = i3Clut::new_object();
	I3ASSERT( pClut);

	bool Rv;
	Rv = pClut->Create( pPng->num_palette, 32, 8);
	if( Rv == false)
	{		
		I3_MUST_RELEASE( pClut);
		return nullptr;
	}

	char* pBuf;
	pBuf = (char*) i3MemAlloc( pPng->num_palette * 4);
	I3ASSERT( pBuf);

	for( INT32 i = 0; i < pPng->num_palette; i++)
	{
		pBuf[i*4 + 0] = pPng->pPalette[i].red;
		pBuf[i*4 + 1] = pPng->pPalette[i].green;
		pBuf[i*4 + 2] = pPng->pPalette[i].blue;
		pBuf[i*4 + 3] = pPng->num_trans ? pPng->pTrans[i] : 255;	// palette-alpha가 없으면 255
	}

	i3Image_Convert( I3G_IMAGE_FORMAT_RGBA_8888, pBuf, pClut->GetColorCount(), 1,	1, nullptr,
								I3G_IMAGE_FORMAT_RGBA_8888, pClut->GetData(), 1, nullptr);

	i3MemFree( pBuf);

	return pClut;
}

bool i3LibPng::WriteCLUT(PNGFILE* pPng, i3Clut* pClut)
{
	INT32 num_colors = pClut->GetColorCount();
	
	png_colorp pPalette;
	pPalette = (png_colorp)i3MemAlloc( num_colors * sizeof(png_color));
	png_bytep pTrans;
	pTrans = (png_bytep)i3MemAlloc( num_colors);

	char* pBuf = pClut->GetData();
	for( INT32 i = 0; i < num_colors; i++)
	{
		pPalette[i].red		= pBuf[i*4 + 0];
		pPalette[i].green	= pBuf[i*4 + 1];
		pPalette[i].blue	= pBuf[i*4 + 2];
		pTrans[i]			= pBuf[i*4 + 3];
	}

	png_set_PLTE( pPng->pStruct, pPng->pInfo, pPalette, num_colors);
	png_set_tRNS( pPng->pStruct, pPng->pInfo, pTrans, num_colors, NULL);

	pPng->pPalette = pPng->pInfo->palette;
	pPng->num_palette = pPng->pInfo->num_palette;
	pPng->pTrans = pPng->pInfo->trans;
	pPng->num_trans = pPng->pInfo->num_trans;

	i3MemFree( pPalette);
	i3MemFree( pTrans);

	return true;
}

void i3LibPng::_ReadData(png_structp pPngStruct, png_bytep pBuf, png_size_t Length)
{
	i3Stream* pStream = (i3Stream*) png_get_io_ptr(pPngStruct);
	if( pStream->Read( pBuf, Length) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Read fail PNG");
	}
}

void i3LibPng::_WriteData(png_structp pPngStruct, png_bytep pBuf, png_size_t Length)
{
	i3Stream* pStream = (i3Stream*) png_get_io_ptr(pPngStruct);
	if( pStream->Write( pBuf, Length) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Write fail PNG");
	}
}

void i3LibPng::_FlushData(png_structp pPngStruct)
{
	//i3FileStream* pStream = (i3FileStream*) png_get_io_ptr(pPngStruct);
	//pStream->Flush();
}

png_voidp i3LibPng::_MemAlloc(png_structp pPngStruct, png_size_t size)
{
	// 할당 실패시 nullptr을 리턴해야한다
    // libpng는 nullptr을 받으면 등록된 에러핸들러 함수를 호출한다

	return i3MemAlloc(size);
}

void i3LibPng::_MemFree(png_structp pPngStruct, png_voidp ptr)
{
	i3MemFree(ptr);
}
