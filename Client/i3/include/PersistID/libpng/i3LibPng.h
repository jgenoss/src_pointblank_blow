#ifndef _i3LibPng_h
#define _i3LibPng_h

//#include "i3Base.h"
//#include "i3Gfx.h"
#include "png.h"


//
//	이하 code는 i3Gfx proj.의 i3ImageFilePNG.h로 옮겼습니다.
//


//#if defined( I3_WINDOWS) && defined( I3_DLL)
//	#if defined( I3_LIBPNG_BUILD)
//		#define	I3_EXPORT_LIBPNG		__declspec( dllexport)
//	#else
//		#define	I3_EXPORT_LIBPNG		__declspec( dllimport)
//	#endif
//#else
//	#if !defined( I3_EXPORT_LIBPNG)
//		#define	I3_EXPORT_LIBPNG
//	#endif
//#endif

/*
typedef struct
{
	INT32		width;
	INT32		height;
	INT32		bit_depth;
	INT32		color_type;
	INT32		rowbytes;
	
	png_colorp	pPalette;
	INT32		num_palette;
	png_bytep	pTrans;
	INT32		num_trans;

	png_structp	pStruct;
	png_infop	pInfo;

	unsigned char*		pImageBuf;
} PNGFILE;


class i3Clut;

namespace i3LibPng
{
	// Read PNG
	I3_EXPORT_LIBPNG BOOL PngInitReadInfo(i3Stream* pStream, PNGFILE* pPng);
	//I3_EXPORT_LIBPNG BOOL PngReadInfo(PNGFILE* pPng);
	I3_EXPORT_LIBPNG BOOL PngReadImage(PNGFILE* pPng);
	I3_EXPORT_LIBPNG void PngFreeReadInfo(PNGFILE* pPng);

	// Write PNG
	I3_EXPORT_LIBPNG BOOL PngInitWriteInfo(i3Stream* pStream, PNGFILE* pPng);
	I3_EXPORT_LIBPNG BOOL PngWriteInfo(PNGFILE* pPng);
	I3_EXPORT_LIBPNG BOOL PngWriteImage(PNGFILE* pPng);
	I3_EXPORT_LIBPNG void PngFreeWriteInfo(PNGFILE* pPng);

	// CLUT
	I3_EXPORT_LIBPNG i3Clut* ReadCLUT(PNGFILE* pPng);
	I3_EXPORT_LIBPNG BOOL WriteCLUT(PNGFILE* pPng, i3Clut* pClut);

	// Custom I/O callback fn.
	void _ReadData(png_structp pPngStruct, png_bytep pBuf, png_size_t Length);
	void _WriteData(png_structp pPngStruct, png_bytep pBuf, png_size_t Length);
	void _FlushData(png_structp pPngStruct);
	
	// Custom memory allocation
	png_voidp _MemAlloc(png_structp pPngStruct, png_size_t size);
	void _MemFree(png_structp pPngStruct, png_voidp ptr);
};
*/
#endif //_i3LibPng_h
