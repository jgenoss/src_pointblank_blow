#if !defined( __I3G_GFX_UTIL_PSP_H)
#define __I3G_GFX_UTIL_PSP_H

#include "i3GfxType.h"
#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxType.h"
#include "i3GfxDefine.h"
#include "i3VertexArray.h"
#include "i3GfxGlobalVariable.h"

#if defined( I3G_PSP)
#include "i3GfxDefinePSP.h"
#include "i3GPUCommandParserPSP.h"

//////////////////////////////////////////////////////////////////////////////
// Raster Format
UINT32	i3PSPUT_GetNativeImageFormat( I3G_IMAGE_FORMAT format, BOOL *bNotExact = NULL );
I3G_IMAGE_FORMAT	i3PSPUT_GetI3ImageFormat( UINT32 format);

/////////////////////////////////////////////////////////////////////////////////////////
// Vertex Format
UINT32 i3PSPUT_GetNativeVertexFormat( i3VertexFormat * pFormat);

#endif

#endif
