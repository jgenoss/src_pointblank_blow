//
//	Dedication Server, Physics Server, make_i3sobj tool에서 사용된다 
//
//

#if !defined( __NxShapeSet_H__ )
#define __NxShapeSet_H__

#pragma warning( once : 4995 34 )

#include <NxBodyDesc.h>

typedef void* NxPtr;
class i3List;

class NxShapeSet;
typedef struct _DS_PHYSIX_HIT_RESULT
{	
	NxShapeSet *		m_pDestShapeSet;
	NxShape *			m_pHitShape;
	I3_PHYSIX_TERRAIN *	m_pTerrain;
	INT32				m_FaceIndex;
	NxVec3				m_Normal;
	NxVec3				m_Intersect;
	NxF32				m_T;		// distance
} DS_PHYSIX_HIT_RESULT;

// i3CollideeLine에 대응
typedef struct _DS_COLLIDEELINE
{
	NxVec3		m_Start;
	NxVec3		m_End;
	NxVec3		m_Dir;
} DS_COLLIDEELINE;

// I3_PHYSIX_TERRAIN에 대응
typedef struct _DS_PHYSIX_TERRAIN
{
	NxU16		m_Terrain;
	NxU16		m_Reverb;
	NxU8		m_Color[4];
	NxI32		m_Thickness;
	NxI32		m_PortalID;
	NxI32		pad[2];
} DS_PHYSIX_TERRAIN;

// physX에서 built-in으로 제공하는 클래스
//
// NxBounds3 
// NxBox 
// NxCapsule 
// NxPlane 
// NxRay 
// NxSegment 
// NxSphere 


#define I3PHYSIX_CONTACT_FLAG_NOTUSE			1
#define I3PHYSIX_CONTACT_FLAG_NONORMAL			2
#define I3PHYSIX_CONTACT_FLAG_NOINTERSECT		4

#define I3PHYSIX_CONTACT_REPORT_COUNT			32

extern NxPhysicsSDK*	gPhysicsSDK;

class NxShapeSet
{

friend class i3scolFile;

public:
	NxMat34&		getTransform()		{	return m_matTransform;		}
	NxU32			getNxDataSize()		{	return m_NxDataSize;		}
	NxPtr			getNxData()			{	return m_pNxData;			}
	NxU32			getShapeType()		{	return m_ShapeType;			}
	void			setShapeType( NxU32 type)	{ m_ShapeType = type;	}
	NxU32			getShapeGroup()		{	return m_ShapeGroup;		}
	NxU32			getVertexCount()	{	return m_numVertices;		}
	NxF32*			getVertexList()		{	return m_pvVertList;		}
	NxU32			getFaceCount()		{	return m_numFaces;			}
	I3_PHYSIX_TRI*	getFaceList()		{	return m_pI3FaceList;		}
	NxF32			getRestitution()	{	return m_restitution;		}
	NxF32			getStaticFriction()	{	return m_staticFriction;	}
	NxF32			getDynamicFriction(){	return m_dynamicFriction;	}
	
	I3_PHYSIX_TRI *		getFace		( INT32 faceID )	{ if(m_pI3FaceList == NULL) return NULL; return m_pI3FaceList + faceID; }
	I3_PHYSIX_TERRAIN*	getTerrain	( UINT32 idx )		{ if( idx < m_numTerrains  ) return (m_pI3Terrain + idx); return NULL;   }

	NxActor *			getNxActor(void)				{ return m_pNxActor; }
	void				setNxActor( NxActor * pActor)	{ m_pNxActor = pActor; }

	void				setGroup( UINT16 grp)			{ if( m_pNxActor == NULL ) return; m_pNxActor->setGroup( grp); }
	UINT16				getGroup(void)					{ if( m_pNxActor == NULL ) return 0xFFFF; return m_pNxActor->getGroup(); }

	INT32				getNxShapeCount(void)			{ if( m_pNxActor == NULL ) return 0; return m_pNxActor->getNbShapes(); }
	NxShape *			getNxShape( INT32 idx)			{ if( m_pNxActor == NULL ) return NULL; return m_pNxActor->getShapes()[ idx]; }

	
	void reset()	{
						m_matTransform.id();
						m_ShapeGroup				= 0;
						m_ShapeType			= 0;
						m_restitution		= 0.0f;		
						m_staticFriction	= 0.0f;
						m_dynamicFriction	= 0.0f;
						
						I3_SAFE_DELETE( m_pI3Terrain );
						m_numTerrains = 0;

						I3_SAFE_DELETE( m_pvVertList );
						m_numVertices		= 0;

						I3_SAFE_DELETE( m_pI3FaceList );
						m_numFaces			= 0;	
						
						I3_SAFE_DELETE( m_pNxData );
						m_NxDataSize		= 0;
						m_pNxActor			= NULL; 
					};
public:
	NxShapeSet()	{
						m_matTransform.id();
						m_ShapeGroup		= 0;
						m_ShapeType			= 0;
						m_restitution		= 0.0f;		
						m_staticFriction	= 0.0f;
						m_dynamicFriction	= 0.0f;
						
						m_pI3Terrain		= NULL;
						m_numTerrains		= 0;
						
						m_pvVertList		= NULL;
						m_numVertices		= 0;
						
						m_pI3FaceList		= NULL;
						m_numFaces			= 0;
						
						m_pNxData			= NULL;
						m_NxDataSize		= 0;

						m_pNxActor			= NULL;
						m_ShapeType			= I3_PHYSIX_SHAPE_NA;
					}

	~NxShapeSet() 
					{
						reset();
					}

public:
	// transforms
	NxMat34						m_matTransform;			// transform 
	NxU32						m_ShapeGroup;			// collision group
	NxU32						m_ShapeType;			// PHYSIX_SHAPE_TYPE (i3에서 define)
	NxF32						m_restitution;			// default material property	
	NxF32						m_staticFriction;		// ...
	NxF32						m_dynamicFriction;		// ...
	
	// 1. terrain data (바닥: 보통 2 triangle로 구성된 평면)
	I3_PHYSIX_TERRAIN*			m_pI3Terrain;			// 재질
	NxU32						m_numTerrains;

	// 2. mesh data 
	NxF32*						m_pvVertList;			// vertex list (OPENGL 호환을 위해서 float를 사용)
	NxU32						m_numVertices;			// vertex count
	
	I3_PHYSIX_TRI*				m_pI3FaceList;			// face list (I3_PHYSIX_TRI) : material index가 추가되어 있다!
	NxU32						m_numFaces;				// face count

	// 3. ccd collectin data 
	char*						m_pNxData;				// collection data (collada)
	NxU32						m_NxDataSize;			// bytes

	// 4. actor
	NxActor *					m_pNxActor;				// ccd가 로드될때 NXU_notifyActor에서 셋팅됨
};

#endif


