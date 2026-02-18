#include "i3ImageDefine.h"

#pragma once

// DXT_1_X
void i3Image_ConvertDXT1_X_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
void i3Image_ConvertDXT1_X_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

// DXT_1
void i3Image_ConvertDXT1_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
void i3Image_ConvertDXT1_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DXT_2
void i3Image_ConvertDXT2_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
void i3Image_ConvertDXT2_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DXT_3
void i3Image_ConvertDXT3_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
void i3Image_ConvertDXT3_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DXT_4
void i3Image_ConvertDXT4_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
void i3Image_ConvertDXT4_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DXT_5
void i3Image_ConvertDXT5_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);
void i3Image_ConvertDXT5_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);
