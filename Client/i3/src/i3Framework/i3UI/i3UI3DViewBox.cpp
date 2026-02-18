#include "i3FrameworkPCH.h"
#include "i3UI3DViewBox.h"

#include "i3Framework.h"

// ============================================================================
//
// i3UI3DViewBox : 3D Viewer
//
// ============================================================================
I3_CLASS_INSTANCE( i3UI3DViewBox);

i3UI3DViewBox::i3UI3DViewBox()
{
	addStyle( I3UI_STYLE_NO_VT);
}

i3UI3DViewBox::~i3UI3DViewBox()
{
	ClearView();

	{
		i3Framework * pFramework = i3Framework::getCurrentFramework();

		if(m_pCamera && (pFramework != nullptr) && (pFramework->getSgContext() != nullptr))
			pFramework->getSgContext()->removeCamera( m_pCamera);
	}

	I3_SAFE_NODE_RELEASE( m_pSprite);
	I3_SAFE_NODE_RELEASE( m_pCamera);
	I3_SAFE_RELEASE( m_pAnotherRenderT);
	I3_SAFE_RELEASE( m_pAnotherRenderD);
}


/*virtual*/ void i3UI3DViewBox::prepareSprites( void)
{
	if( m_hSprites != nullptr)
	{
		if( m_nShapeCount != 1)
		{
			resetSprites( true);
		}
	}

	m_nShapeCount = 1;

	if( m_pSprite == nullptr)
	{
		m_pSprite = i3Sprite2D::new_object();
		m_pSprite->Create( 1, true, true, false);
		m_pSprite->setCoordType( I3_SPRITE2D_COORD_PIXEL);
	}

	// Camera설정
	if( m_pCamera == nullptr)
	{
		m_pCamera = i3Camera::new_object();
		m_pCamera->SetEnable(true);
		m_pCamera->AddFlag( I3_NODEFLAG_DISABLE);
		m_pCamera->addStyle( I3_CAM_STYLE_SUB);

		INT32 w, h;
		
		i3Framework* pFram = i3Framework::getCurrentFramework();
		
		m_iHeight = pFram->GetViewer()->GetViewHeight();
		m_iWidth = pFram->GetViewer()->GetViewWidth();
		
		w = i3Math::GetNearestPowerOf2( (INT32) getWidth());
		h = i3Math::GetNearestPowerOf2( (INT32) getHeight());

		float nTmpW = (float)(w * m_iWidth) / 1024.0f;
		float nTmpH = (float)(h * m_iHeight) / 768.0f;

		m_pCamera->CreateRenderTarget( (REAL32) nTmpW, (REAL32) nTmpH, I3G_IMAGE_FORMAT_BGRA_8888, I3G_IMAGE_FORMAT_D24S8);
		m_pCamera->SetClearColor( 0, 0, 0, 0);

		if( m_bUsingAnotherRenderT && (m_pAnotherRenderT != nullptr) )
		{
			i3RenderTargetAttr * pAttr = m_pCamera->getRenderTargetAttr();
			pAttr->SetColorRenderTarget( m_pAnotherRenderT);
			pAttr->SetDepthRenderTarget( m_pAnotherRenderD);
		}

		float u = (float)(getWidth() * m_iWidth) / 1024.0f;
		float v = (float)(getHeight() * m_iHeight) / 768.0f;

		m_pSprite->SetTexture( m_pCamera->getColorRenderTarget());
		m_pSprite->SetTextureFilter( I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_LINEAR);
		if( m_bUsingAnotherRenderT && (m_pAnotherRenderT != nullptr))
		{
			m_pSprite->SetTextureCoord( 0, getAccumulatedPos()->x, getAccumulatedPos()->y, u, v);
		}
		else
			m_pSprite->SetTextureCoord( 0, 0, 0, nTmpW, nTmpH);
		m_pSprite->SetColor( 0, 255, 255, 255, 255);

		m_pSprite->SetRect( 0, getAccumulatedPos()->x, getAccumulatedPos()->y, u, v);
		m_pSprite->SetEnable( 0, true);
	}

	if( m_hSprites == nullptr)
	{
		m_hSprites = (INT32*)i3MemAlloc( sizeof( INT32));
		*m_hSprites = -1;
	}

	if( *m_hSprites == -1)
	{
		*m_hSprites = g_pUIRenderer->AllocSprite( I3UI_SPRITE_STYLE_NODE, this);
		I3ASSERT( *m_hSprites >= 0);

		g_pUIRenderer->getSprite( *m_hSprites)->setNode( m_pSprite);
	}

	m_bResetSprites = false;
}

void i3UI3DViewBox::Set3DView( i3Node* pResource, MATRIX * pCamMtx)
{
	prepareSprites();

	if( pResource == nullptr)
		return;

	if( pCamMtx != nullptr)
	{
		I3ASSERT( m_pCamera != nullptr);
		m_pCamera->SetMatrix( pCamMtx);
	}

	// 모델 연결	
	I3ASSERT( m_pAttrSet != nullptr);
	m_pAttrSet->AddChild( pResource);

	UpdateViewport();
}

void i3UI3DViewBox::Create3DView( void)
{
	prepareSprites();

	I3_SAFE_NODE_RELEASE( m_pAttrSet);

	I3ASSERT( m_pCamera != nullptr);

	m_pAttrSet = i3AttrSet::new_object();
	I3ASSERT( m_pAttrSet != nullptr);

	m_pCamera->setSubCamSg( m_pAttrSet);

	// Viewport
	{
		m_pViewport = i3ViewportAttr::new_object_ref();
		m_pAttrSet->AddAttr( m_pViewport);
	}

	// View Matrix
	{
		MATRIX mt;
		
		i3Matrix::SetPos( &mt, 0.0f, 0.8f, 3.0f, 1.0f);
		i3Matrix::PostRotateYDeg( &mt, -35.0f);

		m_pCamera->SetMatrix( &mt);
	}

	// Projection Matrix
	{
		m_pCamera->SetPerspective( I3_DEG2RAD( 38.0f), getWidth(), getHeight(), 0.1f, 100.0f);
	}

	// Lighting Enable = true
	{
		m_pLightingEnableAttr = i3LightingEnableAttr::new_object_ref();
		m_pLightingEnableAttr->Set(true);
		m_pAttrSet->AddAttr( m_pLightingEnableAttr);
	}

	// Alpha Test Enable = true
	//{
	//	i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();
	//	pAttr->Set( true);
	//	m_pAttrSet->AddAttr( pAttr);
	//}

	//// Alpha Function
	//{
	//	i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

	//	pAttr->SetFunction( I3G_COMP_GREATER);
	//	pAttr->SetRefValue( 0);

	//	m_pAttrSet->AddAttr( pAttr);
	//}

	//{
	//	i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();
	//	pAttr->Set( I3G_FACE_CULL_BACK);
	//	m_pAttrSet->AddAttr( pAttr);
	//}

	/*{
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
	}*/

	/*{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set( true);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set( true);
		m_pAttrSet->AddAttr( pAttr);
	}*/
}

void i3UI3DViewBox::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{	
}

/*virtual*/ void i3UI3DViewBox::OnUpdate( REAL32 rDeltaSeconds)
{
	bool bEnable = isEnable() && isReadyToShow() && isVisible();

	if( m_pCamera != nullptr)
	{
		if( bEnable)
			m_pCamera->RemoveFlag( I3_NODEFLAG_DISABLE);
		else
			m_pCamera->AddFlag( I3_NODEFLAG_DISABLE);
	}

	UpdateViewport();

	i3GameNode::OnUpdate( rDeltaSeconds);
}

void i3UI3DViewBox::OnVisibleScene(void)
{
	if( m_bControlEnable )
	{
		if( m_pAttrSet != nullptr)
		{
			if( m_pAttrSet->GetChildCount() > 0)
			{
				i3Node * pRes = m_pAttrSet->GetChild( 0);

				if( pRes != nullptr)
				{
					I3_MUST_ADDREF(pRes);
					Create3DView();
					Set3DView( pRes);
					I3_MUST_RELEASE(pRes);

					I3ASSERT( m_pCamera != nullptr);
					m_pCamera->SetMatrix( &m_mtxReviveCam);
					m_pCamera->SetFOV(m_ReviveFov);


				}
			}
		}

		m_bControlEnable = false;
	}

	i3UIControl::OnVisibleScene();

	if((m_pCamera != nullptr) && (m_pCamera->IsFlag( I3_NODEFLAG_DISABLE)))
	{
		m_pCamera->RemoveFlag( I3_NODEFLAG_DISABLE);

		i3Framework * pFramework = i3Framework::getCurrentFramework();
		pFramework->getSgContext()->addCamera( m_pCamera);
	}

	if( m_pSprite != nullptr && m_pSprite->IsFlag( I3_NODEFLAG_DISABLE))
			m_pSprite->RemoveFlag( I3_NODEFLAG_DISABLE);
}

void i3UI3DViewBox::OnInvisibleScene(void)
{
	i3UIControl::OnInvisibleScene();

	if(( m_pCamera != nullptr) && (m_pCamera->IsFlag( I3_NODEFLAG_DISABLE) == false))
	{
		m_pCamera->AddFlag( I3_NODEFLAG_DISABLE);

		i3Framework * pFramework = i3Framework::getCurrentFramework();
		pFramework->getSgContext()->removeCamera( m_pCamera);
		
	}

	if( m_pSprite != nullptr && m_pSprite->IsFlag( I3_NODEFLAG_DISABLE) == false)
			m_pSprite->AddFlag( I3_NODEFLAG_DISABLE);
}


#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 UI3DVIEW
	{
		UINT8	m_ID[4] = { 'U', '3', 'D', '1' };
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3UI3DViewBox::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UI3DVIEW data;

	Result = i3UIControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	
	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3UI3DViewBox::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3UI3DViewBox::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UI3DVIEW data;

	Result = i3UIControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3UI3DViewBox::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

bool i3UI3DViewBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	return bResult;
}

bool i3UI3DViewBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	return bResult;
}

void i3UI3DViewBox::UpdateViewport(void)
{
	i3Framework* pFram = i3Framework::getCurrentFramework();

	INT32 nCurrH = pFram->GetViewer()->GetViewHeight();
	INT32 nCurrW = pFram->GetViewer()->GetViewWidth();

	if (m_iWidth != nCurrW || m_iHeight != nCurrH)
		RecreateTarget();

	if( m_pSprite != nullptr)
	{
		INT32 x, y;

		REAL32 nTmpW = (REAL32)(getWidth() * nCurrW) / 1024.0f;
		REAL32 nTmpH = (REAL32)(getHeight() * nCurrH) / 768.0f;

		x = (INT32) (getAccumulatedPos()->x);
		y = (INT32) (getAccumulatedPos()->y);

		m_pSprite->SetRect( 0, (REAL32) x, (REAL32) y, nTmpW, nTmpH);

		if( m_pViewport != nullptr)
		{
			INT32 w = i3Math::GetNearestPowerOf2( (INT32) getWidth());
			INT32 h = i3Math::GetNearestPowerOf2( (INT32) getHeight());
			m_pViewport->SetViewportRect( 0, 0, (INT32) w, (INT32) h);
		}
	}
}

void i3UI3DViewBox::SetViewMatrix(const MATRIX& mt)
{
	if( !m_pCamera) return;

	MATRIX tmp;

	i3Matrix::Inverse( &tmp, nullptr, &mt);
	m_pCamera->SetMatrix( &tmp);
}

void i3UI3DViewBox::SetLightingEnable( bool bFlag)
{
	m_pLightingEnableAttr->Set( bFlag);
}

void i3UI3DViewBox::ClearView(void)
{
	if( m_pAttrSet)
	{
		if( m_bControlEnable == false )
			m_pCamera->RemoveChild( m_pAttrSet);

		I3_SAFE_RELEASE( m_pAttrSet);
	}
}

void i3UI3DViewBox::SetAnotherRenderTarget( i3Texture * pRT, i3Texture * pDT)
{
	m_bUsingAnotherRenderT = (pRT != nullptr)?true:false;

	I3_REF_CHANGE( m_pAnotherRenderT, pRT);
	I3_REF_CHANGE( m_pAnotherRenderD, pDT);
}

void i3UI3DViewBox::RecreateTarget()
{
	if (m_pCamera == nullptr)
		return;

	i3Framework* pFram = i3Framework::getCurrentFramework();

	m_iHeight = pFram->GetViewer()->GetViewHeight();
	m_iWidth = pFram->GetViewer()->GetViewWidth();

	INT32 w = i3Math::GetNearestPowerOf2( (INT32) getWidth());
	INT32 h = i3Math::GetNearestPowerOf2( (INT32) getHeight());

	float nTmpW = (float)(w * m_iWidth) / 1024.0f;
	float nTmpH = (float)(h * m_iHeight) / 768.0f;

	m_pCamera->CreateRenderTarget( (REAL32) nTmpW, (REAL32) nTmpH, I3G_IMAGE_FORMAT_BGRA_8888, I3G_IMAGE_FORMAT_D24X8);

	m_pSprite->SetTexture( m_pCamera->getColorRenderTarget());
	m_pSprite->SetTextureCoord( 0, 0.0f, 0.0f, nTmpW, nTmpH);

	nTmpW = (float)(getWidth() * m_iWidth) / 1024.0f;
	nTmpH = (float)(getHeight() * m_iHeight) / 768.0f;
	m_pSprite->SetRect( 0, 0.0f, 0.0f, nTmpW, nTmpH);
}

bool i3UI3DViewBox::OnLostDevice( bool bLostDevice)
{
	if( i3UIControl::OnLostDevice( bLostDevice) == false)
		return false;

	if( m_pCamera != nullptr)
	{
		i3Matrix::Copy( &m_mtxReviveCam, m_pCamera->GetMatrix());
		m_ReviveFov = m_pCamera->GetFOV();
	}

	i3Framework * pFramework = i3Framework::getCurrentFramework();

	if(m_pCamera && (pFramework != nullptr) && (pFramework->getSgContext() != nullptr))
		pFramework->getSgContext()->removeCamera( m_pCamera);

	I3_SAFE_NODE_RELEASE( m_pCamera);
	I3_SAFE_NODE_RELEASE( m_pSprite);

	return true;
}

bool i3UI3DViewBox::OnRevive( i3RenderContext * pCtx)
{
	if( i3UIControl::OnRevive( pCtx) == false)
	{
		I3_SAFE_NODE_RELEASE( m_pSprite);
		m_bControlEnable = true;
		return false;
	}

	if( m_pAttrSet != nullptr)
	{
		if( m_pAttrSet->GetChildCount() > 0)
		{
			i3Node * pRes = m_pAttrSet->GetChild( 0);

			if( pRes != nullptr)
			{
				I3_MUST_ADDREF(pRes);
				Create3DView();
				Set3DView( pRes);
				I3_MUST_RELEASE(pRes);

				I3ASSERT( m_pCamera != nullptr);
				m_pCamera->SetMatrix( &m_mtxReviveCam);
				m_pCamera->SetFOV(m_ReviveFov);

				OnVisibleScene();
			}
		}
	}

	return true;
}


