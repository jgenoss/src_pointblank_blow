// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSelectNodeTypeDialog.h"
#include "resource.h"

// i3TDKSelectNodeTypeDialog dialog

IMPLEMENT_DYNAMIC(i3TDKSelectNodeTypeDialog, CDialog)

void i3TDKSelectNodeTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPELIST, m_TypeListCtrl);
}


BEGIN_MESSAGE_MAP(i3TDKSelectNodeTypeDialog, CDialog)
END_MESSAGE_MAP()


// i3TDKSelectNodeTypeDialog message handlers

BOOL i3TDKSelectNodeTypeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_TypeListCtrl.InsertColumn( 0, _T("Node Type"), LVCFMT_LEFT, 180);

		m_TypeListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	}

	{
		INT32 idx;
		i3::vector<i3ClassMeta*> List;
		i3ClassMeta * pMeta;

		i3Node::static_meta()->get_all_derived_metas( List, true);

		for(size_t i = 0; i < List.size(); i++)
		{
			pMeta = List[i];
#ifdef _UNICODE
			i3::stack_wstring strMeta;	i3::mb_to_utf16(pMeta->class_name(), strMeta);
#else
			const i3::fixed_string& strMeta = pMeta->class_name();
#endif
			idx = m_TypeListCtrl.InsertItem( 0, strMeta.c_str(), 0);

			m_TypeListCtrl.SetItemData( idx, (DWORD_PTR) pMeta);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSelectNodeTypeDialog::OnOK()
{
	INT32 i;

	m_MetaList.clear();

	for( i = 0; i < m_TypeListCtrl.GetItemCount(); i++)
	{
		if( m_TypeListCtrl.GetCheck(i))
		{
			m_MetaList.push_back( reinterpret_cast<i3ClassMeta*>(m_TypeListCtrl.GetItemData( i)));
		}
	}

	CDialog::OnOK();
}
