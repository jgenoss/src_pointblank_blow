#if !defined( __I3G_UTIL_DX_H)
#define __I3G_UTIL_DX_H

#if !defined(I3_WINDOWS) || !defined(I3G_DX)
#error "This header file is only for DirectX and Windows."
#endif

#include "i3VertexArray.h"
#include "i3IndexArray.h"


//////////////////////////////////////////////////////////////////////////////
// Resource Type
I3_EXPORT_GFX D3DRESOURCETYPE	i3DXUT_GetNativeResourceType( I3G_RESOURCE_TYPE type);
I3_EXPORT_GFX I3G_RESOURCE_TYPE i3DXUT_GetI3ResourceType( D3DRESOURCETYPE type);

/////////////////////////////////////////////////////////////////////////////
// Raster Format
I3_EXPORT_GFX D3DFORMAT			i3DXUT_GetNativeImageFormat( I3G_IMAGE_FORMAT format, BOOL * bExact = NULL);
I3_EXPORT_GFX I3G_IMAGE_FORMAT	i3DXUT_GetI3ImageFormat( D3DFORMAT format, BOOL * bExact = NULL);

////////////////////////////////////////////////////////////////////////////////////////
// Usage Flags
I3_EXPORT_GFX I3G_USAGE i3DXUT_GetI3Usage( DWORD usage);
I3_EXPORT_GFX DWORD i3DXUT_GetNativeUsage( I3G_USAGE usage);
I3_EXPORT_GFX I3G_USAGE i3DXUT_DetermineRuntimeUsage( i3VertexFormat * pFormat, I3G_USAGE usage);

////////////////////////////////////////////////////////////////////////////////////////
// Vertex Format
I3_EXPORT_GFX DWORD i3DXUT_GetNativeVertexFormat( i3VertexFormat * pFormat);

#endif

