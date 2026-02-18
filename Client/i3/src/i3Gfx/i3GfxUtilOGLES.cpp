#include "i3GfxType.h"
#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxType.h"
#include "i3GfxDefine.h"
#include "i3VertexArray.h"
#include "i3GfxGlobalVariable.h"

#if defined( I3G_OGLES)
#include "i3GfxDefineOGLES.h"

//////////////////////////////////////////////////////////////////////////////
// Raster Format
void	i3OGLESUT_GetNativeImageFormat( I3G_IMAGE_FORMAT format, INT32 * pPixelFormat, INT32 * pPixelType)
{
	i3GfxCaps * pCaps = NULL;

	if( g_pRenderContext)
		pCaps = g_pRenderContext->GetCaps();

	switch( format & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB))
	{
	// 4 bit formats
	case I3G_IMAGE_FORMAT_I4		:
		*pPixelFormat = GL_RGBA;
		if( pCaps->CanCompressTexFormat( GL_PALETTE4_RGBA8_OES))
			*pPixelType = GL_PALETTE4_RGBA8_OES;
		else
			*pPixelType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	// 8bit formats
	case I3G_IMAGE_FORMAT_I8		:
		*pPixelFormat = GL_RGBA;
		if( pCaps->CanCompressTexFormat( GL_PALETTE8_RGBA8_OES))
			*pPixelType = GL_PALETTE8_RGBA8_OES;
		else
			*pPixelType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;
	case i3G_IMAGE_FORMAT_L8		:
	case i3G_IMAGE_FORMAT_A8		:
		*pPixelFormat = GL_RGBA;
		*pPixelType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	// 16 bit formats
	case I3G_IMAGE_FORMAT_RGB_565	:	
		*pPixelFormat = GL_RGB;
		*pPixelType = GL_UNSIGNED_SHORT_5_6_5;
		break;

	case I3G_IMAGE_FORMAT_XRGB_1555	:	
		*pPixelFormat = GL_RGB;
		*pPixelType = GL_UNSIGNED_SHORT_5_5_5_1;
		break;

	case I3G_IMAGE_FORMAT_ARGB_1555	:
		*pPixelFormat = GL_RGBA;
		*pPixelType = GL_UNSIGNED_SHORT_5_5_5_1;
		break;

	case I3G_IMAGE_FORMAT_XRGB_4444	:	
		*pPixelFormat = GL_RGB;
		*pPixelType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	case I3G_IMAGE_FORMAT_ARGB_4444	:
	case I3G_IMAGE_FORMAT_ABGR_4444	:
		*pPixelFormat = GL_RGBA;
		*pPixelType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888	:
	case I3G_IMAGE_FORMAT_BGR_888	:
		*pPixelFormat = GL_RGB;
		*pPixelType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	// 32 bit formats
	case I3G_IMAGE_FORMAT_RGBX_8888	:
	case I3G_IMAGE_FORMAT_BGRX_8888	:
		*pPixelFormat = GL_RGB;
		*pPixelType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	case I3G_IMAGE_FORMAT_RGBA_8888	:
	case I3G_IMAGE_FORMAT_BGRA_8888	:
		*pPixelFormat	= GL_RGBA;
		*pPixelType		= GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	default :
	#if defined( I3_DEBUG)
		I3TRACE1( "i3GfxUtilOGLES - [I3ERROR] Unsupported raster format : %d\n", format);
	#endif
		break;
	}
}

I3G_IMAGE_FORMAT	i3OGLESUT_GetI3ImageFormat( INT32 PixelFormat, INT32 PixelType)
{
	switch( PixelType)
	{
		case GL_PALETTE4_RGBA8_OES :
			return I3G_IMAGE_FORMAT_I4;

		case GL_PALETTE8_RGBA8_OES :
			return I3G_IMAGE_FORMAT_I8;

		case GL_UNSIGNED_SHORT_4_4_4_4 :	
			switch( PixelFormat)
			{
				case GL_RGB :				return I3G_IMAGE_FORMAT_XBGR_4444;
				default :					return I3G_IMAGE_FORMAT_ABGR_4444;
			}
			break;

		case GL_UNSIGNED_SHORT_5_5_5_1:	
			switch( PixelFormat)
			{
				case GL_RGB :				return I3G_IMAGE_FORMAT_XBGR_1555;
				default :					return I3G_IMAGE_FORMAT_ABGR_1555;
			}
			break;

		case GL_UNSIGNED_SHORT_5_6_5:		
			return I3G_IMAGE_FORMAT_RGB_565;

		default :
		#if defined( I3_DEBUG)
			I3TRACE1( "i3GfxUtilDX - [I3ERROR] Unsupported raster format : %d\n", PixelFormat);
		#endif
			break;
	}

	return I3G_IMAGE_FORMAT_NONE;
}

INT32	i3OGLSUT_CalcCompressClutSize( INT32 PixelType)
{
	switch( PixelType)
	{
	case GL_PALETTE4_RGBA8_OES		:	return 64;
	case GL_PALETTE4_RGB8_OES		:	return 48;
	case GL_PALETTE4_R5_G6_B5_OES	:	return 32;
	case GL_PALETTE4_RGBA4_OES		:	return 32;
	case GL_PALETTE4_RGB5_A1_OES	:	return 32;
	case GL_PALETTE8_RGBA8_OES		:	return 1024;
	case GL_PALETTE8_RGB8_OES		:	return 768;
	case GL_PALETTE8_R5_G6_B5_OES	:	return 512;
	case GL_PALETTE8_RGBA4_OES		:	return 512;
	case GL_PALETTE8_RGB5_A1_OES	:	return 512;
	default:	break;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Vertex Format

#endif
