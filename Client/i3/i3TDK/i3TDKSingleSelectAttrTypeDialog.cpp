// i3TDKSingleSelectAttrTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSingleSelectAttrTypeDialog.h"
#include "resource.h"

// i3TDKSingleSelectAttrTypeDialog dialog

IMPLEMENT_DYNAMIC(i3TDKSingleSelectAttrTypeDialog, CDialog)

void i3TDKSingleSelectAttrTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, TDC_CB_TYPE, m_TypeCtrl);
}


BEGIN_MESSAGE_MAP(i3TDKSingleSelectAttrTypeDialog, CDialog)
END_MESSAGE_MAP()


// i3TDKSingleSelectAttrTypeDialog message handlers

BOOL i3TDKSingleSelectAttrTypeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		i3::vector<i3ClassMeta*> List;
		i3ClassMeta * pMeta;

		i3RenderAttr::static_meta()->get_all_derived_metas( List, true);

		for(size_t i = 0; i < List.size(); i++)
		{
			pMeta = List[i];

			if(	pMeta->get_class_meta_style() != i3::cms_abstract)		// (pMeta->IsAbstractClass() == FALSE)
			{
#ifdef _UNICODE
				i3::stack_wstring strMeta;	i3::mb_to_utf16(pMeta->class_name(), strMeta);
#else
				const i3::fixed_string& strMeta = pMeta->class_name();
#endif
				m_TypeCtrl.AddString( strMeta.c_str());
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSingleSelectAttrTypeDialog::OnOK()
{
	m_TypeCtrl.GetWindowText( m_szSelectedMeta, (sizeof(m_szSelectedMeta) - 1) / sizeof(TCHAR));

	CDialog::OnOK();
}
