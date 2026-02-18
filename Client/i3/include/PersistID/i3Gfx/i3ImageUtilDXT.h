#if !defined( __I3_IMAGE_UTIL_DXT_H)
#define __I3_IMAGE_UTIL_DXT_H

#include "i3GfxType.h"
#include "i3Texture.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DXT
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// DXT_1_X
	I3_EXPORT_GFX	void i3Image_ConvertDXT1_X_to_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX	void i3Image_ConvertDXT1_X_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	

	// DXT_1
	I3_EXPORT_GFX	void i3Image_ConvertDXT1_to_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX	void i3Image_ConvertDXT1_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	//PixelProc
	I3_EXPORT_GFX	void i3PixelGet_DXT1( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);
	I3_EXPORT_GFX	void i3PixelSet_DXT1( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DXT_2
	I3_EXPORT_GFX	void i3Image_ConvertDXT2_to_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX	void i3Image_ConvertDXT2_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	//PixelProc
	I3_EXPORT_GFX	void i3PixelGet_DXT2( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);
	I3_EXPORT_GFX	void i3PixelSet_DXT2( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DXT_3
	I3_EXPORT_GFX	void i3Image_ConvertDXT3_to_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX	void i3Image_ConvertDXT3_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	I3_EXPORT_GFX	void i3PixelGet_DXT3( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);
	I3_EXPORT_GFX	void i3PixelSet_DXT3( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DXT_4
	I3_EXPORT_GFX	void i3Image_ConvertDXT4_to_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX	void i3Image_ConvertDXT4_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	I3_EXPORT_GFX	void i3PixelGet_DXT4( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);
	I3_EXPORT_GFX	void i3PixelSet_DXT4( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DXT_5
	I3_EXPORT_GFX	void i3Image_ConvertDXT5_to_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX	void i3Image_ConvertDXT5_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	I3_EXPORT_GFX	void i3PixelGet_DXT5( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);
	I3_EXPORT_GFX	void i3PixelSet_DXT5( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

#endif // __I3_IMAGE_UTIL_DXT_H
