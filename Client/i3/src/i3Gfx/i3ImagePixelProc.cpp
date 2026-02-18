#include "i3GfxType.h"
#include "i3Image.h"

void i3Image::i3GetPixel_Index4( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + ((u * pImage->GetBitsPerPixel()) >> 3);

	if( u & 1)
	{
		// low-nibble
		*pIdx = *pDest & 0x0F;
	}
	else
	{
		// high-nibble
		*pIdx = (*pDest >> 4) & 0x0F;
	}
}

void i3Image::i3SetPixel_Index4( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + ((u * pImage->GetBitsPerPixel()) >> 3);

	if( u & 1)
	{
		// low-nibble
		*pDest = (*pDest & 0xF0) | (*pIdx & 0x0F);
	}
	else
	{
		// high-nibble
		*pDest = (*pDest & 0x0F) | ((*pIdx << 4) & 0xF0);
	}
}

void i3Image::i3GetPixel_Index4H( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx)
{
	char * pDest;

	pDest = pImage->GetData() + (v * pImage->GetStride()) + u;

	*pIdx = (*pDest >> 4) & 0x0F;
}

void i3Image::i3SetPixel_Index4H( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx)
{
	char * pDest;

	pDest = pImage->GetData() + (v * pImage->GetStride()) + u;

	*pDest = (*pIdx << 4) & 0xF0;
}

void i3Image::i3GetPixel_Index4L( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx)
{
	char * pDest;

	pDest = pImage->GetData() + (v * pImage->GetStride()) + u;

	*pIdx = *pDest & 0x0F;
}

void i3Image::i3SetPixel_Index4L( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx)
{
	char * pDest;

	pDest = pImage->GetData() + (v * pImage->GetStride()) + u;

	*pDest = *pIdx & 0x0F;
}

void i3Image::i3GetPixel_Index8( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx)
{
	char * pDest;

	pDest = pImage->GetData() + (v * pImage->GetStride()) + u;

	*pIdx = *pDest;
}

void i3Image::i3SetPixel_Index8( i3Image * pImage, INT32 u, INT32 v, UINT8 * pIdx)
{
	char * pDest;

	pDest = pImage->GetData() + (v * pImage->GetStride()) + u;

	*pDest = *pIdx;
}

void i3Image::i3GetPixel_RGB_16( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u << 1);

	i3Color::ConvertRGB15( *((UINT16 *)pDest), pColor);
}

void i3Image::i3SetPixel_RGB_16( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u << 1);

	*((UINT16 *)pDest) = i3Color::ConvertRGB15( pColor);
}

void i3Image::i3GetPixel_ARGB_16( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u << 1);

	i3Color::ConvertARGB16( *((UINT16 *)pDest), pColor);
}

void i3Image::i3SetPixel_ARGB_16( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u << 1);

	*((UINT16 *)pDest) = i3Color::ConvertARGB16( pColor);
}

void i3Image::i3GetPixel_RGB_24( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u * 3);

	i3Color::Set( pColor, (UINT8)pDest[0], (UINT8)pDest[1], (UINT8)pDest[2], 0xFF);
}

void i3Image::i3SetPixel_RGB_24( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u * 3);

	pDest[0] = i3Color::GetR( pColor);
	pDest[1] = i3Color::GetG( pColor);
	pDest[2] = i3Color::GetB( pColor);
}

void i3Image::i3GetPixel_RGBA_32( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u << 2);

	i3Color::Set( pColor, (UINT8)pDest[0], (UINT8)pDest[1], (UINT8)pDest[2], (UINT8)pDest[3]);
}

void i3Image::i3SetPixel_RGBA_32( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u << 2);

	pDest[0] = i3Color::GetR( pColor);
	pDest[1] = i3Color::GetG( pColor);
	pDest[2] = i3Color::GetB( pColor);
	pDest[3] = i3Color::GetA( pColor);
}

void i3Image::i3GetPixel_RGBX_32( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	char * pDest;

	pDest = pImage->GetData() + (pImage->GetStride() * v) + (u << 2);

	i3Color::Set( pColor, (UINT8)pDest[0], (UINT8)pDest[1], (UINT8)pDest[2], 0xFF);
}

void i3Image::i3SetPixel_RGBX_32( i3Image * pImage, INT32 u, INT32 v, COLOR * pColor)
{
	unsigned char * pDest;

	pDest = (unsigned char *) pImage->GetData() + (pImage->GetStride() * v) + (u << 2);

	pDest[0] = i3Color::GetR( pColor);
	pDest[1] = i3Color::GetG( pColor);
	pDest[2] = i3Color::GetB( pColor);
	pDest[3] = (unsigned char)0xFF;
}
