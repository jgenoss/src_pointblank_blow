// SingleSelectControlTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "SingleSelectControlTypeDialog.h"
#include ".\singleselectcontroltypedialog.h"


// CSingleSelectControlTypeDialog dialog

IMPLEMENT_DYNAMIC(CSingleSelectControlTypeDialog, CDialog)
CSingleSelectControlTypeDialog::CSingleSelectControlTypeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSingleSelectControlTypeDialog::IDD, pParent)
{
}

CSingleSelectControlTypeDialog::~CSingleSelectControlTypeDialog()
{
}

void CSingleSelectControlTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SELECT_CONTROL, m_TypeCtrl);
}


BEGIN_MESSAGE_MAP(CSingleSelectControlTypeDialog, CDialog)
END_MESSAGE_MAP()


// CSingleSelectControlTypeDialog message handlers

BOOL CSingleSelectControlTypeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	{
		i3::vector<i3ClassMeta*> List;
		i3ClassMeta	* pMeta;

		i3GuiControl::static_meta()->get_all_derived_metas(List, true );

		for( size_t i = 0; i < List.size(); i++)
		{
			pMeta = List[i];

			m_TypeCtrl.AddString( pMeta->class_name().c_str() );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSingleSelectControlTypeDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_TypeCtrl.GetWindowText( m_szSelectedMeta, sizeof( m_szSelectedMeta) -1);

	CDialog::OnOK();
}
