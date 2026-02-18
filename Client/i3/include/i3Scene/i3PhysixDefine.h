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


struct I3_PHYSIX_TRI
{
	UINT32		m_Index[3] = { 0, 0, 0 };
	UINT16		m_MaterialIndex = 0;
} ;

struct I3_PHYSIX_TERRAIN
{
	UINT16		m_Terrain = 0;
	UINT16		m_Reverb = 0;
	UINT8		m_Color[4] = { 0, 0, 0, 0 };
	INT32		m_Thickness = 0;
	INT32		m_PortalID = 0;
	INT32		pad[2] = { 0 };
} ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

struct I3_TERRAIN_MATERIAL
{
	REAL32		m_StaticFriction;
	REAL32		m_DynamicFriction;
	REAL32		m_Restitution;
} ;

class i3PhysixShapeSet;

struct I3_PHYSIX_HIT_RESULT
{	
	i3PhysixShapeSet *	m_pDestShapeSet = nullptr;
	NxShape *			m_pHitShape = nullptr;
	I3_PHYSIX_TERRAIN *	m_pTerrain = nullptr;
	INT32				m_FaceIndex = 0xFFFFFFFF;
	VEC3D				m_Normal;
	VEC3D				m_Intersect;
	REAL32				m_T = 0.0f;
} ;


// Contact report는 하나의 Pair 정보를 갖습니다.
struct I3_PHYSIX_CONTACT_INFO
{
	UINT32				m_FaceIndex = 0xFFFFFFFF;
	VEC3D				m_Intersect;
	VEC3D				m_Normal;
	REAL32				m_rNormalForce = 0.0f;
	NxShape *			m_pShape = nullptr;

	void	Reset( void)
	{
		m_FaceIndex = 0xFFFFFFFF;
		i3Vector::Zero( &m_Intersect);
		i3Vector::Zero( &m_Normal);
		m_rNormalForce = 0.f;
		m_pShape = nullptr;
	}

	void	CopyTo( I3_PHYSIX_CONTACT_INFO * pDest)
	{
		pDest->m_FaceIndex = m_FaceIndex;
		pDest->m_Intersect = m_Intersect;
		pDest->m_Normal = m_Normal;
		pDest->m_rNormalForce = m_rNormalForce;
		pDest->m_pShape = m_pShape;
	}
};

#include "i3Base/pool/class_storage_pool.h"
#include "i3Base/smart_ptr/intrusive_ptr.h"
#include "i3Base/smart_ptr/ref_count.h"

namespace i3
{
	template<> struct reserve_storage_pool<struct I3_PHYSIX_CONTACT_REPORT> {  enum { value = 32 * 8 };  };
}

#pragma warning(push)
#pragma warning(disable : 4275)

struct I3_EXPORT_SCENE I3_PHYSIX_CONTACT_REPORT : i3::class_storage_pool<I3_PHYSIX_CONTACT_REPORT, 
									i3::static_ref_count<I3_PHYSIX_CONTACT_REPORT> >
{
	~I3_PHYSIX_CONTACT_REPORT();

	UINT32				m_Events = 0;
	i3PhysixShapeSet *	m_pOwnShapeSet = nullptr;
	i3PhysixShapeSet *	m_pDestShapeSet = nullptr;
	i3::vector<I3_PHYSIX_CONTACT_INFO*> m_ContactList;
};

#pragma warning(pop)

typedef i3::intrusive_ptr<I3_PHYSIX_CONTACT_REPORT>		I3_PHYSIX_CONTACT_REPORT_PTR;


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
		pSrc->m_pDestShapeSet = nullptr;
		pSrc->m_pHitShape		= nullptr;
		pSrc->m_pTerrain		= nullptr;
		pSrc->m_FaceIndex		= 0xFFFFFFFF;
		i3Vector::Zero( &pSrc->m_Normal );
		i3Vector::Zero( &pSrc->m_Intersect );
		pSrc->m_T				= 0.f;
	}

	I3_EXPORT_SCENE
	void			ContactReportCopyTo( I3_PHYSIX_CONTACT_REPORT * pDest, I3_PHYSIX_CONTACT_REPORT * pSrc );
	
	I3_EXPORT_SCENE
	void			ContactReportClear( I3_PHYSIX_CONTACT_REPORT * pSrc );
};

#endif

#endif
