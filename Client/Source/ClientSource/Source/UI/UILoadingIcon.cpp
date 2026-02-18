#include "pch.h"
#include "UILoadingIcon.h"


I3_CLASS_INSTANCE( UILoadingIcon);//, i3GameNode);

i3UITemplate * UILoadingIcon::m_pTemplate = nullptr;

UILoadingIcon::UILoadingIcon()
{
	m_pFrame = nullptr;
	m_pIcon = nullptr;
	m_Delay = 0.f;
	m_ImageIdx = 0;

	if( m_pTemplate == nullptr)
	{
		i3::string strTemplateName = "";

		switch(g_pEnvSet->m_nUIVersion)
		{
		case UFV_1_5	: strTemplateName = "Img_ScreenLoading";		break;	// V1.5
		case UFV_1_1	: strTemplateName = "ButtonImageSet_Loading";	break;	// V1.1
		case UFV_1_0	: strTemplateName = "Img_ScreenLoading";		break;  // v1.0
		case UFV_1_2    : strTemplateName = "Img_ScreenLoading";		break;	// V1.2
		default			: strTemplateName = "Img_ScreenLoading";		break;	// default
		}

		m_pTemplate = i3UI::getUILibrary()->FindByName( strTemplateName.c_str() );
	}
}

UILoadingIcon::~UILoadingIcon()
{
	if( m_pIcon != nullptr)
	{
		i3UI::DeleteControl( m_pIcon);
		I3_SAFE_RELEASE( m_pIcon);
	}
}

void UILoadingIcon::EnableLoadingIcon( i3UIControl * pCtrl, bool bVisibleChild)
{
	I3ASSERT( m_pIcon == nullptr);

	I3ASSERT( m_pFrame == nullptr);
	m_pFrame = pCtrl;

	m_pFrame->setInputDisable( true);

	// FrameWnd의 Child는 안보이게
	if( bVisibleChild == false)
	{
		i3GameNode * pChild = m_pFrame->getFirstChild();
		while( pChild != nullptr)
		{
			pChild->SetVisible( false);
			pChild = pChild->getNext();
		}
	}

	
	// Icon 위치 설정
	REAL32 w = m_pFrame->getWidth() * 0.5f;
	REAL32 h = m_pFrame->getHeight() * 0.5f;

	{
		VEC3D vPos;
		VEC2D vSize;
		
		i3Vector::Set( &vSize, m_pTemplate->getShape(0)->getWidth(), m_pTemplate->getShape(0)->getHeight());

		w -= getX( &vSize) * 0.5f;
		h -= getY( &vSize) * 0.5f;

		i3Vector::Set( &vPos, w, h, 0.f);

		m_pIcon = (i3UIButtonImageSet*) i3UI::CreateControl( m_pTemplate, &vPos, &vSize, m_pFrame);
		m_pIcon->SetViewOnly( true);
		I3_MUST_ADDREF(m_pIcon);
	}
}

void UILoadingIcon::DisableLoadingIcon( void)
{
	I3ASSERT( m_pFrame != nullptr);

	if( m_pIcon != nullptr)
	{
		i3UI::DeleteControl( m_pIcon);
		I3_SAFE_RELEASE( m_pIcon);
	}

	// FrameWnd의 Child는 보이게
	i3GameNode * pChild = m_pFrame->getFirstChild();
	while( pChild != nullptr)
	{
		pChild->SetVisible( true);
		pChild = pChild->getNext();
	}

	m_pFrame->setInputDisable( false);

	m_pFrame = nullptr;
}

/*virtual*/ void UILoadingIcon::OnUpdate( REAL32 rDeltaSeconds)
{
	I3ASSERT( m_pFrame != nullptr);
	
	m_Delay += rDeltaSeconds;
	if( m_Delay > 0.0333f)	// 초당 10frame으로 이미지 변경
	{
		m_pIcon->SetShapeIdx( m_ImageIdx);

		m_ImageIdx++;
		if( m_ImageIdx >= m_pIcon->getShapeCount())
			m_ImageIdx = 0;

		m_Delay = 0.f;
	}

	i3GameNode::OnUpdate( rDeltaSeconds);
}
