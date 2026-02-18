#if !defined( __I3_IMAGE_UTIL_4BITS_H)
#define __I3_IMAGE_UTIL_4BITS_H

#include "i3GfxType.h"
#include "i3Clut.h"
#include "i3Texture.h"

I3_EXPORT_GFX
void i3PixelGet_4( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void i3PixelSet_4( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void i3Image_Convert4to4( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
						  char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to8( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, char * pDest, INT32 destLineAlign, i3Clut * pdestClut);

I3_EXPORT_GFX
void i3Image_Convert4to16_ARGB_X555( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
							    char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to16_ARGB_1555( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
							    char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to16_RGB_565(	char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to16_ARGB_4444(	char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
									char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to16_ABGR_4444(	char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to24_RGB(	char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to24_BGR(	char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to32_RGBA( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pClut,
								char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to32_BGRA( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pClut,
								char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to32_RGBX( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pClut,
								char * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert4to32_BGRX( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pClut,
								char * pDest, INT32 destLineAlign);

#endif
