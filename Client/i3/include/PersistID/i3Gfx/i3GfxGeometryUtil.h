#if !defined( __I3_GFX_GEOMETRY_UTIL_H)
#define __I3_GFX_GEOMETRY_UTIL_H

#include "i3Math.h"
#include "i3GfxType.h"
#include "i3VertexArray.h"

namespace i3GfxGeometryUtil
{
	I3_EXPORT_GFX i3VertexArray *		CreateRectangle( REAL32 Width, REAL32 Height, REAL32 Z,
							BOOL bUV,
							BOOL bNormal,
							COLORREAL * pColor1 = NULL, COLORREAL * pColor2 = NULL,
							COLORREAL * pColor3 = NULL, COLORREAL * pColor4 = NULL
							);
};

#endif
