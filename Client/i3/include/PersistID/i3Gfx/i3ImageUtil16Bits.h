#if !defined( __I3_IMAGE_UTIL_16_H)
#define __I3_IMAGE_UTIL_16_H

#include "i3GfxType.h"
#include "i3Texture.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 0 5 5 5
// A R G B
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_EXPORT_GFX
void	i3PixelGet_XRGB_1555( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_XRGB_1555( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX 
void i3Image_Convert16_X555_to_16_X555( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_X555_to_16_1555( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_X555_to_16_565( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_X555_to_24_RGB( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_X555_to_24_BGR( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_X555_to_32_RGBA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_X555_to_32_BGRA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 1 5 5 5
// A R G B
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_EXPORT_GFX
void	i3PixelGet_ARGB_1555( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_ARGB_1555( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX 
void i3Image_Convert16_1555_to_32_RGBA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_1555_to_32_BGRA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 0 5 6 5
// A R G B
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_EXPORT_GFX
void	i3PixelGet_RGB_565( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_RGB_565( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX 
void i3Image_Convert16_565_to_16_X555( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_565_to_16_1555( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_565_to_24_RGB( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_565_to_24_BGR( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_565_to_32_RGBA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_565_to_32_BGRA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4 4 4 4
// A B G R
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_EXPORT_GFX
void	i3PixelGet_ABGR_4444( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX
void	i3PixelSet_ABGR_4444( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

I3_EXPORT_GFX 
void i3Image_Convert16_ABGR_4444_to_16_ABGR_4444( 
			UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
			UINT16	* pDest, INT32 destLineAlign);

I3_EXPORT_GFX 
void i3Image_Convert16_ABGR_4444_to_16_ARGB_4444( 
			UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
			UINT16	* pDest, INT32 destLineAlign);

#endif
