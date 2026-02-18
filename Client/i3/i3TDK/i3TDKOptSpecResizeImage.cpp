// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKOptSpecResizeImage.h"
#include "resource.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"
#include "i3Base/string/ext/atoi.h"
// i3TDKOptSpecResizeImage dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecResizeImage, i3TDKOptSpecBase)

void i3TDKOptSpecResizeImage::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control( pDX, TDC_CB_TYPE, m_TypeCtrl);
	
	DDX_Control( pDX, IDC_ST_SCALE, m_StScaleCtrl);
	DDX_Control( pDX, IDC_TDK_ED_SCALE, m_ScaleFactorCtrl);

	DDX_Control( pDX, IDC_ST_LIMIT, m_StLimitCtrl);
	DDX_Control( pDX, IDC_ST_MAX,	m_StMaxCtrl);
	DDX_Control( pDX, IDC_ST_MIN,	m_StMinCtrl);
	DDX_Control( pDX, IDC_ST_W,		m_StWCtrl);
	DDX_Control( pDX, IDC_ST_H,		m_StHCtrl);

	DDX_Control( pDX, IDC_ED_MAX_W,	m_MaxWCtrl);
	DDX_Control( pDX, IDC_ED_MAX_H,	m_MaxHCtrl);
	DDX_Control( pDX, IDC_ED_MIN_W,	m_MinWCtrl);
	DDX_Control( pDX, IDC_ED_MIN_H,	m_MinHCtrl);
}

void	i3TDKOptSpecResizeImage::SetType( i3OptResizeTexture::TYPE type)
{
	BOOL bFlag = TRUE;

	switch( type)
	{
		case i3OptResizeTexture::TYPE_SCALE : bFlag = FALSE;	break;
	}

	m_TypeCtrl.SetCurSel( (int) bFlag);

	m_StScaleCtrl.EnableWindow( !bFlag);
	m_ScaleFactorCtrl.EnableWindow( !bFlag);

	m_StLimitCtrl.EnableWindow( bFlag);
	m_StMaxCtrl.EnableWindow( bFlag);
	m_StMinCtrl.EnableWindow( bFlag);
	m_StWCtrl.EnableWindow( bFlag);
	m_StHCtrl.EnableWindow( bFlag);

	m_MaxWCtrl.EnableWindow( bFlag);
	m_MaxHCtrl.EnableWindow( bFlag);
	m_MinWCtrl.EnableWindow( bFlag);
	m_MinHCtrl.EnableWindow( bFlag);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpecResizeImage, i3TDKOptSpecBase)
END_MESSAGE_MAP()


// i3TDKOptSpecResizeImage message handlers

BOOL i3TDKOptSpecResizeImage::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if( m_pOpt != nullptr)
	{
		i3OptResizeTexture * pOpt = (i3OptResizeTexture *) m_pOpt;
		TCHAR conv[256];

		// Type
		SetType( pOpt->getType());

		// Scale
		i3::ftoa( pOpt->getScaleFactor(), conv);
		m_ScaleFactorCtrl.SetWindowText( conv);

		// Max Width
		i3::snprintf( conv, 256, _T("%d"), pOpt->getUpperLimitWidth());
		m_MaxWCtrl.SetWindowText( conv);

		// Max Height
		i3::snprintf( conv, 256, _T("%d"), pOpt->getUpperLimitHeight());
		m_MaxHCtrl.SetWindowText( conv);

		// Min Width
		i3::snprintf( conv, 256, _T("%d"), pOpt->getLowerLimitWidth());
		m_MinWCtrl.SetWindowText( conv);

		// Min Height
		i3::snprintf( conv, 256, _T("%d"), pOpt->getLowerLimitHeight());
		m_MinHCtrl.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecResizeImage::OnOK()
{
	{
		i3OptResizeTexture * pOpt = (i3OptResizeTexture *) m_pOpt;
		TCHAR conv[256];
		INT32 maxW, maxH, minW, minH;

		// Type
		if( m_TypeCtrl.GetCurSel() == 0)
			pOpt->setType( i3OptResizeTexture::TYPE_SCALE);
		else
			pOpt->setType( i3OptResizeTexture::TYPE_LIMIT);

		// Scale
		m_ScaleFactorCtrl.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);
		pOpt->setScaleFactor( (REAL32) i3::atof( conv));

		// Max Width
		m_MaxWCtrl.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);
		maxW = i3::atoi( conv);

		// Max Height
		m_MaxHCtrl.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);
		maxH = i3::atoi( conv);

		// Min Width
		m_MinWCtrl.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);
		minW = i3::atoi( conv);

		// Min Height
		m_MinHCtrl.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);
		minH = i3::atoi( conv);

		pOpt->setLimit( maxW, maxH, minW, minH);
	}

	i3TDKOptSpecBase::OnOK();
}
