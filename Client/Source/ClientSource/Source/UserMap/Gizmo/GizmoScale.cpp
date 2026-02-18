#include "pch.h"
#include "GizmoScale.h"

#define SCALEGIZMO_SIZE		0.5f
#define SCALEGIZMO_SIZE2	0.47f

I3_CLASS_INSTANCE( CGizmoScale, CGizmo);

CGizmoScale::CGizmoScale(void)
{
}

CGizmoScale::~CGizmoScale(void)
{
}


BOOL CGizmoScale::Create( void )
{
	CGizmo::Create();
	
	//	Move Axis
	COLORREAL col;
	VEC3D Vec1, Vec2, Vec3;
	i3Node * pScale, * pScaleLine;
	i3BoundBox * pBoundBox;

	pScale = i3Node::NewObject();
	pScale->SetName( "Scale" );
	getSceneObject()->AddChild( pScale );

	pScaleLine = i3Node::NewObject();
	pScaleLine->SetName( "ScaleLine" );
	getSceneObject()->AddChild( pScaleLine );
	
	// X รเ
	{	// Triangle
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );
		{
			m_pTrans[0] = i3Transform::NewObjectRef();
			i3Matrix::Identity( m_pTrans[0]->GetMatrix() );
			{
				pBoundBox = i3BoundBox::NewObjectRef();

				i3Vector::Set( &Vec1, 0.f, 0.f, 0.f );
				i3Vector::Set( &Vec2, SCALEGIZMO_SIZE, 0.f, 0.f );
				i3Vector::Set( &Vec3, 0.f, 0.f, -SCALEGIZMO_SIZE );
				m_pCap[0] = i3SceneUtil::BuildTriangle( &Vec1, &Vec2, &Vec3, TRUE, TRUE, &col, &col, &col );
				m_pCap[0]->SetName( "ScaleXZ" );
				SetZFunc( m_pCap[0], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pCap[0], I3G_COMP_GREATER );
				SetBlendMode( m_pCap[0], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pCap[0]->GetWrappingBound( pBoundBox );
				m_pCap[0]->SetBound( pBoundBox );
				m_pTrans[0]->AddChild( m_pCap[0] );
				m_pCap[0]->Release();
			}
			pScale->AddChild( m_pTrans[0] );
		}
	}

	{	// ScaleLine
		i3VertexArray * pVA;
		VEC3D Vec;
		i3Color::Set( &col, 1.f, 1.f, 0.f, 1.f );
		m_pLine[0] = i3SceneUtil::BuildLine( pVA, 2, &col );
		pVA->Lock();
		i3Vector::Set( &Vec, SCALEGIZMO_SIZE, 0.f, 0.f );
		pVA->SetPosition( 0, &Vec );
		i3Vector::Set( &Vec, 0.f, 0.f, -SCALEGIZMO_SIZE );
		pVA->SetPosition( 1, &Vec );
		i3Vector::Set( &Vec, SCALEGIZMO_SIZE2, 0.f, 0.f );
		pVA->SetPosition( 2, &Vec );
		i3Vector::Set( &Vec, 0.f, 0.f, -SCALEGIZMO_SIZE2 );
		pVA->SetPosition( 3, &Vec );
		pVA->Unlock();
		m_pLine[0]->SetName( "ScaleLineXZ" );
		SetZFunc( m_pLine[0], I3G_COMP_ALWAYS );
		pScaleLine->AddChild( m_pLine[0] );
		m_pLine[0]->Release();
	}

	// Y รเ
	{	// Triangle
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );

		{
			m_pTrans[1] = i3Transform::NewObjectRef();
			i3Matrix::Identity( m_pTrans[1]->GetMatrix() );
			{
				pBoundBox = i3BoundBox::NewObjectRef();

				i3Vector::Set( &Vec1, 0.f, 0.f, 0.f );
				i3Vector::Set( &Vec2, 0.0f, SCALEGIZMO_SIZE, 0.f );
				i3Vector::Set( &Vec3, 0.0f, 0.f, -SCALEGIZMO_SIZE );
				m_pCap[1] = i3SceneUtil::BuildTriangle( &Vec1, &Vec2, &Vec3, TRUE, TRUE, &col, &col, &col );
				m_pCap[1]->SetName( "ScaleYZ" );
				SetZFunc( m_pCap[1], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pCap[1], I3G_COMP_GREATER );
				SetBlendMode( m_pCap[1], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pCap[1]->GetWrappingBound( pBoundBox );
				m_pCap[1]->SetBound( pBoundBox );
				m_pTrans[1]->AddChild( m_pCap[1] );
				m_pCap[1]->Release();
			}
			pScale->AddChild( m_pTrans[1] );
		}
	}
	{	// ScaleLine
		i3VertexArray * pVA;
		VEC3D Vec;

		i3Color::Set( &col, 1.f, 1.f, 0.f, 1.f );

		m_pLine[1] = i3SceneUtil::BuildLine( pVA, 2, &col );
		pVA->Lock();
		i3Vector::Set( &Vec, 0.f, SCALEGIZMO_SIZE, 0.f );
		pVA->SetPosition( 0, &Vec );
		i3Vector::Set( &Vec, 0.f, 0.f, -SCALEGIZMO_SIZE );
		pVA->SetPosition( 1, &Vec );
		i3Vector::Set( &Vec, 0.f, SCALEGIZMO_SIZE2, 0.f );
		pVA->SetPosition( 2, &Vec );
		i3Vector::Set( &Vec, 0.f, 0.f, -SCALEGIZMO_SIZE2 );
		pVA->SetPosition( 3, &Vec );
		pVA->Unlock();

		m_pLine[1]->SetName( "ScaleLineYZ" );

		SetZFunc( m_pLine[1], I3G_COMP_ALWAYS );

		pScaleLine->AddChild( m_pLine[1] );

		m_pLine[1]->Release();
	}
	
	// Z รเ
	{	// Triangle
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );
		{
			m_pTrans[2] = i3Transform::NewObjectRef();
			i3Matrix::Identity( m_pTrans[2]->GetMatrix() );
			{
				pBoundBox = i3BoundBox::NewObjectRef();

				i3Vector::Set( &Vec1, 0.f, 0.f, 0.f );
				i3Vector::Set( &Vec2, SCALEGIZMO_SIZE, 0.f, 0.f );
				i3Vector::Set( &Vec3, 0.f, SCALEGIZMO_SIZE, 0.f );
				m_pCap[2] = i3SceneUtil::BuildTriangle( &Vec1, &Vec2, &Vec3, TRUE, TRUE, &col, &col, &col );
				m_pCap[2]->SetName( "ScaleXY" );
				SetZFunc( m_pCap[2], I3G_COMP_NOTEQUAL );
				SetAlphaFunc( m_pCap[2], I3G_COMP_GREATER );
				SetBlendMode( m_pCap[2], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pCap[2]->GetWrappingBound( pBoundBox );
				m_pCap[2]->SetBound( pBoundBox );
				m_pTrans[2]->AddChild( m_pCap[2] );
				m_pCap[2]->Release();
			}
			pScale->AddChild( m_pTrans[2] );
		}		
	}
	{	// ScaleLine
		i3VertexArray * pVA;
		VEC3D Vec;
		i3Color::Set( &col, 1.f, 1.f, 0.f, 1.f );
		m_pLine[2] = i3SceneUtil::BuildLine( pVA, 2, &col );
		pVA->Lock();
		i3Vector::Set( &Vec, SCALEGIZMO_SIZE, 0.f, 0.f );
		pVA->SetPosition( 0, &Vec );
		i3Vector::Set( &Vec, 0.f, SCALEGIZMO_SIZE, 0.f );
		pVA->SetPosition( 1, &Vec );
		i3Vector::Set( &Vec, SCALEGIZMO_SIZE2, 0.f, 0.f );
		pVA->SetPosition( 2, &Vec );
		i3Vector::Set( &Vec, 0.f, SCALEGIZMO_SIZE2, 0.f );
		pVA->SetPosition( 3, &Vec );
		pVA->Unlock();
		m_pLine[2]->SetName( "ScaleLineXY" );
		SetZFunc( m_pLine[2], I3G_COMP_ALWAYS );
		pScaleLine->AddChild( m_pLine[2] );
		m_pLine[2]->Release();
	}
	
	return TRUE;
}


void CGizmoScale::Update( void )
{
	INT32	i;
	i3Node * pNode;
	i3Transform * pTrans;
	MATRIX matInv;
	VEC3D vScale;

	for( i = 0; i < getSceneObject()->GetChildCount(); i++ )
	{
		pNode = getSceneObject()->GetChild( i );
		if( pNode->IsExactTypeOf( i3Transform::GetClassMeta() ) )
		{
			pTrans = (i3Transform*) pNode;
			i3Matrix::Inverse( &matInv, 0, pTrans->GetMatrix() );
			i3Matrix::GetScale( &matInv, &vScale );
			i3Matrix::SetScale( pTrans->GetMatrix(), getX( &vScale ), getY( &vScale ), getZ( &vScale ) );
		}
	}
}

bool	CGizmoScale::OnCollision(  VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx)
{
	/*
	INT32 i;
	MATRIX full, temp, inv;
	VEC3D vStart, vEnd;
	COLORREAL col;
	bool bResult = false;
	static VEC3D s_vecAxisMask[3] =
	{
		{	1.0f,	0.0f,	1.0f	},
		{	1.0f,	1.0f,	0.0f	},
		{	0.0f,	1.0f,	1.0f	},
	};

	for( i = 0; i < 3; i++ )
	{			
		{	
			i3Matrix::Mul( &full, m_pTrans[i]->GetMatrix(), getSceneObject()->GetMatrix() );
			i3Matrix::Mul( &temp, &full, pViewProjMtx);
			i3Matrix::Inverse( &inv, NULL, &temp);

   			i3Vector::TransformCoord( &vStart, pVStart, &inv);
			i3Vector::TransformCoord( &vEnd, pVEnd, &inv);
		}

		i3BoundBox * pBoundBox = m_pCap[i]->GetBound();

		if( pBoundBox == NULL )
			continue;

		if( ContainAABBoxLine( &vStart, &vEnd, pBoundBox->GetMin(), pBoundBox->GetMax(), 100000.0f) )
		{
			i3Color::Set( &col, 1.f, 1.f, 0.f, 0.5f );
			ChangeColor( m_pCap[i], &col );
			
			pFramework->SetAxisMask( &s_vecAxisMask[i]);

			bResult = true;
		}
		else
		{
			i3Color::Set( &col, 0.f, 0.f, 0.f, 0.0f );
			ChangeColor( m_pCap[i], &col );
		}
	}

	if( bResult)
		return bResult;
*/
	return CGizmo::OnCollision( pVStart, pVEnd, pViewProjMtx);
}