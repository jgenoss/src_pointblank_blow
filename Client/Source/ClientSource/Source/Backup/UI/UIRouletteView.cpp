#include "pch.h"
#include "UIRouletteView.h"

#include "UIMainFrame.h"


I3_CLASS_INSTANCE( UIRouletteView);//, i3GameNode);

static i3Light * s_pLight = NULL;

UIRouletteView::UIRouletteView()
{
	m_p3DView = NULL;
	m_pRoulette = NULL;
	m_pLight = NULL;

	m_rCharaChangePoseTime = 0.f;

	m_LightTheta = 0.f;
	m_LightRho = 0.f;
	m_LightDist = 0.f;
	m_timeLocal = 0.f;
}

UIRouletteView::~UIRouletteView()
{
	_ReleaseLight();
I3_SAFE_RELEASE(m_pRoulette);
}
/*virtual*/ void UIRouletteView::OnCreate( i3GameNode * pParent)
{

}

void UIRouletteView::_AddLight( void)
{
	VEC3D d_rot;
	COLORREAL d_diffuse, d_specular;

	if( s_pLight == NULL)
	{
		s_pLight = i3Light::new_object();
		s_pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);

		i3Color::Set( &d_diffuse, 1.2f, 1.2f, 1.2f, 1.0f);
		i3Color::Set( &d_specular, 1.0f, 1.0f, 1.0f, 1.0f);
		i3Vector::Set( &d_rot, -12.0f, -34.0f, 4.0f);

		s_pLight->SetDiffuse( &d_diffuse);
		s_pLight->SetSpecular( &d_specular);

		MATRIX mtx;

		i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, getZ( &d_rot));
		i3Matrix::PostRotateX( &mtx, I3_DEG2RAD( getY( &d_rot)));
		i3Matrix::PostRotateY( &mtx, I3_DEG2RAD( getX( &d_rot)));

		//I3TRACE( "%f, %f\n", m_LightTheta, m_LightRho);

		s_pLight->SetWorldSpaceTransform( &mtx);

		g_pFramework->addLight( s_pLight);
	}
	else
		I3_SAFE_ADDREF( s_pLight);

	g_pFramework->setShadowQuality( 0);
}

void UIRouletteView::_ReleaseLight( void)
{
	if( g_pFramework && s_pLight)
	{
		s_pLight->Release();

		if( s_pLight->GetRefCount() == 1)
		{
			g_pFramework->removeLight( s_pLight);
			s_pLight = NULL;
		}
	}
}

void UIRouletteView::ReleaseRoulette( void)
{
	_ReleaseLight();
	if (m_pRoulette != NULL)
	{
		g_pFramework->GetUIMainframe()->RemoveChild(m_pRoulette);
		I3_SAFE_RELEASE(m_pRoulette);
	}

	}
	


void UIRouletteView::Create3DViewBox( i3UIControl * pParent)
{
	I3ASSERT( pParent != NULL);

	m_p3DView = i3UI3DViewBox::new_object();
	I3ASSERT( m_p3DView);

	I3ASSERT( pParent);
	pParent->AddChild( m_p3DView);

	m_p3DView->setSize( pParent->getWidth()-2, pParent->getHeight()-2);
	m_p3DView->setPos( 1, 1);
}

void UIRouletteView::Delete3DViewBox( void)
{
	if( m_p3DView != NULL)
	{
		i3GameNode * pParent = m_p3DView->getParent();
		pParent->RemoveChild( m_p3DView);
		I3_SAFE_RELEASE( m_p3DView);
	}
}

void UIRouletteView::CreateViewRoulette()
{
	VEC3D vPos;
	m_pRoulette = RouletteObject::new_object();
	
	m_pRoulette->Create(g_pFramework,NULL);
	m_pRoulette->OnBindResource();

	I3_GAMENODE_ADDCHILD( g_pFramework->GetUIMainframe(), m_pRoulette );

	m_p3DView->Create3DView();
	m_p3DView->Set3DView( m_pRoulette->getSceneObject());
	_AddLight();

	m_p3DView->OnVisibleScene();


	MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
	i3Matrix::Identity( pCamMtx);
	i3Matrix::SetPos( pCamMtx, 0.17f, 0.92f, 90.0f, 1.0f);


	 m_p3DView->get3DCamera()->SetFOV(I3_DEG2RAD( 41.5f));
		 //	i3Matrix::PostRotateYDeg( pCamMtx, -5.0f);

	
	m_pRoulette->SetMatrix( &I3_IDENTITYMATRIX);
		
	i3Vector::Set( &vPos, 0.f, 0.0f, -7.0f);
	m_pRoulette->SetPos( &vPos);

	
}



void UIRouletteView::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate( rDeltaSeconds);
	
}

bool UIRouletteView::OnKeyInput( i3InputDeviceManager * pInputMng)
{
/*
	i3InputKeyboard * pKey = pInputMng->GetKeyboard();

	if( m_p3DView)
	{

		MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();

		if( pKey->GetStrokeState(I3I_KEY_LEFT))
		{	

			i3Matrix::PostRotateYDeg( pCamMtx, -5.f);
			I3TRACE( "CamRotLeft \n");
			i3Vector::Dump( i3Matrix::GetAt( pCamMtx));
			return true;
		}
		else if( pKey->GetStrokeState( I3I_KEY_RIGHT ))
		{
			i3Matrix::PostRotateYDeg( pCamMtx, 5.f);
			I3TRACE( "CamRotRight \n");
			i3Vector::Dump( i3Matrix::GetAt( pCamMtx));
			return true;

		}

		else if( pKey->GetStrokeState( I3I_KEY_DOWN ))
		{

			i3Matrix::PostTranslate(pCamMtx,0.f,0.0f,5.f);
			//	i3Matrix::PostTranslate(pCamMtx,5.f);
			//	i3Matrix::PostRotateYDeg( pCamMtx, 5.f);
			I3TRACE( "CamRotRight \n");
			i3Vector::Dump( i3Matrix::GetAt( pCamMtx));
			return true;

		}

		else if( pKey->GetStrokeState( I3I_KEY_UP ))
		{

			i3Matrix::PostTranslate(pCamMtx,0.f,0.0f,-5.f);

			//			i3Matrix::PostRotateYDeg( pCamMtx, 5.f);
			I3TRACE( "CamRotRight \n");
			i3Vector::Dump( i3Matrix::GetAt( pCamMtx));
			return true;

		}

	}
	*/

	return false;

}