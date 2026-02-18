// i3TDKSingleSelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSingleSelectNodeTypeDialog.h"
#include "resource.h"

// i3TDKSingleSelectNodeTypeDialog dialog

IMPLEMENT_DYNAMIC(i3TDKSingleSelectNodeTypeDialog, CDialog)

void i3TDKSingleSelectNodeTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, TDC_CB_TYPE, m_TypeCtrl);
}


BEGIN_MESSAGE_MAP(i3TDKSingleSelectNodeTypeDialog, CDialog)
END_MESSAGE_MAP()


// i3TDKSingleSelectNodeTypeDialog message handlers

BOOL i3TDKSingleSelectNodeTypeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
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
			m_TypeCtrl.AddString( strMeta.c_str());
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSingleSelectNodeTypeDialog::OnOK()
{
	m_TypeCtrl.GetWindowText( m_szSelectedMeta, (sizeof(m_szSelectedMeta) - 1) / sizeof(TCHAR) );

	CDialog::OnOK();
}
