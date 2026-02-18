#include "i3ImageDefine.h"

#pragma once

void i3Image_Convert16_X555_to_16_X555(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT16	* pDest, INT32 destLineAlign);


void i3Image_Convert16_X555_to_16_1555(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT16	* pDest, INT32 destLineAlign);


void i3Image_Convert16_X555_to_16_565(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT16	* pDest, INT32 destLineAlign);


void i3Image_Convert16_X555_to_24_RGB(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);


void i3Image_Convert16_X555_to_24_BGR(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);


void i3Image_Convert16_X555_to_32_RGBA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);


void i3Image_Convert16_X555_to_32_BGRA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 1 5 5 5
// A R G B
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void i3Image_Convert16_1555_to_32_RGBA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);


void i3Image_Convert16_1555_to_32_BGRA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 0 5 6 5
// A R G B
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void i3Image_Convert16_565_to_16_X555(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT16	* pDest, INT32 destLineAlign);


void i3Image_Convert16_565_to_16_1555(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT16	* pDest, INT32 destLineAlign);


void i3Image_Convert16_565_to_24_RGB(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);


void i3Image_Convert16_565_to_24_BGR(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);


void i3Image_Convert16_565_to_32_RGBA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);


void i3Image_Convert16_565_to_32_BGRA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT8	* pDest, INT32 destLineAlign);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4 4 4 4
// A B G R
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void i3Image_Convert16_ABGR_4444_to_16_ABGR_4444(
UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT16	* pDest, INT32 destLineAlign);


void i3Image_Convert16_ABGR_4444_to_16_ARGB_4444(
UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
UINT16	* pDest, INT32 destLineAlign);