#if !defined( __I3_IMAGE_UTIL_H)
#define __I3_IMAGE_UTIL_H

#include "i3ImageUtil4Bits.h"
#include "i3ImageUtil8Bits.h"
#include "i3ImageUtil16Bits.h"
#include "i3ImageUtil24Bits.h"
#include "i3ImageUtil32Bits.h"
#include "i3ImageUtilHDR.h"
#include "i3ImageUtilDXT.h"

I3_EXPORT_GFX 
void i3Image_Convert( I3G_IMAGE_FORMAT srcFormat, char * pSrc, INT32 Width, INT32 Height, INT32 srcAlign, i3Clut * pSrcClut,
					  I3G_IMAGE_FORMAT destFormat, char * pDest, INT32 destAlign, i3Clut * pDestClut);

I3_EXPORT_GFX 
void i3Image_BuildMipmap( I3G_IMAGE_FORMAT srcFormat, char * pSrc, INT32 Width, INT32 Height, INT32 srcAlign, i3Clut * pSrcClut,
						char * pDest, INT32 destAlign);

// 일반모드 텍스쳐를 PSP의 고속모드( Swizzled ) 포맷으로 만들기.
// pDest : 대상 버퍼
// pSrc  : 원본 버퍼
// nBytesPerLine  : 버퍼의 가로 사이즈. BYTE 단위.
// nLine  : 버퍼의 세로 사이즈. nLine
I3_EXPORT_GFX  BOOL i3ImageConvertLinearToSwizzled( char* pDest, char* pSrc, UINT32 nBytesPerLine, UINT32 nLine, I3_PLATFORM Platform );

// PSP의 고속모드( Swizzled ) 포맷을 일반모드 텍스쳐로 만들기.
// pDest : 대상 버퍼
// pSrc  : 원본 버퍼
// nBytesPerLine  : 버퍼의 가로 사이즈. BYTE 단위.
// nLine  : 버퍼의 세로 사이즈. nLine
I3_EXPORT_GFX  BOOL i3ImageConvertSwizzledToLinear( char* pDest, char* pSrc, UINT32 nBytesPerLine, UINT32 nLine, I3_PLATFORM Platform );

#endif
