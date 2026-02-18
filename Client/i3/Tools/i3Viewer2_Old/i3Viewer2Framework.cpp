#include "stdafx.h"
#include "i3Viewer2Framework.h"
#include "GlobalVar.h"

I3_CLASS_INSTANCE( i3Viewer2Framework, i3ViewerFramework);

i3Viewer2Framework::i3Viewer2Framework(void)
{
	m_EditMode = I3_VIEWER_EDIT_MODE_NORMAL;
	m_pSelBoundRoot = NULL;
	m_pReflectMapAttr = NULL;
	m_pOctreeBoundRoot = NULL;
	m_pBSPPlane = NULL;
	m_pSgInfo = NULL;

	m_pNormalViewRoot = NULL;
	m_pMeshViewRoot = NULL;
	m_pCollViewRoot = NULL;
	m_pWireViewRoot = NULL;

	m_pInterLinkNode = NULL;

	m_pSelNode = NULL;
}

i3Viewer2Framework::~i3Viewer2Framework(void)
{
	I3_SAFE_NODE_RELEASE( m_pSelBoundRoot);
	I3_SAFE_NODE_RELEASE( m_pOctreeBoundRoot);
	I3_SAFE_NODE_RELEASE( m_pBSPPlane);
	I3_SAFE_NODE_RELEASE( m_pInterLinkNode);
	I3_SAFE_RELEASE( m_pReflectMapAttr);

	I3_SAFE_RELEASE( m_pSgInfo);

	I3_SAFE_RELEASE( m_pNormalViewRoot);
	I3_SAFE_RELEASE( m_pMeshViewRoot);
	I3_SAFE_RELEASE( m_pCollViewRoot);
	I3_SAFE_RELEASE( m_pWireViewRoot);

}

void i3Viewer2Framework::OnCreate(void)
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

	{
		_CreateOctreeBound();
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

					i3Color::Set( &col, (UINT8)255, (UINT8)128, (UINT8)64, (UINT8)128);
					pVA->SetColor( 0, 6, &col);
				}

				pVA->Unlock();

				pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);
			}
		}
	}

	{
		m_pMeshViewRoot = i3AttrSet::NewObject();
		m_pCollViewRoot = i3AttrSet::NewObject();
		m_pWireViewRoot = i3AttrSet::NewObject();

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();

			pAttr->Set( FALSE);

			m_pMeshViewRoot->AddAttr( pAttr);
			m_pCollViewRoot->AddAttr( pAttr);
			m_pWireViewRoot->AddAttr( pAttr);
		}

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::NewObjectRef();

			pAttr->Set( FALSE);

			m_pMeshViewRoot->AddAttr( pAttr);
			m_pCollViewRoot->AddAttr( pAttr);
			m_pWireViewRoot->AddAttr( pAttr);
		}
	}

	{
		m_pInterLinkNode = i3Node::NewObject();

		m_pInterLinkNode->SetFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL);
		m_pInterLinkNode->SetName( "_InterLinkNode_");
	}

	ApplyOpition();
}

void i3Viewer2Framework::OnDestroy(void)
{
	g_pOption->m_LightTheta		= m_LightTheta;
	g_pOption->m_LightRho		= m_LightRho;
	g_pOption->m_LightDist		= m_LightDist;
}

void i3Viewer2Framework::_CreateOctreeBound(void)
{
	m_pOctreeBoundRoot = i3AttrSet::NewObject();
	m_pOctreeBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);

	Get3DRoot()->AddChild( m_pOctreeBoundRoot);

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();

		pAttr->Set( FALSE);

		m_pOctreeBoundRoot->AddAttr( pAttr);
	}

	{
		i3ColorAttr * pAttr = i3ColorAttr::NewObjectRef();
		COLORREAL col;

		i3Color::Set( &col, 1.0f, 0.0f, 0.0f, 1.0f);

		pAttr->Set( &col);

		m_pOctreeBoundRoot->AddAttr( pAttr);
	}

	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::NewObjectRef();
		pAttr->Set( FALSE);
		m_pOctreeBoundRoot->AddAttr( pAttr);
	}

	i3Geometry * pGeo = i3Geometry::NewObjectRef();
	m_pOctreeBoundRoot->AddChild( pGeo);

	i3GeometryAttr * pGeoAttr = i3GeometryAttr::NewObjectRef();
	pGeo->AppendGeometryAttr( pGeoAttr);

	i3VertexFormat fmt;

	fmt.SetHasPosition( true);
	//fmt.SetHasColor( true);

	m_pOctreeBoundLineVA = i3VertexArray::NewObjectRef();
	m_pOctreeBoundLineVA->Create( &fmt, 14, 0);

	pGeoAttr->SetVertexArray( m_pOctreeBoundLineVA, I3G_PRIM_LINELIST, 15);

	i3IndexArray * pIA = i3IndexArray::NewObjectRef();
	pIA->Create( 30, 0);
	pIA->Lock();

	// Upper X-Z rectangle ( 0, 1, 2, 3)
	// Lower X-Z ractangle ( 4, 5, 6, 7)
	// Center of Left/Right ( 8, 9)
	// Center of Front/Back ( 10, 11)
	// Center of Top/Bottom ( 12, 13)

	// Upper rectangle
	pIA->SetIndex( 0,	0);
	pIA->SetIndex( 1,	1);	
	pIA->SetIndex( 2,	1);
	pIA->SetIndex( 3,	2);
	pIA->SetIndex( 4,	2);
	pIA->SetIndex( 5,	3);
	pIA->SetIndex( 6,	3);
	pIA->SetIndex( 7,	0);

	// Lower rectnagle
	pIA->SetIndex( 8,		4);
	pIA->SetIndex( 9,		5);	
	pIA->SetIndex( 10,	5);
	pIA->SetIndex( 11,	6);
	pIA->SetIndex( 12,	6);
	pIA->SetIndex( 13,	7);
	pIA->SetIndex( 14,	7);
	pIA->SetIndex( 15,	4);

	// Vertical Line
	pIA->SetIndex( 16,	0);
	pIA->SetIndex( 17,	4);	
	pIA->SetIndex( 18,	1);
	pIA->SetIndex( 19,	5);
	pIA->SetIndex( 20,	2);
	pIA->SetIndex( 21,	6);
	pIA->SetIndex( 22,	3);
	pIA->SetIndex( 23,	7);

	// Cross lines
	pIA->SetIndex( 24,	8);
	pIA->SetIndex( 25,	9);	
	pIA->SetIndex( 26,	10);
	pIA->SetIndex( 27,	11);
	pIA->SetIndex( 28,	12);
	pIA->SetIndex( 29,	13);

	pIA->Unlock();

	pGeoAttr->SetIndexArray( pIA);
}

void i3Viewer2Framework::_SelectOctree( i3OcTreeNode * pNode)
{
	if( pNode == NULL)
		m_pOctreeBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);
	else
	{
		m_pOctreeBoundRoot->RemoveFlag( I3_NODEFLAG_DISABLE);

		VEC3D pos[14], * pMin, *pMax, *pCenter;
		VEC3D vmin, vmax, dir;

		i3Vector::Copy( &vmin, pNode->getMin());
		i3Vector::Copy( &vmax, pNode->getMax());

		i3Vector::Sub( &dir, &vmax, &vmin);
		i3Vector::Scale( &dir, &dir, 0.5f * (pNode->getK() - 1.0f));

		i3Vector::Sub( &vmin, &vmin, &dir);
		i3Vector::Add( &vmax, &vmax, &dir);

		pMin = &vmin;
		pMax = &vmax;
		pCenter = pNode->getCenter();

		// Upper X-Z rectangle ( 0, 1, 2, 3)
		i3Vector::Set( &pos[0], pMin->x, pMax->y, pMin->z);
		i3Vector::Set( &pos[1], pMin->x, pMax->y, pMax->z);
		i3Vector::Set( &pos[2], pMax->x, pMax->y, pMax->z);
		i3Vector::Set( &pos[3], pMax->x, pMax->y, pMin->z);

		// Lower X-Z ractangle ( 4, 5, 6, 7)
		i3Vector::Set( &pos[4], pMin->x, pMin->y, pMin->z);
		i3Vector::Set( &pos[5], pMin->x, pMin->y, pMax->z);
		i3Vector::Set( &pos[6], pMax->x, pMin->y, pMax->z);
		i3Vector::Set( &pos[7], pMax->x, pMin->y, pMin->z);

		// Center of Left/Right ( 8, 9)
		i3Vector::Set( &pos[8], pMin->x, pCenter->y, pCenter->z);
		i3Vector::Set( &pos[9], pMax->x, pCenter->y, pCenter->z);

		// Center of Front/Back ( 10, 11)
		i3Vector::Set( &pos[10], pCenter->x, pCenter->y, pMin->z);
		i3Vector::Set( &pos[11], pCenter->x, pCenter->y, pMax->z);

		// Center of Top/Bottom ( 12, 13)
		i3Vector::Set( &pos[12], pCenter->x, pMin->y, pCenter->z);
		i3Vector::Set( &pos[13], pCenter->x, pMax->y, pCenter->z);

		m_pOctreeBoundLineVA->Lock();

		m_pOctreeBoundLineVA->SetPositions( 0, 14, pos);

		m_pOctreeBoundLineVA->Unlock();
	}
}

void i3Viewer2Framework::_GetFullTransform( i3Node * pNode, MATRIX * pMtx)
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

void i3Viewer2Framework::SelectNode( i3Node * pNode)
{
	//i3Transform2::GetClassMeta()->IsTypeOf( i3RenderAttr::GetClassMeta());

	m_pSelNode = pNode;

	if( pNode != NULL)
	{
		if( pNode->IsTypeOf( i3OcTreeNode::GetClassMeta()))
		{
			_SelectOctree( (i3OcTreeNode *) pNode);
		}
	}

	if( g_pOption->m_ViewerMode == VIEWER_MODE_SELONLY)
	{
		_SetIsolateMode( TRUE);
	}

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

void i3Viewer2Framework::SetLight( COLORREAL * pDiff, COLORREAL * pSpec, INT32 shadowSize)
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

bool i3Viewer2Framework::CloseI3S(void)
{
	I3_SAFE_RELEASE( m_pSgInfo);
	m_pSelNode = NULL;
	I3_SAFE_RELEASE( m_pNormalViewRoot);

	m_pMeshViewRoot->RemoveAllChild();
	m_pCollViewRoot->RemoveAllChild();
	m_pWireViewRoot->RemoveAllChild();

	setViewMode( VIEWER_MODE_NORMAL, true);

	removeAllInitBone();

	return true;
}

void i3Viewer2Framework::RefineSg( bool bResetView)
{
	i3Node * pRoot = NULL;

	m_pSelNode = NULL;

	m_pInterLinkNode->RemoveFromParent();
	m_pInterLinkNode->RemoveAllChild();

	// CommonSg가 있는 경우, CommonSg.를 상위로, InstanceSg를 하위로 붙여주어야 올바른 Visual이 나온다.
	if( m_pSgInfo != NULL)
	{
		if( m_pSgInfo->getCommonSg() != NULL)
		{
			if( m_pSgInfo->getInstanceSg() != NULL)
			{
				i3Node * pAttachNode = m_pSgInfo->getCommonSg();

				while( pAttachNode->GetChildCount() > 0)
				{
					pAttachNode = pAttachNode->GetChild( pAttachNode->GetChildCount() - 1);
				}

				// CommonSg와 InstanceSg를 임시로 연결하기 위해 m_pInterLinkNode를 사용한다.
				{
					pAttachNode->AddChild( m_pInterLinkNode);

					m_pInterLinkNode->AddChild( m_pSgInfo->getInstanceSg());
				}
			}

			pRoot = m_pSgInfo->getCommonSg();
		}
		else
		{
			pRoot = m_pSgInfo->getInstanceSg();
		}
	}

	if( pRoot != NULL)
	{
		pRoot->Instantiate();
	}

	// Loading된 Sg.의 Physics 상태를 Kinematic으로 설정.
	SetDynamicState( false);

	I3_REF_CHANGE( m_pNormalViewRoot, pRoot);

	// Bone들의 초기값을 저장해둔다.
	BuildInitBone();

	setViewMode( VIEWER_MODE_NORMAL, bResetView);
}

bool i3Viewer2Framework::LoadI3S( const char * pszPath)
{
	i3SceneFile file;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		return false;
	}

	i3SceneGraphInfo * pSgInfo = file.getSceneGraphInfo();

	I3_REF_CHANGE( m_pSgInfo, pSgInfo);

	RefineSg( true);

	SetGlowEnable( g_pOption->m_bGlow);

	return true;
}

bool i3Viewer2Framework::SaveI3S( const char * pszPath)
{
	i3SceneFile file;

	if( m_pSgInfo == NULL)
		return false;

	file.setSceneGraphInfo( m_pSgInfo);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		return false;
	}

	return true;
}

bool	i3Viewer2Framework::LoadAnimation( const char * pszPath)
{
	i3Animation * pAnim = i3Animation::LoadFromFile( pszPath);

	if( pAnim == NULL)
		return false;

	if( m_pSgInfo == NULL)
		return false;

	if( m_pSgInfo->getInstanceSg() == NULL)
		return false;

	i3Body * pBody = (i3Body*) i3Scene::FindNodeByExactType( m_pSgInfo->getInstanceSg(), i3Body::GetClassMeta());

	if( pBody == NULL)
		return false;

	pAnim->AddRef();
	pBody->PlayAnim( pAnim);

	I3_SAFE_RELEASE( pAnim);

	return true;
}

void i3Viewer2Framework::ApplyOpition(void)
{
	// Option에 맞추어 필요한 설정을 한다.
	{
		i3RenderContext * pCtx = GetViewer()->GetRenderContext();

		// Background Color
		{
			COLOR col;

			i3Color::Set( &col, & g_pOption->m_BackColor);

			pCtx->SetClearColor( &col);
		}

		// Ambient Color
		{
			pCtx->setAmbient( & g_pOption->m_Ambient);
		}

		// Light
		{
			m_pLightAttr->setDiffuse( & g_pOption->m_LightDiffuse);
			m_pLightAttr->setSpecular( & g_pOption->m_LightSpecular);

			// Transform
			m_LightTheta	= g_pOption->m_LightTheta;
			m_LightRho		= g_pOption->m_LightRho;
			m_LightDist		= g_pOption->m_LightDist;

			_UpdateLightTx();
		}

		// Shadow
		{
			i3Light * pLight = m_pLightAttr->GetLightObject();

			if( g_pOption->m_bShadowEnable)
			{
				// Shadow On
				pLight->addStyle( I3G_LIGHT_STYLE_SHADOWMAP);
				pLight->setShadowMapSize( g_pOption->m_ShadowSize);		// ShadowMap Size
			}
			else
			{
				// Shadow Off
				pLight->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);
			}
		}
	}

	setViewMode( g_pOption->m_ViewerMode);

	// Grid
	SetGridShow( g_pOption->m_bGridShow == TRUE);
}

void i3Viewer2Framework::setEditMode( I3_VIEWER_EDIT_MODE mode)
{
	m_EditMode = mode;

	switch( mode)
	{
		case I3_VIEWER_EDIT_MODE_NORMAL :
			break;

		case I3_VIEWER_EDIT_MODE_LIGHT :
			break;
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, g_pOption);
}

typedef struct _tagIsolateInfo
{
	i3Node *		m_pNode;
	bool			m_bStatus;
} ISOLATE_INFO;

static I3SG_TRAVERSAL_RESULT _IsolateNodeProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	ISOLATE_INFO * pInfo = (ISOLATE_INFO *) pData;

	bool bVisible = ! pInfo->m_bStatus;

	// 부모 노드에서 대상 Node가 있는지 확인.
	{
		i3Node * pParent = pNode;

		while( pParent != NULL)
		{
			if( pParent == pInfo->m_pNode)
			{
				bVisible = true;
				break;
			}

			pParent = pParent->GetParent();
		}
	}

	if( bVisible)
		pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
	else
		pNode->AddFlag( I3_NODEFLAG_DISABLE);

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Viewer2Framework::_SetIsolateMode( bool bFlag)
{
	if( m_pNormalViewRoot == NULL)
		return;

	ISOLATE_INFO info;

	info.m_pNode = m_pSelNode;
	info.m_bStatus = bFlag;

	i3Scene::Traverse( m_pNormalViewRoot, _IsolateNodeProc, &info);

	// 부모부터 상위 Root까지 모두 Enable
	i3Node * pTemp = m_pSelNode;
	
	while( pTemp != NULL)
	{
		pTemp->RemoveFlag( I3_NODEFLAG_DISABLE);

		pTemp = pTemp->GetParent();
	}
}

void i3Viewer2Framework::setViewMode( VIEWER_MODE mode, bool bReset)
{
	i3Node *	pRoot = NULL;

	g_pOption->m_ViewerMode = mode;

	switch( mode)
	{
		case VIEWER_MODE_NORMAL :
			pRoot = m_pNormalViewRoot;

			_SetIsolateMode( false);
			break;

		case VIEWER_MODE_SELONLY :
			pRoot = m_pNormalViewRoot;

			_SetIsolateMode( true);
			break;

		case VIEWER_MODE_MESH :
			if(( m_pNormalViewRoot != NULL) && (m_pMeshViewRoot->GetChildCount() == 0))
			{
				i3Node * pTemp = i3SceneUtil::BuildInstanceMesh( m_pNormalViewRoot, FALSE);

				m_pMeshViewRoot->AddChild( pTemp);
			}

			pRoot = m_pMeshViewRoot;
			break;

		case VIEWER_MODE_COLL :
			if(( m_pNormalViewRoot != NULL) && (m_pCollViewRoot->GetChildCount() == 0))
			{
				i3Node * pTemp = i3SceneUtil::BuildRendableCollider( m_pNormalViewRoot, NULL, TRUE, FALSE, true, NULL, NULL, false);

				m_pCollViewRoot->AddChild( pTemp);
			}

			pRoot = m_pCollViewRoot;
			break;

		case VIEWER_MODE_WIRE :
			if(( m_pNormalViewRoot != NULL) && (m_pWireViewRoot->GetChildCount() == 0))
			{
				//i3Node * pTemp = i3SceneUtil::BuildWireMesh( m_pNormalViewRoot);
				i3RenderModeAttr * pAttr = i3RenderModeAttr::NewObjectRef();

				pAttr->Set( I3G_RENDER_LINE);

				m_pWireViewRoot->AddAttr( pAttr);

				m_pWireViewRoot->AddChild( m_pNormalViewRoot);
			}

			pRoot = m_pWireViewRoot;
			break;
	}

	SetSceneGraph( pRoot, bReset);

	i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, g_pOption);
}

void i3Viewer2Framework::RecordCam( INT32 idx)
{
	MATRIX * pMatrix = 	m_pCurCamera->GetMatrix();

	i3Vector::Copy( &g_pOption->m_CamPos[idx], i3Matrix::GetPos( pMatrix));
	i3Matrix::GetRotateQuat( &g_pOption->m_CamRot[idx], pMatrix);

	I3TRACE( "Record %d camera\n", idx + 1);
}

void i3Viewer2Framework::SetCamPos( INT32 idx)
{
	MATRIX * pMatrix = 	m_pCurCamera->GetMatrix();

	i3Matrix::SetRotateQuat( pMatrix, & g_pOption->m_CamRot[idx]);
	i3Matrix::SetPos( pMatrix, &g_pOption->m_CamPos[idx]);
}

void i3Viewer2Framework::SetGlowEnable( BOOL bFlag)
{
	if( isEnable_DownScale() != bFlag) 
	{
		ToggleRasterizeChain_DownScale();
	}

	if( isEnable_BrightPassExtract() != bFlag) 
	{
		ToggleRasterizeChain_BrightPassExtract();
	}

	if( isEnable_GaussianBlur() != bFlag) 
	{
		ToggleRasterizeChain_GaussianBlur();
	}

	if( isEnable_BloomCombine() != bFlag) 
	{
		ToggleRasterizeChain_BloomCombine();
	}
}

INIT_BONE_INFO * i3Viewer2Framework::addInitBone( const char * pszBone, MATRIX * pMatrix)
{
	INIT_BONE_INFO * pInfo;

	pInfo = new INIT_BONE_INFO;

	i3String::Copy( pInfo->m_szName, pszBone);
	i3Matrix::Copy( &pInfo->m_Matrix, pMatrix);

	m_InitBoneList.Add( pInfo);

	return pInfo;
}

INIT_BONE_INFO * i3Viewer2Framework::findInitBone( const char * pszBone)
{
	INT32 i;

	for( i = 0; i < getInitBoneCount(); i++)
	{
		INIT_BONE_INFO * pInfo = getInitBone( i);

		if( i3String::Compare( pInfo->m_szName, pszBone) == 0)
			return pInfo;
	}

	return NULL;
}

void i3Viewer2Framework::removeAllInitBone(void)
{
	INT32 i;

	for( i = 0; i < getInitBoneCount(); i++)
	{
		INIT_BONE_INFO * pInfo = getInitBone( i);

		delete pInfo;
	}

	m_InitBoneList.Clear();
}

void i3Viewer2Framework::BuildInitBone(void)
{
	// Sg.를 돌면서 Bone으로 간주되는 노드들의 초기 Matrix를 저장해 둔다.
	removeAllInitBone();

	if( m_pNormalViewRoot != NULL)
	{
		i3Scene::Traverse( m_pNormalViewRoot, _BuildInitBoneProc, this);
	}
}

I3SG_TRAVERSAL_RESULT i3Viewer2Framework::_BuildInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 i;
	i3Viewer2Framework * pFramework = (i3Viewer2Framework *) pUserData;

	if( pNode->IsTypeOf( i3Body::GetClassMeta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD != NULL)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				pFramework->addInitBone( pSkel->getName( i), pSkel->getMatrix( i));
			}
		}
	}
	else if( pNode->IsTypeOf( i3Transform::GetClassMeta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		pFramework->addInitBone( pTrans->GetName(), pTrans->GetMatrix());
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Viewer2Framework::ApplyInitBone(void)
{
	// // Sg.를 돌면서 Bone으로 간주되는 노드들오 초기 Matrix를 설정해준다.
	if( m_pNormalViewRoot != NULL)
		i3Scene::Traverse( m_pNormalViewRoot, _ApplyInitBoneProc, this);
}

I3SG_TRAVERSAL_RESULT i3Viewer2Framework::_ApplyInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 i;
	i3Viewer2Framework * pFramework = (i3Viewer2Framework *) pUserData;

	if( pNode->IsTypeOf( i3Body::GetClassMeta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD != NULL)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				INIT_BONE_INFO * pInfo = pFramework->findInitBone( pSkel->getName( i));

				if( pInfo != NULL)
					pSkel->setMatrix( i, &pInfo->m_Matrix);
			}
		}
	}
	else if( pNode->IsTypeOf( i3Transform::GetClassMeta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		INIT_BONE_INFO * pInfo = pFramework->findInitBone( pTrans->GetName());

		if( pInfo != NULL)
			pTrans->SetMatrix( &pInfo->m_Matrix);
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Viewer2Framework::SetDynamicState( bool bFlag)
{
	if( m_pNormalViewRoot == NULL)
		return;

	UINT32 addState, remState;

	addState = remState = 0;

	if( bFlag)
	{
		remState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}
	else
	{
		addState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}

	VEC3D vel;

	i3Vector::Set( &vel, 0.0f, 0.0f, 0.0f);

	i3Scene::ModifyDynamicState( m_pNormalViewRoot, addState, remState);
	i3Scene::SetDynamicVelocity( m_pNormalViewRoot, NULL, &vel, &vel);
}
