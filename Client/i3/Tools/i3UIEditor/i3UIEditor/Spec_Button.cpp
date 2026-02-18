// Spec_Button.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "Spec_Button.h"
#include ".\spec_button.h"


// CSpec_Button dialog

IMPLEMENT_DYNAMIC(CSpec_Button, i3TDKDialogBase)
CSpec_Button::CSpec_Button(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpec_Button::IDD, pParent)
{
	m_pCtrl = NULL;
}

CSpec_Button::~CSpec_Button()
{
}

void CSpec_Button::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_CAPTION, m_ED_Caption);
	DDX_Control(pDX, IDC_ED_FONT, m_ED_Font);
	DDX_Control(pDX, IDC_CHK_SHADOW, m_CHK_Shadow);
	DDX_Control(pDX, IDC_CB_SHADOWQUALITY, m_CB_ShadowQuality);
	DDX_Control(pDX, IDC_ST_COLORPH, m_ST_ColorPH);
	DDX_Control(pDX, IDC_CB_HORZALIGN, m_CB_AlignHorz);
	DDX_Control(pDX, IDC_CB_VERTALIGN, m_CB_AlignVert);
}


BEGIN_MESSAGE_MAP(CSpec_Button, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_ED_CAPTION, OnEnKillfocusEdCaption)
	ON_BN_CLICKED(IDC_BTN_FONT, OnBnClickedBtnFont)
	ON_BN_CLICKED(IDC_CHK_SHADOW, OnBnClickedChkShadow)
	ON_CBN_SELCHANGE(IDC_CB_SHADOWQUALITY, OnCbnSelchangeCbShadowquality)
	ON_CBN_SELCHANGE(IDC_CB_HORZALIGN, OnCbnSelchangeCbHorzalign)
	ON_CBN_SELCHANGE(IDC_CB_VERTALIGN, OnCbnSelchangeCbVertalign)
END_MESSAGE_MAP()

void CSpec_Button::SetObject( i3ElementBase * pObj)
{
	m_pCtrl = (i3UIButton *) pObj;

	i3TDK::EnableChildCtrl( m_hWnd, m_pCtrl != NULL);

	if( m_pCtrl != NULL)
	{
		// Caption
		if( m_pCtrl->getText() != NULL)
			m_ED_Caption.SetWindowText( m_pCtrl->getText());
		else
			m_ED_Caption.SetWindowText( m_pCtrl->getText());

		// Font
		_updateFontInfo();

		// Shadow
		if( m_pCtrl->isTextShadow())
			m_CHK_Shadow.SetCheck( BST_CHECKED);
		else
			m_CHK_Shadow.SetCheck( BST_UNCHECKED);

		// Shadow Quality
		switch( m_pCtrl->getTextShadowQuality())
		{
			case FONT_SHADOW_QUALITY_HIGH :		m_CB_ShadowQuality.SetCurSel( 2);	break;
			case FONT_SHADOW_QUALITY_NORMAL :	m_CB_ShadowQuality.SetCurSel( 1);	break;
			default :							m_CB_ShadowQuality.SetCurSel( 0);	break;
		}
		m_CB_ShadowQuality.EnableWindow( m_pCtrl->isTextShadow());

		// Color
		m_ColorCtrl.setColor( m_pCtrl->getTextColor());

		// Align Horz
		switch( m_pCtrl->getTextAlignX())
		{
			case ALIGN_LEFT :		m_CB_AlignHorz.SetCurSel( 0);	break;
			case ALIGN_CENTER :		m_CB_AlignHorz.SetCurSel( 1);	break;
			case ALIGN_RIGHT :		m_CB_AlignHorz.SetCurSel( 2);	break;
		}

		// Align Vert
		switch( m_pCtrl->getTextAlignY())
		{
			case ALIGN_TOP :		m_CB_AlignVert.SetCurSel( 0);	break;
			case ALIGN_MIDDLE :		m_CB_AlignVert.SetCurSel( 1);	break;
			case ALIGN_BOTTOM :		m_CB_AlignVert.SetCurSel( 2);	break;
		}
	}
	else
	{
		m_ED_Caption.SetWindowText( "");
		m_ED_Font.SetWindowText( "");
	}
}

void CSpec_Button::_updateFontInfo(void)
{
	char conv[512], * pszWeight;

	if( m_pCtrl->getFontWeight() < 300)
	{
		pszWeight = "Normal";
	}
	else
	{
		pszWeight = "Bold";
	}

	sprintf( conv, "%s (%d pt.), %s", m_pCtrl->getFontName(), m_pCtrl->getFontSize(), pszWeight);

	m_ED_Font.SetWindowText( conv);
}

// CSpec_Button message handlers

BOOL CSpec_Button::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		m_ST_ColorPH.GetWindowRect( &rt);

		ScreenToClient( &rt);

		m_ColorCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpec_Button::OnOK()
{
}

void CSpec_Button::OnCancel()
{
}

LRESULT CSpec_Button::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_SETCOLOR)
	{
		if( m_pCtrl != NULL)
		{
			COLOR col;

			i3Color::Set( &col, m_ColorCtrl.getColor());

			m_pCtrl->SetTextColor( &col);
		}
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CSpec_Button::OnEnKillfocusEdCaption()
{
	if( m_pCtrl == NULL)
		return;

	char conv[1024];

	m_ED_Caption.GetWindowText( conv, sizeof(conv));

	m_pCtrl->SetText( conv);
}

void CSpec_Button::OnBnClickedBtnFont()
{
	// Font 교체 버튼
	LOGFONT log;

	i3mem::FillZero( &log, sizeof(log));

	log.lfHeight			= -i3Gfx::FontPointToPixel( m_pCtrl->getFontSize());
	log.lfWeight			= m_pCtrl->getFontWeight();
	log.lfCharSet			= DEFAULT_CHARSET;
	log.lfQuality			= ANTIALIASED_QUALITY;
	log.lfPitchAndFamily	= DEFAULT_PITCH;
	strcpy( log.lfFaceName, m_pCtrl->getFontName());

	CFontDialog dlg( &log);

	if( dlg.DoModal() == IDCANCEL)
		return;

	memcpy( &log, dlg.m_cf.lpLogFont, sizeof(LOGFONT));

	INT32 fontPoint = i3Gfx::FontPixelToPoint( abs( log.lfHeight));

	m_pCtrl->SetFont( log.lfFaceName, fontPoint , log.lfWeight);

	_updateFontInfo();
}

void CSpec_Button::OnBnClickedChkShadow()
{
	// Shadow Enable/Disable
	if( m_pCtrl == NULL)	return;

	bool bEnable;
	FONT_SHADOW_QUALITY		q;

	bEnable = m_CHK_Shadow.GetCheck() == BST_CHECKED;
	
	switch( m_CB_ShadowQuality.GetCurSel())
	{
		case 2 :	q = FONT_SHADOW_QUALITY_HIGH;	break;
		case 1 :	q = FONT_SHADOW_QUALITY_NORMAL;		break;
		default :	q = FONT_SHADOW_QUALITY_LOW;	break;
	}

	m_pCtrl->SetTextShadow( bEnable, q);

	m_CB_ShadowQuality.EnableWindow( bEnable);
}

void CSpec_Button::OnCbnSelchangeCbShadowquality()
{
	if( m_pCtrl == NULL)		return;

	bool bEnable;
	FONT_SHADOW_QUALITY		q;

	bEnable = m_CHK_Shadow.GetCheck() == BST_CHECKED;
	
	switch( m_CB_ShadowQuality.GetCurSel())
	{
		case 2 :	q = FONT_SHADOW_QUALITY_HIGH;	break;
		case 1 :	q = FONT_SHADOW_QUALITY_NORMAL;		break;
		default :	q = FONT_SHADOW_QUALITY_LOW;	break;
	}

	m_pCtrl->SetTextShadow( bEnable, q);
}

void CSpec_Button::OnCbnSelchangeCbHorzalign()
{
	if( m_pCtrl == NULL)
		return;

	TEXT_HORZ_ALIGN alignx = ALIGN_LEFT;
	TEXT_VERT_ALIGN aligny = ALIGN_TOP;

	switch( m_CB_AlignHorz.GetCurSel())
	{
		case 2 :	alignx = ALIGN_RIGHT;	break;
		case 1 :	alignx = ALIGN_CENTER;	break;
	}

	switch( m_CB_AlignVert.GetCurSel())
	{
		case 2 :	aligny = ALIGN_BOTTOM;	break;
		case 1 :	aligny = ALIGN_MIDDLE;	break;
	}

	m_pCtrl->SetTextAlign( alignx, aligny);
}

void CSpec_Button::OnCbnSelchangeCbVertalign()
{
	OnCbnSelchangeCbHorzalign();
}
