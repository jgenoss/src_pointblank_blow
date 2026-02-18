#if !defined( __I3_DECAL_POOL_H__)
#define __I3_DECAL_POOL_H__

#include "i3Particle.h"
#include "i3SceneTracer.h"
#include "i3DecalDef.h"

class i3PhysixShapeSet;
class i3Particle;

#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif

struct I3_DECAL_HITED_INFO
{
	UINT16		m_Indices[ I3SG_DECAL_MAX_VERTICES * 3 ] = { 0 };
	VEC3D		m_VertexArray[ I3SG_DECAL_MAX_VERTICES ];
	VEC3D		m_NormalArray[ I3SG_DECAL_MAX_VERTICES ];

	UINT16		m_VertexCount = 0;
	UINT16		m_TriangleCount = 0;
};

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

struct ALIGN4 I3SG_DECAL_HITBOX
{
#if defined( I3_PHYSX)
	i3PhysixShapeSet *		m_pHitBox = nullptr;
#endif
	i3Particle *			m_pParticle = nullptr;
	INT32					m_FrameCnt = 0;
	INT32					m_nIndex = 0;

	i3Decal *				m_pDecal = nullptr;
	i3DecalDrawInfo *		m_pDrawDecalInfo = nullptr;

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

};
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3DecalManager : public i3ElementBase			// I3_EXPORT_SCENE 제거...(외부에서 쓰이는 것이 확인되지 않았습니다. 2012.08.30.수빈)
{
	I3_CLASS_DEFINE( i3DecalManager, i3ElementBase);
private:
	UINT32					m_BoxIndices[ 36 ];		// [initialize at constructor]

	// Hit를 위한 Decal 정보
	INT32					m_iCount = 0;

	// Hit 처리용
	I3SG_DECAL_HITBOX *		m_pDecalHitBox = nullptr;
	i3::vector<I3SG_DECAL_HITBOX*>	m_EmitedDecalList;
	
	// i3Decal Pool
	i3FixedMemoryPool *		m_pDecalPool = nullptr;

	// Hit 후 Rendering되는 decal 정보 i3DecalDrawInfo
	// i3DecalDrawInfo Pool
	i3FixedMemoryPool *		m_pDecalDrawInfoPool = nullptr;


protected:
	
	INT32				_FindFreeHitBox(void);

	i3Decal *			_GetDecal( void);
	i3DecalDrawInfo *	_GetDrawDecal( void);

	/** \brief Hit Shape 초기화 및 Decal을 반환한다.*/
	void				_CancelDecalHitBox( I3SG_DECAL_HITBOX * pInfo);

	// return value : created triangle count.. 0 is no create
	// HitShape에 의한 Contact 정보를 갖고 Draw용 정보를 생성한다.
	INT32				_ParseContactReport(i3PhysixShapeSet * pShapeSet);

	bool				_CalcPlaneClip( I3SG_DECAL_HITBOX * pInfo, I3_DECAL_HITED_INFO * pHitInfo);
	bool				_AddPolygon( I3SG_DECAL_HITBOX * pInfo, UINT32 vertexCount, VEC3D * pVertex, VEC3D * pNormal);
	void				_ClipMesh( I3SG_DECAL_HITBOX * pInfo, UINT32 triangleCount, UINT16 *triangle, VEC3D *vertex, VEC3D *normal);
	UINT32				_ClipPolygon( i3Decal * pDecal, UINT32 vertexCount, VEC3D *vertex, VEC3D *normal, VEC3D *newVertex, VEC3D *newNormal);
	UINT32				_ClipPolygonAgainstPlane( VEC4D * plane, UINT32 vertexCount, VEC3D *vertex, VEC3D *normal, VEC3D *newVertex, VEC3D *newNormal);

	bool				_AddTriangleData( I3_DECAL_HITED_INFO * pInfo, VEC3D * pPos1, VEC3D * pPos2, VEC3D * pPos3 );

	
public:
	i3DecalManager();
	virtual ~i3DecalManager();

	void		Create( i3EventReceiver * pER, INT32 count);
	void		Destroy( void);

	void		CreatePhysXBox( i3EventReceiver * pER);
	void		ReleasePhysXBox( void);

	I3SG_DECAL_HITBOX *	EmitDecal( i3SceneTracer * pTracer, i3Particle * pParticle, I3PARTICLEOBJECT * pObj);
	
	void		FreeDecal( i3Decal * pDecal);
	void		FreeDrawDecalInfo(i3DecalDrawInfo * pInfo);

	void		ProcessDecal( i3SceneTracer * pTracer);

	void		ProcessEventContactHitBox( i3PhysixShapeSet * pShapeSet);
};

#endif
