// Dlg_Debug_Sg.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Dlg_Debug_Sg.h"


// CDlg_Debug_Sg dialog

IMPLEMENT_DYNAMIC(CDlg_Debug_Sg, CDialog)

CDlg_Debug_Sg::CDlg_Debug_Sg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Debug_Sg::IDD, pParent)
{

}

CDlg_Debug_Sg::~CDlg_Debug_Sg()
{
}

void CDlg_Debug_Sg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_Debug_Sg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlg_Debug_Sg message handlers

BOOL CDlg_Debug_Sg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rt;

		GetClientRect( &rt);

		m_SgCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
	}

	{
		i3LevelFramework * pFramework = (i3LevelFramework *) i3Framework::getCurrentFramework();
		i3SceneGraphContext * pSgCtx = pFramework->getSgContext();
		i3SceneGraphInfo * pInfo;

		pInfo = i3SceneGraphInfo::new_object_ref();

		pInfo->setInstanceSg( pSgCtx->get3DMainRoot());

		m_SgCtrl.setSgInfo( pInfo);
	}

	PlaceControls( -1, -1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Debug_Sg::PlaceControls( INT32 cx, INT32 cy)
{
	if( cx < 0)
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	if( ::IsWindow( m_SgCtrl.m_hWnd))
	{
		m_SgCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


void CDlg_Debug_Sg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	PlaceControls( cx, cy);
}
