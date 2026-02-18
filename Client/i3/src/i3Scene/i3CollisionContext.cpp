#include "i3SceneDef.h"
#include "i3CollisionContext.h"
#include "i3CollideeGroup.h"
#include "i3Collidee.h"
#include "i3CollideeLine.h"
#include "i3CollideeSphere.h"
#include "i3CollideeCylinder.h"
#include "i3CollideeMesh.h"
#include "i3CollideeCapsule.h"
#include "i3Collider.h"
#include "i3ColliderSet.h"

#define		_I3_COLLISION_STYLE_NODE		0x00000001

#define		_I3_COLLISION_STATE_TRANSFORMED	0x00000001

struct I3_COLLISION_GROUP
{
	UINT32	m_Style = 0;
	UINT32	m_SrcMask = 0;
	UINT32	m_TargetMask = 0;
	char	m_szGroupName[32] = { 0 };
} ;

I3_CLASS_INSTANCE( i3CollisionContext);

I3_COLLISION_RESULT *	g_pResultList = nullptr;
UINT32					g_reserveResult = 0;
UINT32					g_ResultBuffSize = 0;

#if defined( I3_DEBUG)
	UINT32					g_ExceedResultCount = 0;
#endif


i3CollisionContext::i3CollisionContext()
{
	m_pWorldRoot = i3Node::new_object();
}

I3_COLLISION_RESULT *	i3CollisionContext::GetResult(void)
{
	return g_pResultList + g_reserveResult;
}

void			i3CollisionContext::reserveResult( UINT32 cnt)
{
	g_reserveResult = cnt;
}

void			i3CollisionContext::resetResult(void)
{
	g_reserveResult = 0;
}

/*
I3_COLLISION_RESULT *	i3CollisionContext::GetCurrentResult(void)
{
	return g_pResultList + g_ResultCount;
}
*/

i3CollisionContext::~i3CollisionContext()
{
	RemoveAllGroup();

	I3_SAFE_RELEASE( m_pWorldRoot);

	/*
	if(m_pColList != nullptr)
	{	
		i3mem::Free( m_ColListIdx ); 
	}
	*/

	I3MEM_SAFE_FREE_POINTER( m_pColList ); 
	
	/*
	if( g_pResultList != nullptr )
	{	
		i3mem::Free( g_ResultListIdx ); 
	}
	*/

	I3MEM_SAFE_FREE_POINTER( g_pResultList ); 
}

bool i3CollisionContext::Create(INT32 numUnit, INT32 numResult)
{
	m_ColMax = numUnit;
	g_ResultBuffSize = numResult;

	/*
	if(m_pColList != nullptr)
	{		
		i3mem::Free( m_ColListIdx ); 
	}
	*/
	I3MEM_SAFE_FREE_POINTER( m_pColList ); 
	
	/*
	if( g_pResultList != nullptr )
	{
		i3mem::Free( g_ResultListIdx ); 
	}
	*/
	I3MEM_SAFE_FREE_POINTER( g_pResultList ); 


	//m_ColListIdx	= i3MemAlloc( sizeof(I3_COLLISION_UNIT) * m_ColMax);
	m_pColList		= (I3_COLLISION_UNIT *) i3MemAlloc( sizeof(I3_COLLISION_UNIT) * m_ColMax);
	I3ASSERT( m_pColList );


	//g_ResultListIdx = i3MemAlloc( sizeof(I3_COLLISION_RESULT) * g_ResultBuffSize);
	g_pResultList = (I3_COLLISION_RESULT *) i3MemAlloc( sizeof(I3_COLLISION_RESULT) * g_ResultBuffSize);
	I3ASSERT( g_pResultList != nullptr );


	Reset();

	return true;
}

/*
inline I3_COLLISION_RESULT *	AddResult(void)
{
	if( g_ResultCount >= g_ResultBuffSize)
	{
		#if defined( I3_DEBUG)
		g_ExceedResultCount ++;

		i3Log( "i3CollisionContext::AddResult()", "!!! EXCEED COLLISION RESULT POOL !!! : %d\n", g_ExceedResultCount);
		#endif
		return nullptr;
	}

	g_ResultCount++;

	return g_pResultList + (g_ResultCount - 1);
}
*/

void	i3CollisionContext::AddGroup( UINT32 GrpStyle, UINT32 SrcMask, UINT32 TargetMask)
{
	I3_COLLISION_GROUP * pNew;

	pNew = (I3_COLLISION_GROUP *) i3MemAlloc( sizeof(I3_COLLISION_GROUP));

	pNew->m_Style = GrpStyle;
	pNew->m_SrcMask = SrcMask;
	pNew->m_TargetMask = TargetMask;

	m_GroupList.push_back( pNew);
}
/*
void	i3CollisionContext::AddGroup( const char * pszGroupName, UINT32 GrpStyle, UINT32 SrcMask, UINT32 TargetMask)
{
	I3_COLLISION_GROUP * pNew;

	pNew = (I3_COLLISION_GROUP *) i3MemAlloc( sizeof(I3_COLLISION_GROUP));

	pNew->m_Style = GrpStyle;
	pNew->m_SrcMask = SrcMask;
	pNew->m_TargetMask = TargetMask;
	i3::string_ncopy_nullpad( pNew->m_szGroupName, pszGroupName, 32);

	m_GroupList.Add( pNew);
}*/

INT32 i3CollisionContext::FindCollisionGroupIndexByName( const char * pszGroupName)
{
	I3_COLLISION_GROUP * pGroup;

	for( size_t i = 0; i < m_GroupList.size(); i++)
	{
		pGroup = m_GroupList[i];

		if( i3::generic_is_iequal( pszGroupName, pGroup->m_szGroupName) )
		{
			return i;
		}
	}

	return -1;
}

void	i3CollisionContext::RemoveAllGroup(void)
{
	
	I3_COLLISION_GROUP * pGroup;

	for(size_t i = 0; i < m_GroupList.size(); i++)
	{
		pGroup = m_GroupList[i];

		i3MemFree( pGroup);
	}

	m_GroupList.clear();
}

void i3CollisionContext::Add( MATRIX * pMatrix, i3ColliderSet * pCollSet)
{
	I3_COLLISION_UNIT * pUnit;

	if( m_ColCount == m_ColMax)
	{
		#if defined( I3_DEBUG)
		m_ExceedCount ++;
		static UINT32 s_MaxExceedCount = 0;
		if( m_ExceedCount > s_MaxExceedCount )
		{
			s_MaxExceedCount = m_ExceedCount;
			i3Log( "i3CollisionContext::Add()", "!!! Exceed limitation of prepared collision pool %d !!!", m_ExceedCount);
		}
		#endif
		return;
	}

	pUnit = m_pColList + m_ColCount;

	i3Matrix::Copy( &pUnit->m_Matrix, pMatrix);
	pUnit->m_pNode = pCollSet;
	pUnit->m_Style = 0;
	pUnit->m_State = 0;

	m_ColCount++;
}

void i3CollisionContext::AddWorld( i3Node * pNode)
{
	m_pWorldRoot->AddChild( pNode);
}

void i3CollisionContext::RemoveWorld( i3Node * pNode)
{
	m_pWorldRoot->RemoveChild( pNode);
}

void i3CollisionContext::RemoveAllWorld(void)
{
	m_pWorldRoot->RemoveAllChild();
}

void i3CollisionContext::Reset(void)
{
	SoftReset();

	RemoveAllWorld();
}

void i3CollisionContext::SoftReset(void)
{
	m_ColCount = 0;
	//g_ResultCount = 0;

#if defined( I3_DEBUG)
	m_ExceedCount = 0;
	g_ExceedResultCount = 0;
#endif
}

inline UINT32 TestLineToLine( I3_COLLISION_RESULT_CONTEXT *, i3CollideeLine * pSrcCol, i3CollideeLine * pDestCol)
{
	/* 
	i3CollideeLine * pSrc, * pDest;

	pSrc = (i3CollideeLine *) pSrcCol->GetTransformedCollidee();
	pDest = (i3collideeLine *) pSrcCol->
	*/

	return 0;
}

inline UINT32 TestLineToSphere( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeLine * pSrc, i3CollideeSphere * pDest)
{
	VEC3D pc;
	REAL32 t;

	if( i3Math::IntersectLineToSphere( pSrc->GetStart(), pSrc->GetEnd(), pDest->GetCenter(), pDest->GetSquareRadius(), &pc, &t))
	{
		I3_COLLISION_RESULT * pResult;

		if((pCtx->m_Count + 1) > pCtx->m_LimitCount)		// Result 개수 제한 초과
			return 0;

		if( pCtx->m_Style & I3_COLLISION_STYLE_NEAREST_ONE)
		{
			if( pCtx->m_LastT < t)
				return 0;

			pResult = pCtx->m_pResult;
			pCtx->m_Count = 1;
		}
		else
		{
			pResult = pCtx->m_pResult + pCtx->m_Count;

			pCtx->m_Count++;
		}

		pResult->m_pSrc = pSrc;
		pResult->m_pDest = pDest;
		pResult->m_T = t;
		pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

		// 실제로는 사용되는 경우가 없다.
		// if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_INTERSECT)
		{
			i3Vector::Lerp( &pResult->m_Intersect, pSrc->GetStart(), pSrc->GetEnd(), t);

			//if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_NORMAL)
			{
				i3Vector::Sub( &pResult->m_Normal, &pResult->m_Intersect, pDest->GetCenter());
				i3Vector::Normalize( &pResult->m_Normal, &pResult->m_Normal);
			}
		}
		
		return 1;
	}

	return 0;
}

inline UINT32 TestLineToCapsule( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeLine * pSrc, i3CollideeCapsule * pDest)
{
	VEC3D pc, norm;
	REAL32 t;
	
	if( i3Math::IntersectCapsuleToLine( pDest->getStart(), pDest->getDirection(), pDest->getLength(), pDest->getRadius(),
										pSrc->GetStart(), pSrc->GetEnd(), &pc, &norm, &t))
	{
		I3_COLLISION_RESULT * pResult;

		if((pCtx->m_Count + 1) > pCtx->m_LimitCount)		// Result 개수 제한 초과
			return 0;

		if( pCtx->m_Style & I3_COLLISION_STYLE_NEAREST_ONE)
		{
			if( pCtx->m_LastT < t)
				return 0;

			pResult = pCtx->m_pResult;
			pCtx->m_Count = 1;
		}
		else
		{
			pResult = pCtx->m_pResult + pCtx->m_Count;

			pCtx->m_Count++;
		}

		pResult->m_pSrc = pSrc;
		pResult->m_pDest = pDest;
		pResult->m_T = t;
		i3Vector::Copy( &pResult->m_Intersect, &pc);		
		pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

		//if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_NORMAL)
		{
			i3Vector::Copy( &pResult->m_Normal, &norm);
		}
		
		return 1;
	}

	return 0;
}

inline UINT32 TestLineToCylinder( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeLine * pSrc, i3CollideeCylinder * pDestCol)
{
	return 0;
}

inline UINT32 TestLineToMesh( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeLine * pSrc, i3CollideeMesh * pDest)
{
#if 1
	REAL32 rr, t;
	VEC3D pc;

	rr = pDest->GetRadius() * pDest->GetRadius();

	if( i3Math::IntersectLineToSphere( pSrc->GetStart(), pSrc->GetEnd(), 
		pDest->GetCenter(), rr, &pc, &t))
	{
		return pDest->TestLine( pSrc, pCtx);
	}
	
	return 0;
#else
	return pDest->TestLine( pSrc, pCtx);
#endif
}

inline UINT32 TestSphereToSphere( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeSphere * pSrc, i3CollideeSphere * pDest)
{
	VEC3D pc, norm;
	REAL32 t;

	if( i3Math::IntersectSphereToSphere( pSrc->GetCenter(), pSrc->GetRadius(), pDest->GetCenter(), pDest->GetRadius(), &pc, &norm, &t))
	{
		I3_COLLISION_RESULT * pResult;

		if((pCtx->m_Count + 1) > pCtx->m_LimitCount)	// Result 개수 제한 초과
			return 0;

		pResult = pCtx->m_pResult + pCtx->m_Count;
		pCtx->m_Count ++;

		pResult->m_pSrc = pSrc;
		pResult->m_pDest = pDest;
		pResult->m_T = t;
		pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

		if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_INTERSECT)
			i3Vector::Copy( & pResult->m_Intersect, &pc);

		if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_NORMAL)
			i3Vector::Copy( & pResult->m_Normal, &norm);

		return 1;
	}

	return 0;
}

inline UINT32 TestSphereToCylinder( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeSphere * pSrcCol, i3CollideeCylinder * pDest)
{
	return 0;
}

inline UINT32 TestSphereToMesh( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeSphere * pSrc, i3CollideeMesh * pDest)
{
#if 1
	VEC3D pc, norm;
	REAL32 t;

	if( i3Math::IntersectSphereToSphere( pSrc->GetCenter(), pSrc->GetRadius(), 
			pDest->GetCenter(), pDest->GetRadius(), &pc, &norm, &t))
	{
		return pDest->TestSphere( pSrc, pCtx);
	}

	return 0;
#else
	return pDest->TestSphere( pSrc, pCtx);
#endif
}

inline UINT32 TestSphereToCapsule( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeSphere * pSrc, i3CollideeCapsule * pDest)
{
	VEC3D pc, norm;
	REAL32 t;

	if( i3Math::IntersectCapsuleToSphere( pDest->getStart(), pDest->getDirection(), pDest->getLength(), pDest->getRadius(), 
										pSrc->GetCenter(), pSrc->GetRadius(), &pc, &norm, &t))
	{
		I3_COLLISION_RESULT * pResult;

		if((pCtx->m_Count + 1) > pCtx->m_LimitCount)	// Result 개수 제한 초과
			return 0;

		pResult = pCtx->m_pResult + pCtx->m_Count;
		pCtx->m_Count ++;

		pResult->m_pSrc = pSrc;
		pResult->m_pDest = pDest;
		pResult->m_T = t;
		pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

		if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_INTERSECT)
			i3Vector::Copy( & pResult->m_Intersect, &pc);

		if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_NORMAL)
			i3Vector::Copy( & pResult->m_Normal, &norm);

		return 1;
	}

	return 0;
}

inline UINT32 TestCylinderToCylinder( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeCylinder * pSrc, i3CollideeCylinder * pDest)
{
	return 0;
}

inline UINT32 TestCylinderToMesh( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeCylinder * pSrc, i3CollideeMesh * pDest)
{
	return 0;
}

inline UINT32 TestMeshToMesh( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeMesh * pSrc, i3CollideeMesh * pDest)
{
	return 0;
}

inline UINT32 TestCapsuleToCapsule( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeCapsule * pSrc, i3CollideeCapsule * pDest)
{
	VEC3D pc1, pc2, norm;
	REAL32 t;

	if( i3Math::IntersectCapsuleToCapsule( pSrc->getStart(), pSrc->getDirection(), pSrc->getLength(), pSrc->getRadius(),
										   pDest->getStart(), pDest->getDirection(), pDest->getLength(), pDest->getRadius(), 
										   &pc1, &pc2, &norm, &t))
	{
		I3_COLLISION_RESULT * pResult;

		if((pCtx->m_Count + 1) > pCtx->m_LimitCount)		// Result 개수 제한 초과
			return 0;

		if( pCtx->m_Style & I3_COLLISION_STYLE_NEAREST_ONE)
		{
			if( pCtx->m_LastT < t)
				return 0;

			pResult = pCtx->m_pResult;
			pCtx->m_Count = 1;
		}
		else
		{
			pResult = pCtx->m_pResult + pCtx->m_Count;

			pCtx->m_Count++;
		}

		pResult->m_pSrc = pSrc;
		pResult->m_pDest = pDest;
		pResult->m_T = t;		
		pResult->m_pDestColliderSet = pCtx->m_pDestColliderSet;

		if( pCtx->m_Style & I3_COLLISION_STYLE_CALC_INTERSECT)
		{
			i3Vector::Copy( &pResult->m_Intersect, &pc1);		//	캡슐1의 교점
			i3Vector::Copy( &pResult->m_Normal, &pc2);			//	노멀값이 아니라 캡슐2의 교점으로 넘깁니다.
		}
		
		return 1;
	}

	return 0;
}

inline UINT32 TestCapsuleToMesh( I3_COLLISION_RESULT_CONTEXT * pCtx, i3CollideeCapsule * pSrc, i3CollideeMesh * pDest)
{
	return pDest->TestCapsule( pSrc, pCtx);
}

UINT32 i3CollisionContext::_CalcWithWorld( i3Collidee * pTxdCol, i3Node * pNode, I3_COLLISION_RESULT_CONTEXT * pCtx, UINT32 mask)
{
	UINT32 c = 0, i;
	I3_COLLISION_GROUP * pGroup;

	if( pNode->GetBound() != nullptr)
	{
		i3BoundBox * pBox = pNode->GetBound();

		if( i3::same_of<i3CollideeLine*>(pTxdCol))
		{
			i3CollideeLine * pLine = (i3CollideeLine *) pTxdCol;

			#if defined( I3_FIXED_POINT)
				VEC3D vmin, vmax;

				i3Vector::Copy( &vmin, pBox->GetMin());
				i3Vector::Copy( &vmax, pBox->GetMax());

				if( ContainAABBoxLine( pLine->GetStart(), pLine->GetEnd(), &vmin, &vmax, 10000000000000.0f) == false)
					return 0;
			#else
				if( ContainAABBoxLine( pLine->GetStart(), pLine->GetEnd(), pBox->GetMin(), pBox->GetMax(), 10000000000000.0f) == false)
					return 0;
			#endif
		}
		else if( i3::same_of<i3CollideeSphere*>(pTxdCol))
		{
			i3CollideeSphere * pSphere = (i3CollideeSphere *) pTxdCol;

			#if defined( I3_FIXED_POINT)
				VEC3D vmin, vmax;

				i3Vector::Copy( &vmin, pBox->GetMin());
				i3Vector::Copy( &vmax, pBox->GetMax());

				if( ContainAABBoxSphere( pSphere->GetCenter(), pSphere->GetRadius(), &vmin, &vmax) == false)
					return 0;
			#else
				if( ContainAABBoxSphere( pSphere->GetCenter(), pSphere->GetRadius(), pBox->GetMin(), pBox->GetMax()) == false)
					return 0;
			#endif
		}
		else if( i3::same_of<i3CollideeCylinder*>(pTxdCol))
		{
		}
		else if( i3::same_of<i3CollideeMesh*>(pTxdCol))
		{
		}
		else if( i3::same_of<i3CollideeCapsule*>(pTxdCol))
		{			
		}
	}

	if( i3::same_of<i3ColliderSet*>(pNode))
	{
		i3ColliderSet * pColSet = (i3ColliderSet *) pNode;

		pCtx->m_pDestColliderSet = pColSet;

		pGroup = m_GroupList[ pColSet->GetGroup()];

		if((pGroup->m_SrcMask & mask) != 0)
		{
			if( i3::same_of<i3CollideeLine*>(pTxdCol))
			{
				// Line
				for( i = 0; i < (UINT32) pColSet->GetColliderCount(); i++)
				{
					i3Collider * pDestCol = pColSet->GetCollider( i);

					c += TestLineToMesh( pCtx, (i3CollideeLine *) pTxdCol, (i3CollideeMesh *) pDestCol->GetCollidee());
				}
			}
			else if( i3::same_of<i3CollideeSphere*>(pTxdCol))
			{
				// Sphere
				for( i = 0; i < (UINT32) pColSet->GetColliderCount(); i++)
				{
					i3Collider * pDestCol = pColSet->GetCollider( i);

					c += TestSphereToMesh( pCtx, (i3CollideeSphere *) pTxdCol, (i3CollideeMesh *) pDestCol->GetCollidee() );
				}
			}
			else if( i3::same_of<i3CollideeCylinder*>(pTxdCol))
			{
				for( i = 0; i < (UINT32) pColSet->GetColliderCount(); i++)
				{
					i3Collider * pDestCol = pColSet->GetCollider( i);

					c += TestCylinderToMesh( pCtx, (i3CollideeCylinder *) pTxdCol, (i3CollideeMesh *) pDestCol->GetCollidee());
				}
			}
			else if( i3::same_of<i3CollideeMesh*>(pTxdCol))
			{
				// Mesh
				for( i = 0; i < (UINT32) pColSet->GetColliderCount(); i++)
				{
					i3Collider * pDestCol = pColSet->GetCollider( i);

					c += TestMeshToMesh( pCtx, (i3CollideeMesh *) pTxdCol, (i3CollideeMesh *) pDestCol->GetCollidee());
				}
			}
			else if( i3::same_of<i3CollideeCapsule*>(pTxdCol))
			{
				// Capsule
				for( i = 0; i < (UINT32) pColSet->GetColliderCount(); i++)
				{
					i3Collider * pDestCol = pColSet->GetCollider( i);

					c += TestCapsuleToMesh( pCtx, (i3CollideeCapsule *) pTxdCol, (i3CollideeMesh *) pDestCol->GetCollidee());
				}
			}
		}
	}

	for( i = 0; i < (UINT32) pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		c += _CalcWithWorld( pTxdCol, pChild, pCtx, mask);
	}

	return c;
}

UINT32	i3CollisionContext::CollisionTest( i3Collidee * pCollidee, UINT32 GroupIdx, UINT32 Style, i3EventReceiver * pEventRecv)
{
	UINT32 i, Count = 0, j, OldCount;
	i3ColliderSet * pDestSet;
	I3_COLLISION_UNIT * pUnit = m_pColList;
	I3_COLLISION_GROUP * pSrcGroup, * pTargetGroup;
	i3Collider * pTargetCol;
	i3Collidee * pDest;
	UINT32 mask;
	I3_COLLISION_RESULT_CONTEXT ctx;

	// 가장 가까운 충돌점 하나만 반환하는 경우,
	// 비교를 위해 가장 먼점을 미리 설정해 둔다.
	// 잠재적 버그 : 만약 정말 t == 1인 위치에서 교점이 발생했을 경우
	// 올바르게 교점이나 Normal 값이 들어가지 않는다.
	ctx.m_LastT = 1.0f;
	ctx.m_LimitCount = g_ResultBuffSize;
	ctx.m_pResult = g_pResultList + g_reserveResult;
	ctx.m_Count = 0;
	ctx.m_Style = Style;

	pSrcGroup = m_GroupList[ GroupIdx];
	mask = pSrcGroup->m_TargetMask;

	if( pSrcGroup->m_Style & I3_COLLISION_GRP_STYLE_WITH_WORLD)
	{
		Count += _CalcWithWorld( pCollidee, m_pWorldRoot, &ctx, mask);
	}

	for( i = 0; i < m_ColCount; i++, pUnit++)
	{
		pDestSet = pUnit->m_pNode;

		pTargetGroup = m_GroupList[ pDestSet->GetGroup()];
		if( (mask & pTargetGroup->m_SrcMask) == 0)
		{
			// 서로 간에 Collision할 필요가 없기 때문에 건너뛴다.
			continue;
		}

		if( (pEventRecv != nullptr) && 
			(pEventRecv != pDestSet->GetEventReceiver()))
		{
			//특정 EventReceiver를 지정한 경우지정한 EventReceiver가 아닌경우는 Skip한다.
			continue;
		}

		OldCount = Count;

		ctx.m_pDestColliderSet = pDestSet;

		for( j = 0; j < (UINT32) pDestSet->GetColliderCount(); j++)
		{
			pTargetCol = pDestSet->GetCollider( j);

			pDest = pTargetCol->GetTransformedCollidee();

			if( i3::same_of<i3CollideeLine*>(pCollidee))
			{
				switch( pTargetCol->GetType())
				{
					case I3_COLLIDER_NONE :		break;
					case I3_COLLIDER_LINE :		Count += TestLineToLine( &ctx, (i3CollideeLine *)pCollidee, (i3CollideeLine *) pDest);	break;
					case I3_COLLIDER_SPHERE :	Count += TestLineToSphere( &ctx, (i3CollideeLine *)pCollidee, (i3CollideeSphere *) pDest);	break;
					case I3_COLLIDER_CYLINDER :	Count += TestLineToCylinder( &ctx, (i3CollideeLine *)pCollidee, (i3CollideeCylinder *) pDest);	break;
					case I3_COLLIDER_MESH :		Count += TestLineToMesh( &ctx, (i3CollideeLine *)pCollidee, (i3CollideeMesh *) pDest);	break;
					case I3_COLLIDER_CAPSULE :	Count += TestLineToCapsule( &ctx, (i3CollideeLine *) pCollidee, (i3CollideeCapsule *) pDest);	break;
				}
			}
			else if( i3::same_of<i3CollideeSphere*>(pCollidee))
			{
				switch( pTargetCol->GetType())
				{
					case I3_COLLIDER_NONE :		break;
					case I3_COLLIDER_LINE :		Count += TestLineToSphere( &ctx, (i3CollideeLine *) pDest, (i3CollideeSphere *) pCollidee);	break;
					case I3_COLLIDER_SPHERE :	Count += TestSphereToSphere( &ctx, (i3CollideeSphere *) pCollidee, (i3CollideeSphere *) pDest);	break;
					case I3_COLLIDER_CYLINDER :	Count += TestSphereToCylinder( &ctx, (i3CollideeSphere *) pCollidee, (i3CollideeCylinder *) pDest);	break;
					case I3_COLLIDER_MESH :		Count += TestSphereToMesh( &ctx, (i3CollideeSphere *) pCollidee, (i3CollideeMesh *) pDest);	break;
					case I3_COLLIDER_CAPSULE :	Count += TestSphereToCapsule( &ctx, (i3CollideeSphere *) pCollidee, (i3CollideeCapsule *) pDest);	break;
				}
			}
			else if( i3::same_of<i3CollideeMesh*>(pCollidee))
			{
				switch( pTargetCol->GetType())
				{
					case I3_COLLIDER_NONE :		break;
					case I3_COLLIDER_LINE :		Count += TestLineToMesh( &ctx, (i3CollideeLine *) pDest, (i3CollideeMesh *) pCollidee);	break;
					case I3_COLLIDER_SPHERE :	Count += TestSphereToMesh( &ctx, (i3CollideeSphere *) pDest, (i3CollideeMesh *) pCollidee);	break;
					case I3_COLLIDER_CYLINDER :	Count += TestCylinderToMesh( &ctx, (i3CollideeCylinder *) pDest, (i3CollideeMesh *) pCollidee);	break;
					case I3_COLLIDER_MESH :		Count += TestMeshToMesh( &ctx, (i3CollideeMesh *) pCollidee, (i3CollideeMesh *) pDest);	break;
					case I3_COLLIDER_CAPSULE :	Count += TestCapsuleToMesh( &ctx, (i3CollideeCapsule *) pDest, (i3CollideeMesh *) pCollidee);	break;
				}
			}
			/*
				현재는 구현되어있지 않음.
			else if( pCollidee->IsExactTypeOf( i3CollideeCylinder::GetClassMeta()))
			{
				switch( pTargetCol->GetType())
				{
					case I3_COLLIDER_LINE :		Count += TestLineToCylinder( Style, pTargetCol, pCollidee);	break;
					case I3_COLLIDER_SPHERE :	Count += TestSphereToCylinder( Style, pTargetCol, pCollidee);	break;
					case I3_COLLIDER_CYLINDER :	Count += TestCylinderToCylinder( Style, pCollidee, pTargetCol);	break;
					case I3_COLLIDER_MESH :		Count += TestCylinderToMesh( Style, pCollidee, pTargetCol);	break;
				}
			}
			*/
			else if( i3::same_of<i3CollideeCapsule*>(pCollidee))
			{
				switch( pTargetCol->GetType())
				{
					case I3_COLLIDER_LINE :		Count += TestLineToCapsule( &ctx, (i3CollideeLine *) pDest, (i3CollideeCapsule *) pCollidee);	break;
					case I3_COLLIDER_SPHERE :	Count += TestSphereToCapsule( &ctx, (i3CollideeSphere *) pDest, (i3CollideeCapsule *) pCollidee);	break;
					case I3_COLLIDER_CYLINDER :	break;
					case I3_COLLIDER_MESH :		Count += TestCapsuleToMesh( &ctx, (i3CollideeCapsule *) pCollidee, (i3CollideeMesh *) pDest);	break;
					case I3_COLLIDER_CAPSULE :	Count += TestCapsuleToCapsule( &ctx, (i3CollideeCapsule *) pDest, (i3CollideeCapsule *) pCollidee);	break;
				}
			}
		}

		if( (pTargetGroup->m_Style & I3_COLLISION_GRP_STYLE_NOTIFY) && 
			(pDestSet->GetEventReceiver() != nullptr) &&
			(OldCount != Count))
		{
			i3EventReceiver * pObj = pDestSet->GetEventReceiver();

			pObj->OnEvent( I3_EVT_HIT, pDestSet, (UINT32) GetResult(), I3_EVT_CODE_ACTIVATE);
		}
	}

	return Count;
}

UINT32 i3CollisionContext::_Calc( I3_COLLISION_UNIT * pSrcUnit, i3Collider * pCollidee, I3_COLLISION_RESULT_CONTEXT * pCtx)
{
	UINT32 i, Count = 0, j, OldCount;
	i3ColliderSet * pDestSet, * pSrcSet;
	I3_COLLISION_UNIT * pUnit = m_pColList;
	I3_COLLISION_GROUP * pSrcGroup, * pTargetGroup;
	I3_COLLISION_RESULT * pOldResult;
	i3Collider * pTargetCol;
	i3Collidee * pDest, * pSrc;

	pSrcSet = pSrcUnit->m_pNode;
	pSrcGroup = m_GroupList[pSrcSet->GetGroup()];
	pSrc = pCollidee->GetTransformedCollidee();

	pCtx->m_LastT = 1.0f;
	pCtx->m_Style = pSrcSet->GetStyle();

	if( pSrcGroup->m_Style & I3_COLLISION_GRP_STYLE_WITH_WORLD)
	{
		Count += _CalcWithWorld( pCollidee->GetTransformedCollidee(), m_pWorldRoot, pCtx, pSrcGroup->m_TargetMask);
	}

	for( i = 0; i < m_ColCount; i++, pUnit++)
	{
		// 같은 Unit끼리의 검사는 하지 않는다.
		if( pUnit == pSrcUnit)
			continue;

		pDestSet = pUnit->m_pNode;
		pCtx->m_pDestColliderSet = pDestSet;

		// 동일한 Object에 속한 i3ColliderSet이라면 검사하지 않는다.
		if((pSrcSet->GetEventReceiver() != nullptr) && (pSrcSet->GetEventReceiver() == pDestSet->GetEventReceiver()))
			continue;

		pTargetGroup =m_GroupList[ pDestSet->GetGroup()];
		if( (pSrcGroup->m_TargetMask & pTargetGroup->m_SrcMask) == 0)
		{
			// 서로 간에 Collision할 필요가 없기 때문에 건너뛴다.
			continue;
		}

		OldCount = Count;
		pOldResult = pCtx->m_pResult + pCtx->m_Count;

		for( j = 0; j < (UINT32) pDestSet->GetColliderCount(); j++)
		{
			pTargetCol = pDestSet->GetCollider( j);

			pDest = pTargetCol->GetTransformedCollidee();

			switch( pCollidee->GetType())
			{
				case I3_COLLIDER_LINE :
					switch( pTargetCol->GetType())
					{
						case I3_COLLIDER_LINE :		Count += TestLineToLine( pCtx, (i3CollideeLine *)pSrc, (i3CollideeLine *) pDest);	break;
						case I3_COLLIDER_SPHERE :	Count += TestLineToSphere( pCtx, (i3CollideeLine *)pSrc, (i3CollideeSphere *) pDest);	break;
						case I3_COLLIDER_CYLINDER :	Count += TestLineToCylinder( pCtx, (i3CollideeLine *)pSrc, (i3CollideeCylinder *) pDest);	break;
						case I3_COLLIDER_MESH :		Count += TestLineToMesh( pCtx, (i3CollideeLine *)pSrc, (i3CollideeMesh *) pDest);	break;
						case I3_COLLIDER_CAPSULE :	Count += TestLineToCapsule( pCtx, (i3CollideeLine *) pSrc, (i3CollideeCapsule *) pDest);	break;

						default :		
							I3TRACE( "I3_COLLIDER_NONE : Invalid Collider Type\n");
							break;
					}
					break;

				case I3_COLLIDER_SPHERE :
					switch( pTargetCol->GetType())
					{
						case I3_COLLIDER_LINE :		Count += TestLineToSphere( pCtx, (i3CollideeLine *) pDest, (i3CollideeSphere *) pSrc);	break;
						case I3_COLLIDER_SPHERE :	Count += TestSphereToSphere( pCtx, (i3CollideeSphere *) pSrc, (i3CollideeSphere *) pDest);	break;
						case I3_COLLIDER_CYLINDER :	Count += TestSphereToCylinder( pCtx, (i3CollideeSphere *) pSrc, (i3CollideeCylinder *) pDest);	break;
						case I3_COLLIDER_MESH :		Count += TestSphereToMesh( pCtx, (i3CollideeSphere *) pSrc, (i3CollideeMesh *) pDest);	break;
						case I3_COLLIDER_CAPSULE :	Count += TestSphereToCapsule( pCtx, (i3CollideeSphere *) pSrc, (i3CollideeCapsule *) pDest);	break;

						default :		
							I3TRACE( "I3_COLLIDER_NONE : Invalid Collider Type\n");
							break;
					}
					break;

				case I3_COLLIDER_MESH :
					switch( pTargetCol->GetType())
					{
						case I3_COLLIDER_LINE :		Count += TestLineToMesh( pCtx, (i3CollideeLine *) pDest, (i3CollideeMesh *) pSrc);	break;
						case I3_COLLIDER_SPHERE :	Count += TestSphereToMesh( pCtx, (i3CollideeSphere *) pDest, (i3CollideeMesh *) pSrc);	break;
						case I3_COLLIDER_CYLINDER :	Count += TestCylinderToMesh( pCtx, (i3CollideeCylinder *) pDest, (i3CollideeMesh *) pSrc);	break;
						case I3_COLLIDER_MESH :		Count += TestMeshToMesh( pCtx, (i3CollideeMesh *) pSrc, (i3CollideeMesh *) pDest);	break;
						case I3_COLLIDER_CAPSULE :	Count += TestCapsuleToMesh( pCtx, (i3CollideeCapsule *) pDest, (i3CollideeMesh *) pSrc);	break;

						default :		
							I3TRACE( "I3_COLLIDER_NONE : Invalid Collider Type\n");
							break;
					}
					break;

				case I3_COLLIDER_CAPSULE :
					switch( pTargetCol->GetType())
					{
						case I3_COLLIDER_LINE :		Count += TestLineToCapsule( pCtx, (i3CollideeLine *) pDest, (i3CollideeCapsule *) pSrc);	break;
						case I3_COLLIDER_SPHERE :	Count += TestSphereToCapsule( pCtx, (i3CollideeSphere *) pDest, (i3CollideeCapsule *) pSrc);	break;
						case I3_COLLIDER_CYLINDER :	break;
						case I3_COLLIDER_MESH :		Count += TestCapsuleToMesh( pCtx, (i3CollideeCapsule *) pSrc, (i3CollideeMesh *) pDest);	break;
						case I3_COLLIDER_CAPSULE :	Count += TestCapsuleToCapsule( pCtx, (i3CollideeCapsule *) pSrc, (i3CollideeCapsule *) pDest);	break;

						default :		
							I3TRACE( "I3_COLLIDER_NONE : Invalid Collider Type\n");
							break;
					}
					break;

				default :
					I3TRACE( "I3_COLLIDER_NONE : Invalid Collider Type 00\n");
					break;

					/*
				case I3_COLLIDER_CYLINDER :
					switch( pTargetCol->GetType())
					{
						case I3_COLLIDER_LINE :		Count += TestLineToCylinder( Style, pTargetCol, pCollidee);	break;
						case I3_COLLIDER_SPHERE :	Count += TestSphereToCylinder( Style, pTargetCol, pCollidee);	break;
						case I3_COLLIDER_CYLINDER :	Count += TestCylinderToCylinder( Style, pCollidee, pTargetCol);	break;
						case I3_COLLIDER_MESH :		Count += TestCylinderToMesh( Style, pCollidee, pTargetCol);	break;
					}
					break;
					*/

			}
		}

		if((OldCount != Count) && (pTargetGroup->m_Style & I3_COLLISION_GRP_STYLE_NOTIFY))
		{
			if( pDestSet->GetEventReceiver() != nullptr)
			{
				pDestSet->SetResultCount( Count - OldCount);
				pDestSet->SetResult( pOldResult);

				i3EventReceiver * pReceiver = pDestSet->GetEventReceiver();
				pReceiver->OnEvent( I3_EVT_HIT, pDestSet, 0, I3_EVT_CODE_ACTIVATE);
			}
		}
	}

	return Count;
}

void i3CollisionContext::Calculate(void)
{
	UINT32 i, j;
	I3_COLLISION_UNIT * pUnit = m_pColList;
	i3ColliderSet * pColSet;
	i3Collider * pCollider;

	// 등록된 Collider들을 Transform 한다.
	// Transform
	for( i = 0; i < m_ColCount; i++, pUnit++)
	{
		pColSet = pUnit->m_pNode;

		for( j = 0; j < (UINT32) pColSet->GetColliderCount(); j++)
		{
			pCollider = pColSet->GetCollider( j);

			if( !(pCollider->GetStyle() & I3_COLLIDER_STYLE_STATIC))
				pCollider->Transform( & pUnit->m_Matrix);
		}
	}

	// 실제로 계산하는 것은 Vulcanus Game Engine 내부에서 하기 때문에
	// 현재로써는 단지 Transform만 해 둔다.
}

void i3CollisionContext::CollisionTest(void)
{
	I3_COLLISION_RESULT * pResult;
	I3_COLLISION_UNIT * pUnit = m_pColList;
	I3_COLLISION_GROUP * pGroup;
	UINT32 i, Count, j;
	i3ColliderSet * pColSet;
	I3_COLLISION_RESULT_CONTEXT ctx;

	ctx.m_Count = 0;
	ctx.m_pResult = g_pResultList + g_reserveResult;
	ctx.m_LimitCount = g_ResultBuffSize;

	for( i = 0; i < m_ColCount; i++, pUnit++)
	{
		pColSet = pUnit->m_pNode;

		pGroup = m_GroupList[pColSet->GetGroup()];
		if( (pGroup->m_Style & I3_COLLISION_GRP_STYLE_DO_TEST) == 0)
			continue;

		Count = 0;

		ctx.m_LastT = 1.0f;
		pResult = ctx.m_pResult + ctx.m_Count;

		for( j = 0; j < (UINT32) pColSet->GetColliderCount(); j++)
		{
			Count += _Calc( pUnit, pColSet->GetCollider( j), &ctx);
		}

		pColSet->SetResult( pResult);
		pColSet->SetResultCount( Count);

		if((Count > 0) && ( pGroup->m_Style & I3_COLLISION_GRP_STYLE_NOTIFY))
		{
			i3EventReceiver * pReceiver = pColSet->GetEventReceiver();

			if( pReceiver != nullptr)
				pReceiver->OnEvent( I3_EVT_HIT, pColSet, 0, I3_EVT_CODE_ACTIVATE);
		}

		pResult += Count;
	}
}
