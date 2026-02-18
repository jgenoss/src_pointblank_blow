// i3ClutTableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3ImageViewer.h"
#include "i3ClutTableDlg.h"
#include ".\i3cluttabledlg.h"


INT32	WndPosX	= 28;
INT32	WndPosY	= 17;
INT32	nLength = 16;
// i3ClutTableDlg dialog

IMPLEMENT_DYNAMIC(i3ClutTableDlg, CDialog)
i3ClutTableDlg::i3ClutTableDlg(CWnd* pParent /*=NULL*/)
	: CDialog(i3ClutTableDlg::IDD, pParent)
	, m_CtrlRed(0)
	, m_CtrlGreen(0)
	, m_CtrlBlue(0)
	, m_CtrlAlpha(0)
{
	m_pClut = NULL;
	m_CtrlRed	= 0;
	m_CtrlGreen = 0;
	m_CtrlBlue	= 0;
	m_CtrlAlpha = 0;

	m_rtClutRegion.SetRect( WndPosX, WndPosY, WndPosX + nLength *16, WndPosY + nLength * 16 );
}

i3ClutTableDlg::~i3ClutTableDlg()
{
}

void i3ClutTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RED, m_CtrlRed);
	DDV_MinMaxUInt(pDX, m_CtrlRed, 0, 255);
	DDX_Text(pDX, IDC_EDIT_GREEN, m_CtrlGreen);
	DDV_MinMaxUInt(pDX, m_CtrlGreen, 0, 255);
	DDX_Text(pDX, IDC_EDIT_BLUE, m_CtrlBlue);
	DDV_MinMaxUInt(pDX, m_CtrlBlue, 0, 255);
	DDX_Text(pDX, IDC_EDIT_ALPHA, m_CtrlAlpha);
	DDV_MinMaxUInt(pDX, m_CtrlAlpha, 0, 255);
}


BEGIN_MESSAGE_MAP(i3ClutTableDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// i3ClutTableDlg message handlers

void i3ClutTableDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	if( m_pClut == NULL )	return;

	//INT32	WndPosX	= 28;
	//INT32	WndPosY	= 17;
	//INT32	nLength = 16;

	INT32	CurPosX = WndPosX;
	INT32	CurPosY = WndPosY;

	INT32	nColorCount = m_pClut->GetColorCount();
	for( INT32	i = 0 ; i < nColorCount ; ++i )
	{
		if( i != 0 )
		if( i%16 == 0 )
		{
			CurPosX =	WndPosX;
			CurPosY +=	nLength;
		}

		CBrush MyBrush,*hOldBrush;

		I3COLOR Col;

		m_pClut->GetColor( i , &Col );

		MyBrush.CreateSolidBrush( RGB( i3Color::GetR(&Col), i3Color::GetG(&Col), i3Color::GetB(&Col))); 
		hOldBrush = dc.SelectObject(&MyBrush );

		dc.Rectangle( CurPosX, CurPosY, CurPosX + nLength, CurPosY + nLength);

		dc.SelectObject(hOldBrush);
		MyBrush.DeleteObject();

		MyBrush.CreateSolidBrush( RGB( i3Color::GetA(&Col), i3Color::GetA(&Col), i3Color::GetA(&Col))); 
		hOldBrush = dc.SelectObject(&MyBrush );

		dc.Rectangle( CurPosX, CurPosY + nLength - 6, CurPosX + nLength, CurPosY + nLength);
		CurPosX += nLength;

		dc.SelectObject(hOldBrush);
		MyBrush.DeleteObject();
	}
}

void i3ClutTableDlg::SetClut( i3Clut * pClut )
{
	m_pClut = pClut;
}

void i3ClutTableDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_OldMousePos.x == point.x && m_OldMousePos.y == point.y )	return;

	if( m_rtClutRegion.PtInRect( point ) )	SetColorValue( point );
	m_OldMousePos = point;//이전위치 저장

	CDialog::OnMouseMove(nFlags, point);
}

void i3ClutTableDlg::SetColorValue( CPoint point )
{
	if( m_pClut == NULL )	return;

	INT32	CurPosX = (INT32)(point.x - WndPosX)/nLength;
	INT32	CurPosY = (INT32)(point.y - WndPosY)/nLength;

	I3COLOR COL;
	m_pClut->GetColor( CurPosY * 16 + CurPosX , &COL );

	SetDlgItemInt( IDC_EDIT_RED		, i3Color::GetR( &COL ));
	SetDlgItemInt( IDC_EDIT_GREEN	, i3Color::GetG( &COL ));
	SetDlgItemInt( IDC_EDIT_BLUE	, i3Color::GetB( &COL ));
	SetDlgItemInt( IDC_EDIT_ALPHA	, i3Color::GetA( &COL ));

	CBrush MyBrush,*hOldBrush;
	CDC * pDC = this->GetDC();

	MyBrush.CreateSolidBrush( RGB( i3Color::GetR(&COL), i3Color::GetG(&COL), i3Color::GetB(&COL))); 
	hOldBrush = pDC->SelectObject(&MyBrush );

	pDC->Rectangle(28 , 280 , 78 , 316);

	char szColorIndex[32];
	sprintf( szColorIndex , "%d" , CurPosY * 16 + CurPosX );
	INT32	ColorLevel = 255 - (((i3Color::GetR(&COL)*30) + (i3Color::GetG(&COL)*59) + (i3Color::GetB(&COL)*11)) /100);
	if( ColorLevel > 128 )	if( ColorLevel <=200 ) ColorLevel = 200;
	if( ColorLevel < 128 )	if( ColorLevel > 50 ) ColorLevel = 50;

	pDC->SetBkColor( RGB( i3Color::GetR(&COL), i3Color::GetG(&COL), i3Color::GetB(&COL)) );
	pDC->SetTextColor( RGB(ColorLevel,ColorLevel,ColorLevel) );
	pDC->TextOut( 29 , 281 , szColorIndex );
	pDC->SelectObject(hOldBrush);
	MyBrush.DeleteObject();
}

void i3ClutTableDlg::SetColorValue( UINT8	nIndex )
{
	if( m_pClut == NULL )	return;

	I3COLOR COL;
	m_pClut->GetColor( nIndex , &COL );

	SetDlgItemInt( IDC_EDIT_RED		, i3Color::GetR( &COL ));
	SetDlgItemInt( IDC_EDIT_GREEN	, i3Color::GetG( &COL ));
	SetDlgItemInt( IDC_EDIT_BLUE	, i3Color::GetB( &COL ));
	SetDlgItemInt( IDC_EDIT_ALPHA	, i3Color::GetA( &COL ));

	CDC * pDC = this->GetDC();
	///////index rectangle redraw
	pDC->SelectStockObject( BLACK_PEN );
	pDC->SelectStockObject( NULL_BRUSH );
	INT32	nIndexX = m_OldIndex%16;
	INT32	nIndexY = (INT32)(m_OldIndex/16);

	pDC->Rectangle( WndPosX + nIndexX * nLength, WndPosY + nIndexY * nLength,
					WndPosX + nIndexX * nLength +16 ,WndPosY + nIndexY * nLength + 16);

	nIndexX = nIndex%16;
	nIndexY = (INT32)(nIndex/16);

	pDC->SelectStockObject( WHITE_PEN );
	pDC->SelectStockObject( NULL_BRUSH );

	pDC->Rectangle( WndPosX + nIndexX * nLength, WndPosY + nIndexY * nLength,
					WndPosX + nIndexX * nLength +16 ,WndPosY + nIndexY * nLength + 16);

	///
	CBrush MyBrush,*hOldBrush;

	MyBrush.CreateSolidBrush( RGB( i3Color::GetR(&COL), i3Color::GetG(&COL), i3Color::GetB(&COL))); 
	hOldBrush = pDC->SelectObject(&MyBrush );

	pDC->Rectangle(28 , 280 , 78 , 316);

	//////////index text out
	char szColorIndex[32];
	sprintf( szColorIndex , "%d" , nIndex );
	INT32	ColorLevel = 255 - (((i3Color::GetR(&COL)*30) + (i3Color::GetG(&COL)*59) + (i3Color::GetB(&COL)*11)) /100);
	if( ColorLevel > 128 )	if( ColorLevel <=200 ) ColorLevel = 200;
	if( ColorLevel < 128 )	if( ColorLevel > 50 ) ColorLevel = 50;

	pDC->SetBkColor( RGB( i3Color::GetR(&COL), i3Color::GetG(&COL), i3Color::GetB(&COL)) );
	pDC->SetTextColor( RGB( ColorLevel, ColorLevel, ColorLevel) );

	pDC->TextOut( 29 , 281 , szColorIndex );
	pDC->SelectObject(hOldBrush);
	MyBrush.DeleteObject();

	m_OldIndex = nIndex;
}