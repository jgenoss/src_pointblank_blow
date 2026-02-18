#include "pch.h"
#include "GizmoMove.h"
#include "i3SceneUtil.h"

#define	RECTANGLE_SIZE		0.3f

VEC3D VecX[4] = 
{
	{	RECTANGLE_SIZE, 0.f,			0.f				},
	{	RECTANGLE_SIZE, 0.f,			-RECTANGLE_SIZE	},
	{	RECTANGLE_SIZE, 0.f,			0.f				},
	{	RECTANGLE_SIZE, RECTANGLE_SIZE, 0.f				}
};

I3_CLASS_INSTANCE( CGizmoMove, CGizmo);

CGizmoMove::CGizmoMove(void)
{
	m_pRect[0] = NULL;
	m_pRect[1] = NULL;
	m_pRect[2] = NULL;
}

CGizmoMove::~CGizmoMove(void)
{
}

BOOL CGizmoMove::Create(void)
{
	CGizmo::Create();
/*
	//	Move Axis
	COLORREAL col;
	i3Node * pRectangle, * pRectLine;
	i3BoundBox * pBoundBox;

	pRectangle = i3Node::NewObject();
	pRectangle->SetName( "Rectangle" );
	getSceneObject()->AddChild( pRectangle );

	pRectLine = i3Node::NewObject();
	pRectLine->SetName( "RectLine" );
	getSceneObject()->AddChild( pRectLine );
	
	// X รเ
	{
		// Rectangle
		i3Color::Set( &col, 1.0f, 1.0f, 0.f, 0.0f );

		{
			m_pTrans[0] = i3Transform::NewObjectRef();
			i3Matrix::Identity( m_pTrans[0]->GetMatrix() );

			{
				pBoundBox = i3BoundBox::NewObjectRef();

				m_pRect[0] = BuildRectangle( TRUE, FALSE, RECTANGLE_SIZE, RECTANGLE_SIZE, NULL, &col, &col );
				m_pRect[0]->SetName( "RectXZ" );

				m_pTrans[0]->AddChild( m_pRect[0] );

				SetZFunc( m_pRect[0], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pRect[0], I3G_COMP_GREATER );
				SetBlendMode( m_pRect[0], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pRect[0]->GetWrappingBound( pBoundBox );
				m_pRect[0]->SetBound( pBoundBox );
				m_pRect[0]->Release();
			}

			pRectangle->AddChild( m_pTrans[0] );
		}
	}

	{	// RectLine
		i3VertexArray * pVA;
		i3Color::Set( &col, 1.f, 0.f, 0.f, 1.f );

		m_pLine[0] = i3SceneUtil::BuildLine( pVA, 2, &col );

		pVA->Lock();
		pVA->SetPositions( 0, 4, VecX);
		pVA->Unlock();

		m_pLine[0]->SetName( "RectLineX" );
		SetZFunc( m_pLine[0], I3G_COMP_ALWAYS );
		pRectLine->AddChild( m_pLine[0] );
		m_pLine[0]->Release();
	}

	// Y รเ
	{	// Rectangle
		i3Color::Set( &col, 1.0f, 1.0f, 0.f, 0.0f );
		{
			m_pTrans[1] = i3Transform::NewObjectRef();
			i3Matrix::SetRotateZDeg( m_pTrans[1]->GetMatrix(), 90.0f);
			{
				pBoundBox = i3BoundBox::NewObjectRef();

				m_pRect[1] = BuildRectangle( TRUE, FALSE, RECTANGLE_SIZE, RECTANGLE_SIZE, NULL, &col, &col );
				m_pRect[1]->SetName( "RectYZ" );
				m_pTrans[1]->AddChild( m_pRect[1] );
				
				SetZFunc( m_pRect[1], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pRect[1], I3G_COMP_GREATER );
				SetBlendMode( m_pRect[1], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pRect[1]->GetWrappingBound( pBoundBox );
				m_pRect[1]->SetBound( pBoundBox );
				m_pRect[1]->Release();
			}
			pRectangle->AddChild( m_pTrans[1] );
		}
	}
	{	// RectLine
		i3VertexArray * pVA;
		VEC3D Vec;

		i3Color::Set( &col, 0.f, 1.f, 0.f, 1.f );

		m_pLine[1] = i3SceneUtil::BuildLine( pVA, 2, &col );
		pVA->Lock();
		i3Vector::Set( &Vec, 0.0f, RECTANGLE_SIZE, 0.f );
		pVA->SetPosition( 0, &Vec );
		i3Vector::Set( &Vec, RECTANGLE_SIZE, RECTANGLE_SIZE, 0.f );
		pVA->SetPosition( 1, &Vec );
		i3Vector::Set( &Vec, 0.0f, RECTANGLE_SIZE, 0.f );
		pVA->SetPosition( 2, &Vec );
		i3Vector::Set( &Vec, 0.0f, RECTANGLE_SIZE, -RECTANGLE_SIZE );
		pVA->SetPosition( 3, &Vec );
		pVA->Unlock();
		m_pLine[1]->SetName( "RectLineY" );
		SetZFunc( m_pLine[1], I3G_COMP_ALWAYS );
		pRectLine->AddChild( m_pLine[1] );
		m_pLine[1]->Release();
	}

	// Z รเ
	{	// Rectangle
		i3Color::Set( &col, 1.0f, 1.0f, 0.f, 0.0f );
		{
			m_pTrans[2] = i3Transform::NewObjectRef();
			i3Matrix::SetRotateXDeg( m_pTrans[2]->GetMatrix(), 90.0f);
			{
				pBoundBox = i3BoundBox::NewObjectRef();

				m_pRect[2] = BuildRectangle( TRUE, FALSE, RECTANGLE_SIZE, RECTANGLE_SIZE, NULL, &col, &col );
				m_pRect[2]->SetName( "RectXY" );
				SetZFunc( m_pRect[2], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pRect[2], I3G_COMP_GREATER );
				SetBlendMode( m_pRect[2], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pRect[2]->GetWrappingBound( pBoundBox );
				m_pRect[2]->SetBound( pBoundBox );
				m_pTrans[2]->AddChild( m_pRect[2] );
				m_pRect[2]->Release();
			}
			pRectangle->AddChild( m_pTrans[2] );
		}
	}
	{	// RectLine
		i3VertexArray * pVA;
		VEC3D Vec;
		i3Color::Set( &col, 0.f, 0.f, 1.f, 1.f );
		m_pLine[2] = i3SceneUtil::BuildLine( pVA, 2, &col );
		pVA->Lock();
		i3Vector::Set( &Vec, 0.0f, 0.0f, -RECTANGLE_SIZE );
		pVA->SetPosition( 0, &Vec );
		i3Vector::Set( &Vec, RECTANGLE_SIZE, 0.0f, -RECTANGLE_SIZE );
		pVA->SetPosition( 1, &Vec );
		i3Vector::Set( &Vec, 0.0f, 0.0f, -RECTANGLE_SIZE );
		pVA->SetPosition( 2, &Vec );
		i3Vector::Set( &Vec, 0.0f, RECTANGLE_SIZE, -RECTANGLE_SIZE );
		pVA->SetPosition( 3, &Vec );
		pVA->Unlock();
		m_pLine[2]->SetName( "RectLineZ" );
		SetZFunc( m_pLine[2], I3G_COMP_ALWAYS );
		pRectLine->AddChild( m_pLine[2] );
		m_pLine[2]->Release();
	}
*/
	return TRUE;
}

void CGizmoMove::ChangeLineColorX( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < getSceneObject()->GetChildCount(); i++ )
	{
		pNode = getSceneObject()->GetChild( i );
		if( !i3String::Compare( pNode->GetName(), "RectLine" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( pChild->IsExactTypeOf( i3Transform::GetClassMeta() ) )
					pChild = pChild->GetChild( 0 );
				if( !i3String::Compare( pChild->GetName(), "RectLineX" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void CGizmoMove::ChangeLineColorY( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < getSceneObject()->GetChildCount(); i++ )
	{
		pNode = getSceneObject()->GetChild( i );
		if( !i3String::Compare( pNode->GetName(), "RectLine" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( pChild->IsExactTypeOf( i3Transform::GetClassMeta() ) )
					pChild = pChild->GetChild( 0 );
				if( !i3String::Compare( pChild->GetName(), "RectLineY" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void CGizmoMove::ChangeLineColorZ( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < getSceneObject()->GetChildCount(); i++ )
	{
		pNode = getSceneObject()->GetChild( i );
		if( !i3String::Compare( pNode->GetName(), "RectLine" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( pChild->IsExactTypeOf( i3Transform::GetClassMeta() ) )
					pChild = pChild->GetChild( 0 );
				if( !i3String::Compare( pChild->GetName(), "RectLineZ" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void CGizmoMove::ChangeRectColorAll( COLORREAL * pColor )
{
	ChangeRectColorX( pColor );
	ChangeRectColorY( pColor );
	ChangeRectColorZ( pColor );
}

void CGizmoMove::ChangeRectColorX( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < getSceneObject()->GetChildCount(); i++ )
	{
		pNode = getSceneObject()->GetChild( i );
		if( !i3String::Compare( pNode->GetName(), "Rectangle" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( pChild->IsExactTypeOf( i3Transform::GetClassMeta() ) )
					pChild = pChild->GetChild( 0 );
				if( !i3String::Compare( pChild->GetName(), "RectXZ" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void CGizmoMove::ChangeRectColorY( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < getSceneObject()->GetChildCount(); i++ )
	{
		pNode = getSceneObject()->GetChild( i );
		if( !i3String::Compare( pNode->GetName(), "Rectangle" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( pChild->IsExactTypeOf( i3Transform::GetClassMeta() ) )
					pChild = pChild->GetChild( 0 );
				if( !i3String::Compare( pChild->GetName(), "RectYZ" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void CGizmoMove::ChangeRectColorZ( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < getSceneObject()->GetChildCount(); i++ )
	{
		pNode = getSceneObject()->GetChild( i );
		if( !i3String::Compare( pNode->GetName(), "Rectangle" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( pChild->IsExactTypeOf( i3Transform::GetClassMeta() ) )
					pChild = pChild->GetChild( 0 );
				if( !i3String::Compare( pChild->GetName(), "RectXY" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}


bool CGizmoMove::OnCollision( VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx)
{
	/*
	INT32 i;
	MATRIX full, temp, inv;
	VEC3D vStart, vEnd;
	COLORREAL col;
	bool bResult = false;
	static VEC3D	s_vecAxisMask[3]=
	{
		{	1.0f,	0.0f,	1.0f	},
		{	0.0f,	1.0f,	1.0f	},
		{	1.0f,	1.0f,	0.0f	}
	};

	for( i = 0; i < 3; i++ )
	{
		i3Matrix::Mul( &full, m_pTrans[i]->GetMatrix(), getSceneObject()->GetMatrix() );
		i3Matrix::Mul( &temp, &full, pViewProjMtx);
		i3Matrix::Inverse( &inv, NULL, &temp);

   		i3Vector::TransformCoord( &vStart, pVStart, &inv);
		i3Vector::TransformCoord( &vEnd, pVEnd, &inv);
		

		i3BoundBox * pBoundBox = m_pRect[i]->GetBound();

		if((bResult == false) && ContainAABBoxLine( &vStart, &vEnd, pBoundBox->GetMin(), pBoundBox->GetMax(), 100000.0f) )
		{
			i3Color::Set( &col, 1.f, 1.f, 0.f, 0.5f );

			ChangeColor( m_pRect[i], &col );

			pFramework->SetAxisMask( & s_vecAxisMask[i]);
			bResult = true;
		}
		else
		{
			i3Color::Set( &col, 0.f, 0.f, 0.f, 0.0f );

			ChangeColor( m_pRect[i], &col );
		}
	}

	if( bResult)
		return bResult;
*/
	return CGizmo::OnCollision( pVStart, pVEnd, pViewProjMtx);
}
