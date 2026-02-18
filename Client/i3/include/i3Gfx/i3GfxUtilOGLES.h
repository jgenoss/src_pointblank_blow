#if !defined( __I3G_GFX_UTIL_PSP_H)
#define __I3G_GFX_UTIL_PSP_H

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
void i3OGLESUT_GetNativeImageFormat( I3G_IMAGE_FORMAT format, INT32 * pPixelFormat, INT32 * pPixelType);
I3G_IMAGE_FORMAT	i3OGLESUT_GetI3ImageFormat( INT32 PixelFormat, INT32 PixelType);

// CompressedTexture 모드의 경우 이미지 앞에 Clut을 붙여야한다. by dfly79
INT32	i3OGLSUT_CalcCompressClutSize( INT32 PixelType);

/////////////////////////////////////////////////////////////////////////////////////////
// Vertex Format

/////////////////////////////////////////////////////////////////////////////////////////
// Errors

//#define CHECK_OGLERROR

#define OGL_CHECKRETURN(a)\
{\
	INT32 err = I3OGL::glGetError();\
	if( err != GL_NO_ERROR)\
	{\
		I3TRACE2( "%s\nGL Error : %x\n", #a, err);\
	}\
}

	#if defined( CHECK_OGLERROR)
		#define	OGL_CHECK( a)		a; OGL_CHECKRETURN( a)
	#else
		#define	OGL_CHECK( a)		a
	#endif

#endif

#endif
