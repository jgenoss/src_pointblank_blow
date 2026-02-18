#if !defined( __I3_IMAGE_UTIL_24_H)
#define __I3_IMAGE_UTIL_24_H

#include "i3GfxType.h"
#include "i3Texture.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 24 (R G B)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_EXPORT_GFX
void	i3PixelGet_RGB_888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_RGB_888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX 
void i3Image_Convert24_RGB_to_16_ARGB_X555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_RGB_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_RGB_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_RGB_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
										   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_RGB_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_RGB_to_24_RGB( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									  UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_RGB_to_24_BGR( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									  UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_RGB_to_32_RGBA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									  UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_RGB_to_32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									  UINT8 * pDest, INT32 destLineAlign);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 24 (B G R)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_EXPORT_GFX
void	i3PixelGet_BGR_888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_BGR_888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void i3Image_Convert24_BGR_to_16_ARGB_X555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_BGR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_BGR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_BGR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
										   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_BGR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_BGR_to_32_RGBA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									  UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert24_BGR_to_32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									  UINT8 * pDest, INT32 destLineAlign);

#endif
