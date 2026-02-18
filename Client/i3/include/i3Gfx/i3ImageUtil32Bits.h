#if !defined( __I3_IMAGE_UTIL_32_H)
#define __I3_IMAGE_UTIL_32_H

#include "i3GfxType.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 32 (R G B A)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_EXPORT_GFX
void	i3PixelGet_XBGR_8888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_XBGR_8888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelGet_ABGR_8888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_ABGR_8888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_16_ARGB_X555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBX_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


I3_EXPORT_GFX
void i3Image_Convert32_RGBX_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBX_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_24_RGB( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_24_BGR( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);


I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_32_RGBA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBX_to_32_RGBA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBA_to_32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_RGBX_to_32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 32 (B G R A)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_EXPORT_GFX
void	i3PixelGet_XRGB_8888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_XRGB_8888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelGet_ARGB_8888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_ARGB_8888( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void i3Image_Convert32_BGRA_to_16_ARGB_X555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_BGRA_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_BGRX_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_BGRA_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


I3_EXPORT_GFX
void i3Image_Convert32_BGRX_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_BGRA_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_BGRX_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

I3_EXPORT_GFX
void i3Image_Convert32_BGRA_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

//void i3Image_Convert32_BGRA_to_24_BGR( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
// same as i3Image_Convert32_RGBA_to_24_RGB()

//void i3Image_Convert32_BGRA_to_24_RGB( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
// same as i3Image_Convert32_RGBA_to_24_BGR()

//void i3Image_Convert32_BGRA_to_32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
// same as i3Image_Convert32_RGBA_to_32_RGBA()

//void i3Image_Convert32_BGRX_to_32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
// same as i3Image_Convert32_RGBX_to_32_RGBA()

// void i3Image_Convert32_BGRA_to_32_RGBA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
// same as i3Image_Convert32_RGBA_to_32_BGRA()

//void i3Image_Convert32_BGRX_to_32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
// same as i3Image_Convert32_RGBX_to_32_RGBA()

#endif
