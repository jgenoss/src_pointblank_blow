// Spec_EditBox.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "Spec_EditBox.h"
#include ".\spec_editbox.h"


// CSpec_EditBox dialog

IMPLEMENT_DYNAMIC(CSpec_EditBox, i3TDKDialogBase)
CSpec_EditBox::CSpec_EditBox(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpec_EditBox::IDD, pParent)
{
	m_pCtrl = NULL;
}

CSpec_EditBox::~CSpec_EditBox()
{
}

void CSpec_EditBox::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_TXT, m_ED_Text);
	DDX_Control(pDX, IDC_ST_COLORPH, m_ST_ColorPH);
	DDX_Control(pDX, IDC_ED_FONTINFO, m_ED_Font);
	DDX_Control(pDX, IDC_CHK_READONLY, m_CHK_ReadOnly);
	DDX_Control(pDX, IDC_CHK_SHADOW, m_CHK_Shadow);
	DDX_Control(pDX, IDC_ST_MAXTEXT, m_ST_MaxLength);
	DDX_Control(pDX, IDC_ED_MAXTEXT, m_ED_MaxLength);
	DDX_Control(pDX, IDC_ST_SHADOWQUALITY, m_ST_ShadowQuality);
	DDX_Control(pDX, IDC_CB_SHADOWQUALITY, m_CB_ShadowQuality);
	DDX_Control(pDX, IDC_CHK_PASSWORD, m_CHK_Password);
	DDX_Control(pDX, IDC_CHK_AUTOWRAP, m_CHK_AutoWrap);
	DDX_Control(pDX, IDC_CHK_NOBORDER, m_CHK_NoBorder);
}

void CSpec_EditBox::_updateFontInfo(void)
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

void CSpec_EditBox::SetObject( i3ElementBase * pObj)
{
	char conv[256];

	I3ASSERT( pObj->IsTypeOf( i3UIEditBox::GetClassMeta()));
	m_pObject = pObj;

	m_pCtrl = (i3UIEditBox *) pObj;

	if( m_pCtrl != NULL)
	{
		i3TDK::EnableChildCtrl( m_hWnd, TRUE);

		// Text
		m_ED_Text.SetWindowText( m_pCtrl->getText());
		
		// Color
		m_ColorCtrl.setColor( m_pCtrl->getTextColor());

		// Font
		_updateFontInfo();

		// Read-Only
		if( m_pCtrl->isReadOnly())
			m_CHK_ReadOnly.SetCheck( BST_CHECKED);
		else
			m_CHK_ReadOnly.SetCheck( BST_UNCHECKED);

		// Max-Text Length
		{
			m_ST_MaxLength.EnableWindow( ! m_pCtrl->getPasswordEnable());
			m_ED_MaxLength.EnableWindow( ! m_pCtrl->getPasswordEnable());

			sprintf( conv, "%d", m_pCtrl->getMaxTextCount());

			m_ED_MaxLength.SetWindowText( conv);
		}

		// Shadow
		if( m_pCtrl->isTextShadow())
			m_CHK_Shadow.SetCheck( BST_CHECKED);
		else
			m_CHK_Shadow.SetCheck( BST_UNCHECKED);

		// Shadow Quality
		{
			m_ST_ShadowQuality.EnableWindow( m_pCtrl->isTextShadow());
			m_CB_ShadowQuality.EnableWindow( m_pCtrl->isTextShadow());

			switch( m_pCtrl->getTextShadowQuality())
			{
				case FONT_SHADOW_QUALITY_LOW :		m_CB_ShadowQuality.SetCurSel( 0);	break;
				case FONT_SHADOW_QUALITY_NORMAL :	m_CB_ShadowQuality.SetCurSel( 1);	break;
				case FONT_SHADOW_QUALITY_HIGH :		m_CB_ShadowQuality.SetCurSel( 2);	break;
			}
		}

		// Password
		if( m_pCtrl->getPasswordEnable())
			m_CHK_Password.SetCheck( BST_CHECKED);
		else
			m_CHK_Password.SetCheck( BST_UNCHECKED);

		// Auto Wrap
		if( m_pCtrl->isTextAutoWrap())
			m_CHK_AutoWrap.SetCheck( BST_CHECKED);
		else
			m_CHK_AutoWrap.SetCheck( BST_UNCHECKED);

		// Border
		if( m_pCtrl->isBorderVisible())
			m_CHK_NoBorder.SetCheck( BST_UNCHECKED);
		else
			m_CHK_NoBorder.SetCheck( BST_CHECKED);
	}
	else
	{
		m_ED_Text.SetWindowText( "");

		i3TDK::EnableChildCtrl( m_hWnd, FALSE);
	}
}

BEGIN_MESSAGE_MAP(CSpec_EditBox, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BTN_FONT, OnBnClickedBtnFont)
	ON_CBN_SELCHANGE(IDC_CB_SHADOWQUALITY, OnCbnSelchangeCbShadowquality)
	ON_BN_CLICKED(IDC_CHK_READONLY, OnBnClickedChkReadonly)
	ON_BN_CLICKED(IDC_CHK_SHADOW, OnBnClickedChkShadow)
	ON_BN_CLICKED(IDC_CHK_AUTOWRAP, OnBnClickedChkAutowrap)
	ON_BN_CLICKED(IDC_CHK_PASSWORD, OnBnClickedChkPassword)
	ON_EN_KILLFOCUS(IDC_ED_MAXTEXT, OnEnKillfocusEdMaxtext)
	ON_EN_KILLFOCUS(IDC_ED_TXT, OnEnKillfocusEdTxt)
	ON_BN_CLICKED(IDC_CHK_NOBORDER, OnBnClickedChkNoborder)
END_MESSAGE_MAP()


// CSpec_EditBox message handlers

void CSpec_EditBox::OnOK()
{
}

void CSpec_EditBox::OnCancel()
{
}

BOOL CSpec_EditBox::OnInitDialog()
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

LRESULT CSpec_EditBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void CSpec_EditBox::OnEnKillfocusEdTxt()
{
	char conv[4096];

	if( m_pCtrl == NULL)
		return;

	m_ED_Text.GetWindowText( conv, sizeof(conv) - 1);

	INT32 len = strlen( conv);

	{
		if( m_pCtrl->getMaxTextCount() < len)
		{
			m_pCtrl->SetMaxTextCount( len);
		}

		m_pCtrl->SetText( conv);
	}
}

void CSpec_EditBox::OnBnClickedBtnFont()
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

void CSpec_EditBox::OnBnClickedChkReadonly()
{
	if( m_pCtrl == NULL)	return;

	bool bReadOnly = m_CHK_ReadOnly.GetCheck() == BST_CHECKED;

	m_pCtrl->SetReadOnly( bReadOnly);

	m_ST_MaxLength.EnableWindow( !bReadOnly);
	m_ED_MaxLength.EnableWindow( !bReadOnly);

	if( bReadOnly == false)
	{
		// ReadOnly가 아닐 때에는 사용자가 입력할 수 있는 길이의 제한을 설정해 주어야 한다.
		char conv[256];

		m_ED_MaxLength.GetWindowText( conv, sizeof(conv));

		INT32 cnt = atoi( conv);

		if( cnt > 0)
		{
			cnt = atoi( conv);
		}
		else
		{
			cnt = 256;

			sprintf( conv, "%d", cnt);
			m_ED_MaxLength.SetWindowText( conv);
		}

		m_pCtrl->SetMaxTextCount( cnt);
	}
}

void CSpec_EditBox::OnBnClickedChkShadow()
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

	m_ST_ShadowQuality.EnableWindow( bEnable);
	m_CB_ShadowQuality.EnableWindow( bEnable);
}

void CSpec_EditBox::OnCbnSelchangeCbShadowquality()
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


void CSpec_EditBox::OnBnClickedChkAutowrap()
{
	// Auto Wrap Enable/Disable
	if( m_pCtrl == NULL)		return;

	m_pCtrl->SetTextAutoWrap( m_CHK_AutoWrap.GetCheck() == BST_CHECKED);
}

void CSpec_EditBox::OnBnClickedChkPassword()
{
	// Password Enable/Disable
	if( m_pCtrl == NULL)		return;

	m_pCtrl->setPasswordEnable( m_CHK_Password.GetCheck() == BST_CHECKED);
}

void CSpec_EditBox::OnEnKillfocusEdMaxtext()
{
	// Max Text Length 설정.
	if( m_pCtrl == NULL)
		return;

	char conv[256];

	m_ED_MaxLength.GetWindowText( conv, sizeof(conv));

	INT32 cnt = atoi( conv);

	if( cnt <= 0)
	{
		m_ED_MaxLength.SetWindowText( "100");

		cnt = 100;
	}

	m_pCtrl->SetMaxTextCount( cnt);
}


void CSpec_EditBox::OnBnClickedChkNoborder()
{
	// 테두리 보이기 설정
	if( m_pCtrl == NULL)
		return;

	bool bBorder = m_CHK_NoBorder.GetCheck() == BST_UNCHECKED;

	m_pCtrl->setBorderVisible( bBorder);
}
