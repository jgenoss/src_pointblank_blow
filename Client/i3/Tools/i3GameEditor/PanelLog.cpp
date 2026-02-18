// PanelLog.cpp : implementation file
//

#include "stdafx.h"
#include "i3GameEditor.h"
#include "PanelLog.h"
#include ".\panellog.h"


// PanelLog dialog

IMPLEMENT_DYNAMIC(PanelLog, i3TDKDialogBase)
PanelLog::PanelLog(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(PanelLog::IDD, pParent)
{
}

PanelLog::~PanelLog()
{
}

void PanelLog::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PanelLog, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// PanelLog message handlers

void PanelLog::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_LogCtrl.m_hWnd))
	{
        m_LogCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	
	}
}

int PanelLog::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
