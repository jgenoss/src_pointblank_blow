// ColoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "ColoDlg.h"
#include ".\colodlg.h"


// CColoDlg dialog

IMPLEMENT_DYNAMIC(CColorDlg, CDialog)
CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorDlg::IDD, pParent)
{
	m_ColorA = 0;
	m_ColorB = 0;
	m_ColorG = 0;
	m_ColorR = 0;
}

CColorDlg::~CColorDlg()
{
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_COLOR_A, m_ColorA);
	DDX_Text(pDX, IDC_EDIT_COLOR_B, m_ColorB);
	DDX_Text(pDX, IDC_EDIT_COLOR_G, m_ColorG);
	DDX_Text(pDX, IDC_EDIT_COLOR_R, m_ColorR);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_ColorStatic);
}


BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
	ON_BN_CLICKED(IDSELECT, OnBnClickedSelect)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_COLOR_R, OnEnChangeEditColorR)
	ON_EN_CHANGE(IDC_EDIT_COLOR_G, OnEnChangeEditColorG)
	ON_EN_CHANGE(IDC_EDIT_COLOR_B, OnEnChangeEditColorB)
	ON_EN_CHANGE(IDC_EDIT_COLOR_A, OnEnChangeEditColorA)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CColoDlg message handlers

void CColorDlg::OnBnClickedSelect()
{
	// TODO: Add your control notification handler code here
	CColorDialog Dlg( RGB( m_ColorR, m_ColorG, m_ColorB), 0, this);

	if( Dlg.DoModal() == IDOK)
	{
		COLORREF Color = Dlg.GetColor();
		m_ColorR = GetRValue( Color);
		m_ColorG = GetGValue( Color);
		m_ColorB = GetBValue( Color);
		UpdateData(FALSE);		
		Invalidate(TRUE);
	}
}

void CColorDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CColorDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CColorDlg::OnEnChangeEditColorR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_ColorR < 0 || m_ColorR > 255)
	{
		MessageBox("0 ~	255 정수를 넣어 주세요");   
		m_ColorR = 0; 
		UpdateData(FALSE);	
	}
	Invalidate(TRUE);

	// TODO:  Add your control notification handler code here
}

void CColorDlg::OnEnChangeEditColorG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_ColorG < 0 || m_ColorG > 255)
	{
		MessageBox("0 ~	255 정수를 넣어 주세요");   
		m_ColorG = 0; 
		UpdateData(FALSE);	
	}
	Invalidate(TRUE);

	// TODO:  Add your control notification handler code here
}

void CColorDlg::OnEnChangeEditColorB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_ColorB < 0 || m_ColorB > 255)
	{
		MessageBox("0 ~	255 정수를 넣어 주세요");   
		m_ColorB = 0; 
		UpdateData(FALSE);	
	}
	Invalidate(TRUE);

	// TODO:  Add your control notification handler code here
}

void CColorDlg::OnEnChangeEditColorA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_ColorA < 0 || m_ColorA > 255)
	{
		MessageBox("0 ~	255 정수를 넣어 주세요");   
		m_ColorA = 0; 
		UpdateData(FALSE);	
	}
	Invalidate(TRUE);

	// TODO:  Add your control notification handler code here
}

void CColorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CRect Rect;
	COLORREF Color;	
	HBRUSH OldBrush, NewBrush;

	Color = RGB( m_ColorR, m_ColorG, m_ColorB);
	NewBrush = CreateSolidBrush(Color);
	
	ASSERT(NewBrush);

	OldBrush = (HBRUSH) dc.SelectObject(NewBrush);

	m_ColorStatic.GetWindowRect( &Rect);
	ScreenToClient( &Rect);
	dc.Rectangle(&Rect);
	dc.SelectObject(OldBrush);
	DeleteObject(NewBrush);
}
