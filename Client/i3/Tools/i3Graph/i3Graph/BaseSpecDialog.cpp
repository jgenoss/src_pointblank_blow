// BaseSpecDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "AlchemyEditor.h"
#include "BaseSpecDialog.h"


// CBaseSpecDialog dialog

IMPLEMENT_DYNAMIC(CBaseSpecDialog, CDialog)
CBaseSpecDialog::CBaseSpecDialog(CWnd* pParent /*=NULL*/)
{
	m_bDock = FALSE;
}

CBaseSpecDialog::~CBaseSpecDialog()
{

}

void CBaseSpecDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseSpecDialog, CDialog)
END_MESSAGE_MAP()


// CBaseSpecDialog message handlers
BOOL CBaseSpecDialog::Create( BOOL bDock, CWnd * pParentWnd)
{
	m_bDock = bDock;

	return FALSE;
}

//void CBaseSpecDialog::Refresh( INT32 iRefreshMode, WTCObject * pObject)
void CBaseSpecDialog::Refresh( INT32 iRefreshMode)
{

}

BOOL CBaseSpecDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		UINT removeStyle, addStyle;
		UINT removeExStyle, addExStyle;

		//dwStyle = ;
		//dwExStyle = GetWindowLong( m_hWnd, GWL_EXSTYLE);

		if( m_bDock)
		{
			// Main Window의 어딘가에 Docking되어 나타난다.
			// WS_CHILD를 반드시 가져야 하며, 아무런 Border를 갖지 않도록 한다.
			// 또한 Title Bar 및 System Menu 역시 나타나지 않도록 한다.
			removeStyle = WS_BORDER | WS_CAPTION | WS_DLGFRAME | WS_MAXIMIZE | WS_MAXIMIZEBOX | WS_MINIMIZE | 
							WS_MINIMIZEBOX | WS_OVERLAPPED | WS_POPUP | WS_SYSMENU | WS_THICKFRAME |
							DS_MODALFRAME;

			addStyle = WS_CHILD;

			removeExStyle = WS_EX_DLGMODALFRAME;
			addExStyle = 0;
		}
		else
		{
			removeStyle = WS_CHILD;
			addStyle = WS_CAPTION | WS_POPUP;

			removeExStyle = 0;
			addExStyle = WS_EX_TOOLWINDOW;
		}

		ModifyStyle( removeStyle, addStyle);
		ModifyStyleEx( m_hWnd, removeExStyle, addExStyle, 0);
		//SetWindowLong( m_hWnd, GWL_STYLE, dwStyle);
		//SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle);

		//TRACE( "%d, %08X\n", m_bDock, GetWindowLong( m_hWnd, GWL_STYLE));

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
