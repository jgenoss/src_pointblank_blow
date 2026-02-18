#ifndef __I3_IMAGE_FILE_TGA_H
#define __I3_IMAGE_FILE_TGA_H

#include "i3Base.h"
#include "i3Math.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

// change order so we really have RGB or ARGB with little Endian on Intels
#if (defined(I3G_DX) || defined(I3G_XBOX) || defined( I3G_PSP))
	typedef struct PACKED RGBTrippleInt {
		INT32 blu;
		INT32 grn;
		INT32 red;
	} RGBTrippleInt;

	typedef struct PACKED RGBTripple {
		INT8 blu;
		INT8 grn;
		INT8 red;
	} RGBTripple;

	typedef struct PACKED RGBQuad {
		INT8 blu;
		INT8 grn;
		INT8 red;
		INT8 alpha;
	} RGBQuad;
#else
	typedef struct PACKED RGBTrippleInt {
		INT32 red;
		INT32 grn;
		INT32 blu;
	} RGBTrippleInt;

	typedef struct PACKED RGBTripple {
		INT8 red;
		INT8 grn;
		INT8 blu;
	} RGBTripple;

	typedef struct PACKED RGBQuad {
		INT8 red;
		INT8 grn;
		INT8 blu;
		INT8 alpha;
	} RGBQuad;
#endif

typedef UINT16 RGBTripple15;
typedef UINT16 RGBTripple16;
typedef UINT16 RGBTripple1555;
typedef UINT16 RGBQuad4444;

#define UNCOMPRESSED 2
#define RUNLENGTH 10

// TGA Header
typedef struct PACKED _TGAFileHeader
{
	INT8 idLength;
	INT8 colorMapType;
	INT8 imageType;
	INT16 colorMapIndex;
	INT16 colorMapLength;
	INT8 colorMapEntrySize;
	INT16 xOrigin;
	INT16 yOrigin;
	INT16 width;
	INT16 height; 
	INT8 bits;
	INT8 imageDescriptor;
} TGAHEADER;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

// TGA File
typedef struct
{
	TGAHEADER header;
	union
	{
		unsigned char* data;
		RGBTripple *bits24;
		RGBTripple15 *bits16;
		RGBQuad *bits32;
	} image;
	size_t allocatedSize;
	// char fileName[MAX_PATH];
	void *TempBuffer;
	size_t TempBufferSize;
} TGAFILE;

namespace i3Tga
{


	// Read TGA File Header
	BOOL ReadTgaFileHeader( i3Stream* sp, TGAHEADER* pHeader);

	i3Clut *	ReadCLUT( TGAFILE * tgaFile, i3Stream * sp);
	BOOL		WriteCLUT( TGAFILE * tgaFile, i3Stream * sp, i3Clut * pClut);

	// Read 4 bit TGA data
	BOOL Read4BitTgaData ( TGAFILE* tgaFile, i3Stream* sp, i3Texture * pTex);

	// Read 8 bit TGA data
	BOOL Read8BitTgaData ( TGAFILE* tgaFile, i3Stream* sp, i3Texture * pTex);

	// Read 16 bit TGA data
	BOOL Read16BitTgaData ( TGAFILE* tgaFile, i3Stream* sp, i3Texture * pTex);

	// Read 24 bit TGA data
	BOOL Read24BitTgaData ( TGAFILE* tgaFile, i3Stream* sp, i3Texture * pTex);

	// Read 32 bit TGA data
	BOOL Read32BitTgaData (	TGAFILE* tgaFile, i3Stream* sp, i3Texture * pTex);

	INT32 ExportTgaFile (
		TGAFILE *tgaFile,
		INT32 pitch,
		INT32 grnMask,          // only relevant for 15 or 16 bit images
		INT8 imageDescriptor = 0x20
	);

	void ExportImage24 (
		RGBTripple *image,
		INT32 width, 
		INT32 height,
		INT32 pitch,
		char *filename,
		INT8 imageDescriptor = 0x20
	);

	void ExportImage32 (
		RGBQuad *image,
		INT32 width, 
		INT32 height,
		INT32 pitch,
		char *filename,
		INT8 imageDescriptor = 0x20
	);

	void ExportImage16 (
		RGBTripple15 *image,
		INT32 width, 
		INT32 height,
		INT32 pitch,
		INT32 grnMask,
		char *filename,
		INT8 imageDescriptor = 0x20
	);

	void FreeTgaFile (
		TGAFILE *tgafile
	);
};

#endif //__I3_IMAGE_FILE_TGA_H
