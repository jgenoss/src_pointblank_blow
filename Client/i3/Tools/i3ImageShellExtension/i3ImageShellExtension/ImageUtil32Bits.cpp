#include "ImageUtil32Bits.h"

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
// 32 (R G B A)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3Image_Convert32_RGBA_to_16_ARGB_X555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc & 0x00F8) << 7);			pSrc++;
			val |= ((*pSrc & 0x00F8) << 2);			pSrc++;
			val |= ((*pSrc >> 3) & 0x001F);			pSrc += 2;

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_RGBA_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc & 0x00F8) << 7);			pSrc++;			// Red
			val |= ((*pSrc & 0x00F8) << 2);			pSrc++;			// Green
			val |= ((*pSrc >> 3) & 0x001F);			pSrc++;			// Blue
			val |= ((*pSrc & 0x0080) << 8);			pSrc++;			// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_RGBX_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc & 0x00F8) << 7);			pSrc++;			// Red
			val |= ((*pSrc & 0x00F8) << 2);			pSrc++;			// Green
			val |= ((*pSrc >> 3) & 0x001F);			pSrc += 2;		// Blue
			val |= 0x8000;											// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_RGBA_to_16_RGB_565(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc & 0x00F8) << 8);			pSrc++;			// Red
			val |= ((*pSrc & 0x00FC) << 2);			pSrc++;			// Green		0011 1111
			val |= ((*pSrc >> 3) & 0x001F);			pSrc += 2;		// Blue

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_RGBA_to_16_ARGB_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 8;			pSrc++;		// Red
			val |= ((*pSrc >> 4) & 0x0F) << 4;		pSrc++;		// Green		0011 1111
			val |= ((*pSrc >> 4) & 0x0F) << 0;		pSrc++;		// Blue
			val |= ((*pSrc >> 4) & 0x0F) << 12;	pSrc++;		// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_RGBA_to_16_ABGR_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F);			pSrc++;		// Red
			val |= ((*pSrc >> 4) & 0x0F) << 4;		pSrc++;		// Green		0011 1111
			val |= ((*pSrc >> 4) & 0x0F) << 8;		pSrc++;		// Blue
			val |= ((*pSrc >> 4) & 0x0F) << 12;	pSrc++;		// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_RGBX_to_16_ARGB_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 8;		pSrc++;		// Red
			val |= ((*pSrc >> 4) & 0x0F) << 4;		pSrc++;		// Green		0011 1111
			val |= ((*pSrc >> 4) & 0x0F) << 0;		pSrc++;		// Blue
			val |= 0xF000;							pSrc++;		// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_RGBX_to_16_ABGR_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F);			pSrc++;		// Red
			val |= ((*pSrc >> 4) & 0x0F) << 4;		pSrc++;		// Green		0011 1111
			val |= ((*pSrc >> 4) & 0x0F) << 8;		pSrc++;		// Blue
			val |= 0xF000;							pSrc++;		// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_RGBA_to_24_RGB(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;

	CALC_PARAM(32, 24);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = *pSrc;		pDest++;		pSrc++;
			*pDest = *pSrc;		pDest++;		pSrc++;
			*pDest = *pSrc;		pDest++;		pSrc++;
			pSrc++;

		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert32_RGBA_to_24_BGR(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;

	CALC_PARAM(32, 24);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = pSrc[2];		pDest++;
			*pDest = pSrc[1];		pDest++;
			*pDest = pSrc[0];		pDest++;

			pSrc += 4;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert32_RGBA_to_32_RGBA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;

	CALC_PARAM(32, 32);

	if (srcLineGap == destLineGap)
	{
		memcpy(pDest, pSrc, destLineSz * height);
		return;
	}

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < (width >> 2); j++)
		{
			*((UINT32 *)pDest) = *((UINT32 *)pSrc);

			pDest += 4;
			pSrc += 4;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert32_RGBX_to_32_RGBA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;

	CALC_PARAM(32, 32);

	if (srcLineGap == destLineGap)
	{
		memcpy(pDest, pSrc, destLineSz * height);
		return;
	}

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < (width >> 2); j++)
		{
			*pDest = *pSrc;		pDest++;		pSrc++;
			*pDest = *pSrc;		pDest++;		pSrc++;
			*pDest = *pSrc;		pDest++;		pSrc++;
			*pDest = 255;		pDest++;		pSrc++;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert32_RGBA_to_32_BGRA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;

	CALC_PARAM(32, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = pSrc[2];		pDest++;
			*pDest = pSrc[1];		pDest++;
			*pDest = pSrc[0];		pDest++;
			*pDest = pSrc[3];		pDest++;

			pSrc += 4;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

void i3Image_Convert32_RGBX_to_32_BGRA(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;

	CALC_PARAM(32, 32);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*pDest = pSrc[2];		pDest++;
			*pDest = pSrc[1];		pDest++;
			*pDest = pSrc[0];		pDest++;
			*pDest = 255;		pDest++;

			pSrc += 4;
		}

		pSrc += srcLineGap;
		pDest += destLineGap;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 32 (B G R A)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3Image_Convert32_BGRA_to_16_ARGB_X555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 3) & 0x001F);			pSrc++;
			val |= ((*pSrc & 0x00F8) << 7);			pSrc++;
			val |= ((*pSrc & 0x00F8) << 2);			pSrc += 2;

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_BGRA_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 3) & 0x001F);			pSrc++;			// Blue
			val |= ((*pSrc & 0x00F8) << 2);			pSrc++;			// Green
			val |= ((*pSrc & 0x00F8) << 7);			pSrc++;			// Red
			val |= ((*pSrc & 0x0080) << 8);			pSrc++;			// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_BGRX_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 3) & 0x001F);			pSrc++;			// Blue
			val |= ((*pSrc & 0x00F8) << 2);			pSrc++;			// Green
			val |= ((*pSrc & 0x00F8) << 7);			pSrc += 2;		// Red
			val |= 0x8000;											// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_BGRA_to_16_RGB_565(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 3) & 0x001F);			pSrc++;			// Blue
			val |= ((*pSrc & 0x00FC) << 2);			pSrc++;			// Green		0011 1111
			val |= ((*pSrc & 0x00F8) << 8);			pSrc += 2;		// Red

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_BGRA_to_16_ARGB_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 0;		pSrc++;		// Blue
			val |= ((*pSrc >> 4) & 0x0F) << 4;		pSrc++;		// Green		0011 1111
			val |= ((*pSrc >> 4) & 0x0F) << 8;		pSrc++;		// Red
			val |= ((*pSrc >> 4) & 0x0F) << 12;	pSrc++;		// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_BGRA_to_16_ABGR_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 4;		pSrc++;		// Blue
			val |= ((*pSrc >> 4) & 0x0F) << 8;		pSrc++;		// Green		0011 1111
			val |= ((*pSrc >> 4) & 0x0F) << 12;			pSrc++;		// Red
			val |= ((*pSrc >> 4) & 0x0F) << 0;	pSrc++;		// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_BGRX_to_16_ARGB_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 0;		pSrc++;		// Blue
			val |= ((*pSrc >> 4) & 0x0F) << 4;		pSrc++;		// Green		0011 1111
			val |= ((*pSrc >> 4) & 0x0F) << 8;		pSrc++;		// Red
			val |= 0xF000;							pSrc++;		// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}

void i3Image_Convert32_BGRX_to_16_ABGR_4444(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	INT32 srcLineSz, destLineSz;
	INT32 srcLineGap, destLineGap;
	register INT32 i, j;
	register UINT16 val;

	CALC_PARAM(32, 16);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			val = ((*pSrc >> 4) & 0x0F) << 8;		pSrc++;		// Blue
			val |= ((*pSrc >> 4) & 0x0F) << 4;		pSrc++;		// Green		0011 1111
			val |= ((*pSrc >> 4) & 0x0F);			pSrc++;		// Red
			val |= 0xF000;							pSrc++;		// Alpha

			*pDest = val;
			pDest++;
		}

		pSrc += srcLineGap;
		pDest = (UINT16 *)(((char *)pDest) + destLineGap);
	}
}