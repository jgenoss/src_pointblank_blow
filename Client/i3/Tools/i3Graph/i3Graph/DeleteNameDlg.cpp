// DeleteNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "DeleteNameDlg.h"
#include ".\deletenamedlg.h"


// CDeleteNameDlg dialog

IMPLEMENT_DYNAMIC(CDeleteNameDlg, CDialog)
CDeleteNameDlg::CDeleteNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteNameDlg::IDD, pParent)
{
}

CDeleteNameDlg::~CDeleteNameDlg()
{
}

void CDeleteNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NODELIST, m_NodeListCtrl);
}


BEGIN_MESSAGE_MAP(CDeleteNameDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDeleteNameDlg message handlers

BOOL CDeleteNameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	INT32 i; 
	i3ClassMeta * pMeta;
	i3List tempList;

	//Node에 관련된 
	pMeta = i3Node::GetClassMeta();
	pMeta->GetAllDerivedClasses( &tempList);

	{
		m_NodeListCtrl.InsertColumn( 0, "Select", LVCFMT_LEFT, 100);
		m_NodeListCtrl.InsertColumn( 1, "Node Name", LVCFMT_LEFT, 100);
		m_NodeListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	}

	INT32 idx; 
	for( i = 0; i < tempList.GetCount(); i++)
	{		
		pMeta = (i3ClassMeta *) tempList.Items[i];
		idx = m_NodeListCtrl.InsertItem( 0," ", 0);
		m_NodeListCtrl.SetItemData( idx, (DWORD_PTR) pMeta);
		m_NodeListCtrl.SetItemText( idx, 1, pMeta->GetClassName());
	}

	UpdateData( FALSE);
	return TRUE; 

}

void CDeleteNameDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//UpdateData( TRUE );
	INT32 i; 
	i3ClassMeta * pMeta; 

	for( i = 0; i < m_NodeListCtrl.GetItemCount(); i++)
	{
		if(m_NodeListCtrl.GetCheck(i))
		{
			pMeta = (i3ClassMeta * )m_NodeListCtrl.GetItemData(i); 
			m_NodeList.Add( pMeta );
		}
	}
		
	OnOK();
}

void CDeleteNameDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
