#include "i3GuiPCH.h"
#include "../i3Math/i3Matrix.h"
#include "i3Gui3DView.h"

// ============================================================================
//
// i3Gui3DView : 3D Viewer
//
// ============================================================================
I3_CLASS_INSTANCE( i3Gui3DView);

i3Gui3DView::i3Gui3DView()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

i3Gui3DView::~i3Gui3DView()
{	
	if( m_pAttrSet != nullptr)
	{
		m_pAttrSet->RemoveFromParent();
		I3_SAFE_RELEASE( m_pAttrSet);
	}

	if( m_pCamera != nullptr)
	{
		m_pCamera->RemoveFromParent();
		I3_SAFE_RELEASE( m_pCamera);
	}
}

// 갱신
void i3Gui3DView::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GuiControl::OnUpdate( rDeltaSeconds ); 
}

void i3Gui3DView::Set3DView( i3Node* pResource)
{
	if( m_pAttrSet)
	{
		GetNode()->RemoveChild( m_pAttrSet);
		I3_SAFE_RELEASE( m_pAttrSet);
	}

	if( pResource == nullptr)
		return;

	// Camera설정
	if( m_pCamera == nullptr)
	{
		m_pCamera = i3Camera::new_object();
		m_pCamera->SetEnable(true);
		m_pCamera->addStyle( I3_CAM_STYLE_SUB);

		GetNode()->AddChild( m_pCamera);
	}
		
	m_pAttrSet = i3AttrSet::new_object();
	m_pCamera->AddChild( m_pAttrSet);

	// Viewport
	{
		i3ViewportAttr * pAttr = i3ViewportAttr::new_object_ref();
		pAttr->SetViewportRect( (INT32)getTotalPosX(), (INT32)getTotalPosY(), m_nWidth, m_nHeight);

		m_pAttrSet->AddAttr( pAttr);
	}

	// Clear
	{
		// 모든 화면을 다 지우고 있습니다.
		// 이후 Control Rect만 적용하려면, SetClearRect()를 설정해주시고,
		// UpdateViewport()와 유사한 UpdateClearRect()를 만들어주세요.	- praptor
		i3ClearRenderTargetAttr * pAttr = i3ClearRenderTargetAttr::new_object_ref();
		pAttr->SetAddClearMask(I3G_CLEAR_DEPTH);
		pAttr->SetRemoveClearMask(I3G_CLEAR_COLOR | I3G_CLEAR_STENCIL);
		pAttr->SetClearZ(1.0f);
		m_pAttrSet->AddAttr( pAttr);
	}

	// View Matrix
	{		
		MATRIX mt;
		
		i3Matrix::SetPos( &mt, 0.0f, 0.8f, 3.0f, 1.0f);
		i3Matrix::PostRotateYDeg( &mt, -30.0f);

		m_pCamera->SetMatrix( &mt);
	}

	// Projection Matrix
	{
		m_pCamera->SetPerspective( I3_DEG2RAD( 38.0f), (REAL32)m_nWidth, (REAL32)m_nHeight, 0.1f, 100.0f);
	}

	// Texture Enable = true
	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
		pAttr->Set(true);
		m_pAttrSet->AddAttr( pAttr);
	}

	// Lighting Enable = true
	{
		m_pLightingEnableAttr = i3LightingEnableAttr::new_object_ref();
		m_pLightingEnableAttr->Set(true);
		m_pAttrSet->AddAttr( m_pLightingEnableAttr);
	}

	// Alpha Test Enable = true
	{
		i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();
		pAttr->Set(true);
		m_pAttrSet->AddAttr( pAttr);
	}

	// Alpha Function
	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

		pAttr->SetFunction( I3G_COMP_GREATER);
		pAttr->SetRefValue( 0);

		m_pAttrSet->AddAttr( pAttr);
	}

	// Blend Enable = true
	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
		pAttr->Set(true);
		m_pAttrSet->AddAttr( pAttr);
	}

	// Blend Mode( I3G_BLEND_SRCCOLOR, I3G_BLEND_INVSRCCOLOR)
	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
		pAttr->SetBlendOperation( I3G_BLEND_OP_ADD);

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
		pAttr->Set(true);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZFuncAttr * pAttr = i3ZFuncAttr::new_object_ref();
		pAttr->Set( I3G_COMP_LESS);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set(true);
		m_pAttrSet->AddAttr( pAttr);
	}

#if defined( I3G_DX)
	{
		i3ShadeModeAttr * pAttr = i3ShadeModeAttr::new_object_ref();
		pAttr->Set( I3G_SHADE_PHONG);
		m_pAttrSet->AddAttr( pAttr);
	}
#endif


	// Light설정
	i3LightSet * pLightSet = i3LightSet::new_object_ref();
	m_pLightAttr = i3LightAttr::new_object_ref();
	pLightSet->AddLightAttr( m_pLightAttr);
	{
		//i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
		//pLightAttr->setDiffuse( &col);

		//pLightAttr->setSpecular( &col);

		m_pLightAttr->setLightType( I3G_LIGHT_DIRECTIONAL);
	}

	i3Transform *pLightTrans = i3Transform::new_object_ref();
	pLightTrans->AddChild( pLightSet);
	pLightTrans->SetMatrix( &I3_IDENTITYMATRIX);
	m_pAttrSet->AddChild( pLightTrans);


	// 모델 연결	
	pLightTrans->AddChild( pResource);
}

void i3Gui3DView::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable()) 
	{		
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_UP))
	{
		if( i3GuiRoot::getCapturedMouseControl() != this ) return;

		i3GuiRoot::setCapturedMouseControl( nullptr);
		if( IsOnMouse( fX, fY))
		{
			setControlState(I3GUI_CONTROL_STATE_NORMAL);

			PushControlNotify(I3GUI_3DVIEW_NOTIFY_CLICKED, 0, 0);
		}

		PushControlNotify(I3GUI_3DVIEW_NOTIFY_DRAG, MAKE_DRAG_POS(fX,fY), I3GUI_3DVIEW_END_DRAG);
		return;
	}

	if( !IsOnMouse(fX, fY))
	{		
		setControlState(I3GUI_CONTROL_STATE_NORMAL);
		PushControlNotify(I3GUI_3DVIEW_NOTIFY_DRAG, MAKE_DRAG_POS(fX,fY), I3GUI_3DVIEW_DRAGGING);
		return;
	}

	if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_PRESS))
	{
		if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_DOWN) || IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_DCLICK))
		{
			i3GuiRoot::setCapturedMouseControl( this);
			PushControlNotify(I3GUI_3DVIEW_NOTIFY_DRAG, MAKE_DRAG_POS(fX,fY), I3GUI_3DVIEW_BEGIN_DRAG);
		}
		else
		{
			PushControlNotify(I3GUI_3DVIEW_NOTIFY_DRAG, MAKE_DRAG_POS(fX,fY), I3GUI_3DVIEW_DRAGGING);
		}
		
		if( i3GuiRoot::getCapturedMouseControl() == this)
		{
			setControlState(I3GUI_CONTROL_STATE_CLICKED);	
		}		

		return;
	}

	setControlState(I3GUI_CONTROL_STATE_ONMOUSE);
}

void i3Gui3DView::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3Gui3DView*>(pDest))
		return ;

	i3Gui3DView * p3DView = (i3Gui3DView*)pDest;

	p3DView->setControlState( m_ControlState );
}

void i3Gui3DView::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUI3DVIEW
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3Gui3DView::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUI3DVIEW data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3Gui3DView::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3Gui3DView::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUI3DVIEW data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiStatic::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}

bool i3Gui3DView::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3GuiControl::OnSaveXML( pFile, pXML);

	return bResult;
}

bool i3Gui3DView::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3GuiControl::OnLoadXML( pFile, pXML);

	return bResult;
}

void i3Gui3DView::UpdateViewport(void)
{
	if( !m_pAttrSet ) return;

	i3ViewportAttr* pAttr = (i3ViewportAttr*)m_pAttrSet->FindAttr(i3ViewportAttr::static_meta());
	if( pAttr)
	{
		pAttr->SetViewportRect( (INT32)getTotalPosX(), (INT32)getTotalPosY(), m_nWidth, m_nHeight);
	}	
}

void i3Gui3DView::SetViewMatrix(const MATRIX& mt)
{
	if( !m_pCamera) return;

	MATRIX tmp;

	i3Matrix::Inverse( &tmp, nullptr, &mt);
	m_pCamera->SetMatrix( &tmp);
}

void i3Gui3DView::SetLightingEnable( bool bFlag)
{
	m_pLightingEnableAttr->Set( bFlag);
}

void i3Gui3DView::ClearView(void)
{
	if( m_pAttrSet)
	{
		m_pCamera->RemoveChild( m_pAttrSet);
		I3_SAFE_RELEASE( m_pAttrSet);
	}
}
