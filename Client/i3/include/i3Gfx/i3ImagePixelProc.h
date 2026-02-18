#if !defined( __I3_IMAGE_PIXEL_PROC_H)
#define __I3_IMAGE_PIXEL_PROC_H

#include "i3GfxType.h"
#include "i3Image.h"

void i3GetPixel_Index4( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx);
void i3SetPixel_Index4( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx);
void i3GetPixel_Index4H( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx);
void i3SetPixel_Index4H( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx);
void i3GetPixel_Index4L( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx);
void i3SetPixel_Index4L( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx);
void i3GetPixel_Index8( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx);
void i3SetPixel_Index8( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx);
void i3GetPixel_RGB_16( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3SetPixel_RGB_16( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3GetPixel_ARGB_16( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3SetPixel_ARGB_16( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3GetPixel_RGB_24( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3SetPixel_RGB_24( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3GetPixel_ARGB_32( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3SetPixel_ARGB_32( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3GetPixel_XRGB_32( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);
void i3SetPixel_XRGB_32( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor);

#endif
