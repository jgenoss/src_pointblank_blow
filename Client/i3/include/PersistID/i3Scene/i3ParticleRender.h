#if !defined( __I3_PARTICLE_RENDER_H)
#define __I3_PARTICLE_RENDER_H

#include "i3Scene.h"
#include "i3ParticleGroup.h"
#include "i3SceneTracer.h"
#include "i3Decal.h"

typedef struct ALIGN4 _tagDecalHitBoxInfo
{
#if defined( I3_PHYSX)
	i3PhysixShapeSet *		m_pHitBox;
#endif
	i3Particle *			m_pParticle;
	INT32					m_FrameCnt;
	INT32					m_nIndex;

	void					reset(void)
	{
		m_FrameCnt = 0;
		I3_SAFE_RELEASE( m_pParticle);
		m_nIndex = -1;
	}

	void					setParticle( i3Particle * pParticle)
	{
		I3_REF_CHANGE( m_pParticle, pParticle);
	}

} I3SG_DECAL_HITBOX;

class I3_EXPORT_SCENE i3ParticleRender : public i3EventReceiver
{
	I3_CLASS_DEFINE( i3ParticleRender);
protected:
	i3BinList					m_GroupList;

	i3ModelViewMatrixAttr *		m_pWorldMtxAttr;

	I3SG_DECAL_HITBOX *			m_pDecalHitBox;
	i3List						m_UsedDecalList;

	UINT32						m_BoxIndices[ 36 ];

#if defined( I3_DEBUG)
	i3AttrSet *					m_pHitBoxAttrSet;
#endif

protected:

	INT32				_FindFreeHitBox(void);
#if defined( I3_PHYSX)
	// return value : created triangle count.. 0 is no create
	INT32				_ParseContactReport(i3PhysixShapeSet * pShapeSet);
#endif
	void				_CalcPlaneClip( i3Decal * pInfo);
	BOOL				_AddPolygon( i3Decal * pInfo, UINT32 vertexCount, VEC3D *vertex, VEC3D *normal);
	void				_ClipMesh( i3Decal * pInfo, UINT32 triangleCount, UINT16 *triangle, VEC3D *vertex, VEC3D *normal);
	UINT32				_ClipPolygon( i3Decal * pInfo, UINT32 vertexCount, VEC3D *vertex, VEC3D *normal, VEC3D *newVertex, VEC3D *newNormal);
	UINT32				_ClipPolygonAgainstPlane( VEC4D * plane, UINT32 vertexCount, VEC3D *vertex, VEC3D *normal, VEC3D *newVertex, VEC3D *newNormal);

	BOOL				_AddTriangleData( i3Decal * pInfo, VEC3D * pPos1, VEC3D * pPos2, VEC3D * pPos3 );
	
public:
	i3ParticleRender(void);
	virtual ~i3ParticleRender(void);

	bool				Create(void);

	void				RemoveAllGroups(void);
	void				RemoveGroup( i3ParticleGroup * pGroup);

	I3SG_DECAL_HITBOX *	EmitDecalHitBox( i3SceneTracer * pTracer, i3Particle * pParticle, VEC3D * pPos, VEC3D * pSize, MATRIX * pMtx);
	void				CancelDecalHitBox( I3SG_DECAL_HITBOX * pBox);
	void				ProcessDecal( i3SceneTracer * pTracer);

	bool				AddParticleScene( i3TimeSequence * pNode, INT32 maxCount, INT32 priority = 50);
	i3ParticleGroup *	AddParticle( i3Particle * pParticle, INT32 maxCount);
	i3ParticleGroup *	FindParticleGroup( i3Particle * pParticle);

	void				Render( i3SceneTracer * pTracer);

	void				ArrangeRenderOrder(void);

#if defined( I3_DEBUG)
	void				Dump(void);
	void				RenderHitBox( i3Decal * pInfo, i3PhysixShapeSet * pShapeSet);
#endif

	virtual BOOL		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
};

extern I3_EXPORT_SCENE i3ParticleRender *		g_pParticleRender;

#endif
