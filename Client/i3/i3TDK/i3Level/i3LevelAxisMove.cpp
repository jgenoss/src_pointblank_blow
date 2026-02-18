#include "stdafx.h"
#include "i3LevelAxisMove.h"
#include "i3SceneUtil.h"
#include "i3LevelViewport.h"

#define	RECTANGLE_SIZE		0.3f

I3_CLASS_INSTANCE( i3LevelAxisMove);

BOOL i3LevelAxisMove::Create(void)
{
	if( FALSE == i3LevelAxis::Create() )
	{
		return FALSE;
	}

	i3AttrSet* pAxisMoveRoot = i3AttrSet::new_object_ref();
	pAxisMoveRoot->SetName( "AxisMove");
	GetSceneObject()->AddChild( pAxisMoveRoot );

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		pAxisMoveRoot->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		pAxisMoveRoot->AddAttr( pAttr);
	}

	/*{
		i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::NewObjectRef();

		pAttr->Set( TRUE );
		pAxisMoveRoot->AddAttr( pAttr);
	}

	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::NewObjectRef();
		pAttr->SetFunction( I3G_COMP_GREATER );
		pAxisMoveRoot->AddAttr( pAttr );
	}*/

	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

		pAttr->Set( TRUE );
		pAxisMoveRoot->AddAttr( pAttr);
	}

	{
		i3BlendModeAttr* pAttr = i3BlendModeAttr::new_object_ref();

		pAttr->SetSource( I3G_BLEND_SRCALPHA );
		pAttr->SetDestination( I3G_BLEND_INVSRCALPHA );
		pAxisMoveRoot->AddAttr( pAttr);
	}

	//	Move Axis
	COLORREAL col;

	i3Node * pRectangle, * pRectLine;
	i3BoundBox * pBoundBox;

	pRectangle = i3Node::new_object_ref();		// 곧없어질 변수에는 new_object_ref를 걸어야함..(2012.09.11.수빈)
	pRectangle->SetName( "Rectangle" );
	pAxisMoveRoot->AddChild( pRectangle );

	pRectLine = i3Node::new_object_ref();		// 곧없어질 변수에는 new_object_ref를 걸어야함..(2012.09.11.수빈)
	pRectLine->SetName( "RectLine" );
	pAxisMoveRoot->AddChild( pRectLine );

	for( int i = 0; i < 3; ++i )
	{
		m_pTrans[i] = i3Transform::new_object_ref();

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 0.0f );

		{
			pBoundBox = i3BoundBox::new_object_ref();

			m_pRect[i] = BuildRectangle( TRUE, FALSE, RECTANGLE_SIZE, RECTANGLE_SIZE, nullptr, &col, &col );

			switch( i )
			{
			case	0:
					m_pRect[i]->SetName( "RectXZ" );
					break;

			case	1:
					i3Matrix::SetRotateZDeg( m_pTrans[1]->GetMatrix(), 90.0f);
					m_pRect[i]->SetName( "RectYZ" );
					break;

			case	2:
					i3Matrix::SetRotateXDeg( m_pTrans[2]->GetMatrix(), 90.0f);
					m_pRect[i]->SetName( "RectXY" );
					break;
			}			

			m_pTrans[i]->AddChild( m_pRect[i] );

			m_pRect[i]->GetWrappingBound( pBoundBox );
			m_pRect[i]->SetBound( pBoundBox );
			I3_MUST_RELEASE(m_pRect[i]);
		}

		pRectangle->AddChild( m_pTrans[i] );		
	}	

	{	// RectLine
		i3VertexArray * pVA;
		VEC3D Vec;

		i3Color::Set( &col, 0.8f, 0.8f, 0.8f, 1.f );

		m_pLine[0] = i3SceneUtil::BuildLine( pVA, 2, &col );

		pVA->Lock();
		i3Vector::Set( &Vec, RECTANGLE_SIZE, 0.f, 0.f );
		pVA->SetPosition( 0, &Vec );
		i3Vector::Set( &Vec, RECTANGLE_SIZE, 0.f, -RECTANGLE_SIZE );
		pVA->SetPosition( 1, &Vec );
		i3Vector::Set( &Vec, RECTANGLE_SIZE, 0.f, 0.f );
		pVA->SetPosition( 2, &Vec );
		i3Vector::Set( &Vec, RECTANGLE_SIZE, RECTANGLE_SIZE, 0.f );
		pVA->SetPosition( 3, &Vec );
		pVA->Unlock();
		m_pLine[0]->SetName( "RectLineX" );
		pRectLine->AddChild( m_pLine[0] );
		I3_MUST_RELEASE(m_pLine[0]);
	}

	{	// RectLine
		i3VertexArray * pVA;
		VEC3D Vec;

		i3Color::Set( &col, 0.8f, 0.8f, 0.8f, 1.f );

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
		pRectLine->AddChild( m_pLine[1] );
		I3_MUST_RELEASE(m_pLine[1]);
	}

	{	// RectLine
		i3VertexArray * pVA;
		VEC3D Vec;
		
		i3Color::Set( &col, 0.8f, 0.8f, 0.8f, 1.f );

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
		pRectLine->AddChild( m_pLine[2] );
		I3_MUST_RELEASE(m_pLine[2]);
	}

	return TRUE;
}

void i3LevelAxisMove::ChangeLineColorX( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < GetSceneObject()->GetChildCount(); i++ )
	{
		pNode = GetSceneObject()->GetChild( i );
		if( i3::generic_is_iequal( pNode->GetName(), "RectLine" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( i3::same_of<i3Transform*>(pChild))
					pChild = pChild->GetChild( 0 );
				if( i3::generic_is_iequal( pChild->GetName(), "RectLineX" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void i3LevelAxisMove::ChangeLineColorY( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < GetSceneObject()->GetChildCount(); i++ )
	{
		pNode = GetSceneObject()->GetChild( i );
		if( i3::generic_is_iequal( pNode->GetName(), "RectLine" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( i3::same_of<i3Transform*>(pChild))
					pChild = pChild->GetChild( 0 );
				if( i3::generic_is_iequal( pChild->GetName(), "RectLineY" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void i3LevelAxisMove::ChangeLineColorZ( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < GetSceneObject()->GetChildCount(); i++ )
	{
		pNode = GetSceneObject()->GetChild( i );
		if( i3::generic_is_iequal( pNode->GetName(), "RectLine" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( i3::same_of<i3Transform* >(pChild))
					pChild = pChild->GetChild( 0 );
				if( i3::generic_is_iequal( pChild->GetName(), "RectLineZ" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void i3LevelAxisMove::ChangeRectColorAll( COLORREAL * pColor )
{
	ChangeRectColorX( pColor );
	ChangeRectColorY( pColor );
	ChangeRectColorZ( pColor );
}

void i3LevelAxisMove::ChangeRectColorX( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < GetSceneObject()->GetChildCount(); i++ )
	{
		pNode = GetSceneObject()->GetChild( i );
		if( i3::generic_is_iequal( pNode->GetName(), "Rectangle" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( i3::same_of<i3Transform*>(pChild))
					pChild = pChild->GetChild( 0 );
				if( i3::generic_is_iequal( pChild->GetName(), "RectXZ" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void i3LevelAxisMove::ChangeRectColorY( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < GetSceneObject()->GetChildCount(); i++ )
	{
		pNode = GetSceneObject()->GetChild( i );
		if( i3::generic_is_iequal( pNode->GetName(), "Rectangle" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( i3::same_of<i3Transform*>(pChild))
					pChild = pChild->GetChild( 0 );
				if( i3::generic_is_iequal( pChild->GetName(), "RectYZ" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}

void i3LevelAxisMove::ChangeRectColorZ( COLORREAL * pColor )
{
	INT32	i, j;
	i3Node * pNode, * pChild;

	for( i = 0; i < GetSceneObject()->GetChildCount(); i++ )
	{
		pNode = GetSceneObject()->GetChild( i );
		if( i3::generic_is_iequal( pNode->GetName(), "Rectangle" ) )
		{
			for( j = 0; j < pNode->GetChildCount(); j++ )
			{
				pChild = pNode->GetChild( j );
				if( i3::same_of<i3Transform*>(pChild))
					pChild = pChild->GetChild( 0 );
				if( i3::generic_is_iequal( pChild->GetName(), "RectXY" ) )
				{
					ChangeColor( pChild, pColor );
					return;
				}
			}
		}
	}
}


bool i3LevelAxisMove::OnCollision( i3LevelFramework * pFramework, I3_LEVEL_EDIT_CONTEXT * pCtx, VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx)
{
	INT32 i;
	MATRIX full, temp, inv;
	VEC3D vStart, vEnd;
	COLORREAL col;
	bool bResult = false;
	static VEC3D	s_vecAxisMask[3]=
	{
		VEC3D(	1.0f,	0.0f,	1.0f	),
		VEC3D(	0.0f,	1.0f,	1.0f	),
		VEC3D(	1.0f,	1.0f,	0.0f	)
	};

	for( i = 0; i < 3; i++ )
	{
		i3Matrix::Mul( &full, m_pTrans[i]->GetMatrix(), GetSceneObject()->GetMatrix() );
		i3Matrix::Mul( &temp, &full, pViewProjMtx);
		i3Matrix::Inverse( &inv, nullptr, &temp);

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
			i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 0.0f );
			ChangeColor( m_pRect[i], &col );
		}
	}

	if( bResult)
		return bResult;

	return i3LevelAxis::OnCollision( pFramework, pCtx, pVStart, pVEnd, pViewProjMtx);
}
