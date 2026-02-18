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
#include "i3DecalDef.h"
//#include "i3EventReceiver.h"


  



//enum I3SG_DECAL_STATE
//{
//	I3SG_DECAL_STATE_NONE = 0,
//	I3SG_DECAL_STATE_WAIT,				// Contact Report를 받기를 기다리고 있음.
//	I3SG_DECAL_STATE_HIT,				// 정상적으로 Contact되었으며, Vertex 등의 값을 사용할 수 있음.
//	I3SG_DECAL_STATE_NOHIT,				// 충분한 시간(1Frame)이 지났는데도 Contact Report가 없음. 일반적인 Sprite으로 간주.
//};

class i3Particle;
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3DecalDrawInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( i3DecalDrawInfo, i3ElementBase);

private:
	VEC3D		m_NewVertexArray[ I3SG_DECAL_MAX_VERTICES ];	// 새로 생성한 버택스입니다.
	VEC2D		m_UVArray[ I3SG_DECAL_MAX_VERTICES ];

	VEC2D		m_UV;
	VEC2D		m_UVScale;

	REAL32	m_One_over_W = 0.0f;
	REAL32	m_One_over_H = 0.0f;

	UINT16		m_NewIndices[I3SG_DECAL_MAX_VERTICES * 3] = { 0 };

	bool		m_bEnable = false;
	UINT8		m_NewTriangleCount = 0;
	UINT8		m_NewVertexCount = 0;

public:
	bool	isEnable( void)						{ return m_bEnable; }
	void	setEnable( bool bEnable)			{ m_bEnable = bEnable; }

	INT32	getTriangleCount( void)				{ return (INT32) m_NewTriangleCount; }
	void	setTriangleCount( INT32 count)		{ m_NewTriangleCount = (UINT8) count; }

	INT32	getVertexCount( void)				{ return (INT32) m_NewVertexCount; }
	void	setVertexCount( INT32 count)		{ m_NewVertexCount = (UINT8) count; }

	VEC3D *	getVertices( void)					{ return m_NewVertexArray; }

	UINT16 * getIndices( void)					{ return m_NewIndices; }

	VEC2D *	getUVArray( void)					{ return m_UVArray; }


public:	
	void	SetTextureCoord( VEC2D * pMin, VEC2D * pMax);

	void	SetOneOverSize( REAL32 width, REAL32 height);

	// Assign texture mapping coordinates
	void	AssignTextureCoord( VEC3D * pPosition, VEC3D * pTangent, VEC3D * pBinormal);
};
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3Decal : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Decal, i3ElementBase);
protected:
	MATRIX		m_invWorldMtx;

	VEC4D		m_PlaneLeft;
	VEC4D		m_PlaneRight;
	VEC4D		m_PlaneTop;
	VEC4D		m_PlaneBottom;
	VEC4D		m_PlaneFront;
	VEC4D		m_PlaneBack;

	VEC3D			m_Position;
	VEC3D			m_Normal;
	VEC3D			m_Tangent;
	VEC3D			m_Binormal;
	VEC3D			m_Center;

	REAL32		m_Width = 0.0f;
	REAL32		m_Height = 0.0f;
	REAL32		m_Depth = 0.0f;
	REAL32		m_Rotation = 0.0f;
	
#if defined( I3_DEBUG)
	i3Node *	m_pParticle = nullptr;
#endif

public:
	VEC3D *			getPosition( void)							{ return &m_Position; }
	void			setPosition( VEC3D * pPos)					{ i3Vector::Copy( &m_Position, pPos); }

	VEC3D *			getNormal( void)							{ return &m_Normal; }
	void			setNormal( VEC3D * pNormal)					{ i3Vector::Copy( &m_Normal, pNormal); }

	VEC3D *			getTangent( void)							{ return &m_Tangent; }
	void			setTangent( VEC3D * pTangent)				{ i3Vector::Copy( &m_Tangent, pTangent); }

	VEC3D *			getBinormal( void)							{ return &m_Binormal; }
	void			setBinormal( VEC3D * pBinormal)				{ i3Vector::Copy( &m_Binormal, pBinormal); }

	VEC3D *			getCenter( void)							{ return &m_Center; }
	void			setCenter( VEC3D * pPos)					{ i3Vector::Copy( &m_Center, pPos); }

	VEC4D *			getPlaneLeft( void)							{ return &m_PlaneLeft; }
	VEC4D *			getPlaneRight( void)						{ return &m_PlaneRight; }
	VEC4D *			getPlaneTop( void)							{ return &m_PlaneTop; }
	VEC4D *			getPlaneBottom( void)						{ return &m_PlaneBottom; }
	VEC4D *			getPlaneFront( void)						{ return &m_PlaneFront; }
	VEC4D *			getPlaneBack( void)							{ return &m_PlaneBack; }

	MATRIX *		getInverseMatrix( void)						{ return &m_invWorldMtx; }

	void			setRotation( REAL32 rot)					{ m_Rotation = rot; }
	
public:
	void	SetSize( VEC3D * pVec);
	
	// Calculate boundary planes
	void	CalcBoundaryPlane(void);
};

I3_EXPORT_SCENE void i3Decal_InitWorldHitPair();
I3_EXPORT_SCENE void i3Decal_ReleaseWorldHitPair();

#endif
