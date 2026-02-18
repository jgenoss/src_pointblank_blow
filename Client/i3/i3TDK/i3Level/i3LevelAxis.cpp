#include "stdafx.h"
#include "i3LevelAxis.h"
#include "i3LevelViewport.h"

I3_CLASS_INSTANCE( i3LevelAxis);

i3LevelAxis::~i3LevelAxis(void)
{
	I3_SAFE_RELEASE( m_pTargetObj);
}

static COLORREAL	s_InitAxisInfo[3] =
{
	COLORREAL(	1.0f,	0.0f,	0.0f,	0.0f),
	COLORREAL(	0.0f,	1.0f,	0.0f,	0.0f),
	COLORREAL(	0.0f,	0.0f,	1.0f,	0.0f)
};

bool i3LevelAxis::CreateAxis( INT32 AxisDiv, REAL32 Length, REAL32 rad)
{
	i3Node * pNode;
	i3AttrSet * pAxisRoot;
	i3BoundBox * pBoundBox;
	INT32 i;
	i3ShaderAttr * pShaderAttr;

	i3Matrix::Identity( GetSceneObject()->GetMatrix());

	pAxisRoot = i3AttrSet::new_object_ref();		// pAxisRoot변수는 곧 없어질것이니까...new_object_ref로 해야함..(2012.09.11.수빈)
	pAxisRoot->SetName( "Axis" );
	pAxisRoot->SetPriority( 0);
	GetSceneObject()->AddChild( pAxisRoot );

	m_pRoot->SetPriority( 20);

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		pAxisRoot->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		pAxisRoot->AddAttr( pAttr);
	}

	{
		pShaderAttr = i3ShaderAttr::new_object_ref();
		pShaderAttr->setAddCacheState( false);
		pShaderAttr->setTargetVertexShaderVersion( I3G_SHADER_VERSION_2);
		pShaderAttr->setTargetPixelShaderVersion( I3G_SHADER_VERSION_2);
		pShaderAttr->setSource( "i3Gizmo.hlsl");
	}

	COLOR	s_AxisColor[3] =
	{
		COLOR(	255, 0, 0, 255 ),
		COLOR(	0, 255, 0, 255 ),
		COLOR(	0, 0, 255, 255 )
	};

	for( i = 0; i < 3; i++)
	{
		m_pAxis[i] = i3Transform::new_object_ref();
		pAxisRoot->AddChild( m_pAxis[i]);

		m_pAxisAttrSet[i] = i3AttrSet::new_object_ref();
		m_pAxis[i]->AddChild( m_pAxisAttrSet[i]);

		{
			m_pAxisAttrSet[i]->AddAttr( pShaderAttr);

			m_pAxis_ColorAttr[i] = i3ColorAttr::new_object_ref();
			m_pAxis_ColorAttr[i]->Set( &s_AxisColor[i]);
			m_pAxisAttrSet[i]->AddAttr( m_pAxis_ColorAttr[i]);
		}

		{
			pNode = i3SceneUtil::BuildAxisMesh( AxisDiv, Length, rad, TRUE, FALSE, nullptr, nullptr); //&s_InitAxisInfo[i], &s_InitAxisInfo[i]);

			m_pAxisAttrSet[i]->AddChild( pNode);
			I3_MUST_RELEASE(pNode);

			pBoundBox = i3BoundBox::new_object_ref();
			pBoundBox->Reset();
			m_pAxisAttrSet[i]->GetWrappingBound( pBoundBox);
			m_pAxisAttrSet[i]->SetBound( pBoundBox);
		}
	}

	i3Matrix::SetRotateZDeg( m_pAxis[1]->GetMatrix(), 90.0f);
	i3Matrix::SetRotateYDeg( m_pAxis[2]->GetMatrix(), 90.0f);

	return true;
}

BOOL i3LevelAxis::Create(void)
{
	return CreateAxis();
}

i3Node * i3LevelAxis::BuildRectangle( BOOL bTri, BOOL bLine, REAL32 fWidth, REAL32 fHeight, COLORREAL * pLineColor, COLORREAL * pDiffuse, COLORREAL * pAmbient )
{
	i3Node * pRoot;
	i3AttrSet * pTriAttrSet = nullptr;
	i3AttrSet * pLineAttrSet = nullptr;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	i3GeometryAttr * pGeoAttr;
	i3Geometry *	pGeo;


	pRoot = i3Node::new_object();

	if( bTri )
	{
		pTriAttrSet = i3AttrSet::new_object_ref();
		// Cull mode off
		{
			i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();
			pAttr->Set( I3G_FACE_CULL_NONE );
			pTriAttrSet->AddAttr( pAttr );
		}

		if( pDiffuse != nullptr)
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		if( pAmbient != nullptr && pDiffuse != nullptr)
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
			//pAttr->SetAmbient( pAmbient);
			pAttr->SetDiffuse( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pTriAttrSet );
	}

	// Line
	if( bLine )
	{
		pLineAttrSet = i3AttrSet::new_object_ref();

		if( pLineColor != nullptr )
		{
			i3ColorAttr * pColorAttr = i3ColorAttr::new_object_ref();

			pColorAttr->Set( pLineColor );
			pLineAttrSet->AddAttr(pColorAttr);

			if( i3Color::GetA( pLineColor ) < 255 )
			{
				{
					i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();
					pAttr->Set( TRUE );
					pLineAttrSet->AddAttr( pAttr );
				}

				{
					i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();
					pAttr->SetFunction( I3G_COMP_GREATER );
					pLineAttrSet->AddAttr( pAttr );
				}

				{
					i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
					pAttr->Set( TRUE );
					pLineAttrSet->AddAttr( pAttr );
				}

				{
					i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
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

		pVA = i3VertexArray::new_object_ref();
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
		pIA = i3IndexArray::new_object_ref();
		pIA->Create( 6, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();
		pIA->SetIndex( 0,	0);
		pIA->SetIndex( 1,	1);
		pIA->SetIndex( 2,	2);
		pIA->SetIndex( 3, 2);
		pIA->SetIndex( 4, 3);
		pIA->SetIndex( 5, 0);
		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	if( bLine )
	{
		pIA = i3IndexArray::new_object_ref();
		pIA->Create( 8, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		pIA->SetIndex( 0,	0);
		pIA->SetIndex( 1,	1);
		pIA->SetIndex( 2,	1);
		pIA->SetIndex( 3,	2);
		pIA->SetIndex( 4, 2);
		pIA->SetIndex( 5, 3);
		pIA->SetIndex( 6, 3);
		pIA->SetIndex( 7, 0);

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 4 );
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}
	
	return pRoot;
}


void i3LevelAxis::SetZFunc( i3Node * pNode, I3G_COMP_FUNC func )
{
	INT32	i;
	i3Node * pChild;

	I3ASSERT( pNode != nullptr );

	for( i = 0; i < pNode->GetChildCount(); i++ )
	{
		pChild = pNode->GetChild( i );
		if( i3::same_of<i3AttrSet*>(pChild))
		{
			i3ZFuncAttr * pAttr = i3ZFuncAttr::new_object_ref();
			pAttr->Set( func );
			((i3AttrSet*)pChild)->AddAttr( pAttr );
		}
	}
}

void i3LevelAxis::SetAlphaFunc( i3Node * pNode, I3G_COMP_FUNC func )
{
	INT32	i;
	i3Node * pChild;

	I3ASSERT( pNode != nullptr );

	for( i = 0; i < pNode->GetChildCount(); i++ )
	{
		pChild = pNode->GetChild( i );
		if( i3::same_of<i3AttrSet*>(pChild))
		{
			{
				i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();
				pAttr->Set( TRUE );
				((i3AttrSet*)pChild)->AddAttr( pAttr );
			}

			{
				i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();
				pAttr->SetFunction( func );
				((i3AttrSet*)pChild)->AddAttr( pAttr );
			}
		}
	}
}

void i3LevelAxis::SetBlendMode( i3Node * pNode, I3G_BLEND src, I3G_BLEND dest )
{
	INT32	i;
	i3Node * pChild;

	I3ASSERT( pNode != nullptr );

	for( i = 0; i < pNode->GetChildCount(); i++ )
	{
		pChild = pNode->GetChild( i );
		if( i3::same_of<i3AttrSet*>(pChild))
		{	
			{
				i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
				pAttr->Set( TRUE );
				((i3AttrSet*)pChild)->AddAttr( pAttr );
			}

			{
				i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
				pAttr->SetSource( src );
				pAttr->SetDestination( dest ); 
				((i3AttrSet*)pChild)->AddAttr( pAttr );
			}
		}
	}
}

void i3LevelAxis::ChangeColor( i3Node * pNode, COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pChild;
	i3AttrSet * pAttrSet;
	i3RenderAttr * pAttr;

	I3ASSERT( pNode != nullptr );
	I3ASSERT( pColor != nullptr );

	for( i = 0; i < pNode->GetChildCount(); i++ )
	{
		pChild = pNode->GetChild( i );
		if( i3::same_of<i3AttrSet*>(pChild))
		{
			pAttrSet = (i3AttrSet*)pChild;
			for( j = 0; j < pAttrSet->GetAttrCount(); j++ )
			{
				pAttr = pAttrSet->GetAttr( j );
				if( i3::same_of<i3ColorAttr*>(pAttr))
				{
					((i3ColorAttr*)pAttr)->Set( pColor );
				}
			}
		}
	}
}

bool i3LevelAxis::OnCollision( i3LevelFramework * pFramework, I3_LEVEL_EDIT_CONTEXT * pCtx, VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx)
{
	MATRIX full, inv, temp;
	VEC3D vStart, vEnd;
	i3Node * pChild;
	INT32 i;
	COLORREAL col;
	VEC3D axis;
	bool bResult = false;

	for( i = 0; i < 3; i++ )
	{
		i3Matrix::Mul( &full, m_pAxis[i]->GetMatrix(), GetSceneObject()->GetMatrix() );
		i3Matrix::Mul( &temp, &full, pViewProjMtx);
		i3Matrix::Inverse( &inv, nullptr, &temp);

   		i3Vector::TransformCoord( &vStart, pVStart, &inv);
		i3Vector::TransformCoord( &vEnd, pVEnd, &inv);
			
		pChild = m_pAxis[i]->GetChild(0);
		I3ASSERT( pChild != nullptr);

		i3BoundBox * pBoundBox = pChild->GetBound();

		if( pBoundBox == nullptr )
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
			m_pAxis_ColorAttr[i]->Set( &col);

			pFramework->SetAxisMask( &axis);
			bResult = true;
		}
		else
		{
			i3Color::Set( &col, axis.x, axis.y, axis.z, 1.f );
			m_pAxis_ColorAttr[i]->Set( &col);
		}
	}

	return bResult;
}
