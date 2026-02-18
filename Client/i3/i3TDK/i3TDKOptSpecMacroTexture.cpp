// i3TDKOptSpecMacroTexture.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKOptSpecMacroTexture.h"
#include ".\i3tdkoptspecmacrotexture.h"
#include "i3Base/string/ext/atoi.h"


// i3TDKOptSpecMacroTexture dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecMacroTexture, i3TDKOptSpecBase)

void i3TDKOptSpecMacroTexture::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_WIDTH, m_ED_Width);
	DDX_Control(pDX, IDC_TDK_ED_HEIGHT, m_ED_Height);
	DDX_Control(pDX, IDC_TDK_CHK_DUMPMACROTEX, m_CHK_Dump);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpecMacroTexture, i3TDKOptSpecBase)
END_MESSAGE_MAP()


// i3TDKOptSpecMacroTexture message handlers

BOOL i3TDKOptSpecMacroTexture::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if( m_pOpt != nullptr)
	{
		TCHAR conv[256];

		i3OptMacroTexture * pOpt = (i3OptMacroTexture *) m_pOpt;

		// Width
		i3::snprintf( conv, 256, _T("%d"), pOpt->getMacroWidth());
		m_ED_Width.SetWindowText( conv);

		// Height
		i3::snprintf( conv, 256, _T("%d"), pOpt->getMacroHeight());
		m_ED_Height.SetWindowText( conv);

		// Dump state
		if( pOpt->getDumpState())
			m_CHK_Dump.SetCheck( BST_CHECKED);
		else
			m_CHK_Dump.SetCheck( BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecMacroTexture::OnOK()
{
	if( m_pOpt == nullptr)
	{
		i3TDKOptSpecBase::OnOK();
		return;
	}

	TCHAR conv[256];
	i3OptMacroTexture * pOpt = (i3OptMacroTexture *) m_pOpt;

	// Width
	m_ED_Width.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);
	pOpt->setMacroWidth( i3::atoi( conv));

	// Height
	m_ED_Height.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);
	pOpt->setMacroHeight( i3::atoi( conv));

	// Dump state
	if( m_CHK_Dump.GetCheck() == BST_CHECKED)
		pOpt->setDumpState( true);
	else
		pOpt->setDumpState( false);

	i3TDKOptSpecBase::OnOK();
}
