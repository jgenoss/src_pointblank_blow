#include "i3ImageDefine.h"

#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 32 (R G B A)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void i3Image_Convert32_RGBA_to_16_ARGB_X555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBA_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBX_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBA_to_16_RGB_565(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);



void i3Image_Convert32_RGBX_to_16_ARGB_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBA_to_16_ARGB_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBX_to_16_ABGR_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBA_to_16_ABGR_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);



void i3Image_Convert32_RGBA_to_24_RGB(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBA_to_24_BGR(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);



void i3Image_Convert32_RGBA_to_32_RGBA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBX_to_32_RGBA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBA_to_32_BGRA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);


void i3Image_Convert32_RGBX_to_32_BGRA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 32 (B G R A)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void i3Image_Convert32_BGRA_to_16_ARGB_X555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_BGRA_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_BGRX_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_BGRA_to_16_RGB_565(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_BGRX_to_16_ARGB_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_BGRA_to_16_ARGB_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_BGRX_to_16_ABGR_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);


void i3Image_Convert32_BGRA_to_16_ABGR_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign);

