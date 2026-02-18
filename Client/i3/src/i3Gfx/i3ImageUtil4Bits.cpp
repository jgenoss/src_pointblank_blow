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

void i3Image_Convert4to4( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * /*pSrcClut*/,
						  char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;

	CALC_PARAM( 4, 4);

#ifdef I3G_OGLES
	INT32 x, y;
	for( y = 0; y < height; y++)
	{
		for( x = 0; x < (width >> 1) ; x++)
		{		
			*pDest = ( ((*pSrc >> 4) & 0x0F) | ((*pSrc << 4) & 0xF0) );
			pDest++;
			pSrc++;
		}
		pSrc += srcLineGap;
		pDest += destLineGap;
	}
#else
	if( destLineGap == srcLineGap)
	{
		// 아무런 Alignment도 발생하지 않는다면 단순히 Memory Copy 만으로 
		memcpy( pDest, pSrc, srcLineSz * height);
		return;
	}

	for(INT32 i = 0; i < height; i++)
	{
		// line copy
        memcpy( pDest, pSrc, destLineSz);
		/*
		for( j = 0; j < destLineSz; j++)
		{
			UINT8 c;

			c = pSrc[j];
			pDest[j] = ((c >> 4) & 0x0F) | ((c << 4) & 0xF0);
		}
		*/

		pSrc += (srcLineSz + srcLineGap);
		pDest += (destLineSz + destLineGap);
	}
#endif
}

void i3Image_Convert4to8( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * /*pSrcClut*/, char * pDest, INT32 destLineAlign, i3Clut * /*pDestClut*/)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 i, j;
	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;

	CALC_PARAM( 4, 8);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				*pDest = (*pSrc >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				*pDest = *pSrc & 0x0F;
			}

			pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert4to16_ARGB_X555( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
							    char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 4, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest2 =	(((i3Color::GetR( &Color) >> 3) & 0x001F) << 10) |
					(((i3Color::GetG( &Color) >> 3) & 0x001F) << 5) |
					((i3Color::GetB( &Color) >> 3) & 0x001F);

			pDest2 ++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *)(((char *)pDest2) + destLineGap);
	}
}

void i3Image_Convert4to16_ARGB_1555( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut, 
							    char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 4, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest2 =	(((i3Color::GetA( &Color) >> 7) & 0x0001) << 15) |
						(((i3Color::GetR( &Color) >> 3) & 0x001F) << 10) |
						(((i3Color::GetG( &Color) >> 3) & 0x001F) << 5) |
						((i3Color::GetB( &Color) >> 3) & 0x001F);

			pDest2++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *)(((char *)pDest2) + destLineGap);
	}
}

void i3Image_Convert4to16_RGB_565(	char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 4, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest2 =	(((i3Color::GetR( &Color) >> 3) & 0x001F) << 11) |
						(((i3Color::GetG( &Color) >> 2) & 0x003F) << 5) |
						((i3Color::GetB( &Color) >> 3) & 0x001F);

			pDest2++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *) (((char *)pDest2) + destLineGap);
	}
}

void i3Image_Convert4to16_ARGB_4444(	
									char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
									char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 4, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest2 =	(((i3Color::GetR( &Color) >> 4) & 0x0F) << 8) |
						(((i3Color::GetG( &Color) >> 4) & 0x0F) << 4) |
						(((i3Color::GetB( &Color) >> 4) & 0x0F) << 0) |
						(((i3Color::GetA( &Color) >> 4) & 0x0F) << 12);

			pDest2++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *) (((char *)pDest2) + destLineGap);
	}
}

void i3Image_Convert4to16_ABGR_4444(	
					char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
					char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;
	UINT16 * pDest2 = (UINT16 *) pDest;

	CALC_PARAM( 4, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest2 =	(((i3Color::GetR( &Color) >> 4) & 0x0F) << 12) |
						(((i3Color::GetG( &Color) >> 4) & 0x0F) << 8) |
						(((i3Color::GetB( &Color) >> 4) & 0x0F) << 4) |
						(((i3Color::GetA( &Color) >> 4) & 0x0F) << 0);

			pDest2++;
		}

		pSrc += srcLineGap;
		pDest2 = (UINT16 *) (((char *)pDest2) + destLineGap);
	}
}

void i3Image_Convert4to24_RGB(	char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;

	CALC_PARAM( 4, 24);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest = i3Color::GetR( &Color);	pDest++;
			*pDest = i3Color::GetG( &Color);	pDest++;
			*pDest = i3Color::GetB( &Color);	pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert4to24_BGR(	char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;

	CALC_PARAM( 4, 24);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;				
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest = i3Color::GetB( &Color);	pDest++;
			*pDest = i3Color::GetG( &Color);	pDest++;
			*pDest = i3Color::GetR( &Color);	pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert4to32_RGBA( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;

	CALC_PARAM( 4, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest = i3Color::GetR( &Color);	pDest++;
			*pDest = i3Color::GetG( &Color);	pDest++;
			*pDest = i3Color::GetB( &Color);	pDest++;
			*pDest = i3Color::GetA( &Color);	pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert4to32_BGRA( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;

	CALC_PARAM( 4, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;				
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest = i3Color::GetB( &Color);	pDest++;
			*pDest = i3Color::GetG( &Color);	pDest++;
			*pDest = i3Color::GetR( &Color);	pDest++;
			*pDest = i3Color::GetA( &Color);	pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert4to32_RGBX( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;

	CALC_PARAM( 4, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;				
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest = i3Color::GetR( &Color);	pDest++;
			*pDest = i3Color::GetG( &Color);	pDest++;
			*pDest = i3Color::GetB( &Color);	pDest++;
			*((UINT8 *)pDest) = 0xFF;	pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert4to32_BGRX( char * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, i3Clut * pSrcClut,
								char * pDest, INT32 destLineAlign)
{
	I3ASSERT( pSrc != nullptr );
	I3ASSERT( pSrcClut != nullptr );
	I3ASSERT( pDest != nullptr );

	INT32 srcLineGap, destLineGap;
	INT32 srcLineSz, destLineSz;
	INT32 i, j;
	UINT16 idx;
	COLOR Color;

	CALC_PARAM( 4, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			if( j & 1)
			{
				// Odd
				idx = (*pSrc  >> 4) & 0x0F;
				pSrc++;
			}
			else
			{
				// Even
				idx = *pSrc & 0x0F;
			}

			pSrcClut->GetColor( idx, &Color);

			*pDest = i3Color::GetB( &Color);	pDest++;
			*pDest = i3Color::GetG( &Color);	pDest++;
			*pDest = i3Color::GetR( &Color);	pDest++;
			*((UINT8 *)pDest) = 0xFF;	pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

I3_EXPORT_GFX
void i3PixelGet_4( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT8 * pSrc, idx;
	INT32 lv = pTex->getLockedLevel();
	i3Clut * pClut = pTex->GetClut( lv);

	pSrc = (UINT8 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x >> 1));

	if( x & 1)
		idx = *pSrc & 0x0F;
	else
		idx = (*pSrc >> 4) & 0x0F;

	pClut->GetColor( (INT32) idx, pCol);
}

I3_EXPORT_GFX
void i3PixelSet_4( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT8 * pSrc, idx;
	INT32 lv = pTex->getLockedLevel();
	i3Clut * pClut = pTex->GetClut( lv);

	pSrc = (UINT8 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x >> 1));

	{
		COLOR col;

		i3Color::Set( &col, pCol);

		idx = (UINT8) pClut->FindNearestColor(  &col);
	}

	if( x & 1)
		*pSrc = (*pSrc & 0xF0) | (idx & 0x0F);
	else
		*pSrc = (*pSrc & 0x0F) | ((idx << 4) & 0xF0);
}
