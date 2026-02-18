#include "ImageUtilCalc.h"


INT32 CalcRasterBufferSize(INT32 width, INT32 height, INT32 alignbits, I3G_IMAGE_FORMAT format)
{
	INT32 bits, sp, Result;

	{
		// Uncompressed Format
		bits = GetImageFormatBitCount(format);

		bits *= width;

		sp = (bits + alignbits - 1) >> alignbits;

		sp = (sp << alignbits) >> 3;		// byte 단위

		if ((sp <= 0) || (bits < 8))
			Result = height;				// Minimum 1 byte는 차지하도록 예외처리..
		else
			Result = sp * height;
	}

	// DXT1
	switch (format)
	{
	case I3G_IMAGE_FORMAT_DXT1_X:
	case I3G_IMAGE_FORMAT_DXT1:
		if (Result < 8)		Result = 8;				// DXT1 형식에서는 항상 8 bytes 이상의 크기를 가진다.
		break;

	case I3G_IMAGE_FORMAT_DXT2:
	case I3G_IMAGE_FORMAT_DXT3:
	case I3G_IMAGE_FORMAT_DXT4:
	case I3G_IMAGE_FORMAT_DXT5:
		if (Result < 16)	Result = 16;			// DXT2~5 형식에서는 Minumum 16 bytes이다.
		break;
	}

	return Result;
}

INT32 GetImageFormatBitCount(I3G_IMAGE_FORMAT format)
{
	format = I3G_IMAGE_FORMAT(format & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB));

	switch (format)
	{
		// 4 bit formats
	case I3G_IMAGE_FORMAT_I4:	return 4;

		// 8bit formats
	case I3G_IMAGE_FORMAT_I8:
	case i3G_IMAGE_FORMAT_L8:
	case i3G_IMAGE_FORMAT_A8:	return 8;

		// 16 bit formats
	case I3G_IMAGE_FORMAT_RGB_565:
	case I3G_IMAGE_FORMAT_XRGB_1555:
	case I3G_IMAGE_FORMAT_ARGB_1555:
	case I3G_IMAGE_FORMAT_ABGR_4444:
	case I3G_IMAGE_FORMAT_XBGR_4444:
	case I3G_IMAGE_FORMAT_XRGB_4444:
	case I3G_IMAGE_FORMAT_ARGB_4444:	return 16;

		// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888:
	case I3G_IMAGE_FORMAT_BGR_888:	return 24;

		// 32 bit format
	case I3G_IMAGE_FORMAT_RGBX_8888:
	case I3G_IMAGE_FORMAT_BGRX_8888:
	case I3G_IMAGE_FORMAT_RGBA_8888:
	case I3G_IMAGE_FORMAT_BGRA_8888:	return 32;

		// -------------------------
		// Bump-map
		// -------------------------
	case I3G_IMAGE_FORMAT_VU_88:	return 16;
	case I3G_IMAGE_FORMAT_QWVU_8888: return 32;

		// -------------------------
		// Compressed formats
		// -------------------------
	case I3G_IMAGE_FORMAT_DXT1_X:
	case I3G_IMAGE_FORMAT_DXT1:		return 4;

	case I3G_IMAGE_FORMAT_DXT2:
	case I3G_IMAGE_FORMAT_DXT3:
	case I3G_IMAGE_FORMAT_DXT4:
	case I3G_IMAGE_FORMAT_DXT5:		return 8;

		// -------------------------
		// HDR
		// -------------------------
	case I3G_IMAGE_FORMAT_ABGR_16F:	return 64;
	case I3G_IMAGE_FORMAT_ABGR_32F:	return 128;
	case I3G_IMAGE_FORMAT_ABGR_16:		return 64;
	case I3G_IMAGE_FORMAT_GR_16F:		return 32;
	case I3G_IMAGE_FORMAT_R_32F:		return 32;
	case I3G_IMAGE_FORMAT_GR_32F:		return 64;

		// -------------------------
		// depth, stencil, vertex buffers
		// -------------------------
	case I3G_IMAGE_FORMAT_D16:	return 16;

	case I3G_IMAGE_FORMAT_D24S8:
	case I3G_IMAGE_FORMAT_D24X8:
	case I3G_IMAGE_FORMAT_D32F:
	case I3G_IMAGE_FORMAT_D24FS8:
	case I3G_IMAGE_FORMAT_D32:		return 32;

	default:
		return -1;
	}
}
