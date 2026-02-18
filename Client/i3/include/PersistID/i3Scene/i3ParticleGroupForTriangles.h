#if !defined( __I3_PARTICLE_GROUP_TRIANGLES_H)
#define __I3_PARTICLE_GROUP_TRIANGLES_H

#include "i3Decal.h"
#include "i3ParticleGroup.h"

class i3ParticleGroupForTriangles : public i3ParticleGroup
{
	I3_CLASS_DEFINE( i3ParticleGroupForTriangles);
protected:
	virtual void		_Lock(void);
	virtual void		_Unlock(void);

protected:

	virtual INT32		GetTriangleCount(void);

public:
	i3ParticleGroupForTriangles(void);
	virtual ~i3ParticleGroupForTriangles(void);

	virtual bool		SetCapacity( INT32 Count, bool bForce = false);

	virtual void				AddForDecal( i3Decal * pDecal, UINT32 col);
	virtual INT32				AddForSprite( VEC3D * pCenter, VEC3D * pSize, REAL32 rot, bool bFace2Cam);
	virtual void				SetColor( INT32 idx, UINT32 col);
	virtual void				SetTextureCoord( INT32 idx, VEC2D * p1, VEC2D * p2);
};

#endif
