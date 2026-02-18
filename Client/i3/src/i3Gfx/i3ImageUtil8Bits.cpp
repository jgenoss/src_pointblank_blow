#include "i3GfxType.h"
#include "i3Clut.h"
#include "i3Texture.h"

#define		CALC_PARAM( srcBitCount, destBitCount)	\
	{\
		INT32 sp;\
		\
		/* Source Image의 1 line의 길이(bytes)를 구한다. */ \
		srcLineSz = (((width * srcBitCount) + 7) >> 3);\
		\
		/* align을 반영한 길이를 구한다. */ \
		sp = ((srcLineSz + srcLineAlign - 1) / srcLineAlign) * srcLineAlign;\
		\
		/* 1 line 마다 alignment에 의해 추가되어야 하는 padding byte의 개수 */ \
		srcLineGap = sp - srcLineSz;\
		\
		/* Dest. image의 1 line의 길이를 구한다. */ \
		destLineSz = (((width * destBitCount) + 7) >> 3); \
		\
		/* align을 반영한 길이를 구한다. */ \
		sp = ((destLineSz + destLineAlign - 1) / destLineAlign) * destLineAlign;\
		\
		/* 1 line 마다 alignment에 의해 추가되어야 하는 padding byte의 개수 */ \
		destLineGap = sp - destLineSz;\
	}

void i3Image_Convert8to4(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * /*pSrcClut*/, 
						 UINT8 * pDest, INT32 destLineAlign, i3Clut * /*pDestClut*/)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;

	CALC_PARAM( 8, 4);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width/2; j++)
		{
			*pDest =( ((*(pSrc + 1)) << 4) | *(pSrc) );
			pSrc += 2;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}


void i3Image_Convert8to8(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * /*pSrcClut*/, 
							UINT8 * pDest, INT32 destLineAlign, i3Clut * /*pDestClut*/)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i;

	CALC_PARAM( 8, 8);

	if(destLineGap == srcLineGap)
	{
		// 아무런 Alignment도 발생하지 않는다면 단순히 Memory Copy 만으로 
		memcpy( pDest, pSrc, srcLineSz * height);
		return;
	}

	for( i = 0; i < height; i++)
	{
		// line copy
		memcpy( pDest, pSrc, destLineSz);

		pSrc += (srcLineSz + srcLineGap);
		pDest += (destLineSz + destLineGap);
	}
}

void i3Image_Convert8to16_ARGB_X555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
							    UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 8, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			pSrcClut->GetColor( *pSrc, &Color);

			*pDest2 =	(((i3Color::GetR( &Color) >> 3) & 0x001F) << 10) |
					(((i3Color::GetG( &Color) >> 3) & 0x001F) << 5) |
					((i3Color::GetB( &Color) >> 3) & 0x001F);

			pSrc++;
			pDest2 ++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *)(((UINT8 *)pDest2) + destLineGap);
	}
}

void i3Image_Convert8to16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
							    UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 8, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			pSrcClut->GetColor( *pSrc, &Color);

			*pDest2 =	(((i3Color::GetA( &Color) >> 7) & 0x0001) << 15) |
						(((i3Color::GetR( &Color) >> 3) & 0x001F) << 10) |
						(((i3Color::GetG( &Color) >> 3) & 0x001F) << 5) |
						((i3Color::GetB( &Color) >> 3) & 0x001F);

			pSrc++;
			pDest2++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *)(((UINT8 *)pDest2) + destLineGap);
	}
}

void i3Image_Convert8to16_RGB_565(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 8, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			pSrcClut->GetColor( *pSrc, &Color);

			*pDest2 =	(((i3Color::GetR( &Color) >> 3) & 0x001F) << 11) |
						(((i3Color::GetG( &Color) >> 2) & 0x003F) << 5) |
						((i3Color::GetB( &Color) >> 3) & 0x001F);

			pSrc++;
			pDest2++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *) (((UINT8 *)pDest2) + destLineGap);
	}
}

void i3Image_Convert8to16_ARGB_4444(	
									UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
									UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 8, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			pSrcClut->GetColor( *pSrc, &Color);

			*pDest2 =	(((i3Color::GetR( &Color) >> 4) & 0x0F) << 8) |
				(((i3Color::GetG( &Color) >> 4) & 0x0F) << 4) |
				(((i3Color::GetB( &Color) >> 4) & 0x0F) << 0) |
				(((i3Color::GetA( &Color) >> 4) & 0x0F) << 12);

			pSrc++;
			pDest2++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *) (((UINT8 *)pDest2) + destLineGap);
	}
}

void i3Image_Convert8to16_ABGR_4444(	
			UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
			UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT32 r, g, b, a;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 8, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			pSrcClut->GetColor( *pSrc, &Color);
			
			r = (((UINT32)i3Color::GetR( &Color) >> 4) & 0x0F);
			g = (((UINT32)i3Color::GetG( &Color) >> 4) & 0x0F);
			b = (((UINT32)i3Color::GetB( &Color) >> 4) & 0x0F);
			a = (((UINT32)i3Color::GetA( &Color) >> 4) & 0x0F);

			*pDest2 =	(UINT16)( r << 12);
			*pDest2 |=  (UINT16)( g << 8);
			*pDest2 |=  (UINT16)( b << 4);
			*pDest2 |=  (UINT16) a;

			pSrc++;
			pDest2++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *) (((UINT8 *)pDest2) + destLineGap);
	}
}

void i3Image_Convert8to24_RGB(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	COLOR Color;

	CALC_PARAM( 8, 24);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			pSrcClut->GetColor( *pSrc, &Color);

			*pDest = i3Color::GetR( &Color);	pDest++;
			*pDest = i3Color::GetG( &Color);	pDest++;
			*pDest = i3Color::GetB( &Color);	pDest++;

			pSrc++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert8to24_BGR(	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	COLOR Color;

	CALC_PARAM( 8, 24);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			pSrcClut->GetColor( *pSrc, &Color);

			*pDest = i3Color::GetB( &Color);	pDest++;
			*pDest = i3Color::GetG( &Color);	pDest++;
			*pDest = i3Color::GetR( &Color);	pDest++;

			pSrc++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert8to32_RGBA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j, div, mod, k;
	UINT32 c;

	CALC_PARAM( 8, 32);

	div = width >> 2;
	mod = width & 0x03;

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < div; j++)
		{
			c = *((UINT32 *) pSrc);

			for( k = 0; k < 4; k++)
			{
				*((UINT32 *) pDest) = pSrcClut->GetColorRGBA32( c & 0xFF);

				pDest += 4;
				c >>= 8;
			}

			pSrc += 4;
		}

		for( j = 0; j < mod; j++)
		{
			*((UINT32 *) pDest) = pSrcClut->GetColorRGBA32( *pSrc);

			pDest += 4;
			pSrc ++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert8to32_BGRA( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j, div, mod, k;
	UINT32 c;

	CALC_PARAM( 8, 32);

	div = width >> 2;
	mod = width & 0x03;

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < div; j++)
		{
			c = *((UINT32 *) pSrc);

			for( k = 0; k < 4; k++)
			{
				*((UINT32 *) pDest) = pSrcClut->GetColorBGRA32( c & 0xFF);

				pDest += 4;
				c >>= 8;
			}

			pSrc += 4;
		}

		for( j = 0; j < mod; j++)
		{
			*((UINT32 *) pDest) = pSrcClut->GetColorBGRA32( *pSrc);

			pDest += 4;
			pSrc ++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert8to32_RGBX( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j, div, mod, k;
	UINT32 c;

	CALC_PARAM( 8, 32);

	div = width >> 2;
	mod = width & 0x03;

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < div; j++)
		{
			c = *((UINT32 *) pSrc);

			for( k = 0; k < 4; k++)
			{
				*((UINT32 *) pDest) = pSrcClut->GetColorRGBA32( c & 0xFF) | 0x000000FF;

				pDest += 4;
				c >>= 8;
			}

			pSrc += 4;
		}

		for( j = 0; j < mod; j++)
		{
			*((UINT32 *) pDest) = pSrcClut->GetColorRGBA32( *pSrc) | 0x000000FF;

			pDest += 4;
			pSrc ++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert8to32_BGRX( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j, div, mod, k;
	UINT32 c;

	CALC_PARAM( 8, 32);

	div = width >> 2;
	mod = width & 0x03;

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < div; j++)
		{
			c = *((UINT32 *) pSrc);

			for( k = 0; k < 4; k++)
			{
				*((UINT32 *) pDest) = pSrcClut->GetColorBGRA32( c & 0xFF) | 0xFF000000;

				pDest += 4;
				c >>= 8;
			}

			pSrc += 4;
		}

		for( j = 0; j < mod; j++)
		{
			*((UINT32 *) pDest) = pSrcClut->GetColorBGRA32( *pSrc)  | 0xFF000000;

			pDest += 4;
			pSrc ++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

I3_EXPORT_GFX
void i3PixelGet_8( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT8 * pSrc;
	INT32 lv = pTex->getLockedLevel();
	i3Clut * pClut = pTex->GetClut( lv);

	pSrc = (UINT8 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + x);

	pClut->GetColor( (INT32) *pSrc, pCol);
}

I3_EXPORT_GFX
void i3PixelSet_8( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT8 * pSrc;
	INT32 lv = pTex->getLockedLevel();
	i3Clut * pClut = pTex->GetClut( lv);

	pSrc = (UINT8 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + x);

	{
		COLOR col;

		i3Color::Set( &col, pCol);

		*pSrc = (UINT8) pClut->FindNearestColor(  &col);
	}
}
