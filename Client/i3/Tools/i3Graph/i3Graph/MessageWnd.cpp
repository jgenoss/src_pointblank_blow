// MessageWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "MessageWnd.h"
#include ".\messagewnd.h"
#include "i3GraphDlg.h"

// CMessageWnd dialog

IMPLEMENT_DYNAMIC(CMessageWnd, CDialog)
CMessageWnd::CMessageWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageWnd::IDD, pParent)
{
}

CMessageWnd::~CMessageWnd()
{
}

void CMessageWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListControl);
}


BEGIN_MESSAGE_MAP(CMessageWnd, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMessageWnd message handlers

void CMessageWnd::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
//	OnOK();
}

void CMessageWnd::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
//	OnCancel();
}

BOOL CMessageWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect; 

	//Set Size
	GetClientRect(&rect); 
	m_ListControl.MoveWindow(&rect, TRUE); 

	{
		LVCOLUMN col;

		//에러 종류
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.pszText = "#Log Level";
		col.cx = 130;
		m_ListControl.InsertColumn( 0, &col);

		//에러 내용
		col.pszText = "#Contents ";
		col.cx = 1000;
		m_ListControl.InsertColumn( 1, &col);
	}

	SetMessage(I3GRAP_LOG_NORMAL, " Start i3Graph Program ");

	return TRUE; 
}

void CMessageWnd::SetMessage(INT32 LogLevel,  char * pString)
{
	INT32	Idx; 

	//INT32 Count = m_ListControl.GetItemCount(); 
	INT32 Count = 0; // 맨앞에다가 추가하기....
	
	switch(LogLevel)
	{
	case I3GRAP_LOG_NORMAL		: Idx = m_ListControl.InsertItem(Count, "Message", 0);	break; 
	case I3GRAP_LOG_WARNING		: Idx = m_ListControl.InsertItem(Count, "Warning", 0);	break; 
	case I3GRAP_LOG_ERROR		: Idx = m_ListControl.InsertItem(Count, "Error", 0);	break; 
	default						: 
		Idx = m_ListControl.InsertItem(Count, "Warning", 0);	
		strcat(pString, " -- 프로그램 메시지 에러입니다."); 
		break; 
	}	

	UpdateLog(Idx, pString); 
}

void CMessageWnd::UpdateLog(INT32 Idx, char * pstring)
{
	//char conv[100];
	m_ListControl.SetItem( Idx, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0, 0);
	m_ListControl.SetItemText( Idx, 1, pstring);	
}

void CMessageWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if( IsWindow(m_ListControl.m_hWnd) )
		m_ListControl.MoveWindow(0, 0, cx, cy, TRUE); 
	// TODO: Add your message handler code here
}
