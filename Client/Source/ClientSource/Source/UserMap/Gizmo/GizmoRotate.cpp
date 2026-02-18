#include "pch.h"
#include "GizmoRotate.h"

#define ROUND_SIZE	0.7f
#define ARC_ALPHA	0.4f

I3_CLASS_INSTANCE( CGizmoRotate, CGizmo);

CGizmoRotate::CGizmoRotate(void)
{
}

CGizmoRotate::~CGizmoRotate(void)
{
}

BOOL CGizmoRotate::Create()
{
	//	Move Axis
	COLORREAL col;
	i3Node * pNode;
	i3Node * pRotateArc, * pRotateArcLine;
	i3BoundBox * pBoundBox;
	i3Transform * pTrans;
	i3AttrSet * pAttrSet = i3AttrSet::NewObjectRef();

	pAttrSet->SetPriority( 0);
	getSceneObject()->AddChild( pAttrSet);

	m_pRotateRoot = i3Transform::NewObject();
	m_pRotateRoot->SetName( "Rotate" );
	
	pRotateArc = i3Node::NewObjectRef();
	pRotateArc->SetName( "RotateArc" );
	m_pRotateRoot->AddChild( pRotateArc );

	pRotateArcLine = i3Node::NewObjectRef();
	pRotateArcLine->SetName( "RotateArcLine" );
	m_pRotateRoot->AddChild( pRotateArcLine );

	m_pRotateLine = i3Node::NewObjectRef();
	m_pRotateLine->SetName( "RotateLine" );
	m_pRotateRoot->AddChild( m_pRotateLine );

	pAttrSet->AddChild( m_pRotateRoot );

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();
		pAttr->Set( FALSE);
		pAttrSet->AddAttr( pAttr);
	}

	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::NewObjectRef();
		pAttr->Set( TRUE);
		pAttrSet->AddAttr( pAttr);
	}

	{
		i3Color::Set( &col, 0.1f, 0.1f, 0.7f, ARC_ALPHA );
	
		pTrans = i3Transform::NewObjectRef(); 
		pTrans->SetName( "Trans" );
		i3Matrix::SetScale( pTrans->GetMatrix(), 1.41f, 1.41f, 1.41f );
		{
			i3Node * pSphere = i3SceneUtil::BuildSphereMesh( TRUE, FALSE, 20, 10, NULL, &col, &col );
			pSphere->SetName( "Sphere" );
			SetZFunc( pSphere, I3G_COMP_ALWAYS );
			SetAlphaFunc( pSphere, I3G_COMP_GREATER );
			SetBlendMode( pSphere, I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
			pTrans->AddChild( pSphere );
		}
		pAttrSet->AddChild( pTrans);
	}

	// X축 Rotate
	{
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );
		{
			m_pTrans[0] = i3Transform::NewObjectRef();
			m_pTrans[0]->SetName( "TransArcX" );
			i3Matrix::SetRotateZDeg( m_pTrans[0]->GetMatrix(), 90.0f);
			i3Matrix::PreRotateYDeg( m_pTrans[0]->GetMatrix(), 90.f );
			{
				pBoundBox = i3BoundBox::NewObjectRef();

				m_pLine[0] = i3SceneUtil::BuildArc( ROUND_SIZE, 360.f, 30, TRUE, FALSE, NULL, &col, &col );
				m_pLine[0]->SetName( "RotateX" );
				SetZFunc( m_pLine[0], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pLine[0], I3G_COMP_GREATER );
				SetBlendMode( m_pLine[0], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pLine[0]->GetWrappingBound( pBoundBox );
				m_pLine[0]->SetBound( pBoundBox );
				m_pTrans[0]->AddChild( m_pLine[0] );
				m_pLine[0]->Release();
			}
			pRotateArc->AddChild( m_pTrans[0]);
		}
	
		{	// ArcLine
			i3Color::Set( &col, 1.f, 0.f, 0.f, ARC_ALPHA );
			{
				pNode = i3SceneUtil::BuildArc( ROUND_SIZE, 360.f, 30, FALSE, TRUE, &col, NULL, NULL );
				pNode->SetName( "ArcLineX" );
				SetZFunc( pNode, I3G_COMP_ALWAYS );
				m_pTrans[0]->AddChild( pNode );
				pNode->Release();
			}
		}

		{	// x Axis line
			i3Color::Set( &col, 1.f, 0.f, 0.f, 1.f );
			
			m_pAxisTrans[0] = i3Transform::NewObjectRef();
			m_pAxisTrans[0]->SetName( "LineX" );
			m_pRotateLine->AddChild( m_pAxisTrans[0] );

			i3Matrix::SetRotateXDeg( m_pAxisTrans[0]->GetMatrix(), 0.f );
			{
				VEC3D	Vec;
				i3VertexArray * pVA = NULL;
				pNode = i3SceneUtil::BuildLine( pVA, 1, &col );
				pVA->Lock();
				i3Vector::Set( &Vec, 0.f, 0.f, -ROUND_SIZE );
				pVA->SetPosition( 1, &Vec );
				pVA->Unlock();
				pNode->SetName( "LineX" );
				SetZFunc( pNode, I3G_COMP_ALWAYS );
				m_pAxisTrans[0]->AddChild( pNode );
				pNode->Release();
			}
		}
	}
	
	// Y축 Rotate
	{
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );
		{
			m_pTrans[1] = i3Transform::NewObjectRef();
			m_pTrans[1]->SetName( "TransArcY");
			pRotateArc->AddChild( m_pTrans[1] );
			i3Matrix::SetRotateYDeg( m_pTrans[1]->GetMatrix(), 90.0f);

			{
				pBoundBox = i3BoundBox::NewObjectRef();
				m_pLine[1] = i3SceneUtil::BuildArc( ROUND_SIZE, 360.f, 30, TRUE, FALSE, NULL, &col, &col );
				m_pLine[1]->SetName( "RotateY" );
				SetZFunc( m_pLine[1], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pLine[1], I3G_COMP_GREATER );
				SetBlendMode( m_pLine[1], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pLine[1]->GetWrappingBound( pBoundBox );
				m_pLine[1]->SetBound( pBoundBox );
				m_pTrans[1]->AddChild( m_pLine[1] );
				m_pLine[1]->Release();
			}
		}

		{	// ArcLine
			i3Color::Set( &col, 0.f, 1.f, 0.f, ARC_ALPHA );

			pNode = i3SceneUtil::BuildArc( ROUND_SIZE, 360.f, 30, FALSE, TRUE, &col, NULL, NULL );
			pNode->SetName( "ArcLineY" );
			SetZFunc( pNode, I3G_COMP_ALWAYS );
			m_pTrans[1]->AddChild( pNode );
			pNode->Release();
		}

		{	// y Axis line
			i3Color::Set( &col, 0.f, 1.f, 0.f, 1.f );

			m_pAxisTrans[1] = i3Transform::NewObjectRef();
			m_pAxisTrans[1]->SetName( "LineY" );
			i3Matrix::SetRotateYDeg( m_pAxisTrans[1]->GetMatrix(), 0.f );
			{
				VEC3D	Vec;
				i3VertexArray * pVA = NULL;
				pNode = i3SceneUtil::BuildLine( pVA, 1, &col );
				pVA->Lock();
				i3Vector::Set( &Vec, ROUND_SIZE, 0.f, 0.f );
				pVA->SetPosition( 1, &Vec );
				pVA->Unlock();
				pNode->SetName( "LineY" );
				SetZFunc( pNode, I3G_COMP_ALWAYS );
				m_pAxisTrans[1]->AddChild( pNode );
				pNode->Release();
			}
			m_pRotateLine->AddChild( m_pAxisTrans[1] );
		}
	}
	
	// Z축 Rotate
	{
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );
		{
			m_pTrans[2] = i3Transform::NewObjectRef();
			m_pTrans[2]->SetName( "TransZ" );
			pRotateArc->AddChild( m_pTrans[2]);

			i3Matrix::SetRotateXDeg( m_pTrans[2]->GetMatrix(), 270.0f);

			{
				pBoundBox = i3BoundBox::NewObjectRef();
				m_pLine[2] = i3SceneUtil::BuildArc( ROUND_SIZE, 360.f, 30, TRUE, FALSE, NULL, &col, &col );
				m_pLine[2]->SetName( "RotateZ" );
				SetZFunc( m_pLine[2], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pLine[2], I3G_COMP_GREATER );
				SetBlendMode( m_pLine[2], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pLine[2]->GetWrappingBound( pBoundBox );
				m_pLine[2]->SetBound( pBoundBox );
				m_pTrans[2]->AddChild( m_pLine[2] );
				m_pLine[2]->Release();
			}
		}

		{	// ArcLine
			i3Color::Set( &col, 0.f, 0.f, 1.f, ARC_ALPHA );
			{
				pNode = i3SceneUtil::BuildArc( ROUND_SIZE, 360.f, 30, FALSE, TRUE, &col, NULL, NULL );
				pNode->SetName( "ArcLineZ" );
				SetZFunc( pNode, I3G_COMP_ALWAYS );
				m_pTrans[2]->AddChild( pNode );
				pNode->Release();
			}
		}

		{	// z Axis line
			i3Color::Set( &col, 0.f, 0.f, 1.f, 1.f );
			{
				m_pAxisTrans[2] = i3Transform::NewObjectRef();
				m_pAxisTrans[2]->SetName( "LineZ" );
				i3Matrix::SetRotateYDeg( m_pAxisTrans[2]->GetMatrix(), 0.f );
				{
					VEC3D	Vec;
					i3VertexArray * pVA = NULL;
					pNode = i3SceneUtil::BuildLine( pVA, 1, &col );
					pVA->Lock();
					i3Vector::Set( &Vec, 0.f, ROUND_SIZE, 0.f );
					pVA->SetPosition( 1, &Vec );
					pVA->Unlock();
					pNode->SetName( "LineZ" );
					SetZFunc( pNode, I3G_COMP_ALWAYS );
					m_pAxisTrans[2]->AddChild( pNode );
					pNode->Release();
				}
				m_pRotateLine->AddChild( m_pAxisTrans[2] );
			}
		}
	}

	{
		// 하위 AttrSet 들의 Priority를 조정한다.
		INT32 i;
		i3AttrSet * pAttrSet;
		i3List list;

		i3Scene::FindNodeByExactType( getSceneObject(), i3AttrSet::GetClassMeta(), &list);

		for( i = 0; i < list.GetCount(); i++)
		{
			pAttrSet = (i3AttrSet *) list.Items[i];

			pAttrSet->SetPriority( 0);
		}
	}

	// 충돌 처리를 위한 Vertex Array
	{
		REAL32 deg = 0.0f;
		REAL32 step = 360.0f / NUM_LINE_SEG;
		INT32 i;

		for( i = 0; i < NUM_LINE_SEG; i++)
		{
			REAL32 x, z;

			x = i3Math::sin( I3_DEG2RAD( deg));
			z = i3Math::cos( I3_DEG2RAD( deg));

			i3Vector::Set( &m_LineVec[i], x * ROUND_SIZE, 0.0f, z * ROUND_SIZE);

			deg += step;
		}
	}

	return TRUE;
}

BOOL CGizmoRotate::RotateLineX( REAL32 fDeg )
{
	i3Matrix::SetRotateXDeg( m_pAxisTrans[0]->GetMatrix(), fDeg );

	return TRUE;
}

BOOL CGizmoRotate::RotateLineY( REAL32 fDeg )
{
	i3Matrix::SetRotateYDeg( m_pAxisTrans[1]->GetMatrix(), fDeg );

	return TRUE;
}

BOOL CGizmoRotate::RotateLineZ( REAL32 fDeg )
{
	i3Matrix::SetRotateZDeg( m_pAxisTrans[2]->GetMatrix(), fDeg );

	return TRUE;
}
/*
void CGizmoRotate::RotateArc( I3_LEVEL_VIEWPORT_CTRL_CONTEXT * pCtx, VEC3D * pRotate )
{
	if( getX( &pCtx->m_AxisMask ) > I3_EPS )
	{
		i3Matrix::SetRotateX( m_pRotateRoot->GetMatrix(), getX( pRotate ) );
		i3Matrix::PostRotateY( m_pRotateRoot->GetMatrix(), getY( pRotate ) );
		i3Matrix::PostRotateZ( m_pRotateRoot->GetMatrix(), getZ( pRotate ) );
	}
	else if( getY( &pCtx->m_AxisMask ) > I3_EPS )
	{
		i3Matrix::SetRotateX( m_pRotateRoot->GetMatrix(), getX( pRotate ) );
		i3Matrix::PostRotateY( m_pRotateRoot->GetMatrix(), getY( pRotate ) );
		i3Matrix::PostRotateZ( m_pRotateRoot->GetMatrix(), getZ( pRotate ) );
	}
	else if( getZ( &pCtx->m_AxisMask ) > I3_EPS )
	{
		i3Matrix::SetRotateX( m_pRotateRoot->GetMatrix(), getX( pRotate ) );
		i3Matrix::PostRotateY( m_pRotateRoot->GetMatrix(), getY( pRotate ) );
		i3Matrix::PostRotateZ( m_pRotateRoot->GetMatrix(), getZ( pRotate ) );
	}
	else
	{
		VEC3D	Vec;
		i3Vector::Set( &Vec, 1.f, 0.f, 0.f );
		i3Matrix::SetRotateAxis( m_pRotateRoot->GetMatrix(), &Vec, getX( pRotate ) );
		i3Vector::Set( &Vec, 0.f, 1.f, 0.f );
		i3Matrix::PreRotateAxis( m_pRotateRoot->GetMatrix(), &Vec, getY( pRotate ) );
		i3Vector::Set( &Vec, 0.f, 0.f, 1.f );
		i3Matrix::PreRotateAxis( m_pRotateRoot->GetMatrix(), &Vec, getZ( pRotate ) );
	}
}
*/
void CGizmoRotate::RotateLine( VEC3D * pRotate )
{
	i3Matrix::SetRotateX( m_pAxisTrans[0]->GetMatrix(), getX( pRotate ) );
	i3Matrix::SetRotateY( m_pAxisTrans[1]->GetMatrix(), getY( pRotate ) );
	i3Matrix::SetRotateZ( m_pAxisTrans[2]->GetMatrix(), getZ( pRotate ) );
}

bool CGizmoRotate::_HitTestWithLine( VEC3D * pStart, VEC3D * pEnd)
{
	INT32 i;
	VEC3D dir1, dir2;

	i3Vector::Sub( &dir1, pEnd, pStart);
	i3Vector::Normalize( &dir1, &dir1);

	for( i = 0; i < NUM_LINE_SEG; i++)
	{
		REAL32 f1, f2;
		REAL32 len;
		
		if( i < (NUM_LINE_SEG - 1))
		{
			i3Vector::Sub( &dir2, &m_LineVec[i + 1], &m_LineVec[i]);
		}
		else
		{
			i3Vector::Sub( &dir2, &m_LineVec[i], &m_LineVec[0]);
		}

		len = i3Math::GetSqrDistanceLineToLine( pStart, &dir1, & m_LineVec[i], &dir2, &f1, &f2);

		if( len <= 0.001f)
		{
			i3Vector::Scale( &m_PickedPos, &dir1, f1);
			i3Vector::Add( &m_PickedPos, &m_PickedPos, pStart);
			return true;
		}
	}

	return false;
}

bool CGizmoRotate::OnCollision( VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx)
{
	/*
	INT32 i;
	MATRIX	matParent, full, inv, matWS;
	VEC3D vStart, vEnd;
	COLORREAL col;
	bool bResult = false;
	static VEC3D	s_vecAxisMask[3] =
	{
		{	1.0f,	0.0f,	0.0f	},
		{	0.0f,	1.0f,	0.0f	},
		{	0.0f,	0.0f,	1.0f	}
	};

	i3Matrix::Mul( &matWS, m_pRotateRoot->GetMatrix(), getSceneObject()->GetMatrix());
	i3Matrix::Mul( &matParent, &matWS, pViewProjMtx);

	for( i = 0; i < 3; i++ )
	{			
		{
			i3Matrix::Mul( &full, m_pTrans[i]->GetMatrix(), &matParent );
			i3Matrix::Inverse( &inv, NULL, &full);

   			i3Vector::TransformCoord( &vStart, pVStart, &inv);
			i3Vector::TransformCoord( &vEnd, pVEnd, &inv);
		}

		if( bResult == false)
		{
			if( _HitTestWithLine( &vStart, &vEnd))
			{
				i3Color::Set( &col, 1.f, 1.f, 0.f, 0.5f );

				ChangeColor( m_pLine[i], &col );
				pFramework->SetAxisMask( & s_vecAxisMask[i]);
				bResult = true;

				// LocalAxis인 PickedPos를 World-Space로 변환.
				i3Matrix::Mul( &full, m_pTrans[i]->GetMatrix(), &matWS);
				i3Vector::TransformCoord( &m_PickedPos, &m_PickedPos, &full);
			}
			else
			{
				i3Color::Set( &col, 0.f, 0.f, 0.f, 0.0f );

				ChangeColor( m_pLine[i], &col );
			}
		}
		else
		{
			i3Color::Set( &col, 0.f, 0.f, 0.f, 0.0f );

			ChangeColor( m_pLine[i], &col );
		}
	}

	if( bResult)
		return true;
*/
	return false; //CGizmo::OnCollision( pViewport, pVStart, pVEnd, pViewProjMtx);
}
