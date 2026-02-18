// PropertyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "PropertyWnd.h"
#include ".\propertywnd.h"


// CPropertyWnd dialog

IMPLEMENT_DYNAMIC(CPropertyWnd, CDialog)
CPropertyWnd::CPropertyWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertyWnd::IDD, pParent)
{
}

CPropertyWnd::~CPropertyWnd()
{
}

void CPropertyWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CPropertyWnd::OnInitDialog(void)
{
	CDialog::OnInitDialog(); 
	m_uProperty.m_Wnd = this->m_hWnd; 
	m_uProperty.CreateControl(this); 
	m_uProperty.SetBoard(NULL);	

	CRect WndRect; 
	int cx, cy; 

	//Center
	GetWindowRect(WndRect);
	cy = WndRect.Height();
	cx = WndRect.Width(); 	
	m_pCenterx = (INT32)(cx/4) * 3; 


	m_Brect.bottom = m_Brect.left = m_Brect.right = m_Brect.top = 0; 
	m_pCenterBar = FALSE; 
	return TRUE; 
}

BEGIN_MESSAGE_MAP(CPropertyWnd, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CPropertyWnd message handlers

void CPropertyWnd::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_uProperty.Ok();
//	OnOK();
}

void CPropertyWnd::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_uProperty.Cancel();
//	OnCancel();
}

void CPropertyWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here

}

void CPropertyWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CRect WndRect; 
	INT32 cx,cy; 
	GetWindowRect(WndRect);
	cx = WndRect.Width(); 
	cy = WndRect.Height();

	m_uProperty.Draw(&dc, 0, cx, cy, m_pCenterx);
	return; 
}

void CPropertyWnd::SetProperty(i3ElementBase * pBase)
{
	m_uProperty.SetBoard(pBase); 
	Invalidate(TRUE); 
	return; 
}
void CPropertyWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//For Property Center
	if(m_pCenterBar == TRUE)
	{
		//For FocusRect
		CDC * dc; 
		dc = GetDC(); 
		HPEN pen, Oldpen; 

		CRect WndRect; 
		INT32 cx,cy; 
		GetWindowRect(WndRect);
		cx = WndRect.Width(); 
		cy = WndRect.Height();

		RECT rect;
		rect.top = 0; 
		rect.bottom = cy; 
		rect.left = point.x; 
		rect.right = point.x + 2; 			

		pen = CreatePen( PS_SOLID, 1, RGB( 0, 0, 0));
		Oldpen	= (HPEN)dc->SelectObject(pen);
		dc->DrawFocusRect(&m_Brect); 
		dc->DrawFocusRect(&rect); 
		dc->SelectObject(Oldpen);
		DeleteObject(pen);
		m_Brect = rect; 

		ReleaseDC(dc); 
	}

	if(abs(point.x - m_pCenterx) < 3)
	{
		SetCursor( LoadCursor( NULL, IDC_SIZEWE));
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CPropertyWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//PropertyCenter
	if(abs(point.x - m_pCenterx) < 3)
	{
		SetCursor( LoadCursor( NULL, IDC_SIZEWE));
		::SetCapture(this->m_hWnd); 
		m_pCenterBar = TRUE; 
		CDialog::OnLButtonDown(nFlags, point);
		return; 
	}

	//Property Click 
	m_uProperty.MouseDown(point); 

	CDialog::OnLButtonDown(nFlags, point);
}

void CPropertyWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//Property Center Move
	if(m_pCenterBar == TRUE)
	{
		CRect WndRect; 
		int cx; 
        
		GetWindowRect(WndRect);		
		cx = WndRect.Width(); 

		if( (point.x < 1) || (point.x > cx))
		{
			::ReleaseCapture();
			CDialog::OnLButtonUp(nFlags, point);
			return; 
		}
		m_pCenterx = point.x; 
		
		Invalidate(TRUE); 
		m_pCenterBar = FALSE; 
		::ReleaseCapture();

		m_Brect.bottom = m_Brect.left = m_Brect.right = m_Brect.top = 0; 
		CDialog::OnLButtonUp(nFlags, point);
		return; 
	}

	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CPropertyWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL rv; 
	rv = m_uProperty.ActControl(wParam, lParam); 
	if(rv == TRUE) Invalidate(TRUE); 
	return CDialog::OnCommand(wParam, lParam);
}
