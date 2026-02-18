#include "ImageUtilDXT.h"

#define CHAN_MAX 255
#define RCOMP 2
#define GCOMP 1
#define BCOMP 0
#define ACOMP 3

#define EXP5TO8R(packedcol)					\
   ((((packedcol) >> 8) & 0xf8) | (((packedcol) >> 13) & 0x7))

#define EXP6TO8G(packedcol)					\
   ((((packedcol) >> 3) & 0xfc) | (((packedcol) >>  9) & 0x3))

#define EXP5TO8B(packedcol)					\
   ((((packedcol) << 3) & 0xf8) | (((packedcol) >>  2) & 0x7))

#define EXP4TO8(col)						\
   ((col) | ((col) << 4))

static void dxt135_decode_imageblock(const UINT8 *img_block_src,
	INT32 i, INT32 j, INT32 dxt_type, void *texel)
{
	UINT8 *rgba = (UINT8 *)texel;
	const UINT16 color0 = img_block_src[0] | (img_block_src[1] << 8);
	const UINT16 color1 = img_block_src[2] | (img_block_src[3] << 8);
	const INT32 bits = img_block_src[4] | (img_block_src[5] << 8) |
		(img_block_src[6] << 16) | (img_block_src[7] << 24);
	/* What about big/little endian? */
	UINT8 bit_pos = (UINT8)(2 * (j * 4 + i));
	UINT8 code = (UINT8)((bits >> bit_pos) & 3);

	rgba[ACOMP] = CHAN_MAX;
	switch (code) {
	case 0:
		rgba[RCOMP] = (UINT8)(EXP5TO8R(color0));
		rgba[GCOMP] = (UINT8)(EXP6TO8G(color0));
		rgba[BCOMP] = (UINT8)(EXP5TO8B(color0));
		break;
	case 1:
		rgba[RCOMP] = (UINT8)(EXP5TO8R(color1));
		rgba[GCOMP] = (UINT8)(EXP6TO8G(color1));
		rgba[BCOMP] = (UINT8)(EXP5TO8B(color1));
		break;
	case 2:
		if (color0 > color1) {
			rgba[RCOMP] = (UINT8)((((EXP5TO8R(color0) << 1) + EXP5TO8R(color1)) / 3));
			rgba[GCOMP] = (UINT8)((((EXP6TO8G(color0) << 1) + EXP6TO8G(color1)) / 3));
			rgba[BCOMP] = (UINT8)((((EXP5TO8B(color0) << 1) + EXP5TO8B(color1)) / 3));
		}
		else {
			rgba[RCOMP] = (UINT8)(((EXP5TO8R(color0) + EXP5TO8R(color1)) >> 1));
			rgba[GCOMP] = (UINT8)(((EXP6TO8G(color0) + EXP6TO8G(color1)) >> 1));
			rgba[BCOMP] = (UINT8)(((EXP5TO8B(color0) + EXP5TO8B(color1)) >> 1));
		}
		break;
	case 3:
		if ((dxt_type > 1) || (color0 > color1)) {
			rgba[RCOMP] = (UINT8)(((EXP5TO8R(color0) + (EXP5TO8R(color1) << 1)) / 3));
			rgba[GCOMP] = (UINT8)(((EXP6TO8G(color0) + (EXP6TO8G(color1) << 1)) / 3));
			rgba[BCOMP] = (UINT8)(((EXP5TO8B(color0) + (EXP5TO8B(color1) << 1)) / 3));
		}
		else {
			rgba[RCOMP] = 0;
			rgba[GCOMP] = 0;
			rgba[BCOMP] = 0;
			if (dxt_type == 1) rgba[ACOMP] = (0);
		}
		break;
		//default:
		///* CANNOT happen (I hope) */
		//   break;
	}
}

static void dxt135_decode_imageblock_16Bit(const UINT8 *img_block_src,
	INT32 i, INT32 j, INT32 dxt_type, void *texel)
{
	//  UINT16 *rgba = (UINT16 *) texel;
	const UINT16 color0 = img_block_src[0] | (img_block_src[1] << 8);
	const UINT16 color1 = img_block_src[2] | (img_block_src[3] << 8);
	const INT32 bits = img_block_src[4] | (img_block_src[5] << 8) |
		(img_block_src[6] << 16) | (img_block_src[7] << 24);
	/* What about big/little endian? */
	UINT8 bit_pos = (UINT8)(2 * (j * 4 + i));
	UINT8 code = (UINT8)((bits >> bit_pos) & 3);

	UINT16 val = 0;

	val |= ((CHAN_MAX & 0x0080) << 8);
	switch (code) {
	case 0:
		val = (UINT16)((EXP5TO8R(color0) & 0x00F8) << 7);
		val |= (UINT16)((EXP6TO8G(color0) & 0x00F8) << 2);
		val |= (UINT16)((EXP5TO8B(color0) >> 3) & 0x001F);
		break;
	case 1:
		val = (UINT16)((EXP5TO8R(color1) & 0x00F8) << 7);
		val |= (UINT16)((EXP6TO8G(color1) & 0x00F8) << 2);
		val |= (UINT16)((EXP5TO8B(color1) >> 3) & 0x001F);
		if (dxt_type == 1) val |= ((CHAN_MAX & 0x0080) << 8);
		break;
	case 2:
		if (color0 > color1) {
			val = (UINT16)(((((EXP5TO8R(color0) << 1) + EXP5TO8R(color1)) / 3) & 0x00F8) << 7);
			val |= (UINT16)(((((EXP6TO8G(color0) << 1) + EXP6TO8G(color1)) / 3) & 0x00F8) << 2);
			val |= (UINT16)(((((EXP5TO8B(color0) << 1) + EXP5TO8B(color1)) / 3) >> 3) & 0x001F);
		}
		else {
			val = (UINT16)((((EXP5TO8R(color0) + EXP5TO8R(color1)) >> 1) & 0x00F8) << 7);
			val |= (UINT16)((((EXP6TO8G(color0) + EXP6TO8G(color1)) >> 1) & 0x00F8) << 2);
			val |= (UINT16)((((EXP5TO8B(color0) + EXP5TO8B(color1)) >> 1) >> 3) & 0x001F);
		}
		break;
	case 3:
		if ((dxt_type > 1) || (color0 > color1)) {
			val = (UINT16)((((EXP5TO8R(color0) + (EXP5TO8R(color1) << 1)) / 3) & 0x00F8) << 7);
			val |= (UINT16)((((EXP6TO8G(color0) + (EXP6TO8G(color1) << 1)) / 3) & 0x00F8) << 2);
			val |= (UINT16)((((EXP5TO8B(color0) + (EXP5TO8B(color1) << 1)) / 3) >> 3) & 0x001F);
		}
		else {
			val = ((0 & 0x00F8) << 7);
			val |= ((0 & 0x00F8) << 2);
			val |= ((0 >> 3) & 0x001F);
			if (dxt_type == 1) val |= ((0 & 0x0080) << 8);
		}
		break;
	default:
		/* CANNOT happen (I hope) */
		break;
	}

	*((UINT16 *)texel) = val;
}

void fetch_2d_texel_rgb_dxt1(INT32 srcRowStride, const UINT8 *pixdata,
	INT32 i, INT32 j, void *texel, BOOL b32)
{
	/* Extract the (i,j) pixel from pixdata and return it
	* in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
	*/

	const UINT8 *blksrc = (pixdata + (((srcRowStride + 3) >> 2) * (j >> 2) + (i >> 2)) * 8);

	if (b32)
	{
		dxt135_decode_imageblock(blksrc, (i & 3), (j & 3), 0, texel);
	}
	else
	{
		dxt135_decode_imageblock_16Bit(blksrc, (i & 3), (j & 3), 0, texel);
	}
}

void fetch_2d_texel_rgba_dxt1(INT32 srcRowStride, const UINT8 *pixdata,
	INT32 i, INT32 j, void *texel, BOOL b32)
{
	/* Extract the (i,j) pixel from pixdata and return it
	* in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
	*/

	const UINT8 *blksrc = (pixdata + (((srcRowStride + 3) >> 2) * (j >> 2) + (i >> 2)) * 8);

	if (b32)
	{
		dxt135_decode_imageblock(blksrc, (i & 3), (j & 3), 1, texel);
	}
	else
	{
		dxt135_decode_imageblock_16Bit(blksrc, (i & 3), (j & 3), 1, texel);
	}
}

void fetch_2d_texel_rgba_dxt3(INT32 srcRowStride, const UINT8 *pixdata,
	INT32 i, INT32 j, void *texel, BOOL b32)
{

	/* Extract the (i,j) pixel from pixdata and return it
	* in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
	*/

	UINT8 *rgba8 = NULL;
	UINT16 *rgba16 = NULL;

	if (b32)
	{
		rgba8 = (UINT8 *)texel;
	}
	else
	{
		rgba16 = (UINT16 *)texel;
	}

	const UINT8 *blksrc = (pixdata + (((srcRowStride + 3) >> 2) * (j >> 2) + (i >> 2)) * 16);

#if 1
	/* TODO test this! */
	const UINT8 anibble = (UINT8)((blksrc[((j & 3) * 4 + (i & 3)) / 2] >> (4 * (i & 1))) & 0xf);

	if (b32)
	{
		dxt135_decode_imageblock(blksrc + 8, (i & 3), (j & 3), 2, texel);
		rgba8[ACOMP] = (UINT8)(EXP4TO8(anibble));
	}
	else
	{
		dxt135_decode_imageblock_16Bit(blksrc + 8, (i & 3), (j & 3), 2, texel);
		*((UINT16*)texel) |= (UINT16)((EXP4TO8(anibble) & 0x0080) << 8);
	}

#endif

}

void fetch_2d_texel_rgba_dxt5(INT32 srcRowStride, const UINT8 *pixdata,
	INT32 i, INT32 j, void *texel, BOOL b32)
{

	/* Extract the (i,j) pixel from pixdata and return it
	* in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
	*/


	UINT8 *rgba8 = NULL;
	UINT16 *rgba16 = NULL;

	if (b32)
	{
		rgba8 = (UINT8 *)texel;
	}
	else
	{
		rgba16 = (UINT16 *)texel;
	}

	const UINT8 *blksrc = (pixdata + (((srcRowStride + 3) >> 2) * (j >> 2) + (i >> 2)) * 16);
	const UINT8 alpha0 = blksrc[0];
	const UINT8 alpha1 = blksrc[1];

#if 1
	/* TODO test this! */
	const UINT8 bit_pos = (UINT8)(((j & 3) * 4 + (i & 3)) * 3);
	const UINT8 acodelow = blksrc[2 + bit_pos / 8];
	const UINT8 acodehigh = blksrc[3 + bit_pos / 8];
	const UINT8 code = (acodelow >> (bit_pos & 0x7) |
		(acodehigh << (8 - (bit_pos & 0x7)))) & 0x7;
#endif

	if (b32)
	{
		dxt135_decode_imageblock(blksrc + 8, (i & 3), (j & 3), 2, texel);
	}
	else
	{
		dxt135_decode_imageblock_16Bit(blksrc + 8, (i & 3), (j & 3), 2, texel);
	}

	/* not sure. Which version is faster? */
#if 1
	/* TODO test this */
	if (b32)
	{
		if (code == 0)
			rgba8[ACOMP] = (UINT8)(alpha0);
		else if (code == 1)
			rgba8[ACOMP] = (UINT8)(alpha1);
		else if (alpha0 > alpha1)
			rgba8[ACOMP] = (UINT8)(((alpha0 * (8 - code) + (alpha1 * (code - 1))) / 7));
		else if (code < 6)
			rgba8[ACOMP] = (UINT8)(((alpha0 * (6 - code) + (alpha1 * (code - 1))) / 5));
		else if (code == 6)
			rgba8[ACOMP] = 0;
		else
			rgba8[ACOMP] = CHAN_MAX;
	}
	else
	{
		if (code == 0)
			*rgba16 |= (UINT8)((alpha0 & 0x0080) << 8);
		else if (code == 1)
			*rgba16 |= (UINT8)((alpha1 & 0x0080) << 8);
		else if (alpha0 > alpha1)
			*rgba16 |= (UINT8)((((alpha0 * (8 - code) + (alpha1 * (code - 1))) / 7) & 0x0080) << 8);
		else if (code < 6)
			*rgba16 |= (UINT8)((((alpha0 * (6 - code) + (alpha1 * (code - 1))) / 5) & 0x0080) << 8);
		else if (code == 6)
			*rgba16 |= 0;
		else
			*rgba16 |= CHAN_MAX;
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DXT
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DXT1_X
void i3Image_ConvertDXT1_X_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	register INT32 i, j;
	INT32 nStride = 0;

	nStride = width;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			fetch_2d_texel_rgb_dxt1(nStride, pSrc, j, i, pDest, TRUE);
			pDest += 4;
		}
	}
}
void i3Image_ConvertDXT1_X_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	register INT32 i, j;
	INT32 nStride = 0;

	nStride = width;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			fetch_2d_texel_rgb_dxt1(nStride, pSrc, j, i, pDest, FALSE);
			pDest++;
		}
	}
}

// DXT1
void i3Image_ConvertDXT1_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	register INT32 i, j;
	INT32 nStride = 0;

	nStride = width;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			fetch_2d_texel_rgba_dxt1(nStride, pSrc, j, i, pDest, TRUE);
			pDest += 4;
		}
	}
}
void i3Image_ConvertDXT1_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	register INT32 i, j;
	INT32 nStride = 0;

	nStride = width;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			fetch_2d_texel_rgba_dxt1(nStride, pSrc, j, i, pDest, FALSE);
			pDest++;
		}
	}
}

// DXT2
void i3Image_ConvertDXT2_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{

}
void i3Image_ConvertDXT2_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{

}

// DXT3
void i3Image_ConvertDXT3_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	register INT32 i, j;
	INT32 nStride = width;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			fetch_2d_texel_rgba_dxt3(nStride, pSrc, j, i, pDest, TRUE);
			pDest += 4;
		}
	}
}
void i3Image_ConvertDXT3_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	register INT32 i, j;
	INT32 nStride = 0;

	nStride = width;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			fetch_2d_texel_rgba_dxt3(nStride, pSrc, j, i, pDest, FALSE);
			pDest++;
		}
	}
}

// DXT4
void i3Image_ConvertDXT4_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{

}
void i3Image_ConvertDXT4_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{

}

// DXT5
void i3Image_ConvertDXT5_to_RGBA_8888(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
{
	register INT32 i, j;
	INT32 nStride = 0;

	nStride = width;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			fetch_2d_texel_rgba_dxt5(nStride, pSrc, j, i, pDest, TRUE);
			pDest += 4;
		}
	}
}
void i3Image_ConvertDXT5_to_16_ARGB_1555(UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
{
	register INT32 i, j;
	INT32 nStride = 0;

	nStride = width;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			fetch_2d_texel_rgba_dxt5(nStride, pSrc, j, i, pDest, FALSE);
			pDest++;
		}
	}
}

