#include "ImageUtil24Bits.h"

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 24 (R G B)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3Image_Convert24_RGB_to_16_ARGB_X555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = (((UINT16)*pSrc & 0xF8) << 7);	pSrc++;			// >> 3 << 10		1111 1000 : 0xF8
			val |= (((UINT16)*pSrc & 0xF8) << 2);	pSrc++;			// >> 3 << 5		1111 1000 : 0xF8
			val |= ((((UINT16)*pSrc) >> 3) & 0x1F);	pSrc++;			// >> 3 << 0		1111 1000 : 0xF8

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_RGB_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = (((UINT16)*pSrc & 0xF8) << 7);	pSrc++;			// >> 3 << 10		1111 1000 : 0xF8
			val |= (((UINT16)*pSrc & 0xF8) << 2);	pSrc++;			// >> 3 << 5		1111 1000 : 0xF8
			val |= ((((UINT16)*pSrc) >> 3) & 0x1F);	pSrc++;			// >> 3 << 0		1111 1000 : 0xF8
			val |= 0x8000;

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_RGB_to_16_RGB_565(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = (((UINT16)*pSrc & 0xF8) << 8);	pSrc++;			// >> 3 << 11		1111 1000 : 0xF8
			val |= (((UINT16)*pSrc & 0xFC) << 2);	pSrc++;			// >> 2 << 5		1111 1100 : 0xF8
			val |= ((((UINT16)*pSrc) >> 3) & 0x1F);	pSrc++;			// >> 3 << 0		1111 1000 : 0xF8

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_RGB_to_16_ARGB_4444(
	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 8;		pSrc++;		// Red
			val |= ((*pSrc >> 4) & 0x0F) << 4;	pSrc++;		// Green
			val |= ((*pSrc >> 4) & 0x1F) << 0;	pSrc++;		// Blue
			val |= 0xF000;									// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_RGB_to_16_ABGR_4444(
	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F);		pSrc++;		// Red
			val |= ((*pSrc >> 4) & 0x0F) << 4;	pSrc++;		// Green
			val |= ((*pSrc >> 4) & 0x1F) << 8;	pSrc++;		// Blue
			val |= 0xF000;									// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_RGB_to_24_RGB(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i;

	CALC_PARAM(24, 24);

	if (srcLineGap == destLineGap)
	{
		memcpy(pDest, pSrc, destLineSz * height);
		return;
	}

	for (i = 0; i < height; i++)
	{
		memcpy(pDest, pSrc, destLineSz);

		pSrc += (srcLineSz + srcLineGap);
		pDest += (destLineSz + destLineGap);
	}
}

void i3Image_Convert24_RGB_to_24_BGR(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(24, 24);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			pDest[0] = pSrc[2];
			pDest[1] = pSrc[1];
			pDest[2] = pSrc[0];

			pSrc += 3;
			pDest += 3;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert24_RGB_to_32_RGBA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(24, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = *pSrc;		pDest++;		pSrc++;
			*pDest = *pSrc;		pDest++;		pSrc++;
			*pDest = *pSrc;		pDest++;		pSrc++;
			*pDest = 0xFF;		pDest++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert24_RGB_to_32_BGRA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(24, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = pSrc[2];		pDest++;
			*pDest = pSrc[1];		pDest++;
			*pDest = pSrc[0];		pDest++;
			*pDest = 0xFF;			pDest++;

			pSrc += 3;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 24 (B G R)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3Image_Convert24_BGR_to_16_ARGB_X555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((((UINT16)*pSrc) >> 3) & 0x1F);	pSrc++;			// >> 3 << 0		1111 1000 : 0xF8
			val |= (((UINT16)*pSrc & 0xF8) << 2);	pSrc++;			// >> 3 << 5		1111 1000 : 0xF8
			val |= (((UINT16)*pSrc & 0xF8) << 7);	pSrc++;			// >> 3 << 10		1111 1000 : 0xF8

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_BGR_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((((UINT16)*pSrc) >> 3) & 0x1F);	pSrc++;			// >> 3 << 0		1111 1000 : 0xF8
			val |= (((UINT16)*pSrc & 0xF8) << 2);	pSrc++;			// >> 3 << 5		1111 1000 : 0xF8
			val |= (((UINT16)*pSrc & 0xF8) << 7);	pSrc++;			// >> 3 << 10		1111 1000 : 0xF8
			val |= 0x8000;

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_BGR_to_16_RGB_565(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((((UINT16)*pSrc) >> 3) & 0x1F);	pSrc++;			// >> 3 << 0		1111 1000 : 0xF8
			val |= (((UINT16)*pSrc & 0xFC) << 2);	pSrc++;			// >> 2 << 5		1111 1100 : 0xFC
			val |= (((UINT16)*pSrc & 0xF8) << 8);	pSrc++;			// >> 3 << 11		1111 1000 : 0xF8

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_BGR_to_16_ARGB_4444(
	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 0;	pSrc++;		// Blue
			val |= ((*pSrc >> 4) & 0x0F) << 4;	pSrc++;		// Green
			val |= ((*pSrc >> 4) & 0x0F) << 8;	pSrc++;		// Red
			val |= 0xF000;

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_BGR_to_16_ABGR_4444(
	UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;
	register UINT16 val;

	CALC_PARAM(24, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 4;	pSrc++;		// Blue
			val |= ((*pSrc >> 4) & 0x0F) << 8;	pSrc++;		// Green
			val |= ((*pSrc >> 4) & 0x0F) << 12;	pSrc++;		// Red
			val |= 0x000F;

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert24_BGR_to_32_RGBA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(24, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = pSrc[2];		pDest++;
			*pDest = pSrc[1];		pDest++;
			*pDest = pSrc[0];		pDest++;
			*pDest = 0xFF;		pDest++;

			pSrc += 3;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert24_BGR_to_32_BGRA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign,
	UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	INT32 i, j;

	CALC_PARAM(24, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = pSrc[0];		pDest++;
			*pDest = pSrc[1];		pDest++;
			*pDest = pSrc[2];		pDest++;
			*pDest = 0xFF;			pDest++;

			pSrc += 3;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}
