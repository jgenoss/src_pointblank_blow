#if !defined( __I3_PHYSIX_DEFINE_H)
#define __I3_PHYSIX_DEFINE_H

#if defined ( I3_PHYSX)

#if defined( min) || defined( max)
#undef min
#undef max
#endif

#include "NxPhysicsSDK.h"
#include "NxPhysics.h"
#include "NxScene.h"
#include "ControllerManager.h"
#include "NxExtended.h"
#include "NxController.h"
#include "NxCapsuleController.h"
#include "NxBoxController.h"

#if !defined( min) || !defined( max)
#define	min( a, b)			((a < b) ? a : b)
#define	max( a, b)			((a > b) ? a : b)
#endif

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

enum I3_PHYSIX_SHAPE_TYPE
{
	I3_PHYSIX_SHAPE_NA		= 0,
	I3_PHYSIX_SHAPE_SPHERE,
	I3_PHYSIX_SHAPE_CAPSULE,
	I3_PHYSIX_SHAPE_CONVEXMESH,
	I3_PHYSIX_SHAPE_TRIMESH,
	I3_PHYSIX_SHAPE_BOX,
};

#define I3_PHYSIX_CONTACT_REPORT_MAX_POINT	32

#define	I3_PHYSIX_STATE_NO_COLLISION		0x00000001
#define	I3_PHYSIX_STATE_KINEMATIC			0x00000002
#define	I3_PHYSIX_STATE_NO_GRAVITY			0x00000004
#define	I3_PHYSIX_STATE_NO_RESPONSE			0x00000008
#define I3_PHYSIX_STATE_FROZEN				0x00000010
#define	I3_PHYSIX_STATE_NO_RAYCAST			0x00000020
#define I3_PHYSIX_STATE_LOCK_COM			0x00000040			// 시뮬레이션 계산을 하지 않도록 합니다.
#define I3_PHYSIX_STATE_TRIGGER				0x00000080

#define	I3_PHYSIX_STATE_DISABLE			(	I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE | \
											I3_PHYSIX_STATE_FROZEN | I3_PHYSIX_STATE_KINEMATIC | \
											I3_PHYSIX_STATE_NO_GRAVITY | \
											I3_PHYSIX_STATE_NO_RAYCAST | I3_PHYSIX_STATE_LOCK_COM )


typedef struct _tagi3PhysixTriangle
{
	UINT32		m_Index[3];
	UINT16		m_MaterialIndex;
} I3_PHYSIX_TRI;

typedef struct _tagi3PhysixTerrainInfo
{
	UINT16		m_Terrain;
	UINT16		m_Reverb;
	UINT8		m_Color[4];
	INT32		m_Thickness;
	INT32		m_PortalID;
	INT32		pad[2];
} I3_PHYSIX_TERRAIN;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


typedef struct _tagTerrainMaterialInfo
{
	REAL32		m_StaticFriction;
	REAL32		m_DynamicFriction;
	REAL32		m_Restitution;
} I3_TERRAIN_MATERIAL;

class i3PhysixShapeSet;

typedef struct _tagi3PhysixHitResult
{	
	i3PhysixShapeSet *	m_pDestShapeSet;
	NxShape *			m_pHitShape;
	I3_PHYSIX_TERRAIN *	m_pTerrain;
	INT32				m_FaceIndex;
	VEC3D				m_Normal;
	VEC3D				m_Intersect;
	REAL32				m_T;
} I3_PHYSIX_HIT_RESULT;


// Contact report는 하나의 Pair 정보를 갖습니다.
typedef struct _tagi3PhysixContactReport
{
	UINT32				m_Events;
	i3PhysixShapeSet *	m_pOwnShapeSet;
	i3PhysixShapeSet *	m_pDestShapeSet;
	UINT32				m_nPointCount;
	UINT32				m_FaceIndex[ I3_PHYSIX_CONTACT_REPORT_MAX_POINT ];
	VEC3D				m_Intersect[ I3_PHYSIX_CONTACT_REPORT_MAX_POINT ];
	VEC3D				m_Normal[ I3_PHYSIX_CONTACT_REPORT_MAX_POINT ];
	REAL32				m_rNormalForce[ I3_PHYSIX_CONTACT_REPORT_MAX_POINT ];
	NxShape *			m_pShape[ I3_PHYSIX_CONTACT_REPORT_MAX_POINT ];
	UINT32				m_Reserved[4];
} I3_PHYSIX_CONTACT_REPORT;

namespace i3Physix
{
	inline
	void			HitResultCopyTo( I3_PHYSIX_HIT_RESULT * pDest, I3_PHYSIX_HIT_RESULT * pSrc )
	{
		pDest->m_pDestShapeSet	= pSrc->m_pDestShapeSet;
		pDest->m_pHitShape		= pSrc->m_pHitShape;
		pDest->m_pTerrain		= pSrc->m_pTerrain;
		pDest->m_FaceIndex		= pSrc->m_FaceIndex;
		i3Vector::Copy( &pDest->m_Normal, &pSrc->m_Normal );
		i3Vector::Copy( &pDest->m_Intersect, &pSrc->m_Intersect );
		pDest->m_T				= pSrc->m_T;
	}

	inline
	void			HitResultClear( I3_PHYSIX_HIT_RESULT * pSrc )
	{
		pSrc->m_pDestShapeSet = NULL;
		pSrc->m_pHitShape		= NULL;
		pSrc->m_pTerrain		= NULL;
		pSrc->m_FaceIndex		= 0xFFFFFFFF;
		i3Vector::Zero( &pSrc->m_Normal );
		i3Vector::Zero( &pSrc->m_Intersect );
		pSrc->m_T				= 0.f;
	}

	inline
	void			ContactReportCopyTo( I3_PHYSIX_CONTACT_REPORT * pDest, I3_PHYSIX_CONTACT_REPORT * pSrc )
	{
		UINT32 i;
		pDest->m_Events			= pSrc->m_Events;
		pDest->m_pOwnShapeSet	= pSrc->m_pOwnShapeSet;
		pDest->m_pDestShapeSet	= pSrc->m_pDestShapeSet;
		pDest->m_nPointCount	= pSrc->m_nPointCount;
		for( i = 0; i < pSrc->m_nPointCount; i++ )
		{
			pDest->m_FaceIndex[ i ]	= pSrc->m_FaceIndex[ i ];
			i3Vector::Copy( &pDest->m_Intersect[ i ], &pSrc->m_Intersect[ i ] );
			i3Vector::Copy( &pDest->m_Normal[ i ], &pSrc->m_Normal[ i ] );
			pDest->m_rNormalForce[ i ] = pSrc->m_rNormalForce[ i ];
			pDest->m_pShape[ i ] = pSrc->m_pShape[ i ];
		}
		pDest->m_Reserved[0]	= pSrc->m_Reserved[0];
		pDest->m_Reserved[1]	= pSrc->m_Reserved[1];
		pDest->m_Reserved[2]	= pSrc->m_Reserved[2];
		pDest->m_Reserved[3]	= pSrc->m_Reserved[3];
	}

	inline
	void			ContactReportClear( I3_PHYSIX_CONTACT_REPORT * pSrc )
	{
		INT32 i;
		pSrc->m_Events		= 0;
		pSrc->m_nPointCount = 0;
		for( i = 0; i < I3_PHYSIX_CONTACT_REPORT_MAX_POINT; i++ )
		{
			pSrc->m_FaceIndex[ i ]	= 0xFFFFFFFF;
			i3Vector::Zero( &pSrc->m_Intersect[ i ] );
			i3Vector::Zero( &pSrc->m_Normal[ i ] );
			pSrc->m_rNormalForce[ i ] = 0.f;
			pSrc->m_pShape[ i ] = NULL;
		}
		pSrc->m_pDestShapeSet	= NULL;
		pSrc->m_pOwnShapeSet	= NULL;
		pSrc->m_Reserved[0]	= 0;		pSrc->m_Reserved[1]	= 0;		pSrc->m_Reserved[2]	= 0;		pSrc->m_Reserved[3]	= 0;
	}
};

#endif

#endif
