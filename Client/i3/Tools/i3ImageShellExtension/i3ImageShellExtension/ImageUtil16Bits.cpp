#include "ImageUtil16bits.h"

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

static UINT8 s_AlphaTable[2] = { 0x00, 0xFF };

void i3Image_Convert16_X555_to_16_X555(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i;

	CALC_PARAM(16, 16);

	if (srcLineGap == destLineGap)
	{
		memcpy(pDest, pSrc, destLineSz * height);
		return;
	}

	for (i = 0; i < height; i++)
	{
		memcpy(pDest, pSrc, destLineSz);

		pSrc = (UINT16 *)((char *)pSrc + (srcLineSz + srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineSz + destLineGap));
	}
}

void i3Image_Convert16_X555_to_16_1555(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(16, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = *pSrc | 0x8000;

			pDest++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

void i3Image_Convert16_X555_to_16_565(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(16, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = (*pSrc & 0x001F) | ((*pSrc << 1) & 0xFFC0);
			pDest++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

void i3Image_Convert16_X555_to_24_RGB(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 24);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
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

void i3Image_Convert16_X555_to_24_BGR(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 24);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
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

void i3Image_Convert16_X555_to_32_RGBA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
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

void i3Image_Convert16_X555_to_32_BGRA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
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
void i3Image_Convert16_1555_to_32_RGBA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x7C00) >> 7);	pDest++;			// 0111 1100 0000 0000 : 0x7C00
			*pDest = (UINT8)((val & 0x03E0) >> 2);	pDest++;			// 0000 0011 1110 0000 : 0x03E0
			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// 0000 0000 0001 1111 : 0x001F
			*pDest = s_AlphaTable[(val >> 15) & 1]; pDest++;

			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest += destLineGap;
	}
}

void i3Image_Convert16_1555_to_32_BGRA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = *pSrc;

			*pDest = (UINT8)((val & 0x001F) << 3);	pDest++;			// 0000 0000 0001 1111 : 0x001F
			*pDest = (UINT8)((val & 0x03E0) >> 2);	pDest++;			// 0000 0011 1110 0000 : 0x03E0
			*pDest = (UINT8)((val & 0x7C00) >> 7);	pDest++;			// 0111 1100 0000 0000 : 0x7C00
			*pDest = s_AlphaTable[(val >> 15) & 1]; pDest++;

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
void i3Image_Convert16_565_to_16_X555(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(16, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = (*pSrc & 0x001F) | ((*pSrc >> 1) & 0x07FF);			// 0000 0111 1111 1111

			pDest++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

void i3Image_Convert16_565_to_16_1555(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(16, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = (*pSrc & 0x001F) | ((*pSrc >> 1) & 0x07FF) | 0x8000;	// 0000 0111 1111 1111

			pDest++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

void i3Image_Convert16_565_to_24_RGB(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 24);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
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

void i3Image_Convert16_565_to_24_BGR(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 24);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
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

void i3Image_Convert16_565_to_32_RGBA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
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

void i3Image_Convert16_565_to_32_BGRA(UINT16	* pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8	* pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(16, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
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

	CALC_PARAM(16, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = *pSrc;

			pDest++;
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

	CALC_PARAM(16, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = *pSrc & 0xF0F0;
			*pDest |= (*pSrc >> 8) & 0x000F;
			*pDest |= (*pSrc << 8) & 0x0F00;

			pDest++;
			pSrc++;
		}

		pSrc = (UINT16 *)((char *)pSrc + (srcLineGap));
		pDest = (UINT16 *)((char *)pDest + (destLineGap));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
