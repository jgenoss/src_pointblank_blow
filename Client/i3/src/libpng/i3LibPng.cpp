#include "i3LibPng.h"

//
//	이하 code는 i3Gfx proj.의 i3ImageFilePNG.cpp 뒷부분으로 옮겼습니다.
//

//#define PNG_SIGNATURE_BYTE	(8)
//
//BOOL i3LibPng::PngInitReadInfo(i3Stream* pStream, PNGFILE* pPng)
//{
//	// PNG 파일 확인
//	{
//		png_byte pSignature[PNG_SIGNATURE_BYTE] = {0,};
//	
//		pStream->Read( pSignature, PNG_SIGNATURE_BYTE);
//		if(!png_check_sig(pSignature, PNG_SIGNATURE_BYTE))
//		{			
//			I3PRINTLOG(I3LOG_WARN, "Not PNG file.\n");
//			return false;
//		}w
//	}
//
//	// PNG struct 생성
//	{
//		png_structp pStruct = nullptr;
//		png_infop	pInfo = nullptr;
//
//		pStruct = png_create_read_struct_2( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr, nullptr, _MemAlloc, _MemFree);
//		if( !pStruct)
//		{
//			I3PRINTF("Could not create png struct.\n");
//			return false;
//		}
//
//		pInfo = png_create_info_struct( pStruct);
//		if( !pInfo)
//		{
//			png_destroy_read_struct( &pStruct, nullptr, nullptr);
//			
//			I3PRINTF("Could not create png info.\n");
//			return false;
//		}
//
//		pPng->pStruct = pStruct;
//		pPng->pInfo = pInfo;
//	}
//
//	// PNG custom I/O 설정
//	png_set_read_fn( pPng->pStruct, pStream, _ReadData);
//
//	// PNG 파일 확인하려 읽어들였던 bytes 통보
//	png_set_sig_bytes( pPng->pStruct, PNG_SIGNATURE_BYTE);
//
//	// PNG 정보 읽기
//	png_read_info( pPng->pStruct, pPng->pInfo);
//
//	// PNGFILE에 기본 정보 기록
//    pPng->width = pPng->pInfo->width;
//	pPng->height = pPng->pInfo->height;
//	pPng->bit_depth = pPng->pInfo->bit_depth;
//	pPng->color_type = pPng->pInfo->color_type;
//	pPng->rowbytes = pPng->pInfo->rowbytes;
//	pPng->pPalette = pPng->pInfo->palette;
//	pPng->num_palette = pPng->pInfo->num_palette;
//	pPng->pTrans = pPng->pInfo->trans;
//	pPng->num_trans = pPng->pInfo->num_trans;
//
//	return TRUE;		
//}
//
///*
//BOOL i3LibPng::PngReadInfo(PNGFILE* pPng)
//{
//	// PNG 정보 읽기
//	png_read_info( pPng->pStruct, pPng->pInfo);
//
//	// PNGFILE에 기본 정보 기록
//    pPng->width = pPng->pInfo->width;
//	pPng->height = pPng->pInfo->height;
//	pPng->bit_depth = pPng->pInfo->bit_depth;
//	pPng->color_type = pPng->pInfo->color_type;
//	pPng->rowbytes = pPng->pInfo->rowbytes;
//	pPng->pPalette = pPng->pInfo->palette;
//	pPng->num_palette = pPng->pInfo->num_palette;
//	pPng->pTrans = pPng->pInfo->trans;
//	pPng->num_trans = pPng->pInfo->num_trans;
//	
//	return TRUE;
//}
//*/
//
//BOOL i3LibPng::PngReadImage(PNGFILE* pPng)
//{	
//	// png_read_image()는 image data를 각 row의 시작 주소로 읽는다
//
//	png_bytepp ppRowPointers = (png_bytepp)i3MemAlloc( pPng->height * sizeof(png_bytep));
//
//	for( int i = 0; i < pPng->height; i++)
//	{
//		ppRowPointers[i] = pPng->pImageBuf + i * pPng->rowbytes;
//	}
//
//	png_read_image( pPng->pStruct, ppRowPointers);
//	
//	i3MemFree( ppRowPointers);
//	
//	return TRUE;	
//}
//
//void i3LibPng::PngFreeReadInfo(PNGFILE* pPng)
//{
//	png_read_end( pPng->pStruct, pPng->pInfo);
//	png_destroy_read_struct( &pPng->pStruct, &pPng->pInfo, nullptr);
//}
//
//BOOL i3LibPng::PngInitWriteInfo(i3Stream* pStream, PNGFILE* pPng)
//{
//	// PNG stucture 생성
//	{
//		png_structp pStruct = nullptr;
//		png_infop	pInfo = nullptr;
//
//		pStruct = png_create_write_struct_2( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr, nullptr, _MemAlloc, _MemFree);
//		if( !pStruct)
//		{
//			I3PRINTF("Could not create png struct.\n");
//			return false;
//		}
//		
//		pInfo = png_create_info_struct( pStruct);
//		if( !pInfo)
//		{
//			png_destroy_write_struct( &pStruct, nullptr);
//
//			I3PRINTF("Could not create png info.\n");
//			return false;
//		}
//
//		pPng->pStruct = pStruct;
//		pPng->pInfo = pInfo;
//	}
//
//	// PNG custom I/O 설정
//	png_set_write_fn( pPng->pStruct, pStream, _WriteData, _FlushData);
//
//	return TRUE;
//}
//
//BOOL i3LibPng::PngWriteInfo(PNGFILE* pPng)
//{
//	// HDR header 설정
//	png_set_IHDR( pPng->pStruct, pPng->pInfo,
//		pPng->width,
//		pPng->height,
//		pPng->bit_depth,
//		pPng->color_type,
//		PNG_INTERLACE_NONE, 
//		PNG_COMPRESSION_TYPE_DEFAULT, 
//		PNG_FILTER_TYPE_DEFAULT);
//
//	// PNG 정보 쓰기
//	png_write_info( pPng->pStruct, pPng->pInfo);
//
//	pPng->rowbytes = pPng->pInfo->rowbytes;
//
//	return TRUE;
//}
//
//BOOL i3LibPng::PngWriteImage(PNGFILE* pPng)
//{
//	// png_write_image()는 image data를 각 row의 시작 주소로 쓴다
//
//	png_bytepp ppRowPointers = (png_bytepp)i3MemAlloc( pPng->height * sizeof(png_bytep));
//
//	for( INT32 i = 0; i < pPng->height; i++)
//	{
//		ppRowPointers[i] = pPng->pImageBuf + i * (pPng->rowbytes);
//	}
//
//	png_write_image( pPng->pStruct, ppRowPointers);
//
//	i3MemFree( ppRowPointers);
//
//	return TRUE;
//}
//
//void i3LibPng::PngFreeWriteInfo(PNGFILE* pPng)
//{
//	png_write_end( pPng->pStruct, pPng->pInfo);
//	png_destroy_write_struct( &pPng->pStruct, &pPng->pInfo);
//}
//
//i3Clut* i3LibPng::ReadCLUT(PNGFILE* pPng)
//{
//	i3Clut * pClut;
//	pClut = i3Clut::new_object();
//	I3ASSERT( pClut);
//
//	BOOL Rv;
//	Rv = pClut->Create( pPng->num_palette, 32, 8);
//	if( Rv == false)
//	{		
//		pClut->Release();
//		return nullptr;
//	}
//
//	char* pBuf;
//	pBuf = (char*) i3MemAlloc( pPng->num_palette * 4);
//	I3ASSERT( pBuf);
//
//	for( INT32 i = 0; i < pPng->num_palette; i++)
//	{
//		pBuf[i*4 + 0] = pPng->pPalette[i].red;
//		pBuf[i*4 + 1] = pPng->pPalette[i].green;
//		pBuf[i*4 + 2] = pPng->pPalette[i].blue;
//		pBuf[i*4 + 3] = pPng->num_trans ? pPng->pTrans[i] : 255;	// palette-alpha가 없으면 255
//	}
//
//	i3Image_Convert( I3G_IMAGE_FORMAT_RGBA_8888, pBuf, pClut->GetColorCount(), 1,	1, nullptr,
//								I3G_IMAGE_FORMAT_RGBA_8888, pClut->GetData(), 1, nullptr);
//
//	i3MemFree( pBuf);
//
//	return pClut;
//}
//
//BOOL i3LibPng::WriteCLUT(PNGFILE* pPng, i3Clut* pClut)
//{
//	INT32 num_colors = pClut->GetColorCount();
//	
//	png_colorp pPalette;
//	pPalette = (png_colorp)i3MemAlloc( num_colors * sizeof(png_color));
//	png_bytep pTrans;
//	pTrans = (png_bytep)i3MemAlloc( num_colors);
//
//	char* pBuf = pClut->GetData();
//	for( INT32 i = 0; i < num_colors; i++)
//	{
//		pPalette[i].red		= pBuf[i*4 + 0];
//		pPalette[i].green	= pBuf[i*4 + 1];
//		pPalette[i].blue	= pBuf[i*4 + 2];
//		pTrans[i]			= pBuf[i*4 + 3];
//	}
//
//	png_set_PLTE( pPng->pStruct, pPng->pInfo, pPalette, num_colors);
//	png_set_tRNS( pPng->pStruct, pPng->pInfo, pTrans, num_colors, nullptr);
//
//	pPng->pPalette = pPng->pInfo->palette;
//	pPng->num_palette = pPng->pInfo->num_palette;
//	pPng->pTrans = pPng->pInfo->trans;
//	pPng->num_trans = pPng->pInfo->num_trans;
//
//	i3MemFree( pPalette);
//	i3MemFree( pTrans);
//
//	return TRUE;
//}
//
//void i3LibPng::_ReadData(png_structp pPngStruct, png_bytep pBuf, png_size_t Length)
//{
//	i3Stream* pStream = (i3Stream*) png_get_io_ptr(pPngStruct);
//	pStream->Read( pBuf, Length);
//}
//
//void i3LibPng::_WriteData(png_structp pPngStruct, png_bytep pBuf, png_size_t Length)
//{
//	i3Stream* pStream = (i3Stream*) png_get_io_ptr(pPngStruct);
//	pStream->Write( pBuf, Length);
//}
//
//void i3LibPng::_FlushData(png_structp pPngStruct)
//{
//	i3FileStream* pStream = (i3FileStream*) png_get_io_ptr(pPngStruct);
//	pStream->Flush();
//}
//
//png_voidp i3LibPng::_MemAlloc(png_structp pPngStruct, png_size_t size)
//{
//	// 할당 실패시 nullptr을 리턴해야한다
//    // libpng는 nullptr을 받으면 등록된 에러핸들러 함수를 호출한다
//
//	return i3MemAlloc(size);
//}
//
//void i3LibPng::_MemFree(png_structp pPngStruct, png_voidp ptr)
//{
//	i3MemFree(ptr);
//}
