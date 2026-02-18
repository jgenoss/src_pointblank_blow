// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSelectAttrTypeDialog.h"
#include "resource.h"

// i3TDKSelectAttrTypeDialog dialog

IMPLEMENT_DYNAMIC(i3TDKSelectAttrTypeDialog, CDialog)

void i3TDKSelectAttrTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPELIST, m_TypeListCtrl);
}


BEGIN_MESSAGE_MAP(i3TDKSelectAttrTypeDialog, CDialog)
END_MESSAGE_MAP()


// i3TDKSelectAttrTypeDialog message handlers

BOOL i3TDKSelectAttrTypeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_TypeListCtrl.InsertColumn( 0, _T("Attributes Type"), LVCFMT_LEFT, 180);
		m_TypeListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	}

	{
		INT32 idx;
		i3::vector<i3ClassMeta*>	List;
		i3ClassMeta * pMeta;

		i3RenderAttr::static_meta()->get_all_derived_metas( List, true);

		for(size_t i = 0; i < List.size(); i++)
		{
			pMeta = List[i];

			if ( pMeta->get_class_meta_style() != i3::cms_abstract)	// ( pMeta->IsAbstractClass() == false)
			{
#ifdef _UNICODE
				i3::stack_wstring strMeta;	i3::mb_to_utf16(pMeta->class_name(), strMeta);
#else
				const i3::fixed_string& strMeta = pMeta->class_name();
#endif
				idx = m_TypeListCtrl.InsertItem( 0, strMeta.c_str(), 0);

				m_TypeListCtrl.SetItemData( idx, (DWORD_PTR) pMeta);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSelectAttrTypeDialog::OnOK()
{
	INT32 i;

	m_MetaList.clear();

	for( i = 0; i < m_TypeListCtrl.GetItemCount(); i++)
	{
		if( m_TypeListCtrl.GetCheck(i))
		{
			m_MetaList.push_back(reinterpret_cast<i3ClassMeta*>(m_TypeListCtrl.GetItemData( i)));
		}
	}

	CDialog::OnOK();
}
