// DlgLayer.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Dlg_LoadError.h"

// CDlg_LoadError dialog

IMPLEMENT_DYNAMIC(CDlg_LoadError, CDialog)
CDlg_LoadError::CDlg_LoadError(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_LoadError::IDD, pParent)
{
}

CDlg_LoadError::~CDlg_LoadError()
{
}

void CDlg_LoadError::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ELMLIST, m_ElmListCtrl);
}

bool CDlg_LoadError::Execute( CWnd * pParent, const i3::vector< i3LevelElement3D *> & list)
{
	m_ElmList = list;

	if( DoModal() == IDOK)
		return false;			// 삭제의 경우

	return true;
}

void CDlg_LoadError::UpdateElement( INT32 idx, i3LevelElement3D * pElm)
{
	m_ElmListCtrl.SetItemData( idx, (DWORD_PTR) pElm);

	// Type
	m_ElmListCtrl.SetItemText( idx, 1, pElm->getInstanceClassName());

	// Resource
	m_ElmListCtrl.SetItemText( idx, 2, pElm->getResPath());
}

BEGIN_MESSAGE_MAP(CDlg_LoadError, CDialog)
END_MESSAGE_MAP()


// CDlg_LoadError message handlers

BOOL CDlg_LoadError::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_ElmListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 160);
		m_ElmListCtrl.InsertColumn( 1, "Type", LVCFMT_LEFT, 130);
		m_ElmListCtrl.InsertColumn( 2, "Resource", LVCFMT_LEFT, 300);

		m_ElmListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	for( size_t i = 0; i < m_ElmList.size(); i++)
	{
		i3LevelElement3D * pElm = m_ElmList[i];

		INT32 idx = m_ElmListCtrl.InsertItem( i, pElm->GetName());

		UpdateElement( idx, pElm);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_LoadError::OnOK()
{
	i3LevelScene * pScene = i3Level::GetScene();

	for( UINT32 i = 0; i < m_ElmList.size(); i++)
	{
		i3LevelElement3D * pElm = m_ElmList[i];

		pScene->RemoveElement( pElm);
	}

	CDialog::OnOK();
}

void CDlg_LoadError::OnCancel()
{
	CDialog::OnCancel();
}
