// i3TDK_Dlg_SelectImageExt.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDK_Dlg_SelectImageExt.h"


// i3TDK_Dlg_SelectImageExt dialog

IMPLEMENT_DYNAMIC(i3TDK_Dlg_SelectImageExt, CDialog)

void i3TDK_Dlg_SelectImageExt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_EXT, m_CB_Ext);
}

bool i3TDK_Dlg_SelectImageExt::Execute(void)
{
	i3TDK::SetResInstance();

	INT32 rv = DoModal();

	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL)
		return false;

	return true;
}


BEGIN_MESSAGE_MAP(i3TDK_Dlg_SelectImageExt, CDialog)
END_MESSAGE_MAP()


// i3TDK_Dlg_SelectImageExt message handlers

void i3TDK_Dlg_SelectImageExt::OnOK()
{
	switch( m_CB_Ext.GetCurSel())
	{
		case 1 :	i3::safe_string_copy( m_szExt, _T("TGA"), MAX_PATH);	break;
		default :	i3::safe_string_copy( m_szExt, _T("I3I"), MAX_PATH);	break;
	}

	CDialog::OnOK();
}
