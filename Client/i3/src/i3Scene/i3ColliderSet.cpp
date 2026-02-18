#include "i3SceneDef.h"
#include "i3ColliderSet.h"
#include "i3CollideeMesh.h"
#include "i3CollideeCylinder.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/container_util.h"

I3_CLASS_INSTANCE( i3ColliderSet);

I3_TRACE_RESULT I3SG_NODE_PROC( i3ColliderSet)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	MATRIX tmp;

#if NODE_PROFILE
	START_PROFILE( 29);
#endif

	if( pTracer->GetCollisionContext() != nullptr)
	{
		i3CollisionContext * pCollContext = pTracer->GetCollisionContext();

		if( pTracer->IsStyle( I3SG_TRACER_STYLE_NO_VIEWMATRIX))
		{
			_copyTopWorld( pTracer, &tmp, pTracer->GetModelViewMatrixStack());

			#if defined( I3_FIXED_POINT)
				MATRIX tmp2;

				i3Matrix::Copy( &tmp2, &tmp);
				pCollContext->Add( &tmp2, (i3ColliderSet *) pNode);
			#else
				pCollContext->Add( &tmp, (i3ColliderSet *) pNode);
			#endif
		}
		else
		{
			#if defined( I3_FIXED_POINT)
				MATRIX tmp2;

				i3Matrix::Copy( &tmp2, pTracer->GetModelViewMatrixStack()->GetTop());
				pCollContext->Add( &tmp2, (i3ColliderSet *) pNode);
			#else
				pCollContext->Add( pTracer->GetModelViewMatrixStack()->GetTop(), (i3ColliderSet *) pNode);
			#endif
		}
	}

#if NODE_PROFILE
	END_PROFILE( 29);
#endif

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

i3ColliderSet::i3ColliderSet(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3ColliderSet));
}

i3ColliderSet::~i3ColliderSet(void)
{
	RemoveAllColliders();
}

void i3ColliderSet::AddCollider( i3Collider * pAttr)
{
	I3_SAFE_ADDREF( pAttr);
	m_ColList.push_back( pAttr);
}

void i3ColliderSet::RemoveCollider( i3Collider * pAttr)
{
	size_t idx = i3::vu::index_of(m_ColList, pAttr);
	I3ASSERT( idx != m_ColList.size());

	m_ColList.erase(m_ColList.begin() + idx);

	I3_MUST_RELEASE( pAttr);
}

void i3ColliderSet::RemoveAllColliders(void)
{
	//m_ColList.SafeReleaseAll<i3Collider>();
	//m_ColList.SafeReleaseAll();
	i3::cu::for_each_container(m_ColList, i3::fu::mem_fun_safe(&i3Collider::Release));
	m_ColList.clear();
}

void i3ColliderSet::SetEventReceiver( i3EventReceiver * pObj)
{
	m_pReceiver = pObj;
}

void i3ColliderSet::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{

	i3Collider * pCol;
	VEC3D vmin, vmax;
	i3MatrixStack * pStack = pInfo->m_pStack;
	i3BoundBox * pBox = pInfo->m_pBound;

	for( size_t i = 0; i < m_ColList.size(); i++)
	{
		pCol = m_ColList[i];

		switch( pCol->GetType())
		{
			case I3_COLLIDER_NONE :
				break;

			case I3_COLLIDER_LINE :
				{
					i3CollideeLine * pLine = (i3CollideeLine *) pCol->GetCollidee();

					i3Vector::Copy( &vmin, pLine->GetStart());
					i3Vector::Copy( &vmax, pLine->GetEnd());

					i3Vector::TransformCoord( &vmin, &vmin, pStack->GetTop());
					i3Vector::TransformCoord( &vmax, &vmax, pStack->GetTop());

					if( pBox->IsEmpty())
					{
						pBox->SetMinMax( &vmin, &vmax);
					}
					else
					{
						pBox->ExtendByVec( &vmin);
						pBox->ExtendByVec( &vmax);
					}
				}
				break;

			case I3_COLLIDER_SPHERE :
				{
					i3CollideeSphere * pSphere = (i3CollideeSphere *) pCol->GetCollidee();

					i3Vector::Set( &vmin, -pSphere->GetRadius(), -pSphere->GetRadius(), -pSphere->GetRadius());
					i3Vector::Set( &vmax, pSphere->GetRadius(), pSphere->GetRadius(), pSphere->GetRadius());

					#if defined( I3_FIXED_POINT)
						VEC3D center;

						i3Vector::Copy( &center, pSphere->GetCenter());

						i3Vector::Add( &vmin, &vmin, &center);
						i3Vector::Add( &vmax, &vmax, &center);
					#else
						i3Vector::Add( &vmin, &vmin, pSphere->GetCenter());
						i3Vector::Add( &vmax, &vmax, pSphere->GetCenter());
					#endif

					i3Vector::TransformCoord( &vmin, &vmin, pStack->GetTop());
					i3Vector::TransformCoord( &vmax, &vmax, pStack->GetTop());

					if( pBox->IsEmpty())
					{
						pBox->SetMinMax( &vmin, &vmax);
					}
					else
					{
						pBox->ExtendByVec( &vmin);
						pBox->ExtendByVec( &vmax);
					}
				}
				break;

			case I3_COLLIDER_CYLINDER :
				{
					//i3CollideeCylinder * pLine = (i3CollideeCylinder *) pCol->GetCollidee();
				}
				break;

			case I3_COLLIDER_MESH :
				{
					i3CollideeMesh * pMesh = (i3CollideeMesh *) pCol->GetCollidee();
					UINT32 j;

					for( j = 0; j < pMesh->GetTriangleCount(); j++)
					{
						I3_COLLIDEE_TRI32 * pTri = pMesh->GetTriangle( j);

						#if defined( I3_FIXED_POINT)
							VEC3D pos[3];

							i3Vector::Copy( &pos[0], pMesh->GetPosition( pTri->m_Index[0]));
							i3Vector::Copy( &pos[1], pMesh->GetPosition( pTri->m_Index[1]));
							i3Vector::Copy( &pos[2], pMesh->GetPosition( pTri->m_Index[2]));

							if( pBox->IsEmpty())
								pBox->SetMinMax( &pos[0], &pos[0]);
							else
								pBox->ExtendByVec( &pos[0]);

							pBox->ExtendByVec( &pos[1]);
							pBox->ExtendByVec( &pos[2]);
						#else
							if( pBox->IsEmpty())
								pBox->SetMinMax( pMesh->GetPosition( pTri->m_Index[0]), pMesh->GetPosition( pTri->m_Index[0]));
							else
								pBox->ExtendByVec( pMesh->GetPosition( pTri->m_Index[0]));

							pBox->ExtendByVec( pMesh->GetPosition( pTri->m_Index[1]));
							pBox->ExtendByVec( pMesh->GetPosition( pTri->m_Index[2]));
						#endif
					}
				}
				break;
		}
	}
}

void i3ColliderSet::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	i3ColliderSet * pNode = (i3ColliderSet *) pDest;
	INT32 i;

	for( i = 0; i < GetColliderCount(); i++)
	{
		if( method == I3_COPY_INSTANCE)
		{
			i3Collider * pDestColl = i3Collider::new_object_ref();
			i3Collider * pSrcColl = GetCollider( i);

			pSrcColl->CopyTo( pDestColl, method);

			pNode->AddCollider( pDestColl);
		}
		else
		{
			pNode->AddCollider( GetCollider( i));
		}
	}

	pNode->SetGroup( GetGroup());
	pNode->SetStyle( GetStyle());
	pNode->SetDistance( GetDistance());
	pNode->SetVelocity( GetVelocity());
	pNode->SetEventReceiver( GetEventReceiver());
}

void i3ColliderSet::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	
	i3Collider * pAttr;

	for(size_t i = 0; i < m_ColList.size(); i++)
	{
		pAttr = GetCollider( i);

		pAttr->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 COLLIDER_SET
	{
		INT8		m_ID[4] = { 'C', 'L', 'S', '1' };
		UINT32		m_ColliderCount = 0;
		UINT32		m_GroupIndex = 0;
		UINT32		m_Style = 0;
		REAL32		m_DistSq = 0.0f;
		OBJREF		m_pReceiver = 0;
		INT8		m_bPhysics = 0;
		INT8		m_Reserved[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ColliderSet::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	INT32 i;
	OBJREF ref;
	pack::COLLIDER_SET Data;

	// Node
	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	
	Data.m_ColliderCount = GetColliderCount();
	Data.m_DistSq =			m_DistSq;
	Data.m_GroupIndex =		m_GroupIndex;
	Data.m_Style =			m_Style;
	
	Rc = pStream->Write( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ColliderSet::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Attr Object Reference
	for( i = 0; i < GetColliderCount(); i++)
	{
		ref = (OBJREF) pResFile->GetObjectPersistID( GetCollider( i));

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ColliderSet::OnSave()", "Could not write collider object reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3ColliderSet::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3ColliderSet::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i;
	i3Collider * pCol;
	OBJREF ref;
	pack::COLLIDER_SET Data;

	// node
	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ColliderSet::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_GroupIndex	= Data.m_GroupIndex;
	m_DistSq		= Data.m_DistSq;
	m_Style			= Data.m_Style;

	// Collidee Reference
	for( i = 0; i < Data.m_ColliderCount; i++)
	{
		Rc = pStream->Read( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ColliderSet::OnLoad()", "Couild not read collidee object reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pCol = (i3Collider *) pResFile->FindObjectByID( ref);
		I3ASSERT( pCol != nullptr);

		AddCollider( pCol);
	}

	return Result;
}


