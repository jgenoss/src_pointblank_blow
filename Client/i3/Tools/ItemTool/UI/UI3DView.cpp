#include "StdAfx.h"
#include "UI3DView.h"


I3_CLASS_INSTANCE(cUI3DView); //, i3GameNode);

cUI3DView::cUI3DView(void)
:	m_nX(0)
,	m_nY(0)
,	m_nZ(0)
{
	m_X = 0;
	m_Y = 0;
	m_Width = 0;
	m_Height = 0;

	m_pAttrSet		= NULL;
	m_pCamera		= NULL;
	m_pLightingEnableAttr	= NULL;
	m_pViewport		= NULL;

	m_pSprite		= NULL;
	m_bIsRotate		= false;
}

cUI3DView::~cUI3DView(void)
{
	Destroy();
}


void cUI3DView::Destroy(void)
{
	if(m_pCamera && (g_pMainSCFramework != NULL) && (g_pMainSCFramework->getSgContext() != NULL))
		g_pMainSCFramework->getSgContext()->removeCamera( m_pCamera);

	I3_SAFE_NODE_RELEASE( m_pSprite);
	I3_SAFE_NODE_RELEASE( m_pAttrSet);
	I3_SAFE_NODE_RELEASE( m_pCamera);
}


bool cUI3DView::Set3DView(i3Node* pSceneNode, i3SceneGraphInfo* pInfo)
{
	if( pSceneNode == NULL || pInfo == NULL )
		return false;

	i3Node*		pResource	= NULL;

	if( pInfo != NULL)
	{
		if( pInfo->getCommonSg() != NULL)
		{
			if( pInfo->getInstanceSg() != NULL)
			{
				i3Node * pAttachNode = pInfo->getCommonSg();

				while( pAttachNode->GetChildCount() > 0)
				{
					pAttachNode = pAttachNode->GetChild( pAttachNode->GetChildCount() - 1);
				}

				//pAttachNode->AddChild( m_pInterLinkNode);
				//m_pInterLinkNode->AddChild( m_pSgInfo->getInstanceSg());
			}

			pResource = pInfo->getCommonSg();
		}
		else
		{
			pResource = pInfo->getInstanceSg();
		}
	}

	if( pResource != NULL)
		pResource->Instantiate();



	UINT32 addState, remState;

	addState = remState = 0;

// 	if( bFlag)
// 	{
// 		remState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
// 	}
// 	else
// 	{
		addState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
//	}

	//VEC3D vel;

	//i3Vector::Set( &vel, 0.0f, 0.0f, 0.0f);

	i3Scene::ModifyDynamicState( pResource, addState, remState);
	//i3Scene::SetDynamicVelocity( pResource, NULL, &vel, &vel);

	g_pItemSCFramework->SetSceneGraph(pResource);

	//if( _Bind3DView(pResource) == false )
	//	return false;

	OnVisibleScene();

// 	m_pSprite->SetRect( 0, 0.0f, 0.0f, (REAL32)m_Width, (REAL32)m_Height );
// 	m_pSprite->SetTexture( m_pCamera->getColorRenderTarget() );
// 	m_pSprite->SetTextureFilter( I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_LINEAR);
// 	m_pSprite->SetTextureCoord( 0, 0.0f, 0.0f, (REAL32)m_Width, (REAL32)m_Height );




	//m_pSprite->SetEnable( 0, TRUE);
	//pSceneNode->AddChild(m_pSprite);

	return true;
}


bool cUI3DView::_PrepareSprites(void)
{
	if( m_pSprite || m_pCamera )
		return false;


	m_pSprite = i3Sprite2D::new_object();
	m_pSprite->Create(1);
	//m_pSprite->setCoordType( I3_SPRITE2D_COORD_PIXEL);
	m_pSprite->SetColor( 0, 255, 255, 255, 255);
	

	m_pCamera = i3Camera::new_object();
	m_pCamera->SetEnable( TRUE);
	m_pCamera->AddFlag( I3_NODEFLAG_DISABLE);
	m_pCamera->addStyle( I3_CAM_STYLE_SUB);



	m_pCamera->CreateRenderTarget( (REAL32)m_Width, (REAL32)m_Height, I3G_IMAGE_FORMAT_BGRA_8888, I3G_IMAGE_FORMAT_D24S8);
	m_pCamera->SetClearColor( 0, 0, 0, 0);

	// Projection Matrix
	m_pCamera->SetPerspective( I3_DEG2RAD( 38.0f), (REAL32)m_Width, (REAL32)m_Height, 0.1f, 100.0f);

	//return _RecreateTarget();
	return true;
}


bool cUI3DView::_Bind3DView(i3Node* pResource, MATRIX * pCamMtx)
{
	if( pResource == NULL)
		return false;

	Destroy();
	if( _PrepareSprites() == false )
		return false;


	m_pAttrSet = i3AttrSet::new_object();

	m_pCamera->setSubCamSg( m_pAttrSet );

	// Viewport
	{
		m_pViewport = i3ViewportAttr::new_object_ref();
		m_pAttrSet->AddAttr( m_pViewport);
	}

	// View Matrix
	if( pCamMtx == NULL)
	{
		MATRIX	mt;
		i3Matrix::Identity( &mt);
		
		//캐릭터, 무기
		//i3Matrix::SetPos( &mt, 0.0f, 0.8f, 8.0f, 1.0f);

		i3Matrix::SetPos( &mt, 0.0f, 0.8f, 8.0f, 1.0f);

		//마스크, 투구
 		i3Matrix::PostRotateXDeg( &mt, 90.0f);
 		i3Matrix::PostRotateZDeg( &mt, 180.0f);

		//i3Matrix::PostRotateYDeg( &mt, 0);

		m_pCamera->SetMatrix( &mt);
	}
	else
		m_pCamera->SetMatrix( pCamMtx);


	// Lighting Enable = TRUE
	{
		m_pLightingEnableAttr = i3LightingEnableAttr::new_object_ref();
		m_pLightingEnableAttr->Set( TRUE);
		m_pAttrSet->AddAttr( m_pLightingEnableAttr);
	}

	// Alpha Test Enable = TRUE
	{
		i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();
		pAttr->Set( TRUE);
		m_pAttrSet->AddAttr( pAttr);
	}

	// Alpha Function
	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

		pAttr->SetFunction( I3G_COMP_GREATER);
		pAttr->SetRefValue( 0);

		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();
		pAttr->Set( I3G_FACE_CULL_BACK);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3TextureWrapAttr * pAttr = i3TextureWrapAttr::new_object_ref();
		pAttr->SetHorzWrap( I3G_TEXTURE_WRAP_WRAP);
		pAttr->SetVertWrap( I3G_TEXTURE_WRAP_WRAP);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set( TRUE);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set( TRUE);
		m_pAttrSet->AddAttr( pAttr);
	}

 //	((i3Body*)pResource)->removeStyle( I3BS_LOD_AUTO);
 //	((i3Body*)pResource)->setCurrentLevel(1);

	i3Body* pBody = (i3Body*)i3Scene::FindNodeByExactType(pResource, i3Body::static_meta());
	if (pBody)
	{
		pBody->removeStyle(I3BS_LOD_AUTO);
		pBody->setCurrentLevel(1);
	}
	
	// 모델 연결	
	m_pAttrSet->AddChild( pResource);


	_UpdateViewport();

	return true;
}


void cUI3DView::OnVisibleScene(void)
{
	if((m_pCamera != NULL) && (m_pCamera->IsFlag( I3_NODEFLAG_DISABLE)))
	{
		m_pCamera->RemoveFlag( I3_NODEFLAG_DISABLE);

		if( g_pMainSCFramework )
			g_pMainSCFramework->getSgContext()->addCamera( m_pCamera);
	}

	if( m_pSprite != NULL && m_pSprite->IsFlag( I3_NODEFLAG_DISABLE))
			m_pSprite->RemoveFlag( I3_NODEFLAG_DISABLE);
}

void cUI3DView::OnInvisibleScene(void)
{
	if(( m_pCamera != NULL) && (m_pCamera->IsFlag( I3_NODEFLAG_DISABLE) == FALSE))
	{
		m_pCamera->AddFlag( I3_NODEFLAG_DISABLE);

		if( g_pMainSCFramework )
			g_pMainSCFramework->getSgContext()->removeCamera( m_pCamera);
		
	}

	if( m_pSprite != NULL && m_pSprite->IsFlag( I3_NODEFLAG_DISABLE) == FALSE)
			m_pSprite->AddFlag( I3_NODEFLAG_DISABLE);
}


void cUI3DView::SetSize( INT32 nX, INT32 nY, INT32 nWidth, INT32 nHeight )
{
	m_X			= nX;
	m_Y			= nY;
	m_Width		= nWidth;
	m_Height	= nHeight;

	_RecreateTarget();
	_UpdateViewport();
}
void cUI3DView::OnRotateCamera(const float& fDelta)
{
	if (m_pCamera)
	{
		RT_MATRIX pCamMat = *m_pCamera->GetMatrix();
		

		//마스크, 투구
		i3Matrix::PostRotateZDeg( &pCamMat, fDelta * 100.0f);

		//캐릭터, 무기
		//i3Matrix::PostRotateYDeg( &pCamMat, fDelta * 100.0f);
		m_pCamera->SetMatrix( &pCamMat);
	}

}

void cUI3DView::_UpdateViewport(void)
{
	if( m_pViewport != NULL)
	{
		m_pViewport->SetViewportRect( 0, 0, m_Width, m_Height );
	}

	if( m_pSprite != NULL)
	{
		m_pSprite->SetRect( 0, 0.0f, 0.0f, (REAL32)m_Width, (REAL32)m_Height );
	}
}


bool cUI3DView::_RecreateTarget(void)
{
	if (m_pCamera == NULL || m_pSprite == NULL)
		return false;

	m_pCamera->CreateRenderTarget( (REAL32)m_Width, (REAL32)m_Height, I3G_IMAGE_FORMAT_BGRA_8888, I3G_IMAGE_FORMAT_D24S8);
	m_pCamera->SetClearColor( 0, 0, 0, 0);

	// Projection Matrix
	m_pCamera->SetPerspective( I3_DEG2RAD( 38.0f), (REAL32)m_Width, (REAL32)m_Height, 0.1f, 100.0f);

	m_pSprite->SetTexture( m_pCamera->getColorRenderTarget() );
	m_pSprite->SetTextureFilter( I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_LINEAR);
	m_pSprite->SetTextureCoord( 0, 0.0f, 0.0f, (REAL32)m_Width, (REAL32)m_Height );

	return true;
}

void cUI3DView::OnUpdate( RT_REAL32 rDeltaSeconds )
{
	m_nX = 0;
	m_nY = 0;
	m_nZ = 0;
	float fDelta = 0.05f;
	if (GetAsyncKeyState(VK_NUMPAD1))
	{
		m_nX -= fDelta;
	}
	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		m_nX += fDelta;
	}
	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		m_nY -= fDelta;
	}
	if (GetAsyncKeyState(VK_NUMPAD5))
	{
		m_nY += fDelta;
	}
	if (GetAsyncKeyState(VK_NUMPAD3))
	{
		m_nZ -= fDelta;
	}
	if (GetAsyncKeyState(VK_NUMPAD6))
	{
		m_nZ += fDelta;
	}

	if (m_pCamera)
	{
		RT_MATRIX matPrev = *m_pCamera->GetMatrix();
		
		RT_MATRIX mat ;
		i3Matrix::Identity(&mat);
		i3Matrix::SetPos( &mat, m_nX, m_nY, m_nZ, 1.0f);
		i3Matrix::Mul(&mat, &mat, &matPrev);
		m_pCamera->SetMatrix( &mat);

		VEC3D* pVec = i3Matrix::GetPos(&mat);
		I3TRACE("X:%f Y:%f Z:%f\n", pVec->x, pVec->y, pVec->z);
	}

	//i3GameNode::OnUpdate(rDeltaSeconds);
	if (m_bIsRotate)
		OnRotateCamera(rDeltaSeconds);
}