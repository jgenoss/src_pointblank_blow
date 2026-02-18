// PropText.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "PropText.h"
#include ".\proptext.h"


// PropText dialog

IMPLEMENT_DYNAMIC(PropText, CDialog)
PropText::PropText(CWnd* pParent /*=NULL*/)
	: CDialog(PropText::IDD, pParent)
{
}

PropText::~PropText()
{
}

void PropText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONT_DEFAULTSTRING, m_edDefaultString);
	DDX_Control(pDX, IDC_FONT_TYPE, m_edFontType);
	DDX_Control(pDX, IDC_FONT_MAXSIZE, m_edMaxStringLength);
	DDX_Control(pDX, IDC_FONT_SPACE, m_edTextSpace);
	DDX_Control(pDX, IDC_FONT_ALLOWINPUT, m_cbAllowInput);
	DDX_Control(pDX, IDC_FONT_MULTILINE, m_cbMultiLineText);
	DDX_Control(pDX, IDC_FONT_SHADOWENABLE, m_cbShadowEnable);
	DDX_Control(pDX, IDC_FONT_SIZE, m_cbbFontSize);
	DDX_Control(pDX, IDC_FONT_WEIGHT, m_cbbFontWeight);
	DDX_Control(pDX, IDC_FONT_ALIGNX, m_cbbAlignX);
	DDX_Control(pDX, IDC_FONT_ALIGNY, m_cbbAlignY);
	DDX_Control(pDX, IDC_FONT_SHADOWQUALITY, m_cbbShadowQuality);
	DDX_Control(pDX, IDC_FONT_OFFSET_X, m_edOffsetX);
	DDX_Control(pDX, IDC_FONT_OFFSETY, m_edOffsetY);
	DDX_Control(pDX, IDC_FONT_COLOR, m_btnFontColor);
}


BEGIN_MESSAGE_MAP(PropText, CDialog)
	ON_EN_KILLFOCUS(IDC_FONT_DEFAULTSTRING, OnEnKillfocusFontDefaultstring)
	ON_EN_CHANGE(IDC_FONT_DEFAULTSTRING, OnEnChangeFontDefaultstring)
END_MESSAGE_MAP()


// PropText message handlers

BOOL PropText::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetTextInfo( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	PropText::SetTextInfo( i3UIControl * pControl)
{
	if( pControl == m_pControl)
		return;

	BOOL bEnable = TRUE;

	if( pControl != NULL)
	{
		if( pControl->IsTypeOf( i3UIEditBox::GetClassMeta()) == FALSE) bEnable = FALSE;
		if( bEnable)
		{
			i3UIEditBox * pEdit = (i3UIEditBox*)pControl;

			if( pEdit->getTextNodeDX() == NULL)
			{
				//새로 만들까..말까..;;
				bEnable = FALSE;
			}
		}
	}
	else
	{
		bEnable = FALSE;
	}

    {
		m_edDefaultString.EnableWindow( bEnable);
		m_edFontType.EnableWindow( bEnable);
		m_edMaxStringLength.EnableWindow( bEnable);
		m_edTextSpace.EnableWindow( bEnable);
		m_cbAllowInput.EnableWindow( bEnable);
		m_cbMultiLineText.EnableWindow( bEnable);
		m_cbShadowEnable.EnableWindow( bEnable);
		m_cbbFontSize.EnableWindow( bEnable);
		m_cbbFontWeight.EnableWindow( bEnable);
		m_cbbAlignX.EnableWindow( bEnable);
		m_cbbAlignY.EnableWindow( bEnable);
		m_cbbShadowQuality.EnableWindow( bEnable);
		m_edOffsetX.EnableWindow( bEnable);
		m_edOffsetY.EnableWindow( bEnable);
		m_btnFontColor.EnableWindow( bEnable);
	}

	m_pControl = pControl;

	if( bEnable == FALSE)
	{
		m_pControl = NULL;	//연결 끊어버림!!
		return;
	}
	
	{
		i3UIEditBox * pEditBox = (i3UIEditBox*)m_pControl;
		if( pEditBox->getTextNodeDX() == NULL)
			return;

		char conv[MAX_PATH];

		if( pEditBox->getText() == NULL)
			sprintf( conv, "None");
		else
			sprintf( conv, "%s", pEditBox->getText());

		m_edDefaultString.SetWindowText( conv);
	}
}

void PropText::OnEnKillfocusFontDefaultstring()
{
	// TODO: Add your control notification handler code here
}

void PropText::OnOK()
{
}

void PropText::OnCancel()
{
}

void PropText::OnEnChangeFontDefaultstring()
{
	if( m_pControl != NULL)
	{
		char conv[MAX_PATH];
		i3UIEditBox * pEditBox	= (i3UIEditBox*)m_pControl;

		m_edDefaultString.GetWindowText( conv, MAX_PATH - 1);

		pEditBox->SetText( conv);
	}
}
