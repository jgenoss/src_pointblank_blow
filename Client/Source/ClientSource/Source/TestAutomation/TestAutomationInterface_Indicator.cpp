#include "pch.h"

#if defined( TEST_AUTOMATION)

#include "TestAutomationInterface.h"

void TestAutomationInterface::CreateIndicator(void)
{
	m_colIndicator = COLOR(  255, 128, 43, 255);

	m_pInd = i3Sprite2D::new_object();
	m_pInd->Create( 4, false, true);
	m_pInd->SetEnable( true);
	m_pInd->SetColor( &m_colIndicator);
	m_pInd->DeactivateNode();

	m_pInd_Text = i3TextNodeDX2::new_object();
	m_pInd_Text->Create("Tahoma", 256, 10, 0, false, false, DEFAULT_QUALITY, true);
	m_pInd_Text->setAutoWrapEnable(true);
	m_pInd_Text->setSize(1024, 42);
	m_pInd_Text->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pInd_Text->SetColor( &m_colIndicator);
	m_pInd_Text->SetEnable( true);
	m_pInd_Text->DeactivateNode();

	g_pFramework->Get2DRoot()->AddChild( m_pInd);
	g_pFramework->Get2DRoot()->AddChild( m_pInd_Text);
}

#define IND_THICK		2

void TestAutomationInterface::ShowIndicator( const RECT & rect, const wchar_t * pszMsg)
{
	REAL32 x = (REAL32) rect.left;
	REAL32 y = (REAL32) rect.top;
	REAL32 w = (REAL32) (rect.right - rect.left);
	REAL32 h = (REAL32) (rect.bottom - rect.top);

	m_colIndicator.a = 255;

	m_pInd->SetRect( 0,	x, y - IND_THICK,				w, IND_THICK);			// 위쪽 가로바
	m_pInd->SetRect( 1,	x, y + h,						w, IND_THICK);			// 아래쪽 가로바
	m_pInd->SetRect( 2,	x - IND_THICK, y - IND_THICK,	IND_THICK, h + (IND_THICK * 2));	// 왼쪽 세로바
	m_pInd->SetRect( 3,	x + w,	y - IND_THICK,			IND_THICK, h + (IND_THICK * 2));	// 오른쪽 세로바
	m_pInd->SetColor( &m_colIndicator);
	m_pInd->ActivateNode();

	if( pszMsg != nullptr)
	{
		m_pInd_Text->setPos( (UINT32)x, (UINT32)y - 15 - IND_THICK);
		m_pInd_Text->setWidth( (UINT32)w);
		m_pInd_Text->setHeight( 15);
		m_pInd_Text->SetText( pszMsg);
		m_pInd_Text->SetColor( &m_colIndicator);
		m_pInd_Text->ActivateNode();
	}
}

void TestAutomationInterface::UpdateIndicator( float alpha)
{
	m_colIndicator.a = (UINT8) MIN((alpha * 384), 255);

	if( m_pInd->IsFlag( I3_NODEFLAG_DISABLE) == false)
	{
		m_pInd->SetColor( &m_colIndicator);
	}

	if( m_pInd_Text->IsFlag( I3_NODEFLAG_DISABLE) == false)
	{
		m_pInd_Text->SetColor( &m_colIndicator);
	}
}

void TestAutomationInterface::HideIndicator()
{
	m_pInd->DeactivateNode();
	m_pInd_Text->DeactivateNode();
}

void TestAutomationInterface::OnShowIndicator( const RECT & rect, const wchar_t * pszMsg)
{
	ShowIndicator( rect, pszMsg);
}

void TestAutomationInterface::OnIndicating( float alpha)
{
	if( alpha <= 0.0f)
	{
		HideIndicator();
	}
	else
	{
		UpdateIndicator( alpha);
	}
}


#endif