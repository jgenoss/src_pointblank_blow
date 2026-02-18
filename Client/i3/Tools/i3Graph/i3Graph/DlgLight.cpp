// DlgLight.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "DlgLight.h"
#include ".\dlglight.h"


// CDlgLight dialog

IMPLEMENT_DYNAMIC(CDlgLight, CDialog)
CDlgLight::CDlgLight(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLight::IDD, pParent)
{
}

CDlgLight::~CDlgLight()
{
}

void CDlgLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_DIFF_PH, m_ST_DiffusePH);
	DDX_Control(pDX, IDC_ST_SPEC_PH, m_ST_SpecularPH);
	DDX_Control(pDX, IDC_ED_SHADOWMAPSIZE, m_ED_ShadowMapSize);
}


BEGIN_MESSAGE_MAP(CDlgLight, CDialog)
END_MESSAGE_MAP()


// CDlgLight message handlers

BOOL CDlgLight::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rt;

		m_ST_DiffusePH.GetWindowRect( &rt);
		ScreenToClient( &rt);

		m_DiffuseCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
		m_DiffuseCtrl.setColor( g_pConfig->getLightDiffuse());
	}

	{
		CRect rt;

		m_ST_SpecularPH.GetWindowRect( &rt);
		ScreenToClient( &rt);

		m_SpecularCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 101);
		m_SpecularCtrl.setColor( g_pConfig->getLightSpecular());
	}

	{
		char conv[256];

		sprintf( conv, "%d", g_pConfig->getShadowMapSize());
		m_ED_ShadowMapSize.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLight::OnOK()
{
	{
		char conv[256];

		g_pConfig->setLightDiffuse( m_DiffuseCtrl.getColor());
		g_pConfig->setLightSpecular( m_SpecularCtrl.getColor());

		m_ED_ShadowMapSize.GetWindowText( conv, sizeof(conv));
		g_pConfig->setShadowMapSize( atoi( conv));
	}

	CDialog::OnOK();
}

LRESULT CDlgLight::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CDialog::WindowProc(message, wParam, lParam);
}
