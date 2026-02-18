#ifndef TEXUTIL_H
#define TEXUTIL_H

#define PC

typedef struct RGBTrippleInt {
   int red;
   int grn;
   int blu;
} RGBTrippleInt;

typedef struct RGBTripple {
   BYTE red;
   BYTE grn;
   BYTE blu;
} RGBTripple;

typedef struct RGBQuad {
   BYTE red;
   BYTE grn;
   BYTE blu;
   BYTE alpha;
} RGBQuad;

typedef unsigned short RGBTripple15;
typedef unsigned short RGBTripple16;
typedef unsigned short RGBTripple1555;
typedef unsigned short RGBQuad4444;
typedef	unsigned char  RGBIndexed;

typedef struct TgaHeader {
   BYTE idLength;
   BYTE colorMapType;
   BYTE imageType;
   short colorMapIndex;
   short colorMapLength;
   BYTE colorMapEntrySize;
   short xOrigin;
   short yOrigin;
   short width;
   short depth;
   BYTE bits;
   BYTE imageDescriptor;
} TgaHeader;

#define UNCOMPRESSED 2
#define RUNLENGTH 10

#define SWAP_RED_AND_BLU(p) { BYTE _tmp; _tmp = (p)->red; (p)->red = (p)->blu; (p)->blu = _tmp;}

typedef struct TgaFile {
   TgaHeader header;
   union {
      RGBTripple *bits24;
      RGBTripple15 *bits16;
      RGBQuad *bits32;
   } image;
   size_t allocatedSize;
   char fileName[MAX_PATH];
   void *TempBuffer;
   size_t TempBufferSize;
} TgaFile;

// Return Value : Success = 1, Fail = 0
int ExportTgaFile (
   TgaFile *tgaFile,
   int pitch,
   int grnMask,          // only relevant for 15 or 16 bit images
   BYTE imageDescriptor
);

// Return Value : Success = 1, Fail = 0
int ExportImage24 (
   RGBTripple *image,
   int width, 
   int depth,
   int pitch,
   const char *filename,
   BYTE imageDescriptor	// 0x10( 상하반전 )
);

// Return Value : Success = 1, Fail = 0
int ExportImage32 (
   RGBQuad *image,
   int width, 
   int depth,
   int pitch,
   const char *filename,
   BYTE imageDescriptor // 0x10( 상하반전 )
);


#endif //TEXUTIL_H