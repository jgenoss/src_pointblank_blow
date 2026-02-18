// PanelLog.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "PanelLog.h"
#include ".\panellog.h"


// CPanelLog dialog

IMPLEMENT_DYNAMIC(CPanelLog, i3TDKDialogBase)
CPanelLog::CPanelLog(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelLog::IDD, pParent)
{
}

CPanelLog::~CPanelLog()
{
}

void CPanelLog::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelLog, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LOG, OnLvnItemchangedListLog)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CPanelLog message handlers
BOOL CPanelLog::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();
	

	return TRUE;
}

void CPanelLog::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize( nType, cx, cy);

	if( ::IsWindow( m_LogCtrl.m_hWnd))
	{
        m_LogCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	
	}
}

void CPanelLog::OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	(void) pNMLV;
}

int CPanelLog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	{   
		CRect rt;
		GetClientRect( &rt);
  
		rt.left += 3;
		rt.top += 3;
		rt.right -= 3; 
		rt.bottom -= 3;    

		m_LogCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
	}	
 
	return 0;
}
