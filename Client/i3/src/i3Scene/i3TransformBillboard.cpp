#include "i3SceneDef.h"
#include "i3TransformBillboard.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"

I3_CLASS_INSTANCE( i3TransformBillboard);

I3_TRACE_RESULT I3SG_NODE_PROC( i3TransformBillboard)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3TransformBillboard * pTrans = (i3TransformBillboard *) pNode;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();

	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	if( ((pNode->IsFlag( I3_NODEFLAG_MANUAL_UPDATE) == false))
		|| (pTrans->GetUpdateFrameID() == 0xFFFFFFFF))
	{
		// 이미 한번 Update가 된 상황이면 Animation 등을 계산하지 않고
		// 예전 Update된 Cache Matrix를 그냥 활용한다.
		pTrans->SetUpdateFrameID( pTracer->getFrameID());

		pTrans->GetBillboardMatrix( pTrans->GetMatrix(), pStack->GetTop(), pTracer->GetViewMatrix());

		pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
	}
	else
	{
		// I3ASSERT( i3Matrix::isValid( pTrans->GetName(), pTrans->GetCacheMatrix()));
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		if( pTrans->IsFlag( I3_NODEFLAG_MANUAL_UPDATE))
		{
			I3ASSERT( pTrans->isValidMatrix(pTrans->GetCacheMatrix()));
		}
		else if( pTracer->getFrameID() == pTrans->GetUpdateFrameID())
		{
			I3ASSERT( pTrans->isValidMatrix(pTrans->GetCacheMatrix()));
		}
		else
		{
			I3ASSERT( pTrans->isValidMatrix(pTrans->GetCacheMatrix()));
		}
#endif

		pStack->Push( pTrans->GetCacheMatrix(), nullptr);
	}
	
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

i3TransformBillboard::i3TransformBillboard(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3TransformBillboard));
}

void i3TransformBillboard::SetAhchorAxis( VEC3D *pvAxis)
{
	if( pvAxis == nullptr)
		pvAxis = &I3_YAXIS;

	i3Vector::Normalize( &m_vAnchorAxis, pvAxis);
}

void i3TransformBillboard::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Transform::CopyTo( pDest, method);

	i3TransformBillboard * pTransBill = (i3TransformBillboard*)pDest;

	pTransBill->setAnchorEnable( m_bAnchorBillboard);
	pTransBill->SetAhchorAxis( &m_vAnchorAxis);
}

void i3TransformBillboard::GetBillboardMatrix( MATRIX * pOut, MATRIX * pIn, MATRIX * pView)
{
	MATRIX invCam;

	i3Matrix::Identity( pOut);
	if( m_bAnchorBillboard == false)
	{
		i3Matrix::Mul( &invCam, pIn, pView);
		i3Matrix::Transpose3x3( pOut, &invCam);
		i3Matrix::SetPos( pOut, i3Matrix::GetPos( pIn));
	}
	else
	{
		REAL32 dot;
		VEC3D *pRight	= i3Matrix::GetRight( pOut); 
		VEC3D *pUp		= i3Matrix::GetUp( pOut);
		VEC3D *pAt		= i3Matrix::GetAt( pOut);

		i3Matrix::Mul( &invCam, pIn, pView);
		i3Matrix::Transpose3x3( &invCam, &invCam);
		i3Vector::Normalize( pAt, i3Matrix::GetAt( &invCam));

		i3Vector::Normalize( pUp, &m_vAnchorAxis);
		
		dot = i3Vector::Dot( pUp, pAt);
		if( i3Math::abs(dot) >= (1.0f - 0.00001f))
		{
			// 같은 방향의 Vector
			static REAL32 ALIGN16 s_Axis[3][4] =  
			{
				{	1.0f, 0.0f, 0.0f, 0.0f},
				{	0.0f, 1.0f, 0.0f, 0.0f},  
				{	0.0f, 0.0f, 1.0f, 0.0f}
			} ;
			INT32 i = 0;

			for( i = 0; (i < 3) && ( i3Math::abs( dot) >= (1.0f - 0.00001f)); i++)
			{
				dot = i3Vector::Dot( pUp, (VEC3D *) s_Axis[i]);

				if( i3Math::abs( dot) < (1.0f - 0.00001f))
				{
					i3Vector::Cross( pRight, pUp, (VEC3D *) s_Axis[i]);
					i3Vector::Normalize( pRight, pRight);
					break;
				}
			}
		}
		else
		{
			i3Vector::Cross( pRight, pUp, pAt);
			i3Vector::Normalize( pRight, pRight);
		}
		
		i3Vector::Cross( pAt, pUp, pRight);
		i3Vector::Normalize( pAt, pAt);

		i3Matrix::SetPos( pOut, i3Matrix::GetPos( pIn));
	}
}

void	i3TransformBillboard::PreTrace(i3SceneTracer * pTracer)
{
	bool bNeedToUpdate = (IsFlag(I3_NODEFLAG_MANUAL_UPDATE) == false) || (this->GetUpdateFrameID() == 0xFFFFFFFF);
	// && pTracer->getFrameID() != this->GetUpdateFrameID()
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

	if (bNeedToUpdate)			// 아마도 모든게 이미 기처리되었을 것으로 보이므로 그냥 통으로 스킵..
	{
		GetBillboardMatrix(GetMatrix(), pStack->GetTop(), pTracer->GetViewMatrix());

		SetUpdateFrameID(pTracer->getFrameID());

		pStack->PushPre(GetCacheMatrix(), GetMatrix());
	}
	else
	{
		pStack->Push(GetCacheMatrix(), nullptr);
	}

	if (GetChildCount() > 0)
	{
		i3Node::PreTrace(pTracer);
	}

	pStack->Pop();
}

void i3TransformBillboard::OnUpdate( i3SceneTracer * pTracer)
{
	i3MatrixStack * pStack  = pTracer->GetModelViewMatrixStack();

	GetBillboardMatrix( GetMatrix(), pStack->GetTop(), pTracer->GetViewMatrix());

	SetUpdateFrameID( pTracer->getFrameID());

	pStack->PushPre( GetCacheMatrix(), GetMatrix());
	
	if( GetChildCount() > 0)
	{
		i3Node::OnUpdate( pTracer);
	}

	pStack->Pop();
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PACKED TRANSFORM_BILLBOARD
	{
		UINT8				m_ID[4] = { 'T', 'B', 'B', '1' };
		UINT8				m_bAnchorBillboard = 0;
		UINT8				pad1[3] = { 0 };
		i3::pack::VEC3D		m_vAnchorAxis;

		UINT32				m_Reserved[2] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3TransformBillboard::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Result = i3Transform::OnSave( pResFile);
	I3_CHKIO( Result);

	pack::TRANSFORM_BILLBOARD data;

	data.m_bAnchorBillboard = (m_bAnchorBillboard) ? 1 : 0;

	data.m_vAnchorAxis.x = getX( &m_vAnchorAxis);
	data.m_vAnchorAxis.y = getY( &m_vAnchorAxis);
	data.m_vAnchorAxis.z = getZ( &m_vAnchorAxis);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3TransformBillboard::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Result = i3Transform::OnLoad( pResFile);
	I3_CHKIO( Result);

	pack::TRANSFORM_BILLBOARD data;
	
	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	setAnchorEnable( (data.m_bAnchorBillboard == 0)? false : true);

	VEC3D vTemp;
	i3Vector::Set( &vTemp, data.m_vAnchorAxis.x, data.m_vAnchorAxis.y, data.m_vAnchorAxis.z);

	SetAhchorAxis( &vTemp);

	return Result;
}