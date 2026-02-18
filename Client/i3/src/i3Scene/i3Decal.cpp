#include "i3SceneDef.h"
#include "i3Decal.h"
#include "i3SceneTracer.h"

#define I3SG_DECAL_DEFAULT_EPSILON		0.5f

I3_CLASS_INSTANCE( i3DecalDrawInfo);

void i3DecalDrawInfo::SetTextureCoord( VEC2D *pMin, VEC2D * pMax)
{
	i3Vector::Copy( &m_UV, pMin );
	i3Vector::Sub( &m_UVScale, pMax, pMin );
}

void i3DecalDrawInfo::AssignTextureCoord( VEC3D * pPosition, VEC3D * pTangent, VEC3D * pBinormal)
{
	UINT8 i;
	VEC3D vec;
	REAL32 s, t;

	for( i = 0; i < m_NewVertexCount; i++)
	{	
		i3Vector::Sub( &vec, &m_NewVertexArray[i], pPosition );
		s = (i3Vector::Dot( &vec, pTangent) * m_One_over_W ) + 0.5f;
		t = 0.5f - (i3Vector::Dot( &vec, pBinormal) * m_One_over_H);
		
		i3Vector::Set( &m_UVArray[i], s * i3Vector::GetU( &m_UVScale ) + i3Vector::GetU( &m_UV), t * i3Vector::GetV( &m_UVScale ) + i3Vector::GetV( &m_UV) );
	}
}

void i3DecalDrawInfo::SetOneOverSize( REAL32 width, REAL32 height)
{
	m_One_over_W = 1.0f / width;
	m_One_over_H = 1.0f / height;
}

I3_CLASS_INSTANCE( i3Decal);

void i3Decal::SetSize( VEC3D * pVec)
{
	m_Width = pVec->x; 
	m_Height = pVec->y; 
	m_Depth = pVec->z;
}

void i3Decal::CalcBoundaryPlane(void)
{
	REAL32 dot = i3Vector::Dot( &m_Position, &m_Tangent );

	i3Vector::Set( &m_PlaneLeft, m_Tangent.x, m_Tangent.y, m_Tangent.z, m_Width * 0.5F - dot); 
	i3Vector::Set( &m_PlaneRight, -getX( &m_Tangent ), -getY( &m_Tangent ), -getZ( &m_Tangent ), m_Width * 0.5F + dot);
	
	i3Vector::Cross( &m_Binormal, &m_Normal, &m_Tangent);

	dot = i3Vector::Dot( &m_Position, &m_Binormal);
	i3Vector::Set( &m_PlaneBottom, getX( &m_Binormal ), getY( &m_Binormal ), getZ( &m_Binormal ), m_Height * 0.5F - dot);
	i3Vector::Set( &m_PlaneTop, -getX( &m_Binormal ), -getY( &m_Binormal ), -getZ( &m_Binormal ), m_Height * 0.5F + dot);
	
	dot = i3Vector::Dot( &m_Position, &m_Normal );
	i3Vector::Set( &m_PlaneFront, -getX( &m_Normal ), -getY( &m_Normal ), -getZ( &m_Normal ), m_Depth + dot);
	i3Vector::Set( &m_PlaneBack, getX( &m_Normal ), getY( &m_Normal ), getZ( &m_Normal ), m_Depth - dot);
}

void i3Decal_InitWorldHitPair()
{	
#if defined (I3_PHYSX)
	g_pPhysixCtx->setActorGroupPairFlags( I3SG_DECAL_ACTOR_GROUP, g_pPhysixCtx->m_nWorldHitCollisionGroup, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH);
	g_pPhysixCtx->setActorGroupPairFlags( I3SG_DECAL_ACTOR_GROUP, g_pPhysixCtx->m_nObjectHitCollisionGroup, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH);
	g_pPhysixCtx->setActorGroupPairFlags( I3SG_DECAL_ACTOR_GROUP, g_pPhysixCtx->m_nObjectHitCollisionGroup+1, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH);	// Object_Whole

#endif
}

void i3Decal_ReleaseWorldHitPair()
{	
#if defined (I3_PHYSX)
	g_pPhysixCtx->setActorGroupPairFlags( g_pPhysixCtx->m_nWorldHitCollisionGroup, I3SG_DECAL_ACTOR_GROUP, NX_IGNORE_PAIR);
	g_pPhysixCtx->setActorGroupPairFlags( g_pPhysixCtx->m_nObjectHitCollisionGroup, I3SG_DECAL_ACTOR_GROUP, NX_IGNORE_PAIR);	// Object_Hit
	g_pPhysixCtx->setActorGroupPairFlags( g_pPhysixCtx->m_nObjectHitCollisionGroup+1, I3SG_DECAL_ACTOR_GROUP, NX_IGNORE_PAIR);	// Object_Whole

#endif
}
