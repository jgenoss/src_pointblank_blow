#if !defined( __I3_IMAGE_UTIL_HDR_H)
#define __I3_IMAGE_UTIL_HDR_H

#include "i3GfxType.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// HDR
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// I3G_IMAGE_FORMAT_ABGR_16F Convert
	// Convert 16F_ABGR to 16 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 16F_ABGR to 24 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_24_RGB_888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 16F_ABGR to 32 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16F_ABGR_to_32_BGRA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

// I3G_IMAGE_FORMAT_ABGR_32F
	// Convert 32F_ABGR to 16 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 32F_ABGR to 24 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_24_RGB_888( REAL32 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_24_BGR_8884( REAL32 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 32F_ABGR to 32 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_32_RGBX_8888( REAL32 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_32_BGRX_8888( REAL32 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_32_RGBA_8888( REAL32 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_ABGR_to_32_BGRA_8888( REAL32 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);

// I3G_IMAGE_FORMAT_ABGR_16
	// Convert 16 ABGR to 16 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 16 ABGR to 24 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_24_RGB_888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 16 ABGR to 32 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert16_ABGR_to_32_BGRA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

// I3G_IMAGE_FORMAT_R_32F
	// Convert 32F_R to 16 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 32F_R to 24 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_24_RGB_888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 32F_R to 32 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_R_to_32_BGRA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	I3_EXPORT_GFX
	void	i3PixelGet_R_32F( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

	I3_EXPORT_GFX
	void	i3PixelSet_R_32F( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);


// I3G_IMAGE_FORMAT_GR_32F
	// Convert 32F_GR to 16 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	I3_EXPORT_GFX
	void	i3PixelGet_GR_32F( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

	I3_EXPORT_GFX
	void	i3PixelSet_GR_32F( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

	// Convert 32F_GR to 24 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_24_RGB_888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

	// Convert 32F_GR to 32 Bit Format
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
	I3_EXPORT_GFX
	void i3Image_Convert32F_GR_to_32_BGRA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

#endif
