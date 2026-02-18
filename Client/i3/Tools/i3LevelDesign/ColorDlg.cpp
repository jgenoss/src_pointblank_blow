// ColorDlg.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "ColorDlg.h"
#include ".\colordlg.h"


// CColorDlg dialog

IMPLEMENT_DYNAMIC(CColorDlg, CDialog)
CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorDlg::IDD, pParent)
	, m_ColorA(0)
	, m_ColorR(0)
	, m_ColorG(0)
	, m_ColorB(0)
{
}

CColorDlg::~CColorDlg()
{
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_COLOR_A, m_ColorA);
	DDV_MinMaxInt(pDX, m_ColorA, 0, 255);
	DDX_Text(pDX, IDC_EDIT_COLOR_R, m_ColorR);
	DDV_MinMaxInt(pDX, m_ColorR, 0, 255);
	DDX_Text(pDX, IDC_EDIT_COLOR_G, m_ColorG);
	DDV_MinMaxInt(pDX, m_ColorG, 0, 255);
	DDX_Text(pDX, IDC_EDIT_COLOR_B, m_ColorB);
	DDV_MinMaxInt(pDX, m_ColorB, 0, 255);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_ColorStatic);
}


BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_COLSEL, OnBnClickedBtnColsel)
	ON_EN_CHANGE(IDC_EDIT_COLOR_A, OnEnChangeEditColorA)
	ON_EN_CHANGE(IDC_EDIT_COLOR_B, OnEnChangeEditColorB)
	ON_EN_CHANGE(IDC_EDIT_COLOR_G, OnEnChangeEditColorG)
	ON_EN_CHANGE(IDC_EDIT_COLOR_R, OnEnChangeEditColorR)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CColorDlg message handlers

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

void CColorDlg::OnBnClickedBtnColsel()
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

void CColorDlg::OnEnChangeEditColorA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	Invalidate(TRUE);
}

void CColorDlg::OnEnChangeEditColorB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	Invalidate(TRUE);
}

void CColorDlg::OnEnChangeEditColorG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	Invalidate(TRUE);
}

void CColorDlg::OnEnChangeEditColorR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	Invalidate(TRUE);
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

void CColorDlg::SetColor( COLOR * pCol)
{
	m_ColorA = i3Color::GetA( pCol);
	m_ColorR = i3Color::GetR( pCol);
	m_ColorG = i3Color::GetG( pCol);
	m_ColorB = i3Color::GetB( pCol);
}

void CColorDlg::SetColor( COLORREAL * pCol)
{
	m_ColorA = (INT32)(i3Color::GetA( pCol) * 255.0f);
	m_ColorR = (INT32)(i3Color::GetR( pCol)* 255.0f);
	m_ColorG = (INT32)(i3Color::GetG( pCol)* 255.0f);
	m_ColorB = (INT32)(i3Color::GetB( pCol)* 255.0f);
}

void CColorDlg::GetColor( COLOR * pCol)
{
	i3Color::Set( pCol, (UINT8)m_ColorR, (UINT8)m_ColorG, (UINT8)m_ColorB, (UINT8)m_ColorA);
}

void CColorDlg::GetColor( COLORREAL * pCol)
{
	i3Color::Set( pCol, (REAL32)(m_ColorR/255),
						(REAL32)(m_ColorG/255),
						(REAL32)(m_ColorB/255),
						(REAL32)(m_ColorA/255));
}