#include "stdafx.h"
#include "i3LevelAxisScale.h"
#include "i3LevelViewport.h"

#define SCALEGIZMO_SIZE		0.35f
#define SCALEGIZMO_SIZE2	0.32f

const COLORREAL		s_GripColor		= COLORREAL( 0.8f, 0.56f, 0.8f, 1.0f);

I3_CLASS_INSTANCE( i3LevelAxisScale);

BOOL i3LevelAxisScale::Create( void )
{
	CreateAxis( 6, 0.6f, 0.07f);
	
	//	Move Axis
	COLORREAL col;
	VEC3D Vec1, Vec2, Vec3;
	i3AttrSet * pScale, * pScaleLine;
	i3BoundBox * pBoundBox;

	i3LightingEnableAttr * pLightEnableAttr = i3LightingEnableAttr::new_object_ref();
	pLightEnableAttr->Set( FALSE);

	pScale = i3AttrSet::new_object_ref();			// 곧 지워질 변수에 대해서는 new_object_ref가 적합하다..(2012.09.11.수빈)
	pScale->SetName( "Scale" );
	pScale->AddAttr(pLightEnableAttr);
	GetSceneObject()->AddChild( pScale );

	pScaleLine = i3AttrSet::new_object_ref();		// 곧 지워질 변수에 대해서는 new_object_ref가 적합하다..(2012.09.11.수빈)
	pScaleLine->SetName( "ScaleLine" );
	pScaleLine->AddAttr(pLightEnableAttr);
	GetSceneObject()->AddChild( pScaleLine );
	
	// X 축
	{	// Triangle
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );
		{
			m_pTrans[0] = i3Transform::new_object_ref();
			
			{
				pBoundBox = i3BoundBox::new_object_ref();

				i3Vector::Set( &Vec1, 0.f, 0.f, 0.f );
				i3Vector::Set( &Vec2, SCALEGIZMO_SIZE, 0.f, 0.f );
				i3Vector::Set( &Vec3, 0.f, 0.f, -SCALEGIZMO_SIZE );
				m_pCap[0] = i3SceneUtil::BuildTriangle( &Vec1, &Vec2, &Vec3, TRUE, TRUE, &col, &col, &col );
				m_pCap[0]->SetName( "ScaleXZ" );
				SetZFunc( m_pCap[0], I3G_COMP_ALWAYS );
				SetAlphaFunc( m_pCap[0], I3G_COMP_GREATER );
				SetBlendMode( m_pCap[0], I3G_BLEND_SRCALPHA, I3G_BLEND_INVSRCALPHA );
				m_pCap[0]->GetWrappingBound( pBoundBox );
				m_pCap[0]->SetBound( pBoundBox );
				m_pTrans[0]->AddChild( m_pCap[0] );
				I3_MUST_RELEASE(m_pCap[0]);
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
		I3_MUST_RELEASE(m_pLine[0]);
	}

	// Y 축
	{	// Triangle
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );

		{
			m_pTrans[1] = i3Transform::new_object_ref();
			
			{
				pBoundBox = i3BoundBox::new_object_ref();

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
				I3_MUST_RELEASE(m_pCap[1]);
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

		I3_MUST_RELEASE(m_pLine[1]);
	}
	
	// Z 축
	{	// Triangle
		i3Color::Set( &col, 1.f, 1.f, 0.f, 0.f );
		{
			m_pTrans[2] = i3Transform::new_object_ref();
			
			{
				pBoundBox = i3BoundBox::new_object_ref();

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
				I3_MUST_RELEASE(m_pCap[2]);
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
		I3_MUST_RELEASE(m_pLine[2]);
	}

	// Grip들 생성
	{
		i3ShaderAttr *	pShaderAttr;

		{
			pShaderAttr = i3ShaderAttr::new_object_ref();
			pShaderAttr->setAddCacheState( false);
			pShaderAttr->setTargetVertexShaderVersion( I3G_SHADER_VERSION_2);
			pShaderAttr->setTargetPixelShaderVersion( I3G_SHADER_VERSION_2);
			pShaderAttr->setSource( "i3Gizmo.hlsl");
		}

		REAL32	s_Mask[8][3] =
		{
			{	-1.0f,	-0.5f,	-1.0f },
			{	-1.0f,	-0.5f,	 1.0f },
			{	 1.0f,	-0.5f,	-1.0f },
			{	 1.0f,	-0.5f,	 1.0f },

			{	-1.0f,	 0.5f,	-1.0f },
			{	-1.0f,	 0.5f,	 1.0f },
			{	 1.0f,	 0.5f,	-1.0f },
			{	 1.0f,	 0.5f,	 1.0f },
		};

		for( INT32 i = 0; i < 6; i++)
		{
			
			m_pGrip_Color[i] = i3ColorAttr::new_object_ref();
			m_pGrip_Color[i]->Set( (COLORREAL *) &s_GripColor);

			m_pGrip_AttrSet[i] = i3AttrSet::new_object_ref();
			m_pGrip_AttrSet[i]->AddAttr( m_pGrip_Color[i]);
			m_pGrip_AttrSet[i]->AddAttr( pShaderAttr);

			{
				i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

				pAttr->Set( FALSE);
				m_pGrip_AttrSet[i]->AddAttr( pAttr);
			}

			{
				i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();

				pAttr->Set( FALSE);
				m_pGrip_AttrSet[i]->AddAttr( pAttr);
			}

			// Transform
			{
				char conv[256];

				sprintf_s( conv, _countof( conv), "Grip Box(%d)", i);

				m_pGrip_Trans[i] = i3Transform::new_object_ref();
				m_pGrip_Trans[i]->AddChild( m_pGrip_AttrSet[i]);
				m_pGrip_Trans[i]->AddFlag( I3_NODEFLAG_MANUAL_UPDATE);			// 직접 Cache Matrix를 설정하는 방식.
				m_pGrip_Trans[i]->SetName( conv);

				i3Matrix::SetTranslate( m_pGrip_Trans[i]->GetMatrix(), s_Mask[i][0], s_Mask[i][1], s_Mask[i][2]);
				i3Matrix::Identity(m_pGrip_Trans[i]->GetCacheMatrix());
			}
			
			GetSceneObject()->AddChild( m_pGrip_Trans[i]);

			// Box Geometry
			{
				const REAL32 RADIUS = 0.07f;

				i3Node * pSphereNode = i3SceneUtil::BuildSphereMesh( RADIUS, TRUE, FALSE, 10, 20);

				m_pGrip_AttrSet[i]->AddChild( pSphereNode);

				I3_MUST_RELEASE( pSphereNode);
			}

			// Bounds
			{
				i3BoundBox * pBound = i3BoundBox::new_object_ref();

				m_pGrip_AttrSet[i]->GetWrappingBound( pBound);

				m_pGrip_Trans[i]->SetBound( pBound);
			}
		}
	}
	
	return TRUE;
}

void i3LevelAxisScale::OnUpdate( REAL32 tm)
{
}

void i3LevelAxisScale::OnUpdateTransform(void)
{
	MATRIX * pBaseMtx = GetSceneObject()->GetMatrix();
	MATRIX * pMtx;
	VEC3D pos[6];
	i3LevelElement3D * pObj = getTargetObject();

	if( pObj == nullptr)
	{
		// Target Object가 없다면, Grip을 제거한다.
		for( INT32 i = 0; i < 6; i++)
		{
			m_pGrip_Trans[i]->AddFlag( I3_NODEFLAG_DISABLE);
		}

		return;
	}
	/*
	{
		VEC3D vmin, vmax, mid, size;

		vmin = m_BoundMin;
		vmax = m_BoundMax;

		i3Vector::Add( &mid, &vmin, &vmax);
		i3Vector::Scale( &mid, &mid, 0.5f);			// 중앙점

		i3Vector::Sub( &size, &vmax, &vmin);
		i3Vector::Scale( &size, &size, 0.5f);		// 반 크기

		pos[0] = VEC3D( mid.x - size.x,	mid.y,			mid.z	);		// X 방향
		pos[1] = VEC3D( mid.x + size.x,	mid.y,			mid.z	);

		pos[2] = VEC3D( mid.x,			mid.y - size.y,	mid.z	);		// Y 방향
		pos[3] = VEC3D( mid.x,			mid.y + size.y,	mid.z	);

		pos[4] = VEC3D( mid.x,			mid.y,			mid.z - size.z);	// Z 방향
		pos[5] = VEC3D( mid.x,			mid.y,			mid.z + size.z);
	}
	*/

	{
		VEC3D sz, mid;
		MATRIX inv;

		i3Vector::Add( &mid, &m_BoundMin, &m_BoundMax);
		i3Vector::Scale( &mid, &mid, 0.5f);

		i3Matrix::Inverse( &inv, nullptr, pObj->GetTransform());
		i3Vector::TransformCoord( &mid, &mid, &inv);

		i3Vector::Sub( &sz, pObj->GetBoundMax(), pObj->GetBoundMin());
		i3Vector::Scale( &sz, &sz, 0.5f);

		pos[0] = VEC3D( mid.x-sz.x,	mid.y,	mid.z);
		pos[1] = VEC3D( mid.x+sz.x,	mid.y,	mid.z);

		pos[2] = VEC3D( mid.x,	mid.y-sz.y,	mid.z);
		pos[3] = VEC3D( mid.x,	mid.y+sz.y,	mid.z);

		pos[4] = VEC3D( mid.x,	mid.y,	mid.z-sz.z);
		pos[5] = VEC3D( mid.x,	mid.y,	mid.z+sz.z);

		for( INT32 i = 0; i < 6; i++)
		{
			I3ASSERT( i3Vector::isValid( &pos[i]));
		}

		i3Matrix::TransformPoints( pos, pos, 6, pObj->GetTransform());
	}

	for( INT32 i = 0; i < 6; i++)
	{
		m_pGrip_Trans[i]->RemoveFlag( I3_NODEFLAG_DISABLE);

		pMtx = m_pGrip_Trans[i]->GetCacheMatrix();

		*pMtx = *pBaseMtx;

		i3Matrix::SetPos( pMtx, &pos[i]);
	}
}

void i3LevelAxisScale::Update( void )
{
	INT32	i;
	i3Node * pNode;
	i3Transform * pTrans;
	MATRIX matInv;
	VEC3D vScale;

	for( i = 0; i < GetSceneObject()->GetChildCount(); i++ )
	{
		pNode = GetSceneObject()->GetChild( i );
		if( i3::same_of<i3Transform*>(pNode) )
		{
			pTrans = (i3Transform*) pNode;
			i3Matrix::Inverse( &matInv, 0, pTrans->GetMatrix() );
			i3Matrix::GetScale( &matInv, &vScale );
			i3Matrix::SetScale( pTrans->GetMatrix(), getX( &vScale ), getY( &vScale ), getZ( &vScale ) );
		}
	}
}

bool	i3LevelAxisScale::OnCollision(  i3LevelFramework * pFramework, I3_LEVEL_EDIT_CONTEXT * pCtx, VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx)
{
	INT32 i;
	MATRIX temp, inv, WVP;
	VEC3D vStart, vEnd;
	COLORREAL col;
	bool bResult = false;
	static VEC3D s_vecAxisMask[3] =
	{
		VEC3D(	1.0f,	0.0f,	1.0f	),
		VEC3D(	1.0f,	1.0f,	0.0f	),
		VEC3D(	0.0f,	1.0f,	1.0f	),
	};

	i3Matrix::Mul( &WVP, GetSceneObject()->GetMatrix(), pViewProjMtx);

	for( i = 0; i < 3; i++ )
	{			
		{	
			i3Matrix::Mul( &temp, m_pTrans[i]->GetMatrix(), &WVP);
			i3Matrix::Inverse( &inv, nullptr, &temp);

   			i3Vector::TransformCoord( &vStart, pVStart, &inv);
			i3Vector::TransformCoord( &vEnd, pVEnd, &inv);
		}

		i3BoundBox * pBoundBox = m_pCap[i]->GetBound();

		if( pBoundBox == nullptr )
			continue;

		if( ContainAABBoxLine( &vStart, &vEnd, pBoundBox->GetMin(), pBoundBox->GetMax(), 100000.0f) )
		{
			i3Color::Set( &col, 1.f, 1.f, 0.f, 0.5f );
			ChangeColor( m_pCap[i], &col );
			
			pFramework->SetAxisMask( &s_vecAxisMask[i]);			
		
			pCtx->m_idxGrip = -1;

			bResult = true;
		}
		else
		{
			i3Color::Set( &col, 0.f, 0.f, 0.f, 0.0f );
			ChangeColor( m_pCap[i], &col );
		}
	}

	if( bResult == false)
	{
		for( i = 0; i < 6; i++)
		{
			{	// Grip은 World-Space Transform을 Cache Matrix에 직접 설정해 사용한다.
				i3Matrix::Mul( &temp, m_pGrip_Trans[i]->GetCacheMatrix(), pViewProjMtx);
				i3Matrix::Inverse( &inv, nullptr, &temp);

   				i3Vector::TransformCoord( &vStart, pVStart, &inv);
				i3Vector::TransformCoord( &vEnd, pVEnd, &inv);
			}

			i3BoundBox * pBoundBox = m_pGrip_Trans[i]->GetBound();

			if( pBoundBox == nullptr )
				continue;

			if( ContainAABBoxLine( &vStart, &vEnd, pBoundBox->GetMin(), pBoundBox->GetMax(), 100000.0f) )
			{
				i3Color::Set( &col, 1.2f, 1.2f, 0.f, 1.5f );
				ChangeColor( m_pGrip_Trans[i], &col );
				
			//	pFramework->SetAxisMask( &s_vecAxisMask[i]);			// 0,1이 엑스, 1,2가 와이, 3,4가 제트로 보여지므로..수정함..
				pFramework->SetAxisMask(&s_vecAxisMask[i / 2]);		// 커버리티 이슈.2016.07.11. 수빈!
				
				pCtx->m_idxGrip = i;
				bResult = true;
				break;
			}
			else
			{
				ChangeColor( m_pGrip_Trans[i], (COLORREAL *) &s_GripColor );
			}
		}
	}

	if( bResult)
		return bResult;

	pCtx->m_idxGrip = -1;

	return i3LevelAxis::OnCollision( pFramework, pCtx, pVStart, pVEnd, pViewProjMtx);
}