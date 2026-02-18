#include "i3SceneDef.h"
#include "i3Transform2.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"
#include "i3ShapeSetContainer.h"

#if defined (I3_PHYSX)
	#include "i3PhysixContext.h"

	#include "NXU_helper.h"
	#include "NXU_schema.h"
	#include "NXU_Streaming.h"
	#include "NXU_cooking.h"
#endif

I3_CLASS_INSTANCE( i3Transform2);

I3_TRACE_RESULT I3SG_NODE_PROC( i3Transform2)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3Transform2 * pTrans = (i3Transform2 *) pNode;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();
	bool bNeedUpdate = true;
	i3PhysixShapeSet * pShapeSet;

	if( pTrans->isDynamic())
	{
		// Physix -> Animated Transform
		if( pTrans->GetUpdateFrameID() != pTracer->getFrameID())
		{
			pShapeSet = pTrans->getShapeSet();

			if( (pShapeSet != nullptr) && (pShapeSet->isDynamicState( I3_PHYSIX_STATE_NO_COLLISION) == false))
			{
				pTrans->getShapeSet()->getShapeGlobalPose( pTrans->GetCacheMatrix());
				bNeedUpdate = false;
			}
		}
		else
		{
			bNeedUpdate = false;
		}
	}

	bNeedUpdate = bNeedUpdate && (pNode->IsFlag( I3_NODEFLAG_MANUAL_UPDATE) == false);
	bNeedUpdate = bNeedUpdate && ( pTrans->GetUpdateFrameID() != pTracer->getFrameID());

	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	bNeedUpdate = bNeedUpdate || (pTrans->GetUpdateFrameID() == 0xFFFFFFFF);

	if( bNeedUpdate)
	{
		if( pTrans->IsAnimated())
		{
			I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

			pEnv->m_Time = pTracer->GetTime();
			pEnv->m_pIn = pTrans->GetMatrix();
			pEnv->m_pOut = pTrans->GetMatrix();
			pEnv->m_pModelViewMatrix = pStack->GetTop();

			pTrans->GetAnimatedMatrix( pEnv);
		}

		pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
	}
	else
	{
		// I3ASSERT( i3Matrix::isValid( pTrans->GetName(), pTrans->GetCacheMatrix()));
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT( pTrans->isValidMatrix(pTrans->GetCacheMatrix()));
#endif
		pStack->Push( pTrans->GetCacheMatrix(), nullptr);
	}

	if( pTrans->isDynamic() == false)
	{
		pShapeSet = pTrans->getShapeSet();

		if( (pShapeSet != nullptr) && (pShapeSet->isDynamicState( I3_PHYSIX_STATE_NO_COLLISION) == false))
		{
			// Kinematics 모드인 경우에는 Physix의 Shape에 대해 World Transform을
			// 갱신해 주어야 한다.
			// Animated Tranform -> Physix
			pTrans->getShapeSet()->setShapeGlobalPose( pTrans->GetCacheMatrix());
			// pTrans->getShapeSet()->getShapeGlobalPose( pTrans->GetCacheMatrix());
		}
	}

	pTrans->SetUpdateFrameID( pTracer->getFrameID());
	
	{
		// 자신의 Model View Matrix를 tracer에 등록해 둔다.
		register i3ModelViewMatrixAttr * pAttr = pTracer->AllocModelViewMatrixAttr();

		_copyTop( pAttr->GetMatrix(), pStack);

		pStackManager->PushAttr( pAttr);
	}

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	pStackManager->PopAttr( I3_ATTRID_MODEL_VIEW_MATRIX);

	pStack->Pop();

	return Result;
}

void	i3Transform2::PreTrace(i3SceneTracer * pTracer)
{
	bool bNeedUpdate = true;

	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

	if (isDynamic())
	{
		// Physix -> Animated Transform
		if (GetUpdateFrameID() != pTracer->getFrameID())
		{
			if ((getShapeSet() != nullptr) && (getShapeSet()->isDynamicState(I3_PHYSIX_STATE_NO_COLLISION) == false))
			{
				getShapeSet()->getShapeGlobalPose(GetCacheMatrix());

				bNeedUpdate = false;
			}
		}
		else
		{
			bNeedUpdate = false;
		}
	}

	bNeedUpdate = bNeedUpdate && (IsFlag(I3_NODEFLAG_MANUAL_UPDATE) == false);
	bNeedUpdate = bNeedUpdate && (GetUpdateFrameID() != pTracer->getFrameID());

	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	bNeedUpdate = bNeedUpdate || (GetUpdateFrameID() == 0xFFFFFFFF);

	if (bNeedUpdate)
	{
		if (IsAnimated())
		{
			I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

			pEnv->m_Time = pTracer->GetTime();
			pEnv->m_pIn = GetMatrix();
			pEnv->m_pOut = GetMatrix();
			pEnv->m_pModelViewMatrix = pStack->GetTop();

			GetAnimatedMatrix(pEnv);
		}

		pStack->PushPre(GetCacheMatrix(), GetMatrix());
	}
	else
	{
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT(isValidMatrix(GetCacheMatrix()));
#endif

		pStack->Push(GetCacheMatrix(), nullptr);
	}

	if (isDynamic() == false)
	{
		if ((getShapeSet() != nullptr) && (getShapeSet()->isDynamicState(I3_PHYSIX_STATE_NO_COLLISION) == false))
		{
			// Kinematics 모드인 경우에는 Physix의 Shape에 대해 World Transform을
			// 갱신해 주어야 한다.
			// Animated Tranform -> Physix
			getShapeSet()->setShapeGlobalPose(GetCacheMatrix());
		}
	}

	SetUpdateFrameID(pTracer->getFrameID());

	if (GetChildCount() > 0)
	{
		i3Node::PreTrace(pTracer);
	}

	pStack->Pop();
}

void i3Transform2::OnUpdate( i3SceneTracer * pTracer)
{
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	bool bNeedUpdate = true;

	if( isDynamic())
	{
		// Physix -> Animated Transform
		if( GetUpdateFrameID() != pTracer->getFrameID())
		{
			if( (getShapeSet() != nullptr) && (getShapeSet()->isDynamicState( I3_PHYSIX_STATE_NO_COLLISION) == false))
			{
				getShapeSet()->getShapeGlobalPose( GetCacheMatrix());

				bNeedUpdate = false;
			}
		}
		else
		{
			bNeedUpdate = false;
		}
	}

	bNeedUpdate = bNeedUpdate && ( IsFlag( I3_NODEFLAG_MANUAL_UPDATE) == false);
	bNeedUpdate = bNeedUpdate && ( GetUpdateFrameID() != pTracer->getFrameID());

	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	bNeedUpdate = bNeedUpdate || ( GetUpdateFrameID() == 0xFFFFFFFF);

	if( bNeedUpdate)
	{
		if( IsAnimated())
		{
			I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

			pEnv->m_Time = pTracer->GetTime();
			pEnv->m_pIn = GetMatrix();
			pEnv->m_pOut = GetMatrix();
			pEnv->m_pModelViewMatrix = pStack->GetTop();

			GetAnimatedMatrix( pEnv);
		}

		pStack->PushPre( GetCacheMatrix(), GetMatrix());
	}
	else
	{
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT( isValidMatrix(GetCacheMatrix()));
#endif

		pStack->Push( GetCacheMatrix(), nullptr);
	}

	if( isDynamic() == false)
	{
		if( (getShapeSet() != nullptr) && (getShapeSet()->isDynamicState( I3_PHYSIX_STATE_NO_COLLISION) == false))
		{
			// Kinematics 모드인 경우에는 Physix의 Shape에 대해 World Transform을
			// 갱신해 주어야 한다.
			// Animated Tranform -> Physix
			getShapeSet()->setShapeGlobalPose( GetCacheMatrix());
		}
	}

	SetUpdateFrameID( pTracer->getFrameID());

	if( GetChildCount() > 0)
	{
		i3Node::OnUpdate( pTracer);
	}

	pStack->Pop();
}

i3Transform2::i3Transform2(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3Transform2));
}

i3Transform2::~i3Transform2(void)
{
	I3_SAFE_RELEASE( m_pShapeSet);
}

void i3Transform2::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	i3Transform::_GetWrappingBound( pInfo);

	
	if( m_pShapeSet != nullptr)
	{
		m_pShapeSet->GetWrappingBound(pInfo);
	}
	
}

void i3Transform2::OnChangeTime( REAL32 tm)
{
	i3Transform::OnChangeTime( tm);

	if( tm == 0.0f && isDynamic() )
	{
		m_pShapeSet->Reset();
	}
}

void i3Transform2::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Transform::CopyTo( pDest, method);

	i3Transform2 * pNode = (i3Transform2 *) pDest;

	if( method == I3_COPY_REF)
	{
		pNode->setShapeSet( getShapeSet());
	}
	else
	{
		if( getShapeSet() != nullptr)
		{
			i3PhysixShapeSet * pNewShapeSet = i3PhysixShapeSet::new_object_ref();

			getShapeSet()->CopyTo( pNewShapeSet, I3_COPY_INSTANCE);

			pNode->setShapeSet( pNewShapeSet);
		}
	}

	// Joint에 대한 것은 이 단계를 지나, OnInitAfterLoad 단계에서
	// 처리한다.
}

void i3Transform2::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pShapeSet != nullptr)
	{
		m_pShapeSet->OnBuildObjectList( List);
	}

	i3Transform::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 TRANSFORM2
	{
		INT8		m_ID[4] = { 'T', 'X', 'M', '2' };
		OBJREF		m_pShapeSet = 0;
		INT32		m_Reserved[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Transform2::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::TRANSFORM2 Data;

	// Node
	Result = i3Transform::OnSave( pResFile);
	I3_CHKIO( Result);

	// Header
	Data.m_pShapeSet	= pResFile->GetObjectPersistID( getShapeSet());

	Rc = pResFile->Write( &Data, sizeof(Data));
	I3_CHKIO( Rc);

	return Result;
}

UINT32 i3Transform2::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::TRANSFORM2 Data;

	// node
	Result = i3Transform::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &Data, sizeof(Data));
	I3_CHKIO( Rc);

	if( Data.m_pShapeSet != 0)
	{
		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pResFile->FindObjectByID( Data.m_pShapeSet);
		I3ASSERT( pShapeSet != nullptr);

		setShapeSet( pShapeSet);
	}
	else
	{
		I3TRACE( "Empty ShapeSet : %s\n", GetName());
	}

	i3Matrix::Copy( &m_CacheMatrix, GetMatrix());

	return Result;
}

void	i3Transform2::OnInitAfterLoad(void)
{
	i3Transform::OnInitAfterLoad();

	if( getShapeSet() == nullptr)
		return;
}

void i3Transform2::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	I3ASSERT( pInfo != nullptr);
	I3ASSERT( pInfo->_pMatrixStack != nullptr);
	
	MATRIX mtx;
	pInfo->_pMatrixStack->PushPre( &mtx, GetMatrix());

	if( getShapeSet() != nullptr)
	{
		getShapeSet()->OnInstancing( pInfo );

		if( getShapeSet()->getJointInfo() != nullptr)
		{
			// Joint가 존재한다.
			// Heirarchy Structure를 알고 있는 i3Transform2 class가 이에 대한 책임을 진다.
			i3Node * pParent = GetParent();
			
			if(	!i3::same_of<i3Transform2*>(pParent) )
			{
				// World에 Attach된 Joint
				getShapeSet()->Create( nullptr);
			}
			else
			{
				i3Transform2 * pParentTrans = (i3Transform2 *) pParent;

				getShapeSet()->CreateJoint( pParentTrans->getShapeSet());
			}
		}

		//getShapeSet()->setMassLocalPose( i3Matrix::GetPos( GetMatrix() ) );
	}

	i3Node::OnInstancing( pInfo);

	pInfo->_pMatrixStack->Pop();
}


