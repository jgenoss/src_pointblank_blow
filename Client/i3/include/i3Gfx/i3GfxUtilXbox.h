#if !defined( __I3G_UTIL_XB_H)
#define __I3G_UTIL_XB_H

#if !defined(I3_XBOX) || !defined(I3G_XBOX)
#error "This header file is only for Xbox."
#endif

#include "i3VertexArray.h"

//////////////////////////////////////////////////////////////////////////////
// Resource Type
D3DRESOURCETYPE	i3_GetNativeResourceType( I3G_RESOURCE_TYPE type);
I3G_RESOURCE_TYPE i3_GetI3ResourceType( D3DRESOURCETYPE type);

/////////////////////////////////////////////////////////////////////////////
// Raster Format
D3DFORMAT			i3_GetNativeImageFormat( I3G_IMAGE_FORMAT format, BOOL *bExact = NULL );
I3G_IMAGE_FORMAT	i3_GetI3ImageFormat( D3DFORMAT format, BOOL *bExact = NULL );

////////////////////////////////////////////////////////////////////////////////////////
// Usage Flags
I3G_USAGE i3_GetI3Usage( DWORD usage);
DWORD i3_GetNativeUsage( I3G_USAGE usage);

////////////////////////////////////////////////////////////////////////////////////////
// Vertex Format
DWORD i3_GetNativeVertexFormat( i3VertexFormat * pFormat);

#endif