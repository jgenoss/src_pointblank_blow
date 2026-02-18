#include "i3GfxType.h"
#include "i3GfxUtil.h"
#include "i3ImageUtil4Bits.h"
#include "i3ImageUtil8Bits.h"
#include "i3ImageUtil16Bits.h"
#include "i3ImageUtil24Bits.h"
#include "i3ImageUtil32Bits.h"
#include "i3ImageUtilHDR.h"
#include "i3ImageUtilDXT.h"

I3_EXPORT_GFX 
void i3Image_Convert( I3G_IMAGE_FORMAT srcFormat, char * pSrc, INT32 W, INT32 H, INT32 srcAlign, i3Clut * pSrcClut,
					  I3G_IMAGE_FORMAT destFormat, char * pDest, INT32 destAlign, i3Clut * pDestClut)
{
	srcFormat = (I3G_IMAGE_FORMAT)(srcFormat & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB));
	destFormat = (I3G_IMAGE_FORMAT)(destFormat & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB));

	switch( srcFormat)
	{
		case I3G_IMAGE_FORMAT_I4		:   
			switch( destFormat)
			{
				case I3G_IMAGE_FORMAT_I4		: i3Image_Convert4to4( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_I8		: i3Image_Convert4to8( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign, pDestClut);	break;

				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert4to16_RGB_565( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert4to16_ARGB_X555( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert4to16_ARGB_1555( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : i3Image_Convert4to16_ARGB_4444( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444 : i3Image_Convert4to16_ABGR_4444( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert4to24_RGB( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert4to24_BGR( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert4to32_RGBX( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert4to32_BGRX( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert4to32_RGBA( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert4to32_BGRA( pSrc, W, H, srcAlign, pSrcClut, pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 4 bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_I8		: 
			switch( destFormat)
			{
				case I3G_IMAGE_FORMAT_I4		: i3Image_Convert8to4( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign, pDestClut);	break;
				case I3G_IMAGE_FORMAT_I8		: i3Image_Convert8to8( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign, pDestClut);	break;

				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert8to16_RGB_565( (UINT8*) pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert8to16_ARGB_X555( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert8to16_ARGB_1555( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : i3Image_Convert8to16_ARGB_4444( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: i3Image_Convert8to16_ABGR_4444( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert8to24_RGB( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert8to24_BGR( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert8to32_RGBX( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert8to32_BGRX( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert8to32_RGBA( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert8to32_BGRA( (UINT8*)pSrc, W, H, srcAlign, pSrcClut, (UINT8*)pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 8 bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565	: 
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert16_X555_to_16_X555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert16_565_to_16_X555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert16_565_to_16_1555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert16_565_to_24_RGB( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);		break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert16_565_to_24_BGR( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);		break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert16_565_to_32_RGBA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert16_565_to_32_BGRA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert16_565_to_32_RGBA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert16_565_to_32_BGRA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 16(565) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_XRGB_1555	: 
			switch( destFormat)
			{
				// 16 bit formats				  
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert16_X555_to_16_565( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert16_X555_to_16_X555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert16_X555_to_16_1555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert16_X555_to_24_RGB( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert16_X555_to_24_BGR( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert16_X555_to_32_RGBA( (UINT16 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert16_X555_to_32_BGRA( (UINT16 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert16_X555_to_32_RGBA( (UINT16 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert16_X555_to_32_BGRA( (UINT16 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 16(X555) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_ARGB_1555	: 
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert16_X555_to_16_565( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert16_X555_to_16_X555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert16_X555_to_16_X555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert16_X555_to_24_RGB( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert16_X555_to_24_BGR( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert16_1555_to_32_RGBA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert16_1555_to_32_BGRA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert16_1555_to_32_RGBA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert16_1555_to_32_BGRA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 16(1555) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_ARGB_4444 :

		case I3G_IMAGE_FORMAT_ABGR_4444 :
			switch( destFormat)
			{
				// 16 bit formats
				//case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert16_X555_to_16_565( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				//case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert16_X555_to_16_X555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				//case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert16_X555_to_16_X555( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444 :	i3Image_Convert16_ABGR_4444_to_16_ABGR_4444( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 :	i3Image_Convert16_ABGR_4444_to_16_ARGB_4444( (UINT16 *)pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				//case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert16_X555_to_24_RGB( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				//case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert16_X555_to_24_BGR( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				// 32 bit formats
				//case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert16_1555_to_32_RGBA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				//case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert16_1555_to_32_BGRA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				//case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert16_1555_to_32_RGBA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				//case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert16_1555_to_32_BGRA( (UINT16 *)pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 16(1555) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888	: 
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert24_RGB_to_16_RGB_565( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert24_RGB_to_16_ARGB_X555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert24_RGB_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : i3Image_Convert24_RGB_to_16_ARGB_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: i3Image_Convert24_RGB_to_16_ABGR_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert24_RGB_to_24_RGB( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);		break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert24_RGB_to_24_BGR( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);		break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert24_RGB_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert24_RGB_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert24_RGB_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert24_RGB_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 24(RGB) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_BGR_888	: 
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert24_BGR_to_16_RGB_565( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert24_BGR_to_16_ARGB_X555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert24_BGR_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : i3Image_Convert24_BGR_to_16_ARGB_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: i3Image_Convert24_BGR_to_16_ABGR_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XBGR_4444 : i3Image_Convert24_BGR_to_16_ABGR_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert24_RGB_to_24_BGR( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);		break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert24_RGB_to_24_RGB( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);		break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert24_BGR_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert24_BGR_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert24_BGR_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert24_BGR_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 24(BGR) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		// 32 bit formats
		case I3G_IMAGE_FORMAT_RGBX_8888	: 
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert32_RGBA_to_16_RGB_565( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert32_RGBA_to_16_ARGB_X555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert32_RGBX_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : i3Image_Convert32_RGBX_to_16_ARGB_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444 : i3Image_Convert32_RGBX_to_16_ABGR_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert32_RGBA_to_24_RGB( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert32_RGBA_to_24_BGR( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert32_RGBX_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert32_RGBX_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert32_RGBX_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert32_RGBX_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 32(XRGB) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_BGRX_8888	: 
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert32_BGRA_to_16_RGB_565( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert32_BGRA_to_16_ARGB_X555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert32_BGRX_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : i3Image_Convert32_BGRX_to_16_ARGB_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: i3Image_Convert32_BGRX_to_16_ABGR_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert32_RGBA_to_24_BGR( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert32_RGBA_to_24_RGB( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert32_RGBA_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert32_RGBA_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert32_RGBX_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert32_RGBX_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 32(XBGR) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_RGBA_8888	: 
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert32_RGBA_to_16_RGB_565( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert32_RGBA_to_16_ARGB_X555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert32_RGBA_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : i3Image_Convert32_RGBA_to_16_ARGB_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444 : i3Image_Convert32_RGBA_to_16_ABGR_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert32_RGBA_to_24_RGB( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert32_RGBA_to_24_BGR( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert32_RGBA_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert32_RGBA_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert32_RGBA_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert32_RGBA_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					i3Log( "i3Image_Convert()", "Source is 32(RGBA) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;

			}
			break;

		case I3G_IMAGE_FORMAT_BGRA_8888	: 
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: i3Image_Convert32_BGRA_to_16_RGB_565( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: i3Image_Convert32_BGRA_to_16_ARGB_X555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_Convert32_BGRA_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : i3Image_Convert32_BGRA_to_16_ARGB_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: i3Image_Convert32_BGRA_to_16_ABGR_4444( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert32_RGBA_to_24_BGR( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGR_888	: i3Image_Convert32_RGBA_to_24_RGB( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: i3Image_Convert32_RGBA_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: i3Image_Convert32_RGBA_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_Convert32_RGBA_to_32_BGRA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert32_RGBA_to_32_RGBA( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is 32(BGRA) bits image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		// HDR Format
		case I3G_IMAGE_FORMAT_ABGR_16F :
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: break;
				case I3G_IMAGE_FORMAT_BGR_888	: break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is HDR image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_ABGR_32F:
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: i3Image_Convert32F_ABGR_to_24_RGB_888((REAL32*) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign); break;
				case I3G_IMAGE_FORMAT_BGR_888	: break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: i3Image_Convert32F_ABGR_to_32_BGRA_8888((REAL32*) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign); break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is HDR image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_ABGR_16 :
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: break;
				case I3G_IMAGE_FORMAT_BGR_888	: break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is HDR image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_GR_16F :
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: break;
				case I3G_IMAGE_FORMAT_BGR_888	: break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is HDR image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_R_32F :
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: break;
				case I3G_IMAGE_FORMAT_BGR_888	: break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is HDR image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_GR_32F :
			switch( destFormat)
			{
				// 16 bit formats
				case I3G_IMAGE_FORMAT_RGB_565	: break;
				case I3G_IMAGE_FORMAT_XRGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: break;
				case I3G_IMAGE_FORMAT_ARGB_4444 : break;
				case I3G_IMAGE_FORMAT_ABGR_4444	: break;

				// 24 bit formats
				case I3G_IMAGE_FORMAT_RGB_888	: break;
				case I3G_IMAGE_FORMAT_BGR_888	: break;

				// 32 bit formats
				case I3G_IMAGE_FORMAT_RGBX_8888	: break;
				case I3G_IMAGE_FORMAT_BGRX_8888	: break;
				case I3G_IMAGE_FORMAT_RGBA_8888	: break;
				case I3G_IMAGE_FORMAT_BGRA_8888	: break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is HDR image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_DXT1_X :
			switch( destFormat)
			{
				// 32 bit formats
				case I3G_IMAGE_FORMAT_BGRX_8888 :
				case I3G_IMAGE_FORMAT_BGRA_8888 :
				case I3G_IMAGE_FORMAT_RGBX_8888 :
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_ConvertDXT1_X_to_RGBA_8888( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;

				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_ConvertDXT1_X_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is DXT1_X image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_DXT1 :
			switch( destFormat)
			{
				// 32 bit formats
				case I3G_IMAGE_FORMAT_BGRX_8888 :
				case I3G_IMAGE_FORMAT_BGRA_8888 :
				case I3G_IMAGE_FORMAT_RGBX_8888 :
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_ConvertDXT1_to_RGBA_8888( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_ConvertDXT1_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is DXT1 image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_DXT2 :
			switch( destFormat)
			{
				// 32 bit formats
				case I3G_IMAGE_FORMAT_BGRX_8888 :
				case I3G_IMAGE_FORMAT_BGRA_8888 :
				case I3G_IMAGE_FORMAT_RGBX_8888 :
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_ConvertDXT2_to_RGBA_8888( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_ConvertDXT2_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is DXT2 image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_DXT3 :
			switch( destFormat)
			{
				// 32 bit formats
				case I3G_IMAGE_FORMAT_BGRX_8888 :
				case I3G_IMAGE_FORMAT_BGRA_8888 :
				case I3G_IMAGE_FORMAT_RGBX_8888 :
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_ConvertDXT3_to_RGBA_8888( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_ConvertDXT3_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is DXT3 image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_DXT4 :
			switch( destFormat)
			{
				// 32 bit formats
				case I3G_IMAGE_FORMAT_BGRX_8888 :
				case I3G_IMAGE_FORMAT_BGRA_8888 :
				case I3G_IMAGE_FORMAT_RGBX_8888 :
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_ConvertDXT4_to_RGBA_8888( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_ConvertDXT4_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is DXT4 image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		case I3G_IMAGE_FORMAT_DXT5 :
			switch( destFormat)
			{
				// 32 bit formats
				case I3G_IMAGE_FORMAT_BGRX_8888 :
				case I3G_IMAGE_FORMAT_BGRA_8888 :
				case I3G_IMAGE_FORMAT_RGBX_8888 :
				case I3G_IMAGE_FORMAT_RGBA_8888	: i3Image_ConvertDXT5_to_RGBA_8888( (UINT8 *) pSrc, W, H, srcAlign, (UINT8 *) pDest, destAlign);	break;
				case I3G_IMAGE_FORMAT_ARGB_1555	: i3Image_ConvertDXT5_to_16_ARGB_1555( (UINT8 *) pSrc, W, H, srcAlign, (UINT16 *) pDest, destAlign);	break;

				default :
				#if defined( I3_DEBUG)
					I3TRACE1( "i3Image_Convert() - Source is DXT5 image, but destination format is invalid : %d\n", destFormat);
				#endif
					break;
			}
			break;

		default :
			break;
	}
}

// 일반모드 텍스쳐를 PSP의 고속모드( Swizzled ) 포맷으로 만들기.
// pDest : 대상 버퍼
// pSrc  : 원본 버퍼
// nBytesPerLine  : 버퍼의 가로 사이즈. BYTE 단위.
// nLine  : 버퍼의 세로 사이즈. nLine
I3_EXPORT_GFX 
bool i3ImageConvertLinearToSwizzled(char* pDest, char* pSrc, UINT32 nBytesPerLine, UINT32 nLine, I3_PLATFORM Platform )
{
	// 현재는 PSP의 Swizzled 포맷만 지원함.
	I3ASSERT( Platform == I3_PLATFORM_PSP );

	UINT32 x, y, i;
	char* pDestBuffer = pDest;

	// Cash Buffer Num
	UINT32 Cx = nBytesPerLine/16; 
	UINT32 Cy = nLine/8;

	for( y = 0; y < Cy; y++)
	{
		for( x = 0; x < Cx; x++ )
		{       
			char* pSrcBuffer = pSrc + (x * 16) + (y * 8 * nBytesPerLine);
			for( i = 0; i < 8; i++)
			{
				i3mem::Copy( pDestBuffer, pSrcBuffer, 16);
				pDestBuffer += 16;
				pSrcBuffer += nBytesPerLine;
			}
		}
	}


	return true;
}

// PSP의 고속모드( Swizzled ) 포맷을 일반모드 텍스쳐로 만들기.
// pDest : 대상 버퍼
// pSrc  : 원본 버퍼
// nBytesPerLine  : 버퍼의 가로 사이즈. BYTE 단위.
// nLine  : 버퍼의 세로 사이즈. nLine
I3_EXPORT_GFX 
bool i3ImageConvertSwizzledToLinear( char* pDest, char* pSrc, UINT32 nBytesPerLine, UINT32 nLine, I3_PLATFORM Platform)
{
	// 현재는 PSP의 Swizzled 포맷만 지원함.
	I3ASSERT( Platform == I3_PLATFORM_PSP );

	UINT32 x, y, i;
	char* pSrcBuffer = pSrc;

	// Cash Buffer Num
	UINT32 Cx = nBytesPerLine/16; 
	UINT32 Cy = nLine/8;

	for( y = 0; y < Cy; y++ )
	{
		for( x = 0; x < Cx; x++)
		{       
			char* pDestBuffer = pDest + (x * 16) + (y * 8 * nBytesPerLine);
			for( i = 0; i < 8; i++ )
			{
				i3mem::Copy( pDestBuffer, pSrcBuffer, 16);
				pSrcBuffer += 16;
				pDestBuffer += nBytesPerLine;
			}
		}
	}

	return true;
}
