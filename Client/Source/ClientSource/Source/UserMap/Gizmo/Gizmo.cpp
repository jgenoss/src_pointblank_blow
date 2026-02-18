#include "pch.h"
#include "Gizmo.h"

I3_CLASS_INSTANCE( CGizmo, i3GameObj);

CGizmo::CGizmo(void)
{
	m_pAxisRoot			= NULL;
	m_pTransformRoot	= NULL;

	i3mem::FillZero( m_pAxis, sizeof( i3Transform*) * 3);

	i3Vector::Zero( &m_vAxisMask);
}

CGizmo::~CGizmo(void)
{
	//axisroot이외의 멤버는 ref로 생성하여 직접 릴리즈할 필요 없음.
	I3_SAFE_NODE_RELEASE( m_pAxisRoot);
}

static COLORREAL	s_InitAxisInfo[3] =
{
	{	1.0f,	0.0f,	0.0f,	0.0f},
	{	0.0f,	1.0f,	0.0f,	0.0f},
	{	0.0f,	0.0f,	1.0f,	0.0f}
};

BOOL CGizmo::Create()
{
	i3Node * pNode;
	i3BoundBox * pBoundBox;
	INT32 i;

	m_pAxisRoot = i3AttrSet::NewObject();
	m_pAxisRoot->SetName( "Axis" );
	m_pAxisRoot->SetPriority( 0);

	getSceneObject()->AddChild( m_pAxisRoot );

	m_pTransformRoot = i3Transform::NewObjectRef();

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();

		pAttr->Set( FALSE);
		m_pAxisRoot->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::NewObjectRef();

		pAttr->Set( FALSE);
		m_pAxisRoot->AddAttr( pAttr);
	}

	for( i = 0; i < 3; i++)
	{
		m_pAxis[i] = i3Transform::NewObjectRef();
		m_pTransformRoot->AddChild( m_pAxis[i]);

		{
			pNode = i3SceneUtil::BuildAxisMesh( TRUE, TRUE, &s_InitAxisInfo[i], &s_InitAxisInfo[i]);

			pBoundBox = i3BoundBox::NewObjectRef();
			pBoundBox->Reset();
			pNode->GetWrappingBound( pBoundBox);
			pNode->SetBound( pBoundBox);

			m_pAxis[i]->AddChild( pNode);
			pNode->Release();
		}
	}

	i3Matrix::SetRotateZDeg( m_pAxis[1]->GetMatrix(), 90.0f);
	i3Matrix::SetRotateYDeg( m_pAxis[2]->GetMatrix(), 90.0f);

	return TRUE;
}

i3Node * CGizmo::BuildRectangle( BOOL bTri, BOOL bLine, REAL32 fWidth, REAL32 fHeight, COLORREAL * pLineColor, COLORREAL * pDiffuse, COLORREAL * pAmbient )
{
	i3Node * pRoot;
	i3AttrSet * pTriAttrSet = NULL;
	i3AttrSet * pLineAttrSet = NULL;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	i3GeometryAttr * pGeoAttr;
	i3Geometry *	pGeo;


	pRoot = i3Node::NewObject();

	if( bTri )
	{
		pTriAttrSet = i3AttrSet::NewObjectRef();
		// Cull mode off
		{
			i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::NewObjectRef();
			pAttr->Set( I3G_FACE_CULL_NONE );
			pTriAttrSet->AddAttr( pAttr );
		}

		if( pDiffuse != NULL)
		{
			i3ColorAttr * pAttr = i3ColorAttr::NewObjectRef();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);

			// alpha set
			if( i3Color::GetA( pDiffuse ) < 255 )
			{
				{
					i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::NewObjectRef();

					pAttr->Set( TRUE );
					pTriAttrSet->AddAttr( pAttr );
				}

				{
					i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::NewObjectRef();

					pAttr->SetFunction( I3G_COMP_GREATER );
					pTriAttrSet->AddAttr( pAttr );
				}

				{
					i3BlendEnableAttr * pAttr = i3BlendEnableAttr::NewObjectRef();

					pAttr->Set( TRUE );
					pTriAttrSet->AddAttr( pAttr );
				}

				{
					i3BlendModeAttr * pAttr = i3BlendModeAttr::NewObjectRef();

					pAttr->SetDestination( I3G_BLEND_INVSRCALPHA ); 
					pTriAttrSet->AddAttr( pAttr );
				}
			}
		}

		if( pAmbient != NULL && pDiffuse != NULL)
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::NewObjectRef();

			pAttr->SetDiffuse( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pTriAttrSet );
	}

	// Line
	if( bLine )
	{
		pLineAttrSet = i3AttrSet::NewObjectRef();

		if( pLineColor != NULL )
		{
			i3ColorAttr * pAttr = i3ColorAttr::NewObjectRef();

			pAttr->Set( pLineColor );
			pLineAttrSet->AddAttr( pAttr );

			if( i3Color::GetA( pLineColor ) < 255 )
			{
				{
					i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::NewObjectRef();

					pAttr->Set( TRUE );
					pLineAttrSet->AddAttr( pAttr );
				}

				{
					i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::NewObjectRef();

					pAttr->SetFunction( I3G_COMP_GREATER );
					pLineAttrSet->AddAttr( pAttr );
				}

				{
					i3BlendEnableAttr * pAttr = i3BlendEnableAttr::NewObjectRef();

					pAttr->Set( TRUE );
					pLineAttrSet->AddAttr( pAttr );
				}

				{
					i3BlendModeAttr * pAttr = i3BlendModeAttr::NewObjectRef();

					pAttr->SetDestination( I3G_BLEND_INVSRCALPHA ); 
					pLineAttrSet->AddAttr( pAttr );
				}
			}
		}

		pRoot->AddChild( pLineAttrSet );
	}

	// Vertex Array
	{
		VEC3D	v1, v2, v3, v4;
		i3VertexFormat format;

		format.SetHasPosition( true);
		format.SetHasNormal( false);
		format.SetHasColor( false);

		pVA = i3VertexArray::NewObjectRef();
		pVA->Create( &format, 4, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();
		
		i3Vector::Set( &v1, 0.f, 0.f, 0.f );
		i3Vector::Set( &v2, fWidth, 0.f, 0.f );
		i3Vector::Set( &v3, fWidth, 0.f, -fHeight );
		i3Vector::Set( &v4, 0.f, 0.f, -fHeight );

		pVA->SetPosition( 0, &v1 );
		pVA->SetPosition( 1, &v2 );
		pVA->SetPosition( 2, &v3 );
		pVA->SetPosition( 3, &v4 );
		
		pVA->Unlock();
	}

	// Index Array for Triangle
	if( bTri)
	{
		pIA = i3IndexArray::NewObjectRef();
		pIA->Create( 6, 0);				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();
		pIA->SetIndex16( 0,	0);
		pIA->SetIndex16( 1,	1);
		pIA->SetIndex16( 2,	2);
		pIA->SetIndex16( 3, 2);
		pIA->SetIndex16( 4, 3);
		pIA->SetIndex16( 5, 0);
		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::NewObjectRef();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::NewObjectRef();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	if( bLine )
	{
		pIA = i3IndexArray::NewObjectRef();
		pIA->Create( 8, 0);				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		pIA->SetIndex16( 0,	0);
		pIA->SetIndex16( 1,	1);
		pIA->SetIndex16( 2,	1);
		pIA->SetIndex16( 3,	2);
		pIA->SetIndex16( 4, 2);
		pIA->SetIndex16( 5, 3);
		pIA->SetIndex16( 6, 3);
		pIA->SetIndex16( 7, 0);

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::NewObjectRef();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 4 );
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::NewObjectRef();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}
	
	return pRoot;
}


void CGizmo::SetZFunc( i3Node * pNode, I3G_COMP_FUNC func )
{
	INT32	i;
	i3Node * pChild;

	I3ASSERT( pNode != NULL );

	for( i = 0; i < pNode->GetChildCount(); i++ )
	{
		pChild = pNode->GetChild( i );
		if( pChild->IsExactTypeOf( i3AttrSet::GetClassMeta() ) )
		{
			i3ZFuncAttr * pAttr = i3ZFuncAttr::NewObjectRef();
			pAttr->Set( func );
			((i3AttrSet*)pChild)->AddAttr( pAttr );
		}
	}
}

void CGizmo::SetAlphaFunc( i3Node * pNode, I3G_COMP_FUNC func )
{
	INT32	i;
	i3Node * pChild;

	I3ASSERT( pNode != NULL );

	for( i = 0; i < pNode->GetChildCount(); i++ )
	{
		pChild = pNode->GetChild( i );
		if( pChild->IsExactTypeOf( i3AttrSet::GetClassMeta() ) )
		{
			{
				i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::NewObjectRef();
				pAttr->Set( TRUE );
				((i3AttrSet*)pChild)->AddAttr( pAttr );
			}

			{
				i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::NewObjectRef();
				pAttr->SetFunction( func );
				((i3AttrSet*)pChild)->AddAttr( pAttr );
			}
		}
	}
}

void CGizmo::SetBlendMode( i3Node * pNode, I3G_BLEND src, I3G_BLEND dest )
{
	INT32	i;
	i3Node * pChild;

	I3ASSERT( pNode != NULL );

	for( i = 0; i < pNode->GetChildCount(); i++ )
	{
		pChild = pNode->GetChild( i );
		if( pChild->IsExactTypeOf( i3AttrSet::GetClassMeta() ) )
		{	
			{
				i3BlendEnableAttr * pAttr = i3BlendEnableAttr::NewObjectRef();
				pAttr->Set( TRUE );
				((i3AttrSet*)pChild)->AddAttr( pAttr );
			}

			{
				i3BlendModeAttr * pAttr = i3BlendModeAttr::NewObjectRef();
				pAttr->SetSource( src );
				pAttr->SetDestination( dest ); 
				((i3AttrSet*)pChild)->AddAttr( pAttr );
			}
		}
	}
}

void CGizmo::ChangeColor( i3Node * pNode, COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pChild;
	i3AttrSet * pAttrSet;
	i3RenderAttr * pAttr;

	I3ASSERT( pNode != NULL );
	I3ASSERT( pColor != NULL );

	for( i = 0; i < pNode->GetChildCount(); i++ )
	{
		pChild = pNode->GetChild( i );
		if( pChild->IsExactTypeOf( i3AttrSet::GetClassMeta() ) )
		{
			pAttrSet = (i3AttrSet*)pChild;
			for( j = 0; j < pAttrSet->GetAttrCount(); j++ )
			{
				pAttr = pAttrSet->GetAttr( j );
				if( pAttr->IsExactTypeOf( i3ColorAttr::GetClassMeta() ) )
				{
					((i3ColorAttr*)pAttr)->Set( pColor );
				}
			}
		}
	}
}

bool CGizmo::OnCollision( VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx)
{
	MATRIX full, inv, temp;
	VEC3D vStart, vEnd;
	i3Node * pChild;
	INT32 i;
	COLORREAL col;
	VEC3D axis;
	bool bResult = false;

	MATRIX mid;
	i3Matrix::Mul( &mid, m_pTransformRoot->GetMatrix(), getSceneObject()->GetMatrix());

	for( i = 0; i < 3; i++ )
	{
		i3Matrix::Mul( &full, m_pAxis[i]->GetMatrix(), &mid);
		i3Matrix::Mul( &temp, &full, pViewProjMtx);
		i3Matrix::Inverse( &inv, NULL, &temp);

   		i3Vector::TransformCoord( &vStart, pVStart, &inv);
		i3Vector::TransformCoord( &vEnd, pVEnd, &inv);
			
		pChild = m_pAxis[i]->GetChild(0);
		I3ASSERT( pChild != NULL);

		i3BoundBox * pBoundBox = pChild->GetBound();

		if( pBoundBox == NULL )
			continue;

		switch( i)
		{
			default :	i3Vector::Set( &axis, 1.0f,	0.0f,	0.0f);	break;
			case 1 :	i3Vector::Set( &axis, 0.0f,	1.0f,	0.0f);	break;
			case 2 :	i3Vector::Set( &axis, 0.0f,	0.0f,	1.0f);	break;
		}

		if( ContainAABBoxLine( &vStart, &vEnd, pBoundBox->GetMin(), pBoundBox->GetMax(), 100000.0f) )
		{
			i3Color::Set( &col, 1.f, 1.f, 0.f, 1.f );
			ChangeColor( pChild, &col );

			i3Vector::Copy( &m_vAxisMask, &axis);
			bResult = true;
		}
		else
		{
			i3Color::Set( &col, axis.x, axis.y, axis.z, 1.f );
			ChangeColor( pChild, &col );
		}
	}

	return bResult;
}
