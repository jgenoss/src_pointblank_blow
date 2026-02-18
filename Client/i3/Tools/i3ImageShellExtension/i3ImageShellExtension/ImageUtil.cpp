#include "ImageUtil.h"

void i3Image_Convert(I3G_IMAGE_FORMAT srcFormat, char * pSrc, INT32 W, INT32 H, INT32 srcAlign,
	I3G_IMAGE_FORMAT destFormat, char * pDest, INT32 destAlign)
{
	srcFormat = (I3G_IMAGE_FORMAT)(srcFormat & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB));
	destFormat = (I3G_IMAGE_FORMAT)(destFormat & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB));

	switch (srcFormat)
	{
		// 16 bit formats
	case I3G_IMAGE_FORMAT_RGB_565:
		switch (destFormat)
		{
			// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert16_X555_to_16_X555((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert16_565_to_16_X555((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert16_565_to_16_1555((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert16_565_to_24_RGB((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);		break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert16_565_to_24_BGR((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);		break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert16_565_to_32_RGBA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert16_565_to_32_BGRA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert16_565_to_32_RGBA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert16_565_to_32_BGRA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_XRGB_1555:
		switch (destFormat)
		{
			// 16 bit formats				  
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert16_X555_to_16_565((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert16_X555_to_16_X555((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert16_X555_to_16_1555((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert16_X555_to_24_RGB((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert16_X555_to_24_BGR((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert16_X555_to_32_RGBA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert16_X555_to_32_BGRA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert16_X555_to_32_RGBA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert16_X555_to_32_BGRA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_ARGB_1555:
		switch (destFormat)
		{
			// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert16_X555_to_16_565((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert16_X555_to_16_X555((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert16_X555_to_16_X555((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert16_X555_to_24_RGB((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert16_X555_to_24_BGR((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert16_1555_to_32_RGBA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert16_1555_to_32_BGRA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert16_1555_to_32_RGBA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert16_1555_to_32_BGRA((UINT16 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_ARGB_4444:

	case I3G_IMAGE_FORMAT_ABGR_4444:
		switch (destFormat)
		{
		case I3G_IMAGE_FORMAT_ABGR_4444:	i3Image_Convert16_ABGR_4444_to_16_ABGR_4444((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_4444:	i3Image_Convert16_ABGR_4444_to_16_ARGB_4444((UINT16 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

		// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888:
		switch (destFormat)
		{
			// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert24_RGB_to_16_RGB_565((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert24_RGB_to_16_ARGB_X555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert24_RGB_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_4444: i3Image_Convert24_RGB_to_16_ARGB_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ABGR_4444: i3Image_Convert24_RGB_to_16_ABGR_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert24_RGB_to_24_RGB((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);		break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert24_RGB_to_24_BGR((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);		break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert24_RGB_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert24_RGB_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert24_RGB_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert24_RGB_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_BGR_888:
		switch (destFormat)
		{
			// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert24_BGR_to_16_RGB_565((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert24_BGR_to_16_ARGB_X555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert24_BGR_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_4444: i3Image_Convert24_BGR_to_16_ARGB_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ABGR_4444: i3Image_Convert24_BGR_to_16_ABGR_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XBGR_4444: i3Image_Convert24_BGR_to_16_ABGR_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert24_RGB_to_24_BGR((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);		break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert24_RGB_to_24_RGB((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);		break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert24_BGR_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert24_BGR_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert24_BGR_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert24_BGR_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

		// 32 bit formats
	case I3G_IMAGE_FORMAT_RGBX_8888:
		switch (destFormat)
		{
			// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert32_RGBA_to_16_RGB_565((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert32_RGBA_to_16_ARGB_X555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert32_RGBX_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_4444: i3Image_Convert32_RGBX_to_16_ARGB_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ABGR_4444: i3Image_Convert32_RGBX_to_16_ABGR_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert32_RGBA_to_24_RGB((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert32_RGBA_to_24_BGR((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert32_RGBX_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert32_RGBX_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert32_RGBX_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert32_RGBX_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_BGRX_8888:
		switch (destFormat)
		{
			// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert32_BGRA_to_16_RGB_565((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert32_BGRA_to_16_ARGB_X555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert32_BGRX_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_4444: i3Image_Convert32_BGRX_to_16_ARGB_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ABGR_4444: i3Image_Convert32_BGRX_to_16_ABGR_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert32_RGBA_to_24_BGR((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert32_RGBA_to_24_RGB((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert32_RGBA_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert32_RGBA_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert32_RGBX_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert32_RGBX_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_RGBA_8888:
		switch (destFormat)
		{
			// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert32_RGBA_to_16_RGB_565((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert32_RGBA_to_16_ARGB_X555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert32_RGBA_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_4444: i3Image_Convert32_RGBA_to_16_ARGB_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ABGR_4444: i3Image_Convert32_RGBA_to_16_ABGR_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert32_RGBA_to_24_RGB((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert32_RGBA_to_24_BGR((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert32_RGBA_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert32_RGBA_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert32_RGBA_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert32_RGBA_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;

		}
		break;

	case I3G_IMAGE_FORMAT_BGRA_8888:
		switch (destFormat)
		{
			// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565: i3Image_Convert32_BGRA_to_16_RGB_565((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_XRGB_1555: i3Image_Convert32_BGRA_to_16_ARGB_X555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_Convert32_BGRA_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_4444: i3Image_Convert32_BGRA_to_16_ARGB_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ABGR_4444: i3Image_Convert32_BGRA_to_16_ABGR_4444((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

			// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888: i3Image_Convert32_RGBA_to_24_BGR((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGR_888: i3Image_Convert32_RGBA_to_24_RGB((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

			// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888: i3Image_Convert32_RGBA_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRX_8888: i3Image_Convert32_RGBA_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_Convert32_RGBA_to_32_BGRA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_BGRA_8888: i3Image_Convert32_RGBA_to_32_RGBA((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_DXT1_X:
		switch (destFormat)
		{
			// 32 bit formats
		case I3G_IMAGE_FORMAT_BGRX_8888:
		case I3G_IMAGE_FORMAT_BGRA_8888:
		case I3G_IMAGE_FORMAT_RGBX_8888:
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_ConvertDXT1_X_to_RGBA_8888((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;

		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_ConvertDXT1_X_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_DXT1:
		switch (destFormat)
		{
			// 32 bit formats
		case I3G_IMAGE_FORMAT_BGRX_8888:
		case I3G_IMAGE_FORMAT_BGRA_8888:
		case I3G_IMAGE_FORMAT_RGBX_8888:
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_ConvertDXT1_to_RGBA_8888((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_ConvertDXT1_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_DXT2:
		switch (destFormat)
		{
			// 32 bit formats
		case I3G_IMAGE_FORMAT_BGRX_8888:
		case I3G_IMAGE_FORMAT_BGRA_8888:
		case I3G_IMAGE_FORMAT_RGBX_8888:
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_ConvertDXT2_to_RGBA_8888((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_ConvertDXT2_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_DXT3:
		switch (destFormat)
		{
			// 32 bit formats
		case I3G_IMAGE_FORMAT_BGRX_8888:
		case I3G_IMAGE_FORMAT_BGRA_8888:
		case I3G_IMAGE_FORMAT_RGBX_8888:
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_ConvertDXT3_to_RGBA_8888((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_ConvertDXT3_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_DXT4:
		switch (destFormat)
		{
			// 32 bit formats
		case I3G_IMAGE_FORMAT_BGRX_8888:
		case I3G_IMAGE_FORMAT_BGRA_8888:
		case I3G_IMAGE_FORMAT_RGBX_8888:
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_ConvertDXT4_to_RGBA_8888((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_ConvertDXT4_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	case I3G_IMAGE_FORMAT_DXT5:
		switch (destFormat)
		{
			// 32 bit formats
		case I3G_IMAGE_FORMAT_BGRX_8888:
		case I3G_IMAGE_FORMAT_BGRA_8888:
		case I3G_IMAGE_FORMAT_RGBX_8888:
		case I3G_IMAGE_FORMAT_RGBA_8888: i3Image_ConvertDXT5_to_RGBA_8888((UINT8 *)pSrc, W, H, srcAlign, (UINT8 *)pDest, destAlign);	break;
		case I3G_IMAGE_FORMAT_ARGB_1555: i3Image_ConvertDXT5_to_16_ARGB_1555((UINT8 *)pSrc, W, H, srcAlign, (UINT16 *)pDest, destAlign);	break;

		default:
			break;
		}
		break;

	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Raster Format
//////////////////////////////////////////////////////////////////////////////
D3DFORMAT	i3DXUT_GetNativeImageFormat(I3G_IMAGE_FORMAT format, bool *bExact)
{
#define SET_NOTEXACT { if( bExact ) *bExact = false; }

	if (bExact) *bExact = true;

	switch (format & ~(I3G_IMAGE_FORMAT_MASK_ZLIB | I3G_IMAGE_FORMAT_MASK_PLATFORM))
	{
	case I3G_IMAGE_FORMAT_DEFAULT_COLOR:		return D3DFMT_A8R8G8B8;

		// 4 bit formats
		// DirectX에서는 Index Image는 256까지만 지원하기 때문에
		// 4 Bits Image라고 하더라도 8 Bits Image로 생성한다.
	case I3G_IMAGE_FORMAT_I4: SET_NOTEXACT; return D3DFMT_P8; // D3DFMT_P4;

		// 8bit formats
	case I3G_IMAGE_FORMAT_I8:	return D3DFMT_P8;
	case i3G_IMAGE_FORMAT_L8:	return D3DFMT_L8;
	case i3G_IMAGE_FORMAT_A8:	return D3DFMT_A8;

		// 16 bit formats
	case I3G_IMAGE_FORMAT_BGR_565:
	case I3G_IMAGE_FORMAT_RGB_565:	return D3DFMT_R5G6B5;
	case I3G_IMAGE_FORMAT_XRGB_1555:	return D3DFMT_X1R5G5B5;
	case I3G_IMAGE_FORMAT_ARGB_1555:	return D3DFMT_A1R5G5B5;
	case I3G_IMAGE_FORMAT_XRGB_4444:	return D3DFMT_X4R4G4B4;
	case I3G_IMAGE_FORMAT_ARGB_4444:	return D3DFMT_A4R4G4B4;
	case I3G_IMAGE_FORMAT_ABGR_4444:	return D3DFMT_A4R4G4B4;

		// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888:	SET_NOTEXACT; return D3DFMT_R8G8B8;
	case I3G_IMAGE_FORMAT_BGR_888:	return D3DFMT_R8G8B8; // D3DFMT_B8G8R8;

		// 32 bit formats
	case I3G_IMAGE_FORMAT_RGBX_8888:	SET_NOTEXACT; return D3DFMT_X8R8G8B8;
	case I3G_IMAGE_FORMAT_BGRX_8888:	return D3DFMT_X8R8G8B8; // D3DFMT_X8B8G8R8;
	case I3G_IMAGE_FORMAT_RGBA_8888:	SET_NOTEXACT; return D3DFMT_A8R8G8B8;
	case I3G_IMAGE_FORMAT_BGRA_8888:	return D3DFMT_A8R8G8B8;

		// -------------------------
		// Bump-map
		// -------------------------
	case I3G_IMAGE_FORMAT_VU_88:		return D3DFMT_V8U8;
	case I3G_IMAGE_FORMAT_QWVU_8888:	return D3DFMT_Q8W8V8U8;

		// -------------------------
		// Compressed formats
		// -------------------------
	case I3G_IMAGE_FORMAT_DXT1_X:
	{
		return D3DFMT_DXT1;
	}
	case I3G_IMAGE_FORMAT_DXT1:
	{
		return D3DFMT_DXT1;
	}
	case I3G_IMAGE_FORMAT_DXT2:
	{
		return D3DFMT_DXT2;
	}
	case I3G_IMAGE_FORMAT_DXT3:
	{
		return D3DFMT_DXT3;
	}
	case I3G_IMAGE_FORMAT_DXT4:
	{
		return D3DFMT_DXT4;
	}
	case I3G_IMAGE_FORMAT_DXT5:
	{
		return D3DFMT_DXT5;
	}

	// -------------------------
	// HDR
	// -------------------------
	case I3G_IMAGE_FORMAT_ABGR_16F:	return D3DFMT_A16B16G16R16F;
	case I3G_IMAGE_FORMAT_ABGR_32F:	return D3DFMT_A32B32G32R32F;
	case I3G_IMAGE_FORMAT_ABGR_16:		return D3DFMT_A16B16G16R16;
	case I3G_IMAGE_FORMAT_GR_16F:		return D3DFMT_G16R16F;
	case I3G_IMAGE_FORMAT_R_32F:		return D3DFMT_R32F;
	case I3G_IMAGE_FORMAT_GR_32F:		return D3DFMT_G32R32F;

		// -------------------------
		// depth, stencil, vertex buffers
		// -------------------------
	case I3G_IMAGE_FORMAT_D16:			return D3DFMT_D16;
	case I3G_IMAGE_FORMAT_D24S8:		return D3DFMT_D24S8;
	case I3G_IMAGE_FORMAT_D24X8:		return D3DFMT_D24X8;
	case I3G_IMAGE_FORMAT_D32:			return D3DFMT_D32;
	case I3G_IMAGE_FORMAT_D32F:		return D3DFMT_D32F_LOCKABLE;
	case I3G_IMAGE_FORMAT_D24FS8:		return D3DFMT_D24FS8;
	default:
		break;
	}

	return D3DFMT_UNKNOWN;
}

I3G_IMAGE_FORMAT	i3DXUT_GetI3ImageFormat(D3DFORMAT format, bool *bExact)
{
#define SET_NOTEXACT { if( bExact ) *bExact = false; }

	if (bExact) *bExact = true;

	switch (format)
	{
	case D3DFMT_P8:				return I3G_IMAGE_FORMAT_I8;
	case D3DFMT_X1R5G5B5:		return I3G_IMAGE_FORMAT_XRGB_1555;
	case D3DFMT_A1R5G5B5:		return I3G_IMAGE_FORMAT_ARGB_1555;
	case D3DFMT_R5G6B5:			return I3G_IMAGE_FORMAT_RGB_565;
	case D3DFMT_R8G8B8:			return I3G_IMAGE_FORMAT_BGR_888;
	case D3DFMT_A4R4G4B4:		return I3G_IMAGE_FORMAT_ARGB_4444;
	case D3DFMT_X8R8G8B8:		return I3G_IMAGE_FORMAT_BGRX_8888;
	case D3DFMT_A8R8G8B8:		return I3G_IMAGE_FORMAT_BGRA_8888;
	case D3DFMT_DXT1:			return I3G_IMAGE_FORMAT_DXT1;
	case D3DFMT_DXT2:			return I3G_IMAGE_FORMAT_DXT2;
	case D3DFMT_DXT3:			return I3G_IMAGE_FORMAT_DXT3;
	case D3DFMT_DXT4:			return I3G_IMAGE_FORMAT_DXT4;
	case D3DFMT_DXT5:			return I3G_IMAGE_FORMAT_DXT5;
	case D3DFMT_A16B16G16R16F:	return I3G_IMAGE_FORMAT_ABGR_16F;
	case D3DFMT_A32B32G32R32F:	return I3G_IMAGE_FORMAT_ABGR_32F;
	case D3DFMT_A16B16G16R16:	return I3G_IMAGE_FORMAT_ABGR_16;
	case D3DFMT_G16R16F:		return I3G_IMAGE_FORMAT_GR_16F;
	case D3DFMT_R32F:			return I3G_IMAGE_FORMAT_R_32F;
	case D3DFMT_G32R32F:		return I3G_IMAGE_FORMAT_GR_32F;

	case D3DFMT_D16:			return I3G_IMAGE_FORMAT_D16;
	case D3DFMT_D24S8:			return I3G_IMAGE_FORMAT_D24S8;
	case D3DFMT_D24X8:			return I3G_IMAGE_FORMAT_D24X8;
	case D3DFMT_D32:			return I3G_IMAGE_FORMAT_D32;
	case D3DFMT_D32F_LOCKABLE:	return I3G_IMAGE_FORMAT_D32F;
	case D3DFMT_D24FS8:		return I3G_IMAGE_FORMAT_D24FS8;
	}

	return I3G_IMAGE_FORMAT_NONE;
}


