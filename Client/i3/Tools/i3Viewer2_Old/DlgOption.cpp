// DlgOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "DlgOption.h"
#include "GlobalVar.h"

// CDlgOption dialog

IMPLEMENT_DYNAMIC(CDlgOption, CDialog)
CDlgOption::CDlgOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOption::IDD, pParent)
{
}

CDlgOption::~CDlgOption()
{
}

void CDlgOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_ACOLOR_PH, m_ST_A);
	DDX_Control(pDX, IDC_ST_BCOLOR_PH, m_ST_B);
	DDX_Control(pDX, IDC_ED_SHADOWSIZE, m_ED_ShadowSize);
	DDX_Control(pDX, IDC_ST_LIT_DCOLORPH, m_ST_LitD);
	DDX_Control(pDX, IDC_ST_LIT_SCOLORPH, m_ST_LitS);
	DDX_Control(pDX, IDC_CHK_GLOW, m_CHK_Glow);
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
END_MESSAGE_MAP()


// CDlgOption message handlers

BOOL CDlgOption::OnInitDialog()
{
	char conv[256];

	CDialog::OnInitDialog();

	{
		CRect rt;

		m_ST_A.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_A_Ctrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 100);

		m_ST_B.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_B_Ctrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 101);

		m_ST_LitD.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_LightD_Ctrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 102);

		m_ST_LitS.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_LightS_Ctrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 103);
	}

	// Ambient
	{
		m_A_Ctrl.setColor( & g_pOption->m_Ambient);
	}

	// Backcolor
	{
		m_B_Ctrl.setColor( & g_pOption->m_BackColor);
	}

	// Shadow
	{
		// Size
		i3String::Format( conv, sizeof(conv), "%d", g_pOption->m_ShadowSize);
		m_ED_ShadowSize.SetWindowText( conv);
	}

	// Light
	{
		// Diffuse
		m_LightD_Ctrl.setColor( & g_pOption->m_LightDiffuse);

		// Specular
		m_LightS_Ctrl.setColor( & g_pOption->m_LightSpecular);
	}

	// Glow
	{
		if( g_pOption->m_bGlow)
			m_CHK_Glow.SetCheck( BST_CHECKED);
		else
			m_CHK_Glow.SetCheck( BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOption::OnOK()
{
	i3RenderContext *	pCtx = g_pViewer->GetRenderContext();

	// Ambient
	{
		i3Color::Set( &g_pOption->m_Ambient, m_A_Ctrl.getColor());

		pCtx->setAmbient( &g_pOption->m_Ambient);
	}

	// Backcolor
	{
		i3Color::Set( &g_pOption->m_BackColor, m_B_Ctrl.getColor());

		COLOR col;

		i3Color::Set( &col, &g_pOption->m_BackColor);
		pCtx->SetClearColor( &col);
	}

	// Shadow
	{
		char conv[256];

		m_ED_ShadowSize.GetWindowText( conv, sizeof(conv));

		INT32 sz = atoi( conv);

		sz = max( sz, 128);
		sz = min( sz, 4096);

		g_pFramework->setShadowMapSize( sz);
		
		g_pOption->m_ShadowSize = sz;
	}

	// Light
	{
		i3Light * pLight = g_pFramework->getDefLight();

		// Diffuse
		pLight->SetDiffuse( m_LightD_Ctrl.getColor());
		i3Color::Set( &g_pOption->m_LightDiffuse, m_LightD_Ctrl.getColor());

		// Specular
		pLight->SetSpecular( m_LightS_Ctrl.getColor());
		i3Color::Set( &g_pOption->m_LightSpecular, m_LightS_Ctrl.getColor());
	}

	// Glow
	{
		g_pOption->m_bGlow = m_CHK_Glow.GetCheck() == BST_CHECKED;
		g_pFramework->SetGlowEnable( g_pOption->m_bGlow);
	}

	g_pFramework->ApplyOpition();

	CDialog::OnOK();
}
