#if !defined( __I3_FRUSTUM_H)
#define __I3_FRUSTUM_H

#include "i3Volume.h"
#include "i3BoundBox.h"

class I3_EXPORT_MATH i3Frustum : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3Frustum, i3ElementBase);
public:
    VEC4D	vPlanes[6];
	INT32	nVertexLUT[6] = { 0, 0, 0, 0, 0, 0 };
    VEC3D	pntList[8];

public:
    void	Set( const MATRIX* matrix );

    bool	TestSphere     ( VEC3D * pCenter, REAL32 r );
    INT32	TestBox        ( i3BoundBox * box );
    bool	TestSweptSphere( VEC3D * pCenter, REAL32 r, VEC3D * sweepDir );
};

#endif
