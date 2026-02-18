#ifndef _i3ImageFilePNG_h
#define _i3ImageFilePNG_h

#include "i3LibPng.h"

struct PNGFILE
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
} ;

class i3Clut;

namespace i3LibPng
{
	// Read PNG
	bool PngInitReadInfo(i3Stream* pStream, PNGFILE* pPng);
	//I3_EXPORT_LIBPNG bool PngReadInfo(PNGFILE* pPng);
	bool PngReadImage(PNGFILE* pPng);
	void PngFreeReadInfo(PNGFILE* pPng);

	// Write PNG
	bool PngInitWriteInfo(i3Stream* pStream, PNGFILE* pPng);
	bool PngWriteInfo(PNGFILE* pPng);
	bool PngWriteImage(PNGFILE* pPng);
	void PngFreeWriteInfo(PNGFILE* pPng);

	// CLUT
	i3Clut* ReadCLUT(PNGFILE* pPng);
	bool WriteCLUT(PNGFILE* pPng, i3Clut* pClut);

	// Custom I/O callback fn.
	void _ReadData(png_structp pPngStruct, png_bytep pBuf, png_size_t Length);
	void _WriteData(png_structp pPngStruct, png_bytep pBuf, png_size_t Length);
	void _FlushData(png_structp pPngStruct);
	
	// Custom memory allocation
	png_voidp _MemAlloc(png_structp pPngStruct, png_size_t size);
	void _MemFree(png_structp pPngStruct, png_voidp ptr);
};

#endif // _i3ImageFilePNG_h