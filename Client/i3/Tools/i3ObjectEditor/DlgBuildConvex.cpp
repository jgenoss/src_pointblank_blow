// DlgBuildConvex.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "DlgBuildConvex.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/ftoa.h"

// CDlgBuildConvex dialog

IMPLEMENT_DYNAMIC(CDlgBuildConvex, CDialog)
CDlgBuildConvex::CDlgBuildConvex(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBuildConvex::IDD, pParent)
{
}

CDlgBuildConvex::~CDlgBuildConvex()
{
}

void CDlgBuildConvex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_GLOBALHULL, m_GlobalHullCtrl);
	DDX_Control(pDX, IDC_CHK_DYNAMIC, m_DynamicCtrl);
	DDX_Control(pDX, IDC_ED_ELASTICITY, m_ElasticityCtrl);
	DDX_Control(pDX, IDC_ED_FRICTION, m_FrictionCtrl);
}


BEGIN_MESSAGE_MAP(CDlgBuildConvex, CDialog)
END_MESSAGE_MAP()


// CDlgBuildConvex message handlers

BOOL CDlgBuildConvex::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		if( g_pOption->m_bGlobalHull)
			m_GlobalHullCtrl.SetCheck( BST_CHECKED);
		else
			m_GlobalHullCtrl.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bDynamicHull)
			m_DynamicCtrl.SetCheck( BST_CHECKED);
		else
			m_DynamicCtrl.SetCheck( BST_UNCHECKED);

		char conv[256];

		i3::ftoa( g_pOption->m_Elasticity, conv);
		m_ElasticityCtrl.SetWindowText( conv);

		i3::ftoa( g_pOption->m_Friction, conv);
		m_FrictionCtrl.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBuildConvex::OnOK()
{
	g_pOption->m_bGlobalHull	= (m_GlobalHullCtrl.GetCheck() == BST_CHECKED);
	g_pOption->m_bDynamicHull	= (m_GlobalHullCtrl.GetCheck() == BST_CHECKED);

	char conv[256];
	m_ElasticityCtrl.GetWindowText( conv, sizeof(conv) - 1);
	g_pOption->m_Elasticity		= (REAL32) atof( conv);

	m_FrictionCtrl.GetWindowText( conv, sizeof(conv) - 1);
	g_pOption->m_Friction		= (REAL32) atof( conv);

	CDialog::OnOK();
}
