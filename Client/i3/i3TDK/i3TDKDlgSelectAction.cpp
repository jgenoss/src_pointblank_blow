// i3TDKDlgSelectAction.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlgSelectAction.h"
#include ".\i3tdkdlgselectaction.h"


// i3TDKDlgSelectAction dialog

IMPLEMENT_DYNAMIC(i3TDKDlgSelectAction, i3TDKDialogBase)

void i3TDKDlgSelectAction::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_ACTION, m_ActionCtrl);
	DDX_Control(pDX, IDC_TDK_ACTION_HELP, m_HelpCtrl);
}


BEGIN_MESSAGE_MAP(i3TDKDlgSelectAction, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_ACTION, OnCbnSelchangeTdkCbAction)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// i3TDKDlgSelectAction message handlers

BOOL i3TDKDlgSelectAction::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		i3::vector<i3ClassMeta*>	list;
		INT32 idx;
		i3ClassMeta * pMeta;
		i3Action * pAction;
		char szName[256];

		i3Action::static_meta()->get_all_derived_metas( list);

		for(size_t i = 0; i < list.size(); i++)
		{
			pMeta = list[i];

			pAction = (i3Action *) pMeta->create_instance();		//CREATEINSTANCE( pMeta);
			I3ASSERT( pAction != nullptr);
			
			I3_MUST_ADDREF(pAction);

			pAction->GetInfoString( szName, true, _countof(szName) - 1);
#ifdef _UNICODE
			i3::stack_wstring wstrName;	i3::mb_to_utf16(szName, wstrName);
			const wchar_t* pszName = wstrName.c_str();
#else
			const char* pszName = szName;
#endif
			idx = m_ActionCtrl.AddString( pszName);
			
			m_ActionCtrl.SetItemData( idx, (DWORD_PTR) pAction);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlgSelectAction::OnOK()
{
	INT32 idx = m_ActionCtrl.GetCurSel();

	if( idx == -1)
	{
		m_szSelActionName[0] = 0;
	}
	else
	{
		i3Action * pAction = (i3Action *) m_ActionCtrl.GetItemData( idx);

		i3::safe_string_copy( m_szSelActionName, pAction->meta()->class_name().c_str(), MAX_PATH);
	}

	i3TDKDialogBase::OnOK();
}

void i3TDKDlgSelectAction::OnCbnSelchangeTdkCbAction()
{
	INT32 idx = m_ActionCtrl.GetCurSel();

	if( idx == -1)
	{
		m_HelpCtrl.SetWindowText( _T("") );
	}
	else
	{
		char conv[4096];
		i3Action * pAction = (i3Action *) m_ActionCtrl.GetItemData( idx);

		pAction->GetInfoString( conv, false, _countof(conv) - 1);
#ifdef _UNICODE		
		i3::stack_wstring wstrInfo;	i3::mb_to_utf16(conv, wstrInfo);
		const wchar_t* szInfo = wstrInfo.c_str();
#else
		const char* szInfo = conv;
#endif
		m_HelpCtrl.SetWindowText( szInfo);
	}
}

void i3TDKDlgSelectAction::OnDestroy()
{
	INT32 i;
	i3Action * pAction;

	for( i = 0; i < m_ActionCtrl.GetCount(); i++)
	{
		pAction = (i3Action *) m_ActionCtrl.GetItemData( i);

		I3_MUST_RELEASE(pAction);
	}

	i3TDKDialogBase::OnDestroy();
}
