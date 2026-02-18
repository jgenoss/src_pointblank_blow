#if !defined( __I3_GFX_GEOMETRY_UTIL_H)
#define __I3_GFX_GEOMETRY_UTIL_H

#include "i3Math.h"
#include "i3GfxType.h"
#include "i3VertexArray.h"

namespace i3GfxGeometryUtil
{
	I3_EXPORT_GFX i3VertexArray *		CreateRectangle( REAL32 Width, REAL32 Height, REAL32 Z,
							bool bUV,
							bool bNormal,
							COLORREAL * pColor1 = nullptr, COLORREAL * pColor2 = nullptr,
							COLORREAL * pColor3 = nullptr, COLORREAL * pColor4 = nullptr
							);
};

#endif
