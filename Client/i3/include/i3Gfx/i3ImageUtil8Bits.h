#if !defined( __I3_IMAGE_UTIL_8_H)
#define __I3_IMAGE_UTIL_8_H

#include "i3Texture.h"

I3_EXPORT_GFX
void i3PixelGet_8( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void i3PixelSet_8( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void i3Image_Convert8to4( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
						 UINT8 * pDest, INT32 destLineAlign, i3Clut * pDestClut);

I3_EXPORT_GFX
void i3Image_Convert8to8( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
						  UINT8 * pDest, INT32 destLineAlign, i3Clut * pDestClut);

I3_EXPORT_GFX
void i3Image_Convert8to16_ARGB_X555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
							    UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
							    UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to16_RGB_565(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to16_ARGB_4444(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
									UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to16_ABGR_4444(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to24_RGB(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to24_BGR(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to32_RGBA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to32_RGBX( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert8to32_BGRX( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign);

#endif
