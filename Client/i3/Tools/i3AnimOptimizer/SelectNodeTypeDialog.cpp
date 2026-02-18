// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3AnimOptimizer.h"
#include "SelectNodeTypeDialog.h"
#include ".\selectnodetypedialog.h"


// CSelectNodeTypeDialog dialog

IMPLEMENT_DYNAMIC(CSelectNodeTypeDialog, CDialog)
CSelectNodeTypeDialog::CSelectNodeTypeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectNodeTypeDialog::IDD, pParent)
{
}

CSelectNodeTypeDialog::~CSelectNodeTypeDialog()
{
}

void CSelectNodeTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPELIST, m_TypeListCtrl);
}


BEGIN_MESSAGE_MAP(CSelectNodeTypeDialog, CDialog)
END_MESSAGE_MAP()


// CSelectNodeTypeDialog message handlers

BOOL CSelectNodeTypeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_TypeListCtrl.InsertColumn( 0, "Node Type", LVCFMT_LEFT, 180);
		m_TypeListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	}

	{
		INT32 i, idx;
		i3List List;
		i3ClassMeta * pMeta;

		i3Node::GetClassMeta()->GetAllDerivedClasses( &List, true);

		for( i = 0; i < List.GetCount(); i++)
		{
			pMeta = (i3ClassMeta *) List.Items[i];

			idx = m_TypeListCtrl.InsertItem( 0, pMeta->GetClassName(), 0);

			m_TypeListCtrl.SetItemData( idx, (DWORD_PTR) pMeta);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectNodeTypeDialog::OnOK()
{
	INT32 i;

	m_MetaList.Clear();

	for( i = 0; i < m_TypeListCtrl.GetItemCount(); i++)
	{
		if( m_TypeListCtrl.GetCheck(i))
		{
			m_MetaList.Add( (void *) m_TypeListCtrl.GetItemData( i));
		}
	}

	CDialog::OnOK();
}
