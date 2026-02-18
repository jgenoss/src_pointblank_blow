// SingleSelectControlTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "SingleSelectControlTypeDialog.h"


// CSingleSelectControlTypeDialog dialog

IMPLEMENT_DYNAMIC(CSingleSelectControlTypeDialog, CDialog)
CSingleSelectControlTypeDialog::CSingleSelectControlTypeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSingleSelectControlTypeDialog::IDD, pParent)
{
	m_pMeta = NULL;
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
		INT32	i;
		i3List	List;
		i3ClassMeta * pMeta = NULL;

		if( m_pMeta != NULL)
		{
			m_pMeta->GetAllDerivedClasses(&List, true);

			for( i = 0; i < List.GetCount(); i++)
			{
				pMeta = (i3ClassMeta*)List.Items[i];

				m_TypeCtrl.AddString( pMeta->GetClassName() + 4);
			}
		}
	}

	if( m_pMeta != NULL)
	{
		if( m_pMeta->IsTypeOf( i3UIControl::GetClassMeta()))
		{
			SetWindowText( "货 牧飘费 积己");
		}
		else if( m_pMeta->IsTypeOf( i3UITemplate::GetClassMeta()))
		{
			SetWindowText("货 袍敲复 积己");
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSingleSelectControlTypeDialog::OnOK()
{
	char conv[MAX_PATH];

	m_TypeCtrl.GetWindowText( conv, sizeof( conv) -1);

	sprintf( m_szSelectedMeta, "i3UI%s", conv);

	CDialog::OnOK();
}
