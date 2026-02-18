#if !defined( __I3_DECAL_H)
#define __I3_DECAL_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Node.h"
#include "i3AttrSet.h"

#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureWrapAttr.h"
#include "i3TextureEnableAttr.h"
#include "i3AlphaTestEnableAttr.h"
#include "i3Geometry.h"
#include "i3GeometryAttr.h"

#include "i3SceneGraphInfo.h"
#include "i3PhysixShapeSet.h"

//#include "i3EventReceiver.h"

#define I3SG_DECAL_MAX_COUNT			128
#define I3SG_DECAL_MAX_VERTICES			128

#define	I3SG_DECAL_ACTOR_GROUP			23

enum I3SG_DECAL_STATE
{
	I3SG_DECAL_STATE_NONE = 0,
	I3SG_DECAL_STATE_WAIT,				// Contact Report를 받기를 기다리고 있음.
	I3SG_DECAL_STATE_HIT,				// 정상적으로 Contact되었으며, Vertex 등의 값을 사용할 수 있음.
	I3SG_DECAL_STATE_NOHIT,				// 충분한 시간(1Frame)이 지났는데도 Contact Report가 없음. 일반적인 Sprite으로 간주.
};

class i3Particle;

class i3Decal : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Decal);
protected:
	I3SG_DECAL_STATE	m_State;
	UINT16			m_nTerrainType;

public:

	VEC3D			m_Position;
	VEC3D			m_Normal;
	VEC3D			m_Tangent;
	VEC3D			m_Binormal;
	VEC3D			m_Center;

	RT_REAL32		m_Width;
	RT_REAL32		m_Height;
	RT_REAL32		m_Depth;
	RT_REAL32		m_One_over_W;
	RT_REAL32		m_One_over_H;
	VEC3D			m_vSize;
	
	RT_REAL32		m_Epsilon;
	RT_REAL32		m_Rotation;

	VEC4D		m_PlaneLeft;
	VEC4D		m_PlaneRight;
	VEC4D		m_PlaneTop;
	VEC4D		m_PlaneBottom;
	VEC4D		m_PlaneFront;
	VEC4D		m_PlaneBack;
	
	UINT32		m_VertexCount;
	UINT32		m_TriangleCount;

	COLOR		m_Color;
	VEC2D		m_UV[ 2 ];
	VEC2D		m_UVScale;

	MATRIX		m_invWorldMtx;

	//COLORREAL	m_ColorArray[ I3SG_DECAL_MAX_VERTICES ];
	VEC2D		m_UVArray[ I3SG_DECAL_MAX_VERTICES ];

	UINT32		m_NewTriangleCount;
	UINT32		m_NewVertexCount;
	//UINT8		m_mapVertexWard[ I3SG_DECAL_MAX_VERTICES ];
	//VEC3D		m_mapVertexCenter[ 4];
	VEC3D		m_NewVertexArray[ I3SG_DECAL_MAX_VERTICES ];	// 새로 생성한 버택스입니다.
	//VEC3D		m_NewNormalArray[ I3SG_DECAL_MAX_VERTICES ];	// 새로 생성한 노멀입니다.
	UINT16		m_NewIndices[ I3SG_DECAL_MAX_VERTICES * 3 ];

	//VEC3D		m_NewAxis[ I3SG_DECAL_MAX_VERTICES ];
	//VEC3D		m_NewTangentArray[ I3SG_DECAL_MAX_VERTICES ];
	//VEC3D		m_NewBinormalArray[ I3SG_DECAL_MAX_VERTICES ];

	// 히트된 메쉬의 정보입니다.
	UINT16		m_Indices[ I3SG_DECAL_MAX_VERTICES * 3 ];
	VEC3D		m_VertexArray[ I3SG_DECAL_MAX_VERTICES ];
	VEC3D		m_NormalArray[ I3SG_DECAL_MAX_VERTICES ];

	// Position과 Plane의 거리
	//REAL32		m_rPlaneDistanceFromCenterPos[ I3SG_DECAL_MAX_VERTICES ];
	//REAL32		m_rPolygonDotFromDecal[ I3SG_DECAL_MAX_VERTICES ];

#if defined( I3_DEBUG)
	i3Node *	m_pParticle;
#endif

public:
	I3SG_DECAL_STATE getDecalState( void)								{ return m_State; }
	void			setDecalState( I3SG_DECAL_STATE state )				{ m_State = state; }

	UINT16			getTerrainType(void)								{ return m_nTerrainType; }
	void			setTerrainType( UINT16 nType)						{ m_nTerrainType = nType; }
	void			setCenter( VEC3D * pPos)							{ i3Vector::Copy( &m_Center, pPos); }
	void			setRotation( REAL32 rot)							{ m_Rotation = rot; }
	
public:
	i3Decal();
	virtual ~i3Decal();

	void	Reset( void);

	void	SetTextureCoord( VEC2D * pMin, VEC2D * pMax);
	void	SetSize( VEC3D * pVec);
	
	// Calculate boundary planes
	void	CalcBoundaryPlane(void);

	// Assign texture mapping coordinates
	void	AssignTextureCoord(void);
};

I3_EXPORT_SCENE void i3Decal_InitWorldHitPair();
I3_EXPORT_SCENE void i3Decal_ReleaseWorldHitPair();

#endif
