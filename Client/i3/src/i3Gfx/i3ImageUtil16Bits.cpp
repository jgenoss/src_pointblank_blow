#include "i3GfxType.h"
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

static UINT8 s_AlphaTable[2] = { 0x00, 0xFF};

void i3Image_Convert16_X555_to_16_X555( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i;

	CALC_PARAM( 16, 16);

	if( srcLineGap == destLineGap)
	{
		memcpy( pDest, pSrc, destLineSz * height);
		return;
	}

	for( i = 0; i < height; i++)
	{
		memcpy( pDest, pSrc, destLineSz);

		pSrc = (UINT16 *)((char *)pSrc + (srcLineSz + srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineSz + destLineGap));
	}
}

void i3Image_Convert16_X555_to_16_1555( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM( 16, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			*pDest = *pSrc | 0x8000;

			pDest ++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

void i3Image_Convert16_X555_to_16_565( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM( 16, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			*pDest = (*pSrc & 0x001F) | ((*pSrc << 1) & 0xFFC0);
			pDest ++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

void i3Image_Convert16_X555_to_24_RGB( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 24);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x7C00) >> 7);	pDest++;			// 0111 1100 0000 0000 : 0x7C00
			*pDest = (UINT8)((val & 0x03E0) >> 2);	pDest++;			// 0000 0011 1110 0000 : 0x03E0
			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// 0000 0000 0001 1111 : 0x001F

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

void i3Image_Convert16_X555_to_24_BGR( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 24);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// 0000 0000 0001 1111 : 0x001F
			*pDest = (UINT8)((val & 0x03E0) >> 2);	pDest++;			// 0000 0011 1110 0000 : 0x03E0
			*pDest = (UINT8)((val & 0x7C00) >> 7);	pDest++;			// 0111 1100 0000 0000 : 0x7C00

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

void i3Image_Convert16_X555_to_32_RGBA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x7C00) >> 7);	pDest++;			// 0111 1100 0000 0000 : 0x7C00
			*pDest = (UINT8)((val & 0x03E0) >> 2);	pDest++;			// 0000 0011 1110 0000 : 0x03E0
			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// 0000 0000 0001 1111 : 0x001F
			*pDest = 0xFF;							pDest++;

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

void i3Image_Convert16_X555_to_32_BGRA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// 0000 0000 0001 1111 : 0x001F
			*pDest = (UINT8)((val & 0x03E0) >> 2);	pDest++;			// 0000 0011 1110 0000 : 0x03E0
			*pDest = (UINT8)((val & 0x7C00) >> 7);	pDest++;			// 0111 1100 0000 0000 : 0x7C00
			*pDest = 0xFF;							pDest++;

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 1 5 5 5
// A R G B
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3Image_Convert16_1555_to_32_RGBA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x7C00) >> 7);	pDest++;			// 0111 1100 0000 0000 : 0x7C00
			*pDest = (UINT8)((val & 0x03E0) >> 2);	pDest++;			// 0000 0011 1110 0000 : 0x03E0
			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// 0000 0000 0001 1111 : 0x001F
			*pDest = s_AlphaTable[ (val >> 15) & 1]; pDest++;

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

void i3Image_Convert16_1555_to_32_BGRA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// 0000 0000 0001 1111 : 0x001F
			*pDest = (UINT8)((val & 0x03E0) >> 2);	pDest++;			// 0000 0011 1110 0000 : 0x03E0
			*pDest = (UINT8)((val & 0x7C00) >> 7);	pDest++;			// 0111 1100 0000 0000 : 0x7C00
			*pDest = s_AlphaTable[ (val >> 15) & 1]; pDest++;

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 0 5 6 5
// A R G B
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3Image_Convert16_565_to_16_X555( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM( 16, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			*pDest = (*pSrc & 0x001F) | ((*pSrc >> 1) & 0x07FF);			// 0000 0111 1111 1111

			pDest ++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

void i3Image_Convert16_565_to_16_1555( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									    UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM( 16, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			*pDest = (*pSrc & 0x001F) | ((*pSrc >> 1) & 0x07FF) | 0x8000;	// 0000 0111 1111 1111

			pDest ++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

void i3Image_Convert16_565_to_24_RGB( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 24);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0xF800) >> 8);	pDest++;			// >> 11 << 3 		1111 1000 0000 0000 : 0xF800
			*pDest = (UINT8)((val & 0x07E0) >> 3);	pDest++;			// >> 5 << 2 		0000 0111 1110 0000 : 0x07E0
			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// >> 0 << 3		0000 0000 0001 1111 : 0x001F

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

void i3Image_Convert16_565_to_24_BGR( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 24);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// >> 0 << 3		0000 0000 0001 1111 : 0x001F
			*pDest = (UINT8)((val & 0x07E0) >> 3);	pDest++;			// >> 5 << 2 		0000 0111 1110 0000 : 0x07E0
			*pDest = (UINT8)((val & 0xF800) >> 8);	pDest++;			// >> 11 << 3 		1111 1000 0000 0000 : 0xF800

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

void i3Image_Convert16_565_to_32_RGBA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0xF800) >> 8);	pDest++;			// >> 11 << 3 		1111 1000 0000 0000 : 0xF800
			*pDest = (UINT8)((val & 0x07E0) >> 3);	pDest++;			// >> 5 << 2 		0000 0111 1110 0000 : 0x07E0
			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// >> 0 << 3		0000 0000 0001 1111 : 0x001F
			*pDest = 0xFF;							pDest++;

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

void i3Image_Convert16_565_to_32_BGRA( UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
									   UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM( 16, 32);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// >> 0 << 3		0000 0000 0001 1111 : 0x001F
			*pDest = (UINT8)((val & 0x07E0) >> 3);	pDest++;			// >> 5 << 2 		0000 0111 1110 0000 : 0x07E0
			*pDest = (UINT8)((val & 0xF800) >> 8);	pDest++;			// >> 11 << 3 		1111 1000 0000 0000 : 0xF800
			*pDest = 0xFF;							pDest++;

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4 4 4 4
// A B G R
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3Image_Convert16_ABGR_4444_to_16_ABGR_4444( 
			UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
			UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM( 16, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			*pDest = *pSrc;

			pDest ++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}


void i3Image_Convert16_ABGR_4444_to_16_ARGB_4444( 
			UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
			UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM( 16, 16);

	for( i = 0; i < height; i++)
	{
		for( j = 0; j < width; j++)
		{
			*pDest = *pSrc & 0xF0F0;
			*pDest |= (*pSrc >> 8) & 0x000F;
			*pDest |= (*pSrc << 8) & 0x0F00;

			pDest ++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
I3_EXPORT_GFX
void	i3PixelGet_XRGB_1555( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT16 * pSrc;
	INT32 lv =  pTex->getLockedLevel();

	pSrc = (UINT16 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

	i3Color::Set( pCol, ((*pSrc >> 10) & 0x1F) * 0.032258064516129032258064516129032f,
						((*pSrc >> 5) & 0x1F) * 0.032258064516129032258064516129032f,
						((*pSrc >> 0) & 0x1F) * 0.032258064516129032258064516129032f,
						1.0f);
}

I3_EXPORT_GFX
void	i3PixelSet_XRGB_1555( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT16 * pSrc;
	INT32 lv =  pTex->getLockedLevel();

	pSrc = (UINT16 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

	*pSrc = ((UINT16)(i3Color::GetR( pCol) * 31.0f) << 10) |		// red
			((UINT16)(i3Color::GetG( pCol) * 31.0f) << 5)  |		// green
			((UINT16)(i3Color::GetB( pCol) * 31.0f) << 0) |			// blue
			0x8000;													// alpha
}

I3_EXPORT_GFX
void	i3PixelGet_ARGB_1555( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT16 * pSrc;
	INT32 lv =  pTex->getLockedLevel();

	pSrc = (UINT16 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

	i3Color::Set( pCol, ((*pSrc >> 10) & 0x1F) * 0.032258064516129032258064516129032f,
						((*pSrc >> 5) & 0x1F) * 0.032258064516129032258064516129032f,
						((*pSrc >> 0) & 0x1F) * 0.032258064516129032258064516129032f,
						(REAL32) ((*pSrc >> 15) & 0x01));
}

I3_EXPORT_GFX
void	i3PixelSet_ARGB_1555( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT16 * pSrc;
	INT32 lv =  pTex->getLockedLevel();

	pSrc = (UINT16 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

	*pSrc = ((UINT16)(i3Color::GetR( pCol) * 31.0f) << 10) |		// red
			((UINT16)(i3Color::GetG( pCol) * 31.0f) << 5)  |		// green
			((UINT16)(i3Color::GetB( pCol) * 31.0f) << 0)  |		// blue
			((UINT16)(i3Color::GetA( pCol) + 0.5f)	<< 15);			// alpha
}

I3_EXPORT_GFX
void	i3PixelGet_RGB_565( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT16 * pSrc;
	INT32 lv =  pTex->getLockedLevel();

	pSrc = (UINT16 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

	i3Color::Set( pCol, ((*pSrc >> 11) & 0x1F) * 0.032258064516129032258064516129032f,
						((*pSrc >> 5) & 0x3F) * 0.015873015873015873015873015873016f,
						((*pSrc >> 0) & 0x1F) * 0.032258064516129032258064516129032f,
						1.0f);
}

I3_EXPORT_GFX
void	i3PixelSet_RGB_565( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT16 * pSrc;
	INT32 lv =  pTex->getLockedLevel();

	pSrc = (UINT16 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

	*pSrc = ((UINT16)(i3Color::GetR( pCol) * 31.0f) << 11) |		// red
			((UINT16)(i3Color::GetG( pCol) * 63.0f) << 5)  |		// green
			((UINT16)(i3Color::GetB( pCol) * 31.0f) << 0);			// blue
}

I3_EXPORT_GFX
void	i3PixelGet_ABGR_4444( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT16 * pSrc;
	INT32 lv =  pTex->getLockedLevel();

	pSrc = (UINT16 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

	i3Color::Set( pCol, ((*pSrc >> 8 ) & 0x0F) * 0.066666666666666666666666666666667f,
						((*pSrc >> 4 ) & 0x0F) * 0.066666666666666666666666666666667f,
						((*pSrc >> 0 ) & 0x0F) * 0.066666666666666666666666666666667f,
						((*pSrc >> 12) & 0x0F) * 0.066666666666666666666666666666667f);
}

I3_EXPORT_GFX
void	i3PixelSet_ABGR_4444( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
{
	UINT16 * pSrc;
	INT32 lv =  pTex->getLockedLevel();

	pSrc = (UINT16 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

	*pSrc = ((UINT16)(i3Color::GetR( pCol) * 15.0f) << 8) |			// red
			((UINT16)(i3Color::GetG( pCol) * 15.0f) << 4)  |		// green
			((UINT16)(i3Color::GetB( pCol) * 15.0f) << 0)  |		// blue
			((UINT16)(i3Color::GetA( pCol) * 15.0f) << 12);			// alpha
}
