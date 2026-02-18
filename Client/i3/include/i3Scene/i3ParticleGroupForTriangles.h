#if !defined( __I3_PARTICLE_GROUP_TRIANGLES_H)
#define __I3_PARTICLE_GROUP_TRIANGLES_H

//#include "i3Decal.h"
#include "i3ParticleGroup.h"

//
// 劳胶器飘可记 力芭..(2012.09.28.荐后)
//
class i3ParticleGroupForTriangles : public i3ParticleGroup
{
	I3_CLASS_DEFINE( i3ParticleGroupForTriangles, i3ParticleGroup);
protected:
	virtual void		_Lock(void);
	virtual void		_Unlock(void);
	

public:
	virtual INT32		GetTriangleCount(void) const;
	virtual INT32		GetEndIndex() const;
	virtual void		PostTrace() override;
	virtual bool		SetCapacity( INT32 Count, bool bForce = false) override;

	virtual void				AddForDecal( i3DecalDrawInfo * pDecal, UINT32 col, i3ParticleTransformMatrix* matSet);
	virtual INT32				AddForSprite( VEC3D * pCenter, VEC3D * pSize, REAL32 rot, bool bFace2Cam, i3ParticleTransformMatrix* matSet) override;
	virtual void				SetColor( INT32 idx, UINT32 col) override;
	virtual void				SetTextureCoord( INT32 idx, VEC2D * p1, VEC2D * p2) override;
private:
	INT32				m_AccVertexCount = 0;

};

#endif
