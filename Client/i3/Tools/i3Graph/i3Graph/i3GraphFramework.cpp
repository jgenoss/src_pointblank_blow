#include "stdafx.h"
#include "i3GraphFramework.h"

I3_CLASS_INSTANCE( i3GraphFramework, i3ViewerFramework);

i3GraphFramework::i3GraphFramework(void)
{
	m_pSelBoundRoot = NULL;
	m_pReflectMapAttr = NULL;
	m_pBSPPlane = NULL;
}

i3GraphFramework::~i3GraphFramework(void)
{
	I3_SAFE_NODE_RELEASE( m_pSelBoundRoot);
	I3_SAFE_NODE_RELEASE( m_pBSPPlane);
	I3_SAFE_RELEASE( m_pReflectMapAttr);
}

void i3GraphFramework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();

	//
	{
		m_pSelBoundRoot = i3Transform::NewObject();
		m_pSelBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);

		Get3DRoot()->AddChild( m_pSelBoundRoot);

		i3AttrSet * pAttrSet = i3AttrSet::NewObjectRef();
		m_pSelBoundRoot->AddChild( pAttrSet);

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();
			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::NewObjectRef();
			COLORREAL col;

			i3Color::Set( &col, 1.0f, 0.8f, 0.2f, 1.0f);

			pAttr->Set( &col);
			pAttrSet->AddAttr( pAttr);
		}

		i3Geometry * pGeo = i3Geometry::NewObjectRef();
		pAttrSet->AddChild( pGeo);

		i3GeometryAttr * pGeoAttr = i3GeometryAttr::NewObjectRef();
		pGeo->AppendGeometryAttr( pGeoAttr);

		{
			i3VertexArray * pVA;
			i3VertexFormat fmt;

			pVA = i3VertexArray::NewObjectRef();
			pVA->Create( &fmt, 24, 0);

			pVA->Lock();

			VEC3D pos[24];

			i3Vector::Set( &pos[ 0], -0.5f, -0.5f, -0.5f);		// 아래쪽 사각형
			i3Vector::Set( &pos[ 1],  0.5f, -0.5f, -0.5f);
			i3Vector::Set( &pos[ 2],  0.5f, -0.5f, -0.5f);
			i3Vector::Set( &pos[ 3],  0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[ 4],  0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[ 5], -0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[ 6], -0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[ 7], -0.5f, -0.5f, -0.5f);

			i3Vector::Set( &pos[ 8], -0.5f,  0.5f, -0.5f);		// 위쪽 사각형
			i3Vector::Set( &pos[ 9],  0.5f,  0.5f, -0.5f);
			i3Vector::Set( &pos[10],  0.5f,  0.5f, -0.5f);
			i3Vector::Set( &pos[11],  0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[12],  0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[13], -0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[14], -0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[15], -0.5f,  0.5f, -0.5f);

			i3Vector::Set( &pos[16], -0.5f, -0.5f, -0.5f);		// 세로 선
			i3Vector::Set( &pos[17], -0.5f,  0.5f, -0.5f);
			i3Vector::Set( &pos[18],  0.5f, -0.5f, -0.5f);
			i3Vector::Set( &pos[19],  0.5f,  0.5f, -0.5f);
			i3Vector::Set( &pos[20],  0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[21],  0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[22], -0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[23], -0.5f, -0.5f,  0.5f);

			pVA->SetPositions( 0, 24, pos);
			pVA->Unlock();
			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 12);
		}
	}

	// Default ReflectMap
	{
		i3TextureCube * pTex;
		i3ImageFile file;
		char szPath[ MAX_PATH], szFull[ MAX_PATH];

		i3System::ResolvePath( "$(I3_ROOT)/Bin/res", szPath, sizeof(szPath));

		sprintf( szFull, "%s/defl_cubemap.dds", szPath);

		if( i3System::IsFile( szFull) == FALSE)
		{
			i3System::GetExecPath( szPath);

			sprintf( szFull, "%s/res/defl_cubemap.dds", szPath);
		}

		pTex = (i3TextureCube *) file.Load( szFull);

		if( pTex != NULL)
		{
			m_pReflectMapAttr = i3ReflectMapBindAttr::NewObject();

			m_pReflectMapAttr->SetTexture( pTex);

			pTex->Release();

			m_pAttrSet->AddAttr( m_pReflectMapAttr);
		}
	}

	{
		m_pBSPPlane = i3Transform::NewObject();
		m_pBSPPlane->AddFlag( I3_NODEFLAG_DISABLE);

		Get3DRoot()->AddChild( m_pBSPPlane);

		// Plane
		{
			i3AttrSet * pAttrSet = i3AttrSet::NewObjectRef();
			m_pBSPPlane->AddChild( pAttrSet);

			{
				i3BlendEnableAttr * pAttr = i3BlendEnableAttr::NewObjectRef();
				pAttr->Set( TRUE);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3BlendModeAttr * pAttr = i3BlendModeAttr::NewObjectRef();
				pAttr->SetSource( I3G_BLEND_SRCALPHA);
				pAttr->SetDestination( I3G_BLEND_ONE);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();
				pAttr->Set( FALSE);
				pAttrSet->AddAttr( pAttr);
			}

			i3Geometry * pGeo = i3Geometry::NewObjectRef();
			pAttrSet->AddChild( pGeo);

			i3GeometryAttr * pGeoAttr = i3GeometryAttr::NewObjectRef();
			pGeo->AppendGeometryAttr( pGeoAttr);

			{
				i3VertexFormat fmt;
				i3VertexArray * pVA = i3VertexArray::NewObjectRef();

				fmt.SetHasPosition();
				fmt.SetHasColor( true);

				pVA->Create( &fmt, 6, 0);
				pVA->Lock();

				{
					VEC3D pos[6];
					
					i3Vector::Set( &pos[0], -0.5f, 0.5f, 0.0f);
					i3Vector::Set( &pos[1], -0.5f, -0.5f, 0.0f);
					i3Vector::Set( &pos[2],  0.5f, 0.5f, 0.0f);
					i3Vector::Set( &pos[3],  0.5f, 0.5f, 0.0f);
					i3Vector::Set( &pos[4], -0.5f, -0.5f, 0.0f);
					i3Vector::Set( &pos[5],  0.5f, -0.5f, 0.0f);

					pVA->SetPositions( 0, 6, pos);
				}

				{
					COLOR col;

					i3Color::Set( &col, (UINT8) 255, 128, 64, 128);
					pVA->SetColor( 0, 6, &col);
				}

				pVA->Unlock();

				pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);
			}
		}

	}
}

void i3GraphFramework::_GetFullTransform( i3Node * pNode, MATRIX * pMtx)
{
	i3List parentList;

	pNode = pNode->GetParent();

	while((pNode != NULL))
	{
		parentList.Add( pNode);
		pNode = pNode->GetParent();
	}

	INT32 i;
	i3Body * pBody = NULL;

	i3Matrix::Identity( pMtx);

	for( i = 0; i < parentList.GetCount(); i++)
	{
		pNode = (i3Node *) parentList.Items[i];

		if( pNode->IsTypeOf( i3Transform::GetClassMeta()))
		{
			i3Transform * pTrans = (i3Transform *) pNode;

			i3Matrix::Mul( pMtx, pMtx, pTrans->GetMatrix());
		}
		else if( pNode->IsTypeOf( i3Body::GetClassMeta()))
		{
		}
	}
}

void i3GraphFramework::SelectNode( i3Node * pNode)
{
	//i3Transform2::GetClassMeta()->IsTypeOf( i3RenderAttr::GetClassMeta());

	if((pNode != NULL) && (pNode->GetBound() != NULL))
	{
		m_pSelBoundRoot->RemoveFlag( I3_NODEFLAG_DISABLE);
		
		i3BoundBox * pBox = pNode->GetBound();
		VEC3D diff;
		MATRIX mtx;

		_GetFullTransform( pNode, &mtx);

		i3Vector::Sub( &diff, pBox->GetMax(), pBox->GetMin());

		i3Matrix::SetScale( m_pSelBoundRoot->GetMatrix(), i3Math::abs( diff.x), i3Math::abs( diff.y), i3Math::abs( diff.z));

		pBox->GetCenter( &diff);
		i3Matrix::PostTranslate( m_pSelBoundRoot->GetMatrix(), &diff);

		i3Matrix::Mul( m_pSelBoundRoot->GetMatrix(), m_pSelBoundRoot->GetMatrix(), &mtx);
	}
	else
	{
		m_pSelBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);
	}

	if( pNode->IsExactTypeOf( i3BinaryPartitionNode::GetClassMeta()))
	{
		i3BinaryPartitionNode * pBSPNode = (i3BinaryPartitionNode *) pNode;

		MATRIX mtx;
		i3BoundBox box;

		pBSPNode->GetPlaceMatrix( &mtx);

		pBSPNode->GetWrappingBound( &box);
		VEC3D diff;

		i3Vector::Sub( &diff, box.GetMax(), box.GetMin());

		i3Matrix::PreScale( &mtx, diff.x, diff.y, diff.z);

		VEC3D center;
		mtx, box.GetCenter( &center);
		i3Matrix::PostTranslate( &mtx, &center);

		m_pBSPPlane->SetMatrix( &mtx);

		m_pBSPPlane->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
	else
	{
		m_pBSPPlane->AddFlag( I3_NODEFLAG_DISABLE);
	}

	i3Transform2::GetClassMeta()->IsTypeOf( i3RenderAttr::GetClassMeta());
}

void i3GraphFramework::SetLight( COLORREAL * pDiff, COLORREAL * pSpec, INT32 shadowSize)
{
	i3Light * pLight = m_pLightAttr->GetLightObject();

	pLight->SetDiffuse( pDiff);
	pLight->SetSpecular( pSpec);
	pLight->setShadowMapSize( shadowSize);

	if( pLight->getStyle() & I3G_LIGHT_STYLE_SHADOWMAP )
	{
		pLight->CreateShadowMap();
	}
}
